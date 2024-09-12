#define JSON_NOEXCEPTION 1
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
#include <pins_arduino.h>
#include <src/extras/Pixel.h>

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
PN532_SPI pn532spi(SS, SCK, MISO, MOSI);
PN532 nfc(pn532spi);
QueueHandle_t gpio_led_handle = nullptr;
QueueHandle_t neopixel_handle = nullptr;
QueueHandle_t gpio_lock_handle = nullptr;
TaskHandle_t *gpio_led_task_handle = nullptr;
TaskHandle_t *neopixel_task_handle = nullptr;
TaskHandle_t *gpio_lock_task_handle = nullptr;

nvs_handle savedData;
readerData_t readerData;
uint8_t ecpData[18] = { 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 };
std::map<HK_COLOR, const char*> hk_color_vals = { {TAN, "AQTO1doA"}, {GOLD, "AQSq1uwA"}, {SILVER, "AQTj4+MA"}, {BLACK, "AQQAAAAA"} };
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
    std::string lwtTopic = MQTT_LWT_TOPIC;
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
    std::map<std::string, int> customLockStates = { {"C_LOCKED", C_LOCKED}, {"C_UNLOCKING", C_UNLOCKING}, {"C_UNLOCKED", C_UNLOCKED}, {"C_LOCKING", C_LOCKING}, {"C_JAMMED", C_JAMMED}, {"C_UNKNOWN", C_UNKNOWN} };
    std::map<std::string, int> customLockActions = { {"UNLOCK", UNLOCK}, {"LOCK", LOCK} };
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(espConfig::mqttConfig_t, mqttBroker, mqttPort, mqttUsername, mqttPassword, mqttClientId, lwtTopic, hkTopic, lockStateTopic, lockStateCmd, lockCStateCmd, lockTStateCmd, lockCustomStateTopic, lockCustomStateCmd, lockEnableCustomState, hassMqttDiscoveryEnabled, customLockStates, customLockActions)
  } mqttData;

  struct misc_config_t
  {
    enum colorMap
    {
      R,
      G,
      B
    };
    std::string deviceName = DEVICE_NAME;
    std::string otaPasswd = OTA_PWD;
    uint8_t hk_key_color = HOMEKEY_COLOR;
    std::string setupCode = SETUP_CODE;
    bool lockAlwaysUnlock = HOMEKEY_ALWAYS_UNLOCK;
    bool lockAlwaysLock = HOMEKEY_ALWAYS_LOCK;
    uint8_t controlPin = HS_PIN;
    uint8_t hsStatusPin = HS_STATUS_LED;
    uint8_t nfcNeopixelPin = NFC_NEOPIXEL_PIN;
    std::map<colorMap, int> neopixelSuccessColor = { {R, NEOPIXEL_SUCCESS_R}, {G, NEOPIXEL_SUCCESS_G}, {B, NEOPIXEL_SUCCESS_B} };
    std::map<colorMap, int> neopixelFailureColor = { {R, NEOPIXEL_FAIL_R}, {G, NEOPIXEL_FAIL_G}, {B, NEOPIXEL_FAIL_B} };
    uint16_t neopixelSuccessTime = NEOPIXEL_SUCCESS_TIME;
    uint16_t neopixelFailTime = NEOPIXEL_FAIL_TIME;
    uint8_t nfcSuccessPin = NFC_SUCCESS_PIN;
    uint16_t nfcSuccessTime = NFC_SUCCESS_TIME;
    bool nfcSuccessHL = NFC_SUCCESS_HL;
    uint8_t nfcFailPin = NFC_FAIL_PIN;
    uint16_t nfcFailTime = NFC_FAIL_TIME;
    bool nfcFailHL = NFC_FAIL_HL;
    uint8_t gpioActionPin = GPIO_ACTION_PIN;
    bool gpioActionLockState = GPIO_ACTION_LOCK_STATE;
    bool gpioActionUnlockState = GPIO_ACTION_UNLOCK_STATE;
    bool gpioActionMomentaryEnabled = GPIO_ACTION_MOMENTARY_STATE;
    uint16_t gpioActionMomentaryTimeout = GPIO_ACTION_MOMENTARY_TIMEOUT;
    bool webAuthEnabled = WEB_AUTH_ENABLED;
    std::string webUsername = WEB_AUTH_USERNAME;
    std::string webPassword = WEB_AUTH_PASSWORD;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(misc_config_t, deviceName, otaPasswd, hk_key_color, setupCode, lockAlwaysUnlock, lockAlwaysLock, controlPin, hsStatusPin, nfcSuccessPin, nfcSuccessTime, nfcNeopixelPin, neopixelSuccessColor, neopixelFailureColor, neopixelSuccessTime, neopixelFailTime, nfcSuccessHL, nfcFailPin, nfcFailTime, nfcFailHL, gpioActionPin, gpioActionLockState, gpioActionUnlockState, gpioActionMomentaryEnabled, gpioActionMomentaryTimeout, webAuthEnabled, webUsername, webPassword)
  } miscConfig;
};

KeyFlow hkFlow = KeyFlow::kFlowFAST;
SpanCharacteristic* lockCurrentState;
SpanCharacteristic* lockTargetState;
esp_mqtt_client_handle_t client = nullptr;

std::shared_ptr<Pixel> pixel;

bool save_to_nvs() {
  std::vector<uint8_t> serialized = nlohmann::json::to_msgpack(readerData);
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", serialized.data(), serialized.size());
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


void gpio_task(void* arg) {
  uint8_t status = 0;
  while (1) {
    if (gpio_lock_handle != nullptr) {
      status = 0;
      if (uxQueueMessagesWaiting(gpio_lock_handle) > 0) {
        xQueueReceive(gpio_lock_handle, &status, 0);
        LOG(D, "Got something in queue %d", status);
        if (status == 1) {
          if (espConfig::miscConfig.gpioActionMomentaryEnabled) {
            LOG(D, "%d - %d - %d -%d", espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionMomentaryEnabled, espConfig::miscConfig.lockAlwaysUnlock, espConfig::miscConfig.lockAlwaysLock);
            if (espConfig::miscConfig.lockAlwaysUnlock) {
              lockTargetState->setVal(lockStates::UNLOCKED);
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
              lockCurrentState->setVal(lockStates::UNLOCKED);
              delay(espConfig::miscConfig.gpioActionMomentaryTimeout);
              lockTargetState->setVal(lockStates::LOCKED);
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
              lockCurrentState->setVal(lockStates::LOCKED);
            } else if (espConfig::miscConfig.lockAlwaysLock) {
              lockTargetState->setVal(lockStates::LOCKED);
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
              lockCurrentState->setVal(lockStates::LOCKED);
              delay(espConfig::miscConfig.gpioActionMomentaryTimeout);
              lockTargetState->setVal(lockStates::UNLOCKED);
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
              lockCurrentState->setVal(lockStates::UNLOCKED);
            } else {
              int currentState = lockCurrentState->getVal();
              lockTargetState->setVal(!currentState);
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
              lockCurrentState->setVal(!currentState);
              delay(espConfig::miscConfig.gpioActionMomentaryTimeout);
              lockTargetState->setVal(currentState);
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
              lockCurrentState->setVal(currentState);
            }
          }
        }
        else if (status == 2) {
          return;
        }
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void neopixel_task(void* arg) {
  uint8_t status = 0;
  while (1) {
    if (neopixel_handle != nullptr) {
      status = 0;
      if (uxQueueMessagesWaiting(neopixel_handle) > 0) {
        xQueueReceive(neopixel_handle, &status, 0);
        LOG(D, "Got something in queue %d", status);
        switch (status) {
        case 0:
          if (espConfig::miscConfig.nfcNeopixelPin && espConfig::miscConfig.nfcNeopixelPin != 255) {
            LOG(D, "SUCCESS PIXEL %d:%d,%d,%d", espConfig::miscConfig.nfcNeopixelPin, espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::R], espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::G], espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::B]);
            pixel->set(pixel->RGB(espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::R], espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::G], espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::B]));
            delay(espConfig::miscConfig.neopixelFailTime);
            pixel->off();
          }
          break;
        case 1:
          if (espConfig::miscConfig.nfcNeopixelPin && espConfig::miscConfig.nfcNeopixelPin != 255) {
            LOG(D, "FAIL PIXEL %d:%d,%d,%d", espConfig::miscConfig.nfcNeopixelPin, espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::R], espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::G], espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::B]);
            pixel->set(pixel->RGB(espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::R], espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::G], espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::B]));
            delay(espConfig::miscConfig.neopixelSuccessTime);
            pixel->off();
          }
          break;
        default:
          return;
          break;
        }
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void nfc_gpio_task(void* arg) {
  uint8_t status = 0;
  while (1) {
    if (gpio_led_handle != nullptr) {
      status = 0;
      if (uxQueueMessagesWaiting(gpio_led_handle) > 0) {
        xQueueReceive(gpio_led_handle, &status, 0);
        LOG(D, "Got something in queue %d", status);
        switch (status) {
        case 0:
          if (espConfig::miscConfig.nfcFailPin && espConfig::miscConfig.nfcFailPin != 255) {
            LOG(D, "FAIL LED %d:%d", espConfig::miscConfig.nfcFailPin, espConfig::miscConfig.nfcFailHL);
            digitalWrite(espConfig::miscConfig.nfcFailPin, espConfig::miscConfig.nfcFailHL);
            delay(espConfig::miscConfig.nfcFailTime);
            digitalWrite(espConfig::miscConfig.nfcFailPin, !espConfig::miscConfig.nfcFailHL);
          }
          break;
        case 1:
          if (espConfig::miscConfig.nfcSuccessPin && espConfig::miscConfig.nfcSuccessPin != 255) {
            LOG(D, "SUCCESS LED %d:%d", espConfig::miscConfig.nfcSuccessPin, espConfig::miscConfig.nfcSuccessHL);
            digitalWrite(espConfig::miscConfig.nfcSuccessPin, espConfig::miscConfig.nfcSuccessHL);
            delay(espConfig::miscConfig.nfcSuccessTime);
            digitalWrite(espConfig::miscConfig.nfcSuccessPin, !espConfig::miscConfig.nfcSuccessHL);
          }
          break;
        default:
          LOG(I, "STOP");
          return;
          break;
        }
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
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
    if (espConfig::miscConfig.gpioActionPin != 255) {
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
    if (espConfig::miscConfig.gpioActionPin != 255) {
      switch (targetState) {
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
          esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]).c_str(), 0, 0, false);
        }
        else if (targetState == lockStates::LOCKED) {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["LOCK"]).c_str(), 0, 0, false);
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
    TLV8 conf(NULL, 0);
    conf.add(0x01, 0x10);
    conf.add(0x02, 0x10);
    nfcSupportedConfiguration = new Characteristic::NFCAccessSupportedConfiguration(conf);
  }

  boolean update() {
    LOG(D, "PROVISIONED READER KEY: %s", utils::bufToHexString(readerData.reader_pk.data(), readerData.reader_pk.size()).c_str());
    LOG(D, "READER GROUP IDENTIFIER: %s", utils::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size()).c_str());
    LOG(D, "READER UNIQUE IDENTIFIER: %s", utils::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size()).c_str());

    TLV8 ctrlData(NULL, 0);
    nfcControlPoint->getNewTLV(ctrlData);
    std::vector<uint8_t> tlvData(ctrlData.pack_size());
    ctrlData.pack(tlvData.data());
    if (tlvData.size() == 0)
      return false;
    LOG(D, "Decoded data: %s", utils::bufToHexString(tlvData.data(), tlvData.size()).c_str());
    LOG(D, "Decoded data length: %d", tlvData.size());
    HK_HomeKit hkCtx(readerData, savedData, "READERDATA", tlvData);
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
      newIssuer.issuer_id = std::vector<uint8_t>{ id.begin(), id.begin() + 8 };
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
  if (espConfig::mqttData.customLockStates["C_UNLOCKING"] == state) {
    lockTargetState->setVal(lockStates::UNLOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKING).c_str(), 0, 1, true);
    return;
  }
  else if (espConfig::mqttData.customLockStates["C_LOCKING"] == state) {
    lockTargetState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
    return;
  }
  else if (espConfig::mqttData.customLockStates["C_UNLOCKED"] == state) {
    if (espConfig::miscConfig.gpioActionPin != 255) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
    }
    lockCurrentState->setVal(lockStates::UNLOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 0, 1, true);
    return;
  }
  else if (espConfig::mqttData.customLockStates["C_LOCKED"] == state) {
    if (espConfig::miscConfig.gpioActionPin != 255) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
    }
    lockCurrentState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);
    return;
  }
  else if (espConfig::mqttData.customLockStates["C_JAMMED"] == state) {
    lockCurrentState->setVal(lockStates::JAMMED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::JAMMED).c_str(), 0, 1, true);
    return;
  }
  else if (espConfig::mqttData.customLockStates["C_UNKNOWN"] == state) {
    lockCurrentState->setVal(lockStates::UNKNOWN);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNKNOWN).c_str(), 0, 1, true);
    return;
  }
  LOG(D, "Update state failed! Recv value not valid");
}

void set_state_handler(esp_mqtt_client_handle_t client, int state) {
  switch (state) {
  case lockStates::UNLOCKED:
    if (espConfig::miscConfig.gpioActionPin != 255) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
    }
    lockTargetState->setVal(state);
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 0, 1, true);
    if (espConfig::mqttData.lockEnableCustomState) {
      esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]).c_str(), 0, 0, false);
    }
    break;
  case lockStates::LOCKED:
    if (espConfig::miscConfig.gpioActionPin != 255) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
    }
    lockTargetState->setVal(state);
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);
    if (espConfig::mqttData.lockEnableCustomState) {
      esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["LOCK"]).c_str(), 0, 0, false);
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

void mqtt_connected_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  esp_mqtt_client_handle_t client = event->client;
  const esp_app_desc_t* app_desc = esp_ota_get_app_description();
  std::string app_version = app_desc->version;
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[18] = { 0 };
  sprintf(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
  std::string serialNumber = "HK-";
  serialNumber.append(macStr);
  LOG(I, "MQTT connected");
  if (espConfig::mqttData.hassMqttDiscoveryEnabled) {
    json payload;
    payload["topic"] = espConfig::mqttData.hkTopic.c_str();
    payload["value_template"] = "{{ value_json.uid }}";
    json device;
    device["name"] = espConfig::miscConfig.deviceName.c_str();
    char identifier[18];
    sprintf(identifier, "%.2s%.2s%.2s%.2s%.2s%.2s", HAPClient::accessory.ID, HAPClient::accessory.ID + 3, HAPClient::accessory.ID + 6, HAPClient::accessory.ID + 9, HAPClient::accessory.ID + 12, HAPClient::accessory.ID + 15);
    std::string id = identifier;
    device["identifiers"].push_back(id);
    device["identifiers"].push_back(serialNumber);
    device["manufacturer"] = "rednblkx";
    device["model"] = "HomeKey-ESP32";
    device["sw_version"] = app_version.c_str();
    device["serial_number"] = serialNumber;
    payload["device"] = device;
    std::string bufferpub = payload.dump();
    std::string rfidTopic;
    rfidTopic.append("homeassistant/tag/").append(espConfig::mqttData.mqttClientId).append("/rfid/config");
    esp_mqtt_client_publish(client, rfidTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    payload = json();
    payload["topic"] = espConfig::mqttData.hkTopic;
    payload["value_template"] = "{{ value_json.issuerId }}";
    payload["device"] = device;
    bufferpub = payload.dump();
    std::string issuerTopic;
    issuerTopic.append("homeassistant/tag/").append(espConfig::mqttData.mqttClientId).append("/hkIssuer/config");
    esp_mqtt_client_publish(client, issuerTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    payload = json();
    payload["topic"] = espConfig::mqttData.hkTopic;
    payload["value_template"] = "{{ value_json.endpointId }}";
    payload["device"] = device;
    bufferpub = payload.dump();
    std::string endpointTopic;
    endpointTopic.append("homeassistant/tag/").append(espConfig::mqttData.mqttClientId).append("/hkEndpoint/config");
    esp_mqtt_client_publish(client, endpointTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    payload = json();
    payload["name"] = "Lock";
    payload["state_topic"] = espConfig::mqttData.lockStateTopic.c_str();
    payload["command_topic"] = espConfig::mqttData.lockStateCmd.c_str();
    payload["payload_lock"] = "1";
    payload["payload_unlock"] = "0";
    payload["state_locked"] = "1";
    payload["state_unlocked"] = "0";
    payload["state_unlocking"] = "4";
    payload["state_locking"] = "5";
    payload["state_jammed"] = "2";
    payload["availability_topic"] = espConfig::mqttData.lwtTopic.c_str();
    payload["unique_id"] = id;
    payload["device"] = device;
    payload["retain"] = "false";
    bufferpub = payload.dump();
    std::string lockConfigTopic;
    lockConfigTopic.append("homeassistant/lock/").append(espConfig::mqttData.mqttClientId.c_str()).append("/lock/config");
    esp_mqtt_client_publish(client, lockConfigTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    LOG(D, "MQTT PUBLISHED DISCOVERY");
  }
  esp_mqtt_client_publish(client, espConfig::mqttData.lwtTopic.c_str(), "online", 6, 1, true);
  if (espConfig::mqttData.lockEnableCustomState) {
    esp_mqtt_client_subscribe(client, espConfig::mqttData.lockCustomStateCmd.c_str(), 0);
  }
  esp_mqtt_client_subscribe(client, espConfig::mqttData.lockStateCmd.c_str(), 0);
  esp_mqtt_client_subscribe(client, espConfig::mqttData.lockCStateCmd.c_str(), 0);
  esp_mqtt_client_subscribe(client, espConfig::mqttData.lockTStateCmd.c_str(), 0);
}

void mqtt_data_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  ESP_LOGD(TAG, "Event dispatched from callback type=%d", event_id);
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  esp_mqtt_client_handle_t client = event->client;
  std::string topic(event->topic, event->topic + event->topic_len);
  std::string data(event->data, event->data + event->data_len);
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

/**
 * The function `mqtt_app_start` initializes and starts an MQTT client with specified configuration
 * parameters.
 */
static void mqtt_app_start(void) {
  esp_mqtt_client_config_t mqtt_cfg = { };
  mqtt_cfg.host = espConfig::mqttData.mqttBroker.c_str();
  mqtt_cfg.port = espConfig::mqttData.mqttPort;
  mqtt_cfg.client_id = espConfig::mqttData.mqttClientId.c_str();
  mqtt_cfg.username = espConfig::mqttData.mqttUsername.c_str();
  mqtt_cfg.password = espConfig::mqttData.mqttPassword.c_str();
  mqtt_cfg.lwt_topic = espConfig::mqttData.lwtTopic.c_str();
  mqtt_cfg.lwt_msg = "offline";
  mqtt_cfg.lwt_qos = 1;
  mqtt_cfg.lwt_retain = 1;
  mqtt_cfg.lwt_msg_len = 7;
  client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(client, MQTT_EVENT_CONNECTED, mqtt_connected_event, client);
  esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, mqtt_data_handler, client);
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
  else if (var == "OTAPASSWD") {
    return String(espConfig::miscConfig.otaPasswd.c_str());
  }
  else if (var == "HKSETUPCODE") {
    return String(espConfig::miscConfig.setupCode.c_str());
  }
  else if (var == "CONTROLPIN") {
    return String(espConfig::miscConfig.controlPin);
  }
  else if (var == "LEDPIN") {
    return String(espConfig::miscConfig.hsStatusPin);
  }
  else if (var == "ALWAYSUNLOCK") {
    return String(espConfig::miscConfig.lockAlwaysUnlock);
  }
  else if (var == "ALWAYSLOCK") {
    return String(espConfig::miscConfig.lockAlwaysLock);
  }
  else if (var == "HWFINISH") {
    return String(espConfig::miscConfig.hk_key_color);
  }
  else if (var == "WEBENABLE") {
    return String(espConfig::miscConfig.webAuthEnabled);
  }
  else if (var == "WEBUSERNAME") {
    return String(espConfig::miscConfig.webUsername.c_str());
  }
  else if (var == "WEBPASSWORD") {
    return String(espConfig::miscConfig.webPassword.c_str());
  }
  return String();
}

String hkInfoHtmlProcess(const String& var) {
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
    String result = "";
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
  return "";
}

String mqttHtmlProcess(const String& var) {
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
  else if (var == "MQTTLWTTOPIC") {
    return String(espConfig::mqttData.lwtTopic.c_str());
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
  else if (var == "CACTIONUNLOCK") {
    return String(espConfig::mqttData.customLockActions["UNLOCK"]);
  }
  else if (var == "CACTIONLOCK") {
    return String(espConfig::mqttData.customLockActions["LOCK"]);
  }
  else if (var == "CSTATEUNLOCKING") {
    return String(espConfig::mqttData.customLockStates["C_UNLOCKING"]);
  }
  else if (var == "CSTATELOCKING") {
    return String(espConfig::mqttData.customLockStates["C_LOCKING"]);
  }
  else if (var == "CSTATEUNLOCKED") {
    return String(espConfig::mqttData.customLockStates["C_UNLOCKED"]);
  }
  else if (var == "CSTATELOCKED") {
    return String(espConfig::mqttData.customLockStates["C_LOCKED"]);
  }
  else if (var == "CSTATEJAMMED") {
    return String(espConfig::mqttData.customLockStates["C_JAMMED"]);
  }
  else if (var == "CSTATEUNKNOWN") {
    return String(espConfig::mqttData.customLockStates["C_UNKNOWN"]);
  }
  return "";
}

String indexProcess(const String& var) {
  if (var == "VERSION") {
    const esp_app_desc_t* app_desc = esp_ota_get_app_description();
    std::string app_version = app_desc->version;
    return String(app_version.c_str());
  }
  return "";
}

String actionsProcess(const String& var) {
  if (var == "NFCNEOPIXELPIN") {
    return String(espConfig::miscConfig.nfcNeopixelPin);
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
  else if (var == "GPIOAPIN") {
    return String(espConfig::miscConfig.gpioActionPin);
  }
  else if (var == "GPIOALOCK") {
    return String(espConfig::miscConfig.gpioActionLockState);
  }
  else if (var == "GPIOAUNLOCK") {
    return String(espConfig::miscConfig.gpioActionUnlockState);
  }
  else if (var == "GPIOAMOEN") {
    return String(espConfig::miscConfig.gpioActionMomentaryEnabled);
  }
  else if (var == "GPIOAMOTIME") {
    return String(espConfig::miscConfig.gpioActionMomentaryTimeout);
  }
  else if (var == "NEOSTIME") {
    return String(espConfig::miscConfig.neopixelSuccessTime);
  }
  else if (var == "NEOFTIME") {
    return String(espConfig::miscConfig.neopixelFailTime);
  }
  else if (var == "SREDPIXELVAL") {
    return String(espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::R]);
  }
  else if (var == "SGREENPIXELVAL") {
    return String(espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::G]);
  }
  else if (var == "SBLUEPIXELVAL") {
    return String(espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::B]);
  }
  else if (var == "FREDPIXELVAL") {
    return String(espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::R]);
  }
  else if (var == "FGREENPIXELVAL") {
    return String(espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::G]);
  }
  else if (var == "FBLUEPIXELVAL") {
    return String(espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::B]);
  }
  return "";
}
bool headersFix(AsyncWebServerRequest* request) { request->addInterestingHeader("ANY"); return true; };
void setupWeb() {
  auto infoHandle = new AsyncStaticWebHandler("/info", LittleFS, "/info.html", NULL);
  webServer.addHandler(infoHandle);
  infoHandle->setTemplateProcessor(hkInfoHtmlProcess).setFilter(headersFix);
  auto mqttHandle = new AsyncStaticWebHandler("/mqtt", LittleFS, "/mqtt.html", NULL);
  webServer.addHandler(mqttHandle);
  mqttHandle->setTemplateProcessor(mqttHtmlProcess).setFilter(headersFix);
  auto miscHandle = new AsyncStaticWebHandler("/misc", LittleFS, "/misc.html", NULL);
  webServer.addHandler(miscHandle);
  miscHandle->setTemplateProcessor(miscHtmlProcess).setFilter(headersFix);
  auto actionsHandle = new AsyncStaticWebHandler("/actions", LittleFS, "/actions.html", NULL);
  webServer.addHandler(actionsHandle);
  actionsHandle->setTemplateProcessor(actionsProcess).setFilter(headersFix);
  auto assetsHandle = new AsyncStaticWebHandler("/assets", LittleFS, "/assets/", NULL);
  webServer.addHandler(assetsHandle);
  actionsHandle->setFilter(headersFix);
  AsyncStaticWebHandler* rootHandle = new AsyncStaticWebHandler("/", LittleFS, "/", NULL);
  webServer.addHandler(rootHandle);
  rootHandle->setDefaultFile("index.html").setTemplateProcessor(indexProcess).setFilter(headersFix);
  auto mqttConfigHandle = new AsyncCallbackWebHandler();
  mqttConfigHandle->setUri("/mqttconfig");
  mqttConfigHandle->setMethod(HTTP_POST);
  mqttConfigHandle->onRequest([](AsyncWebServerRequest* request) {
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
      else if (!strcmp(p->name().c_str(), "mqtt-lwt-topic")) {
        espConfig::mqttData.lwtTopic = p->value().c_str();
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
      else if (!strcmp(p->name().c_str(), "caction-unlock")) {
        espConfig::mqttData.customLockActions["UNLOCK"] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "caction-lock")) {
        espConfig::mqttData.customLockActions["LOCK"] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "cstate-unlocking")) {
        espConfig::mqttData.customLockStates["C_UNLOCKING"] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "cstate-locking")) {
        espConfig::mqttData.customLockStates["C_LOCKING"] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "cstate-locked")) {
        espConfig::mqttData.customLockStates["C_LOCKED"] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "cstate-unlocked")) {
        espConfig::mqttData.customLockStates["C_UNLOCKED"] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "cstate-jammed")) {
        espConfig::mqttData.customLockStates["C_JAMMED"] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "cstate-unknown")) {
        espConfig::mqttData.customLockStates["C_UNKNOWN"] = p->value().toInt();
      }
    }
    json json_mqtt_config = espConfig::mqttData;
    std::vector<uint8_t> string_mqtt = json::to_msgpack(json_mqtt_config);
    esp_err_t set_nvs = nvs_set_blob(savedData, "MQTTDATA", string_mqtt.data(), string_mqtt.size());
    esp_err_t commit_nvs = nvs_commit(savedData);
    LOG(V, "SET_STATUS: %s", esp_err_to_name(set_nvs));
    LOG(V, "COMMIT_STATUS: %s", esp_err_to_name(commit_nvs));

    request->send(200, "text/plain", "Config Saved, Restarting...");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP.restart();
    });
  webServer.addHandler(mqttConfigHandle);
  auto miscConfigHandle = new AsyncCallbackWebHandler();
  miscConfigHandle->setUri("/misc-config");
  miscConfigHandle->setMethod(HTTP_POST);
  miscConfigHandle->onRequest([](AsyncWebServerRequest* request) {
    const char* TAG = "misc-config";
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      LOG(V, "POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      if (!strcmp(p->name().c_str(), "device-name")) {
        espConfig::miscConfig.deviceName = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "ota-passwd")) {
        espConfig::miscConfig.otaPasswd = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "hk-setupcode")) {
        if (strcmp(espConfig::miscConfig.setupCode.c_str(), p->value().c_str()) && p->value().length() == 8) {
          if (homeSpan.controllerListBegin() == homeSpan.controllerListEnd()) {
            homeSpan.setPairingCode(p->value().c_str());
            espConfig::miscConfig.setupCode = p->value().c_str();
          }
        }
      }
      else if (!strcmp(p->name().c_str(), "control-pin")) {
        espConfig::miscConfig.controlPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "led-pin")) {
        espConfig::miscConfig.hsStatusPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "hk-always-unlock")) {
        espConfig::miscConfig.lockAlwaysUnlock = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "hk-always-lock")) {
        espConfig::miscConfig.lockAlwaysLock = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "hk-hwfinish")) {
        espConfig::miscConfig.hk_key_color = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "web-auth-enable")) {
        espConfig::miscConfig.webAuthEnabled = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "web-auth-username")) {
        espConfig::miscConfig.webUsername = p->value().c_str();
      }
      else if (!strcmp(p->name().c_str(), "web-auth-password")) {
        espConfig::miscConfig.webPassword = p->value().c_str();
      }
    }
    json json_misc_config = espConfig::miscConfig;
    std::vector<uint8_t> misc_buf = nlohmann::json::to_msgpack(json_misc_config);
    esp_err_t set_nvs = nvs_set_blob(savedData, "MISCDATA", misc_buf.data(), misc_buf.size());
    esp_err_t commit_nvs = nvs_commit(savedData);
    LOG(V, "SET_STATUS: %s", esp_err_to_name(set_nvs));
    LOG(V, "COMMIT_STATUS: %s", esp_err_to_name(commit_nvs));

    request->send(200, "text/plain", "Config Saved, Restarting...");
    delay(1000);
    ESP.restart();
    });
  webServer.addHandler(miscConfigHandle);
  auto actionsConfigHandle = new AsyncCallbackWebHandler();
  actionsConfigHandle->setUri("/actions-config");
  actionsConfigHandle->setMethod(HTTP_POST);
  actionsConfigHandle->onRequest([](AsyncWebServerRequest* request) {
    const char* TAG = "actions-config";
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      LOG(V, "POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      if (!strcmp(p->name().c_str(), "nfc-neopixel-pin")) {
        if (espConfig::miscConfig.nfcNeopixelPin == 255 && p->value().toInt() != 255) {
          xTaskCreate(neopixel_task, "neopixel_task", 4096, NULL, 2, neopixel_task_handle);
          pixel = std::make_shared<Pixel>(p->value().toInt(), PixelType::GRB);
        }
        else if (espConfig::miscConfig.nfcNeopixelPin != 255 && p->value().toInt() == 255 && neopixel_task_handle != nullptr) {
          uint8_t status = 2;
          xQueueSend(neopixel_handle, &status, 0);
          vTaskDelete(neopixel_task_handle);
        }
        espConfig::miscConfig.nfcNeopixelPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "neopixel-s-time")) {
        espConfig::miscConfig.neopixelSuccessTime = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "neopixel-f-time")) {
        espConfig::miscConfig.neopixelFailTime = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-s-red-pixel")) {
        espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::R] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-s-green-pixel")) {
        espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::G] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-s-blue-pixel")) {
        espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::B] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-f-red-pixel")) {
        espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::R] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-f-green-pixel")) {
        espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::G] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-f-blue-pixel")) {
        espConfig::miscConfig.neopixelFailureColor[espConfig::misc_config_t::colorMap::B] = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-s-pin")) {
        if (espConfig::miscConfig.nfcSuccessPin == 255 && p->value().toInt() != 255 && gpio_led_task_handle == nullptr) {
          pinMode(p->value().toInt(), OUTPUT);
          xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, gpio_led_task_handle);
        }
        else if (espConfig::miscConfig.nfcSuccessPin != 255 && p->value().toInt() == 255 && gpio_led_task_handle != nullptr) {
          uint8_t status = 2;
          xQueueSend(gpio_led_handle, &status, 0);
          vTaskDelete(gpio_led_task_handle);
        }
        espConfig::miscConfig.nfcSuccessPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "nfc-f-pin")) {
        if (espConfig::miscConfig.nfcFailPin == 255 && p->value().toInt() != 255 && gpio_led_task_handle == nullptr) {
          pinMode(p->value().toInt(), OUTPUT);
          xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, gpio_led_task_handle);
        }
        else if (espConfig::miscConfig.nfcFailPin != 255 && p->value().toInt() == 255 && gpio_led_task_handle != nullptr) {
          uint8_t status = 2;
          xQueueSend(gpio_led_handle, &status, 0);
          vTaskDelete(gpio_led_task_handle);
        }
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
      else if (!strcmp(p->name().c_str(), "gpio-a-pin")) {
        if (espConfig::miscConfig.gpioActionPin == 255 && p->value().toInt() != 255) {
          pinMode(p->value().toInt(), OUTPUT);
          xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, gpio_lock_task_handle);
        }
        else if (espConfig::miscConfig.gpioActionPin != 255 && p->value().toInt() == 255 && gpio_lock_task_handle != nullptr) {
          uint8_t status = 2;
          xQueueSend(gpio_lock_handle, &status, 0);
          vTaskDelete(gpio_lock_task_handle);
        }
        espConfig::miscConfig.gpioActionPin = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "gpio-a-lock")) {
        espConfig::miscConfig.gpioActionLockState = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "gpio-a-unlock")) {
        espConfig::miscConfig.gpioActionUnlockState = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "gpio-a-momentary")) {
        espConfig::miscConfig.gpioActionMomentaryEnabled = p->value().toInt();
      }
      else if (!strcmp(p->name().c_str(), "gpio-a-mo-timeout")) {
        espConfig::miscConfig.gpioActionMomentaryTimeout = p->value().toInt();
      }
    }
    json json_misc_config = espConfig::miscConfig;
    std::vector<uint8_t> misc_buf = nlohmann::json::to_msgpack(json_misc_config);
    esp_err_t set_nvs = nvs_set_blob(savedData, "MISCDATA", misc_buf.data(), misc_buf.size());
    esp_err_t commit_nvs = nvs_commit(savedData);
    LOG(V, "SET_STATUS: %s", esp_err_to_name(set_nvs));
    LOG(V, "COMMIT_STATUS: %s", esp_err_to_name(commit_nvs));

    request->send(200, "text/plain", "Received Config!");
    });
  webServer.addHandler(actionsConfigHandle);
  if (espConfig::miscConfig.webAuthEnabled) {
    LOG(I, "Web Authentication Enabled");
    infoHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    mqttHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    miscHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    actionsHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    assetsHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    rootHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    mqttConfigHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    miscConfigHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    actionsConfigHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
  }
  webServer.onNotFound(notFound);
  webServer.begin();
}

void mqttConfigReset(const char* buf) {
  nvs_erase_key(savedData, "MQTTDATA");
  nvs_commit(savedData);
  ESP.restart();
}

void wifiCallback() {
  if (strcmp(espConfig::mqttData.mqttBroker.c_str(), "0.0.0.0")) {
    mqtt_app_start();
  }
  setupWeb();
}

void mqtt_publish(std::string topic, std::string payload, uint8_t qos, bool retain) {
  if (client != nullptr) {
    esp_mqtt_client_publish(client, topic.c_str(), payload.c_str(), payload.length(), 0, retain);
  } else LOG(W, "MQTT Client not initialized, cannot publish message");
}

std::string hex_representation(const std::vector<uint8_t>& v) {
  std::string hex_tmp;
  for (auto x : v) {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (unsigned)x;
    hex_tmp += oss.str();
  }
  return hex_tmp;
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
          if (espConfig::miscConfig.nfcSuccessPin != 255) {
            xQueueSend(gpio_led_handle, &status, 0);
          }
          if (espConfig::miscConfig.nfcNeopixelPin != 255) {
            xQueueSend(neopixel_handle, &status, 0);
          }
          if (espConfig::miscConfig.gpioActionPin != 255) {
            xQueueSend(gpio_lock_handle, &status, 0);
          }
          json payload;
          payload["issuerId"] = hex_representation(std::get<0>(authResult));
          payload["endpointId"] = hex_representation(std::get<1>(authResult));
          payload["readerId"] = hex_representation(readerData.reader_id);
          payload["homekey"] = true;
          std::string payloadStr = payload.dump();
          mqtt_publish(espConfig::mqttData.hkTopic, payloadStr, 0, false);
          if (espConfig::miscConfig.lockAlwaysUnlock) {
            lockCurrentState->setVal(lockStates::UNLOCKED);
            lockTargetState->setVal(lockStates::UNLOCKED);
            if (espConfig::miscConfig.gpioActionPin != 255 && !espConfig::miscConfig.gpioActionMomentaryEnabled) {
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
            }
            mqtt_publish(espConfig::mqttData.lockStateTopic, std::to_string(lockStates::UNLOCKED), 1, true);
            if (espConfig::mqttData.lockEnableCustomState) {
              mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]), 0, false);
            }
          }
          else if (espConfig::miscConfig.lockAlwaysLock) {
            lockCurrentState->setVal(lockStates::LOCKED);
            lockTargetState->setVal(lockStates::LOCKED);
            if (espConfig::miscConfig.gpioActionPin != 255 && !espConfig::miscConfig.gpioActionMomentaryEnabled) {
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
            }
            mqtt_publish(espConfig::mqttData.lockStateTopic, std::to_string(lockStates::LOCKED), 1, true);
            if (espConfig::mqttData.lockEnableCustomState) {
              mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["LOCK"]), 0, false);
            }
          }
          else {
            int currentState = lockCurrentState->getVal();
            if (espConfig::miscConfig.gpioActionPin != 255 && !espConfig::miscConfig.gpioActionMomentaryEnabled) {
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
                mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["LOCK"]), 0, false);
              }
              else if (currentState == lockStates::LOCKED) {
                mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]), 0, false);
              }
            }
          }

          auto stopTime = std::chrono::high_resolution_clock::now();
          LOG(I, "Total Time (detection->auth->gpio->mqtt): %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
        }
        else {
          bool status = false;
          if (espConfig::miscConfig.nfcFailPin != 255) {
            xQueueSend(gpio_led_handle, &status, 0);
          }
          if (espConfig::miscConfig.nfcNeopixelPin != 255) {
            xQueueSend(neopixel_handle, &status, 0);
          }
          LOG(W, "We got status FlowFailed, mqtt untouched!");
        }
        nfc.setRFField(0x02, 0x01);
      }
      else {
        LOG(W, "Invalid Response, probably not Homekey, publishing target's UID");
        bool status = true;
        if (espConfig::miscConfig.nfcSuccessPin != 255) {
          xQueueSend(gpio_led_handle, &status, 0);
        }
        if (espConfig::miscConfig.nfcNeopixelPin != 255) {
          xQueueSend(neopixel_handle, &status, 0);
        }
        json payload;
        payload["atqa"] = hex_representation(std::vector<uint8_t>(atqa, atqa + 2));
        payload["sak"] = hex_representation(std::vector<uint8_t>(sak, sak + 1));
        payload["uid"] = hex_representation(std::vector<uint8_t>(uid, uid + uidLen));
        payload["homekey"] = false;
        std::string payload_dump = payload.dump();
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

void setup() {
  Serial.begin(115200);
  const esp_app_desc_t* app_desc = esp_ota_get_app_description();
  std::string app_version = app_desc->version;
  gpio_led_handle = xQueueCreate(2, sizeof(uint8_t));
  neopixel_handle = xQueueCreate(2, sizeof(uint8_t));
  gpio_lock_handle = xQueueCreate(2, sizeof(uint8_t));
  size_t len;
  const char* TAG = "SETUP";
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData);
  if (!nvs_get_blob(savedData, "READERDATA", NULL, &len)) {
    std::vector<uint8_t> savedBuf(len);
    nvs_get_blob(savedData, "READERDATA", savedBuf.data(), &len);
    LOG(D, "NVS READERDATA LENGTH: %d", len);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, savedBuf.data(), savedBuf.size(), ESP_LOG_VERBOSE);
    nlohmann::json data = nlohmann::json::from_msgpack(savedBuf);
    if (!data.is_discarded()) {
      data.get_to<readerData_t>(readerData);
    }
  }
  if (!nvs_get_blob(savedData, "MQTTDATA", NULL, &len)) {
    std::vector<uint8_t> dataBuf(len);
    nvs_get_blob(savedData, "MQTTDATA", dataBuf.data(), &len);
    LOG(D, "NVS MQTTDATA LENGTH: %d", len);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, dataBuf.data(), dataBuf.size(), ESP_LOG_VERBOSE);
    auto isValidJson = nlohmann::json::accept(dataBuf);
    if (isValidJson) {
      nlohmann::json data = nlohmann::json::parse(dataBuf);
      if (!data.contains("lwtTopic") && data.contains("mqttClientId")) {
        std::string lwt = data["mqttClientId"];
        lwt.append("/status");
        data["lwtTopic"] = lwt;
      }
      if (!data.is_discarded()) {
        data.get_to<espConfig::mqttConfig_t>(espConfig::mqttData);
      }
    } else {
      nlohmann::json data = nlohmann::json::from_msgpack(dataBuf);
      if (!data.is_discarded()) {
        data.get_to<espConfig::mqttConfig_t>(espConfig::mqttData);
      }
    }
  }
  if (!nvs_get_blob(savedData, "MISCDATA", NULL, &len)) {
    std::vector<uint8_t> dataBuf(len);
    nvs_get_blob(savedData, "MISCDATA", dataBuf.data(), &len);
    std::string str(dataBuf.begin(), dataBuf.end());
    LOG(D, "NVS MQTTDATA LENGTH: %d", len);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, dataBuf.data(), dataBuf.size(), ESP_LOG_VERBOSE);
    auto isValidJson = nlohmann::json::accept(dataBuf);
    if (isValidJson) {
      nlohmann::json data = nlohmann::json::parse(str);
      if (!data.is_discarded()) {
        data.get_to<espConfig::misc_config_t>(espConfig::miscConfig);
      }
    } else {
      nlohmann::json data = nlohmann::json::from_msgpack(dataBuf);
      if (!data.is_discarded()) {
        data.get_to<espConfig::misc_config_t>(espConfig::miscConfig);
      }
    }
  }
  if (espConfig::miscConfig.nfcSuccessPin && espConfig::miscConfig.nfcSuccessPin != 255) {
    pinMode(espConfig::miscConfig.nfcSuccessPin, OUTPUT);
    digitalWrite(espConfig::miscConfig.nfcSuccessPin, !espConfig::miscConfig.nfcSuccessHL);
  }
  if (espConfig::miscConfig.nfcFailPin && espConfig::miscConfig.nfcFailPin != 255) {
    pinMode(espConfig::miscConfig.nfcFailPin, OUTPUT);
    digitalWrite(espConfig::miscConfig.nfcFailPin, !espConfig::miscConfig.nfcFailHL);
  }
  if (espConfig::miscConfig.gpioActionPin && espConfig::miscConfig.gpioActionPin != 255) {
    pinMode(espConfig::miscConfig.gpioActionPin, OUTPUT);
  }
  if (!LittleFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LITTLEFS");
    return;
  }
  listDir(LittleFS, "/", 0);
  if (espConfig::miscConfig.controlPin != 255) {
    homeSpan.setControlPin(espConfig::miscConfig.controlPin);
  }
  if (espConfig::miscConfig.hsStatusPin != 255) {
    homeSpan.setStatusPin(espConfig::miscConfig.hsStatusPin);
  }
  homeSpan.setStatusAutoOff(15);
  homeSpan.setLogLevel(0);
  homeSpan.setSketchVersion(app_version.c_str());

  LOG(I, "READER GROUP ID (%d): %s", readerData.reader_gid.size(), utils::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size()).c_str());
  LOG(I, "READER UNIQUE ID (%d): %s", readerData.reader_id.size(), utils::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size()).c_str());

  LOG(I, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto&& issuer : readerData.issuers) {
    LOG(D, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str(), utils::bufToHexString(issuer.issuer_pk.data(), issuer.issuer_pk.size()).c_str());
  }
  homeSpan.enableAutoStartAP();
  homeSpan.enableOTA(espConfig::miscConfig.otaPasswd.c_str());
  homeSpan.setPortNum(1201);
  homeSpan.begin(Category::Locks, espConfig::miscConfig.deviceName.c_str(), "HK", "HomeKey-ESP32");

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
  std::vector<uint8_t> decB64 = utils::decodeB64(hk_color_vals[HK_COLOR(espConfig::miscConfig.hk_key_color)]);
  TLV8 hwfinish(NULL, 0);
  hwfinish.unpack(decB64.data(), decB64.size());
  new Characteristic::HardwareFinish(hwfinish);

  new LockManagement();
  new LockMechanism();
  new NFCAccess();
  new Service::HAPProtocolInformation();
  new Characteristic::Version();
  homeSpan.setControllerCallback(pairCallback);
  homeSpan.setWifiCallback(wifiCallback);
  if (espConfig::miscConfig.nfcNeopixelPin != 255) {
    pixel = std::make_shared<Pixel>(espConfig::miscConfig.nfcNeopixelPin, PixelType::GRB);
    xTaskCreate(neopixel_task, "neopixel_task", 4096, NULL, 2, neopixel_task_handle);
  }
  if (espConfig::miscConfig.nfcSuccessPin != 255 || espConfig::miscConfig.nfcFailPin != 255) {
    xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, gpio_led_task_handle);
  }
  if (espConfig::miscConfig.gpioActionPin != 255) {
    xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, gpio_lock_task_handle);
  }
  xTaskCreate(nfc_thread_entry, "nfc_task", 8192, NULL, 1, NULL);
}

//////////////////////////////////////

void loop() {
  homeSpan.poll();
  vTaskDelay(5);
}
