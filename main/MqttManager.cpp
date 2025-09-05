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

const char* MqttManager::TAG = "MqttManager";

MqttManager::MqttManager(ConfigManager& configManager)
    : m_mqttConfig(configManager.getConfig<espConfig::mqttConfig_t>()),
      m_client(nullptr),
      device_name(configManager.getConfig<espConfig::misc_config_t>().deviceName)
{
  espp::EventManager::get().add_subscriber(
      "lock/stateChanged", "MqttManager",
      [&](const std::vector<uint8_t> &data) {
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(data, ec);
        if(!ec) {
          publishLockState(s.currentState, s.targetState);
        }
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
    mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
    mqtt_cfg.credentials.client_id = m_mqttConfig.mqttClientId.c_str();
    mqtt_cfg.credentials.username = m_mqttConfig.mqttUsername.c_str();
    mqtt_cfg.credentials.authentication.password = m_mqttConfig.mqttPassword.c_str();
    mqtt_cfg.session.last_will.topic = m_mqttConfig.lwtTopic.c_str();
    mqtt_cfg.session.last_will.msg = "offline";
    mqtt_cfg.session.last_will.msg_len = 7;
    mqtt_cfg.session.last_will.retain = true;
    mqtt_cfg.session.last_will.qos = 1;

    m_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_ANY, mqttEventHandler, this);
    esp_mqtt_client_start(m_client);

    ESP_LOGI(TAG, "MQTT client started.");
    return true;
}

void MqttManager::publish(const std::string& topic, const std::string& payload, int qos, bool retain) {
    if (!m_client) {
        ESP_LOGW(TAG, "Cannot publish, MQTT client not initialized.");
        return;
    }
    esp_mqtt_client_publish(m_client, topic.c_str(), payload.c_str(), payload.length(), qos, retain);
}

// --- Event Handling Logic ---

void MqttManager::mqttEventHandler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    MqttManager* instance = static_cast<MqttManager*>(handler_args);
    instance->onMqttEvent(base, event_id, event_data);
}

void MqttManager::onMqttEvent(esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            onConnected();
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT client disconnected.");
            break;
        case MQTT_EVENT_DATA: {
            std::string topic(event->topic, event->topic_len);
            std::string data(event->data, event->data_len);
            onData(topic, data);
            break;
        }
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Event Error");
            break;
        default:
            ESP_LOGD(TAG, "Unhandled MQTT event: %d", event->event_id);
            break;
    }
}

void MqttManager::onConnected() {
    ESP_LOGI(TAG, "MQTT client connected.");
    
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

// --- Publishing Methods ---

void MqttManager::publishLockState(int currentState, int targetState) {
    std::string stateStr;
    if (currentState != targetState) {
        stateStr = (targetState == LockManager::UNLOCKED) ? std::to_string(LockManager::UNLOCKING) : std::to_string(LockManager::LOCKING);
    } else {
        stateStr = std::to_string(currentState);
    }
    publish(m_mqttConfig.lockStateTopic, stateStr, 0, true);
}

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
