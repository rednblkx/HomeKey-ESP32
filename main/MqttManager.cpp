#include "config.hpp"
#include "event_manager.hpp"
#include "MqttManager.hpp"
#include "LockManager.hpp"
#include "ConfigManager.hpp"
#include <esp_log.h>
#include <esp_app_desc.h>
#include "eventStructs.hpp"
#include <cJSON.h>
#include <vector>
#include <cstring>

const char* MqttManager::TAG = "MqttManager";

/**
 * @brief Initialize MqttManager from configuration and register MQTT-related event subscribers and publishers.
 *
 * Constructs the manager using settings from the provided ConfigManager, initializes internal MQTT configuration
 * and device name, and registers event subscribers and publishers used to bridge internal events (lock state,
 * alternate lock actions, and NFC/Home Key events) to MQTT publishing and vice versa.
 *
 * @param configManager Source of runtime configuration values used to initialize the MQTT manager.
 */
MqttManager::MqttManager(const ConfigManager& configManager)
    : m_mqttConfig(configManager.getConfig<espConfig::mqttConfig_t>()),
      m_client(nullptr),
      device_name(configManager.getConfig<espConfig::misc_config_t>().deviceName),
      m_sslConfigured(false),
      m_isConnected(false),
      m_isReconnecting(false),
      m_reconnectionTimer(nullptr),
      m_reconnectionAttempts(0),
      m_maxReconnectionAttempts(10),
      m_reconnectionDelayMs(INITIAL_RECONNECTION_DELAY_MS),
      m_healthCheckTimer(nullptr),
      m_lastHealthCheckTime(0)
{
    // Initialize TLS configuration
    memset(&m_tls_cfg, 0, sizeof(m_tls_cfg));
  espp::EventManager::get().add_subscriber(
      "lock/stateChanged", "MqttManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(data, ec);
        if(!ec) {
          publishLockState(s.currentState, s.targetState);
        }
      },3072);
  espp::EventManager::get().add_subscriber(
      "lock/altAction", "MqttManager",
      [&](const std::vector<uint8_t> &data) {
        publish(m_mqttConfig.hkAltActionTopic, "1");
      },3072);
  espp::EventManager::get().add_publisher("lock/targetStateChanged", "MqttManager");
  espp::EventManager::get().add_publisher("lock/overrideState", "MqttManager");
  espp::EventManager::get().add_publisher("homekit/event", "MqttManager");
  espp::EventManager::get().add_subscriber("nfc/event", "MqttManager", [&](const std::vector<uint8_t> &data){
    std::error_code ec;
    NfcEvent event = alpaca::deserialize<NfcEvent>(data, ec);
    if(ec) return;
    switch(event.type) {
      case HOMEKEY_TAP: {
        EventHKTap s = alpaca::deserialize<EventHKTap>(event.data, ec);
        if(!ec && s.status){
          publishHomeKeyTap(s.issuerId, s.endpointId, s.readerId);
        }
        break;
      }
      case TAG_TAP: {
        EventTagTap s = alpaca::deserialize<EventTagTap>(event.data, ec);
        if(!ec){
          publishUidTap(s.uid, s.atqa, s.sak);
        }
        break;
      }
      default:
        break;
    }
  }, 3072);
}

MqttManager::~MqttManager() {
   // Clean up timers
   stopReconnectionTimer();
   stopHealthCheckTimer();

   // Clean up MQTT client if it exists
   if (m_client) {
       esp_mqtt_client_stop(m_client);
       esp_mqtt_client_destroy(m_client);
       m_client = nullptr;
   }
}

/**
 * @brief Initialize and start the MQTT client using stored configuration and a device identifier.
 *
 * Configures the MQTT client from m_mqttConfig, registers the instance-level MQTT event handler,
 * and starts the client if a valid broker hostname is present.
 *
 * @param deviceID Unique device identifier to associate with the MQTT client and discovery topics.
 * @return true if the MQTT client was started, false if startup was skipped because the broker host was not configured.
 */
bool MqttManager::begin(std::string deviceID) {
    ESP_LOGI(TAG, "Initializing...");

    // Do not start if the broker configuration is invalid or default
    if (m_mqttConfig.mqttBroker.empty() || m_mqttConfig.mqttBroker == "0.0.0.0") {
        ESP_LOGW(TAG, "MQTT broker host is not configured. MQTT client will not start.");
        return false;
    }

    this->deviceID = deviceID;

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.hostname = m_mqttConfig.mqttBroker.c_str();
    mqtt_cfg.broker.address.port = m_mqttConfig.mqttPort;
    
    // Configure SSL/TLS if enabled
    if (m_mqttConfig.useSSL) {
        ESP_LOGI(TAG, "SSL/TLS is enabled for MQTT connection");
        mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_SSL;
        
        if (!configureSSL(mqtt_cfg)) {
            ESP_LOGE(TAG, "Failed to configure SSL/TLS for MQTT connection");
            return false;
        }
    } else {
        mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
    }
    
    mqtt_cfg.credentials.client_id = m_mqttConfig.mqttClientId.c_str();
    mqtt_cfg.credentials.username = m_mqttConfig.mqttUsername.c_str();
    mqtt_cfg.credentials.authentication.password = m_mqttConfig.mqttPassword.c_str();
    mqtt_cfg.session.last_will.topic = m_mqttConfig.lwtTopic.c_str();
    mqtt_cfg.session.last_will.msg = "offline";
    mqtt_cfg.session.last_will.msg_len = 7;
    mqtt_cfg.session.last_will.retain = true;
    mqtt_cfg.session.last_will.qos = 1;

    m_client = esp_mqtt_client_init(&mqtt_cfg);
    if (!m_client) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return false;
    }
    
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_ANY, mqttEventHandler, this);
    esp_err_t start_result = esp_mqtt_client_start(m_client);
    
    if (start_result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(start_result));
        esp_mqtt_client_destroy(m_client);
        m_client = nullptr;
        return false;
    }

    ESP_LOGI(TAG, "MQTT client started successfully%s", m_mqttConfig.useSSL ? " with SSL/TLS" : "");
    return true;
}

/**
 * @brief Publish a message to the configured MQTT broker.
 *
 * Publishes the given payload to the specified MQTT topic using the provided QoS and retain flag.
 *
 * @param topic Destination MQTT topic.
 * @param payload Message payload (may be empty).
 * @param qos MQTT quality of service level (typically 0, 1, or 2).
 * @param retain If true, the broker will retain the message as the last known value for the topic.
 */
void MqttManager::publish(const std::string& topic, const std::string& payload, int qos, bool retain) {
    if (!m_client) {
        ESP_LOGW(TAG, "Cannot publish, MQTT client not initialized.");
        return;
    }
    esp_mqtt_client_publish(m_client, topic.c_str(), payload.c_str(), payload.length(), qos, retain);
}

/**
 * @brief MQTT event callback that forwards received events to the associated MqttManager instance.
 *
 * Casts @p handler_args to a MqttManager pointer and invokes the instance's onMqttEvent method with
 * the provided MQTT event parameters.
 *
 * @param handler_args Pointer to the MqttManager instance (passed by the MQTT library).
 * @param base MQTT event base identifier.
 * @param event_id Numeric MQTT event identifier.
 * @param event_data Pointer to event-specific data provided by the MQTT library.
 */

void MqttManager::mqttEventHandler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    MqttManager* instance = static_cast<MqttManager*>(handler_args);
    instance->onMqttEvent(base, event_id, event_data);
}

/**
 * @brief Dispatches incoming MQTT events to the appropriate handlers and logs status.
 *
 * Interprets the MQTT event contained in `event_data` and:
 * - invokes onConnected() when a connection is established,
 * - logs disconnection and errors,
 * - extracts topic and payload and forwards them to onData() for incoming messages,
 * - logs unhandled event types.
 *
 * @param base MQTT event base (unused by this method).
 * @param event_id Raw event identifier (unused; the method reads the id from `event_data`).
 * @param event_data Pointer to an `esp_mqtt_event_t`-compatible structure containing the MQTT event, topic, and payload.
 */
void MqttManager::onMqttEvent(esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    // Log all events with timestamps for monitoring
    ESP_LOGD(TAG, "MQTT Event received: %d", event->event_id);

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED: Connection established successfully");
            if (m_mqttConfig.useSSL) {
                ESP_LOGI(TAG, "TLS connection state: SECURE - SSL/TLS handshake completed");
            } else {
                ESP_LOGI(TAG, "Connection state: UNSECURE - TCP connection established");
            }
            onConnected();
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED: Client disconnected from broker");
            ESP_LOGI(TAG, "Connection state: DISCONNECTED - Previous state was %s",
                     m_isConnected ? "CONNECTED" : "NOT_CONNECTED");
            m_isConnected = false;
            // Stop health checks when disconnected
            stopHealthCheckTimer();
            ESP_LOGI(TAG, "Connection health monitoring: DISABLED - Connection lost");
            // Start reconnection timer for any disconnection
            startReconnectionTimer();
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED: Successfully subscribed to topic (msg_id=%d)", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED: Successfully unsubscribed from topic (msg_id=%d)", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED: Message published successfully (msg_id=%d)", event->msg_id);
            break;
        case MQTT_EVENT_DATA: {
            std::string topic(event->topic, event->topic_len);
            std::string data(event->data, event->data_len);
            ESP_LOGD(TAG, "MQTT_EVENT_DATA: Received %d bytes on topic '%s'", event->data_len, topic.c_str());
            onData(topic, data);
            break;
        }
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT: Initiating connection attempt to %s:%d",
                     m_mqttConfig.mqttBroker.c_str(), m_mqttConfig.mqttPort);
            if (m_mqttConfig.useSSL) {
                ESP_LOGI(TAG, "TLS handshake state: INITIATING - Starting SSL/TLS connection process");
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR: Connection or protocol error occurred");
            if (event->error_handle) {
                ESP_LOGE(TAG, "MQTT Error Type: %d", event->error_handle->error_type);

                // Categorize and handle different error types with detailed logging
                switch (event->error_handle->error_type) {
                    case MQTT_ERROR_TYPE_TCP_TRANSPORT:
                        ESP_LOGE(TAG, "TCP transport error - network connectivity issue");
                        ESP_LOGE(TAG, "Troubleshooting: Check network connection, broker availability, and firewall settings");
                        if (event->error_handle->esp_transport_sock_errno != 0) {
                            ESP_LOGE(TAG, "Transport socket error: errno=%d (%s)",
                                     event->error_handle->esp_transport_sock_errno,
                                     strerror(event->error_handle->esp_transport_sock_errno));
                        }
                        break;
                    case MQTT_ERROR_TYPE_CONNECTION_REFUSED:
                        ESP_LOGE(TAG, "Connection refused - broker may be down or rejecting connection");
                        ESP_LOGE(TAG, "Troubleshooting: Verify broker address/port, check broker logs, ensure client credentials are correct");
                        break;
                    case MQTT_ERROR_TYPE_SUBSCRIBE_FAILED:
                        ESP_LOGE(TAG, "Subscribe failed - check topic permissions");
                        ESP_LOGE(TAG, "Troubleshooting: Verify MQTT client has permission to subscribe to configured topics");
                        break;
                    default:
                        ESP_LOGE(TAG, "Unknown MQTT error type: %d", event->error_handle->error_type);
                        break;
                }

                // Log SSL/TLS specific errors if available
                if (event->error_handle->esp_tls_last_esp_err != 0) {
                    logSSLError("MQTT SSL/TLS connection", event->error_handle->esp_tls_last_esp_err);
                    ESP_LOGI(TAG, "TLS handshake state: FAILED - SSL/TLS error occurred");
                }

                if (event->error_handle->esp_tls_stack_err != 0) {
                    ESP_LOGE(TAG, "TLS stack error: 0x%x", event->error_handle->esp_tls_stack_err);
                    ESP_LOGE(TAG, "Troubleshooting: Check SSL/TLS configuration, certificates, and cipher suites");
                }

                // Log connection attempt details
                ESP_LOGI(TAG, "Connection attempt details: broker=%s:%d, client_id=%s, ssl=%s",
                         m_mqttConfig.mqttBroker.c_str(), m_mqttConfig.mqttPort,
                         m_mqttConfig.mqttClientId.c_str(), m_mqttConfig.useSSL ? "enabled" : "disabled");
            } else {
                ESP_LOGE(TAG, "MQTT error occurred but no error handle available");
            }
            // Start reconnection on error if not already connected
            if (!m_isConnected && !m_isReconnecting) {
                ESP_LOGI(TAG, "Starting reconnection due to MQTT error");
                startReconnectionTimer();
            }
            break;
        default:
            ESP_LOGD(TAG, "Unhandled MQTT event: %d", event->event_id);
            break;
    }
}

/**
 * @brief Handle actions to perform immediately after the MQTT client connects.
 *
 * Publishes the configured "online" last-will/retain presence message, subscribes to configured command topics used for lock and battery commands, and triggers Home Assistant MQTT discovery if enabled.
 *
 * The retained presence message is published to the configured LWT topic with QoS 1. Subscriptions include lock state/command topics and the battery level command topic; the custom lock state command topic is subscribed only when custom states are enabled. If Home Assistant discovery is enabled in configuration, discovery payloads are published.
 */
void MqttManager::onConnected() {
    ESP_LOGI(TAG, "MQTT client connected%s", m_mqttConfig.useSSL ? " with SSL/TLS encryption" : "");

    m_isConnected = true;
    m_isReconnecting = false;
    m_reconnectionAttempts = 0; // Reset reconnection attempts on successful connection
    stopReconnectionTimer(); // Stop any ongoing reconnection attempts

    if (m_mqttConfig.useSSL) {
        ESP_LOGI(TAG, "SSL/TLS connection established successfully");
    }

    // Start periodic health checks
    startHealthCheckTimer();
    ESP_LOGI(TAG, "Connection health monitoring: ENABLED - Health checks every %d minutes", HEALTH_CHECK_INTERVAL_MS / 60000);

    publish(m_mqttConfig.lwtTopic, "online", 1, true);

    ESP_LOGI(TAG, "Subscribing to command topics...");
    esp_mqtt_client_subscribe(m_client, m_mqttConfig.lockStateCmd.c_str(), 0);
    esp_mqtt_client_subscribe(m_client, m_mqttConfig.lockCStateCmd.c_str(), 0);
    esp_mqtt_client_subscribe(m_client, m_mqttConfig.lockTStateCmd.c_str(), 0);
    esp_mqtt_client_subscribe(m_client, m_mqttConfig.btrLvlCmdTopic.c_str(), 0);
    if (m_mqttConfig.lockEnableCustomState) {
        esp_mqtt_client_subscribe(m_client, m_mqttConfig.lockCustomStateCmd.c_str(), 0);
    }

    if (m_mqttConfig.hassMqttDiscoveryEnabled) {
        publishHassDiscovery();
    }
}

/**
 * @brief Handle an incoming MQTT message and translate it into internal events.
 *
 * Processes messages on configured MQTT command topics to update or override lock
 * state, handle custom lock-state codes, and forward battery level changes as
 * HomeKit events. Matching topic payloads are converted into the appropriate
 * EventManager events.
 *
 * @param topic MQTT topic of the received message.
 * @param data Payload of the received message as a string.
 */
void MqttManager::onData(const std::string& topic, const std::string& data) {
    ESP_LOGI(TAG, "Received message on topic '%s': %s", topic.c_str(), data.c_str());

    if (topic == m_mqttConfig.lockStateCmd) {
      EventLockState s{
        .currentState = static_cast<uint8_t>(std::stoi(data)),
        .targetState = static_cast<uint8_t>(std::stoi(data)),
        .source = LockManager::MQTT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/overrideState", d);
    } else if (topic == m_mqttConfig.lockTStateCmd) {
      EventLockState s{
        .currentState = LockManager::UNKNOWN,
        .targetState = static_cast<uint8_t>(std::stoi(data)),
        .source = LockManager::MQTT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/targetStateChanged", d);
    } else if (topic == m_mqttConfig.lockCStateCmd) {
      EventLockState s{
        .currentState = static_cast<uint8_t>(std::stoi(data)),
        .targetState = LockManager::UNKNOWN,
        .source = LockManager::MQTT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/updateState", d);
    } else if (m_mqttConfig.lockEnableCustomState &&
               topic == m_mqttConfig.lockCustomStateCmd) {
      int state = std::stoi(data);
      if (m_mqttConfig.customLockStates.at("C_UNLOCKING") == state) {
        EventLockState s{
          .currentState = LockManager::UNKNOWN,
          .targetState = LockManager::UNLOCKED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/targetStateChanged", d);
      } else if (m_mqttConfig.customLockStates.at("C_LOCKING") == state) {
        EventLockState s{
          .currentState = LockManager::UNKNOWN,
          .targetState = LockManager::LOCKED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/targetStateChanged", d);
      } else if (m_mqttConfig.customLockStates.at("C_UNLOCKED") == state) {
        EventLockState s{
          .currentState = LockManager::UNLOCKED,
          .targetState = LockManager::UNLOCKED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/overrideState", d);
      } else if (m_mqttConfig.customLockStates.at("C_LOCKED") == state) {
        EventLockState s{
          .currentState = LockManager::LOCKED,
          .targetState = LockManager::LOCKED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/overrideState", d);
      } else if (m_mqttConfig.customLockStates.at("C_JAMMED") == state) {
        EventLockState s{
          .currentState = LockManager::JAMMED,
          .targetState = LockManager::JAMMED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/overrideState", d);
      } else if (m_mqttConfig.customLockStates.at("C_UNKNOWN") == state) {
        EventLockState s{
          .currentState = LockManager::UNKNOWN,
          .targetState = LockManager::UNKNOWN,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/overrideState", d);
      }
    } else if (topic == m_mqttConfig.btrLvlCmdTopic) { 
        EventValueChanged s{
          .name = "btrLevel",
          .oldValue = 0,
          .newValue = static_cast<uint8_t>(std::stoi(data)),
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        HomekitEvent event{.type = HomekitEventType::BTR_PROP_CHANGED, .data = d};
        std::vector<uint8_t> event_data;
        alpaca::serialize(event, event_data);
        espp::EventManager::get().publish("homekit/event", event_data);
    }
}

/**
 * @brief Publish the lock's current/target state to the configured MQTT state topic.
 *
 * Publishes a string representation of the lock state to the MQTT topic configured in m_mqttConfig.lockStateTopic.
 * If the current state differs from the target state, the published value indicates an in-transition state
 * ("locking" or "unlocking"); otherwise the numeric current state is published. The message is sent with QoS 0
 * and retained.
 *
 * @param currentState Numeric code representing the lock's current state.
 * @param targetState Numeric code representing the lock's target state.
 */

void MqttManager::publishLockState(int currentState, int targetState) {
    std::string stateStr;
    if (currentState != targetState) {
        stateStr = (targetState == LockManager::UNLOCKED) ? std::to_string(LockManager::UNLOCKING) : std::to_string(LockManager::LOCKING);
    } else {
        stateStr = std::to_string(currentState);
    }
    publish(m_mqttConfig.lockStateTopic, stateStr, 0, true);
}

/**
 * @brief Publish a Home Key Tap event to the configured Home Key MQTT topic.
 *
 * Constructs a JSON payload containing hex-encoded identifiers and a "homekey" flag, then publishes it to the configured Home Key topic.
 *
 * @param issuerId Byte sequence of the issuer identifier; encoded as an uppercase hex string in the `issuerId` JSON field.
 * @param endpointId Byte sequence of the endpoint identifier; encoded as an uppercase hex string in the `endpointId` JSON field.
 * @param readerId Byte sequence of the reader identifier; encoded as an uppercase hex string in the `readerId` JSON field.
 */
void MqttManager::publishHomeKeyTap(const std::vector<uint8_t>& issuerId, const std::vector<uint8_t>& endpointId, const std::vector<uint8_t>& readerId) {
    cJSON *doc = cJSON_CreateObject();
    cJSON_AddStringToObject(doc, "issuerId", fmt::format("{:02X}", fmt::join(issuerId, "")).c_str());
    cJSON_AddStringToObject(doc, "endpointId", fmt::format("{:02X}", fmt::join(endpointId, "")).c_str());
    cJSON_AddStringToObject(doc, "readerId", fmt::format("{:02X}", fmt::join(readerId, "")).c_str());
    cJSON_AddBoolToObject(doc, "homekey", true);
    char *payload_cstr = cJSON_Print(doc);
    std::string payload(payload_cstr);
    free(payload_cstr);
    publish(m_mqttConfig.hkTopic, payload);
    cJSON_Delete(doc);
}

/**
 * @brief Publish an NFC tag UID tap payload to the configured Home Key topic.
 *
 * When configured to allow NFC tag publishing, builds a JSON payload containing
 * the tag UID, ATQA, and SAK as uppercase hex strings and a `homekey` flag set
 * to `false`, then publishes it to the manager's configured hkTopic.
 *
 * @param uid Byte vector of the tag UID to include in the payload.
 * @param atqa Byte vector of the tag ATQA to include in the payload.
 * @param sak Byte vector of the tag SAK to include in the payload.
 *
 * If NFC tag publishing is disabled in the MQTT configuration, no publish is performed.
 */
void MqttManager::publishUidTap(const std::vector<uint8_t>& uid, const std::vector<uint8_t> &atqa, const std::vector<uint8_t> &sak) {
    if(!m_mqttConfig.nfcTagNoPublish){
      cJSON *doc = cJSON_CreateObject();
      cJSON_AddStringToObject(doc, "uid", fmt::format("{:02X}", fmt::join(uid, "")).c_str());
      cJSON_AddBoolToObject(doc, "homekey", false);
      cJSON_AddStringToObject(doc, "atqa", fmt::format("{:02X}", fmt::join(atqa, "")).c_str());
      cJSON_AddStringToObject(doc, "sak", fmt::format("{:02X}", fmt::join(sak, "")).c_str());
      char *payload_cstr = cJSON_Print(doc);
      std::string payload(payload_cstr);
      free(payload_cstr);
      publish(m_mqttConfig.hkTopic, payload);
      cJSON_Delete(doc);
    } else ESP_LOGW(TAG, "MQTT publishing of Tag UID not enabled, ignoring!");
}

/**
 * @brief Publish Home Assistant MQTT discovery payloads for the device's lock and (optionally) NFC tag.
 *
 * Composes a device descriptor and sends retained, QoS 1 discovery messages to Home Assistant discovery topics
 * so the lock entity and, if enabled, an NFC tag entity are automatically discovered. The lock payload includes
 * state and command topics, payload values for lock states, and the availability topic. The NFC/tag payload
 * includes the topic and a JSON value template for extracting the tag UID.
 */
void MqttManager::publishHassDiscovery() {
    ESP_LOGI(TAG, "Publishing Home Assistant discovery messages...");

    cJSON *device = cJSON_CreateObject();
    cJSON *identifiers = cJSON_CreateArray();
    cJSON_AddItemToArray(identifiers, cJSON_CreateString(deviceID.c_str()));
    cJSON_AddItemToObject(device, "identifiers", identifiers);
    cJSON_AddStringToObject(device, "name", device_name.c_str());
    cJSON_AddStringToObject(device, "manufacturer", "rednblkx");
    cJSON_AddStringToObject(device, "model", "HomeKey-ESP32");
    cJSON_AddStringToObject(device, "sw_version", esp_app_get_description()->version);
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    const std::string macStr = fmt::format("HK-{:02X}{:02X}{:02X}{:02X}", mac[2], mac[3], mac[4], mac[5]);
    cJSON_AddStringToObject(device, "serial_number", macStr.c_str());

    cJSON *lockPayload = cJSON_CreateObject();
    cJSON_AddStringToObject(lockPayload, "name", "Lock");
    cJSON_AddStringToObject(lockPayload, "unique_id", deviceID.c_str());
    cJSON_AddItemToObject(lockPayload, "device", cJSON_Duplicate(device, true));
    cJSON_AddStringToObject(lockPayload, "state_topic", m_mqttConfig.lockStateTopic.c_str());
    cJSON_AddStringToObject(lockPayload, "command_topic", m_mqttConfig.lockTStateCmd.c_str());
    cJSON_AddStringToObject(lockPayload, "payload_lock", std::to_string(LockManager::LOCKED).c_str());
    cJSON_AddStringToObject(lockPayload, "payload_unlock", std::to_string(LockManager::UNLOCKED).c_str());
    cJSON_AddStringToObject(lockPayload, "state_locked", std::to_string(LockManager::LOCKED).c_str());
    cJSON_AddStringToObject(lockPayload, "state_unlocked", std::to_string(LockManager::UNLOCKED).c_str());
    cJSON_AddStringToObject(lockPayload, "state_locking", std::to_string(LockManager::LOCKING).c_str());
    cJSON_AddStringToObject(lockPayload, "state_unlocking", std::to_string(LockManager::UNLOCKING).c_str());
    cJSON_AddStringToObject(lockPayload, "state_jammed", std::to_string(LockManager::JAMMED).c_str());
    cJSON_AddStringToObject(lockPayload, "availability_topic", m_mqttConfig.lwtTopic.c_str());

    char *payload_cstr = cJSON_Print(lockPayload);
    std::string payload(payload_cstr);
    free(payload_cstr);
    std::string lockConfigTopic = "homeassistant/lock/" + m_mqttConfig.mqttClientId + "/lock/config";
    publish(lockConfigTopic, payload, 1, true);
    cJSON_Delete(lockPayload);

    if (!m_mqttConfig.nfcTagNoPublish) {
        cJSON *rfidPayload = cJSON_CreateObject();
        cJSON_AddStringToObject(rfidPayload, "name", "NFC Tag");
        cJSON_AddStringToObject(rfidPayload, "unique_id", deviceID.c_str());
        cJSON_AddItemToObject(rfidPayload, "device", cJSON_Duplicate(device, true));
        cJSON_AddStringToObject(rfidPayload, "topic", m_mqttConfig.hkTopic.c_str());
        cJSON_AddStringToObject(rfidPayload, "value_template", "{{ value_json.uid }}");
        char *payload_cstr = cJSON_Print(rfidPayload);
        std::string payload(payload_cstr);
        free(payload_cstr);
        std::string rfidConfigTopic = "homeassistant/tag/" + m_mqttConfig.mqttClientId + "/rfid/config";
        publish(rfidConfigTopic, payload, 1, true);
        cJSON_Delete(rfidPayload);
    }

    cJSON_Delete(device);

    ESP_LOGI(TAG, "HASS discovery messages published.");
}

// --- SSL/TLS Configuration Methods ---

bool MqttManager::configureSSL(esp_mqtt_client_config_t& mqtt_cfg) {
    ESP_LOGI(TAG, "Configuring SSL/TLS for MQTT connection...");
    ESP_LOGI(TAG, "TLS handshake state: CONFIGURING - Setting up SSL/TLS parameters");

    // Load certificates first
    if (!loadCertificates()) {
        ESP_LOGE(TAG, "Failed to load SSL/TLS certificates");
        ESP_LOGI(TAG, "TLS handshake state: FAILED - Certificate loading failed");
        return false;
    }

    // Configure TLS settings
    mqtt_cfg.broker.verification.use_global_ca_store = false;

    if (!m_mqttConfig.caCert.empty()) {
        mqtt_cfg.broker.verification.certificate = m_mqttConfig.caCert.c_str();
        mqtt_cfg.broker.verification.certificate_len = m_mqttConfig.caCert.length();
        mqtt_cfg.broker.verification.skip_cert_common_name_check = m_mqttConfig.allowInsecure;
        ESP_LOGI(TAG, "Server certificate validation: ENABLED - CA certificate configured");
        ESP_LOGI(TAG, "Certificate validation mode: %s", m_mqttConfig.allowInsecure ? "SKIP_COMMON_NAME" : "FULL_VALIDATION");
        ESP_LOGI(TAG, "TLS handshake state: CERTIFICATES_LOADED - CA certificate ready for validation");
    } else if (m_mqttConfig.allowInsecure) {
        // When allowInsecure is true and no CA cert provided, skip certificate verification entirely
        ESP_LOGW(TAG, "Server certificate validation: DISABLED - No CA certificate provided (allowInsecure=true)");
        ESP_LOGW(TAG, "Security warning: Server certificate will not be validated - connection is vulnerable to MITM attacks");
        ESP_LOGI(TAG, "TLS handshake state: INSECURE_MODE - Certificate validation disabled");
    } else {
        // When allowInsecure is false and no CA cert provided, fail with clear error
        ESP_LOGE(TAG, "Server certificate validation: FAILED - No CA certificate provided and allowInsecure is false");
        ESP_LOGE(TAG, "Cannot establish secure connection without certificate validation");
        ESP_LOGI(TAG, "TLS handshake state: ABORTED - Certificate validation requirements not met");
        return false;
    }

    if (!m_mqttConfig.clientCert.empty() && !m_mqttConfig.clientKey.empty()) {
        mqtt_cfg.credentials.authentication.certificate = m_mqttConfig.clientCert.c_str();
        mqtt_cfg.credentials.authentication.certificate_len = m_mqttConfig.clientCert.length();
        mqtt_cfg.credentials.authentication.key = m_mqttConfig.clientKey.c_str();
        mqtt_cfg.credentials.authentication.key_len = m_mqttConfig.clientKey.length();
        ESP_LOGI(TAG, "Client authentication: ENABLED - Mutual TLS configured");
        ESP_LOGI(TAG, "TLS handshake state: MUTUAL_AUTH_READY - Client certificate and key configured");
    } else {
        ESP_LOGI(TAG, "Client authentication: DISABLED - No client certificate configured");
        ESP_LOGI(TAG, "TLS handshake state: SERVER_AUTH_ONLY - One-way TLS authentication");
    }

    // Set SSL/TLS transport specific settings
    mqtt_cfg.network.disable_auto_reconnect = false;
    mqtt_cfg.session.keepalive = 60;
    mqtt_cfg.session.disable_clean_session = false;

    m_sslConfigured = true;
    ESP_LOGI(TAG, "SSL/TLS configuration completed successfully");
    ESP_LOGI(TAG, "TLS handshake state: READY - Configuration complete, ready for connection attempt");
    return true;
}

bool MqttManager::loadCertificates() {
    ESP_LOGI(TAG, "Loading SSL/TLS certificates...");
    ESP_LOGI(TAG, "Certificate validation state: STARTING - Beginning certificate format validation");

    // Validate certificate formats (basic check for PEM format)
    if (!m_mqttConfig.caCert.empty()) {
        ESP_LOGI(TAG, "Validating CA certificate format...");
        if (m_mqttConfig.caCert.find("-----BEGIN CERTIFICATE-----") == std::string::npos ||
            m_mqttConfig.caCert.find("-----END CERTIFICATE-----") == std::string::npos) {
            ESP_LOGE(TAG, "Certificate validation FAILED: Invalid CA certificate format - missing PEM headers");
            ESP_LOGE(TAG, "Expected format: PEM certificate with '-----BEGIN CERTIFICATE-----' and '-----END CERTIFICATE-----'");
            return false;
        }
        ESP_LOGI(TAG, "Certificate validation PASSED: CA certificate format valid (%zu bytes)", m_mqttConfig.caCert.length());

        // Additional validation - check for basic certificate structure
        size_t certCount = 0;
        size_t pos = 0;
        while ((pos = m_mqttConfig.caCert.find("-----BEGIN CERTIFICATE-----", pos)) != std::string::npos) {
            certCount++;
            pos += 27; // length of "-----BEGIN CERTIFICATE-----"
        }
        ESP_LOGI(TAG, "CA certificate chain contains %zu certificate(s)", certCount);
    } else {
        ESP_LOGI(TAG, "No CA certificate configured - will use insecure mode if allowInsecure=true");
    }

    if (!m_mqttConfig.clientCert.empty()) {
        ESP_LOGI(TAG, "Validating client certificate format...");
        if (m_mqttConfig.clientCert.find("-----BEGIN CERTIFICATE-----") == std::string::npos ||
            m_mqttConfig.clientCert.find("-----END CERTIFICATE-----") == std::string::npos) {
            ESP_LOGE(TAG, "Certificate validation FAILED: Invalid client certificate format - missing PEM headers");
            ESP_LOGE(TAG, "Expected format: PEM certificate with '-----BEGIN CERTIFICATE-----' and '-----END CERTIFICATE-----'");
            return false;
        }
        ESP_LOGI(TAG, "Certificate validation PASSED: Client certificate format valid (%zu bytes)", m_mqttConfig.clientCert.length());
    }

    if (!m_mqttConfig.clientKey.empty()) {
        ESP_LOGI(TAG, "Validating client private key format...");
        if (m_mqttConfig.clientKey.find("-----BEGIN") == std::string::npos ||
            m_mqttConfig.clientKey.find("-----END") == std::string::npos) {
            ESP_LOGE(TAG, "Certificate validation FAILED: Invalid client key format - missing PEM headers");
            ESP_LOGE(TAG, "Expected format: PEM private key with '-----BEGIN [TYPE] PRIVATE KEY-----' and '-----END [TYPE] PRIVATE KEY-----'");
            return false;
        }

        // Check if it's RSA or EC key
        bool isRSA = m_mqttConfig.clientKey.find("-----BEGIN RSA PRIVATE KEY-----") != std::string::npos;
        bool isEC = m_mqttConfig.clientKey.find("-----BEGIN EC PRIVATE KEY-----") != std::string::npos;
        bool isPKCS8 = m_mqttConfig.clientKey.find("-----BEGIN PRIVATE KEY-----") != std::string::npos;

        if (isRSA) {
            ESP_LOGI(TAG, "Certificate validation PASSED: RSA private key format valid (%zu bytes)", m_mqttConfig.clientKey.length());
        } else if (isEC) {
            ESP_LOGI(TAG, "Certificate validation PASSED: EC private key format valid (%zu bytes)", m_mqttConfig.clientKey.length());
        } else if (isPKCS8) {
            ESP_LOGI(TAG, "Certificate validation PASSED: PKCS#8 private key format valid (%zu bytes)", m_mqttConfig.clientKey.length());
        } else {
            ESP_LOGW(TAG, "Private key format detected but type unknown - proceeding anyway (%zu bytes)", m_mqttConfig.clientKey.length());
        }
    }

    if (m_mqttConfig.allowInsecure) {
        ESP_LOGW(TAG, "SECURITY WARNING: SSL/TLS certificate validation is disabled (allowInsecure=true)");
        ESP_LOGW(TAG, "This configuration allows man-in-the-middle attacks and should only be used for testing");
        ESP_LOGI(TAG, "Certificate validation state: DISABLED - allowInsecure flag set");
    } else {
        ESP_LOGI(TAG, "Certificate validation state: ENABLED - Full certificate validation will be performed");
    }

    ESP_LOGI(TAG, "Certificate loading completed successfully");
    ESP_LOGI(TAG, "TLS handshake preparation: CERTIFICATES_VALIDATED - Ready for SSL/TLS connection");
    return true;
}

void MqttManager::logSSLError(const char* operation, esp_err_t error) {
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "SSL/TLS error during %s: %s (0x%x)", operation, esp_err_to_name(error), error);

        // Provide detailed error information and troubleshooting hints for common SSL/TLS errors
        switch (error) {
            case ESP_FAIL:
                ESP_LOGE(TAG, "SSL/TLS operation failed - general failure");
                ESP_LOGE(TAG, "Troubleshooting: Check certificate validity, expiration dates, and chain completeness");
                ESP_LOGE(TAG, "Troubleshooting: Verify server certificate matches hostname (unless allowInsecure=true)");
                ESP_LOGE(TAG, "Troubleshooting: Ensure network connectivity and DNS resolution");
                break;
            case ESP_ERR_INVALID_ARG:
                ESP_LOGE(TAG, "Invalid SSL/TLS configuration argument");
                ESP_LOGE(TAG, "Troubleshooting: Check certificate format (must be PEM), private key format, and configuration parameters");
                break;
            case ESP_ERR_NO_MEM:
                ESP_LOGE(TAG, "Insufficient memory for SSL/TLS operation");
                ESP_LOGE(TAG, "Troubleshooting: Free up system memory or reduce certificate sizes");
                break;
            case ESP_ERR_INVALID_STATE:
                ESP_LOGE(TAG, "Invalid SSL/TLS state for operation");
                ESP_LOGE(TAG, "Troubleshooting: Ensure SSL context is properly initialized before use");
                break;
            case ESP_ERR_INVALID_SIZE:
                ESP_LOGE(TAG, "Invalid size in SSL/TLS operation");
                ESP_LOGE(TAG, "Troubleshooting: Check certificate/key buffer sizes and ensure data is not truncated");
                break;
            default:
                ESP_LOGE(TAG, "SSL/TLS error code: 0x%x", error);
                if (error >= 0x1000 && error < 0x2000) {
                    ESP_LOGE(TAG, "This appears to be a TLS protocol error - check cipher suite compatibility");
                } else if (error >= 0x2000 && error < 0x3000) {
                    ESP_LOGE(TAG, "This appears to be a certificate validation error - verify certificate chain");
                }
                break;
        }

        // Log current SSL configuration state for debugging
        ESP_LOGI(TAG, "SSL Configuration state: useSSL=%s, allowInsecure=%s, hasCACert=%s, hasClientCert=%s, hasClientKey=%s",
                 m_mqttConfig.useSSL ? "true" : "false",
                 m_mqttConfig.allowInsecure ? "true" : "false",
                 !m_mqttConfig.caCert.empty() ? "true" : "false",
                 !m_mqttConfig.clientCert.empty() ? "true" : "false",
                 !m_mqttConfig.clientKey.empty() ? "true" : "false");
    }
}

// --- Reconnection Methods ---

bool MqttManager::reconnectWithNewCertificates() {
    ESP_LOGI(TAG, "Reconnecting MQTT with new certificates...");
    
    if (!m_client) {
        ESP_LOGE(TAG, "Cannot reconnect: MQTT client not initialized");
        return false;
    }
    
    // Stop current connection
    esp_mqtt_client_stop(m_client);
    esp_mqtt_client_destroy(m_client);
    m_client = nullptr;
    m_isConnected = false;
    
    // Re-initialize with new configuration
    return begin(deviceID);
}

bool MqttManager::isConnected() const {
    return m_isConnected;
}

void MqttManager::startReconnectionTimer() {
    if (m_reconnectionTimer || m_isReconnecting) {
        ESP_LOGW(TAG, "Reconnection already in progress");
        return;
    }

    m_reconnectionAttempts++;

    if (m_reconnectionAttempts > m_maxReconnectionAttempts) {
        ESP_LOGE(TAG, "Max reconnection attempts reached (%d), giving up", m_maxReconnectionAttempts);
        m_reconnectionAttempts = 0; // Reset for future attempts
        return;
    }

    // Calculate exponential backoff delay with jitter
    uint32_t baseDelay = INITIAL_RECONNECTION_DELAY_MS * (1 << (m_reconnectionAttempts - 1));
    m_reconnectionDelayMs = std::min(baseDelay, MAX_RECONNECTION_DELAY_MS);
    // Add some jitter to avoid thundering herd
    m_reconnectionDelayMs += (esp_random() % 1000);

    ESP_LOGI(TAG, "Starting reconnection timer: attempt %d/%d, delay %d ms",
             m_reconnectionAttempts, m_maxReconnectionAttempts, m_reconnectionDelayMs);

    esp_timer_create_args_t timer_args = {
        .callback = [](void* arg) {
            MqttManager* instance = static_cast<MqttManager*>(arg);
            instance->handleReconnection();
        },
        .arg = this,
        .name = "mqtt_reconnect"
    };

    esp_timer_create(&timer_args, &m_reconnectionTimer);
    esp_timer_start_once(m_reconnectionTimer, m_reconnectionDelayMs * 1000); // Convert to microseconds
}

void MqttManager::stopReconnectionTimer() {
    if (m_reconnectionTimer) {
        esp_timer_stop(m_reconnectionTimer);
        esp_timer_delete(m_reconnectionTimer);
        m_reconnectionTimer = nullptr;
    }
}

void MqttManager::handleReconnection() {
    ESP_LOGI(TAG, "Handling MQTT reconnection attempt %d/%d", m_reconnectionAttempts, m_maxReconnectionAttempts);

    if (m_isConnected) {
        ESP_LOGI(TAG, "Already connected, cancelling reconnection");
        m_isReconnecting = false;
        stopReconnectionTimer();
        return;
    }

    m_isReconnecting = true;
    bool success = false;

    if (m_mqttConfig.useSSL) {
        // For SSL connections, reload certificates and recreate client
        ESP_LOGI(TAG, "Attempting SSL reconnection with certificate reload");
        success = reconnectWithNewCertificates();
    } else {
        // For non-SSL connections, use simple reconnect
        ESP_LOGI(TAG, "Attempting non-SSL reconnection");
        if (m_client) {
            esp_err_t err = esp_mqtt_client_reconnect(m_client);
            success = (err == ESP_OK);
            if (success) {
                ESP_LOGI(TAG, "Non-SSL reconnection initiated successfully");
            } else {
                ESP_LOGE(TAG, "Non-SSL reconnection failed: %s", esp_err_to_name(err));
            }
        } else {
            ESP_LOGE(TAG, "MQTT client not initialized, cannot reconnect");
        }
    }

    if (success) {
        ESP_LOGI(TAG, "Reconnection attempt %d successful", m_reconnectionAttempts);
        m_reconnectionAttempts = 0; // Reset on successful connection
        m_isReconnecting = false;
        stopReconnectionTimer();
    } else {
        ESP_LOGE(TAG, "Reconnection attempt %d failed", m_reconnectionAttempts);
        m_isReconnecting = false;
        // Timer will be restarted with exponential backoff in the next attempt
        startReconnectionTimer();
    }
}

// --- Health Check Methods ---

void MqttManager::startHealthCheckTimer() {
    if (m_healthCheckTimer) {
        ESP_LOGW(TAG, "Health check timer already running");
        return;
    }

    ESP_LOGI(TAG, "Starting MQTT connection health check timer (%d minutes interval)", HEALTH_CHECK_INTERVAL_MS / 60000);

    esp_timer_create_args_t timer_args = {
        .callback = [](void* arg) {
            MqttManager* instance = static_cast<MqttManager*>(arg);
            instance->performHealthCheck();
        },
        .arg = this,
        .name = "mqtt_health_check"
    };

    esp_timer_create(&timer_args, &m_healthCheckTimer);
    esp_timer_start_periodic(m_healthCheckTimer, HEALTH_CHECK_INTERVAL_MS * 1000); // Convert to microseconds
    m_lastHealthCheckTime = esp_timer_get_time() / 1000; // Store in milliseconds
}

void MqttManager::stopHealthCheckTimer() {
    if (m_healthCheckTimer) {
        ESP_LOGI(TAG, "Stopping MQTT connection health check timer");
        esp_timer_stop(m_healthCheckTimer);
        esp_timer_delete(m_healthCheckTimer);
        m_healthCheckTimer = nullptr;
    }
}

void MqttManager::performHealthCheck() {
    uint32_t currentTime = esp_timer_get_time() / 1000; // Current time in milliseconds
    uint32_t timeSinceLastCheck = currentTime - m_lastHealthCheckTime;

    ESP_LOGI(TAG, "MQTT Connection Health Check");
    ESP_LOGI(TAG, "Connection status: %s", m_isConnected ? "CONNECTED" : "DISCONNECTED");
    ESP_LOGI(TAG, "SSL/TLS enabled: %s", m_mqttConfig.useSSL ? "YES" : "NO");
    ESP_LOGI(TAG, "Broker: %s:%d", m_mqttConfig.mqttBroker.c_str(), m_mqttConfig.mqttPort);
    ESP_LOGI(TAG, "Client ID: %s", m_mqttConfig.mqttClientId.c_str());
    ESP_LOGI(TAG, "Time since last health check: %d seconds", timeSinceLastCheck / 1000);

    if (m_isConnected) {
        ESP_LOGI(TAG, "Health status: GOOD - Connection is active");

        // Test connection by publishing a small message (will be logged if it fails)
        static int healthCheckCounter = 0;
        healthCheckCounter++;
        std::string healthTopic = m_mqttConfig.lwtTopic + "/health";
        publish(healthTopic, std::to_string(healthCheckCounter), 0, false);

        ESP_LOGD(TAG, "Health check ping sent (counter: %d)", healthCheckCounter);
    } else {
        ESP_LOGW(TAG, "Health status: POOR - Connection is down");
        ESP_LOGI(TAG, "Reconnection status: %s", m_isReconnecting ? "IN_PROGRESS" : "IDLE");
        if (m_reconnectionAttempts > 0) {
            ESP_LOGI(TAG, "Reconnection attempts: %d/%d", m_reconnectionAttempts, m_maxReconnectionAttempts);
        }
    }

    // Check for potential issues
    if (m_mqttConfig.useSSL && m_mqttConfig.allowInsecure) {
        ESP_LOGW(TAG, "Security warning: SSL/TLS is enabled but certificate validation is disabled");
    }

    if (!m_isConnected && !m_isReconnecting) {
        ESP_LOGW(TAG, "Connection issue detected: Not connected and not attempting reconnection");
    }

    m_lastHealthCheckTime = currentTime;
    ESP_LOGI(TAG, "Health check completed");
}
