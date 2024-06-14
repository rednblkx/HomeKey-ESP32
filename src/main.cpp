#include <hkAuthContext.h>
#include <HomeKey.h>
#include <utils.h>
#include <HomeSpan.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <HAP.h>
#include <chrono>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <HK_HomeKit.h>
#include "config.h"
#include <mqtt_client.h>
#include <esp_ota_ops.h>
#include <esp_task.h>

const char* TAG = "MAIN";

enum lockStates
{
  UNLOCKED,
  LOCKED,
  JAMMED,
  UNKNOWN,
  UNLOCKING,
  LOCKING
};
AsyncWebServer webServer(80);
PN532_SPI pn532spi;
PN532 nfc(pn532spi);
QueueHandle_t gpio_led_handle = nullptr;

nvs_handle savedData;
readerData_t readerData;
uint8_t ecpData[18] = { 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 };
namespace espConfig
{
  struct mqttConfig_t
  {
    /* MQTT Broker */
    std::string mqttBroker = MQTT_HOST;
    uint16_t mqttPort = MQTT_PORT;
    std::string mqttUsername = MQTT_USERNAME;
    std::string mqttPassword = MQTT_PASSWORD;
    std::string mqttClientId = MQTT_CLIENTID;
    /* MQTT Topics */
    std::string hkTopic = MQTT_AUTH_TOPIC;
    std::string lockStateTopic = MQTT_STATE_TOPIC;
    std::string lockStateCmd = MQTT_SET_STATE_TOPIC;
    std::string lockCStateCmd = MQTT_SET_CURRENT_STATE_TOPIC;
    std::string lockTStateCmd = MQTT_SET_TARGET_STATE_TOPIC;
    /* MQTT Custom State */
    std::string lockCustomStateTopic = MQTT_CUSTOM_STATE_TOPIC;
    std::string lockCustomStateCmd = MQTT_CUSTOM_STATE_CTRL_TOPIC;
    /* Flags */
    bool lockEnableCustomState = MQTT_CUSTOM_STATE_ENABLED;
    bool hassMqttDiscoveryEnabled = MQTT_DISCOVERY;
  } mqttData;

  struct misc_config_t
  {
    std::string deviceName = DEVICE_NAME;
    bool lockAlwaysUnlock = HOMEKEY_ALWAYS_UNLOCK;
    bool lockAlwaysLock = HOMEKEY_ALWAYS_LOCK;
    uint8_t controlPin = HS_PIN;
    uint8_t hsStatusPin = HS_STATUS_LED;
    uint8_t nfcSuccessPin = NFC_SUCCESS_PIN;
    uint16_t nfcSuccessTime = NFC_SUCCESS_TIME;
    bool nfcSuccessHL = NFC_SUCCESS_HL;
    uint8_t nfcFailPin = NFC_FAIL_PIN;
    uint16_t nfcFailTime = NFC_FAIL_TIME;
    bool nfcFailHL = NFC_FAIL_HL;
    bool gpioActionEnable = GPIO_ACTION_ENABLE;
    uint8_t gpioActionPin = GPIO_ACTION_PIN;
    bool gpioActionLockState = GPIO_ACTION_LOCK_STATE;
    bool gpioActionUnlockState = GPIO_ACTION_UNLOCK_STATE;
  } miscConfig;
}
JSONCONS_ALL_MEMBER_TRAITS(espConfig::mqttConfig_t, mqttBroker, mqttPort, mqttUsername, mqttPassword, mqttClientId, hkTopic, lockStateTopic, lockStateCmd, lockCStateCmd, lockTStateCmd, lockCustomStateTopic, lockCustomStateCmd, lockEnableCustomState, hassMqttDiscoveryEnabled)
JSONCONS_ALL_MEMBER_TRAITS(espConfig::misc_config_t, deviceName, lockAlwaysUnlock, lockAlwaysLock, controlPin, hsStatusPin, nfcSuccessPin, nfcSuccessHL, nfcFailPin, nfcFailHL, gpioActionEnable, gpioActionPin, gpioActionLockState, gpioActionUnlockState)

KeyFlow hkFlow = KeyFlow::kFlowFAST;
SpanCharacteristic* lockCurrentState;
SpanCharacteristic* lockTargetState;
esp_mqtt_client_handle_t client = nullptr;

bool save_to_nvs() {
  std::vector<uint8_t> cborBuf;
  jsoncons::msgpack::encode_msgpack(readerData, cborBuf);
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", cborBuf.data(), cborBuf.size());
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG(D, "NVS SET STATUS: %s", esp_err_to_name(set_nvs));
  LOG(D, "NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
  return !set_nvs && !commit_nvs;
}

struct LockManagement : Service::LockManagement
{
  SpanCharacteristic* lockControlPoint;
  SpanCharacteristic* version;
  const char* TAG = "LockManagement";

  LockManagement() : Service::LockManagement() {

    LOG(D, "Configuring LockManagement"); // initialization message

    lockControlPoint = new Characteristic::LockControlPoint();
    version = new Characteristic::Version();

  } // end constructor

}; // end LockManagement

// Function to calculate CRC16
void crc16a(unsigned char* data, unsigned int size, unsigned char* result) {
  unsigned short w_crc = 0x6363;

  for (unsigned int i = 0; i < size; ++i) {
    unsigned char byte = data[i];
    byte = (byte ^ (w_crc & 0x00FF));
    byte = ((byte ^ (byte << 4)) & 0xFF);
    w_crc = ((w_crc >> 8) ^ (byte << 8) ^ (byte << 3) ^ (byte >> 4)) & 0xFFFF;
  }

  result[0] = static_cast<unsigned char>(w_crc & 0xFF);
  result[1] = static_cast<unsigned char>((w_crc >> 8) & 0xFF);
}

// Function to append CRC16 to data
void with_crc16(unsigned char* data, unsigned int size, unsigned char* result) {
  crc16a(data, size, result);
}

struct LockMechanism : Service::LockMechanism
{
  const char* TAG = "LockMechanism";

  LockMechanism() : Service::LockMechanism() {
    LOG(I, "Configuring LockMechanism"); // initialization message
    lockCurrentState = new Characteristic::LockCurrentState(1, true);
    lockTargetState = new Characteristic::LockTargetState(1, true);
    memcpy(ecpData + 8, readerData.reader_gid.data(), readerData.reader_gid.size());
    with_crc16(ecpData, 16, ecpData + 16);
    if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
      if (lockCurrentState->getVal() == lockStates::LOCKED) {
        digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
      }
      else if (lockCurrentState->getVal() == lockStates::UNLOCKED) {
        digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
      }
    }
  } // end constructor

  boolean update() {
    int targetState = lockTargetState->getNewVal();
    LOG(I, "New LockState=%d, Current LockState=%d", targetState, lockCurrentState->getVal());
    if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
      switch (targetState)
      {
        case lockStates::UNLOCKED:
          digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
          lockCurrentState->setVal(lockStates::UNLOCKED);
          break;

        case lockStates::LOCKED:
          digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
          lockCurrentState->setVal(lockStates::LOCKED);
          break;

        default:
          break;
      }
    }
    int currentState = lockCurrentState->getNewVal();
    if (client != nullptr) {
      if (targetState != currentState) {
        esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), targetState == lockStates::UNLOCKED ? std::to_string(lockStates::UNLOCKING).c_str() : std::to_string(lockStates::LOCKING).c_str(), 1, 1, true);
      }
      else {
        esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(currentState).c_str(), 1, 1, true);
      }
      if (espConfig::mqttData.lockEnableCustomState) {
        if (targetState == lockStates::UNLOCKED) {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(customLockActions::UNLOCK).c_str(), 0, 0, false);
        }
        else if (targetState == lockStates::LOCKED) {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(customLockActions::LOCK).c_str(), 0, 0, false);
        }
      }
    }
    else LOG(W, "MQTT Client not initialized, cannot publish message");

    return (true);
  }
};

struct NFCAccess : Service::NFCAccess
{
  SpanCharacteristic* configurationState;
  SpanCharacteristic* nfcControlPoint;
  SpanCharacteristic* nfcSupportedConfiguration;
  const char* TAG = "NFCAccess";

  NFCAccess() : Service::NFCAccess() {
    LOG(I, "Configuring NFCAccess"); // initialization message
    configurationState = new Characteristic::ConfigurationState();
    nfcControlPoint = new Characteristic::NFCAccessControlPoint();
    nfcSupportedConfiguration = new Characteristic::NFCAccessSupportedConfiguration();
  }

  boolean update() {
    LOG(D, "PROVISIONED READER KEY: %s", utils::bufToHexString(readerData.reader_pk.data(), readerData.reader_pk.size()).c_str());
    LOG(D, "READER GROUP IDENTIFIER: %s", utils::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size()).c_str());
    LOG(D, "READER UNIQUE IDENTIFIER: %s", utils::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size()).c_str());

    // char* dataConfState = configurationState->getNewString(); // Underlying functionality currently unknown
    char* dataNfcControlPoint = nfcControlPoint->getNewString();
    LOG(D, "NfcControlPoint Length: %d", strlen(dataNfcControlPoint));
    std::vector<uint8_t> decB64 = utils::decodeB64(dataNfcControlPoint);
    if (decB64.size() == 0)
      return false;
    LOG(D, "Decoded data: %s", utils::bufToHexString(decB64.data(), decB64.size()).c_str());
    LOG(D, "Decoded data length: %d", decB64.size());
    HK_HomeKit hkCtx(readerData, savedData, "READERDATA", decB64);
    std::vector<uint8_t> result = hkCtx.processResult();
    if (readerData.reader_gid.size() > 0) {
      memcpy(ecpData + 8, readerData.reader_gid.data(), readerData.reader_gid.size());
      with_crc16(ecpData, 16, ecpData + 16);
    }
    TLV8 res(NULL, 0);
    res.unpack(result.data(), result.size());
    nfcControlPoint->setTLV(res, false);
    return true;
  }

};

void deleteReaderData(const char* buf) {
  esp_err_t erase_nvs = nvs_erase_key(savedData, "READERDATA");
  esp_err_t commit_nvs = nvs_commit(savedData);
  readerData.issuers.clear();
  readerData.reader_gid.clear();
  readerData.reader_id.clear();
  readerData.reader_pk.clear();
  readerData.reader_pk_x.clear();
  readerData.reader_sk.clear();
  LOG(D, "*** NVS W STATUS");
  LOG(D, "ERASE: %s", esp_err_to_name(erase_nvs));
  LOG(D, "COMMIT: %s", esp_err_to_name(commit_nvs));
  LOG(D, "*** NVS W STATUS");
}

void pairCallback() {
  if (HAPClient::nAdminControllers() == 0) {
    deleteReaderData(NULL);
    return;
  }
  for (auto it = homeSpan.controllerListBegin(); it != homeSpan.controllerListEnd(); ++it) {
    std::vector<uint8_t> id = utils::getHashIdentifier(it->getLTPK(), 32, true);
    LOG(D, "Found allocated controller - Hash: %s", utils::bufToHexString(id.data(), 8).c_str());
    hkIssuer_t* foundIssuer = nullptr;
    for (auto&& issuer : readerData.issuers) {
      if (std::equal(issuer.issuer_id.begin(), issuer.issuer_id.end(), id.begin())) {
        LOG(D, "Issuer %s already added, skipping", utils::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str());
        foundIssuer = &issuer;
        break;
      }
    }
    if (foundIssuer == nullptr) {
      LOG(D, "Adding new issuer - ID: %s", utils::bufToHexString(id.data(), 8).c_str());
      hkIssuer_t newIssuer;
      newIssuer.issuer_id = std::vector<uint8_t>{id.begin(), id.begin() + 8};
      newIssuer.issuer_pk.insert(newIssuer.issuer_pk.begin(), it->getLTPK(), it->getLTPK() + 32);
      readerData.issuers.emplace_back(newIssuer);
    }
  }
  save_to_nvs();
}

void setFlow(const char* buf) {
  switch (buf[1]) {
  case '0':
    hkFlow = KeyFlow::kFlowFAST;
    Serial.println("FAST Flow");
    break;

  case '1':
    hkFlow = KeyFlow::kFlowSTANDARD;
    Serial.println("STANDARD Flow");
    break;
  case '2':
    hkFlow = KeyFlow::kFlowATTESTATION;
    Serial.println("ATTESTATION Flow");
    break;

  default:
    Serial.println("0 = FAST flow, 1 = STANDARD Flow, 2 = ATTESTATION Flow");
    break;
  }
}

void setLogLevel(const char* buf) {
  esp_log_level_t level = esp_log_level_get("*");
  if (strncmp(buf + 1, "E", 1) == 0) {
    level = ESP_LOG_ERROR;
    Serial.println("ERROR");
  }
  else if (strncmp(buf + 1, "W", 1) == 0) {
    level = ESP_LOG_WARN;
    Serial.println("WARNING");
  }
  else if (strncmp(buf + 1, "I", 1) == 0) {
    level = ESP_LOG_INFO;
    Serial.println("INFO");
  }
  else if (strncmp(buf + 1, "D", 1) == 0) {
    level = ESP_LOG_DEBUG;
    Serial.println("DEBUG");
  }
  else if (strncmp(buf + 1, "V", 1) == 0) {
    level = ESP_LOG_VERBOSE;
    Serial.println("VERBOSE");
  }
  else if (strncmp(buf + 1, "N", 1) == 0) {
    level = ESP_LOG_NONE;
    Serial.println("NONE");
  }

  esp_log_level_set(TAG, level);
  esp_log_level_set("HK_HomeKit", level);
  esp_log_level_set("HKAuthCtx", level);
  esp_log_level_set("HKFastAuth", level);
  esp_log_level_set("HKStdAuth", level);
  esp_log_level_set("HKAttestAuth", level);
  esp_log_level_set("PN532", level);
  esp_log_level_set("PN532_SPI", level);
  esp_log_level_set("ISO18013_SC", level);
  esp_log_level_set("LockMechanism", level);
  esp_log_level_set("NFCAccess", level);
}

void print_issuers(const char* buf) {
  for (auto&& issuer : readerData.issuers) {
    LOG(I, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str(), utils::bufToHexString(issuer.issuer_pk.data(), issuer.issuer_pk.size()).c_str());
    for (auto&& endpoint : issuer.endpoints) {
      LOG(I, "Endpoint ID: %s, Public Key: %s", utils::bufToHexString(endpoint.endpoint_id.data(), endpoint.endpoint_id.size()).c_str(), utils::bufToHexString(endpoint.endpoint_pk.data(), endpoint.endpoint_pk.size()).c_str());
    }
  }
}

/**
 * The function `set_custom_state_handler` translate the custom states to their HomeKit counterpart
 * updating the state of the lock and publishes the new state to the `MQTT_STATE_TOPIC` MQTT topic.
 *
 * @param client The `client` parameter in the `set_custom_state_handler` function is of type
 * `esp_mqtt_client_handle_t`, which is a handle to the MQTT client object for this event. This
 * parameter is used to interact with the MQTT client
 * @param state The `state` parameter in the `set_custom_state_handler` function represents the
 *  received custom state value
 */
void set_custom_state_handler(esp_mqtt_client_handle_t client, int state) {
  switch (state) {
  case customLockStates::C_UNLOCKING:
    lockTargetState->setVal(lockStates::UNLOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKING).c_str(), 0, 1, true);
    break;
  case customLockStates::C_LOCKING:
    lockTargetState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
    break;
  case customLockStates::C_UNLOCKED:
    if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
    }
    lockCurrentState->setVal(lockStates::UNLOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 0, 1, true);
    break;
  case customLockStates::C_LOCKED:
    if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
    }
    lockCurrentState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);
    break;
  case customLockStates::C_JAMMED:
    lockCurrentState->setVal(lockStates::JAMMED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::JAMMED).c_str(), 0, 1, true);
    break;
  case customLockStates::C_UNKNOWN:
    lockCurrentState->setVal(lockStates::UNKNOWN);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNKNOWN).c_str(), 0, 1, true);
  default:
    LOG(D, "Update state failed! Recv value not valid");
    break;
  }
}

void set_state_handler(esp_mqtt_client_handle_t client, int state) {
  switch (state) {
  case lockStates::UNLOCKED:
    if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
    }
    lockTargetState->setVal(state);
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 0, 1, true);
    if (espConfig::mqttData.lockEnableCustomState) {
      esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(customLockActions::UNLOCK).c_str(), 0, 0, false);
    }
    break;
  case lockStates::LOCKED:
    if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
    }
    lockTargetState->setVal(state);
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);
    if (espConfig::mqttData.lockEnableCustomState) {
      esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(customLockActions::LOCK).c_str(), 0, 0, false);
    }
    break;
  case lockStates::JAMMED:
  case lockStates::UNKNOWN:
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(state).c_str(), 0, 1, true);
    break;
  default:
    LOG(D, "Update state failed! Recv value not valid");
    break;
  }
}

/**
 * The `mqtt_event_handler` function handles MQTT events, including connection, message reception, and
 * publishing of device discovery information.
 *
 * @param event_data The `event_data` parameter in the `mqtt_event_handler` function is a handle to the
 * MQTT event data structure. It contains information about the event that occurred during the MQTT
 * communication, such as the event type, client handle, topic, data, and other relevant details. The
 * function processes different MQTT
 *
 * @return The function `mqtt_event_handler` is returning an `esp_err_t` type, specifically `ESP_OK`.
 */
esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event_data) {
  ESP_LOGD(TAG, "Event dispatched from callback type=%d", event_data->event_id);
  esp_mqtt_client_handle_t client = event_data->client;
  const esp_app_desc_t* app_desc = esp_ota_get_app_description();
  std::string app_version = app_desc->version;
  std::string topic(event_data->topic, event_data->topic + event_data->topic_len);
  std::string data(event_data->data, event_data->data + event_data->data_len);
  if (event_data->event_id == MQTT_EVENT_CONNECTED) {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char macStr[18] = { 0 };
    sprintf(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
    std::string serialNumber = "HK-";
    serialNumber.append(macStr);
    LOG(I, "MQTT connected");
    std::string statusTopic;
    statusTopic.append(espConfig::mqttData.mqttClientId).append("/status");
    if (espConfig::mqttData.hassMqttDiscoveryEnabled) {
      json payload;
      payload["topic"] = espConfig::mqttData.hkTopic.c_str();
      payload["value_template"] = "{{ value_json.uid }}";
      json device;
      device["name"] = "Lock";
      char identifier[18];
      sprintf(identifier, "%.2s%.2s%.2s%.2s%.2s%.2s", HAPClient::accessory.ID, HAPClient::accessory.ID + 3, HAPClient::accessory.ID + 6, HAPClient::accessory.ID + 9, HAPClient::accessory.ID + 12, HAPClient::accessory.ID + 15);
      std::string id = identifier;
      device["identifiers"] = json_array_arg;
      device["identifiers"].push_back(id);
      device["identifiers"].push_back(serialNumber);
      device["manufacturer"] = "rednblkx";
      device["model"] = "HomeKey-ESP32";
      device["sw_version"] = app_version.c_str();
      device["serial_number"] = serialNumber;
      payload["device"] = device;
      std::string bufferpub = payload.as<std::string>();
      std::string rfidTopic;
      rfidTopic.append("homeassistant/tag/").append(espConfig::mqttData.mqttClientId).append("/rfid/config");
      esp_mqtt_client_publish(client, rfidTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
      payload = json();
      payload["topic"] = espConfig::mqttData.hkTopic;
      payload["value_template"] = "{{ value_json.issuerId }}";
      payload["device"] = device;
      bufferpub = payload.as<std::string>();
      std::string issuerTopic;
      issuerTopic.append("homeassistant/tag/").append(espConfig::mqttData.mqttClientId).append("/hkIssuer/config");
      esp_mqtt_client_publish(client, issuerTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
      payload = json();
      payload["topic"] = espConfig::mqttData.hkTopic;
      payload["value_template"] = "{{ value_json.endpointId }}";
      payload["device"] = device;
      bufferpub = payload.as<std::string>();
      std::string endpointTopic;
      endpointTopic.append("homeassistant/tag/").append(espConfig::mqttData.mqttClientId).append("/hkEndpoint/config");
      esp_mqtt_client_publish(client, endpointTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
      payload = json();
      payload["name"] = espConfig::miscConfig.deviceName.c_str();
      payload["state_topic"] = espConfig::mqttData.lockStateTopic.c_str();
      payload["command_topic"] = espConfig::mqttData.lockStateCmd.c_str();
      payload["payload_lock"] = "1";
      payload["payload_unlock"] = "0";
      payload["state_locked"] = "1";
      payload["state_unlocked"] = "0";
      payload["state_unlocking"] = "4";
      payload["state_locking"] = "5";
      payload["state_jammed"] = "2";
      payload["availability_topic"] = statusTopic.c_str();
      payload["unique_id"] = id;
      payload["device"] = device;
      payload["retain"] = "false";
      bufferpub = payload.as<std::string>();
      std::string lockConfigTopic;
      lockConfigTopic.append("homeassistant/lock/").append(espConfig::mqttData.mqttClientId.c_str()).append("/lock/config");
      esp_mqtt_client_publish(client, lockConfigTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
      LOG(D, "MQTT PUBLISHED DISCOVERY");
    }
    esp_mqtt_client_publish(client, statusTopic.c_str(), "online", 6, 1, true);
    if (espConfig::mqttData.lockEnableCustomState) {
      esp_mqtt_client_subscribe(client, espConfig::mqttData.lockCustomStateCmd.c_str(), 0);
    }
    esp_mqtt_client_subscribe(client, espConfig::mqttData.lockStateCmd.c_str(), 0);
    esp_mqtt_client_subscribe(client, espConfig::mqttData.lockCStateCmd.c_str(), 0);
    esp_mqtt_client_subscribe(client, espConfig::mqttData.lockTStateCmd.c_str(), 0);
  }
  else if (event_data->event_id == MQTT_EVENT_DATA) {
    LOG(D, "Received message in topic \"%s\": %s", topic.c_str(), data.c_str());
    int state = atoi(data.c_str());
    if (!strcmp(espConfig::mqttData.lockCustomStateCmd.c_str(), topic.c_str())) {
      set_custom_state_handler(client, state);
    }
    else if (!strcmp(espConfig::mqttData.lockStateCmd.c_str(), topic.c_str())) {
      set_state_handler(client, state);
    }
    else if (!strcmp(espConfig::mqttData.lockTStateCmd.c_str(), topic.c_str())) {
      if (state == lockStates::UNLOCKED || state == lockStates::LOCKED) {
        lockTargetState->setVal(state);
        esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), state == lockStates::UNLOCKED ? std::to_string(lockStates::UNLOCKING).c_str() : std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
      }
    }
    else if (!strcmp(espConfig::mqttData.lockCStateCmd.c_str(), topic.c_str())) {
      if (state == lockStates::UNLOCKED || state == lockStates::LOCKED || state == lockStates::JAMMED || state == lockStates::UNKNOWN) {
        lockCurrentState->setVal(state);
        esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockCurrentState->getVal()).c_str(), 0, 1, true);
      }
    }
  }
  return ESP_OK;
}

/**
 * The function `mqtt_app_start` initializes and starts an MQTT client with specified configuration
 * parameters.
 */
static void mqtt_app_start(void) {
  std::string statusTopic;
  statusTopic.append(espConfig::mqttData.mqttClientId).append("/status");
  esp_mqtt_client_config_t mqtt_cfg = { };
  mqtt_cfg.host = espConfig::mqttData.mqttBroker.c_str();
  mqtt_cfg.port = espConfig::mqttData.mqttPort;
  mqtt_cfg.client_id = espConfig::mqttData.mqttClientId.c_str();
  mqtt_cfg.username = espConfig::mqttData.mqttUsername.c_str();
  mqtt_cfg.password = espConfig::mqttData.mqttPassword.c_str();
  mqtt_cfg.lwt_topic = statusTopic.c_str();
  mqtt_cfg.lwt_msg = "offline";
  mqtt_cfg.lwt_qos = 1;
  mqtt_cfg.lwt_retain = 1;
  mqtt_cfg.lwt_msg_len = 7;
  mqtt_cfg.event_handle = mqtt_event_handler;
  client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_start(client);
}

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

void listDir(fs::FS& fs, const char* dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

String miscHtmlProcess(const String& var) {
  if (var == "DEVICENAME") {
    return String(espConfig::miscConfig.deviceName.c_str());
  }
  else if (var == "CONTROLPIN") {
    return String(espConfig::miscConfig.controlPin);
  }
  else if (var == "LEDPIN") {
    return String(espConfig::miscConfig.hsStatusPin);
  }
  else if (var == "NFC1PIN") {
    return String(espConfig::miscConfig.nfcSuccessPin);
  }
  else if (var == "NFC2PIN") {
    return String(espConfig::miscConfig.nfcFailPin);
  }
  else if (var == "NFC1HL") {
    return String(espConfig::miscConfig.nfcSuccessHL);
  }
  else if (var == "NFC2HL") {
    return String(espConfig::miscConfig.nfcFailHL);
  }
  else if (var == "NFC1TIME") {
    return String(espConfig::miscConfig.nfcSuccessTime);
  }
  else if (var == "NFC2TIME") {
    return String(espConfig::miscConfig.nfcFailTime);
  }
  else if (var == "ALWAYSUNLOCK") {
    return String(espConfig::miscConfig.lockAlwaysUnlock);
  }
  else if (var == "ALWAYSLOCK") {
    return String(espConfig::miscConfig.lockAlwaysLock);
  }
  else if (var == "GPIOAEN") {
    return String(espConfig::miscConfig.gpioActionEnable);
  }
  else if (var == "GPIOAPIN") {
    return String(espConfig::miscConfig.gpioActionPin);
  }
  else if (var == "GPIOALOCK") {
    return String(espConfig::miscConfig.gpioActionLockState);
  }
  else if (var == "GPIOAUNLOCK") {
    return String(espConfig::miscConfig.gpioActionUnlockState);
  }
  return String();
}

String hkInfoHtmlProcess(const String& var) {
  String result = "";
  if (var == "READERGID") {
    return String(utils::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size(), true).c_str());
  }
  else if (var == "READERID") {
    return String(utils::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size(), true).c_str());
  }
  else if (var == "ISSUERSNO") {
    return String(readerData.issuers.size());
  }
  else if (var == "ISSUERSLIST") {
    for (auto&& issuer : readerData.issuers) {
      char issuerBuff[21 + 8];
      result += "<li>";
      snprintf(issuerBuff, sizeof(issuerBuff), "Issuer ID: %s", utils::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size(), true).c_str());
      result += issuerBuff;
      result += "</li>\n";
      result += "\t\t<ul>";
      for (auto&& endpoint : issuer.endpoints) {
        char endBuff[23 + 6];
        result += "\n\t\t\t<li>";
        snprintf(endBuff, sizeof(endBuff), "Endpoint ID: %s", utils::bufToHexString(endpoint.endpoint_id.data(), endpoint.endpoint_id.size(), true).c_str());
        result += endBuff;
        result += "</li>\n";
      }
      result += "\t\t</ul>";
    }
    return result;
  }
  return result;
}

String mqttHtmlProcess(const String& var) {
  String result = "";
  if (var == "MQTTBROKER") {
    return String(espConfig::mqttData.mqttBroker.c_str());
  }
  else if (var == "MQTTPORT") {
    return String(espConfig::mqttData.mqttPort);
  }
  else if (var == "MQTTCLIENTID") {
    return String(espConfig::mqttData.mqttClientId.c_str());
  }
  else if (var == "MQTTUSERNAME") {
    return String(espConfig::mqttData.mqttUsername.c_str());
  }
  else if (var == "MQTTPASSWORD") {
    return String(espConfig::mqttData.mqttPassword.c_str());
  }
  else if (var == "HKTOPIC") {
    return String(espConfig::mqttData.hkTopic.c_str());
  }
  else if (var == "STATETOPIC") {
    return String(espConfig::mqttData.lockStateTopic.c_str());
  }
  else if (var == "STATECMD") {
    return String(espConfig::mqttData.lockStateCmd.c_str());
  }
  else if (var == "CSTATECMD") {
    return String(espConfig::mqttData.lockCStateCmd.c_str());
  }
  else if (var == "TSTATECMD") {
    return String(espConfig::mqttData.lockTStateCmd.c_str());
  }
  else if (var == "CUSTOMSTATE_ENABLED") {
    return String(espConfig::mqttData.lockEnableCustomState);
  }
  else if (var == "CUSTOMSTATETOPIC") {
    return String(espConfig::mqttData.lockCustomStateTopic.c_str());
  }
  else if (var == "CUSTOMSTATECMD") {
    return String(espConfig::mqttData.lockCustomStateCmd.c_str());
  }
  else if (var == "DISCOVERY_ENABLED") {
    return String(espConfig::mqttData.hassMqttDiscoveryEnabled);
  }
  return result;
}

void setupWeb() {
  webServer.on("/info", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/info.html", "text/html", false, hkInfoHtmlProcess);
    });
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/index.html", "text/html", false, nullptr);
    });
  webServer.on("/mqtt", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/mqtt.html", "text/html", false, mqttHtmlProcess);
    });
  webServer.on("/misc", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/misc.html", "text/html", false, miscHtmlProcess);
    });
  webServer.on("/mqttconfig", HTTP_POST, [](AsyncWebServerRequest* request) {
    const char* TAG = "mqttconfig";
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      LOG(V, "POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      if (!strcmp(p->name().c_str(), "mqtt-broker")) {
        espConfig::mqttData.mqttBroker = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-port")) {
        int port = atoi(p->value().c_str());
        if (port > 0 && port < 65535) {
          espConfig::mqttData.mqttPort = port;
        }
      }
      else if (!strcmp(p->name().c_str(), "mqtt-clientid")) {
        espConfig::mqttData.mqttClientId = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-username")) {
        espConfig::mqttData.mqttUsername = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-password")) {
        espConfig::mqttData.mqttPassword = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-hktopic")) {
        espConfig::mqttData.hkTopic = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-statetopic")) {
        espConfig::mqttData.lockStateTopic = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-statecmd")) {
        espConfig::mqttData.lockStateCmd = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-cstatecmd")) {
        espConfig::mqttData.lockCStateCmd = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-tstatecmd")) {
        espConfig::mqttData.lockTStateCmd = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-customstate-enable")) {
        espConfig::mqttData.lockEnableCustomState = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-customstate-topic")) {
        espConfig::mqttData.lockCustomStateTopic = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-customstate-cmd")) {
        espConfig::mqttData.lockCustomStateCmd = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "mqtt-discovery-enable")) {
        espConfig::mqttData.hassMqttDiscoveryEnabled = p->value().toInt();
      }
    }
    try {
      std::string strNvs;
      encode_json(espConfig::mqttData, strNvs, indenting::indent);
      esp_err_t set_nvs = nvs_set_blob(savedData, "MQTTDATA", strNvs.data(), strNvs.size());
      esp_err_t commit_nvs = nvs_commit(savedData);
      LOG(V, "SET_STATUS: %s", esp_err_to_name(set_nvs));
      LOG(V, "COMMIT_STATUS: %s", esp_err_to_name(commit_nvs));
    }
    catch (const std::exception& e) {
      LOG(E, "%s", e.what());
    }

    request->send(200, "text/plain", "Received Config, Restarting...");
    ESP.restart();
    });
  webServer.on("/misc-config", HTTP_POST, [](AsyncWebServerRequest* request) {
    const char* TAG = "misc-config";
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      LOG(V, "POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      if (!strcmp(p->name().c_str(), "device-name")) {
        espConfig::miscConfig.deviceName = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "control-pin")) {
        espConfig::miscConfig.controlPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "led-pin")) {
        espConfig::miscConfig.hsStatusPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-s-pin")) {
        espConfig::miscConfig.nfcSuccessPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-f-pin")) {
        espConfig::miscConfig.nfcFailPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-s-hl")) {
        espConfig::miscConfig.nfcSuccessHL = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-f-hl")) {
        espConfig::miscConfig.nfcFailHL = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-s-time")) {
        espConfig::miscConfig.nfcSuccessTime = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-f-time")) {
        espConfig::miscConfig.nfcFailTime = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "hk-always-unlock")) {
        espConfig::miscConfig.lockAlwaysUnlock = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "hk-always-lock")) {
        espConfig::miscConfig.lockAlwaysLock = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "gpio-a-en")) {
        espConfig::miscConfig.gpioActionEnable = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "gpio-a-pin")) {
        espConfig::miscConfig.gpioActionPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "gpio-a-lock")) {
        espConfig::miscConfig.gpioActionLockState = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "gpio-a-unlock")) {
        espConfig::miscConfig.gpioActionUnlockState = p->value().toInt();
      }
    }
    try {
      std::string strNvs;
      encode_json(espConfig::miscConfig, strNvs, indenting::indent);
      esp_err_t set_nvs = nvs_set_blob(savedData, "MISCDATA", strNvs.data(), strNvs.size());
      esp_err_t commit_nvs = nvs_commit(savedData);
      LOG(V, "SET_STATUS: %s", esp_err_to_name(set_nvs));
      LOG(V, "COMMIT_STATUS: %s", esp_err_to_name(commit_nvs));
    }
    catch (const std::exception& e) {
      LOG(E, "%s", e.what());
    }

    request->send(200, "text/plain", "Received Config, Restarting...");
    ESP.restart();
    });
  webServer.onNotFound(notFound);
  webServer.begin();
}

void mqttConfigReset(const char* buf) {
  nvs_erase_key(savedData, "MQTTDATA");
  nvs_commit(savedData);
  ESP.restart();
}

void wifiCallback() {
  mqtt_app_start();
  setupWeb();
}

void mqtt_publish(std::string topic, std::string payload, uint8_t qos, bool retain) {
  if (client != nullptr) {
    esp_mqtt_client_publish(client, topic.c_str(), payload.c_str(), payload.length(), 0, retain);
  } else LOG(W, "MQTT Client not initialized, cannot publish message");
}

void nfc_thread_entry(void* arg) {
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    ESP_LOGE("NFC_SETUP", "Didn't find PN53x board");
  }
  else {
    unsigned int model = (versiondata >> 24) & 0xFF;
    ESP_LOGI("NFC_SETUP", "Found chip PN5%x", model);
    int maj = (versiondata >> 16) & 0xFF;
    int min = (versiondata >> 8) & 0xFF;
    ESP_LOGI("NFC_SETUP", "Firmware ver. %d.%d", maj, min);
    nfc.SAMConfig();
    nfc.setRFField(0x02, 0x01);
    nfc.setPassiveActivationRetries(0);
    ESP_LOGI("NFC_SETUP", "Waiting for an ISO14443A card");
  }
  memcpy(ecpData + 8, readerData.reader_gid.data(), readerData.reader_gid.size());
  with_crc16(ecpData, 16, ecpData + 16);
  while (1) {
    uint8_t res[4];
    uint16_t resLen = 4;
    nfc.writeRegister(0x633d, 0, true);
    nfc.inCommunicateThru(ecpData, sizeof(ecpData), res, &resLen, 100, true);
    uint8_t uid[16];
    uint8_t uidLen = 0;
    uint8_t atqa[2];
    uint8_t sak[1];
    bool passiveTarget = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, atqa, sak, 500, true, true);
    if (passiveTarget) {
      nfc.setPassiveActivationRetries(5);
      LOG(D, "ATQA: %02x", atqa[0]);
      LOG(D, "SAK: %02x", sak[0]);
      ESP_LOG_BUFFER_HEX_LEVEL(TAG, uid, (size_t)uidLen, ESP_LOG_VERBOSE);
      LOG(I, "*** PASSIVE TARGET DETECTED ***");
      auto startTime = std::chrono::high_resolution_clock::now();
      uint8_t data[13] = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x0 };
      uint8_t selectCmdRes[9];
      uint16_t selectCmdResLength = 9;
      LOG(D, "SELECT HomeKey Applet, APDU: ");
      ESP_LOG_BUFFER_HEX_LEVEL(TAG, data, sizeof(data), ESP_LOG_VERBOSE);
      bool status = nfc.inDataExchange(data, sizeof(data), selectCmdRes, &selectCmdResLength);
      LOG(D, "SELECT HomeKey Applet, Response");
      ESP_LOG_BUFFER_HEX_LEVEL(TAG, selectCmdRes, selectCmdResLength, ESP_LOG_VERBOSE);
      if (status && selectCmdRes[selectCmdResLength - 2] == 0x90 && selectCmdRes[selectCmdResLength - 1] == 0x00) {
        LOG(D, "*** SELECT HOMEKEY APPLET SUCCESSFUL ***");
        LOG(D, "Reader Private Key: %s", utils::bufToHexString(readerData.reader_pk.data(), readerData.reader_pk.size()).c_str());
        HKAuthenticationContext authCtx(nfc, readerData, savedData);
        auto authResult = authCtx.authenticate(hkFlow);
        if (std::get<2>(authResult) != kFlowFailed) {
          bool status = true;
          xQueueSend(gpio_led_handle, &status, 0);
          json payload;
          payload["issuerId"] = json(byte_string_arg, std::get<0>(authResult), semantic_tag::base16);
          payload["endpointId"] = json(byte_string_arg, std::get<1>(authResult), semantic_tag::base16);
          payload["homekey"] = true;
          std::string payloadStr = payload.as<std::string>();
          mqtt_publish(espConfig::mqttData.hkTopic, payloadStr, 0, false);
          if (espConfig::miscConfig.lockAlwaysUnlock) {
            lockCurrentState->setVal(lockStates::UNLOCKED);
            lockTargetState->setVal(lockStates::UNLOCKED);
            if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
            }
            mqtt_publish(espConfig::mqttData.lockStateTopic, std::to_string(lockStates::UNLOCKED), 1, true);
            if (espConfig::mqttData.lockEnableCustomState) {
              mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(customLockActions::UNLOCK), 0, false);
            }
          }
          else if (espConfig::miscConfig.lockAlwaysLock) {
            lockCurrentState->setVal(lockStates::LOCKED);
            lockTargetState->setVal(lockStates::LOCKED);
            if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
            }
            mqtt_publish(espConfig::mqttData.lockStateTopic, std::to_string(lockStates::LOCKED), 1, true);
            if (espConfig::mqttData.lockEnableCustomState) {
              mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(customLockActions::LOCK), 0, false);
            }
          }
          else {
            int currentState = lockCurrentState->getVal();
            if (espConfig::miscConfig.gpioActionEnable && espConfig::miscConfig.gpioActionPin != 0) {
              if (currentState == lockStates::UNLOCKED) {
                digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
                lockTargetState->setVal(lockStates::LOCKED);
                lockCurrentState->setVal(lockStates::LOCKED);
                mqtt_publish(espConfig::mqttData.lockStateTopic, std::to_string(lockStates::LOCKED), 0, false);
              } else if (currentState == lockStates::LOCKED) {
                digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
                lockTargetState->setVal(lockStates::UNLOCKED);
                lockCurrentState->setVal(lockStates::UNLOCKED);
                mqtt_publish(espConfig::mqttData.lockStateTopic, std::to_string(lockStates::UNLOCKED), 0, false);
              }
            }
            if (espConfig::mqttData.lockEnableCustomState) {
              if (currentState == lockStates::UNLOCKED) {
                mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(customLockActions::LOCK), 0, false);
              }
              else if (currentState == lockStates::LOCKED) {
                mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(customLockActions::UNLOCK), 0, false);
              }
            }
          }

          auto stopTime = std::chrono::high_resolution_clock::now();
          LOG(I, "Total Time (detection->auth->gpio->mqtt): %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
        }
        else {
          bool status = false;
          xQueueSend(gpio_led_handle, &status, 0);
          LOG(W, "We got status FlowFailed, mqtt untouched!");
        }
        nfc.setRFField(0x02, 0x01);
      }
      else {
        LOG(I, "Invalid Response, probably not Homekey, publishing target's UID");
        bool status = true;
        xQueueSend(gpio_led_handle, &status, 0);
        json_options options;
        options.byte_string_format(byte_string_chars_format::base16);
        json payload;
        payload["atqa"] = byte_string(atqa, 2);
        payload["sak"] = byte_string(sak, 1);
        payload["uid"] = byte_string(uid, uidLen);
        payload["homekey"] = false;
        std::string payload_dump;
        payload.dump(payload_dump, options);
        if (client != nullptr) {
          esp_mqtt_client_publish(client, espConfig::mqttData.hkTopic.c_str(), payload_dump.c_str(), 0, 0, false);
        }
        else LOG(W, "MQTT Client not initialized, cannot publish message");
      }
      vTaskDelay(50 / portTICK_PERIOD_MS);
      nfc.inRelease();
      int counter = 50;
      bool deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
      LOG(D, "Target still present: %d", deviceStillInField);
      while (deviceStillInField) {
        if (counter == 0) break;
        vTaskDelay(50 / portTICK_PERIOD_MS);
        nfc.inRelease();
        deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
        --counter;
        LOG(D, "Target still present: %d Counter=%d", deviceStillInField, counter);
      }
      nfc.inRelease();
      nfc.setPassiveActivationRetries(0);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void gpio_task(void* arg) {
  bool status = false;
  while (1) {
    if (gpio_led_handle != nullptr) {
      if (uxQueueMessagesWaiting(gpio_led_handle) > 0) {
        LOG(D, "Got something in queue");
        xQueueReceive(gpio_led_handle, &status, 0);
        if (status) {
          if (espConfig::miscConfig.nfcSuccessPin && espConfig::miscConfig.nfcSuccessPin != 0) {
            digitalWrite(espConfig::miscConfig.nfcSuccessPin, espConfig::miscConfig.nfcSuccessHL);
            delay(espConfig::miscConfig.nfcSuccessTime);
            digitalWrite(espConfig::miscConfig.nfcSuccessPin, !espConfig::miscConfig.nfcSuccessHL);
          }
        } else {
          if (espConfig::miscConfig.nfcFailPin && espConfig::miscConfig.nfcFailPin != 0) {
            digitalWrite(espConfig::miscConfig.nfcFailPin, espConfig::miscConfig.nfcFailHL);
            delay(espConfig::miscConfig.nfcFailTime);
            digitalWrite(espConfig::miscConfig.nfcFailPin, !espConfig::miscConfig.nfcFailHL);
          }
        }
      }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  const esp_app_desc_t* app_desc = esp_ota_get_app_description();
  std::string app_version = app_desc->version;
  gpio_led_handle = xQueueCreate(2, sizeof(bool));
  size_t len;
  const char* TAG = "SETUP";
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData);
  if (!nvs_get_blob(savedData, "READERDATA", NULL, &len)) {
    std::vector<uint8_t> savedBuf(len);
    nvs_get_blob(savedData, "READERDATA", savedBuf.data(), &len);
    LOG(I, "NVS DATA LENGTH: %d", len);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, savedBuf.data(), savedBuf.size(), ESP_LOG_DEBUG);
    try {
      readerData = msgpack::decode_msgpack<readerData_t>(savedBuf);
    }
    catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
    }

  }
  if (!nvs_get_blob(savedData, "MQTTDATA", NULL, &len)) {
    uint8_t msgpack[len];
    nvs_get_blob(savedData, "MQTTDATA", msgpack, &len);
    std::string str(msgpack, msgpack + len);
    LOG(D, "MQTTDATA - JSON(%d): %s", len, str.c_str());
    try {
      espConfig::mqttData = decode_json<espConfig::mqttConfig_t>(str);
    }
    catch (const std::exception& e) {
      LOG(E, "%s", e.what());
    }
  }
  if (!nvs_get_blob(savedData, "MISCDATA", NULL, &len)) {
    uint8_t msgpack[len];
    nvs_get_blob(savedData, "MISCDATA", msgpack, &len);
    std::string str(msgpack, msgpack + len);
    LOG(D, "MISCDATA - JSON(%d): %s", len, str.c_str());
    try {
      espConfig::miscConfig = decode_json<espConfig::misc_config_t>(str);
    }
    catch (const std::exception& e) {
      LOG(E, "%s", e.what());
    }
  }
  if (espConfig::miscConfig.nfcSuccessPin && espConfig::miscConfig.nfcSuccessPin != 0) {
    pinMode(espConfig::miscConfig.nfcSuccessPin, OUTPUT);
  }
  if (espConfig::miscConfig.nfcFailPin && espConfig::miscConfig.nfcFailPin != 0) {
    pinMode(espConfig::miscConfig.nfcFailPin, OUTPUT);
  }
  if (espConfig::miscConfig.gpioActionPin && espConfig::miscConfig.gpioActionPin != 0) {
    pinMode(espConfig::miscConfig.gpioActionPin, OUTPUT);
  }
  if (!LittleFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LITTLEFS");
    return;
  }
  listDir(LittleFS, "/", 0);
  homeSpan.setControlPin(espConfig::miscConfig.controlPin);
  homeSpan.setStatusPin(espConfig::miscConfig.hsStatusPin);
  homeSpan.setStatusAutoOff(15);
  homeSpan.reserveSocketConnections(2);
  if (strcmp(SETUP_CODE, "46637726")) {
    homeSpan.setPairingCode(SETUP_CODE);
  }
  homeSpan.setLogLevel(0);
  homeSpan.setSketchVersion(app_version.c_str());

  LOG(I, "READER GROUP ID (%d): %s", readerData.reader_gid.size(), utils::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size()).c_str());
  LOG(I, "READER UNIQUE ID (%d): %s", readerData.reader_id.size(), utils::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size()).c_str());

  LOG(I, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto&& issuer : readerData.issuers) {
    LOG(D, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str(), utils::bufToHexString(issuer.issuer_pk.data(), issuer.issuer_pk.size()).c_str());
  }
  homeSpan.enableOTA(OTA_PWD);
  homeSpan.setPortNum(1201);
  homeSpan.begin(Category::Locks, espConfig::miscConfig.deviceName.c_str(), "HK", "HomeKey-ESP32");
  homeSpan.autoPoll();

  new SpanUserCommand('D', "Delete Home Key Data", deleteReaderData);
  new SpanUserCommand('L', "Set Log Level", setLogLevel);
  new SpanUserCommand('F', "Set HomeKey Flow", setFlow);
  new SpanUserCommand('P', "Print Issuers", print_issuers);
  new SpanUserCommand('M', "Erase MQTT Config and restart", mqttConfigReset);
  new SpanUserCommand('R', "Remove Endpoints", [](const char*) {
    for (auto&& issuer : readerData.issuers) {
      issuer.endpoints.clear();
    }
    save_to_nvs();
  });


  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new Characteristic::Manufacturer("rednblkx");
  new Characteristic::Model("HomeKey-ESP32");
  new Characteristic::Name(DEVICE_NAME);
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[18] = { 0 };
  sprintf(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
  std::string serialNumber = "HK-";
  serialNumber.append(macStr);
  new Characteristic::SerialNumber(serialNumber.c_str());
  new Characteristic::FirmwareRevision(app_version.c_str());
  new Characteristic::HardwareFinish();

  new LockManagement();
  new LockMechanism();
  new NFCAccess();
  new Service::HAPProtocolInformation();
  new Characteristic::Version();
  homeSpan.setControllerCallback(pairCallback);
  homeSpan.setWifiCallback(wifiCallback);
  xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 1, NULL);
  xTaskCreate(nfc_thread_entry, "nfc_task", 8192, NULL, 2, NULL);
}

//////////////////////////////////////

void loop() {
}
