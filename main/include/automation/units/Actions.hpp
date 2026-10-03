#pragma once
/**
 * @file Actions.hpp
 * @brief Built-in action units, executed in order on the engine worker task.
 *
 * GPIO actions run through the engine's GpioLeaseCache: a gpio_set keeps its
 * lease (and thus its driven level) for the engine's lifetime, because
 * GPIOAllocator resets a pin to input when the last lease on it dies. Pins
 * already claimed by another consumer fail to actuate (logged, not fatal).
 */
#include "automation/AutomationRegistry.hpp"
#include "automation/GpioLeaseCache.hpp"
#include "eventStructs.hpp"
#include "LockManager.hpp"
#include "MqttManager.hpp"
#include "HardwareManager.hpp"
#include "GPIOAllocator.hpp"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace automation::units {

// Helpers -------------------------------------------------------------------
inline std::optional<uint8_t> u8Param(const ParamMap& p, const char* key) {
  auto it = p.find(key);
  if (it == p.end() || it->second.empty()) return std::nullopt;
  const long v = strtol(it->second.c_str(), nullptr, 10);
  if (v < 0 || v > 255) return std::nullopt;
  return static_cast<uint8_t>(v);
}
inline std::optional<uint32_t> u32Param(const ParamMap& p, const char* key) {
  auto it = p.find(key);
  if (it == p.end() || it->second.empty()) return std::nullopt;
  return static_cast<uint32_t>(strtoul(it->second.c_str(), nullptr, 10));
}

// --- gpio_pulse ------------------------------------------------------------
// Params: "pin", "level" (0/1), "ms" pulse duration. Drives the level, waits,
// then restores the level the pin had before the pulse.
struct GpioPulseAction {
  static constexpr const char* kType = "gpio_pulse";
  static constexpr const char* kLabel = "Pulse GPIO";
  static constexpr ParamDesc kParams[] = {
      {"pin", ParamDesc::Kind::Number, nullptr},
      {"level", ParamDesc::Kind::Number, nullptr},
      {"ms", ParamDesc::Kind::Number, nullptr},
  };
  static void execute(const EngineContext& ctx, const ParamMap& p) {
    const auto pin = u8Param(p, "pin"), level = u8Param(p, "level");
    const auto ms = u32Param(p, "ms");
    if (!pin || !level || !ms || *ms == 0 || !ctx.gpio) return;
    auto* lease = ctx.gpio->leaseFor(*pin, "automation");
    if (!lease || !lease->valid()) {
      if (ctx.services) ctx.services->logActionError("gpio_pulse: cannot acquire pin %d", *pin);
      return;
    }
    const bool prev = lease->get_level();
    lease->set_level(*level != 0);
    vTaskDelay(pdMS_TO_TICKS(*ms));
    lease->set_level(prev);
  }
};

// --- gpio_set --------------------------------------------------------------
// Params: "pin", "level". Latches the level until another gpio_set/pulse on
// the same pin changes it (the lease is held by the engine's cache).
struct GpioSetAction {
  static constexpr const char* kType = "gpio_set";
  static constexpr const char* kLabel = "Set GPIO level";
  static constexpr ParamDesc kParams[] = {
      {"pin", ParamDesc::Kind::Number, nullptr},
      {"level", ParamDesc::Kind::Number, nullptr},
  };
  static void execute(const EngineContext& ctx, const ParamMap& p) {
    const auto pin = u8Param(p, "pin"), level = u8Param(p, "level");
    if (!pin || !level || !ctx.gpio) return;
    auto* lease = ctx.gpio->leaseFor(*pin, "automation");
    if (!lease || !lease->valid()) {
      if (ctx.services) ctx.services->logActionError("gpio_set: cannot acquire pin %d", *pin);
      return;
    }
    lease->set_level(*level != 0);
  }
};

// --- lock_target -----------------------------------------------------------
// Params: "state": 0 = unlock, 1 = lock.
struct LockTargetAction {
  static constexpr const char* kType = "lock_target";
  static constexpr const char* kLabel = "Set lock target state";
  static constexpr const char* kStateOptions[] = {"0 (unlock)", "1 (lock)", nullptr};
  static constexpr ParamDesc kParams[] = {
      {"state", ParamDesc::Kind::Select, kStateOptions},
  };
  static void execute(const EngineContext& ctx, const ParamMap& p) {
    const auto state = u8Param(p, "state");
    if (!state || *state > 1 || !ctx.services) return;
    ctx.services->requestLockTarget(*state);
  }
};

// --- mqtt_publish ----------------------------------------------------------
// Params: "topic", "payload".
struct MqttPublishAction {
  static constexpr const char* kType = "mqtt_publish";
  static constexpr const char* kLabel = "Publish MQTT";
  static constexpr ParamDesc kParams[] = {
      {"topic", ParamDesc::Kind::String, nullptr},
      {"payload", ParamDesc::Kind::String, nullptr},
  };
  static void execute(const EngineContext& ctx, const ParamMap& p) {
    const auto topic = p.find("topic"), payload = p.find("payload");
    if (topic == p.end() || topic->second.empty() || !ctx.services) return;
    ctx.services->mqttPublish(topic->second,
                            payload == p.end() ? std::string{} : payload->second);
  }
};

// --- delay -----------------------------------------------------------------
// Params: "ms".
struct DelayAction {
  static constexpr const char* kType = "delay";
  static constexpr const char* kLabel = "Wait";
  static constexpr ParamDesc kParams[] = {
      {"ms", ParamDesc::Kind::Number, nullptr},
  };
  static void execute(const EngineContext&, const ParamMap& p) {
    const auto ms = u32Param(p, "ms");
    if (!ms || *ms == 0 || *ms > 60000) return;
    vTaskDelay(pdMS_TO_TICKS(*ms));
  }
};

} // namespace automation::units

namespace automation {
using units::GpioPulseAction;
using units::GpioSetAction;
using units::LockTargetAction;
using units::MqttPublishAction;
using units::DelayAction;

inline constexpr ActionUnit kActions[] = {
    makeActionUnit<GpioPulseAction>(),
    makeActionUnit<GpioSetAction>(),
    makeActionUnit<LockTargetAction>(),
    makeActionUnit<MqttPublishAction>(),
    makeActionUnit<DelayAction>(),
};
} // namespace automation
