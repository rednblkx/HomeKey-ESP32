#include <memory>
#include "ConsoleLogSinker.h"
#include "config.hpp"
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
#include "loggable.hpp"
#include "loggable_espidf.hpp"
#include "WebSocketLogSinker.h"

LockManager *lockManager;
ReaderDataManager *readerDataManager;
ConfigManager *configManager;
HardwareManager *hardwareManager;
MqttManager *mqttManager;
WebServerManager *webServerManager;
HomeKitLock *homekitLock;
NfcManager *nfcManager;

std::function<void(int)> lambda = [](int status) {
  if (status == 1) {
    char identifier[18];
    sprintf(identifier, "%.2s%.2s%.2s%.2s%.2s%.2s", HAPClient::accessory.ID, HAPClient::accessory.ID + 3, HAPClient::accessory.ID + 6, HAPClient::accessory.ID + 9, HAPClient::accessory.ID + 12, HAPClient::accessory.ID + 15);
    mqttManager->begin(std::string(identifier));
    webServerManager->begin(); 
  }
};
using namespace loggable;

/**
 * @brief Initialize runtime, configure logging/serial, and instantiate core subsystem managers.
 *
 * Initializes the global runtime infrastructure (EventBus, Sinker), sets logging levels and Serial,
 * constructs and assigns global manager instances (ReaderDataManager, ConfigManager, WebServerManager,
 * HardwareManager, LockManager, MqttManager, HomeKitLock, NfcManager), reads NFC-related configuration,
 * and starts managers that require explicit startup.
 *
 * @note This function allocates and assigns globals used across the application and invokes their
 *       initialization routines (calls to `begin()` where applicable). It also logs the resolved NFC
 *       GPIO pin configuration based on persisted settings.
 */
void setup() {
  Serial.begin(115200);
  loggable::espidf::LogHook::install(false);
  Sinker::instance().add_sinker(std::make_shared<loggable::ConsoleLogSinker>());
  EventBus::Bus::instance().init();
  readerDataManager = new ReaderDataManager;
  configManager = new ConfigManager;
  configManager->begin();
  esp_log_level_set("*", static_cast<esp_log_level_t>(configManager->getConfig<espConfig::misc_config_t>().logLevel));
  loggable::Sinker::instance().set_level((loggable::LogLevel)configManager->getConfig<espConfig::misc_config_t>().logLevel);
  webServerManager = new WebServerManager(*configManager, *readerDataManager);
  Sinker::instance().add_sinker(std::make_shared<loggable::WebSocketLogSinker>(webServerManager));
  hardwareManager = new HardwareManager(configManager->getConfig<espConfig::actions_config_t>());
  lockManager = new LockManager(configManager->getConfig<espConfig::misc_config_t>(), configManager->getConfig<espConfig::actions_config_t>());
  mqttManager = new MqttManager(*configManager);
  homekitLock = new HomeKitLock(lambda, *lockManager, *configManager, *readerDataManager);
  espConfig::misc_config_t miscConfig = configManager->getConfig<espConfig::misc_config_t>();
  static const char* TAG = "Main";
  if(miscConfig.nfcPinsPreset != 255){
    ESP_LOGI(TAG, "NFC GPIO pins preset: %s", nfcGpioPinsPresets[miscConfig.nfcPinsPreset].name.c_str());
    ESP_LOGI(TAG, "NFC preset pins: %d, %d, %d, %d", nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[0], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[1], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[2], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[3]);
  } else {
    ESP_LOGI(TAG, "NFC GPIO pins preset: Custom");
    ESP_LOGI(TAG, "NFC Custom GPIO pins: %d, %d, %d, %d", miscConfig.nfcGpioPins[0], miscConfig.nfcGpioPins[1], miscConfig.nfcGpioPins[2], miscConfig.nfcGpioPins[3]);
  }
  readerDataManager->begin();
  if(miscConfig.ethernetEnabled){
    std::vector<uint8_t> ethPins;
    if(miscConfig.ethActivePreset == 255){
      ethPins = {miscConfig.ethSpiConfig[4], miscConfig.ethSpiConfig[5], miscConfig.ethSpiConfig[6]};
    } else { 
        const eth_board_presets_t& ethPreset = eth_config_ns::boardPresets[miscConfig.ethActivePreset];
        ethPins = {ethPreset.spi_conf.pin_sck, ethPreset.spi_conf.pin_miso, ethPreset.spi_conf.pin_mosi};
    }
    std::vector<uint8_t> nfcPins;
    if(miscConfig.nfcPinsPreset == 255){
      nfcPins = {miscConfig.nfcGpioPins[1], miscConfig.nfcGpioPins[2], miscConfig.nfcGpioPins[3]};
    } else {
      nfcPins = {nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[1], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[2], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[3]};
    }
    std::vector<uint8_t> pins_intersection;
    std::ranges::set_intersection(ethPins.begin(), ethPins.end(), nfcPins.begin(), nfcPins.end(), std::back_inserter(pins_intersection));
    if((!pins_intersection.empty() && miscConfig.ethSpiBus == SPI2_HOST)){
      goto nfc_init;
    }
    #if SOC_SPI_PERIPH_NUM > 2
    else if (pins_intersection.empty() && miscConfig.ethSpiBus == SPI3_HOST) goto nfc_init;
    #endif
    else {
      if(miscConfig.ethSpiBus == SPI2_HOST){
        ESP_LOGE(TAG, "Ethernet enabled on SPI2 Bus, PN532 has to use the same GPIO pins as Ethernet");
      } else {
        ESP_LOGE(TAG, "Ethernet enabled on SPI3 Bus, PN532 cannot use the same GPIO pins as Ethernet");
        for(auto& pin : pins_intersection){
          ESP_LOGE(TAG, "GPIO Intersection: %d", pin);
        }
      }
    }
  } else {
  nfc_init:
    nfcManager = new NfcManager(*readerDataManager,
                                miscConfig.nfcPinsPreset == 255 ? miscConfig.nfcGpioPins : nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins,
                                miscConfig.hkAuthPrecomputeEnabled);
    nfcManager->begin();
  }
  hardwareManager->begin();
  homekitLock->begin();
  lockManager->begin();
}

/**
 * @brief Run the main application loop: service HomeSpan events and yield to the RTOS.
 *
 * Polls HomeSpan to process HomeKit and internal events, then delays 50 ms to allow other
 * FreeRTOS tasks to run.
 */

void loop() {
    homeSpan.poll();
    vTaskDelay(pdMS_TO_TICKS(50));
}
