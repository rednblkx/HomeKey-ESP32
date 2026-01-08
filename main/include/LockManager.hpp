#pragma once
#include "config.hpp"
#include "esp_timer.h"
#include "event_bus.hpp"
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
        MQTT = 4,
    };

    enum lockStates { UNLOCKED, LOCKED, JAMMED, UNKNOWN, UNLOCKING, LOCKING };

    /**
     * @brief Constructs the LockManager.
     * @param stateChanged Callback to notify of a state change
     * @param hardwareManager Reference to the manager that controls physical GPIOs.
     * @param configManager Reference to the application configuration manager.
     */
    LockManager(const espConfig::misc_config_t& miscConfig, const espConfig::actions_config_t& actionsConfig);
    ~LockManager() {EventBus::Bus::instance().unsubscribe(m_override_state_event);EventBus::Bus::instance().unsubscribe(m_target_state_event);EventBus::Bus::instance().unsubscribe(m_update_state_event);EventBus::Bus::instance().unsubscribe(m_nfc_event);}

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
     * This method updates the state WITHOUT triggering a physical action.
     * Useful for syncing with external state changes reported via MQTT.
     * @param state The new current state
     * @param tstate The new target state
     */
    void overrideState(uint8_t cstate, uint8_t tstate);
private:
    const espConfig::misc_config_t& m_miscConfig;
    const espConfig::actions_config_t& m_actionsConfig;

    uint8_t m_currentState;
    uint8_t m_targetState;
    EventBus::SubscriberHandle m_override_state_event;
    EventBus::SubscriberHandle m_target_state_event;
    EventBus::SubscriberHandle m_update_state_event;
    EventBus::SubscriberHandle m_nfc_event;

    esp_timer_handle_t momentaryStateTimer;

    static const char* TAG;
    const EventBus::TopicHandle bus_topic;
    static void handleTimer(void* instance);
};

