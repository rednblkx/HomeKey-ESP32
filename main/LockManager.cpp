#include "esp_log.h"
#include "esp_timer.h"
#include "event_manager.hpp"
#include "config.hpp"
#include "LockManager.hpp"
#include "HardwareManager.hpp"
#include "eventStructs.hpp"

const char* LockManager::TAG = "LockManager";

LockManager::LockManager(HardwareManager& hardwareManager, const espConfig::misc_config_t& miscConfig)
    : m_hardwareManager(hardwareManager),
      m_miscConfig(miscConfig),
      m_currentState(lockStates::LOCKED),
      m_targetState(lockStates::LOCKED)
{
  espp::EventManager::get().add_publisher("lock/stateChanged", "LockManager");
  espp::EventManager::get().add_publisher("lock/feedback", "LockManager");
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
      "nfc/HomeKeyTap", "LockManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventHKTap s = alpaca::deserialize<EventHKTap>(data, ec);
        if (!ec && s.status) {
          processNfcRequest(true);
        } else
          processNfcRequest(false);
      },
      3072);
  esp_timer_create_args_t momentaryStateTimer_arg = {
    .callback = handleTimer,
    .arg = this,
    .name = "momentaryStateTimer"
  };
  esp_timer_create(&momentaryStateTimer_arg, &momentaryStateTimer);
}

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

void LockManager::handleTimer(void* instance){
  static_cast<LockManager*>(instance)->setTargetState(LOCKED, INTERNAL);
}

// --- State Getters ---

int LockManager::getCurrentState() const {
    return m_currentState;
}

int LockManager::getTargetState() const {
    return m_targetState;
}

// --- Command Methods ---

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
    if (m_miscConfig.hkDumbSwitchMode) {
      ESP_LOGI(TAG, "Dummy Action is enabled!");
      m_currentState = m_targetState;
      s.currentState = m_targetState;
      d.clear();
      alpaca::serialize(s, d);
    } else if((source == NFC && m_miscConfig.hkGpioControlledState) || source != NFC) {
      espp::EventManager::get().publish("lock/action", d);
    }
    espp::EventManager::get().publish("lock/stateChanged", d);

    uint8_t momentarySources = m_miscConfig.gpioActionMomentaryEnabled;
    bool isMomentarySource = ((static_cast<uint8_t>(source) & momentarySources) != 0);

    if (m_targetState == lockStates::UNLOCKED && isMomentarySource) {
        ESP_LOGI(TAG, "Starting momentary unlock timer for %d ms.", m_miscConfig.gpioActionMomentaryTimeout);
        esp_timer_start_once(momentaryStateTimer, m_miscConfig.gpioActionMomentaryTimeout * 1000);
    }
}

void LockManager::processNfcRequest(bool status) {
    ESP_LOGI(TAG, "Processing NFC tap request...");
    if(status){
      EventBinaryStatus s{status};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/feedback", d);
      if (m_miscConfig.lockAlwaysUnlock) {
        setTargetState(lockStates::UNLOCKED, Source::NFC);
      } else if (m_miscConfig.lockAlwaysLock) {
        setTargetState(lockStates::LOCKED, Source::NFC);
      } else {
        int newState = (m_currentState == lockStates::LOCKED) ? lockStates::UNLOCKED : lockStates::LOCKED;
        setTargetState(newState, Source::NFC);
      }
    } else {
      EventBinaryStatus s{false};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/feedback", d);
    }
}

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

