#pragma once
/**
 * @file AutomationTypes.hpp
 * @brief Data model for the trigger-condition-action automation engine.
 *
 * A rule is authored in the web UI as JSON and stored as a JSON blob in the
 * "automation" config section. AutomationEngine parses that blob into these
 * structs. Conditions are ANDed; actions run in order on the engine's worker
 * task.
 */

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "esp_event.h"
#include "app_events.hpp"

namespace automation {

constexpr size_t kMaxRules = 16;
constexpr size_t kMaxConditionsPerRule = 8;
constexpr size_t kMaxActionsPerRule = 8;
constexpr size_t kMaxRulesJsonSize = 8192;

struct Trigger {
  std::string type;
  std::map<std::string, std::string> filter;
};

struct Condition {
  std::string type;
  std::map<std::string, std::string> params;
};

struct Action {
  std::string type;
  std::map<std::string, std::string> params;
};

struct Rule {
  std::string id;
  std::string name;
  bool enabled = true;
  Trigger trigger;
  std::vector<Condition> conditions;
  std::vector<Action> actions;
};

struct AutomationConfig {
  std::vector<Rule> rules;
};

struct TriggerSource {
  esp_event_base_t base;
  int32_t id;
};

} // namespace automation
