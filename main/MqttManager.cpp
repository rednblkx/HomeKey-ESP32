#include "event_manager.hpp"
#include "MqttManager.hpp"
#include "LockManager.hpp"
#include "ConfigManager.hpp"
#include <format.hpp>
#include <esp_log.h>
#include <esp_app_desc.h>
#include "eventStructs.hpp"
#include <ArduinoJson.hpp>
#include <vector>

const char* MqttManager::TAG = "MqttManager";

MqttManager::MqttManager(ConfigManager& configManager)
    : m_mqttConfig(configManager.getMqttConfig()),
      m_client(nullptr),
      device_name(configManager.getMiscConfig().deviceName)
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
  espp::EventManager::get().add_subscriber("nfc/TagTap", "MqttManager", [&](const std::vector<uint8_t> &data){
    std::error_code ec;
    EventTagTap s = alpaca::deserialize<EventTagTap>(data, ec);
    if(!ec){
      publishUidTap(s.uid, s.atqa, s.sak);
    }
  });
  espp::EventManager::get().add_subscriber("nfc/HomeKeyTap", "MqttManager", [&](const std::vector<uint8_t> &data){
    std::error_code ec;
    EventHKTap s = alpaca::deserialize<EventHKTap>(data, ec);
    if(!ec && s.status){
      publishHomeKeyTap(s.issuerId, s.endpointId, s.readerId);
    }
  }, 3072);
  espp::EventManager::get().add_subscriber("mqtt/uidPublishChanged", "MqttManager", [&](const std::vector<uint8_t> &data){
    std::error_code ec;
    EventBinaryStatus s = alpaca::deserialize<EventBinaryStatus>(data, ec);
    if(!ec){
      if(s.status){
        std::string rfidConfigTopic = "homeassistant/tag/" + m_mqttConfig.mqttClientId + "/rfid/config";
        publish(rfidConfigTopic, "");
      } else {
        ArduinoJson::JsonDocument device;
        device["identifiers"].to<ArduinoJson::JsonArray>().add(deviceID);
        device["name"] = device_name;
        device["manufacturer"] = "rednblkx";
        device["model"] = "HomeKey-ESP32";
        device["sw_version"] = esp_app_get_description()->version;

        ArduinoJson::JsonDocument rfidPayload;
        rfidPayload["name"] = "NFC Tag";
        rfidPayload["unique_id"] = deviceID;
        rfidPayload["device"] = device;
        rfidPayload["topic"] = m_mqttConfig.hkTopic;
        rfidPayload["value_template"] = "{{ value_json.uid }}";
        std::string rfidConfigTopic = "homeassistant/tag/" + m_mqttConfig.mqttClientId + "/rfid/config";
        std::string payload;
        serializeJson(rfidPayload, payload);
        publish(rfidConfigTopic, payload, 1, true);
      }
    }
  }, 4096);
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
        .currentState = static_cast<uint8_t>(std::stoi(data)),
        .targetState = static_cast<uint8_t>(std::stoi(data)),
        .source = LockManager::MQTT
      };
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("lock/targetStateChanged", d);
    } else if (topic == m_mqttConfig.lockCStateCmd) {
      EventLockState s{
        .currentState = LockManager::UNKNOWN,
        .targetState = static_cast<uint8_t>(std::stoi(data)),
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
    ArduinoJson::JsonDocument doc;
    doc["issuerId"] = fmt::format("{:02X}", fmt::join(issuerId, "")).c_str();
    doc["endpointId"] = fmt::format("{:02X}", fmt::join(endpointId, "")).c_str();
    doc["readerId"] = fmt::format("{:02X}", fmt::join(readerId, "")).c_str();
    doc["homekey"] = true;
    std::string payload;
    serializeJson(doc, payload);
    publish(m_mqttConfig.hkTopic, payload);
}

void MqttManager::publishUidTap(const std::vector<uint8_t>& uid, const std::vector<uint8_t> &atqa, const std::vector<uint8_t> &sak) {
    if(!m_mqttConfig.nfcTagNoPublish){
      ArduinoJson::JsonDocument doc;
      doc["uid"] = fmt::format("{:02X}", fmt::join(uid, ""));
      doc["homekey"] = false;
      doc["atqa"] = fmt::format("{:02X}", fmt::join(atqa, ""));
      doc["sak"] = fmt::format("{:02X}", fmt::join(sak, ""));
      std::string payload;
      serializeJson(doc, payload);
      publish(m_mqttConfig.hkTopic, payload);
    } else ESP_LOGW(TAG, "MQTT publishing of Tag UID not enabled, ignoring!");
}

void MqttManager::publishHassDiscovery() {
    ESP_LOGI(TAG, "Publishing Home Assistant discovery messages...");

    ArduinoJson::JsonDocument device;
    device["identifiers"].to<ArduinoJson::JsonArray>().add(deviceID);
    device["name"] = device_name;
    device["manufacturer"] = "rednblkx";
    device["model"] = "HomeKey-ESP32";
    device["sw_version"] = esp_app_get_description()->version;

    ArduinoJson::JsonDocument lockPayload;
    lockPayload["name"] = "Lock";
    lockPayload["unique_id"] = deviceID;
    lockPayload["device"] = device;
    lockPayload["state_topic"] = m_mqttConfig.lockStateTopic;
    lockPayload["command_topic"] = m_mqttConfig.lockStateCmd;
    lockPayload["payload_lock"] = std::to_string(LockManager::LOCKED);
    lockPayload["payload_unlock"] = std::to_string(LockManager::UNLOCKED);
    lockPayload["state_locked"] = std::to_string(LockManager::LOCKED);
    lockPayload["state_unlocked"] = std::to_string(LockManager::UNLOCKED);
    lockPayload["state_locking"] = std::to_string(LockManager::LOCKING);
    lockPayload["state_unlocking"] = std::to_string(LockManager::UNLOCKING);
    lockPayload["state_jammed"] = std::to_string(LockManager::JAMMED);
    lockPayload["availability_topic"] = m_mqttConfig.lwtTopic;

    std::string payload;
    serializeJson(lockPayload, payload);
    std::string lockConfigTopic = "homeassistant/lock/" + m_mqttConfig.mqttClientId + "/lock/config";
    publish(lockConfigTopic, payload, 1, true);

    if (!m_mqttConfig.nfcTagNoPublish) {
        ArduinoJson::JsonDocument rfidPayload;
        rfidPayload["name"] = "NFC Tag";
        rfidPayload["unique_id"] = deviceID;
        rfidPayload["device"] = device;
        rfidPayload["topic"] = m_mqttConfig.hkTopic;
        rfidPayload["value_template"] = "{{ value_json.uid }}";
        std::string rfidConfigTopic = "homeassistant/tag/" + m_mqttConfig.mqttClientId + "/rfid/config";
        std::string payload;
        serializeJson(rfidPayload, payload);
        publish(rfidConfigTopic, payload, 1, true);
    }

    ESP_LOGI(TAG, "HASS discovery messages published.");
}
