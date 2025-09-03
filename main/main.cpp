#define FMT_HEADER_ONLY
#include "config.hpp"
#include "logger.hpp"
#include "eth_structs.hpp"
#include "HomeKitLock.hpp"
#include "LockManager.hpp"
#include "NfcManager.hpp"
#include "ConfigManager.hpp"
#include "ReaderDataManager.hpp"
#include "HardwareManager.hpp"
#include "MqttManager.hpp"
#include "WebServerManager.hpp"
#include <functional>
#include <sodium/crypto_sign.h>
#include <sodium/crypto_box.h>
#include "HAP.h"

const char* TAG = "LockApp";
espp::Logger logger({.tag = "LockApp"});

static void ethEventHandler(arduino_event_id_t event, arduino_event_info_t info) {
  uint8_t mac[6] = { 0, 0, 0, 0, 0, 0 };
  char macStr[13] = {0};
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      logger.info("ETH Started");
      ETH.macAddress(mac);
      sprintf(macStr, "ESP32_%02X%02X%02X", mac[0], mac[1], mac[2]);
      ETH.setHostname(macStr);
      break;
    case ARDUINO_EVENT_ETH_CONNECTED: logger.info("ETH Connected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:    logger.info("ETH Got IP: '%s'\n", esp_netif_get_desc(info.got_ip.esp_netif)); break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      logger.info("ETH Lost IP");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      logger.info("ETH Disconnected");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      logger.info("ETH Stopped");
      break;
    default: break;
  }
}


LockManager *lockManager;
ReaderDataManager *readerDataManager;
ConfigManager *configManager;
HardwareManager *hardwareManager;
MqttManager *mqttManager;
WebServerManager *webServerManager;
HomeKitLock *homekitLock;
NfcManager *nfcManager;

void initializeETH(){
  const auto& miscConfig = configManager->getConfig<espConfig::misc_config_t>();


    if (!miscConfig.ethernetEnabled) {
        logger.info( "Ethernet is disabled. HomeSpan will manage Wi-Fi.");
        return; // Do nothing and let HomeSpan handle Wi-Fi
    }

    logger.info( "Ethernet is enabled. Initializing...");
    Network.onEvent(ethEventHandler);

    // --- Preset-based Configuration ---
    if (miscConfig.ethActivePreset != 255) {
        if (miscConfig.ethActivePreset >= eth_config_ns::boardPresets.size()) {
            logger.error( "Invalid ethActivePreset index (%d). Not initializing Ethernet.", miscConfig.ethActivePreset);
            return;
        }

        const eth_board_presets_t& ethPreset = eth_config_ns::boardPresets[miscConfig.ethActivePreset];
        logger.info( "Initializing with preset: %s", ethPreset.name.c_str());

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
            logger.error( "Preset requires EMAC, but this board does not have a built-in Ethernet MAC.");
            #endif
        }
    }
    // --- Custom Configuration ---
    else {
        logger.info( "Initializing with custom pin configuration.");
        auto phy_type = static_cast<eth_phy_type_t>(miscConfig.ethPhyType);
        
        if (eth_config_ns::supportedChips.count(phy_type) == 0) {
            logger.error( "Custom phy_type (%d) is not supported.", miscConfig.ethPhyType);
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
            logger.error( "Custom config requires EMAC, but this board does not have a built-in Ethernet MAC.");
            #endif
        }
    }
}

std::function<void(int)> lambda = [](int status) {
  if (status == 1) {
    char identifier[18];
    sprintf(identifier, "%.2s%.2s%.2s%.2s%.2s%.2s", HAPClient::accessory.ID, HAPClient::accessory.ID + 3, HAPClient::accessory.ID + 6, HAPClient::accessory.ID + 9, HAPClient::accessory.ID + 12, HAPClient::accessory.ID + 15);
    mqttManager->begin(std::string(identifier));
    webServerManager->begin();
  }
};
void setup() {
  Serial.begin(115200);
  // espp::EventManager::get().set_log_level(espp::Logger::Verbosity::DEBUG);
  readerDataManager = new ReaderDataManager;
  configManager = new ConfigManager;
  configManager->begin();
  initializeETH();
  hardwareManager = new HardwareManager(configManager->getConfig<espConfig::misc_config_t>());
  lockManager = new LockManager(*hardwareManager, configManager->getConfig<espConfig::misc_config_t>());
  mqttManager = new MqttManager(*configManager);
  webServerManager = new WebServerManager(*configManager, *readerDataManager);
  homekitLock = new HomeKitLock(lambda, *lockManager, *configManager, *readerDataManager);
  nfcManager = new NfcManager(*readerDataManager, configManager->getConfig<espConfig::misc_config_t>().nfcGpioPins);
  homekitLock->begin();
  hardwareManager->begin();
  lockManager->begin();
  nfcManager->begin();
}

//////////////////////////////////////

void loop() {
    homeSpan.poll();
    vTaskDelay(pdMS_TO_TICKS(50));
}
