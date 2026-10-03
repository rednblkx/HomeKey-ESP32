#pragma once
/**
 * @file Conditions.hpp
 * @brief Built-in condition units. All conditions are ANDed by the engine.
 */
#include "automation/AutomationRegistry.hpp"
#include "eventStructs.hpp"
#include "LockManager.hpp"

namespace automation::units {

// --- lock_state ------------------------------------------------------------
// Params: "value": lock state number to compare current state against.
// Reads the engine's cached last-known lock state (kept in sync from
// LOCK_EVENT/LOCK_STATE_CHANGED) so this never touches the lock manager task.
struct LockStateCondition {
  static constexpr const char* kType = "lock_state";
  static constexpr const char* kLabel = "Lock state is";
  static constexpr const char* kStateOptions[] = {
      "0 (unlocked)", "1 (locked)", "2 (jammed)", "3 (unknown)", "4 (unlocking)", "5 (locking)", nullptr};
  static constexpr ParamDesc kParams[] = {
      {"value", ParamDesc::Kind::Select, kStateOptions},
  };
  static bool evaluate(const EngineContext& ctx, const ParamMap& p) {
    auto it = p.find("value");
    if (!ctx.services || it == p.end() || it->second.empty()) return false;
    return ctx.services->lastLockState() == static_cast<uint8_t>(atoi(it->second.c_str()));
  }
};

// --- time_window -----------------------------------------------------------
// Params: "from" / "to" as HH:MM local time. Matches midnight wrap (22:00-06:00).
struct TimeWindowCondition {
  static constexpr const char* kType = "time_window";
  static constexpr const char* kLabel = "Time of day between";
  static constexpr ParamDesc kParams[] = {
      {"from", ParamDesc::Kind::String, nullptr},
      {"to", ParamDesc::Kind::String, nullptr},
  };
  static int minutesOfDay(const std::string& hhmm) {
    if (hhmm.size() < 4) return -1;
    // Accept both "HH:MM" and "HHMM".
    const size_t colon = hhmm.find(':');
    const int h = atoi(hhmm.substr(0, colon).c_str());
    const int m = atoi(hhmm.substr(colon + 1).c_str());
    if (h < 0 || h > 23 || m < 0 || m > 59) return -1;
    return h * 60 + m;
  }
  static bool evaluate(const EngineContext&, const ParamMap& p) {
    const auto from = p.find("from"), to = p.find("to");
    if (from == p.end() || to == p.end()) return false;
    const int a = minutesOfDay(from->second), b = minutesOfDay(to->second);
    if (a < 0 || b < 0) return false;
    time_t now = time(nullptr);
    struct tm tmnow;
    localtime_r(&now, &tmnow);
    const int cur = tmnow.tm_hour * 60 + tmnow.tm_min;
    if (a <= b) return cur >= a && cur < b;
    return cur >= a || cur < b; // window wraps midnight
  }
};

// --- mqtt_connected --------------------------------------------------------
// Params: "value": "true"/"false".
struct MqttConnectedCondition {
  static constexpr const char* kType = "mqtt_connected";
  static constexpr const char* kLabel = "MQTT connection state is";
  static constexpr const char* kBoolOptions[] = {"true", "false", nullptr};
  static constexpr ParamDesc kParams[] = {
      {"value", ParamDesc::Kind::Select, kBoolOptions},
  };
  static bool evaluate(const EngineContext& ctx, const ParamMap& p) {
    auto it = p.find("value");
    const bool want = !(it != p.end() && it->second == "false");
    return ctx.services && ctx.services->mqttConnected() == want;
  }
};

} // namespace automation::units

namespace automation {
using units::LockStateCondition;
using units::TimeWindowCondition;
using units::MqttConnectedCondition;

inline constexpr ConditionUnit kConditions[] = {
    makeConditionUnit<LockStateCondition>(),
    makeConditionUnit<TimeWindowCondition>(),
    makeConditionUnit<MqttConnectedCondition>(),
};
} // namespace automation
