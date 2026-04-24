#include <cstdint>
#include <memory>
#include "ConsoleLogSinker.h"
#include "HomeSpan.h"
#include "app_event_loop.hpp"
#include "config.hpp"
#include <esp_event.h>
#include "eth_structs.hpp"
#include "dns_server.h"
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
#include "lwip/inet.h"

std::unique_ptr<LockManager> lockManager;
std::unique_ptr<ReaderDataManager> readerDataManager;
std::unique_ptr<ConfigManager> configManager;
std::unique_ptr<HardwareManager> hardwareManager;
std::unique_ptr<MqttManager> mqttManager;
std::unique_ptr<WebServerManager> webServerManager;
std::unique_ptr<HomeKitLock> homekitLock;
std::unique_ptr<NfcManager> nfcManager;

static dns_server_handle_t dns_server = NULL;

bool pollHS = false;

static void dhcp_set_captiveportal_url(void) {
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    ESP_LOGI("Main", "Setting up captive portal on IP: %s", ip_addr);

    char captiveportal_uri[32];
    snprintf(captiveportal_uri, sizeof(captiveportal_uri), "http://%s", ip_addr);

    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_stop(netif));
    ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVEPORTAL_URI, captiveportal_uri, strlen(captiveportal_uri)));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_start(netif));
}

static void start_captive_portal(void)
{
    dhcp_set_captiveportal_url();

    dns_server_config_t dns_config = DNS_SERVER_CONFIG_SINGLE("*", "WIFI_AP_DEF");
    dns_server = start_dns_server(&dns_config);
    ESP_LOGI("Main", "DNS server started for captive portal");
}

std::function<void(int)> lambda = [](int status) {
  if (status == 1) {
    char identifier[18];
    sprintf(identifier, "%.2s%.2s%.2s%.2s%.2s%.2s", HAPClient::accessory.ID, HAPClient::accessory.ID + 3, HAPClient::accessory.ID + 6, HAPClient::accessory.ID + 9, HAPClient::accessory.ID + 12, HAPClient::accessory.ID + 15);
    mqttManager->begin(std::string(identifier));
    webServerManager->begin(); 
  } else if (status == 0){
    pollHS = false;
    mqttManager->end();
    webServerManager->end();
    WiFi.mode(WIFI_AP);
    WiFi.softAP("HomeKey-ESP32", "homekey123", 11, false, 2, false, WIFI_AUTH_WPA2_WPA3_PSK, WIFI_CIPHER_TYPE_AES_CMAC128); 
    start_captive_portal();
    webServerManager->begin();
    while(true){
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
};
using namespace loggable;

/**
 * @brief Initialize runtime, configure logging/serial, and instantiate core subsystem managers.
 *
 * Initializes the global runtime infrastructure (Sinker), sets logging levels and Serial,
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
  loggable::espidf::LogHook::install(false, false);
  Sinker::instance().add_sinker(std::make_shared<loggable::ConsoleLogSinker>());
  esp_err_t err = esp_event_loop_create_default();
  if (err != ESP_OK) {
    ESP_LOGE("Main", "Failed to create default event loop: %d", err);
  }
  readerDataManager = std::make_unique<ReaderDataManager>();
  configManager = std::make_unique<ConfigManager>();
  configManager->begin();
  esp_log_level_set("*", static_cast<esp_log_level_t>(configManager->getConfig<espConfig::misc_config_t>().logLevel));
  loggable::Sinker::instance().set_level((loggable::LogLevel)configManager->getConfig<espConfig::misc_config_t>().logLevel);
  webServerManager = std::make_unique<WebServerManager>(*configManager, *readerDataManager);
  Sinker::instance().add_sinker(std::make_shared<loggable::WebSocketLogSinker>(webServerManager.get()));
  hardwareManager = std::make_unique<HardwareManager>(configManager->getConfig<espConfig::actions_config_t>());
  lockManager = std::make_unique<LockManager>(configManager->getConfig<espConfig::misc_config_t>(), configManager->getConfig<espConfig::actions_config_t>());
  mqttManager = std::make_unique<MqttManager>(*configManager);
  homekitLock = std::make_unique<HomeKitLock>(lambda, *lockManager, *configManager, *readerDataManager);
  espConfig::misc_config_t miscConfig = configManager->getConfig<espConfig::misc_config_t>();
  static const char* TAG = "Main";
  if(miscConfig.nfcPinsPreset != PIN_UNSET){
    ESP_LOGI(TAG, "NFC GPIO pins preset: %s", nfcGpioPinsPresets[miscConfig.nfcPinsPreset].name.c_str());
    ESP_LOGI(TAG, "NFC preset pins: %d, %d, %d, %d", nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[0], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[1], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[2], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[3]);
  } else {
    ESP_LOGI(TAG, "NFC GPIO pins preset: Custom");
    ESP_LOGI(TAG, "NFC Custom GPIO pins: %d, %d, %d, %d", miscConfig.nfcGpioPins[0], miscConfig.nfcGpioPins[1], miscConfig.nfcGpioPins[2], miscConfig.nfcGpioPins[3]);
  }
  readerDataManager->begin();
  if(miscConfig.ethernetEnabled){
    std::vector<uint8_t> ethPins;
    if(miscConfig.ethActivePreset == PIN_UNSET){
      ethPins = {miscConfig.ethSpiConfig[4], miscConfig.ethSpiConfig[5], miscConfig.ethSpiConfig[6]};
    } else { 
        const eth_board_presets_t& ethPreset = eth_config_ns::boardPresets[miscConfig.ethActivePreset];
        ethPins = {ethPreset.spi_conf.pin_sck, ethPreset.spi_conf.pin_miso, ethPreset.spi_conf.pin_mosi};
    }
    std::vector<uint8_t> nfcPins;
    if(miscConfig.nfcPinsPreset == PIN_UNSET){
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
    nfcManager = std::make_unique<NfcManager>(*readerDataManager,
                                miscConfig.nfcPinsPreset == PIN_UNSET ? miscConfig.nfcGpioPins : nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins,
                                miscConfig.hkAuthPrecomputeEnabled);
    nfcManager->begin();
    webServerManager->setNfcManager(nfcManager.get());
    webServerManager->setMqttManager(mqttManager.get());
  }
  hardwareManager->begin();
  homekitLock->begin();
  lockManager->begin();
  WiFi.onEvent([](arduino_event_id_t event){
    static uint8_t count = 0;
    if(count >= 6){
      homeSpan.processSerialCommand("A");
      count = 0;
    } else {
      count++;
    }
  }, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  pollHS = true;
}

/**
 * @brief Run the main application loop: service HomeSpan events and yield to the RTOS.
*
 * Polls HomeSpan to process HomeKit and internal events, then delays 50 ms to allow other
 * FreeRTOS tasks to run.
 */

void loop() {
  if(pollHS)
    homeSpan.poll();
  vTaskDelay(pdMS_TO_TICKS(50));
}
