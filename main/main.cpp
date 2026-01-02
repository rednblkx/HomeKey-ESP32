#include <memory>
#include <cstdio>
#define FMT_HEADER_ONLY

#include "config.hpp"
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
 * @brief Configure runtime and initialize core subsystems and manager instances.
 *
 * Initializes logging and serial output, brings up Ethernet according to configuration,
 * constructs global manager objects (ReaderDataManager, ConfigManager, HardwareManager,
 * LockManager, MqttManager, WebServerManager, HomeKitLock, NfcManager) and starts those
 * managers that require explicit startup.
 *
 * The function has the side effect of allocating and assigning globals used across the
 * application and invoking their initialization routines.
 */
void setup() {
  auto& distributor = Sinker::instance();

  distributor.set_level(LogLevel::Verbose);
  distributor.hook_esp_log(true);
  Serial.begin(115200);
  readerDataManager = new ReaderDataManager;
  configManager = new ConfigManager;
  configManager->begin();
  esp_log_level_set("*", static_cast<esp_log_level_t>(configManager->getConfig<espConfig::misc_config_t>().logLevel));
  webServerManager = new WebServerManager(*configManager, *readerDataManager);
  distributor.add_sinker(std::make_shared<loggable::WebSocketLogSinker>(webServerManager));
  hardwareManager = new HardwareManager(configManager->getConfig<espConfig::misc_config_t>(), configManager->getConfig<espConfig::actions_config_t>());
  lockManager = new LockManager(configManager->getConfig<espConfig::misc_config_t>(), configManager->getConfig<espConfig::actions_config_t>());
  mqttManager = new MqttManager(*configManager);
  homekitLock = new HomeKitLock(lambda, *lockManager, *configManager, *readerDataManager, *hardwareManager);
  espConfig::misc_config_t miscConfig = configManager->getConfig<espConfig::misc_config_t>();
  static const char* TAG = "Main";
  if(miscConfig.nfcPinsPreset != 255){
    ESP_LOGI(TAG, "NFC GPIO pins preset: %s", nfcGpioPinsPresets[miscConfig.nfcPinsPreset].name.c_str());
    ESP_LOGI(TAG, "NFC preset pins: %d, %d, %d, %d", nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[0], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[1], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[2], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[3]);
  } else {
    ESP_LOGI(TAG, "NFC GPIO pins preset: Custom");
    ESP_LOGI(TAG, "NFC Custom GPIO pins: %d, %d, %d, %d", miscConfig.nfcGpioPins[0], miscConfig.nfcGpioPins[1], miscConfig.nfcGpioPins[2], miscConfig.nfcGpioPins[3]);
  }
  nfcManager = new NfcManager(*readerDataManager,
                              miscConfig.nfcPinsPreset == 255 ? miscConfig.nfcGpioPins : nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins,
                              miscConfig.hkAuthPrecomputeEnabled);
  readerDataManager->begin();
  nfcManager->begin();
  homekitLock->begin();
  hardwareManager->begin();
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
