#include "esp_log.h"
#include "esp_timer.h"
#include "config.hpp"
#include "LockManager.hpp"
#include "HardwareManager.hpp"
#include "eventStructs.hpp"

const char* LockManager::TAG = "LockManager";

static EventBus::Bus& event_bus = EventBus::Bus::instance();

/**
 * @brief Initialize a LockManager with configuration and wire its event handlers.
 *
 * Sets initial current and target states to LOCKED, registers EventBus topics and
 * subscriptions for lock state updates and NFC events, and creates the momentary-state
 * timer used to revert temporary unlocks.
 *
 * @param miscConfig Configuration controlling NFC behavior, momentary timeout, and high-level lock flags
 *                   (for example: lockAlwaysUnlock, lockAlwaysLock, and HK-related GPIO settings).
 * @param actionsConfig Configuration defining which sources can trigger momentary unlocks and related action masks/timeouts.
 */
LockManager::LockManager(const espConfig::misc_config_t& miscConfig, const espConfig::actions_config_t& actionsConfig)
    : m_miscConfig(miscConfig),
      m_actionsConfig(actionsConfig),
      m_currentState(lockStates::LOCKED),
      m_targetState(lockStates::LOCKED),
      bus_topic(event_bus.register_topic(LOCK_BUS_TOPIC))
{
  m_override_state_event = event_bus.subscribe(event_bus.register_topic(LOCK_O_STATE_CHANGED), [&](const EventBus::Event& event, void* context){
      if(event.payload_size == 0 || event.payload == nullptr) return;
      std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) { ESP_LOGE(TAG, "Failed to deserialize override state event: %s", ec.message().c_str()); return; }
      ESP_LOGD(TAG, "Received override state event: %d -> %d", s.currentState, s.targetState);
      overrideState(s.currentState, s.targetState);
    });
  m_target_state_event = event_bus.subscribe(event_bus.register_topic(LOCK_T_STATE_CHANGED), [&](const EventBus::Event& event, void* context){
      if(event.payload_size == 0 || event.payload == nullptr) return;
      std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) { ESP_LOGE(TAG, "Failed to deserialize target state event: %s", ec.message().c_str()); return; }
      ESP_LOGD(TAG, "Received target state event: %d -> %d", s.currentState, s.targetState);
      setTargetState(s.targetState, Source(s.source));
    });
  m_update_state_event = event_bus.subscribe(event_bus.register_topic(LOCK_UPDATE_BUS_TOPIC), [&](const EventBus::Event& event, void* context){
      if(event.payload_size == 0 || event.payload == nullptr) return;
      std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) { ESP_LOGE(TAG, "Failed to deserialize update state event: %s", ec.message().c_str()); return; }
      ESP_LOGD(TAG, "Received update state event: %d -> %d", s.currentState, s.targetState);
      m_currentState = s.currentState;
      s.targetState = m_targetState;
      s.source = LockManager::INTERNAL;
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      event_bus.publish({bus_topic, 0, d.data(), d.size()});
    });
  esp_timer_create_args_t momentaryStateTimer_arg = {
    .callback = handleTimer,
    .arg = this,
    .name = "momentaryStateTimer"
  };
  esp_timer_create(&momentaryStateTimer_arg, &momentaryStateTimer);
}

/**
 * @brief Subscribes to NFC events and publishes the initial hardware action state.
 *
 * Begins NFC handling by subscribing to the NFC event topic so HomeKey tap events
 * can set or toggle the lock's target state according to configuration. Ignores
 * empty or invalid NFC payloads. After subscribing, publishes an EventLockState
 * describing the current and target states with source INTERNAL to the hardware
 * action topic.
 */
void LockManager::begin() {
  m_nfc_event = event_bus.subscribe(event_bus.get_topic(NFC_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), [&](const EventBus::Event& event, void* context){
      if(event.payload_size == 0 || event.payload == nullptr) return;
      std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
      std::error_code ec;
      NfcEvent nfc_event = alpaca::deserialize<NfcEvent>(payload, ec);
      if(ec) { ESP_LOGE(TAG, "Failed to deserialize NFC event: %s", ec.message().c_str()); return; }
      ESP_LOGD(TAG, "Received NFC event: %d", nfc_event.type);
      if(nfc_event.type == HOMEKEY_TAP) {
        ESP_LOGI(TAG, "Processing NFC tap request...");
        EventHKTap s = alpaca::deserialize<EventHKTap>(nfc_event.data, ec);
        if (!ec) {
          if (s.status) {
            if (m_miscConfig.lockAlwaysUnlock) {
              setTargetState(lockStates::UNLOCKED, Source::NFC);
            } else if (m_miscConfig.lockAlwaysLock) {
              setTargetState(lockStates::LOCKED, Source::NFC);
            } else {
              int newState = (m_currentState == lockStates::LOCKED) ? lockStates::UNLOCKED : lockStates::LOCKED;
              setTargetState(newState, Source::NFC);
            }
          }
        } else {
          ESP_LOGE(TAG, "Failed to deserialize HomeKey event: %s", ec.message().c_str());
          return;
        }
      }
    });
  EventLockState s{
    .currentState = static_cast<uint8_t>(m_currentState),
    .targetState = static_cast<uint8_t>(m_targetState),
    .source = LockManager::INTERNAL
  };
  std::vector<uint8_t> d;
  alpaca::serialize(s, d);
  event_bus.publish({event_bus.get_topic(HARDWARE_ACTION_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
}

/**
 * @brief Timer callback that sets the lock's target state to LOCKED with source INTERNAL.
 *
 * @param instance Pointer to the LockManager instance provided to the timer callback.
 */
void LockManager::handleTimer(void* instance){
  static_cast<LockManager*>(instance)->setTargetState(LOCKED, INTERNAL);
}

/**
 * @brief Gets the current lock state.
 *
 * @return int Current lock state as an integer code (e.g., LOCKED or UNLOCKED).
 */

int LockManager::getCurrentState() const {
    return m_currentState;
}

/**
 * @brief Retrieves the manager's configured target lock state.
 *
 * @return int The target lock state (e.g., LOCKED or UNLOCKED).
 */
int LockManager::getTargetState() const {
    return m_targetState;
}

/**
 * @brief Change the lock's target state, propagate the change, and trigger any momentary unlock timer.
 *
 * Updates the stored target state, stops any running momentary timer, and publishes the resulting
 * current/target state to the lock state topic. If hardware actions should be performed for the
 * given source (and dumb-switch mode is not enabled), publishes a hardware action event as well.
 * When a target unlock is requested by a configured momentary source, starts the momentary timer.
 *
 * @param state Desired target lock state (use values from lockStates).
 * @param source Origin of the request which influences publishing behavior and momentary handling.
 */

void LockManager::setTargetState(uint8_t state, Source source) {
    if (state == m_targetState && m_currentState == m_targetState) {
        ESP_LOGD(TAG, "Requested state is already the current state. No action taken.");
        return;
    }

    ESP_LOGI(TAG, "Setting target state to %d (c:%d,t:%d), from source %d", state, m_currentState, m_targetState, static_cast<int>(source));

    if(esp_timer_is_active(momentaryStateTimer)) esp_timer_stop(momentaryStateTimer);

    m_targetState = state;

    EventLockState s{
      .currentState = m_currentState,
      .targetState = m_targetState,
      .source = LockManager::INTERNAL
    };
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    if (m_actionsConfig.hkDumbSwitchMode) {
      ESP_LOGI(TAG, "Dummy Action is enabled!");
      m_currentState = m_targetState;
      s.currentState = m_targetState;
      d.clear();
      alpaca::serialize(s, d);
    } else if((source == NFC && m_actionsConfig.hkGpioControlledState) || source != NFC) {
      event_bus.publish({event_bus.get_topic(HARDWARE_ACTION_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
    }
    event_bus.publish({bus_topic, 0, d.data(), d.size()});

    uint8_t momentarySources = (((m_actionsConfig.gpioActionMomentaryEnabled |
                                  m_actionsConfig.gpioActionPin) == 255) &
                                !m_actionsConfig.hkDumbSwitchMode)
                                   ? 0
                                   : m_actionsConfig.gpioActionMomentaryEnabled;
    bool isMomentarySource = ((static_cast<uint8_t>(source) & momentarySources) != 0);

    if (m_targetState == lockStates::UNLOCKED && isMomentarySource) {
        ESP_LOGI(TAG, "Starting momentary unlock timer for %d ms.", m_actionsConfig.gpioActionMomentaryTimeout);
        esp_timer_start_once(momentaryStateTimer, m_actionsConfig.gpioActionMomentaryTimeout * 1000);
    }
}

/**
 * @brief Apply an external override to the lock's current and/or target state and notify observers.
 *
 * If a provided state equals 255, that state is left unchanged. Stops any active momentary-state timer,
 * updates the internal current and target states, and publishes the resulting state to "lock/action"
 * and "lock/stateChanged".
 *
 * @param c_state External current state to apply, or 255 to leave the current state unchanged.
 * @param t_state External target state to apply, or 255 to leave the target state unchanged.
 */
void LockManager::overrideState(uint8_t c_state, uint8_t t_state) {
    if (c_state == m_currentState && t_state == m_targetState) return;

    ESP_LOGI(TAG, "External source reported new c_state: %d t_state: %d. Overriding internal state.", c_state, t_state);

    m_currentState = c_state != 255 ? c_state : m_currentState;
    m_targetState = t_state != 255 ? t_state : m_targetState;

    if(momentaryStateTimer && esp_timer_is_active(momentaryStateTimer)) esp_timer_stop(momentaryStateTimer);
    EventLockState s{
      .currentState = static_cast<uint8_t>(m_currentState),
      .targetState = static_cast<uint8_t>(m_targetState),
      .source = LockManager::INTERNAL
    };
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    event_bus.publish({event_bus.get_topic(HARDWARE_ACTION_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
    event_bus.publish({bus_topic, 0, d.data(), d.size()});
}
