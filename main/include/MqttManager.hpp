#pragma once
#include "app_event_loop.hpp"
#include "eventStructs.hpp"
#include "mqtt_client.h"
#include <string>
#include <vector>

class LockManager;
class ConfigManager;
namespace espConfig { struct mqttConfig_t; struct mqtt_ssl_t;}

/**
 * @class MqttManager
 * @brief Manages all MQTT client operations, including connection, discovery, and communication.
 *
 * This class handles the full lifecycle of the MQTT connection. It publishes device
 * state, reports events like NFC taps, and listens for commands from the MQTT broker
 * to control the lock.
 */
class MqttManager {
public:
    /**
     * @brief Constructs the MqttManager.
     * @param lockManager Reference to the core lock logic manager.
     * @param configManager Reference to the application configuration manager.
     */
     MqttManager(const ConfigManager& configManager);

     /**
      * @brief Destroys the MqttManager and cleans up resources.
      */
     ~MqttManager();

    /**
     * @brief Initializes and starts the MQTT client.
     * @return True if the client was started, false if the configuration was invalid.
     */
    bool begin(std::string deviceID);

    /**
     * @brief Checks if the MQTT client is currently connected.
     * @return True if connected, false otherwise.
     */
    bool isConnected() const;

    /**
     * @brief Gets the last MQTT error code.
     * @return The last error code.
     */
    MqttErrorCode getLastErrorCode() const { return m_lastErrorCode; }

    /**
     * @brief Gets the last MQTT error message.
     * @return The last error message.
     */
    const std::string& getLastErrorMessage() const { return m_lastErrorMessage; }

    /**
     * @brief Stops the MQTT client and unsubscribes from all EventBus listeners.
     *
     * Performs a clean shutdown of the MQTT client by stopping and destroying it,
     * then removes all EventBus subscriptions registered by this instance.
     */
    void end();

private:
    /**
      * @brief Publishes the current state of the lock.
      * Called by the main application loop when a state change is detected.
      * @param currentState The current physical state of the lock.
      * @param targetState The intended state of the lock.
      */
    void publishLockState(const int currentState, const int targetState);

    /**
      * @brief Publishes a successful HomeKey authentication event.
      * @param issuerId The ID of the authenticated issuer.
      * @param endpointId The ID of the authenticated endpoint.
      * @param readerId The ID of this reader device.
      */
    void publishHomeKeyTap(const std::vector<uint8_t>& issuerId, const std::vector<uint8_t>& endpointId, const std::vector<uint8_t>& readerId);

    /**
      * @brief Publishes a generic (non-HomeKey) NFC tag scan event.
      * @param uid Pointer to the UID byte array.
      * @param uidLen Length of the UID.
      * @param atqa Pointer to the ATQA byte array (can be null).
      * @param sak Pointer to the SAK byte array (can be null).
      */
    void publishUidTap(const std::vector<uint8_t>& uid, const std::array<uint8_t,2> &atqa, const uint8_t &sak);
    // --- Event Handling ---
    static void mqttEventHandler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
    void onMqttEvent(esp_event_base_t base, int32_t event_id, void* event_data);
    void onConnected();
    void onData(const std::string& topic, const std::string& data);

    // --- Publishing Logic ---
    void publish(const std::string& topic, const std::string& payload, int qos = 0, bool retain = false);
    void publishHassDiscovery();
    void publishMqttStatus(bool connected, MqttErrorCode errorCode, const std::string& errorMessage = "");

    // --- SSL/TLS Configuration ---
    bool configureSSL(esp_mqtt_client_config_t& mqtt_cfg);
    void logSSLError(const char* operation, esp_err_t error);

    // --- Member Variables ---
    std::string deviceID;
    const espConfig::mqttConfig_t& m_mqttConfig;
    const espConfig::mqtt_ssl_t& m_mqttSslConfig;
    esp_mqtt_client_handle_t m_client;
    const std::string &device_name;
    bool m_isConnected = false;
    
    // SSL/TLS related members
    bool m_sslConfigured;
    
    static const char* TAG;
    AppEventLoop::SubscriptionHandle m_lock_state_changed;
    AppEventLoop::SubscriptionHandle m_alt_action;
    AppEventLoop::SubscriptionHandle m_nfc_event;

    // Status tracking (replaces event-based status publishing)
    MqttErrorCode m_lastErrorCode = MqttErrorCode::NONE;
    std::string m_lastErrorMessage;
};

