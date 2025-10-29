#pragma once
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
     * @brief Publishes the current state of the lock.
     * Called by the main application loop when a state change is detected.
     * @param currentState The current physical state of the lock.
     * @param targetState The intended state of the lock.
     */
    void publishLockState(int currentState, int targetState);

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
    void publishUidTap(const std::vector<uint8_t>& uid, const std::vector<uint8_t> &atqa, const std::vector<uint8_t> &sak);

    // --- Reconnection and Connection Status ---
    /**
     * @brief Checks if the MQTT client is currently connected.
     * @return True if connected, false otherwise.
     */
    bool isConnected() const;

private:
    // --- Event Handling ---
    static void mqttEventHandler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
    void onMqttEvent(esp_event_base_t base, int32_t event_id, void* event_data);
    void onConnected();
    void onData(const std::string& topic, const std::string& data);

    // --- Publishing Logic ---
    void publish(const std::string& topic, const std::string& payload, int qos = 0, bool retain = false);
    void publishHassDiscovery();

    // --- SSL/TLS Configuration ---
    bool configureSSL(esp_mqtt_client_config_t& mqtt_cfg);
    void logSSLError(const char* operation, esp_err_t error);

    // --- Member Variables ---
    std::string deviceID;
    const espConfig::mqttConfig_t& m_mqttConfig;
    const espConfig::mqtt_ssl_t* m_mqttSslConfig;
    esp_mqtt_client_handle_t m_client;
    const std::string &device_name;
    bool m_isConnected;
    
    // SSL/TLS related members
    bool m_sslConfigured;
    
    static const char* TAG;
};

