#pragma once
#include "config.hpp"
#include "esp_timer.h"
#include "app_event_loop.hpp"
class HardwareManager;
class ConfigManager;
class LockApplication;

namespace espConfig { struct misc_config_t; struct actions_config_t; };

/**
 * @class LockManager
 * @brief Manages the state and core logic of the lock mechanism.
 *
 * This class is the single source of truth for the lock's state (Locked, Unlocked, etc.).
 * It receives commands from various sources (HomeKit, MQTT, NFC) and orchestrates the
 * necessary actions, including controlling the physical hardware and managing timers.
 */
class LockManager {
public:
    /**
     * @enum Source
     * @brief Defines the origin of a lock command, allowing for different logic paths.
     * Not really used at the moment
     */
    enum Source {
        INTERNAL,
        HOMEKIT,
        NFC,
        MQTT,
    };

    enum lockStates { UNLOCKED, LOCKED, JAMMED, UNKNOWN, UNLOCKING, LOCKING, MAX };

    /**
     * @brief Constructs the LockManager.
     * @param stateChanged Callback to notify of a state change
     * @param hardwareManager Reference to the manager that controls physical GPIOs.
     * @param configManager Reference to the application configuration manager.
     */
    LockManager(const espConfig::misc_config_t& miscConfig, const espConfig::actions_config_t& actionsConfig);
    /**
     * @brief Releases LockManager resources and unsubscribes its event handlers.
     *
     * Unsubscribes the manager's EventBus subscriber handles and cleans up the
     * momentary state timer if present. Unsubscribe calls are safe when handles
     * are null (no-op). If the timer exists, it is stopped if active and then
     * deleted.
     */
    ~LockManager() {
      if(momentaryStateTimer) {
        if(esp_timer_is_active(momentaryStateTimer)) {
          esp_timer_stop(momentaryStateTimer);
        }
        esp_timer_delete(momentaryStateTimer);
      }
    }

    /**
     * @brief Initializes the lock state to its default.
     */
    void begin();

    // --- State Getters ---
    int getCurrentState() const;
    int getTargetState() const;

    // --- Command Methods ---

    /**
     * @brief Sets the intended target state of the lock, triggering a physical action.
     * @param state The desired state (e.g., lockStates::LOCKED).
     * @param source The origin of the command.
     */
    void setTargetState(uint8_t state, Source source);

    /**
     * @brief Forces the manager's internal state to match an external report.
     * Useful for syncing with external state changes reported via MQTT or HomeKit.
     * @param state The new current state
     * @param tstate The new target state
     * @param source The origin of the override.
     */
    void overrideState(uint8_t cstate, uint8_t tstate, Source source);
private:
    const espConfig::misc_config_t& m_miscConfig;
    const espConfig::actions_config_t& m_actionsConfig;

    uint8_t m_currentState;
    uint8_t m_targetState;
    AppEventLoop::SubscriptionHandle m_override_state_event;
    AppEventLoop::SubscriptionHandle m_target_state_event;
    AppEventLoop::SubscriptionHandle m_update_state_event;
    AppEventLoop::SubscriptionHandle m_nfc_event;

    esp_timer_handle_t momentaryStateTimer = nullptr;

    static const char* TAG;
    static void handleTimer(void* instance);
    void stopMomentaryTimer();
    void startMomentaryTimerIfNeeded(Source source);
};
