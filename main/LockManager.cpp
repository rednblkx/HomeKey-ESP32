#include "esp_log.h"
#include "esp_timer.h"
#include "event_manager.hpp"
#include "config.hpp"
#include "LockManager.hpp"
#include "HardwareManager.hpp"
#include "eventStructs.hpp"

const char* LockManager::TAG = "LockManager";

/**
 * @brief Construct a LockManager configured for managing lock state and events.
 *
 * Initializes internal state from the provided miscConfig, registers event
 * publishers and subscribers for lock state/action updates and NFC events,
 * and creates the momentary-state timer used for temporary unlocks.
 *
 * @param miscConfig Configuration values that control NFC behavior, momentary
 *                   timeout and related lock handling flags (e.g. lockAlwaysUnlock,
 *                   lockAlwaysLock, GPIO/momentary source settings).
 */
LockManager::LockManager(const espConfig::misc_config_t& miscConfig, const espConfig::actions_config_t& actionsConfig)
    : m_miscConfig(miscConfig),
      m_actionsConfig(actionsConfig),
      m_currentState(lockStates::LOCKED),
      m_targetState(lockStates::LOCKED)
{
  espp::EventManager::get().add_publisher("lock/stateChanged", "LockManager");
  espp::EventManager::get().add_publisher("lock/action", "LockManager");
  espp::EventManager::get().add_subscriber(
      "lock/overrideState", "LockManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(data, ec);
        if (!ec) {
          overrideState(s.currentState, s.targetState);
        }
      }, 3072);
  espp::EventManager::get().add_subscriber(
      "lock/targetStateChanged", "LockManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(data, ec);
        if (!ec) {
          setTargetState(s.targetState, Source(s.source));
        }
      }, 3072);
  espp::EventManager::get().add_subscriber(
      "lock/updateState", "LockManager", [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(data, ec);
        if (!ec) {
          m_currentState = s.currentState;
          s.currentState = m_currentState;
          s.targetState = m_targetState;
          EventLockState s{
            .currentState = static_cast<uint8_t>(m_currentState),
            .targetState = static_cast<uint8_t>(m_targetState),
            .source = LockManager::INTERNAL
          };
          std::vector<uint8_t> d;
          alpaca::serialize(s, d);
          espp::EventManager::get().publish("lock/stateChanged", d);
        }
      }, 4096);
  espp::EventManager::get().add_subscriber(
      "nfc/event", "LockManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        NfcEvent event = alpaca::deserialize<NfcEvent>(data, ec);
        if(ec) return;
        if(event.type == HOMEKEY_TAP) {
          ESP_LOGI(TAG, "Processing NFC tap request...");
          EventHKTap s = alpaca::deserialize<EventHKTap>(event.data, ec);
          if (!ec && s.status) {
            if (m_miscConfig.lockAlwaysUnlock) {
              setTargetState(lockStates::UNLOCKED, Source::NFC);
            } else if (m_miscConfig.lockAlwaysLock) {
              setTargetState(lockStates::LOCKED, Source::NFC);
            } else {
              int newState = (m_currentState == lockStates::LOCKED) ? lockStates::UNLOCKED : lockStates::LOCKED;
              setTargetState(newState, Source::NFC);
            }
          }
        }
      },
      3072);
  esp_timer_create_args_t momentaryStateTimer_arg = {
    .callback = handleTimer,
    .arg = this,
    .name = "momentaryStateTimer"
  };
  esp_timer_create(&momentaryStateTimer_arg, &momentaryStateTimer);
}

/**
 * @brief Publish the manager's current and target lock state with source INTERNAL to initialize external listeners.
 *
 * Constructs an EventLockState reflecting the LockManager's current and target states (source set to INTERNAL)
 * and publishes it on the "lock/action" event channel so external components receive the initial lock state.
 */
void LockManager::begin() {
  ESP_LOGI(TAG, "Initializing to Default/NVS state");
  EventLockState s{
    .currentState = static_cast<uint8_t>(m_currentState),
    .targetState = static_cast<uint8_t>(m_targetState),
    .source = LockManager::INTERNAL
  };
  std::vector<uint8_t> d;
  alpaca::serialize(s, d);
  espp::EventManager::get().publish("lock/action", d);
  // espp::EventManager::get().publish("lock/stateChanged", d);
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
 * @brief Request a change of the lock's target state and notify observers.
 *
 * Updates the stored target state, optionally updates the current state in dumb-switch mode,
 * publishes a `lock/action` event when an external action should be performed, always publishes
 * a `lock/stateChanged` event with the resulting current and target states, and starts the
 * momentary unlock timer when an unlock was requested by a configured momentary source.
 *
 * @param state Desired target lock state (use values from lockStates).
 * @param source Origin of the request which may affect publishing behavior and momentary handling.
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
      espp::EventManager::get().publish("lock/action", d);
    }
    espp::EventManager::get().publish("lock/stateChanged", d);

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
    espp::EventManager::get().publish("lock/action", d);
    espp::EventManager::get().publish("lock/stateChanged", d);
}

