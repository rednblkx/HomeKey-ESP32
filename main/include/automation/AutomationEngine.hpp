#pragma once
/**
 * @file AutomationEngine.hpp
 * @brief Runs user-defined trigger-condition-action rules.
 *
 * The engine subscribes (via AppEventLoop) to every event source referenced
 * by the registry's trigger units, matches rules on the event-loop thread
 * (cheap filter checks only) and executes matched rules on its own worker
 * task so actions can block (delay, gpio pulse) without stalling events.
 *
 * Rules come from the "automation" config section as a JSON blob; the engine
 * re-parses when it sees HW_EVENT/HW_CONFIG_CHANGED (published after a save).
 */
#include <atomic>
#include <memory>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "app_event_loop.hpp"
#include "automation/AutomationTypes.hpp"
#include "automation/AutomationRegistry.hpp"
#include "automation/GpioLeaseCache.hpp"

class MqttManager;
class LockManager;
class ConfigManager;

namespace automation {

class AutomationEngine : public automation::EngineServices {
public:
  AutomationEngine(ConfigManager& configManager, MqttManager* mqttManager, LockManager* lockManager);
  ~AutomationEngine();

  AutomationEngine(const AutomationEngine&) = delete;
  AutomationEngine& operator=(const AutomationEngine&) = delete;

  void begin();

  uint8_t lastLockState() const { return m_lastLockState.load(std::memory_order_relaxed); }
  bool mqttConnected() const;
  void requestLockTarget(uint8_t state);   // 0 = unlock, 1 = lock
  void mqttPublish(const std::string& topic, const std::string& payload);
  void logActionError(const char* fmt, ...) __attribute__((format(printf, 2, 3)));

private:
  struct ExecRequest {
    uint32_t ruleIndex;
  };

  static void workerTaskEntry(void* instance);
  void workerTask();
  void onEvent(const TriggerUnit& unit, const uint8_t* data, size_t size);
  bool parseRules(const std::string& json);
  void reloadFromConfig();
  void executeRule(const Rule& rule);
  bool conditionsPass(const Rule& rule);
  void subscribeAll();

  std::vector<Rule> m_rules;
  mutable SemaphoreHandle_t m_rulesMutex = nullptr;

  ConfigManager& m_configManager;
  MqttManager* m_mqttManager;
  LockManager* m_lockManager;
  EngineContext m_ctx{this};
  GpioLeaseCache m_gpioCache;

  std::atomic<uint8_t> m_lastLockState{255};
  std::atomic<bool> m_running{false};

  QueueHandle_t m_execQueue = nullptr;
  TaskHandle_t m_taskHandle = nullptr;
  std::vector<AppEventLoop::SubscriptionHandle> m_subscriptions;

  static const char* TAG;
};

} // namespace automation
