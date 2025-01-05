#define JSON_NOEXCEPTION 1
#include <sodium/crypto_sign.h>
#include <sodium/crypto_box.h>
#include "HAP.h"
#include "hkAuthContext.h"
#include "HomeKey.h"
#include "array"
#include "logging.h"
#include "HomeSpan.h"
#include "PN532_SPI.h"
#include "PN532.h"
#include "chrono"
#include "ESPAsyncWebServer.h"
#include "LittleFS.h"
#include "HK_HomeKit.h"
#include "config.h"
#include "mqtt_client.h"
#include "esp_ota_ops.h"
#include "esp_task.h"
#include "pins_arduino.h"
#include "string_view"
#include "NFC_SERV_CHARS.h"
#include <mbedtls/sha256.h>
#include <esp_mac.h>

const char* TAG = "MAIN";

AsyncWebServer webServer(80);
PN532_SPI *pn532spi;
PN532 *nfc;
QueueHandle_t gpio_led_handle = nullptr;
QueueHandle_t neopixel_handle = nullptr;
QueueHandle_t gpio_lock_handle = nullptr;
TaskHandle_t gpio_led_task_handle = nullptr;
TaskHandle_t neopixel_task_handle = nullptr;
TaskHandle_t gpio_lock_task_handle = nullptr;
TaskHandle_t alt_action_task_handle = nullptr;
TaskHandle_t nfc_reconnect_task = nullptr;
TaskHandle_t nfc_poll_task = nullptr;

nvs_handle savedData;
readerData_t readerData;
uint8_t ecpData[18] = { 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 };
const std::array<std::array<uint8_t, 6>, 4> hk_color_vals = { {{0x01,0x04,0xce,0xd5,0xda,0x00}, {0x01,0x04,0xaa,0xd6,0xec,0x00}, {0x01,0x04,0xe3,0xe3,0xe3,0x00}, {0x01,0x04,0x00,0x00,0x00,0x00}} };
const std::array<const char*, 6> pixelTypeMap = { "RGB", "RBG", "BRG", "BGR", "GBR", "GRB" };
struct gpioLockAction
{
  enum
  {
    HOMEKIT = 1,
    HOMEKEY = 2,
    OTHER = 3
  };
  uint8_t source;
  uint8_t action;
};

std::string platform_create_id_string(void) {
  uint8_t mac[6];
  char id_string[32];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  sprintf(id_string, "ESP32_%02x%02X%02X", mac[3], mac[4], mac[5]);
  return std::string();
}

namespace espConfig
{
  struct mqttConfig_t
  {
    mqttConfig_t() {
      std::string id = platform_create_id_string();
      mqttClientId = id;
      lwtTopic.append(id).append("/" MQTT_LWT_TOPIC);
      hkTopic.append(id).append("/" MQTT_AUTH_TOPIC);
      lockStateTopic.append(id).append("/" MQTT_STATE_TOPIC);
      lockStateCmd.append(id).append("/" MQTT_SET_STATE_TOPIC);
      lockCStateCmd.append(id).append("/" MQTT_SET_CURRENT_STATE_TOPIC);
      lockTStateCmd.append(id).append("/" MQTT_SET_TARGET_STATE_TOPIC);
      lockCustomStateTopic.append(id).append("/" MQTT_CUSTOM_STATE_TOPIC);
      lockCustomStateCmd.append(id).append("/" MQTT_CUSTOM_STATE_CTRL_TOPIC);
      btrLvlCmdTopic.append(id).append("/" MQTT_PROX_BAT_TOPIC);
      hkAltActionTopic.append(id).append("/" MQTT_HK_ALT_ACTION_TOPIC);
    }
    /* MQTT Broker */
    std::string mqttBroker = MQTT_HOST;
    uint16_t mqttPort = MQTT_PORT;
    std::string mqttUsername = MQTT_USERNAME;
    std::string mqttPassword = MQTT_PASSWORD;
    std::string mqttClientId;
    /* MQTT Topics */
    std::string lwtTopic;
    std::string hkTopic;
    std::string lockStateTopic;
    std::string lockStateCmd;
    std::string lockCStateCmd;
    std::string lockTStateCmd;
    std::string btrLvlCmdTopic;
    std::string hkAltActionTopic;
    /* MQTT Custom State */
    std::string lockCustomStateTopic;
    std::string lockCustomStateCmd;
    /* Flags */
    bool lockEnableCustomState = MQTT_CUSTOM_STATE_ENABLED;
    bool hassMqttDiscoveryEnabled = MQTT_DISCOVERY;
    bool nfcTagNoPublish = false;
    std::map<std::string, int> customLockStates = { {"C_LOCKED", C_LOCKED}, {"C_UNLOCKING", C_UNLOCKING}, {"C_UNLOCKED", C_UNLOCKED}, {"C_LOCKING", C_LOCKING}, {"C_JAMMED", C_JAMMED}, {"C_UNKNOWN", C_UNKNOWN} };
    std::map<std::string, int> customLockActions = { {"UNLOCK", UNLOCK}, {"LOCK", LOCK} };
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(espConfig::mqttConfig_t, mqttBroker, mqttPort, mqttUsername, mqttPassword, mqttClientId, lwtTopic, hkTopic, lockStateTopic,
      lockStateCmd, lockCStateCmd, lockTStateCmd, lockCustomStateTopic, lockCustomStateCmd, lockEnableCustomState, hassMqttDiscoveryEnabled, customLockStates, customLockActions,
      nfcTagNoPublish, btrLvlCmdTopic, hkAltActionTopic)
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
    uint8_t neoPixelType = 5;
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
    uint8_t gpioActionMomentaryEnabled = GPIO_ACTION_MOMENTARY_STATE;
    bool hkGpioControlledState = true;
    uint16_t gpioActionMomentaryTimeout = GPIO_ACTION_MOMENTARY_TIMEOUT;
    bool webAuthEnabled = WEB_AUTH_ENABLED;
    std::string webUsername = WEB_AUTH_USERNAME;
    std::string webPassword = WEB_AUTH_PASSWORD;
    std::array<uint8_t, 4> nfcGpioPins{SS, SCK, MISO, MOSI};
    uint8_t btrLowStatusThreshold = 10;
    bool proxBatEnabled = false;
    bool hkDumbSwitchMode = false;
    uint8_t hkAltActionInitPin = GPIO_HK_ALT_ACTION_INIT_PIN;
    uint8_t hkAltActionInitLedPin = GPIO_HK_ALT_ACTION_INIT_LED_PIN;
    uint16_t hkAltActionInitTimeout = GPIO_HK_ALT_ACTION_INIT_TIMEOUT;
    uint8_t hkAltActionPin = GPIO_HK_ALT_ACTION_PIN;
    uint16_t hkAltActionTimeout = GPIO_HK_ALT_ACTION_TIMEOUT;
    uint8_t hkAltActionGpioState = GPIO_HK_ALT_ACTION_GPIO_STATE;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(misc_config_t, deviceName, otaPasswd, hk_key_color, setupCode, lockAlwaysUnlock, lockAlwaysLock, controlPin, hsStatusPin,
      nfcSuccessPin, nfcSuccessTime, nfcNeopixelPin, neoPixelType, neopixelSuccessColor, neopixelFailureColor, neopixelSuccessTime, neopixelFailTime, nfcSuccessHL, nfcFailPin,
      nfcFailTime, nfcFailHL, gpioActionPin, gpioActionLockState, gpioActionUnlockState, gpioActionMomentaryEnabled, gpioActionMomentaryTimeout, webAuthEnabled, webUsername,
      webPassword, nfcGpioPins, btrLowStatusThreshold, proxBatEnabled, hkDumbSwitchMode, hkAltActionInitPin, hkAltActionInitLedPin, hkAltActionInitTimeout, hkAltActionPin,
      hkAltActionTimeout, hkAltActionGpioState, hkGpioControlledState)
  } miscConfig;
};

KeyFlow hkFlow = KeyFlow::kFlowFAST;
bool hkAltActionActive = false;
SpanCharacteristic* lockCurrentState;
SpanCharacteristic* lockTargetState;
SpanCharacteristic* statusLowBtr;
SpanCharacteristic* btrLevel;
esp_mqtt_client_handle_t client = nullptr;

std::unique_ptr<Pixel> pixel;

bool save_to_nvs() {
  std::vector<uint8_t> serialized = nlohmann::json::to_msgpack(readerData);
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", serialized.data(), serialized.size());
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG(D, "NVS SET STATUS: %s", esp_err_to_name(set_nvs));
  LOG(D, "NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
  return !set_nvs && !commit_nvs;
}

struct PhysicalLockBattery : Service::BatteryService
{
  PhysicalLockBattery() {
    LOG(I, "Configuring PhysicalLockBattery");
    statusLowBtr = new Characteristic::StatusLowBattery(0, true);
    btrLevel = new Characteristic::BatteryLevel(100, true);
  }
};

struct LockManagement : Service::LockManagement
{
  SpanCharacteristic* lockControlPoint;
  SpanCharacteristic* version;
  const char* TAG = "LockManagement";

  LockManagement() : Service::LockManagement() {

    LOG(I, "Configuring LockManagement"); // initialization message

    lockControlPoint = new Characteristic::LockControlPoint();
    version = new Characteristic::Version();

  } // end constructor

}; // end LockManagement

struct NFCAccessoryInformation : Service::AccessoryInformation
{
  const char* TAG = "NFCAccessoryInformation";

  NFCAccessoryInformation() : Service::AccessoryInformation() {

    LOG(I, "Configuring NFCAccessoryInformation"); // initialization message

    opt.push_back(&_CUSTOM_HardwareFinish);
    new Characteristic::Identify();
    new Characteristic::Manufacturer("rednblkx");
    new Characteristic::Model("HomeKey-ESP32");
    new Characteristic::Name(DEVICE_NAME);
    const esp_app_desc_t* app_desc = esp_ota_get_app_description();
    std::string app_version = app_desc->version;
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char macStr[18] = { 0 };
    sprintf(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
    std::string serialNumber = "HK-";
    serialNumber.append(macStr);
    new Characteristic::SerialNumber(serialNumber.c_str());
    new Characteristic::FirmwareRevision(app_version.c_str());
    std::array<uint8_t, 6> decB64 = hk_color_vals[HK_COLOR(espConfig::miscConfig.hk_key_color)];
    TLV8 hwfinish(NULL, 0);
    hwfinish.unpack(decB64.data(), decB64.size());
    new Characteristic::HardwareFinish(hwfinish);

  } // end constructor
};

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

void alt_action_task(void* arg) {
  uint8_t buttonState = 0;
  hkAltActionActive = false;
  LOG(I, "Starting Alt Action button task");
  while (true)
  {
    buttonState = digitalRead(espConfig::miscConfig.hkAltActionInitPin);
    if (buttonState == HIGH) {
      LOG(D, "BUTTON HIGH");
      hkAltActionActive = true;
      if(espConfig::miscConfig.hkAltActionInitLedPin != 255) {
        digitalWrite(espConfig::miscConfig.hkAltActionInitLedPin, HIGH);
      }
      vTaskDelay(espConfig::miscConfig.hkAltActionInitTimeout / portTICK_PERIOD_MS);
      if (espConfig::miscConfig.hkAltActionInitLedPin != 255) {
        digitalWrite(espConfig::miscConfig.hkAltActionInitLedPin, LOW);
      }
      LOG(D, "TIMEOUT");
      hkAltActionActive = false;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void gpio_task(void* arg) {
  gpioLockAction status;
  while (1) {
    if (gpio_lock_handle != nullptr) {
      status = {};
      if (uxQueueMessagesWaiting(gpio_lock_handle) > 0) {
        xQueueReceive(gpio_lock_handle, &status, 0);
        LOG(D, "Got something in queue - source = %d action = %d", status.source, status.action);
        if (status.action == 0) {
          LOG(D, "%d - %d - %d -%d", espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionMomentaryEnabled, espConfig::miscConfig.lockAlwaysUnlock, espConfig::miscConfig.lockAlwaysLock);
          if (espConfig::miscConfig.lockAlwaysUnlock && status.source != gpioLockAction::HOMEKIT) {
            lockTargetState->setVal(lockStates::UNLOCKED);
            digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
            lockCurrentState->setVal(lockStates::UNLOCKED);
            if (client != nullptr) {
              esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 1, 0, false);
            } else LOG(W, "MQTT Client not initialized, cannot publish message");

            if (static_cast<uint8_t>(espConfig::miscConfig.gpioActionMomentaryEnabled) & status.source) {
              delay(espConfig::miscConfig.gpioActionMomentaryTimeout);
              lockTargetState->setVal(lockStates::LOCKED);
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
              lockCurrentState->setVal(lockStates::LOCKED);
              if (client != nullptr) {
                esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 1, 0, false);
              } else LOG(W, "MQTT Client not initialized, cannot publish message");
            }
          } else if (espConfig::miscConfig.lockAlwaysLock && status.source != gpioLockAction::HOMEKIT) {
            lockTargetState->setVal(lockStates::LOCKED);
            digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
            lockCurrentState->setVal(lockStates::LOCKED);
            if (client != nullptr) {
              esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 1, 0, false);
            } else LOG(W, "MQTT Client not initialized, cannot publish message");
          } else {
            int currentState = lockCurrentState->getVal();
            if (status.source != gpioLockAction::HOMEKIT) {
              lockTargetState->setVal(!currentState);
            }
            digitalWrite(espConfig::miscConfig.gpioActionPin, currentState == lockStates::UNLOCKED ? espConfig::miscConfig.gpioActionLockState : espConfig::miscConfig.gpioActionUnlockState);
            lockCurrentState->setVal(!currentState);
            if (client != nullptr) {
              esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockCurrentState->getNewVal()).c_str(), 1, 0, false);
            } else LOG(W, "MQTT Client not initialized, cannot publish message");
            if ((static_cast<uint8_t>(espConfig::miscConfig.gpioActionMomentaryEnabled) & status.source) && currentState == lockStates::LOCKED) {
              delay(espConfig::miscConfig.gpioActionMomentaryTimeout);
              lockTargetState->setVal(currentState);
              digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
              lockCurrentState->setVal(currentState);
              if (client != nullptr) {
                esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockCurrentState->getNewVal()).c_str(), 1, 0, false);
              } else LOG(W, "MQTT Client not initialized, cannot publish message");
            }
          }
        } else if (status.action == 2) {
          vTaskDelete(NULL);
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
          vTaskDelete(NULL);
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
        case 2:
          if(hkAltActionActive){
            digitalWrite(espConfig::miscConfig.hkAltActionPin, espConfig::miscConfig.hkAltActionGpioState);
            delay(espConfig::miscConfig.hkAltActionTimeout);
            digitalWrite(espConfig::miscConfig.hkAltActionPin, !espConfig::miscConfig.hkAltActionGpioState);
          }
        default:
          LOG(I, "STOP");
          vTaskDelete(NULL);
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
      } else if (lockCurrentState->getVal() == lockStates::UNLOCKED) {
        digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
      }
    }
  } // end constructor

  boolean update() {
    int targetState = lockTargetState->getNewVal();
    LOG(I, "New LockState=%d, Current LockState=%d", targetState, lockCurrentState->getVal());
    if (espConfig::miscConfig.gpioActionPin != 255) {
      const gpioLockAction gpioAction{ .source = gpioLockAction::HOMEKIT, .action = 0 };
      xQueueSend(gpio_lock_handle, &gpioAction, 0);
    } else if (espConfig::miscConfig.hkDumbSwitchMode) {
      lockCurrentState->setVal(targetState);
    }
    int currentState = lockCurrentState->getNewVal();
    if (client != nullptr) {
      if (espConfig::miscConfig.gpioActionPin == 255) {
        if (targetState != currentState) {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), targetState == lockStates::UNLOCKED ? std::to_string(lockStates::UNLOCKING).c_str() : std::to_string(lockStates::LOCKING).c_str(), 1, 1, true);
        } else {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(currentState).c_str(), 1, 1, true);
        }
      }
      if (espConfig::mqttData.lockEnableCustomState) {
        if (targetState == lockStates::UNLOCKED) {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]).c_str(), 0, 0, false);
        } else if (targetState == lockStates::LOCKED) {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["LOCK"]).c_str(), 0, 0, false);
        }
      }
    } else LOG(W, "MQTT Client not initialized, cannot publish message");

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
    LOG(D, "PROVISIONED READER KEY: %s", red_log::bufToHexString(readerData.reader_pk.data(), readerData.reader_pk.size()).c_str());
    LOG(D, "READER GROUP IDENTIFIER: %s", red_log::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size()).c_str());
    LOG(D, "READER UNIQUE IDENTIFIER: %s", red_log::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size()).c_str());

    TLV8 ctrlData(NULL, 0);
    nfcControlPoint->getNewTLV(ctrlData);
    std::vector<uint8_t> tlvData(ctrlData.pack_size());
    ctrlData.pack(tlvData.data());
    if (tlvData.size() == 0)
      return false;
    LOG(D, "Decoded data: %s", red_log::bufToHexString(tlvData.data(), tlvData.size()).c_str());
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

void deleteReaderData(const char* buf = "") {
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

std::vector<uint8_t> getHashIdentifier(const uint8_t* key, size_t len) {
  const char* TAG = "getHashIdentifier";
  LOG(V, "Key: %s, Length: %d", red_log::bufToHexString(key, len).c_str(), len);
  std::vector<unsigned char> hashable;
  std::string string = "key-identifier";
  hashable.insert(hashable.begin(), string.begin(), string.end());
  hashable.insert(hashable.end(), key, key + len);
  LOG(V, "Hashable: %s", red_log::bufToHexString(&hashable.front(), hashable.size()).c_str());
  uint8_t hash[32];
  mbedtls_sha256(&hashable.front(), hashable.size(), hash, 0);
  LOG(V, "HashIdentifier: %s", red_log::bufToHexString(hash, 8).c_str());
  return std::vector<uint8_t>{hash, hash + 8};
}

void pairCallback() {
  if (HAPClient::nAdminControllers() == 0) {
    deleteReaderData(NULL);
    return;
  }
  for (auto it = homeSpan.controllerListBegin(); it != homeSpan.controllerListEnd(); ++it) {
    std::vector<uint8_t> id = getHashIdentifier(it->getLTPK(), 32);
    LOG(D, "Found allocated controller - Hash: %s", red_log::bufToHexString(id.data(), 8).c_str());
    hkIssuer_t* foundIssuer = nullptr;
    for (auto&& issuer : readerData.issuers) {
      if (std::equal(issuer.issuer_id.begin(), issuer.issuer_id.end(), id.begin())) {
        LOG(D, "Issuer %s already added, skipping", red_log::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str());
        foundIssuer = &issuer;
        break;
      }
    }
    if (foundIssuer == nullptr) {
      LOG(D, "Adding new issuer - ID: %s", red_log::bufToHexString(id.data(), 8).c_str());
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
  } else if (strncmp(buf + 1, "W", 1) == 0) {
    level = ESP_LOG_WARN;
    Serial.println("WARNING");
  } else if (strncmp(buf + 1, "I", 1) == 0) {
    level = ESP_LOG_INFO;
    Serial.println("INFO");
  } else if (strncmp(buf + 1, "D", 1) == 0) {
    level = ESP_LOG_DEBUG;
    Serial.println("DEBUG");
  } else if (strncmp(buf + 1, "V", 1) == 0) {
    level = ESP_LOG_VERBOSE;
    Serial.println("VERBOSE");
  } else if (strncmp(buf + 1, "N", 1) == 0) {
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
  esp_log_level_set("actions-config", level);
  esp_log_level_set("misc-config", level);
  esp_log_level_set("mqttconfig", level);
}

void print_issuers(const char* buf) {
  for (auto&& issuer : readerData.issuers) {
    LOG(I, "Issuer ID: %s, Public Key: %s", red_log::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str(), red_log::bufToHexString(issuer.issuer_pk.data(), issuer.issuer_pk.size()).c_str());
    for (auto&& endpoint : issuer.endpoints) {
      LOG(I, "Endpoint ID: %s, Public Key: %s", red_log::bufToHexString(endpoint.endpoint_id.data(), endpoint.endpoint_id.size()).c_str(), red_log::bufToHexString(endpoint.endpoint_pk.data(), endpoint.endpoint_pk.size()).c_str());
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
  } else if (espConfig::mqttData.customLockStates["C_LOCKING"] == state) {
    lockTargetState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_UNLOCKED"] == state) {
    if (espConfig::miscConfig.gpioActionPin != 255) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionUnlockState);
    }
    lockCurrentState->setVal(lockStates::UNLOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_LOCKED"] == state) {
    if (espConfig::miscConfig.gpioActionPin != 255) {
      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
    }
    lockCurrentState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_JAMMED"] == state) {
    lockCurrentState->setVal(lockStates::JAMMED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::JAMMED).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_UNKNOWN"] == state) {
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
  const esp_app_desc_t* app_desc = esp_app_get_description();
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
    if (!espConfig::mqttData.nfcTagNoPublish) {
      esp_mqtt_client_publish(client, rfidTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    }
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
  if (espConfig::miscConfig.proxBatEnabled) {
    esp_mqtt_client_subscribe(client, espConfig::mqttData.btrLvlCmdTopic.c_str(), 0);
  }
  esp_mqtt_client_subscribe(client, espConfig::mqttData.lockStateCmd.c_str(), 0);
  esp_mqtt_client_subscribe(client, espConfig::mqttData.lockCStateCmd.c_str(), 0);
  esp_mqtt_client_subscribe(client, espConfig::mqttData.lockTStateCmd.c_str(), 0);
}

void mqtt_data_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  // ESP_LOGD(TAG, "Event dispatched from callback type=%d", event_id);
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  esp_mqtt_client_handle_t client = event->client;
  std::string topic(event->topic, event->topic + event->topic_len);
  std::string data(event->data, event->data + event->data_len);
  LOG(D, "Received message in topic \"%s\": %s", topic.c_str(), data.c_str());
  int state = atoi(data.c_str());
  if (!strcmp(espConfig::mqttData.lockCustomStateCmd.c_str(), topic.c_str())) {
    set_custom_state_handler(client, state);
  } else if (!strcmp(espConfig::mqttData.lockStateCmd.c_str(), topic.c_str())) {
    set_state_handler(client, state);
  } else if (!strcmp(espConfig::mqttData.lockTStateCmd.c_str(), topic.c_str())) {
    if (state == lockStates::UNLOCKED || state == lockStates::LOCKED) {
      lockTargetState->setVal(state);
      esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), state == lockStates::UNLOCKED ? std::to_string(lockStates::UNLOCKING).c_str() : std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
    }
  } else if (!strcmp(espConfig::mqttData.lockCStateCmd.c_str(), topic.c_str())) {
    if (state == lockStates::UNLOCKED || state == lockStates::LOCKED || state == lockStates::JAMMED || state == lockStates::UNKNOWN) {
      lockCurrentState->setVal(state);
      esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockCurrentState->getVal()).c_str(), 0, 1, true);
    }
  } else if (!strcmp(espConfig::mqttData.btrLvlCmdTopic.c_str(), topic.c_str())) {
    btrLevel->setVal(state);
    if (state <= espConfig::miscConfig.btrLowStatusThreshold) {
      statusLowBtr->setVal(1);
    } else {
      statusLowBtr->setVal(0);
    }
  }
}

/**
 * The function `mqtt_app_start` initializes and starts an MQTT client with specified configuration
 * parameters.
 */
static void mqtt_app_start(void) {
  esp_mqtt_client_config_t mqtt_cfg;
  mqtt_cfg.broker.address.hostname = espConfig::mqttData.mqttBroker.c_str();
  mqtt_cfg.broker.address.port = espConfig::mqttData.mqttPort;
  mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
  mqtt_cfg.credentials.client_id = espConfig::mqttData.mqttClientId.c_str();
  mqtt_cfg.credentials.username = espConfig::mqttData.mqttUsername.c_str();
  mqtt_cfg.credentials.authentication.password = espConfig::mqttData.mqttPassword.c_str();
  mqtt_cfg.session.last_will.topic = espConfig::mqttData.lwtTopic.c_str();
  mqtt_cfg.session.last_will.msg = "offline";
  mqtt_cfg.session.last_will.msg_len = 7;
  mqtt_cfg.session.last_will.retain = true;
  mqtt_cfg.session.last_will.qos = 1;
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
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

String indexProcess(const String& var) {
  if (var == "VERSION") {
    const esp_app_desc_t* app_desc = esp_ota_get_app_description();
    std::string app_version = app_desc->version;
    return String(app_version.c_str());
  }
  return "";
}

bool headersFix(AsyncWebServerRequest* request) { request->addInterestingHeader("ANY"); return true; };
void setupWeb() {
  auto assetsHandle = new AsyncStaticWebHandler("/assets", LittleFS, "/assets/", NULL);
  assetsHandle->setFilter(headersFix);
  webServer.addHandler(assetsHandle);
  auto routesHandle = new AsyncStaticWebHandler("/fragment", LittleFS, "/routes", NULL);
  routesHandle->setFilter(headersFix);
  webServer.addHandler(routesHandle);
  AsyncCallbackWebHandler* dataProvision = new AsyncCallbackWebHandler();
  webServer.addHandler(dataProvision);
  dataProvision->setUri("/config");
  dataProvision->setMethod(HTTP_GET);
  dataProvision->onRequest([](AsyncWebServerRequest* req) {
    if (req->hasParam("type")) {
    json serializedData;
      AsyncWebParameter* data = req->getParam(0);
      std::array<std::string, 4> pages = {"mqtt", "actions", "misc", "hkinfo"};
      if (std::equal(data->value().begin(), data->value().end(), pages[0].begin(), pages[0].end())) {
        LOG(D, "MQTT CONFIG REQ");
        serializedData = espConfig::mqttData;
      } else if (std::equal(data->value().begin(), data->value().end(),pages[1].begin(), pages[1].end()) || std::equal(data->value().begin(), data->value().end(),pages[2].begin(), pages[2].end())) {
        LOG(D, "ACTIONS CONFIG REQ");
        serializedData = espConfig::miscConfig;
      } else if (std::equal(data->value().begin(), data->value().end(),pages[3].begin(), pages[3].end())) {
        LOG(D, "HK DATA REQ");
        json inputData = readerData;
        if (inputData.contains("group_identifier")) {
          serializedData["group_identifier"] = red_log::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size(), true);
        }
        if (inputData.contains("unique_identifier")) {
          serializedData["unique_identifier"] = red_log::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size(), true);
        }
        if (inputData.contains("issuers")) {
          serializedData["issuers"] = json::array();
          for (auto it = inputData.at("issuers").begin(); it != inputData.at("issuers").end(); ++it)
          {
            json issuer;
            if (it.value().contains("issuerId")) {
              std::vector<uint8_t> id = it.value().at("issuerId").get<std::vector<uint8_t>>();
              issuer["issuerId"] = red_log::bufToHexString(id.data(), id.size(), true);
            }
            if (it.value().contains("endpoints") && it.value().at("endpoints").size() > 0) {
              issuer["endpoints"] = json::array();
              for (auto it2 = it.value().at("endpoints").begin(); it2 != it.value().at("endpoints").end(); ++it2) {
                json endpoint;
                if (it2.value().contains("endpointId")) {
                  std::vector<uint8_t> id = it2.value().at("endpointId").get<std::vector<uint8_t>>();
                  endpoint["endpointId"] = red_log::bufToHexString(id.data(), id.size(), true);
                }
                issuer["endpoints"].push_back(endpoint);
              }
            }
            serializedData["issuers"].push_back(issuer);
          }
        }
      } else {
        req->send(400);
        return;
      }
      if (!serializedData.empty()) {
        req->send(200, "application/json", serializedData.dump().c_str());
      } else {
        req->send(500);
      }
    } else req->send(500);
  });
  AsyncCallbackWebHandler* dataClear = new AsyncCallbackWebHandler();
  webServer.addHandler(dataClear);
  dataClear->setUri("/config/clear");
  dataClear->setMethod(HTTP_POST);
  dataClear->onRequest([](AsyncWebServerRequest* req) {
    if (req->hasParam("type")) {
      AsyncWebParameter* data = req->getParam(0);
      std::array<std::string, 3> pages = { "mqtt", "actions", "misc" };
      if (std::equal(data->value().begin(), data->value().end(), pages[0].begin(), pages[0].end())) {
        LOG(D, "MQTT CONFIG SEL");
        nvs_erase_key(savedData, "MQTTDATA");
        espConfig::mqttData = {};
        req->send(200, "text/plain", "200 Success");
      } else if (std::equal(data->value().begin(), data->value().end(), pages[1].begin(), pages[1].end())) {
        LOG(D, "ACTIONS CONFIG SEL");
        nvs_erase_key(savedData, "MISCDATA");
        espConfig::miscConfig = {};
        req->send(200, "text/plain", "200 Success");
      } else if (std::equal(data->value().begin(), data->value().end(), pages[2].begin(), pages[2].end())) {
        LOG(D, "MISC CONFIG SEL");
        nvs_erase_key(savedData, "MISCDATA");
        espConfig::miscConfig = {};
        req->send(200, "text/plain", "200 Success");
      } else {
        req->send(400);
        return;
      }
    } else {
        req->send(400);
        return;
      }
  });
  AsyncCallbackWebHandler* dataLoad = new AsyncCallbackWebHandler();
  webServer.addHandler(dataLoad);
  dataLoad->setUri("/config/save");
  dataLoad->setMethod(HTTP_POST);
  dataLoad->onBody([](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
    json *dataJson = new json(json::parse(data, data + len));
    if (!dataJson->is_discarded()) {
      LOG(I, "%s", dataJson->dump().c_str());
      request->_tempObject = dataJson;
    }
  });
  dataLoad->onRequest([=](AsyncWebServerRequest* req) {
    json *serializedData = static_cast<json*>(req->_tempObject);
    if (req->hasParam("type") && serializedData) {
      AsyncWebParameter* data = req->getParam(0);
      json configData;
      std::array<std::string, 3> pages = { "mqtt", "actions", "misc" };
      uint8_t selConfig;
      if (std::equal(data->value().begin(), data->value().end(), pages[0].begin(), pages[0].end())) {
        LOG(D, "MQTT CONFIG SEL");
        configData = espConfig::mqttData;
        selConfig = 0;
      } else if (std::equal(data->value().begin(), data->value().end(), pages[1].begin(), pages[1].end())) {
        LOG(D, "ACTIONS CONFIG SEL");
        configData = espConfig::miscConfig;
        selConfig = 1;
      } else if (std::equal(data->value().begin(), data->value().end(), pages[2].begin(), pages[2].end())) {
        LOG(D, "MISC CONFIG SEL");
        configData = espConfig::miscConfig;
        selConfig = 2;
      } else {
        req->send(400);
        return;
      }
      uint8_t propertiesProcessed = 0;
      for (auto it = serializedData->begin(); it != serializedData->end(); ++it) {
        if (configData.contains(it.key()) && ((configData.at(it.key()).type() == it.value().type()) || configData.at(it.key()).is_boolean())) {
          if (it.key() == std::string("setupCode")) {
            std::string code = it.value().template get<std::string>();
            if (it.value().is_string() && (!code.empty() && std::find_if(code.begin(), code.end(), [](unsigned char c) { return !std::isdigit(c); }) == code.end()) && it.value().template get<std::string>().length() == 8) {
              if (homeSpan.controllerListBegin() != homeSpan.controllerListEnd() && code.compare(configData.at(it.key()).template get<std::string>())) {
                LOG(E, "The Setup Code can only be set if no devices are paired, reset if any issues!");
                req->send(400, "text/plain", "The Setup Code can only be set if no devices are paired, reset if any issues!");
                break;
              }
            } else {
              LOG(E, "\"%s\" could not validate!", it.key().c_str());
              std::string msg = "\"\" is not a valid value for \"\"";
              msg.insert(1, it.value().dump().c_str()).insert(msg.length() - 1, it.key());
              req->send(400, "text/plain", msg.c_str());
              break;
            }
          } else if (!(std::char_traits<char>::compare(it.key().data() + (it.key().length() - 3), "Pin", 3))) {
            if (it.value().is_number() && it.value() > 0 && it.value() < 256) {
              if (!GPIO_IS_VALID_GPIO(it.value().template get<uint8_t>()) && !GPIO_IS_VALID_OUTPUT_GPIO(it.value().template get<uint8_t>()) && it.value() != 255) {
                LOG(E, "\"%s\" could not validate!", it.key().c_str());
                std::string msg = "\"\" is not a valid GPIO Pin for \"\"";
                msg.insert(1, it.value().dump().c_str()).insert(msg.length() - 1, it.key());
                req->send(400, "text/plain", msg.c_str());
                break;
              }
            } else {
                LOG(E, "\"%s\" could not validate!", it.key().c_str());
                std::string msg = "\"\" is not a valid value for \"\"";
                msg.insert(1, it.value().dump().c_str()).insert(msg.length() - 1, it.key());
                req->send(400, "text/plain", msg.c_str());
                break;
            }
          }
          if (configData.at(it.key()).is_boolean() && it.value().is_number()) {
            it.value() = static_cast<bool>(it.value().template get<uint8_t>());
          } else if(configData.at(it.key()).is_boolean() && !it.value().is_number()) {
            LOG(E, "\"%s\" could not validate!", it.key().c_str());
            std::string msg = "\"\" is not a valid value for \"\"";
            msg.insert(1, it.value().dump().c_str()).insert(msg.length() - 1, it.key());
            req->send(400, "text/plain", msg.c_str());
            break;
          }
          propertiesProcessed++;
        } else {
          LOG(E, "\"%s\" could not validate!", it.key().c_str());
          std::string msg = "\"\" not of correct type or does not exist in config";
          msg.insert(1, it.key());
          req->send(400, "text/plain", msg.c_str());
          break;
        }
      }
      if (propertiesProcessed != serializedData->size()) {
        LOG(E, "Not all properties could be validated, cannot continue!");
        if(!req->client()->disconnected() || !req->client()->disconnecting()) {
          req->send(500, "text/plain", "Something went wrong!");
        }
        return;
      }
      bool rebootNeeded = false;
      std::string rebootMsg;
      for (auto it = serializedData->begin(); it != serializedData->end(); ++it) {
        if (it.key() == std::string("nfcTagNoPublish") && (it.value() != 0)) {
          std::string clientId;
          if (serializedData->contains("mqttClientId")) {
            clientId = serializedData->at("mqttClientId");
          }
          std::string rfidTopic;
          rfidTopic.append("homeassistant/tag/").append(!clientId.empty() ? clientId : espConfig::mqttData.mqttClientId).append("/rfid/config");
          esp_mqtt_client_publish(client, rfidTopic.c_str(), "", 0, 0, false);
        } else if (it.key() == std::string("setupCode")) {
          std::string code = it.value().template get<std::string>();
          if (espConfig::miscConfig.setupCode.c_str() != it.value() && code.length() == 8) {
            if (homeSpan.controllerListBegin() == homeSpan.controllerListEnd()) {
              homeSpan.setPairingCode(code.c_str());
            }
          }
        } else if (it.key() == std::string("nfcNeopixelPin")) {
          if (espConfig::miscConfig.nfcNeopixelPin == 255 && it.value() != 255 && neopixel_task_handle == nullptr) {
            xTaskCreate(neopixel_task, "neopixel_task", 4096, NULL, 2, &neopixel_task_handle);
            if (!pixel) {
              pixel = std::make_unique<Pixel>(it.value(), PixelType::GRB);
            }
          } else if (espConfig::miscConfig.nfcNeopixelPin != 255 && it.value() == 255 && neopixel_task_handle != nullptr) {
            uint8_t status = 2;
            xQueueSend(neopixel_handle, &status, 0);
            neopixel_task_handle = nullptr;
          }
        } else if (it.key() == std::string("nfcSuccessPin")) {
          if (espConfig::miscConfig.nfcSuccessPin == 255 && it.value() != 255 && gpio_led_task_handle == nullptr) {
            pinMode(it.value(), OUTPUT);
            xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, &gpio_led_task_handle);
          } else if (espConfig::miscConfig.nfcSuccessPin != 255 && it.value() == 255 && gpio_led_task_handle != nullptr) {
            if (serializedData->contains("nfcFailPin") && serializedData->at("nfcFailPin") == 255) {
              uint8_t status = 2;
              xQueueSend(gpio_led_handle, &status, 0);
              gpio_led_task_handle = nullptr;
            }
          } else if (it.value() != 255) {
            pinMode(it.value(), OUTPUT);
          }
        } else if (it.key() == std::string("nfcFailPin")) {
          if (espConfig::miscConfig.nfcFailPin == 255 && it.value() != 255 && gpio_led_task_handle == nullptr) {
            pinMode(it.value(), OUTPUT);
            xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, &gpio_led_task_handle);
          } else if (espConfig::miscConfig.nfcFailPin != 255 && it.value() == 255 && gpio_led_task_handle != nullptr) {
            if (serializedData->contains("nfcSuccessPin") && serializedData->at("nfcSuccessPin") == 255) {
              uint8_t status = 2;
              xQueueSend(gpio_led_handle, &status, 0);
              gpio_led_task_handle = nullptr;
            }
          } else if (it.value() != 255) {
            pinMode(it.value(), OUTPUT);
          }
        } else if (it.key() == std::string("btrLowStatusThreshold")) {
          if (statusLowBtr && btrLevel) {
            if (btrLevel->getVal() <= it.value()) {
              statusLowBtr->setVal(1);
            } else {
              statusLowBtr->setVal(0);
            }
          }
        } else if (it.key() == std::string("neoPixelType")) {
          std::string pixelType = it.value().template get<std::string>();
          if (pixelType.compare(configData.at(it.key()).template get<std::string>())) {
            rebootNeeded = true;
            rebootMsg = "Pixel Type was changed, reboot needed! Rebooting...";
          }
        } else if (it.key() == std::string("gpioActionPin")) {
          if (espConfig::miscConfig.gpioActionPin == 255 && it.value() != 255 && gpio_lock_task_handle == nullptr) {
            LOG(D, "ENABLING HomeKit Trigger - Simple GPIO");
            pinMode(it.value(), OUTPUT);
            xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, &gpio_lock_task_handle);
          } else if (espConfig::miscConfig.gpioActionPin != 255 && it.value() == 255 && gpio_lock_task_handle != nullptr) {
            LOG(D, "DISABLING HomeKit Trigger - Simple GPIO");
            gpioLockAction status{ .source = gpioLockAction::OTHER, .action = 2 };
            xQueueSend(gpio_lock_handle, &status, 0);
            gpio_lock_task_handle = nullptr;
            gpio_reset_pin(gpio_num_t(espConfig::miscConfig.gpioActionPin));
          }
        }
        configData.at(it.key()) = it.value();
      }
      std::vector<uint8_t> vectorData = json::to_msgpack(configData);
      esp_err_t set_nvs = nvs_set_blob(savedData, selConfig == 0 ? "MQTTDATA" : "MISCDATA", vectorData.data(), vectorData.size());
      esp_err_t commit_nvs = nvs_commit(savedData);
      LOG(D, "SET_STATUS: %s", esp_err_to_name(set_nvs));
      LOG(D, "COMMIT_STATUS: %s", esp_err_to_name(commit_nvs));
      if (set_nvs == ESP_OK && commit_nvs == ESP_OK) {
        LOG(I, "Config successfully saved to NVS");
        if (selConfig == 0) {
          configData.get_to<espConfig::mqttConfig_t>(espConfig::mqttData);
        } else {
          configData.get_to<espConfig::misc_config_t>(espConfig::miscConfig);
        }
      } else {
        LOG(E, "Something went wrong, could not save to NVS");
      }
      if (selConfig == 0 || selConfig == 2) {
        req->send(200, "text/plain", "Saved! Restarting...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP.restart();
      } else {
        if(rebootNeeded){
          req->send(200, "text/plain", rebootMsg.c_str());
        } else {
          req->send(200, "text/plain", "Saved and applied!");
        }
      }
    }
  });
  auto rebootDeviceHandle = new AsyncCallbackWebHandler();
  rebootDeviceHandle->setUri("/reboot_device");
  rebootDeviceHandle->setMethod(HTTP_GET);
  rebootDeviceHandle->onRequest([](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Rebooting the device...");
    delay(1000);
    ESP.restart();
    });
  webServer.addHandler(rebootDeviceHandle);
  auto resetHkHandle = new AsyncCallbackWebHandler();
  resetHkHandle->setUri("/reset_hk_pair");
  resetHkHandle->setMethod(HTTP_GET);
  resetHkHandle->onRequest([](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Erasing HomeKit pairings and restarting...");
    delay(1000);
    deleteReaderData();
    homeSpan.processSerialCommand("H");
    });
  webServer.addHandler(resetHkHandle);
  auto resetWifiHandle = new AsyncCallbackWebHandler();
  resetWifiHandle->setUri("/reset_wifi_cred");
  resetWifiHandle->setMethod(HTTP_GET);
  resetWifiHandle->onRequest([](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Erasing WiFi credentials and restarting, AP will start on boot...");
    delay(1000);
    homeSpan.processSerialCommand("X");
    });
  webServer.addHandler(resetWifiHandle);
  auto getWifiRssi = new AsyncCallbackWebHandler();
  getWifiRssi->setUri("/get_wifi_rssi");
  getWifiRssi->setMethod(HTTP_GET);
  getWifiRssi->onRequest([](AsyncWebServerRequest* request) {
    std::string rssi_val = std::to_string(WiFi.RSSI());
    request->send(200, "text/plain", rssi_val.c_str());
    });
  webServer.addHandler(getWifiRssi);
  AsyncCallbackWebHandler* rootHandle = new AsyncCallbackWebHandler();
  webServer.addHandler(rootHandle);
  rootHandle->setUri("/");
  rootHandle->setMethod(HTTP_GET);
  rootHandle->onRequest([](AsyncWebServerRequest* req) {
    req->send(LittleFS, "/index.html", "text/html", false, indexProcess);
  });
  AsyncCallbackWebHandler* hashPage = new AsyncCallbackWebHandler();
  webServer.addHandler(hashPage);
  hashPage->setUri("/#*");
  hashPage->setMethod(HTTP_GET);
  hashPage->onRequest([](AsyncWebServerRequest* req) {
    req->send(LittleFS, "/index.html", "text/html", false, indexProcess);
  });
  if (espConfig::miscConfig.webAuthEnabled) {
    LOG(I, "Web Authentication Enabled");
    routesHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    dataProvision->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    dataLoad->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    dataClear->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    rootHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    hashPage->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    resetHkHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    resetWifiHandle->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    getWifiRssi->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
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
  if (espConfig::mqttData.mqttBroker.size() >= 7 && espConfig::mqttData.mqttBroker.size() <= 16 && !std::equal(espConfig::mqttData.mqttBroker.begin(), espConfig::mqttData.mqttBroker.end(), "0.0.0.0")) {
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

void nfc_retry(void* arg) {
  ESP_LOGI(TAG, "Starting reconnecting PN532");
  while (1) {
    nfc->begin();
    uint32_t versiondata = nfc->getFirmwareVersion();
    if (!versiondata) {
      ESP_LOGE("NFC_SETUP", "Error establishing PN532 connection");
    } else {
      unsigned int model = (versiondata >> 24) & 0xFF;
      ESP_LOGI("NFC_SETUP", "Found chip PN5%x", model);
      int maj = (versiondata >> 16) & 0xFF;
      int min = (versiondata >> 8) & 0xFF;
      ESP_LOGI("NFC_SETUP", "Firmware ver. %d.%d", maj, min);
      nfc->SAMConfig();
      nfc->setRFField(0x02, 0x01);
      nfc->setPassiveActivationRetries(0);
      ESP_LOGI("NFC_SETUP", "Waiting for an ISO14443A card");
      vTaskResume(nfc_poll_task);
      vTaskDelete(NULL);
      return;
    }
    nfc->stop();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void nfc_thread_entry(void* arg) {
  uint32_t versiondata = nfc->getFirmwareVersion();
  if (!versiondata) {
    ESP_LOGE("NFC_SETUP", "Error establishing PN532 connection");
    nfc->stop();
    xTaskCreate(nfc_retry, "nfc_reconnect_task", 8192, NULL, 1, &nfc_reconnect_task);
    vTaskSuspend(NULL);
  } else {
    unsigned int model = (versiondata >> 24) & 0xFF;
    ESP_LOGI("NFC_SETUP", "Found chip PN5%x", model);
    int maj = (versiondata >> 16) & 0xFF;
    int min = (versiondata >> 8) & 0xFF;
    ESP_LOGI("NFC_SETUP", "Firmware ver. %d.%d", maj, min);
    nfc->SAMConfig();
    nfc->setRFField(0x02, 0x01);
    nfc->setPassiveActivationRetries(0);
    ESP_LOGI("NFC_SETUP", "Waiting for an ISO14443A card");
  }
  memcpy(ecpData + 8, readerData.reader_gid.data(), readerData.reader_gid.size());
  with_crc16(ecpData, 16, ecpData + 16);
  while (1) {
    uint8_t res[4];
    uint16_t resLen = 4;
    bool writeStatus = nfc->writeRegister(0x633d, 0, true);
    if (!writeStatus) {
      LOG(W, "writeRegister has failed, abandoning ship !!");
      nfc->stop();
      xTaskCreate(nfc_retry, "nfc_reconnect_task", 8192, NULL, 1, &nfc_reconnect_task);
      vTaskSuspend(NULL);
    }
    nfc->inCommunicateThru(ecpData, sizeof(ecpData), res, &resLen, 100, true);
    uint8_t uid[16];
    uint8_t uidLen = 0;
    uint8_t atqa[2];
    uint8_t sak[1];
    bool passiveTarget = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, atqa, sak, 500, true, true);
    if (passiveTarget) {
      nfc->setPassiveActivationRetries(5);
      LOG(D, "ATQA: %02x", atqa[0]);
      LOG(D, "SAK: %02x", sak[0]);
      ESP_LOG_BUFFER_HEX_LEVEL(TAG, uid, (size_t)uidLen, ESP_LOG_VERBOSE);
      LOG(I, "*** PASSIVE TARGET DETECTED ***");
      auto startTime = std::chrono::high_resolution_clock::now();
      uint8_t data[13] = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x0 };
      uint8_t selectCmdRes[9];
      uint16_t selectCmdResLength = 9;
      LOG(I, "Requesting supported HomeKey versions");
      LOG(D, "SELECT HomeKey Applet, APDU: ");
      ESP_LOG_BUFFER_HEX_LEVEL(TAG, data, sizeof(data), ESP_LOG_VERBOSE);
      bool status = nfc->inDataExchange(data, sizeof(data), selectCmdRes, &selectCmdResLength);
      LOG(D, "SELECT HomeKey Applet, Response");
      ESP_LOG_BUFFER_HEX_LEVEL(TAG, selectCmdRes, selectCmdResLength, ESP_LOG_VERBOSE);
      if (status && selectCmdRes[selectCmdResLength - 2] == 0x90 && selectCmdRes[selectCmdResLength - 1] == 0x00) {
        LOG(D, "*** SELECT HOMEKEY APPLET SUCCESSFUL ***");
        LOG(D, "Reader Private Key: %s", red_log::bufToHexString(readerData.reader_pk.data(), readerData.reader_pk.size()).c_str());
        HKAuthenticationContext authCtx([](uint8_t* s, uint8_t l, uint8_t* r, uint16_t* rl, bool il) -> bool {return nfc->inDataExchange(s, l, r, rl, il);}, readerData, savedData);
        auto authResult = authCtx.authenticate(hkFlow);
        if (std::get<2>(authResult) != kFlowFailed) {
          bool status = true;
          if (espConfig::miscConfig.nfcSuccessPin != 255) {
            xQueueSend(gpio_led_handle, &status, 0);
          }
          if (espConfig::miscConfig.nfcNeopixelPin != 255) {
            xQueueSend(neopixel_handle, &status, 0);
          }
          if (espConfig::miscConfig.gpioActionPin != 255 && espConfig::miscConfig.hkGpioControlledState) {
            const gpioLockAction action{ .source = gpioLockAction::HOMEKEY, .action = 0 };
            xQueueSend(gpio_lock_handle, &action, 0);
          }
          if (espConfig::miscConfig.hkAltActionInitPin != 255 && espConfig::miscConfig.hkAltActionPin != 255) {
            uint8_t status = 2;
            xQueueSend(gpio_led_handle, &status, 0);
          }
          if (hkAltActionActive) {
            mqtt_publish(espConfig::mqttData.hkAltActionTopic, "alt_action", 0, false);
          }
          json payload;
          payload["issuerId"] = hex_representation(std::get<0>(authResult));
          payload["endpointId"] = hex_representation(std::get<1>(authResult));
          payload["readerId"] = hex_representation(readerData.reader_id);
          payload["homekey"] = true;
          std::string payloadStr = payload.dump();
          mqtt_publish(espConfig::mqttData.hkTopic, payloadStr, 0, false);
          if (espConfig::miscConfig.lockAlwaysUnlock) {
            if (espConfig::miscConfig.gpioActionPin == 255 || !espConfig::miscConfig.hkGpioControlledState) {
              lockCurrentState->setVal(lockStates::UNLOCKED);
              lockTargetState->setVal(lockStates::UNLOCKED);
              mqtt_publish(espConfig::mqttData.lockStateTopic, std::to_string(lockStates::UNLOCKED), 1, true);
            }
            if (espConfig::mqttData.lockEnableCustomState) {
              mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]), 0, false);
            }
          } else if (espConfig::miscConfig.lockAlwaysLock) {
            if (espConfig::miscConfig.gpioActionPin == 255 || espConfig::miscConfig.hkGpioControlledState) {
              lockCurrentState->setVal(lockStates::LOCKED);
              lockTargetState->setVal(lockStates::LOCKED);
              mqtt_publish(espConfig::mqttData.lockStateTopic, std::to_string(lockStates::LOCKED), 1, true);
            }
            if (espConfig::mqttData.lockEnableCustomState) {
              mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["LOCK"]), 0, false);
            }
          } else {
            int currentState = lockCurrentState->getVal();
            if (espConfig::mqttData.lockEnableCustomState) {
              if (currentState == lockStates::UNLOCKED) {
                mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["LOCK"]), 0, false);
              } else if (currentState == lockStates::LOCKED) {
                mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]), 0, false);
              }
            }
          }

          auto stopTime = std::chrono::high_resolution_clock::now();
          LOG(I, "Total Time (detection->auth->gpio->mqtt): %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
        } else {
          bool status = false;
          if (espConfig::miscConfig.nfcFailPin != 255) {
            xQueueSend(gpio_led_handle, &status, 0);
          }
          if (espConfig::miscConfig.nfcNeopixelPin != 255) {
            xQueueSend(neopixel_handle, &status, 0);
          }
          LOG(W, "We got status FlowFailed, mqtt untouched!");
        }
        nfc->setRFField(0x02, 0x01);
      } else if(!espConfig::mqttData.nfcTagNoPublish) {
        LOG(W, "Invalid Response, probably not Homekey, publishing target's UID");
        bool status = false;
        if (espConfig::miscConfig.nfcFailPin != 255) {
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
        } else LOG(W, "MQTT Client not initialized, cannot publish message");
      }
      vTaskDelay(50 / portTICK_PERIOD_MS);
      nfc->inRelease();
      int counter = 50;
      bool deviceStillInField = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
      LOG(D, "Target still present: %d", deviceStillInField);
      while (deviceStillInField) {
        if (counter == 0) break;
        vTaskDelay(50 / portTICK_PERIOD_MS);
        nfc->inRelease();
        deviceStillInField = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
        --counter;
        LOG(D, "Target still present: %d Counter=%d", deviceStillInField, counter);
      }
      nfc->inRelease();
      nfc->setPassiveActivationRetries(0);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
  return;
}

void setup() {
  Serial.begin(115200);
  const esp_app_desc_t* app_desc = esp_ota_get_app_description();
  std::string app_version = app_desc->version;
  gpio_led_handle = xQueueCreate(2, sizeof(uint8_t));
  neopixel_handle = xQueueCreate(2, sizeof(uint8_t));
  gpio_lock_handle = xQueueCreate(2, sizeof(gpioLockAction));
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
      LOG(I, "Reader Data loaded from NVS");
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
        LOG(I, "MQTT Config loaded from NVS");
      }
    } else {
      nlohmann::json data = nlohmann::json::from_msgpack(dataBuf);
      if (!data.is_discarded()) {
        data.get_to<espConfig::mqttConfig_t>(espConfig::mqttData);
        LOG(I, "MQTT Config loaded from NVS");
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
        LOG(I, "Misc Config loaded from NVS");
      }
    } else {
      nlohmann::json data = nlohmann::json::from_msgpack(dataBuf);
      if (!data.is_discarded()) {
        data.get_to<espConfig::misc_config_t>(espConfig::miscConfig);
        LOG(I, "Misc Config loaded from NVS");
      }
    }
  }
  pn532spi = new PN532_SPI(espConfig::miscConfig.nfcGpioPins[0], espConfig::miscConfig.nfcGpioPins[1], espConfig::miscConfig.nfcGpioPins[2], espConfig::miscConfig.nfcGpioPins[3]);
  nfc = new PN532(*pn532spi);
  nfc->begin();
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
  if (espConfig::miscConfig.hkAltActionInitPin != 255) {
    pinMode(espConfig::miscConfig.hkAltActionInitPin, INPUT);
    if (espConfig::miscConfig.hkAltActionPin != 255) {
      pinMode(espConfig::miscConfig.hkAltActionPin, OUTPUT);
    }
    if (espConfig::miscConfig.hkAltActionInitLedPin != 255) {
      pinMode(espConfig::miscConfig.hkAltActionInitLedPin, OUTPUT);
    }
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

  LOG(I, "READER GROUP ID (%d): %s", readerData.reader_gid.size(), red_log::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size()).c_str());
  LOG(I, "READER UNIQUE ID (%d): %s", readerData.reader_id.size(), red_log::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size()).c_str());

  LOG(I, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto&& issuer : readerData.issuers) {
    LOG(D, "Issuer ID: %s, Public Key: %s", red_log::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str(), red_log::bufToHexString(issuer.issuer_pk.data(), issuer.issuer_pk.size()).c_str());
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
  new SpanUserCommand('N', "Btr status low", [](const char* arg) {
    const char* TAG = "BTR_LOW";
    if (strncmp(arg + 1, "0", 1) == 0) {
      statusLowBtr->setVal(0);
      LOG(I, "Low status set to NORMAL");
    } else if (strncmp(arg + 1, "1", 1) == 0) {
      statusLowBtr->setVal(1);
      LOG(I, "Low status set to LOW");
    }
  });
  new SpanUserCommand('B', "Btr level", [](const char* arg) {
    uint8_t level = atoi(static_cast<const char *>(arg + 1));
    btrLevel->setVal(level);
  });

  new SpanAccessory();
  new Service::HAPProtocolInformation();
  new Characteristic::Version();
  new NFCAccessoryInformation();
  new LockManagement();
  new LockMechanism();
  new NFCAccess();
  if (espConfig::miscConfig.proxBatEnabled) {
    new PhysicalLockBattery();
  }
  homeSpan.setControllerCallback(pairCallback);
  homeSpan.setWifiCallback(wifiCallback);
  if (espConfig::miscConfig.nfcNeopixelPin != 255) {
    pixel = std::make_unique<Pixel>(espConfig::miscConfig.nfcNeopixelPin, pixelTypeMap[espConfig::miscConfig.neoPixelType]);
    xTaskCreate(neopixel_task, "neopixel_task", 4096, NULL, 2, &neopixel_task_handle);
  }
  if (espConfig::miscConfig.nfcSuccessPin != 255 || espConfig::miscConfig.nfcFailPin != 255) {
    xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, &gpio_led_task_handle);
  }
  if (espConfig::miscConfig.gpioActionPin != 255) {
    xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, &gpio_lock_task_handle);
  }
  if (espConfig::miscConfig.hkAltActionInitPin != 255) {
    xTaskCreate(alt_action_task, "alt_action_task", 2048, NULL, 2, &alt_action_task_handle);
  }
  xTaskCreate(nfc_thread_entry, "nfc_task", 8192, NULL, 1, &nfc_poll_task);
}

//////////////////////////////////////

void loop() {
  homeSpan.poll();
  vTaskDelay(5);
}
