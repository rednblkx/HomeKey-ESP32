#include "MqttManager.hpp"
#include "LockManager.hpp"
#include "ConfigManager.hpp"
#include "logging.h"
#include <sodium/crypto_sign.h>
#include <sodium/crypto_box.h>
#include "HAP.h"

#include <esp_log.h>
#include <nlohmann/json.hpp>
#include <esp_app_desc.h>

const char* MqttManager::TAG = "MqttManager";

MqttManager::MqttManager(LockManager& lockManager, ConfigManager& configManager)
    : m_lockManager(lockManager),
      m_mqttConfig(configManager.getMqttConfig()),
      m_client(nullptr),
      device_name(configManager.getMiscConfig().deviceName)
{}

bool MqttManager::begin() {
    ESP_LOGI(TAG, "Initializing...");

    // Do not start if the broker configuration is invalid or default
    if (m_mqttConfig.mqttBroker.empty() || m_mqttConfig.mqttBroker == "0.0.0.0") {
        ESP_LOGW(TAG, "MQTT broker host is not configured. MQTT client will not start.");
        return false;
    }

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

    // Delegate the action to the LockManager
    if (topic == m_mqttConfig.lockStateCmd) {
        m_lockManager.setTargetState(std::stoi(data), LockManager::Source::MQTT);
    } else if (topic == m_mqttConfig.lockTStateCmd) {
        m_lockManager.setTargetState(std::stoi(data), LockManager::Source::MQTT);
    } else if (topic == m_mqttConfig.lockCStateCmd) {
        m_lockManager.overrideState(std::stoi(data), -1);
    } else if (m_mqttConfig.lockEnableCustomState && topic == m_mqttConfig.lockCustomStateCmd) {
        m_lockManager.processCustomMqttCommand(std::stoi(data));
    }
}

// --- Publishing Methods ---

void MqttManager::publishLockState(int currentState, int targetState) {
    std::string stateStr;
    if (currentState != targetState) {
        stateStr = (targetState == lockStates::UNLOCKED) ? std::to_string(lockStates::UNLOCKING) : std::to_string(lockStates::LOCKING);
    } else {
        stateStr = std::to_string(currentState);
    }
    publish(m_mqttConfig.lockStateTopic, stateStr, 1, true);
}

void MqttManager::publishHomeKeyTap(const std::vector<uint8_t>& issuerId, const std::vector<uint8_t>& endpointId, const std::vector<uint8_t>& readerId) {
    m_lockManager.processNfcRequest(true);
    nlohmann::json payload;
    payload["issuerId"] = red_log::bufToHexString(issuerId.data(), issuerId.size());
    payload["endpointId"] = red_log::bufToHexString(endpointId.data(), endpointId.size());
    payload["readerId"] = red_log::bufToHexString(readerId.data(), readerId.size());
    payload["homekey"] = true;
    publish(m_mqttConfig.hkTopic, payload.dump());
}

void MqttManager::publishUidTap(const uint8_t* uid, uint8_t uidLen, const uint8_t* atqa, const uint8_t* sak) {
    if(!m_mqttConfig.nfcTagNoPublish){
      nlohmann::json payload;
      payload["uid"] = red_log::bufToHexString(uid, uidLen);
      payload["homekey"] = false;
      if (atqa) payload["atqa"] = red_log::bufToHexString(atqa, 2);
      if (sak) payload["sak"] = red_log::bufToHexString(sak, 1);
      publish(m_mqttConfig.hkTopic, payload.dump());
    } else ESP_LOGD(TAG, "Generic Tag UID publishing via MQTT not enabled, ignoring!");
}

void MqttManager::publishHassDiscovery() {
    ESP_LOGI(TAG, "Publishing Home Assistant discovery messages...");

    nlohmann::json device;
    char identifier[18];
    sprintf(identifier, "%.2s%.2s%.2s%.2s%.2s%.2s", HAPClient::accessory.ID, HAPClient::accessory.ID + 3, HAPClient::accessory.ID + 6, HAPClient::accessory.ID + 9, HAPClient::accessory.ID + 12, HAPClient::accessory.ID + 15);
    device["identifiers"] = { std::string(identifier) };
    device["name"] = device_name;
    device["manufacturer"] = "rednblkx";
    device["model"] = "HomeKey-ESP32";
    device["sw_version"] = esp_app_get_description()->version;

    nlohmann::json lockPayload;
    lockPayload["name"] = "Lock";
    lockPayload["unique_id"] = std::string(identifier) + "_lock";
    lockPayload["device"] = device;
    lockPayload["state_topic"] = m_mqttConfig.lockStateTopic;
    lockPayload["command_topic"] = m_mqttConfig.lockStateCmd;
    lockPayload["payload_lock"] = std::to_string(lockStates::LOCKED);
    lockPayload["payload_unlock"] = std::to_string(lockStates::UNLOCKED);
    lockPayload["state_locked"] = std::to_string(lockStates::LOCKED);
    lockPayload["state_unlocked"] = std::to_string(lockStates::UNLOCKED);
    lockPayload["state_locking"] = std::to_string(lockStates::LOCKING);
    lockPayload["state_unlocking"] = std::to_string(lockStates::UNLOCKING);
    lockPayload["state_jammed"] = std::to_string(lockStates::JAMMED);
    lockPayload["availability_topic"] = m_mqttConfig.lwtTopic;
    
    std::string lockConfigTopic = "homeassistant/lock/" + m_mqttConfig.mqttClientId + "/lock/config";
    publish(lockConfigTopic, lockPayload.dump(), 1, true);

    if (!m_mqttConfig.nfcTagNoPublish) {
        nlohmann::json rfidPayload;
        rfidPayload["name"] = "NFC Tag";
        rfidPayload["unique_id"] = std::string(identifier) + "_nfc_tag";
        rfidPayload["device"] = device;
        rfidPayload["topic"] = m_mqttConfig.hkTopic;
        rfidPayload["value_template"] = "{{ value_json.uid }}";
        std::string rfidConfigTopic = "homeassistant/tag/" + m_mqttConfig.mqttClientId + "/rfid/config";
        publish(rfidConfigTopic, rfidPayload.dump(), 1, true);
    }
    
    ESP_LOGI(TAG, "HASS discovery messages published.");
}
