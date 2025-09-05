#include "config.hpp"
#include "eventStructs.hpp"
#include "HomeKitLock.hpp"
#include <functional>
#include <sodium/crypto_sign.h>
#include <sodium/crypto_box.h>
#include "HAP.h"
#include "HomeSpan.h"
#include "LockManager.hpp"
#include "ConfigManager.hpp"
#include "ReaderDataManager.hpp"
#include "HK_HomeKit.h"
#include "esp_mac.h"
#include "utils.hpp"

const char* HomeKitLock::TAG = "HomeKitBridge";
static HomeKitLock* s_instance = nullptr;

HomeKitLock::HomeKitLock(std::function<void(int)> &conn_cb, LockManager& lockManager, ConfigManager& configManager, ReaderDataManager& readerDataManager)
    : m_lockManager(lockManager),
      m_configManager(configManager),
      m_readerDataManager(readerDataManager),
      conn_cb(conn_cb) {
    if (s_instance != nullptr) {
        ESP_LOGE(TAG, "ERROR: More than one instance of HomeKitBridge created!");
        esp_restart();
    }
    s_instance = this;
    espp::EventManager::get().add_publisher("homekit/internal", "HomeKitLock");
    espp::EventManager::get().add_subscriber(
        "lock/stateChanged", "HomeKitLock",
        [&](const std::vector<uint8_t> &data) {
          std::error_code ec;
          EventLockState s = alpaca::deserialize<EventLockState>(data, ec);
          if(!ec) {
            updateLockState(s.currentState, s.targetState);
          }
        }, 3072); 
    espp::EventManager::get().add_subscriber(
        "homekit/event", "HomeKitLock",
        [&](const std::vector<uint8_t> &data) {
          std::error_code ec;
          HomekitEvent event = alpaca::deserialize<HomekitEvent>(data, ec);
          if(ec) return;
          EventValueChanged s = alpaca::deserialize<EventValueChanged>(event.data, ec);
          if(ec) return;
          switch(event.type) {
            case HomekitEventType::SETUP_CODE_CHANGED:
              homeSpan.setPairingCode(s.str.c_str(), false);
              break;
            case HomekitEventType::BTR_PROP_CHANGED:
              if(s.name == "btrLevel") {
                updateBatteryStatus(s.newValue, m_statusLowBattery->getVal());
              } else if(s.name == "btrLowThreshold"){
                updateBatteryStatus(m_batteryLevel->getVal(), s.newValue);
              }
              break;
            default:
            break;
          }
        }, 3072); 
}

void HomeKitLock::begin() {
    const auto& miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();
    const auto& app_version = esp_app_get_description()->version;
    ESP_LOGI(TAG, "Starting HomeSpan setup...");

    if (miscConfig.controlPin != 255) homeSpan.setControlPin(miscConfig.controlPin);
    if (miscConfig.hsStatusPin != 255) homeSpan.setStatusPin(miscConfig.hsStatusPin);
    homeSpan.setStatusAutoOff(15);
    homeSpan.setLogLevel(0);
    homeSpan.setSketchVersion(app_version);
    homeSpan.enableAutoStartAP();
    homeSpan.enableOTA(miscConfig.otaPasswd.c_str());
    homeSpan.setPortNum(1201);
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    std::string macStr = fmt::format("{:02X}{:02X}{:02X}{:02X}", mac[0], mac[1], mac[2], mac[3]);
    homeSpan.setHostNameSuffix(macStr.c_str());

    m_readerDataManager.begin();

    homeSpan.begin(Category::Locks, miscConfig.deviceName.c_str(), "HK-", "HomeKey-ESP32");

    new SpanAccessory();
      new NFCAIS(miscConfig);
      new Service::HAPProtocolInformation();
      new Characteristic::Version();
      new LockManagementService();
      new LockMechanismService(*this, m_lockManager);
      new NFCAccessService(m_readerDataManager);
      if(miscConfig.proxBatEnabled) {
          new PhysicalLockBatteryService(*this);
      }

    setupDebugCommands();
    
    homeSpan.setControllerCallback(staticControllerCallback);
    homeSpan.setConnectionCallback(connectionEstablished);
    ESP_LOGI(TAG, "HomeSpan setup complete.");
}

void HomeKitLock::setupDebugCommands() {
    new SpanUserCommand('D', "Delete Home Key Data", [](const char* c) {
        s_instance->m_readerDataManager.deleteAllReaderData();
        ESP_LOGI(TAG, "HomeKey Reader Data deleted.");
    });

    new SpanUserCommand('L', "Set Log Level", [](const char *buf){
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
      esp_log_level_set("HardwareManager", level);
      esp_log_level_set("ReaderDataManager", level);
      esp_log_level_set("LockManager", level);
      esp_log_level_set("MqttManager", level);
      esp_log_level_set("WebServerManager", level);
      esp_log_level_set("ConfigManager", level);
      esp_log_level_set("NfcManager", level);
    });
    new SpanUserCommand('F', "Set HomeKey Flow", [](const char *buf){
      KeyFlow hkFlow = KeyFlow::kFlowFAST;
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
    EventValueChanged s{.newValue = static_cast<uint8_t>(hkFlow)};
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    HomekitEvent event{.type=DEBUG_AUTH_FLOW, .data=d};
    std::vector<uint8_t> event_data;
    alpaca::serialize(event, event_data);
    espp::EventManager::get().publish("homekit/internal", event_data);
    });
    new SpanUserCommand('M', "Erase MQTT Config and restart", [](const char*){s_instance->m_configManager.deleteConfig<espConfig::mqttConfig_t>();});
    new SpanUserCommand('N', "Btr status low", [](const char* arg) {
      const char* TAG = "BTR_LOW";
      if (strncmp(arg + 1, "0", 1) == 0) {
        s_instance->m_statusLowBattery->setVal(0);
         LOG(I, "Low status set to NORMAL");
      } else if (strncmp(arg + 1, "1", 1) == 0) {
        s_instance->m_statusLowBattery->setVal(1);
         LOG(I, "Low status set to LOW");
      }
    });
    new SpanUserCommand('B', "Btr level", [](const char* arg) {
      uint8_t level = atoi(static_cast<const char *>(arg + 1));
      s_instance->m_batteryLevel->setVal(level);
    });

    new SpanUserCommand('P', "Print Issuers", [](const char* c) {
        const auto& issuers = s_instance->m_readerDataManager.getReaderData().issuers;
        ESP_LOGI(TAG, "--- Registered HomeKey Issuers ---");
        if (issuers.empty()) {
            ESP_LOGI(TAG, "None");
        }
        for(const auto& issuer : issuers) {
             ESP_LOGI(TAG, "ID: %s, PK: %s",
                 fmt::format("{:02X}", fmt::join(issuer.issuer_id, "")).c_str(),
                 fmt::format("{:02X}", fmt::join(issuer.issuer_pk, "")).c_str());
        }
        ESP_LOGI(TAG, "------------------------------------");
    });
}


void HomeKitLock::updateLockState(int currentState, int targetState) {
    if (m_lockCurrentState->getNewVal() != currentState) {
        m_lockCurrentState->setVal(currentState);
    }
    if (m_lockTargetState->getNewVal() != targetState) {
        m_lockTargetState->setVal(targetState);
    }
}

void HomeKitLock::updateBatteryStatus(uint8_t batteryLevel, bool isLow) {
    if (m_batteryLevel && m_batteryLevel->getVal() != batteryLevel) {
        m_batteryLevel->setVal(batteryLevel);
    }
    if (m_statusLowBattery && m_statusLowBattery->getVal() != (int)isLow) {
        m_statusLowBattery->setVal(isLow);
    }
}

void HomeKitLock::staticControllerCallback() {
    if (s_instance) s_instance->controllerCallback();
}

void HomeKitLock::connectionEstablished(int status) {
    if (s_instance) s_instance->conn_cb(status);
}

void HomeKitLock::controllerCallback() {
    ESP_LOGI(TAG, "HomeKit controller list changed.");
    if (HAPClient::nAdminControllers() == 0) {
        ESP_LOGW(TAG, "Last controller unpaired. Wiping HomeKey data.");
        m_readerDataManager.deleteAllReaderData();
        return;
    }
    bool dataChanged = false;
    for (auto it = homeSpan.controllerListBegin(); it != homeSpan.controllerListEnd(); ++it) {
        std::vector<uint8_t> issuerId = Utils::getHashIdentifier(it->getLTPK(), 32);
        if (m_readerDataManager.addIssuerIfNotExists(issuerId, it->getLTPK())) {
            dataChanged = true;
        }
    }
    if(dataChanged) {
        ESP_LOGI(TAG, "New issuers added, saving reader data to NVS.");
        if (!m_readerDataManager.saveData()) {
            ESP_LOGE(TAG, "Failed to save updated reader data after pairing!");
        }
    }
}
