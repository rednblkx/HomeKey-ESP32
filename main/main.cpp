#include "config.hpp"
#include "esp_log.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#define JSON_NOEXCEPTION 1
#include <sodium/crypto_sign.h>
#include <sodium/crypto_box.h>
#include "HAP.h"
#include "hkAuthContext.h"
#include "HomeKey.h"
#include "array"
#include "logging.h"
#include "PN532_SPI.h"
#include "PN532.h"
#include "chrono"
#include "ESPAsyncWebServer.h"
#include "LittleFS.h"
#include "HK_HomeKit.h"
#include "mqtt_client.h"
#include "esp_app_desc.h"
#include "NFC_SERV_CHARS.hpp"
#include <mbedtls/sha256.h>
#include <esp_mac.h>
#include "ConfigManager.hpp"
#include "ReaderDataManager.hpp"

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

ReaderDataManager readerData;
ConfigManager configManager;
uint8_t ecpData[18] = { 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 };

KeyFlow hkFlow = KeyFlow::kFlowFAST;
bool hkAltActionActive = false;
SpanCharacteristic* lockCurrentState;
SpanCharacteristic* lockTargetState;
SpanCharacteristic* statusLowBtr;
SpanCharacteristic* btrLevel;
esp_mqtt_client_handle_t client = nullptr;

std::shared_ptr<Pixel> pixel;

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
    const esp_app_desc_t* app_desc = esp_app_get_description();
    std::string app_version = app_desc->version;
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    char macStr[9] = { 0 };
    sprintf(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
    std::string serialNumber = "HK-";
    serialNumber.append(macStr);
    new Characteristic::SerialNumber(serialNumber.c_str());
    new Characteristic::FirmwareRevision(app_version.c_str());
    std::array<uint8_t, 6> decB64 = hk_color_vals[static_cast<HK_COLOR>(configManager.getMiscConfig().hk_key_color)];
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
    buttonState = digitalRead(configManager.getMiscConfig().hkAltActionInitPin);
    if (buttonState == HIGH) {
      LOG(D, "BUTTON HIGH");
      hkAltActionActive = true;
      if(configManager.getMiscConfig().hkAltActionInitLedPin != 255) {
        digitalWrite(configManager.getMiscConfig().hkAltActionInitLedPin, HIGH);
      }
      vTaskDelay(configManager.getMiscConfig().hkAltActionInitTimeout / portTICK_PERIOD_MS);
      if (configManager.getMiscConfig().hkAltActionInitLedPin != 255) {
        digitalWrite(configManager.getMiscConfig().hkAltActionInitLedPin, LOW);
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
          LOG(D, "%d - %d - %d -%d", configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionMomentaryEnabled, configManager.getMiscConfig().lockAlwaysUnlock, configManager.getMiscConfig().lockAlwaysLock);
          if (configManager.getMiscConfig().lockAlwaysUnlock && status.source != gpioLockAction::HOMEKIT) {
            lockTargetState->setVal(lockStates::UNLOCKED);
            if(configManager.getMiscConfig().gpioActionPin != 255){
              digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionUnlockState);
            }
            lockCurrentState->setVal(lockStates::UNLOCKED);
            if (client != nullptr) {
              esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 1, 0, false);
            } else LOG(W, "MQTT Client not initialized, cannot publish message");

            if (static_cast<uint8_t>(configManager.getMiscConfig().gpioActionMomentaryEnabled) & status.source) {
              delay(configManager.getMiscConfig().gpioActionMomentaryTimeout);
              lockTargetState->setVal(lockStates::LOCKED);
              if(configManager.getMiscConfig().gpioActionPin != 255){
                digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionLockState);
              }
              lockCurrentState->setVal(lockStates::LOCKED);
              if (client != nullptr) {
                esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 1, 0, false);
              } else LOG(W, "MQTT Client not initialized, cannot publish message");
            }
          } else if (configManager.getMiscConfig().lockAlwaysLock && status.source != gpioLockAction::HOMEKIT) {
            lockTargetState->setVal(lockStates::LOCKED);
            if(configManager.getMiscConfig().gpioActionPin != 255){
              digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionLockState);
            }
            lockCurrentState->setVal(lockStates::LOCKED);
            if (client != nullptr) {
              esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 1, 0, false);
            } else LOG(W, "MQTT Client not initialized, cannot publish message");
          } else {
            int currentState = lockCurrentState->getVal();
            if (status.source != gpioLockAction::HOMEKIT) {
              lockTargetState->setVal(!currentState);
            }
            if(configManager.getMiscConfig().gpioActionPin != 255){
              digitalWrite(configManager.getMiscConfig().gpioActionPin, currentState == lockStates::UNLOCKED ? configManager.getMiscConfig().gpioActionLockState : configManager.getMiscConfig().gpioActionUnlockState);
            }
            lockCurrentState->setVal(!currentState);
            if (client != nullptr) {
              esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockCurrentState->getNewVal()).c_str(), 1, 0, false);
            } else LOG(W, "MQTT Client not initialized, cannot publish message");
            if ((static_cast<uint8_t>(configManager.getMiscConfig().gpioActionMomentaryEnabled) & status.source) && currentState == lockStates::LOCKED) {
              delay(configManager.getMiscConfig().gpioActionMomentaryTimeout);
              lockTargetState->setVal(currentState);
              if(configManager.getMiscConfig().gpioActionPin != 255){
                digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionLockState);
              }
              lockCurrentState->setVal(currentState);
              if (client != nullptr) {
                esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockCurrentState->getNewVal()).c_str(), 1, 0, false);
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
          if (configManager.getMiscConfig().nfcNeopixelPin && configManager.getMiscConfig().nfcNeopixelPin != 255) {
            LOG(D, "SUCCESS PIXEL %d:%d,%d,%d", configManager.getMiscConfig().nfcNeopixelPin, configManager.getMiscConfig().neopixelFailureColor.at(espConfig::misc_config_t::colorMap::R), configManager.getMiscConfig().neopixelFailureColor.at(espConfig::misc_config_t::colorMap::G), configManager.getMiscConfig().neopixelFailureColor.at(espConfig::misc_config_t::colorMap::B));
            pixel->set(pixel->RGB(configManager.getMiscConfig().neopixelFailureColor.at(espConfig::misc_config_t::colorMap::R), configManager.getMiscConfig().neopixelFailureColor.at(espConfig::misc_config_t::colorMap::G), configManager.getMiscConfig().neopixelFailureColor.at(espConfig::misc_config_t::colorMap::B)));
            delay(configManager.getMiscConfig().neopixelFailTime);
            pixel->off();
          }
          break;
        case 1:
          if (configManager.getMiscConfig().nfcNeopixelPin && configManager.getMiscConfig().nfcNeopixelPin != 255) {
            LOG(D, "FAIL PIXEL %d:%d,%d,%d", configManager.getMiscConfig().nfcNeopixelPin, configManager.getMiscConfig().neopixelSuccessColor.at(espConfig::misc_config_t::colorMap::R), configManager.getMiscConfig().neopixelSuccessColor.at(espConfig::misc_config_t::colorMap::G), configManager.getMiscConfig().neopixelSuccessColor.at(espConfig::misc_config_t::colorMap::B));
            pixel->set(pixel->RGB(configManager.getMiscConfig().neopixelSuccessColor.at(espConfig::misc_config_t::colorMap::R), configManager.getMiscConfig().neopixelSuccessColor.at(espConfig::misc_config_t::colorMap::G), configManager.getMiscConfig().neopixelSuccessColor.at(espConfig::misc_config_t::colorMap::B)));
            delay(configManager.getMiscConfig().neopixelSuccessTime);
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
          if (configManager.getMiscConfig().nfcFailPin && configManager.getMiscConfig().nfcFailPin != 255) {
            LOG(D, "FAIL LED %d:%d", configManager.getMiscConfig().nfcFailPin, configManager.getMiscConfig().nfcFailHL);
            digitalWrite(configManager.getMiscConfig().nfcFailPin, configManager.getMiscConfig().nfcFailHL);
            delay(configManager.getMiscConfig().nfcFailTime);
            digitalWrite(configManager.getMiscConfig().nfcFailPin, !configManager.getMiscConfig().nfcFailHL);
          }
          break;
        case 1:
          if (configManager.getMiscConfig().nfcSuccessPin && configManager.getMiscConfig().nfcSuccessPin != 255) {
            LOG(D, "SUCCESS LED %d:%d", configManager.getMiscConfig().nfcSuccessPin, configManager.getMiscConfig().nfcSuccessHL);
            digitalWrite(configManager.getMiscConfig().nfcSuccessPin, configManager.getMiscConfig().nfcSuccessHL);
            delay(configManager.getMiscConfig().nfcSuccessTime);
            digitalWrite(configManager.getMiscConfig().nfcSuccessPin, !configManager.getMiscConfig().nfcSuccessHL);
          }
          break;
        case 2:
          if(hkAltActionActive){
            digitalWrite(configManager.getMiscConfig().hkAltActionPin, configManager.getMiscConfig().hkAltActionGpioState);
            delay(configManager.getMiscConfig().hkAltActionTimeout);
            digitalWrite(configManager.getMiscConfig().hkAltActionPin, !configManager.getMiscConfig().hkAltActionGpioState);
          }
          break;
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
    with_crc16(ecpData, 16, ecpData + 16);
    if (configManager.getMiscConfig().gpioActionPin != 255) {
      if (lockCurrentState->getVal() == lockStates::LOCKED) {
        digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionLockState);
      } else if (lockCurrentState->getVal() == lockStates::UNLOCKED) {
        digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionUnlockState);
      }
    }
  } // end constructor

  boolean update() {
    int targetState = lockTargetState->getNewVal();
    LOG(I, "New LockState=%d, Current LockState=%d", targetState, lockCurrentState->getVal());
    if (configManager.getMiscConfig().gpioActionPin != 255) {
      const gpioLockAction gpioAction{ .source = gpioLockAction::HOMEKIT, .action = 0 };
      xQueueSend(gpio_lock_handle, &gpioAction, 0);
    } else if (configManager.getMiscConfig().hkDumbSwitchMode) {
      const gpioLockAction gpioAction{ .source = gpioLockAction::HOMEKIT, .action = 0 };
      xQueueSend(gpio_lock_handle, &gpioAction, 0);
    }
    int currentState = lockCurrentState->getNewVal();
    if (client != nullptr) {
      if (configManager.getMiscConfig().gpioActionPin == 255) {
        if (targetState != currentState) {
          esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), targetState == lockStates::UNLOCKED ? std::to_string(lockStates::UNLOCKING).c_str() : std::to_string(lockStates::LOCKING).c_str(), 1, 1, true);
        } else {
          esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(currentState).c_str(), 1, 1, true);
        }
      }
      if (configManager.getMqttConfig().lockEnableCustomState) {
        if (targetState == lockStates::UNLOCKED) {
          esp_mqtt_client_publish(client, configManager.getMqttConfig().lockCustomStateTopic.c_str(), std::to_string(configManager.getMqttConfig().customLockActions.at("UNLOCK")).c_str(), 0, 0, false);
        } else if (targetState == lockStates::LOCKED) {
          esp_mqtt_client_publish(client, configManager.getMqttConfig().lockCustomStateTopic.c_str(), std::to_string(configManager.getMqttConfig().customLockActions.at("LOCK")).c_str(), 0, 0, false);
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
    readerData.begin();
    memcpy(ecpData + 8, readerData.getReaderGid().data(), readerData.getReaderGid().size());
    configurationState = new Characteristic::ConfigurationState();
    nfcControlPoint = new Characteristic::NFCAccessControlPoint();
    TLV8 conf(NULL, 0);
    conf.add(0x01, 0x10);
    conf.add(0x02, 0x10);
    nfcSupportedConfiguration = new Characteristic::NFCAccessSupportedConfiguration(conf);
  }

  boolean update() {
    // LOG(D, "PROVISIONED READER KEY: %s", red_log::bufToHexString(readerData.reader_pk.data(), readerData.reader_pk.size()).c_str());
    LOG(D, "READER GROUP IDENTIFIER: %s", red_log::bufToHexString(readerData.getReaderGid().data(), readerData.getReaderGid().size()).c_str());
    LOG(D, "READER UNIQUE IDENTIFIER: %s", red_log::bufToHexString(readerData.getReaderData().reader_id.data(), readerData.getReaderData().reader_id.size()).c_str());

    TLV8 ctrlData(NULL, 0);
    nfcControlPoint->getNewTLV(ctrlData);
    std::vector<uint8_t> tlvData(ctrlData.pack_size());
    ctrlData.pack(tlvData.data());
    if (tlvData.size() == 0)
      return false;
    LOG(D, "Decoded data: %s", red_log::bufToHexString(tlvData.data(), tlvData.size()).c_str());
    LOG(D, "Decoded data length: %d", tlvData.size());
    auto r = readerData.getReaderData();
    HK_HomeKit hkCtx(r, [](const readerData_t& dataToSave) {
            return readerData.saveReaderData(dataToSave);
        }, []() {
            return readerData.deleteReaderKey();
        }, tlvData);
    std::vector<uint8_t> result = hkCtx.processResult();
    if (readerData.getReaderGid().size() > 0) {
      memcpy(ecpData + 8, readerData.getReaderGid().data(), readerData.getReaderGid().size());
      with_crc16(ecpData, 16, ecpData + 16);
    }
    TLV8 res(NULL, 0);
    res.unpack(result.data(), result.size());
    nfcControlPoint->setTLV(res, false);
    return true;
  }

};

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
    readerData.deleteAllReaderData();
    return;
  }
  for (auto it = homeSpan.controllerListBegin(); it != homeSpan.controllerListEnd(); ++it) {
    std::vector<uint8_t> id = getHashIdentifier(it->getLTPK(), 32);
    LOG(D, "Found allocated controller - Hash: %s", red_log::bufToHexString(id.data(), 8).c_str());
    readerData.addIssuerIfNotExists(id, it->getLTPK());
  }
  readerData.save();
}

void setFlow(const char* buf) {
  switch (buf[1]) {
  case '0':
    hkFlow = KeyFlow::kFlowFAST;
    LOG(I, "FAST Flow");
    break;

  case '1':
    hkFlow = KeyFlow::kFlowSTANDARD;
    LOG(I, "STANDARD Flow");
    break;
  case '2':
    hkFlow = KeyFlow::kFlowATTESTATION;
    LOG(I, "ATTESTATION Flow");
    break;

  default:
    LOG(I, "0 = FAST flow, 1 = STANDARD Flow, 2 = ATTESTATION Flow");
    break;
  }
}

void setLogLevel(const char* buf) {
  esp_log_level_t level = esp_log_level_get("*");
  if (strncmp(buf + 1, "E", 1) == 0) {
    level = ESP_LOG_ERROR;
    LOG(I, "ERROR");
  } else if (strncmp(buf + 1, "W", 1) == 0) {
    level = ESP_LOG_WARN;
    LOG(I, "WARNING");
  } else if (strncmp(buf + 1, "I", 1) == 0) {
    level = ESP_LOG_INFO;
    LOG(I, "INFO");
  } else if (strncmp(buf + 1, "D", 1) == 0) {
    level = ESP_LOG_DEBUG;
    LOG(I, "DEBUG");
  } else if (strncmp(buf + 1, "V", 1) == 0) {
    level = ESP_LOG_VERBOSE;
    LOG(I, "VERBOSE");
  } else if (strncmp(buf + 1, "N", 1) == 0) {
    level = ESP_LOG_NONE;
    LOG(I, "NONE");
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
  for (auto&& issuer : readerData.getReaderData().issuers) {
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
  if (configManager.getMqttConfig().customLockStates.at("C_UNLOCKING") == state) {
    lockTargetState->setVal(lockStates::UNLOCKED);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKING).c_str(), 0, 1, true);
    return;
  } else if (configManager.getMqttConfig().customLockStates.at("C_LOCKING") == state) {
    lockTargetState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
    return;
  } else if (configManager.getMqttConfig().customLockStates.at("C_UNLOCKED") == state) {
    if (configManager.getMiscConfig().gpioActionPin != 255) {
      digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionUnlockState);
    }
    lockCurrentState->setVal(lockStates::UNLOCKED);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 0, 1, true);
    return;
  } else if (configManager.getMqttConfig().customLockStates.at("C_LOCKED") == state) {
    if (configManager.getMiscConfig().gpioActionPin != 255) {
      digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionLockState);
    }
    lockCurrentState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);
    return;
  } else if (configManager.getMqttConfig().customLockStates.at("C_JAMMED") == state) {
    lockCurrentState->setVal(lockStates::JAMMED);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::JAMMED).c_str(), 0, 1, true);
    return;
  } else if (configManager.getMqttConfig().customLockStates.at("C_UNKNOWN") == state) {
    lockCurrentState->setVal(lockStates::UNKNOWN);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::UNKNOWN).c_str(), 0, 1, true);
    return;
  }
  LOG(D, "Update state failed! Recv value not valid");
}

void set_state_handler(esp_mqtt_client_handle_t client, int state) {
  switch (state) {
  case lockStates::UNLOCKED:
    if (configManager.getMiscConfig().gpioActionPin != 255) {
      digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionUnlockState);
    }
    lockTargetState->setVal(state);
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 0, 1, true);
    if (configManager.getMqttConfig().lockEnableCustomState) {
      esp_mqtt_client_publish(client, configManager.getMqttConfig().lockCustomStateTopic.c_str(), std::to_string(configManager.getMqttConfig().customLockActions.at("UNLOCK")).c_str(), 0, 0, false);
    }
    break;
  case lockStates::LOCKED:
    if (configManager.getMiscConfig().gpioActionPin != 255) {
      digitalWrite(configManager.getMiscConfig().gpioActionPin, configManager.getMiscConfig().gpioActionLockState);
    }
    lockTargetState->setVal(state);
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);
    if (configManager.getMqttConfig().lockEnableCustomState) {
      esp_mqtt_client_publish(client, configManager.getMqttConfig().lockCustomStateTopic.c_str(), std::to_string(configManager.getMqttConfig().customLockActions.at("LOCK")).c_str(), 0, 0, false);
    }
    break;
  case lockStates::JAMMED:
  case lockStates::UNKNOWN:
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(state).c_str(), 0, 1, true);
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
  esp_read_mac(mac, ESP_MAC_BT);
  char macStr[18] = { 0 };
  sprintf(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
  std::string serialNumber = "HK-";
  serialNumber.append(macStr);
  LOG(I, "MQTT connected");
  if (configManager.getMqttConfig().hassMqttDiscoveryEnabled) {
    json payload;
    payload["topic"] = configManager.getMqttConfig().hkTopic.c_str();
    payload["value_template"] = "{{ value_json.uid }}";
    json device;
    device["name"] = configManager.getMiscConfig().deviceName.c_str();
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
    rfidTopic.append("homeassistant/tag/").append(configManager.getMqttConfig().mqttClientId).append("/rfid/config");
    if (!configManager.getMqttConfig().nfcTagNoPublish) {
      esp_mqtt_client_publish(client, rfidTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    }
    payload = json();
    payload["topic"] = configManager.getMqttConfig().hkTopic;
    payload["value_template"] = "{{ value_json.issuerId }}";
    payload["device"] = device;
    bufferpub = payload.dump();
    std::string issuerTopic;
    issuerTopic.append("homeassistant/tag/").append(configManager.getMqttConfig().mqttClientId).append("/hkIssuer/config");
    esp_mqtt_client_publish(client, issuerTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    payload = json();
    payload["topic"] = configManager.getMqttConfig().hkTopic;
    payload["value_template"] = "{{ value_json.endpointId }}";
    payload["device"] = device;
    bufferpub = payload.dump();
    std::string endpointTopic;
    endpointTopic.append("homeassistant/tag/").append(configManager.getMqttConfig().mqttClientId).append("/hkEndpoint/config");
    esp_mqtt_client_publish(client, endpointTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    payload = json();
    payload["name"] = "Lock";
    payload["state_topic"] = configManager.getMqttConfig().lockStateTopic.c_str();
    payload["command_topic"] = configManager.getMqttConfig().lockStateCmd.c_str();
    payload["payload_lock"] = "1";
    payload["payload_unlock"] = "0";
    payload["state_locked"] = "1";
    payload["state_unlocked"] = "0";
    payload["state_unlocking"] = "4";
    payload["state_locking"] = "5";
    payload["state_jammed"] = "2";
    payload["availability_topic"] = configManager.getMqttConfig().lwtTopic.c_str();
    payload["unique_id"] = id;
    payload["device"] = device;
    payload["retain"] = "false";
    bufferpub = payload.dump();
    std::string lockConfigTopic;
    lockConfigTopic.append("homeassistant/lock/").append(configManager.getMqttConfig().mqttClientId.c_str()).append("/lock/config");
    esp_mqtt_client_publish(client, lockConfigTopic.c_str(), bufferpub.c_str(), bufferpub.length(), 1, true);
    LOG(D, "MQTT PUBLISHED DISCOVERY");
  }
  esp_mqtt_client_publish(client, configManager.getMqttConfig().lwtTopic.c_str(), "online", 6, 1, true);
  if (configManager.getMqttConfig().lockEnableCustomState) {
    esp_mqtt_client_subscribe(client, configManager.getMqttConfig().lockCustomStateCmd.c_str(), 0);
  }
  if (configManager.getMiscConfig().proxBatEnabled) {
    esp_mqtt_client_subscribe(client, configManager.getMqttConfig().btrLvlCmdTopic.c_str(), 0);
  }
  esp_mqtt_client_subscribe(client, configManager.getMqttConfig().lockStateCmd.c_str(), 0);
  esp_mqtt_client_subscribe(client, configManager.getMqttConfig().lockCStateCmd.c_str(), 0);
  esp_mqtt_client_subscribe(client, configManager.getMqttConfig().lockTStateCmd.c_str(), 0);
}

void mqtt_data_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  // ESP_LOGD(TAG, "Event dispatched from callback type=%d", event_id);
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  esp_mqtt_client_handle_t client = event->client;
  std::string topic(event->topic, event->topic + event->topic_len);
  std::string data(event->data, event->data + event->data_len);
  LOG(D, "Received message in topic \"%s\": %s", topic.c_str(), data.c_str());
  int state = atoi(data.c_str());
  if (!strcmp(configManager.getMqttConfig().lockCustomStateCmd.c_str(), topic.c_str())) {
    set_custom_state_handler(client, state);
  } else if (!strcmp(configManager.getMqttConfig().lockStateCmd.c_str(), topic.c_str())) {
    set_state_handler(client, state);
  } else if (!strcmp(configManager.getMqttConfig().lockTStateCmd.c_str(), topic.c_str())) {
    if (state == lockStates::UNLOCKED || state == lockStates::LOCKED) {
      lockTargetState->setVal(state);
      esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), state == lockStates::UNLOCKED ? std::to_string(lockStates::UNLOCKING).c_str() : std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
    }
  } else if (!strcmp(configManager.getMqttConfig().lockCStateCmd.c_str(), topic.c_str())) {
    if (state == lockStates::UNLOCKED || state == lockStates::LOCKED || state == lockStates::JAMMED || state == lockStates::UNKNOWN) {
      lockCurrentState->setVal(state);
      esp_mqtt_client_publish(client, configManager.getMqttConfig().lockStateTopic.c_str(), std::to_string(lockCurrentState->getVal()).c_str(), 0, 1, true);
    }
  } else if (!strcmp(configManager.getMqttConfig().btrLvlCmdTopic.c_str(), topic.c_str())) {
    btrLevel->setVal(state);
    if (state <= configManager.getMiscConfig().btrLowStatusThreshold) {
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
  esp_mqtt_client_config_t mqtt_cfg {};
  mqtt_cfg.broker.address.hostname = configManager.getMqttConfig().mqttBroker.c_str();
  mqtt_cfg.broker.address.port = configManager.getMqttConfig().mqttPort;
  mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
  mqtt_cfg.credentials.client_id = configManager.getMqttConfig().mqttClientId.c_str();
  mqtt_cfg.credentials.username = configManager.getMqttConfig().mqttUsername.c_str();
  mqtt_cfg.credentials.authentication.password = configManager.getMqttConfig().mqttPassword.c_str();
  mqtt_cfg.session.last_will.topic = configManager.getMqttConfig().lwtTopic.c_str();
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
  LOG(I, "Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    LOG(I, "- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    LOG(I, " - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      LOG(I, "%s", file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      LOG(I, "%d", file.size());
    }
    file = root.openNextFile();
  }
}

String indexProcess(const String& var) {
  if (var == "VERSION") {
    const esp_app_desc_t* app_desc = esp_app_get_description();
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
        serializedData = configManager.getMqttConfig();
      } else if (std::equal(data->value().begin(), data->value().end(),pages[1].begin(), pages[1].end()) || std::equal(data->value().begin(), data->value().end(),pages[2].begin(), pages[2].end())) {
        LOG(D, "ACTIONS CONFIG REQ");
        serializedData = configManager.getMiscConfig();
      } else if (std::equal(data->value().begin(), data->value().end(),pages[3].begin(), pages[3].end())) {
        LOG(D, "HK DATA REQ");
        json inputData = readerData.getReaderData();
        if (inputData.contains("group_identifier")) {
          serializedData["group_identifier"] = red_log::bufToHexString(readerData.getReaderGid().data(), readerData.getReaderGid().size(), true);
        }
        if (inputData.contains("unique_identifier")) {
          serializedData["unique_identifier"] = red_log::bufToHexString(readerData.getReaderId().data(), readerData.getReaderId().size(), true);
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
  AsyncCallbackWebHandler* ethSuppportConfig = new AsyncCallbackWebHandler();
  webServer.addHandler(ethSuppportConfig);
  ethSuppportConfig->setUri("/eth_get_config");
  ethSuppportConfig->setMethod(HTTP_GET);
  ethSuppportConfig->onRequest([](AsyncWebServerRequest *req) {
    json eth_config;
    eth_config["supportedChips"] = json::array();
    for (auto &&v : eth_config_ns::supportedChips) {
      eth_config.at("supportedChips").push_back(v.second);
    }
    eth_config["boardPresets"] = eth_config_ns::boardPresets;
    eth_config["ethEnabled"] = configManager.getMiscConfig().ethernetEnabled;
    req->send(200, "application/json", eth_config.dump().c_str());
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
        configManager.getMqttConfig();
        configManager.deleteMqttConfig();
        req->send(200, "text/plain", "200 Success");
      } else if (std::equal(data->value().begin(), data->value().end(), pages[1].begin(), pages[1].end())) {
        LOG(D, "ACTIONS CONFIG SEL");
        req->send(200, "text/plain", "200 Success");
      } else if (std::equal(data->value().begin(), data->value().end(), pages[2].begin(), pages[2].end())) {
        LOG(D, "MISC CONFIG SEL");
        configManager.deleteMiscConfig();
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
        configData = configManager.getMqttConfig();
        selConfig = 0;
      } else if (std::equal(data->value().begin(), data->value().end(), pages[1].begin(), pages[1].end())) {
        LOG(D, "ACTIONS CONFIG SEL");
        configData = configManager.getMiscConfig();
        selConfig = 1;
      } else if (std::equal(data->value().begin(), data->value().end(), pages[2].begin(), pages[2].end())) {
        LOG(D, "MISC CONFIG SEL");
        configData = configManager.getMiscConfig();
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
          rfidTopic.append("homeassistant/tag/").append(!clientId.empty() ? clientId : configManager.getMqttConfig().mqttClientId).append("/rfid/config");
          esp_mqtt_client_publish(client, rfidTopic.c_str(), "", 0, 0, false);
        } else if (it.key() == std::string("setupCode")) {
          std::string code = it.value().template get<std::string>();
          if (configManager.getMiscConfig().setupCode.c_str() != it.value() && code.length() == 8) {
            if (homeSpan.controllerListBegin() == homeSpan.controllerListEnd()) {
              homeSpan.setPairingCode(code.c_str());
            }
          }
        } else if (it.key() == std::string("nfcNeopixelPin")) {
          if (configManager.getMiscConfig().nfcNeopixelPin == 255 && it.value() != 255 && neopixel_task_handle == nullptr) {
            xTaskCreate(neopixel_task, "neopixel_task", 4096, NULL, 2, &neopixel_task_handle);
            if (!pixel) {
              pixel = std::make_shared<Pixel>(it.value(), PixelType::GRB);
            }
          } else if (configManager.getMiscConfig().nfcNeopixelPin != 255 && it.value() == 255 && neopixel_task_handle != nullptr) {
            uint8_t status = 2;
            xQueueSend(neopixel_handle, &status, 0);
            neopixel_task_handle = nullptr;
          }
        } else if (it.key() == std::string("nfcSuccessPin")) {
          if (configManager.getMiscConfig().nfcSuccessPin == 255 && it.value() != 255 && gpio_led_task_handle == nullptr) {
            pinMode(it.value(), OUTPUT);
            xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, &gpio_led_task_handle);
          } else if (configManager.getMiscConfig().nfcSuccessPin != 255 && it.value() == 255 && gpio_led_task_handle != nullptr) {
            if (serializedData->contains("nfcFailPin") && serializedData->at("nfcFailPin") == 255) {
              uint8_t status = 2;
              xQueueSend(gpio_led_handle, &status, 0);
              gpio_led_task_handle = nullptr;
            }
          } else if (it.value() != 255) {
            pinMode(it.value(), OUTPUT);
          }
        } else if (it.key() == std::string("nfcFailPin")) {
          if (configManager.getMiscConfig().nfcFailPin == 255 && it.value() != 255 && gpio_led_task_handle == nullptr) {
            pinMode(it.value(), OUTPUT);
            xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, &gpio_led_task_handle);
          } else if (configManager.getMiscConfig().nfcFailPin != 255 && it.value() == 255 && gpio_led_task_handle != nullptr) {
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
          uint8_t pixelType = it.value().template get<uint8_t>();
          if (pixelType != configData.at(it.key()).template get<uint8_t>()) {
            rebootNeeded = true;
            rebootMsg = "Pixel Type was changed, reboot needed! Rebooting...";
          }
        } else if (it.key() == std::string("gpioActionPin")) {
          if (configManager.getMiscConfig().gpioActionPin == 255 && it.value() != 255 ) {
            LOG(D, "ENABLING HomeKit Trigger - Simple GPIO");
            pinMode(it.value(), OUTPUT);
            if(gpio_lock_task_handle == nullptr){
              xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, &gpio_lock_task_handle);
            }
            if(configManager.getMiscConfig().hkDumbSwitchMode){
              serializedData->at("hkDumbSwitchMode") = false;
            }
          } else if (configManager.getMiscConfig().gpioActionPin != 255 && it.value() == 255) {
            LOG(D, "DISABLING HomeKit Trigger - Simple GPIO");
            if( gpio_lock_task_handle != nullptr){
              gpioLockAction status{ .source = gpioLockAction::OTHER, .action = 2 };
              xQueueSend(gpio_lock_handle, &status, 0);
              gpio_lock_task_handle = nullptr;
            }
            gpio_reset_pin(gpio_num_t(configManager.getMiscConfig().gpioActionPin));
          }
        } else if (it.key() == std::string("hkDumbSwitchMode") && gpio_lock_task_handle == nullptr) {
          xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, &gpio_lock_task_handle);
        }
        configData.at(it.key()) = it.value();
      }
      if(selConfig == 0){
        espConfig::mqttConfig_t mqtt;
        configData.get_to<espConfig::mqttConfig_t>(mqtt);
        configManager.saveMqttConfig(mqtt);
      } else if(selConfig == 1){
        espConfig::misc_config_t misc;
        configData.get_to<espConfig::misc_config_t>(misc);
        configManager.saveMiscConfig(misc);
      } else if(selConfig == 2){
        espConfig::misc_config_t misc;
        configData.get_to<espConfig::misc_config_t>(misc);
        configManager.saveMiscConfig(misc);
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
  auto startConfigAP = new AsyncCallbackWebHandler();
  startConfigAP->setUri("/start_config_ap");
  startConfigAP->setMethod(HTTP_GET);
  startConfigAP->onRequest([](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Starting the AP...");
    delay(1000);
    webServer.end();
    homeSpan.processSerialCommand("A");
    });
  webServer.addHandler(startConfigAP);
  auto resetHkHandle = new AsyncCallbackWebHandler();
  resetHkHandle->setUri("/reset_hk_pair");
  resetHkHandle->setMethod(HTTP_GET);
  resetHkHandle->onRequest([](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Erasing HomeKit pairings and restarting...");
    delay(1000);
    readerData.deleteAllReaderData();
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
  if (configManager.getMiscConfig().webAuthEnabled) {
    LOG(I, "Web Authentication Enabled");
    routesHandle->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    dataProvision->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    dataLoad->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    dataClear->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    rootHandle->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    hashPage->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    resetHkHandle->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    resetWifiHandle->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    getWifiRssi->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    startConfigAP->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
    ethSuppportConfig->setAuthentication(configManager.getMiscConfig().webUsername.c_str(), configManager.getMiscConfig().webPassword.c_str());
  }
  webServer.onNotFound(notFound);
  webServer.begin();
}

void wifiCallback(int status) {
  if (status == 1) {
    if (configManager.getMqttConfig().mqttBroker.size() >= 7 && configManager.getMqttConfig().mqttBroker.size() <= 16 && !std::equal(configManager.getMqttConfig().mqttBroker.begin(), configManager.getMqttConfig().mqttBroker.end(), "0.0.0.0")) {
      mqtt_app_start();
    }
    setupWeb();
  }
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
  memcpy(ecpData + 8, readerData.getReaderGid().data(), readerData.getReaderGid().size());
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
        // LOG(D, "Reader Private Key: %s", red_log::bufToHexString(readerData.reader_pk.data(), readerData.reader_pk.size()).c_str());
        auto r = readerData.getReaderData();
        HKAuthenticationContext authCtx([](std::vector<uint8_t>& s, std::vector<uint8_t>& r, bool il) -> bool {uint8_t tr[256]; uint16_t trl = 256; bool st = nfc->inDataExchange(s.data(), s.size(), tr, &trl, il); r.reserve(trl); r.insert(r.begin(), tr, tr+trl); return st;}, 
            r, 
            [](const readerData_t &dataToSave) -> bool {
            return readerData.saveReaderData(dataToSave);
        });
        auto authResult = authCtx.authenticate(hkFlow);
        if (std::get<2>(authResult) != kFlowFailed) {
          bool status = true;
          if (configManager.getMiscConfig().nfcSuccessPin != 255) {
            xQueueSend(gpio_led_handle, &status, 0);
          }
          if (configManager.getMiscConfig().nfcNeopixelPin != 255) {
            xQueueSend(neopixel_handle, &status, 0);
          }
          if ((configManager.getMiscConfig().gpioActionPin != 255 && configManager.getMiscConfig().hkGpioControlledState) || configManager.getMiscConfig().hkDumbSwitchMode) {
            const gpioLockAction action{ .source = gpioLockAction::HOMEKEY, .action = 0 };
            xQueueSend(gpio_lock_handle, &action, 0);
          }
          if (configManager.getMiscConfig().hkAltActionInitPin != 255 && configManager.getMiscConfig().hkAltActionPin != 255) {
            uint8_t status = 2;
            xQueueSend(gpio_led_handle, &status, 0);
          }
          if (hkAltActionActive) {
            mqtt_publish(configManager.getMqttConfig().hkAltActionTopic, "alt_action", 0, false);
          }
          json payload;
          payload["issuerId"] = hex_representation(std::get<0>(authResult));
          payload["endpointId"] = hex_representation(std::get<1>(authResult));
          payload["readerId"] = hex_representation(readerData.getReaderId());
          payload["homekey"] = true;
          std::string payloadStr = payload.dump();
          mqtt_publish(configManager.getMqttConfig().hkTopic, payloadStr, 0, false);
          if (configManager.getMiscConfig().lockAlwaysUnlock) {
            if (configManager.getMiscConfig().gpioActionPin == 255 || !configManager.getMiscConfig().hkGpioControlledState) {
              lockCurrentState->setVal(lockStates::UNLOCKED);
              lockTargetState->setVal(lockStates::UNLOCKED);
              mqtt_publish(configManager.getMqttConfig().lockStateTopic, std::to_string(lockStates::UNLOCKED), 1, true);
            }
            if (configManager.getMqttConfig().lockEnableCustomState) {
              mqtt_publish(configManager.getMqttConfig().lockCustomStateTopic, std::to_string(configManager.getMqttConfig().customLockActions.at("UNLOCK")), 0, false);
            }
          } else if (configManager.getMiscConfig().lockAlwaysLock) {
            if (configManager.getMiscConfig().gpioActionPin == 255 || configManager.getMiscConfig().hkGpioControlledState) {
              lockCurrentState->setVal(lockStates::LOCKED);
              lockTargetState->setVal(lockStates::LOCKED);
              mqtt_publish(configManager.getMqttConfig().lockStateTopic, std::to_string(lockStates::LOCKED), 1, true);
            }
            if (configManager.getMqttConfig().lockEnableCustomState) {
              mqtt_publish(configManager.getMqttConfig().lockCustomStateTopic, std::to_string(configManager.getMqttConfig().customLockActions.at("LOCK")), 0, false);
            }
          } else {
            int currentState = lockCurrentState->getVal();
            if (configManager.getMqttConfig().lockEnableCustomState) {
              if (currentState == lockStates::UNLOCKED) {
                mqtt_publish(configManager.getMqttConfig().lockCustomStateTopic, std::to_string(configManager.getMqttConfig().customLockActions.at("LOCK")), 0, false);
              } else if (currentState == lockStates::LOCKED) {
                mqtt_publish(configManager.getMqttConfig().lockCustomStateTopic, std::to_string(configManager.getMqttConfig().customLockActions.at("UNLOCK")), 0, false);
              }
            }
          }

          auto stopTime = std::chrono::high_resolution_clock::now();
          LOG(I, "Total Time (detection->auth->gpio->mqtt): %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
        } else {
          bool status = false;
          if (configManager.getMiscConfig().nfcFailPin != 255) {
            xQueueSend(gpio_led_handle, &status, 0);
          }
          if (configManager.getMiscConfig().nfcNeopixelPin != 255) {
            xQueueSend(neopixel_handle, &status, 0);
          }
          LOG(W, "We got status FlowFailed, mqtt untouched!");
        }
        nfc->setRFField(0x02, 0x01);
      } else if(!configManager.getMqttConfig().nfcTagNoPublish) {
        LOG(W, "Invalid Response, probably not Homekey, publishing target's UID");
        bool status = false;
        if (configManager.getMiscConfig().nfcFailPin != 255) {
          xQueueSend(gpio_led_handle, &status, 0);
        }
        if (configManager.getMiscConfig().nfcNeopixelPin != 255) {
          xQueueSend(neopixel_handle, &status, 0);
        }
        json payload;
        payload["atqa"] = hex_representation(std::vector<uint8_t>(atqa, atqa + 2));
        payload["sak"] = hex_representation(std::vector<uint8_t>(sak, sak + 1));
        payload["uid"] = hex_representation(std::vector<uint8_t>(uid, uid + uidLen));
        payload["homekey"] = false;
        std::string payload_dump = payload.dump();
        if (client != nullptr) {
          esp_mqtt_client_publish(client, configManager.getMqttConfig().hkTopic.c_str(), payload_dump.c_str(), 0, 0, false);
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

void onEvent(arduino_event_id_t event, arduino_event_info_t info) {
  uint8_t mac[6] = { 0, 0, 0, 0, 0, 0 };
  char macStr[13] = {0};
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      LOG(I, "ETH Started");
      ETH.macAddress(mac);
      sprintf(macStr, "ESP32_%02X%02X%02X", mac[0], mac[1], mac[2]);
      ETH.setHostname(macStr);
      break;
    case ARDUINO_EVENT_ETH_CONNECTED: LOG(I, "ETH Connected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:    LOG(I, "ETH Got IP: '%s'\n", esp_netif_get_desc(info.got_ip.esp_netif)); break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      LOG(I, "ETH Lost IP");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      LOG(I, "ETH Disconnected");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      LOG(I, "ETH Stopped");
      break;
    default: break;
  }
}

void setup() {
  Serial.begin(115200);
  const esp_app_desc_t* app_desc = esp_app_get_description();
  std::string app_version = app_desc->version;
  gpio_led_handle = xQueueCreate(2, sizeof(uint8_t));
  neopixel_handle = xQueueCreate(2, sizeof(uint8_t));
  gpio_lock_handle = xQueueCreate(2, sizeof(gpioLockAction));
  size_t len;
  const char* TAG = "SETUP";
  configManager.begin();
  pn532spi = new PN532_SPI(configManager.getMiscConfig().nfcGpioPins[0], configManager.getMiscConfig().nfcGpioPins[1], configManager.getMiscConfig().nfcGpioPins[2], configManager.getMiscConfig().nfcGpioPins[3]);
  nfc = new PN532(*pn532spi);
  nfc->begin();
  if (configManager.getMiscConfig().nfcSuccessPin && configManager.getMiscConfig().nfcSuccessPin != 255) {
    pinMode(configManager.getMiscConfig().nfcSuccessPin, OUTPUT);
    digitalWrite(configManager.getMiscConfig().nfcSuccessPin, !configManager.getMiscConfig().nfcSuccessHL);
  }
  if (configManager.getMiscConfig().nfcFailPin && configManager.getMiscConfig().nfcFailPin != 255) {
    pinMode(configManager.getMiscConfig().nfcFailPin, OUTPUT);
    digitalWrite(configManager.getMiscConfig().nfcFailPin, !configManager.getMiscConfig().nfcFailHL);
  }
  if (configManager.getMiscConfig().gpioActionPin && configManager.getMiscConfig().gpioActionPin != 255) {
    pinMode(configManager.getMiscConfig().gpioActionPin, OUTPUT);
  }
  if (configManager.getMiscConfig().hkAltActionInitPin != 255) {
    pinMode(configManager.getMiscConfig().hkAltActionInitPin, INPUT);
    if (configManager.getMiscConfig().hkAltActionPin != 255) {
      pinMode(configManager.getMiscConfig().hkAltActionPin, OUTPUT);
    }
    if (configManager.getMiscConfig().hkAltActionInitLedPin != 255) {
      pinMode(configManager.getMiscConfig().hkAltActionInitLedPin, OUTPUT);
    }
  }
  if (!LittleFS.begin(true)) {
    LOG(I, "An Error has occurred while mounting LITTLEFS");
    return;
  }
  listDir(LittleFS, "/", 0);
  LOG(I, "LittleFS used space: %d / %d", LittleFS.usedBytes(), LittleFS.totalBytes());
  if (configManager.getMiscConfig().ethernetEnabled) {
    Network.onEvent(onEvent);
    if (configManager.getMiscConfig().ethActivePreset != 255) {
      if (configManager.getMiscConfig().ethActivePreset >= eth_config_ns::boardPresets.size()) {
        LOG(E, "Invalid preset index, not initializing ethernet!");
      } else {
        eth_board_presets_t ethPreset = eth_config_ns::boardPresets[configManager.getMiscConfig().ethActivePreset];
        if (!ethPreset.ethChip.emac) {
          ETH.begin(ethPreset.ethChip.phy_type, 1, ethPreset.spi_conf.pin_cs, ethPreset.spi_conf.pin_irq, ethPreset.spi_conf.pin_rst, SPI2_HOST, ethPreset.spi_conf.pin_sck, ethPreset.spi_conf.pin_miso, ethPreset.spi_conf.pin_mosi, ethPreset.spi_conf.spi_freq_mhz);
        } else {
  #if CONFIG_ETH_USE_ESP32_EMAC
          ETH.begin(ethPreset.ethChip.phy_type, ethPreset.rmii_conf.phy_addr, ethPreset.rmii_conf.pin_mcd, ethPreset.rmii_conf.pin_mdio, ethPreset.rmii_conf.pin_power, ethPreset.rmii_conf.pin_rmii_clock);
  #else
          LOG(E, "Selected a chip without MAC but %s doesn't have a builtin MAC, cannot initialize ethernet!", CONFIG_IDF_TARGET);
  #endif
        }
      }
    } else if (configManager.getMiscConfig().ethActivePreset == 255) {
      eth_chip_desc_t chipType = eth_config_ns::supportedChips[eth_phy_type_t(configManager.getMiscConfig().ethPhyType)];
      if (!chipType.emac) {
        ETH.begin(chipType.phy_type, 1, configManager.getMiscConfig().ethSpiConfig[1], configManager.getMiscConfig().ethSpiConfig[2], configManager.getMiscConfig().ethSpiConfig[3], SPI2_HOST, configManager.getMiscConfig().ethSpiConfig[4], configManager.getMiscConfig().ethSpiConfig[5], configManager.getMiscConfig().ethSpiConfig[6], configManager.getMiscConfig().ethSpiConfig[0]);
      } else {
#if CONFIG_ETH_USE_ESP32_EMAC
        ETH.begin(chipType.phy_type, configManager.getMiscConfig().ethRmiiConfig[0], configManager.getMiscConfig().ethRmiiConfig[1], configManager.getMiscConfig().ethRmiiConfig[2], configManager.getMiscConfig().ethRmiiConfig[3], eth_clock_mode_t(configManager.getMiscConfig().ethRmiiConfig[4]));
#endif
      }
    }
  }
  if (configManager.getMiscConfig().controlPin != 255) {
    homeSpan.setControlPin(configManager.getMiscConfig().controlPin);
  }
  if (configManager.getMiscConfig().hsStatusPin != 255) {
    homeSpan.setStatusPin(configManager.getMiscConfig().hsStatusPin);
  }
  homeSpan.setStatusAutoOff(15);
  homeSpan.setLogLevel(0);
  homeSpan.setSketchVersion(app_version.c_str());
  homeSpan.enableAutoStartAP();
  homeSpan.enableOTA(configManager.getMiscConfig().otaPasswd.c_str());
  homeSpan.setPortNum(1201);
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);
  char macStr[9] = { 0 };
  sprintf(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
  homeSpan.setHostNameSuffix(macStr);
  homeSpan.begin(Category::Locks, configManager.getMiscConfig().deviceName.c_str(), "HK-", "HomeKey-ESP32");

  new SpanUserCommand('D', "Delete Home Key Data", [](const char *){readerData.deleteAllReaderData();});
  new SpanUserCommand('L', "Set Log Level", setLogLevel);
  new SpanUserCommand('F', "Set HomeKey Flow", setFlow);
  new SpanUserCommand('P', "Print Issuers", print_issuers);
  new SpanUserCommand('M', "Erase MQTT Config and restart", [](const char*){configManager.deleteMqttConfig();});
  // new SpanUserCommand('R', "Remove Endpoints", [](const char*) {
  //   for (auto&& issuer : readerData.issuers) {
  //     issuer.endpoints.clear();
  //   }
  //   save_to_nvs();
  //   });
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
  new NFCAccessoryInformation();
  new Service::HAPProtocolInformation();
  new Characteristic::Version();
  new LockManagement();
  new LockMechanism();
  new NFCAccess();
  if (configManager.getMiscConfig().proxBatEnabled) {
    new PhysicalLockBattery();
  }

  LOG(I, "READER GROUP ID (%d): %s", readerData.getReaderGid().size(), red_log::bufToHexString(readerData.getReaderGid().data(), readerData.getReaderGid().size()).c_str());
  LOG(I, "READER UNIQUE ID (%d): %s", readerData.getReaderId().size(), red_log::bufToHexString(readerData.getReaderId().data(), readerData.getReaderId().size()).c_str());

  LOG(I, "HOMEKEY ISSUERS: %d", readerData.getReaderData().issuers.size());
  for (auto&& issuer : readerData.getReaderData().issuers) {
    LOG(D, "Issuer ID: %s, Public Key: %s", red_log::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str(), red_log::bufToHexString(issuer.issuer_pk.data(), issuer.issuer_pk.size()).c_str());
  }

  homeSpan.setControllerCallback(pairCallback);
  homeSpan.setConnectionCallback(wifiCallback);
  if (configManager.getMiscConfig().nfcNeopixelPin != 255) {
    pixel = std::make_shared<Pixel>(configManager.getMiscConfig().nfcNeopixelPin, pixelTypeMap[configManager.getMiscConfig().neoPixelType]);
    xTaskCreate(neopixel_task, "neopixel_task", 4096, NULL, 2, &neopixel_task_handle);
  }
  if (configManager.getMiscConfig().nfcSuccessPin != 255 || configManager.getMiscConfig().nfcFailPin != 255) {
    xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, &gpio_led_task_handle);
  }
  if (configManager.getMiscConfig().gpioActionPin != 255 || configManager.getMiscConfig().hkDumbSwitchMode) {
    xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, &gpio_lock_task_handle);
  }
  if (configManager.getMiscConfig().hkAltActionInitPin != 255) {
    xTaskCreate(alt_action_task, "alt_action_task", 2048, NULL, 2, &alt_action_task_handle);
  }
  xTaskCreate(nfc_thread_entry, "nfc_task", 8192, NULL, 1, &nfc_poll_task);
}

//////////////////////////////////////

void loop() {
  homeSpan.poll();
  vTaskDelay(5);
}
