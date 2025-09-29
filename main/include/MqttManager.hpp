#pragma once
#include "mqtt_client.h"
#include <string>
#include <vector>
#include "esp_tls.h"
#include "esp_timer.h"

class LockManager;
class ConfigManager;
namespace espConfig { struct mqttConfig_t; }

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
    
    /**
     * @brief Reconnects the MQTT client with new certificates.
     * @return True if reconnection was triggered successfully, false otherwise.
     */
    bool reconnectWithNewCertificates();

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
    bool loadCertificates();
    void logSSLError(const char* operation, esp_err_t error);
    
    // Internal reconnection methods
    void startReconnectionTimer();
    void stopReconnectionTimer();
    void handleReconnection();

    // Health check methods
    void startHealthCheckTimer();
    void stopHealthCheckTimer();
    void performHealthCheck();

    // --- Member Variables ---
    std::string deviceID;
    const espConfig::mqttConfig_t& m_mqttConfig;
    esp_mqtt_client_handle_t m_client;
    const std::string &device_name;
    
    // SSL/TLS related members
    esp_tls_cfg_t m_tls_cfg;
    bool m_sslConfigured;
    
    // Reconnection state
    bool m_isConnected;
    bool m_isReconnecting;
    esp_timer_handle_t m_reconnectionTimer;
    uint32_t m_reconnectionAttempts;
    uint32_t m_maxReconnectionAttempts;
    uint32_t m_reconnectionDelayMs;
    static constexpr uint32_t MAX_RECONNECTION_DELAY_MS = 30000; // 30 seconds
    static constexpr uint32_t INITIAL_RECONNECTION_DELAY_MS = 1000; // 1 second

    // Health check state
    esp_timer_handle_t m_healthCheckTimer;
    uint32_t m_lastHealthCheckTime;
    static constexpr uint32_t HEALTH_CHECK_INTERVAL_MS = 300000; // 5 minutes
    
    static const char* TAG;
};

