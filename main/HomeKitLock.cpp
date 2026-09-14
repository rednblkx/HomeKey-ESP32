#include "HardwareManager.hpp"
#include "EthernetDriver.hpp"
#include "fmt/ranges.h"
#include "config.hpp"
#include "esp_log.h"
#include "eth_structs.hpp"
#include "eventStructs.hpp"
#include "HomeKitLock.hpp"
#include <cstdint>
#include <functional>
#include <sodium/crypto_sign.h>
#include <sodium/crypto_box.h>
#include "HAP.h"
#include "HomeSpan.h"
#include "LockManager.hpp"
#include "ConfigManager.hpp"
#include "ReaderDataManager.hpp"
#include "esp_mac.h"
#include "hal/spi_types.h"
#include "utils.hpp"
#include "SharedLed.hpp"

const char* HomeKitLock::TAG = "HomeKitBridge";
static HomeKitLock* s_instance = nullptr;


/**
 * @brief Construct the HomeKitLock singleton, register internal event publishers/subscribers, and store manager callbacks.
 *
 * Initializes the singleton instance, registers the internal HomeKit EventBus topic, and subscribes to internal HomeKit events required
 * to synchronize pairing code and battery-related characteristics. If a second instance is created, logs an error and restarts the device.
 *
 * @param conn_cb Callback invoked when connection status changes; receives an integer status code.
 * @param lockManager Reference to the LockManager used to control and query lock state.
 * @param configManager Reference to the ConfigManager used for configuration access.
 * @param readerDataManager Reference to the ReaderDataManager used to manage reader/issuer data.
 */
HomeKitLock::HomeKitLock(std::function<void(int)> &conn_cb, LockManager& lockManager, ConfigManager& configManager, NvsCredentialStore& readerDataManager)
    : m_lockManager(lockManager),
      m_configManager(configManager),
      m_readerDataManager(readerDataManager),
      conn_cb(conn_cb)
{
    if (s_instance != nullptr) {
        ESP_LOGE(TAG, "ERROR: More than one instance of HomeKitBridge created!");
        esp_restart();
    }
    s_instance = this;
    m_hk_event = AppEventLoop::subscribe(HK_EVENT, HK_INTERNAL_EVENT, [&](const uint8_t* data, size_t size){
      if(size == 0 || data == nullptr) return;
      std::span<const uint8_t> payload(data, size);
      std::error_code ec;
      HomekitEvent hk_event = alpaca::deserialize<HomekitEvent>(payload, ec);
      if(ec) { ESP_LOGE(TAG, "Failed to deserialize HomeKit event: %s", ec.message().c_str()); return; }
      switch(hk_event.type) {
          case HomekitEventType::SETUP_CODE_CHANGED:{
              EventValueChanged s = alpaca::deserialize<EventValueChanged>(hk_event.data, ec);
              if(ec) { ESP_LOGE(TAG, "Failed to deserialize EventValueChanged event: %s", ec.message().c_str()); return; }
              homeSpan.setPairingCode(s.str.c_str(), false);
          }
          break;
          case HomekitEventType::BTR_PROP_CHANGED:{
              EventValueChanged s = alpaca::deserialize<EventValueChanged>(hk_event.data, ec);
              if(ec) { ESP_LOGE(TAG, "Failed to deserialize EventValueChanged event: %s", ec.message().c_str()); return; }
              if(s.name == "btrLevel") {
                  updateBatteryStatus(s.newValue, m_statusLowBattery->getVal());
              } else if(s.name == "btrLowThreshold"){
                  updateBatteryStatus(m_batteryLevel->getVal(), s.newValue);
              }
          }
          break;
          default:
          break;
      }
  });
}

/**
 * @brief Initializes the Ethernet subsystem according to saved configuration.
 *
 * Delegates to EthernetDriver::start(), which owns the ethernet event handler,
 * GPIO allocation, and driver bring-up. See EthernetDriver.cpp for the
 * observable behaviours.
 */
void HomeKitLock::initializeETH() {
  const auto& miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();
  EthernetDriver::start(miscConfig);
}
/**
 * @brief Initialize HomeSpan, expose lock-related accessories/services, and register runtime callbacks.
 *
 * Configures HomeSpan using settings from ConfigManager (pins, OTA password, port, host name suffix), initializes reader data handling, creates the lock accessory and its services/characteristics (including lock mechanism, management, NFC access, protocol/version, and optional physical battery service), installs developer debug commands, and registers controller and connection callbacks.
 */
void HomeKitLock::begin() {
    m_lock_state_changed = AppEventLoop::subscribe(LOCK_EVENT, LOCK_STATE_CHANGED, [&](const uint8_t* data, size_t size){
        if(size == 0 || data == nullptr) return;
        std::span<const uint8_t> payload(data, size);
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
        if(ec) { ESP_LOGE(TAG, "Failed to deserialize EventLockState event: %s", ec.message().c_str()); return; }
        ESP_LOGI(TAG, "Received lock state event: %d -> %d", m_lockTargetState->getVal(), s.targetState);
        updateLockState(s.currentState, s.targetState);
    });
    const auto& miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();
    const auto& app_version = esp_app_get_description()->version;
    ESP_LOGI(TAG, "Starting HomeSpan setup...");

    if (miscConfig.controlPin != 255){
      static auto hsControlPin = GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.controlPin), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::GpioIn, GPIOAllocator::PinConsumer::HomeKit, "HS_CONTROL_PIN");
      if(hsControlPin.has_value())
        homeSpan.setControlPin(miscConfig.controlPin);
      else
        ESP_LOGW(TAG, "Could not acquire pin for the HomeSpan Control pin, error: %d", hsControlPin.error());
    }
    if (miscConfig.hsStatusPin != 255){
      static auto hsStatusPin = GPIOAllocator::instance().acquire(gpio_num_t(miscConfig.hsStatusPin), GPIO_MODE_OUTPUT, GPIOAllocator::PinRole::Led, GPIOAllocator::PinConsumer::HomeKit, "HS_STATUS_PIN");
      if(hsStatusPin.has_value()) {
        static SharedLed statusLed{std::move(hsStatusPin.value())};
        statusLed.set_restore_hook([]() {homeSpan.refreshStatusDevice();});
        homeSpan.setStatusDevice(&statusLed);
      } else {
        ESP_LOGW(TAG, "Could not acquire pin for the HomeSpan Status pin, error: %d", hsStatusPin.error());
      }
    }
    #ifdef CONFIG_INIT_ARDU_SERIAL_LOGGING
    ESP_LOGI(TAG, "Press any key within 1 second for console access.");
    vTaskDelay(pdMS_TO_TICKS(1000));
    if(Serial.available()){
      homeSpan.setLogLevel(0);
    } else {
      homeSpan.setLogLevel(-1);
      homeSpan.setSerialInputDisable(true);
    }
    #else
    homeSpan.setLogLevel(-1);
    homeSpan.setSerialInputDisable(true);
    #endif
    homeSpan.setStatusAutoOff(15);
    homeSpan.setSketchVersion(app_version);
    homeSpan.enableAutoStartAP();
    homeSpan.enableOTA(miscConfig.otaPasswd.c_str());
    homeSpan.setPortNum(1201);
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    const std::string macStr = fmt::format("{:02X}{:02X}{:02X}{:02X}", mac[2], mac[3], mac[4], mac[5]);
    homeSpan.setHostNameSuffix(macStr.c_str());

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
    homeSpan.setConnectionTimes(8, 30, 8);
    homeSpan.setApFunction(apStarted);
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
        ESP_LOGI(TAG, "ERROR");
      } else if (strncmp(buf + 1, "W", 1) == 0) {
        level = ESP_LOG_WARN;
        ESP_LOGI(TAG, "WARNING");
      } else if (strncmp(buf + 1, "I", 1) == 0) {
        level = ESP_LOG_INFO;
        ESP_LOGI(TAG, "INFO");
      } else if (strncmp(buf + 1, "D", 1) == 0) {
        level = ESP_LOG_DEBUG;
        ESP_LOGI(TAG, "DEBUG");
      } else if (strncmp(buf + 1, "V", 1) == 0) {
        level = ESP_LOG_VERBOSE;
        ESP_LOGI(TAG, "VERBOSE");
      } else if (strncmp(buf + 1, "N", 1) == 0) {
        level = ESP_LOG_NONE;
        ESP_LOGI(TAG, "NONE");
      }

      esp_log_level_set("*", level);
    });
    new SpanUserCommand('F', "Set HomeKey Flow", [](const char *buf){
      ddk::KeyFlow hkFlow = ddk::KeyFlow::kFlowFAST;
      switch (buf[1]) {
      case '0':
        hkFlow = ddk::KeyFlow::kFlowFAST;
        ESP_LOGI(TAG, "FAST Flow");
        break;

      case '1':
        hkFlow = ddk::KeyFlow::kFlowSTANDARD;
        ESP_LOGI(TAG, "STANDARD Flow");
        break;
      case '2':
        hkFlow = ddk::KeyFlow::kFlowATTESTATION;
        ESP_LOGI(TAG, "ATTESTATION Flow");
        break;

      default:
        ESP_LOGI(TAG, "0 = FAST flow, 1 = STANDARD Flow, 2 = ATTESTATION Flow");
        break;
      }
      EventValueChanged s{.newValue = static_cast<uint8_t>(hkFlow)};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      HomekitEvent event{.type=DEBUG_AUTH_FLOW, .data=d};
      std::vector<uint8_t> event_data;
      alpaca::serialize(event, event_data);
      AppEventLoop::publish(HK_EVENT, HK_INTERNAL_EVENT, event_data.data(), event_data.size());
    });
    new SpanUserCommand('M', "Erase MQTT Config and restart", [](const char*){s_instance->m_configManager.deleteConfig<espConfig::mqttConfig_t>();});
    new SpanUserCommand('N', "Btr status low", [](const char* arg) {
      if (!s_instance->m_statusLowBattery) {
        ESP_LOGW(TAG, "Battery service is disabled");
        return;
      }
      const char* TAG = "BTR_LOW";
      if (strncmp(arg + 1, "0", 1) == 0) {
        s_instance->m_statusLowBattery->setVal(0);
        ESP_LOGI(TAG, "Low status set to NORMAL");
      } else if (strncmp(arg + 1, "1", 1) == 0) {
        s_instance->m_statusLowBattery->setVal(1);
        ESP_LOGI(TAG, "Low status set to LOW");
      }
    });
    new SpanUserCommand('B', "Btr level", [](const char* arg) {
      if (!s_instance->m_batteryLevel) {
        ESP_LOGW(TAG, "Battery service is disabled");
        return;
      }
      uint8_t level = atoi(static_cast<const char *>(arg + 1));
      s_instance->m_batteryLevel->setVal(level);
    });

    new SpanUserCommand('P', "Print Issuers", [](const char* c) {
        const auto& issuers = s_instance->m_readerDataManager.issuers();
        ESP_LOGI(TAG, "--- Registered HomeKey Issuers ---");
        if (issuers.empty()) {
            ESP_LOGI(TAG, "None");
        }
        for(const auto& issuer : issuers) {
             ESP_LOGI(TAG, "ID: %s, PK: %s",
                 fmt::format("{:02X}", fmt::join(issuer.id, "")).c_str(),
                 fmt::format("{:02X}", fmt::join(issuer.public_key, "")).c_str());
        }
        ESP_LOGI(TAG, "------------------------------------");
    });
    new SpanUserCommand('G', "Who owns this GPIO Pin?", [](const char* c) {
      uint8_t i = atoi(c+1);
      auto s = GPIOAllocator::instance().owner_of(i);
      ESP_LOGI(TAG, "Owner: %s", s.has_value() ? s->c_str() : "Not allocated");
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

void HomeKitLock::apStarted() {
    if (s_instance) s_instance->conn_cb(0);
}

/**
 * @brief Handle changes to the HomeKit controller list and synchronize stored HomeKey issuers.
 *
 * When called, this updates the ReaderDataManager to match the current set of paired controllers:
 * - If there are zero admin controllers, deletes all stored reader data.
 * - Otherwise, ensures each controller's LTPK has a corresponding issuer entry, removes any stored
 *   issuers that no longer correspond to a paired controller, and persists the reader data to NVS
 *   if anything changed.
 *
 * Side effects: may delete reader data, add or remove issuer entries, and write data to NVS; logs
 * success or failure.
 */
void HomeKitLock::controllerCallback() {
    ESP_LOGI(TAG, "HomeKit controller list changed.");
    if (HAPClient::nAdminControllers() == 0) {
        ESP_LOGW(TAG, "Last controller unpaired. Wiping HomeKey data.");
        m_readerDataManager.deleteAllReaderData();
        return;
    }

    bool dataChanged = false;

    // Add any controllers that don't yet have a corresponding issuer,
    // and track their identifiers so we know which issuers are still valid.
    std::vector<std::vector<uint8_t>> currentIssuerIds;
    for (auto it = homeSpan.controllerListBegin(); it != homeSpan.controllerListEnd(); ++it) {
        std::vector<uint8_t> issuerId = Utils::getHashIdentifier(it->getLTPK(), 32);
        currentIssuerIds.push_back(issuerId);
        if (m_readerDataManager.addIssuerIfNotExists(issuerId, it->getLTPK())) {
            ESP_LOGI(TAG, "New controller paired, issuer added.");
            dataChanged = true;
        }
    }

    // Remove any stored issuers that no longer correspond to a paired controller.
    // Iterate over a snapshot copy since removeIssuerIfItExists locks internally
    // and may mutate the manager's live issuer list.
    auto readerDataSnapshot = m_readerDataManager.issuers();
    for (const auto& issuer : readerDataSnapshot) {
        bool stillPaired = std::any_of(currentIssuerIds.begin(), currentIssuerIds.end(),
            [&issuer](const std::vector<uint8_t>& id) {
                return issuer.id.size() == id.size() &&
                       std::equal(issuer.id.begin(), issuer.id.end(), id.begin());
            });
        if (!stillPaired) {
            if (m_readerDataManager.removeIssuerIfExists(issuer.id)) {
                ESP_LOGI(TAG, "Controller unpaired, issuer removed.");
                dataChanged = true;
            }
        }
    }

    if (dataChanged) {
        ESP_LOGI(TAG, "Issuer list changed, saving reader data to NVS.");
        m_readerDataManager.save();
    }
}
