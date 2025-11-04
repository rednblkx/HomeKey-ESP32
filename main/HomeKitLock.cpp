#include "config.hpp"
#include "esp_log.h"
#include "eth_structs.hpp"
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

/**
 * @brief Initialize the HomeKitLock singleton, register internal event publishers/subscribers, and store manager callbacks.
 *
 * Enforces a single instance (calls esp_restart() if another instance exists), registers the "homekit/internal" publisher,
 * and subscribes to "lock/stateChanged" and "homekit/event" to keep lock state, pairing code, and battery status synchronized.
 *
 * @param conn_cb Callback invoked when connection status changes; receives an integer status code.
 * @param lockManager Reference to the LockManager used to control and query lock state.
 * @param configManager Reference to the ConfigManager used for configuration access.
 * @param readerDataManager Reference to the ReaderDataManager used to manage reader/issuer data.
 */
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

/**
 * @brief Handle Ethernet events, update hostname at start, and log state changes.
 *
 * This event handler responds to Arduino Ethernet events: on start it retrieves
 * the MAC address, constructs and sets a hostname derived from the MAC; on
 * connection, disconnection, IP acquisition/loss, and stop events it logs the
 * corresponding state change. For the GOT_IP event the network interface
 * descriptor from `info` is included in the log.
 *
 * @param event The Ethernet event identifier (arduino_event_id_t).
 * @param info  Event-specific data; used for logging the network interface
 *              descriptor when the event is ARDUINO_EVENT_ETH_GOT_IP.
 */
void HomeKitLock::ethEventHandler(arduino_event_id_t event, arduino_event_info_t info) {
  uint8_t mac[6] = { 0, 0, 0, 0, 0, 0 };
  char macStr[13] = {0};
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      ESP_LOGI(TAG,"ETH Started");
      // ETH.macAddress(mac);
      // sprintf(macStr, "ESP32_%02X%02X%02X", mac[0], mac[1], mac[2]);
      // ETH.setHostname(macStr);
      break;
    case ARDUINO_EVENT_ETH_CONNECTED: ESP_LOGI(TAG,"ETH Connected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:    ESP_LOGI(TAG,"ETH Got IP: '{}'\n", esp_netif_get_desc(info.got_ip.esp_netif)); break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      ESP_LOGI(TAG,"ETH Lost IP");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      ESP_LOGI(TAG,"ETH Disconnected");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      ESP_LOGI(TAG,"ETH Stopped");
      break;
    default: break;
  }
}

/**
 * @brief Initializes the Ethernet subsystem according to saved configuration.
 *
 * Initializes and starts Ethernet if enabled in configuration, registers the Ethernet
 * event handler, and applies either a selected board preset or a custom pin/PHY
 * configuration. If Ethernet is disabled, or if the active preset/index or custom
 * PHY is invalid or unsupported, the function logs an error and exits without
 * initializing Ethernet.
 *
 * Observable behaviours:
 * - Registers ethEventHandler with Network for Ethernet events when initialization begins.
 * - Uses a preset configuration when `ethActivePreset` is a valid index; otherwise
 *   uses custom configuration from `miscConfig`.
 * - If a configuration requires a built-in EMAC but the build does not include
 *   CONFIG_ETH_USE_ESP32_EMAC, logs an error and does not initialize Ethernet.
 */
void HomeKitLock::initializeETH(){
  const auto& miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();


    if (!miscConfig.ethernetEnabled) {
        ESP_LOGI(TAG, "Ethernet is disabled. HomeSpan will manage Wi-Fi.");
        return; // Do nothing and let HomeSpan handle Wi-Fi
    }

    ESP_LOGI(TAG,"Ethernet is enabled. Initializing...");
    Network.onEvent(ethEventHandler);

    // --- Preset-based Configuration ---
    if (miscConfig.ethActivePreset != 255) {
        if (miscConfig.ethActivePreset >= eth_config_ns::boardPresets.size()) {
            ESP_LOGE(TAG,"Invalid ethActivePreset index (%d). Not initializing Ethernet.", miscConfig.ethActivePreset);
            return;
        }

        const eth_board_presets_t& ethPreset = eth_config_ns::boardPresets[miscConfig.ethActivePreset];
        ESP_LOGI(TAG,"Initializing with preset: %s", ethPreset.name.c_str());

        if (!ethPreset.ethChip.emac) {
            // SPI-based Ethernet Module
            const auto& spiConf = ethPreset.spi_conf;
            ETH.begin(ethPreset.ethChip.phy_type, 1, spiConf.pin_cs, spiConf.pin_irq, spiConf.pin_rst,
                      SPI2_HOST, spiConf.pin_sck, spiConf.pin_miso, spiConf.pin_mosi, spiConf.spi_freq_mhz);
        } else {
            // Internal MAC (RMII) Ethernet Module
            #if CONFIG_ETH_USE_ESP32_EMAC
            const auto& rmiiConf = ethPreset.rmii_conf;
            ETH.begin(ethPreset.ethChip.phy_type, rmiiConf.phy_addr, rmiiConf.pin_mcd, rmiiConf.pin_mdio,
                      rmiiConf.pin_power, rmiiConf.pin_rmii_clock);
            #else
            ESP_LOGE(TAG,"Preset requires EMAC, but this board does not have a built-in Ethernet MAC.");
            #endif
        }
    }
    // --- Custom Configuration ---
    else {
        ESP_LOGI(TAG,"Initializing with custom pin configuration.");
        auto phy_type = static_cast<eth_phy_type_t>(miscConfig.ethPhyType);
        
        if (eth_config_ns::supportedChips.count(phy_type) == 0) {
            ESP_LOGE(TAG,"Custom phy_type (%d) is not supported.", miscConfig.ethPhyType);
            return;
        }

        const eth_chip_desc_t& chipType = eth_config_ns::supportedChips.at(phy_type);
        
        if (!chipType.emac) {
            // Custom SPI pins
            const auto& spiConf = miscConfig.ethSpiConfig;
            ETH.begin(chipType.phy_type, 1, spiConf[1], spiConf[2], spiConf[3],
                      SPI2_HOST, spiConf[4], spiConf[5], spiConf[6], spiConf[0]);
        } else {
            // Custom RMII pins
            #if CONFIG_ETH_USE_ESP32_EMAC
            const auto& rmiiConf = miscConfig.ethRmiiConfig;
            ETH.begin(chipType.phy_type, rmiiConf[0], rmiiConf[1], rmiiConf[2], rmiiConf[3],
                      static_cast<eth_clock_mode_t>(rmiiConf[4]));
            #else
            ESP_LOGE(TAG,"Custom config requires EMAC, but this board does not have a built-in Ethernet MAC.");
            #endif
        }
    }
}
/**
 * @brief Initialize HomeSpan, expose lock-related accessories/services, and register runtime callbacks.
 *
 * Configures HomeSpan using settings from ConfigManager (pins, OTA password, port, host name suffix), initializes reader data handling, creates the lock accessory and its services/characteristics (including lock mechanism, management, NFC access, protocol/version, and optional physical battery service), installs developer debug commands, and registers controller and connection callbacks.
 */
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
    const std::string macStr = fmt::format("{:02X}{:02X}{:02X}{:02X}", mac[2], mac[3], mac[4], mac[5]);
    homeSpan.setHostNameSuffix(macStr.c_str());

    m_readerDataManager.begin();
    initializeETH();

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

/**
 * @brief Register developer/user debug commands for HomeKit lock operations.
 *
 * Installs a set of SpanUserCommand handlers that allow interactive/debug control of
 * runtime behaviors and data related to HomeKey, logging, authentication flow,
 * MQTT configuration, battery state, and registered issuers.
 *
 * Available commands:
 * - 'D' : Delete all HomeKey reader data.
 * - 'L' : Set runtime log level for multiple subsystems (E/W/I/D/V/N).
 * - 'F' : Select HomeKey authentication flow (FAST, STANDARD, ATTESTATION) and publish a debug auth-flow event.
 * - 'M' : Erase stored MQTT configuration.
 * - 'N' : Toggle battery-low status (0 = normal, 1 = low).
 * - 'B' : Set battery level percentage.
 * - 'P' : Print registered HomeKey issuers (issuer IDs and public keys).
 */
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


/**
 * @brief Update the accessory's current and target lock state characteristics.
 *
 * Applies the provided current and target lock state values to the corresponding HomeKit characteristics
 * when those values differ from the characteristics' existing values.
 *
 * @param currentState The current lock state to set (use HAP lock state values appropriate for the platform).
 * @param targetState The desired/target lock state to set (use HAP lock target state values appropriate for the platform).
 */
void HomeKitLock::updateLockState(int currentState, int targetState) {
    if (m_lockCurrentState->getNewVal() != currentState) {
        m_lockCurrentState->setVal(currentState);
    }
    if (m_lockTargetState->getNewVal() != targetState) {
        m_lockTargetState->setVal(targetState);
    }
}

/**
 * @brief Update HomeKit battery level and low-battery status characteristics.
 *
 * Updates the accessory's battery level characteristic and the low-battery status
 * characteristic only when the corresponding characteristic exists and its value
 * differs from the provided one.
 *
 * @param batteryLevel Battery charge percentage (0–100) to set on the accessory.
 * @param isLow True if the battery should be marked as low, false otherwise.
 */
void HomeKitLock::updateBatteryStatus(uint8_t batteryLevel, bool isLow) {
    if (m_batteryLevel && m_batteryLevel->getVal() != batteryLevel) {
        m_batteryLevel->setVal(batteryLevel);
    }
    if (m_statusLowBattery && m_statusLowBattery->getVal() != (int)isLow) {
        m_statusLowBattery->setVal(isLow);
    }
}

/**
 * @brief Relay HomeSpan controller-list change notifications to the singleton instance.
 *
 * Invoked as a static callback; if a HomeKitLock singleton exists, forwards the event by calling
 * the instance's controllerCallback() method.
 */
void HomeKitLock::staticControllerCallback() {
    if (s_instance) s_instance->controllerCallback();
}

/**
 * @brief Invoke the configured connection callback with the provided status if the singleton exists.
 *
 * Calls the instance's connection callback passing `status` when the global singleton `s_instance` is set.
 *
 * @param status Connection status code forwarded to the callback.
 */
void HomeKitLock::connectionEstablished(int status) {
    if (s_instance) s_instance->conn_cb(status);
}

/**
 * @brief Handle changes to the HomeKit controller list and synchronize stored HomeKey issuers.
 *
 * When called, this updates the ReaderDataManager to match the current set of paired controllers:
 * - If there are zero admin controllers, deletes all stored reader data.
 * - Otherwise, ensures each controller's LTPK has a corresponding issuer entry and, if any new issuers
 *   were added, attempts to persist the updated reader data to NVS.
 *
 * Side effects: may delete reader data, add issuer entries, and write data to NVS; logs success or failure.
 */
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
