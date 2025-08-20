#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include "config.hpp"
#include "esp_log.h"
#include <functional>

class HardwareManager;
class ConfigManager;
class LockApplication;

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
    enum class Source {
        INTERNAL,
        HOMEKIT,
        MQTT,
        NFC
    };

    /**
     * @brief Constructs the LockManager.
     * @param stateChanged Callback to notify of a state change
     * @param hardwareManager Reference to the manager that controls physical GPIOs.
     * @param configManager Reference to the application configuration manager.
     */
    LockManager(void(*stateChanged)(int cstate, int tstate), HardwareManager& hardwareManager, ConfigManager& configManager);

    /**
     * @brief Initializes the lock state to its default.
     */
    void begin();

    /**
     * @brief A periodic function to be called from the main loop to handle time-based events.
     * Manages the momentary unlock timeout at the moment.
     */
    void loop();

    // --- State Getters ---
    int getCurrentState() const;
    int getTargetState() const;

    // --- Command Methods ---

    /**
     * @brief Sets the intended target state of the lock, triggering a physical action.
     * @param state The desired state (e.g., lockStates::LOCKED).
     * @param source The origin of the command.
     */
    void setTargetState(int state, Source source);

    /**
     * @brief Processes a request originating from an NFC tap.
     * @param status NFC Interaction result
     * This method contains the specific business logic for NFC (always unlock, toggle, etc.).
     * Probably should be moved somewhere else
     */
    void processNfcRequest(bool status);
    
    /**
     * @brief Processes a custom command from MQTT.
     * Translates custom MQTT states/commands into standard lock actions.
     * @param command The integer value of the custom command.
     * TODO: Move to MqttManager and just use setTargetState or overrideState
     */
    void processCustomMqttCommand(int command);

    /**
     * @brief Forces the manager's internal state to match an external report.
     * This method updates the state WITHOUT triggering a physical action.
     * Useful for syncing with external state changes reported via MQTT.
     * @param state The new current state
     * @param tstate The new target state
     */
    void overrideState(int cstate, int tstate);

private:
    const std::function<void(int, int)> m_stateChanged = {};
    HardwareManager& m_hardwareManager;
    const espConfig::misc_config_t& m_miscConfig;
    const std::map<std::string, int> &customLockActions;

    int m_currentState;
    int m_targetState;

    bool m_momentaryUnlockActive;
    unsigned long m_momentaryUnlockStartTime;

    static const char* TAG;
};

#endif // LOCK_MANAGER_H
