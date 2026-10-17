#pragma once
/**
 * @file AutomationRegistry.hpp
 * @brief Registration tables for trigger / condition / action units.
 *
 * Adding a new kind:
 *  1. write a small stateless struct following the unit interfaces (see
 *     automation/units/Triggers.hpp, Conditions.hpp, Actions.hpp),
 *  2. add one line to the kTriggers / kConditions / kActions arrays.
 *
 * Each registry entry is a type-erased vtable generated from the unit struct
 * at compile time, exposing the wire name, human label, param schema (served
 * to the web UI via /automation/schema) and the unit's behavior.
 */

#include <cstdint>
#include <cstring>
#include <string>
#include <map>
#include "app_events.hpp"
#include "eventStructs.hpp"
#include "AutomationTypes.hpp"

namespace automation {

using ParamMap = std::map<std::string, std::string>;

// Event payload snapshot handed to trigger matching. The raw alpaca bytes are
// kept so trigger units can decode the payload types they care about via
// decodeEventPayload().
struct EventContext {
  esp_event_base_t base;
  int32_t id;
  const uint8_t* data;
  size_t size;
};

// Payload decode helper for trigger units; defined in AutomationEngine.cpp.
// eventStructs.hpp is included above so the payload types are complete.
bool decodeEventPayload(const EventContext& ctx, ::NfcEvent& out);

// Services the condition/action units may use. The engine implements this;
// a null pointer means the service is unavailable and the unit should no-op.
class EngineServices {
public:
  virtual ~EngineServices() = default;
  virtual uint8_t lastLockState() const = 0;
  virtual bool mqttConnected() const = 0;
  virtual void requestLockTarget(uint8_t state) = 0; // 0 = unlock, 1 = lock
  virtual void mqttPublish(const std::string& topic, const std::string& payload) = 0;
  virtual void logActionError(const char* fmt, ...) __attribute__((format(printf, 2, 3))) = 0;
};

// Owns the engine's long-lived GPIO leases (see GpioLeaseCache.hpp).
class GpioLeaseCache;

// Services the condition/action units may use. Filled in by AutomationEngine;
// members may be null when a dependency isn't present.
struct EngineContext {
  EngineServices* services = nullptr;
  // Owns the engine's long-lived GPIO leases; null disables GPIO actions.
  GpioLeaseCache* gpio = nullptr;
};

struct ParamDesc {
  const char* name;
  enum class Kind : uint8_t { String, Number, Bool, Select } kind;
  // Select options as a nullptr-terminated list; null for other kinds.
  const char* const* options = nullptr;
  // Visibility dependency: param is only relevant when the param named
  // `showIfParam` currently equals `showIfValue` (null = always visible).
  // The schema endpoint exposes this so the UI can hide irrelevant fields.
  const char* showIfParam = nullptr;
  const char* showIfValue = nullptr;
};

// --- Trigger units ---------------------------------------------------------
struct TriggerUnit {
  const char* type;
  const char* label;
  // esp_event bases are `extern const` variables, not constexpr, so they can't
  // appear in constant expressions; the unit stores a pointer to its base and
  // the engine dereferences at subscribe time.
  const esp_event_base_t* base;
  int32_t id;
  bool (*matches)(const EventContext& ctx, const ParamMap& filter);
  const ParamDesc* params;
  size_t paramCount;
};

template <typename Unit>
constexpr TriggerUnit makeTriggerUnit() {
  return TriggerUnit{
      Unit::kType, Unit::kLabel, &Unit::kBase, Unit::kId, &Unit::matches,
      Unit::kParams, sizeof(Unit::kParams) / sizeof(Unit::kParams[0])};
}

// --- Condition units -------------------------------------------------------
struct ConditionUnit {
  const char* type;
  const char* label;
  bool (*evaluate)(const EngineContext& ctx, const ParamMap& params);
  const ParamDesc* params;
  size_t paramCount;
};

template <typename Unit>
constexpr ConditionUnit makeConditionUnit() {
  return ConditionUnit{
      Unit::kType, Unit::kLabel, &Unit::evaluate,
      Unit::kParams, sizeof(Unit::kParams) / sizeof(Unit::kParams[0])};
}

// --- Action units ----------------------------------------------------------
struct ActionUnit {
  const char* type;
  const char* label;
  void (*execute)(const EngineContext& ctx, const ParamMap& params);
  const ParamDesc* params;
  size_t paramCount;
};

template <typename Unit>
constexpr ActionUnit makeActionUnit() {
  return ActionUnit{
      Unit::kType, Unit::kLabel, &Unit::execute,
      Unit::kParams, sizeof(Unit::kParams) / sizeof(Unit::kParams[0])};
}

template <typename UnitT, size_t N>
const UnitT* findUnit(const UnitT (&table)[N], const std::string& type) {
  for (const auto& u : table) {
    if (type == u.type) return &u;
  }
  return nullptr;
}

} // namespace automation

// The per-kind unit headers define the concrete units; they are included by
// the registry only, so the engine translation unit doesn't pull their deps.
#include "automation/units/Triggers.hpp"
#include "automation/units/Conditions.hpp"
#include "automation/units/Actions.hpp"
