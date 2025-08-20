#include "LockManager.hpp"
#include "HardwareManager.hpp"
#include "ConfigManager.hpp"

#include <esp_log.h>
#include <functional>

const char* LockManager::TAG = "LockManager";

LockManager::LockManager(void(*stateChanged)(int cstate, int tstate), HardwareManager& hardwareManager, ConfigManager& configManager)
    : m_stateChanged(stateChanged),
      m_hardwareManager(hardwareManager),
      m_miscConfig(configManager.getMiscConfig()),
      customLockActions(configManager.getMqttConfig().customLockActions),
      m_currentState(lockStates::LOCKED),
      m_targetState(lockStates::LOCKED),
      m_momentaryUnlockActive(false),
      m_momentaryUnlockStartTime(0)
{}

void LockManager::begin() {
    ESP_LOGI(TAG, "Initializing to default state: LOCKED");
    m_hardwareManager.setLockOutput(m_currentState);
    m_stateChanged(m_currentState, m_targetState);
}

void LockManager::loop() {
    if (m_momentaryUnlockActive && ((esp_timer_get_time()/1000) - m_momentaryUnlockStartTime > m_miscConfig.gpioActionMomentaryTimeout)) {
        ESP_LOGI(TAG, "Momentary unlock timeout reached. Re-locking.");
        m_momentaryUnlockActive = false;
        setTargetState(lockStates::LOCKED, Source::INTERNAL);
    }
}

// --- State Getters ---

int LockManager::getCurrentState() const {
    return m_currentState;
}

int LockManager::getTargetState() const {
    return m_targetState;
}

// --- Command Methods ---

void LockManager::setTargetState(int state, Source source) {
    if (state == m_targetState && m_currentState == m_targetState) {
        ESP_LOGD(TAG, "Requested state is already the current state. No action taken.");
        return;
    }

    ESP_LOGI(TAG, "Setting target state to %d from source %d", state, static_cast<int>(source));

    m_momentaryUnlockActive = false;

    m_targetState = state;
    
    m_hardwareManager.setLockOutput(m_targetState);

    m_currentState = m_targetState;
    m_stateChanged(m_currentState, m_targetState);

    uint8_t momentarySources = m_miscConfig.gpioActionMomentaryEnabled;
    bool isMomentarySource = ((static_cast<uint8_t>(source) & momentarySources) != 0);

    if (m_targetState == lockStates::UNLOCKED && isMomentarySource) {
        ESP_LOGI(TAG, "Starting momentary unlock timer for %d ms.", m_miscConfig.gpioActionMomentaryTimeout);
        m_momentaryUnlockActive = true;
        m_momentaryUnlockStartTime = (esp_timer_get_time()/1000);
    }
}

void LockManager::processNfcRequest(bool status) {
    ESP_LOGI(TAG, "Processing NFC tap request...");
    if(status){
      m_hardwareManager.showSuccessFeedback();
      if (m_miscConfig.lockAlwaysUnlock) {
        setTargetState(lockStates::UNLOCKED, Source::NFC);
      } else if (m_miscConfig.lockAlwaysLock) {
        setTargetState(lockStates::LOCKED, Source::NFC);
      } else {
        int newState = (m_currentState == lockStates::LOCKED) ? lockStates::UNLOCKED : lockStates::LOCKED;
        setTargetState(newState, Source::NFC);
      }
    } else {
      m_hardwareManager.showFailureFeedback();
    }
}

void LockManager::overrideState(int c_state, int t_state) {
    if (c_state == m_currentState && t_state == m_targetState) return;

    ESP_LOGI(TAG, "External source reported new c_state: %d t_state: %d. Overriding internal state.", c_state, t_state);

    m_currentState = c_state != -1 ? c_state : m_currentState;
    m_targetState = t_state != -1 ? t_state : m_targetState;

    m_momentaryUnlockActive = false;
}

void LockManager::processCustomMqttCommand(int command) {
    ESP_LOGI(TAG, "Processing custom MQTT command: %d", command);

    if (customLockActions.count("UNLOCK") && customLockActions.at("UNLOCK") == command) {
        setTargetState(lockStates::UNLOCKED, Source::MQTT);
    } else if (customLockActions.count("LOCK") && customLockActions.at("LOCK") == command) {
        setTargetState(lockStates::LOCKED, Source::MQTT);
    } else {
        ESP_LOGW(TAG, "Received unknown custom MQTT command: %d", command);
    }
}
