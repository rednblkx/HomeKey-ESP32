#include "automation/AutomationEngine.hpp"
#include "automation/AutomationTypes.hpp"
#include "automation/AutomationRegistry.hpp"
#include "ConfigManager.hpp"
#include "LockManager.hpp"
#include "MqttManager.hpp"
#include "eventStructs.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include <cJSON.h>
#include <map>
#include <set>
#include <cstdarg>

namespace automation {

const char* AutomationEngine::TAG = "Automation";

// --- JSON parsing ----------------------------------------------------------

static std::map<std::string, std::string> paramsFromJson(cJSON* obj) {
  std::map<std::string, std::string> out;
  if (!obj || !cJSON_IsObject(obj)) return out;
  for (cJSON* it = obj->child; it; it = it->next) {
    if (!it->string) continue;
    if (cJSON_IsString(it)) {
      out[it->string] = it->valuestring;
    } else if (cJSON_IsNumber(it)) {
      out[it->string] = std::to_string(it->valuedouble);
    } else if (cJSON_IsBool(it)) {
      out[it->string] = cJSON_IsTrue(it) ? "true" : "false";
    }
  }
  return out;
}

bool AutomationEngine::parseRules(const std::string& json) {
  std::vector<Rule> parsed;
  if (!json.empty()) {
    cJSON* root = cJSON_ParseWithLength(json.c_str(), json.size());
    if (!root) {
      ESP_LOGE(TAG, "Rule JSON parse failed");
      return false;
    }
    cJSON* rules = cJSON_GetObjectItem(root, "rules");
    if (!cJSON_IsArray(rules)) {
      // Config saves echo back the wrapped section shape
      // ({"rulesJson": "<json string>"}); unwrap it if present.
      cJSON* wrapper = cJSON_GetObjectItem(root, "rulesJson");
      if (cJSON_IsString(wrapper)) {
        cJSON* unwrapped = cJSON_ParseWithLength(wrapper->valuestring,
                                                 strlen(wrapper->valuestring));
        cJSON_Delete(root);
        root = unwrapped;
        rules = root ? cJSON_GetObjectItem(root, "rules") : nullptr;
        if (!root) {
          ESP_LOGE(TAG, "Rule JSON unwrap failed");
          return false;
        }
      }
    }
    if (cJSON_IsArray(rules)) {
      cJSON* it = nullptr;
      cJSON_ArrayForEach(it, rules) {
        if (parsed.size() >= kMaxRules) {
          ESP_LOGW(TAG, "Rule limit (%d) reached, ignoring the rest", (int)kMaxRules);
          break;
        }
        Rule r;
        cJSON* id = cJSON_GetObjectItem(it, "id");
        cJSON* name = cJSON_GetObjectItem(it, "name");
        cJSON* enabled = cJSON_GetObjectItem(it, "enabled");
        if (id && cJSON_IsString(id)) r.id = id->valuestring;
        if (name && cJSON_IsString(name)) r.name = name->valuestring;
        r.enabled = enabled ? cJSON_IsTrue(enabled) : true;
        cJSON* trigger = cJSON_GetObjectItem(it, "trigger");
        if (trigger) {
          cJSON* type = cJSON_GetObjectItem(trigger, "type");
          if (type && cJSON_IsString(type)) r.trigger.type = type->valuestring;
          r.trigger.filter = paramsFromJson(cJSON_GetObjectItem(trigger, "filter"));
        }
        cJSON* conds = cJSON_GetObjectItem(it, "conditions");
        if (cJSON_IsArray(conds)) {
          cJSON* c = nullptr;
          cJSON_ArrayForEach(c, conds) {
            if (r.conditions.size() >= kMaxConditionsPerRule) break;
            Condition cond;
            cJSON* t = cJSON_GetObjectItem(c, "type");
            if (t && cJSON_IsString(t)) cond.type = t->valuestring;
            cond.params = paramsFromJson(cJSON_GetObjectItem(c, "params"));
            r.conditions.push_back(std::move(cond));
          }
        }
        cJSON* acts = cJSON_GetObjectItem(it, "actions");
        if (cJSON_IsArray(acts)) {
          cJSON* a = nullptr;
          cJSON_ArrayForEach(a, acts) {
            if (r.actions.size() >= kMaxActionsPerRule) break;
            Action act;
            cJSON* t = cJSON_GetObjectItem(a, "type");
            if (t && cJSON_IsString(t)) act.type = t->valuestring;
            act.params = paramsFromJson(cJSON_GetObjectItem(a, "params"));
            r.actions.push_back(std::move(act));
          }
        }
        if (!r.trigger.type.empty() && !r.actions.empty()) {
          parsed.push_back(std::move(r));
        } else {
          ESP_LOGW(TAG, "Skipping rule without trigger or actions (id=%s)", r.id.c_str());
        }
      }
    }
    cJSON_Delete(root);
  }

  if (m_rulesMutex) xSemaphoreTake(m_rulesMutex, portMAX_DELAY);
  m_rules = std::move(parsed);
  if (m_rulesMutex) xSemaphoreGive(m_rulesMutex);
  ESP_LOGI(TAG, "Loaded %u automation rule(s)", (unsigned)m_rules.size());
  return true;
}

// --- Event payload decode helper (used by trigger units) --------------------

bool decodeEventPayload(const EventContext& ctx, NfcEvent& out) {
  if (ctx.size == 0 || ctx.data == nullptr) return false;
  std::span<const uint8_t> payload(ctx.data, ctx.size);
  std::error_code ec;
  out = alpaca::deserialize<NfcEvent>(payload, ec);
  return !ec;
}

// --- Lifecycle --------------------------------------------------------------

AutomationEngine::AutomationEngine(ConfigManager& configManager, MqttManager* mqttManager,
                                   LockManager* lockManager)
    : m_configManager(configManager), m_mqttManager(mqttManager), m_lockManager(lockManager) {
  m_ctx.services = this;
  m_ctx.gpio = &m_gpioCache;
  m_rulesMutex = xSemaphoreCreateMutex();
  parseRules(m_configManager.getConfig<espConfig::automation_config_t>().rulesJson);
}

AutomationEngine::~AutomationEngine() {
  m_running = false;
  m_subscriptions.clear(); // RAII unsubscribe
  if (m_taskHandle) {
    // Worker may be mid-action (delay); give it a bounded window to exit.
    for (int i = 0; i < 60 && eTaskGetState(m_taskHandle) != eSuspended &&
                    eTaskGetState(m_taskHandle) != eBlocked;
         ++i) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(m_taskHandle);
    m_taskHandle = nullptr;
  }
  if (m_execQueue) {
    vQueueDelete(m_execQueue);
    m_execQueue = nullptr;
  }
  if (m_rulesMutex) {
    vSemaphoreDelete(m_rulesMutex);
    m_rulesMutex = nullptr;
  }
}

void AutomationEngine::begin() {
  if (m_running.exchange(true)) return;
  subscribeAll();
  m_execQueue = xQueueCreate(8, sizeof(ExecRequest));
#if CONFIG_FREERTOS_UNICORE
  const BaseType_t core = tskNO_AFFINITY;
#else
  const BaseType_t core = 1;
#endif
  xTaskCreatePinnedToCore(workerTaskEntry, "automation", 4096, this, 4, &m_taskHandle, core);
}

void AutomationEngine::reloadFromConfig() {
  parseRules(m_configManager.getConfig<espConfig::automation_config_t>().rulesJson);
  std::set<uint8_t> pins;
  if (m_rulesMutex) xSemaphoreTake(m_rulesMutex, portMAX_DELAY);
  for (const auto& r : m_rules) {
    for (const auto& a : r.actions) {
      if (a.type != "gpio_set" && a.type != "gpio_pulse") continue;
      auto it = a.params.find("pin");
      if (it != a.params.end()) {
        const long v = strtol(it->second.c_str(), nullptr, 10);
        if (v >= 0 && v <= 255) pins.insert(static_cast<uint8_t>(v));
      }
    }
  }
  if (m_rulesMutex) xSemaphoreGive(m_rulesMutex);
  m_gpioCache.retainOnly(pins);
}

void AutomationEngine::subscribeAll() {
  struct SrcKey {
    esp_event_base_t base;
    int32_t id;
    bool operator==(const SrcKey& o) const { return base == o.base && id == o.id; }
  };
  std::vector<SrcKey> sources;
  for (const auto& t : kTriggers) {
    SrcKey key{*t.base, t.id};
    bool found = false;
    for (const auto& s : sources) found = found || s == key;
    if (!found) sources.push_back(key);
  }
  for (const auto& src : sources) {
    m_subscriptions.push_back(AppEventLoop::subscribe(
        src.base, src.id, [this, base = src.base, id = src.id](const uint8_t* data, size_t size) {
          for (const auto& unit : kTriggers) {
            if (*unit.base == base && unit.id == id) {
              onEvent(unit, data, size);
            }
          }
        }));
  }
  m_subscriptions.push_back(AppEventLoop::subscribe(
      LOCK_EVENT, LOCK_STATE_CHANGED, [this](const uint8_t* data, size_t size) {
        if (size == 0 || !data) return;
        std::span<const uint8_t> payload(data, size);
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
        if (!ec) m_lastLockState.store(s.currentState, std::memory_order_relaxed);
      }));
  m_subscriptions.push_back(AppEventLoop::subscribe(
      HW_EVENT, HW_CONFIG_CHANGED, [this](const uint8_t* data, size_t size) {
        if (size == 0 || !data) return;
        std::span<const uint8_t> payload(data, size);
        std::error_code ec;
        EventValueChanged ev = alpaca::deserialize<EventValueChanged>(payload, ec);
        if (!ec && ev.name == "rules") reloadFromConfig();
      }));
}

// --- Matching ---------------------------------------------------------------

void AutomationEngine::onEvent(const TriggerUnit& unit, const uint8_t* data, size_t size) {
  EventContext ctx{*unit.base, unit.id, data, size};
  if (m_rulesMutex) xSemaphoreTake(m_rulesMutex, portMAX_DELAY);
  std::vector<size_t> matched;
  for (size_t i = 0; i < m_rules.size(); ++i) {
    const Rule& r = m_rules[i];
    if (!r.enabled || r.trigger.type != unit.type) continue;
    if (unit.matches(ctx, r.trigger.filter)) matched.push_back(i);
  }
  if (m_rulesMutex) xSemaphoreGive(m_rulesMutex);
  for (size_t idx : matched) {
    ExecRequest req{static_cast<uint32_t>(idx)};
    if (xQueueSend(m_execQueue, &req, 0) != pdTRUE) {
      ESP_LOGW(TAG, "Exec queue full, dropping rule index %u", (unsigned)idx);
    }
  }
}

// --- Execution --------------------------------------------------------------

bool AutomationEngine::conditionsPass(const Rule& rule) {
  for (const auto& cond : rule.conditions) {
    const ConditionUnit* unit = findUnit(kConditions, cond.type);
    if (!unit) {
      ESP_LOGW(TAG, "Unknown condition type '%s' in rule %s", cond.type.c_str(), rule.id.c_str());
      return false; // fail closed
    }
    if (!unit->evaluate(m_ctx, cond.params)) return false;
  }
  return true;
}

void AutomationEngine::executeRule(const Rule& rule) {
  if (!conditionsPass(rule)) return;
  ESP_LOGI(TAG, "Rule '%s' triggered", rule.name.c_str());
  for (const auto& act : rule.actions) {
    const ActionUnit* unit = findUnit(kActions, act.type);
    if (!unit) {
      ESP_LOGW(TAG, "Unknown action type '%s' in rule %s", act.type.c_str(), rule.id.c_str());
      continue;
    }
    unit->execute(m_ctx, act.params);
  }
}

void AutomationEngine::workerTaskEntry(void* instance) {
  static_cast<AutomationEngine*>(instance)->workerTask();
}

void AutomationEngine::workerTask() {
  ExecRequest req;
  while (m_running) {
    if (xQueueReceive(m_execQueue, &req, portMAX_DELAY) != pdTRUE) continue;
    if (m_rulesMutex) xSemaphoreTake(m_rulesMutex, portMAX_DELAY);
    const bool valid = req.ruleIndex < m_rules.size();
    Rule copy;
    if (valid) copy = m_rules[req.ruleIndex];
    if (m_rulesMutex) xSemaphoreGive(m_rulesMutex);
    if (valid && copy.enabled) executeRule(copy);
  }
}

// --- Services for units -----------------------------------------------------

bool AutomationEngine::mqttConnected() const {
  return m_mqttManager && m_mqttManager->isConnected();
}

void AutomationEngine::mqttPublish(const std::string& topic, const std::string& payload) {
  if (!m_mqttManager) {
    ESP_LOGW(TAG, "mqtt_publish: MQTT manager unavailable");
    return;
  }
  m_mqttManager->publish(topic, payload);
}

void AutomationEngine::requestLockTarget(uint8_t state) {
  if (!m_lockManager) {
    ESP_LOGW(TAG, "lock_target: lock manager unavailable");
    return;
  }
  EventLockState s{
      .currentState = m_lastLockState.load(std::memory_order_relaxed),
      .targetState = state,
      .source = LockManager::INTERNAL};
  std::vector<uint8_t> d;
  alpaca::serialize(s, d);
  AppEventLoop::publish(LOCK_EVENT, LOCK_TARGET_STATE_CHANGED, d.data(), d.size());
}

void AutomationEngine::logActionError(const char* fmt, ...) {
  char buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  ESP_LOGE(TAG, "%s", buf);
}

} // namespace automation
