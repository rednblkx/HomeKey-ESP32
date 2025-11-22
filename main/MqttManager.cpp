#include "config.hpp"
#include "event_manager.hpp"
#include "MqttManager.hpp"
#include "LockManager.hpp"
#include "ConfigManager.hpp"
#include <cstdint>
#include <cstdlib>
#include <esp_log.h>
#include <esp_app_desc.h>
#include "eventStructs.hpp"
#include "fmt/format.h"
#include <cJSON.h>
#include <sys/_types.h>
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
      m_sslConfigured(false)
{
  m_mqttSslConfig = configManager.getMqttSslConfig();
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
    if (m_mqttConfig.mqttBroker.empty() || m_mqttConfig.mqttBroker == "0.0.0.0") {
        ESP_LOGW(TAG, "MQTT broker host is not configured. MQTT client will not start.");
        return false;
    }

    this->deviceID = deviceID;

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.hostname = m_mqttConfig.mqttBroker.c_str();
    mqtt_cfg.broker.address.port = m_mqttConfig.mqttPort;
    
    if (m_mqttConfig.useSSL) {
        ESP_LOGI(TAG, "SSL/TLS is enabled for MQTT connection");
        mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_SSL;
        
        if (m_mqttConfig.useSSL && m_mqttConfig.allowInsecure) {
            ESP_LOGW(TAG, "Security warning: SSL/TLS is enabled but certificate validation is disabled");
        }
        if(!m_mqttSslConfig){
          ESP_LOGE(TAG, "SSL config ptr is null");
          return false;
        }
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

    ESP_LOGD(TAG, "MQTT Event received: %d", event->event_id);

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED: Connection established successfully");
            onConnected();
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED: Client disconnected from broker");
            m_isConnected = false;
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
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR: Connection or protocol error occurred");
            if (event->error_handle) {
                ESP_LOGE(TAG, "MQTT Error Type: %d", event->error_handle->error_type);

                // Categorize and handle different error types with detailed logging
                switch (event->error_handle->error_type) {
                    case MQTT_ERROR_TYPE_TCP_TRANSPORT:
                        if (event->error_handle->esp_transport_sock_errno != 0) {
                            ESP_LOGE(TAG, "Transport socket error: errno=%d (%s)",
                                     event->error_handle->esp_transport_sock_errno,
                                     strerror(event->error_handle->esp_transport_sock_errno));
                        }
                        break;
                    case MQTT_ERROR_TYPE_CONNECTION_REFUSED:
                        ESP_LOGE(TAG, "Connection refused - broker may be down or rejecting connection");
                        break;
                    case MQTT_ERROR_TYPE_SUBSCRIBE_FAILED:
                        ESP_LOGE(TAG, "Subscribe failed - check topic permissions");
                        break;
                    default:
                        ESP_LOGE(TAG, "Unknown MQTT error type: %d", event->error_handle->error_type);
                        break;
                }

                if (event->error_handle->esp_tls_last_esp_err != 0) {
                    logSSLError("MQTT SSL/TLS connection", event->error_handle->esp_tls_last_esp_err);
                }

                if (event->error_handle->esp_tls_stack_err != 0) {
                    ESP_LOGE(TAG, "TLS stack error: 0x%x", event->error_handle->esp_tls_stack_err);
                }

                ESP_LOGI(TAG, "Connection attempt details: broker=%s:%d, client_id=%s, ssl=%s",
                         m_mqttConfig.mqttBroker.c_str(), m_mqttConfig.mqttPort,
                         m_mqttConfig.mqttClientId.c_str(), m_mqttConfig.useSSL ? "enabled" : "disabled");
            } else {
                ESP_LOGE(TAG, "MQTT error occurred but no error handle available");
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
    m_isConnected = true;

    publish(m_mqttConfig.lwtTopic, "online", 1, true);

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
    
    auto to_u8 = [](const std::string &str, uint8_t& out) -> bool {
      const char* begin = str.c_str(); char* end = nullptr;
      unsigned long v = strtoul(begin, &end, 10);
      if(end == begin || v < 0 || v > 255) return false;
      out = static_cast<uint8_t>(v); return true;
    };

    if (topic == m_mqttConfig.lockStateCmd) {
      uint8_t v; if (!to_u8(data, v)) { ESP_LOGW(TAG, "Invalid lockStateCmd payload: %s", data.c_str()); return; }
      EventLockState s{
        .currentState = v,
        .targetState = v,
        .source = LockManager::MQTT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/overrideState", d);
    } else if (topic == m_mqttConfig.lockTStateCmd) {
      uint8_t v; if (!to_u8(data, v)) { ESP_LOGW(TAG, "Invalid lockTStateCmd payload: %s", data.c_str()); return; }
      EventLockState s{
        .currentState = LockManager::UNKNOWN,
        .targetState = v,
        .source = LockManager::MQTT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/targetStateChanged", d);
    } else if (topic == m_mqttConfig.lockCStateCmd) {
      uint8_t v; if (!to_u8(data, v)) { ESP_LOGW(TAG, "Invalid lockCStateCmd payload: %s", data.c_str()); return; }
      EventLockState s{
        .currentState = v,
        .targetState = LockManager::UNKNOWN,
        .source = LockManager::MQTT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/updateState", d);
    } else if (m_mqttConfig.lockEnableCustomState &&
               topic == m_mqttConfig.lockCustomStateCmd) {
      uint8_t v; if (!to_u8(data, v)) { ESP_LOGW(TAG, "Invalid lockCStateCmd payload: %s", data.c_str()); return; }
      if (m_mqttConfig.customLockStates.at("C_UNLOCKING") == v) {
        EventLockState s{
          .currentState = LockManager::UNKNOWN,
          .targetState = LockManager::UNLOCKED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/targetStateChanged", d);
      } else if (m_mqttConfig.customLockStates.at("C_LOCKING") == v) {
        EventLockState s{
          .currentState = LockManager::UNKNOWN,
          .targetState = LockManager::LOCKED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/targetStateChanged", d);
      } else if (m_mqttConfig.customLockStates.at("C_UNLOCKED") == v) {
        EventLockState s{
          .currentState = LockManager::UNLOCKED,
          .targetState = LockManager::UNLOCKED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/overrideState", d);
      } else if (m_mqttConfig.customLockStates.at("C_LOCKED") == v) {
        EventLockState s{
          .currentState = LockManager::LOCKED,
          .targetState = LockManager::LOCKED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/overrideState", d);
      } else if (m_mqttConfig.customLockStates.at("C_JAMMED") == v) {
        EventLockState s{
          .currentState = LockManager::JAMMED,
          .targetState = LockManager::JAMMED,
          .source = LockManager::MQTT
        };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish("lock/overrideState", d);
      } else if (m_mqttConfig.customLockStates.at("C_UNKNOWN") == v) {
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
        uint8_t v; if (!to_u8(data, v)) { ESP_LOGW(TAG, "Invalid btrLvlCmdTopic payload: %s", data.c_str()); return; }
        EventValueChanged s{
          .name = "btrLevel",
          .oldValue = 0,
          .newValue = v,
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
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    std::string macStr = fmt::format("HK-{:02X}{:02X}{:02X}{:02X}", mac[2], mac[3], mac[4], mac[5]);
    cJSON_AddItemToArray(identifiers, cJSON_CreateString(deviceID.c_str()));
    cJSON_AddItemToArray(identifiers, cJSON_CreateString(macStr.c_str()));
    cJSON_AddItemToObject(device, "identifiers", identifiers);
    cJSON_AddStringToObject(device, "name", device_name.c_str());
    cJSON_AddStringToObject(device, "manufacturer", "rednblkx");
    cJSON_AddStringToObject(device, "model", "HomeKey-ESP32");
    cJSON_AddStringToObject(device, "sw_version", esp_app_get_description()->version);
    cJSON_AddStringToObject(device, "configuration_url", fmt::format("http://{}.local", macStr).c_str());
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

/**
 * @brief Configure SSL/TLS settings for MQTT connection
 *
 * This method sets up SSL/TLS parameters for the MQTT client using pre-validated certificates
 * from ConfigManager. Certificate validation and loading is now centralized in ConfigManager.
 *
 * @param mqtt_cfg Reference to MQTT client configuration structure to be updated
 * @return true if SSL configuration was successful, false otherwise
 */
bool MqttManager::configureSSL(esp_mqtt_client_config_t& mqtt_cfg) {
    mqtt_cfg.broker.verification.use_global_ca_store = false;
    if (!m_mqttSslConfig->caCert.empty()) {
        mqtt_cfg.broker.verification.certificate = m_mqttSslConfig->caCert.c_str();
        mqtt_cfg.broker.verification.skip_cert_common_name_check = m_mqttConfig.allowInsecure;
        ESP_LOGI(TAG, "Certificate validation mode: %s", m_mqttConfig.allowInsecure ? "SKIP_COMMON_NAME" : "FULL_VALIDATION");
    } else if (m_mqttConfig.allowInsecure) {
        ESP_LOGW(TAG, "Server certificate validation: DISABLED - No CA certificate provided (allowInsecure=true)");
    } else {
        ESP_LOGE(TAG, "Server certificate validation: FAILED - No CA certificate provided and allowInsecure is false");
        return false;
    }

    if (!m_mqttSslConfig->clientCert.empty() && !m_mqttSslConfig->clientKey.empty()) {
        mqtt_cfg.credentials.authentication.certificate = m_mqttSslConfig->clientCert.c_str();
        mqtt_cfg.credentials.authentication.key = m_mqttSslConfig->clientKey.c_str();
        ESP_LOGI(TAG, "TLS authentication configured");
    } else {
        ESP_LOGI(TAG, "No client certificate and/or private key configured");
        return false;
    }

    m_sslConfigured = true;
    return true;
}


void MqttManager::logSSLError(const char* operation, esp_err_t error) {
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "SSL/TLS error during %s: %s (0x%x)", operation, esp_err_to_name(error), error);

        // Provide detailed error information and troubleshooting hints for common SSL/TLS errors
        switch (error) {
            case ESP_FAIL:
                ESP_LOGE(TAG, "SSL/TLS operation failed - general failure");
                break;
            case ESP_ERR_INVALID_ARG:
                ESP_LOGE(TAG, "Invalid SSL/TLS configuration argument");
                break;
            case ESP_ERR_NO_MEM:
                ESP_LOGE(TAG, "Insufficient memory for SSL/TLS operation");
                break;
            case ESP_ERR_INVALID_STATE:
                ESP_LOGE(TAG, "Invalid SSL/TLS state for operation");
                break;
            case ESP_ERR_INVALID_SIZE:
                ESP_LOGE(TAG, "Invalid size in SSL/TLS operation");
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
                 !m_mqttSslConfig->caCert.empty() ? "true" : "false",
                 !m_mqttSslConfig->clientCert.empty() ? "true" : "false",
                 !m_mqttSslConfig->clientKey.empty() ? "true" : "false");
    }
}

bool MqttManager::isConnected() const {
    return m_isConnected;
}
