#include "esp_log.h"
#include "esp_timer.h"
#include "config.hpp"
#include "LockManager.hpp"
#include "HardwareManager.hpp"
#include "eventStructs.hpp"

const char* LockManager::TAG = "LockManager";

static uint8_t sourceToMomentaryMask(LockManager::Source source) {
    switch (source) {
        case LockManager::HOMEKIT:
            return static_cast<uint8_t>(gpioMomentaryStateStatus::M_HOME);
        case LockManager::NFC:
            return static_cast<uint8_t>(gpioMomentaryStateStatus::M_HK);
        default:
            return 0;
    }
}

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
      m_targetState(lockStates::LOCKED)
{
  m_override_state_event = AppEventLoop::subscribe(LOCK_EVENT, LOCK_OVERRIDE_STATE, [&](const uint8_t* data, size_t size){
      if(size == 0 || data == nullptr) return;
      std::span<const uint8_t> payload(data, size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) { ESP_LOGE(TAG, "Failed to deserialize override state event: %s", ec.message().c_str()); return; }
      ESP_LOGD(TAG, "Received override state event: %d -> %d from source %d", s.currentState, s.targetState, s.source);
      overrideState(s.currentState, s.targetState, Source(s.source));
    });
  m_target_state_event = AppEventLoop::subscribe(LOCK_EVENT, LOCK_TARGET_STATE_CHANGED, [&](const uint8_t* data, size_t size){
      if(size == 0 || data == nullptr) return;
      std::span<const uint8_t> payload(data, size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) { ESP_LOGE(TAG, "Failed to deserialize target state event: %s", ec.message().c_str()); return; }
      ESP_LOGD(TAG, "Received target state event: %d -> %d", s.currentState, s.targetState);
      setTargetState(s.targetState, Source(s.source));
    });
  m_update_state_event = AppEventLoop::subscribe(LOCK_EVENT, LOCK_UPDATE_STATE, [&](const uint8_t* data, size_t size){
      if(size == 0 || data == nullptr) return;
      std::span<const uint8_t> payload(data, size);
      std::error_code ec;
      EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
      if(ec) { ESP_LOGE(TAG, "Failed to deserialize update state event: %s", ec.message().c_str()); return; }
      ESP_LOGD(TAG, "Received update state event: %d -> %d", s.currentState, s.targetState);
      m_currentState = s.currentState;
      s.targetState = m_targetState;
      s.source = LockManager::INTERNAL;
      std::array<uint8_t, sizeof(EventLockState)> d{};
      size_t d_len = alpaca::serialize(s, d);
      AppEventLoop::publish(LOCK_EVENT, LOCK_STATE_CHANGED, d.data(), d_len);
    });
  esp_timer_create_args_t momentaryStateTimer_arg = {
    .callback = handleTimer,
    .arg = this,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "momentaryStateTimer",
    .skip_unhandled_events = false
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
  m_nfc_event = AppEventLoop::subscribe(NFC_EVENT, NFC_TAP_EVENT, [&](const uint8_t* data, size_t size){
      if(size == 0 || data == nullptr) return;
      std::span<const uint8_t> payload(data, size);
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
  std::array<uint8_t, sizeof(EventLockState)> d{};
  size_t d_len = alpaca::serialize(s, d);
  AppEventLoop::publish(HW_EVENT, HW_ACTION, d.data(), d_len);
}

/**
 * @brief Timer callback that sets the lock's target state to LOCKED with source INTERNAL.
 *
 * @param instance Pointer to the LockManager instance provided to the timer callback.
 */
void LockManager::handleTimer(void* instance){
  static_cast<LockManager*>(instance)->setTargetState(LOCKED, INTERNAL);
}

void LockManager::stopMomentaryTimer() {
    if (momentaryStateTimer && esp_timer_is_active(momentaryStateTimer)) {
        esp_timer_stop(momentaryStateTimer);
    }
}

void LockManager::startMomentaryTimerIfNeeded(Source source) {
    uint8_t momentarySources = (((m_actionsConfig.gpioActionMomentaryEnabled |
                                  m_actionsConfig.gpioActionPin) == 255) &
                                !m_actionsConfig.hkDumbSwitchMode)
                                   ? 0
                                   : m_actionsConfig.gpioActionMomentaryEnabled;
    bool isMomentarySource = ((sourceToMomentaryMask(source) & momentarySources) != 0);

    if (m_targetState == lockStates::UNLOCKED && isMomentarySource) {
        if (!momentaryStateTimer) {
            ESP_LOGE(TAG, "Cannot start momentary unlock timer: timer was not created.");
            return;
        }
        if (esp_timer_is_active(momentaryStateTimer)) {
            return;
        }
        ESP_LOGI(TAG,
                 "Starting momentary unlock timer for %d ms from source %d.",
                 m_actionsConfig.gpioActionMomentaryTimeout,
                 static_cast<int>(source));
        esp_err_t err = esp_timer_start_once(momentaryStateTimer, m_actionsConfig.gpioActionMomentaryTimeout * 1000);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start momentary unlock timer: %s", esp_err_to_name(err));
        }
    }
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

    stopMomentaryTimer();

    m_targetState = state;

    EventLockState s{
      .currentState = m_currentState,
      .targetState = m_targetState,
      .source = LockManager::INTERNAL
    };
    std::array<uint8_t, sizeof(EventLockState)> d{};
    size_t d_len = alpaca::serialize(s, d);
    if (m_actionsConfig.hkDumbSwitchMode) {
      ESP_LOGI(TAG, "Dummy Action is enabled!");
      m_currentState = m_targetState;
      s.currentState = m_targetState;
      d_len = alpaca::serialize(s, d);
      AppEventLoop::publish(HW_EVENT, HW_ACTION, d.data(), d_len);
    } else if((source == NFC && m_actionsConfig.hkGpioControlledState) || source != NFC) {
      AppEventLoop::publish(HW_EVENT, HW_ACTION, d.data(), d_len);
    }
    AppEventLoop::publish(LOCK_EVENT, LOCK_STATE_CHANGED, d.data(), d_len);

    startMomentaryTimerIfNeeded(source);
}

/**
 * @brief Apply an external override to the lock's current and/or target state and notify observers.
 *
 * If a provided state equals 255, that state is left unchanged. Stops any active momentary-state timer,
 * updates the internal current and target states, publishes the resulting state to "lock/action"
 * and "lock/stateChanged", and restarts momentary timing when the override requests an unlocked
 * state from a configured momentary source.
 *
 * @param c_state External current state to apply, or 255 to leave the current state unchanged.
 * @param t_state External target state to apply, or 255 to leave the target state unchanged.
 * @param source Origin of the override, used for momentary timer decisions.
 */
void LockManager::overrideState(uint8_t c_state, uint8_t t_state, Source source) {
    if (c_state == m_currentState && t_state == m_targetState) {
        stopMomentaryTimer();
        startMomentaryTimerIfNeeded(source);
        return;
    }

    ESP_LOGI(TAG, "External source %d reported new c_state: %d t_state: %d. Overriding internal state.", static_cast<int>(source), c_state, t_state);

    m_currentState = c_state != lockStates::MAX ? c_state : m_currentState;
    m_targetState = t_state != lockStates::MAX ? t_state : m_targetState;

    stopMomentaryTimer();
    EventLockState s{
      .currentState = static_cast<uint8_t>(m_currentState),
      .targetState = static_cast<uint8_t>(m_targetState),
      .source = LockManager::INTERNAL
    };
    std::array<uint8_t, sizeof(EventLockState)> d{};
    size_t d_len = alpaca::serialize(s, d);
    AppEventLoop::publish(HW_EVENT, HW_ACTION, d.data(), d_len);
    AppEventLoop::publish(LOCK_EVENT, LOCK_STATE_CHANGED, d.data(), d_len);
    startMomentaryTimerIfNeeded(source);
}
