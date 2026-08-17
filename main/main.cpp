#include <cstdint>
#include <memory>
#include "ConsoleLogSinker.h"
#include "HomeSpan.h"
#include "config.hpp"
#include <esp_event.h>
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
#include "nvs_flash.h"

std::unique_ptr<LockManager> lockManager;
ReaderDataManager readerDataManager;
ConfigManager configManager;
std::unique_ptr<HardwareManager> hardwareManager;
std::unique_ptr<MqttManager> mqttManager;
WebServerManager webServerManager(configManager, readerDataManager);
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
    webServerManager.begin(); 
  } else if (status == 0){
    pollHS = false;
    mqttManager->end();
    webServerManager.end();
    WiFi.mode(WIFI_AP_STA);
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    const std::string macStr = fmt::format("HK_{:02X}{:02X}{:02X}{:02X}", mac[2], mac[3], mac[4], mac[5]);
    auto misc = configManager.getConfig<espConfig::misc_config_t>();
    WiFi.softAP(macStr.c_str(), misc.accessPointPassword.c_str(), 11, false, 2, false, WIFI_AUTH_WPA2_WPA3_PSK, WIFI_CIPHER_TYPE_AES_CMAC128); 
    start_captive_portal();
    webServerManager.begin();
    while(true){
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
};
using namespace loggable;

bool initLogging(){
  uint8_t logLevel;
  if(!configManager.getNVSLogLevel(logLevel)) return false;
  uint16_t backlogMaxSize = 0;
  if(!configManager.getBacklogMaxSize(backlogMaxSize)) return false;
  webServerManager.setWSBackLogSize(backlogMaxSize);
  esp_log_level_set("*", static_cast<esp_log_level_t>(logLevel));
  loggable::Sinker::instance().set_level(static_cast<loggable::LogLevel>(logLevel));
  loggable::espidf::LogHook::install(false, true);
  Sinker::instance().add_sinker(std::make_shared<loggable::ConsoleLogSinker>());
  Sinker::instance().add_sinker(std::make_shared<loggable::WebSocketLogSinker>(webServerManager));
  return true;
}

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
  #ifdef CONFIG_IDF_TARGET_ESP32
  gpio_set_pull_mode(GPIO_NUM_3, GPIO_PULLUP_ONLY); // U0RXD idle-HIGH in case UART-bridge not present
  #endif
  #ifdef CONFIG_INIT_ARDU_SERIAL_LOGGING
  Serial.begin(115200);
  #endif
  if(esp_err_t err = nvs_flash_init(); err != ESP_OK){
    ESP_LOGE("Main", "Failed to initialize NVS. Aborting. err=%d", err);
    return;
  }
  if(!initLogging()){
    ESP_LOGE("Main", "Could not initialize logging. Aborting.");
    return;
  }
  esp_err_t err = esp_event_loop_create_default();
  if (err != ESP_OK) {
    ESP_LOGE("Main", "Failed to create default event loop: %d", err);
  }
  // Why did we just boot? Without this a crash-reboot is indistinguishable in
  // the logs from a hang: the log simply stops and later resumes. The reset
  // reason separates a software panic from a watchdog timeout from a brownout,
  // which need entirely different fixes.
  {
    const esp_reset_reason_t why = esp_reset_reason();
    const char *name = "unknown";
    switch (why) {
      case ESP_RST_POWERON:  name = "power-on"; break;
      case ESP_RST_EXT:      name = "external pin"; break;
      case ESP_RST_SW:       name = "software restart"; break;
      case ESP_RST_PANIC:    name = "PANIC (exception / assert)"; break;
      case ESP_RST_INT_WDT:  name = "INTERRUPT WATCHDOG"; break;
      case ESP_RST_TASK_WDT: name = "TASK WATCHDOG"; break;
      case ESP_RST_WDT:      name = "other watchdog"; break;
      case ESP_RST_DEEPSLEEP:name = "deep sleep wake"; break;
      case ESP_RST_BROWNOUT: name = "BROWNOUT (supply dipped)"; break;
      case ESP_RST_SDIO:     name = "SDIO"; break;
      default: break;
    }
    const bool unexpected = (why == ESP_RST_PANIC || why == ESP_RST_INT_WDT ||
                             why == ESP_RST_TASK_WDT || why == ESP_RST_WDT ||
                             why == ESP_RST_BROWNOUT);
    if (unexpected) {
      ESP_LOGE("Boot", "*** UNEXPECTED RESET: %s (reason %d) ***", name, (int)why);
    } else {
      ESP_LOGI("Boot", "Reset reason: %s (%d)", name, (int)why);
    }
  }

  configManager.begin();
  hardwareManager = std::make_unique<HardwareManager>(configManager.getConfig<espConfig::actions_config_t>());
  lockManager = std::make_unique<LockManager>(configManager.getConfig<espConfig::misc_config_t>(), configManager.getConfig<espConfig::actions_config_t>());
  mqttManager = std::make_unique<MqttManager>(configManager);
  homekitLock = std::make_unique<HomeKitLock>(lambda, *lockManager, configManager, readerDataManager);
  espConfig::misc_config_t miscConfig = configManager.getConfig<espConfig::misc_config_t>();
  static const char* TAG = "Main";
  if(miscConfig.nfcPinsPreset != PIN_UNSET){
    ESP_LOGI(TAG, "NFC GPIO pins preset: %s", nfcGpioPinsPresets[miscConfig.nfcPinsPreset].name.c_str());
    ESP_LOGI(TAG, "NFC preset pins: %d, %d, %d, %d", nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[0], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[1], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[2], nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins[3]);
  } else {
    ESP_LOGI(TAG, "NFC GPIO pins preset: Custom");
    ESP_LOGI(TAG, "NFC Custom GPIO pins: %d, %d, %d, %d", miscConfig.nfcGpioPins[0], miscConfig.nfcGpioPins[1], miscConfig.nfcGpioPins[2], miscConfig.nfcGpioPins[3]);
  }
  // Resolve the pins once. NfcManager is handed the preset array when one is
  // selected, so logging miscConfig.nfcGpioPins here would report SDA/SCL that
  // the reader is not actually using.
  const std::array<uint8_t, 4> &activeNfcPins =
      miscConfig.nfcPinsPreset == PIN_UNSET ? miscConfig.nfcGpioPins
                                            : nfcGpioPinsPresets[miscConfig.nfcPinsPreset].gpioPins;
  const char *readerName = miscConfig.nfcReaderType == 0   ? "PN532 (SPI)"
                           : miscConfig.nfcReaderType == 1 ? "PN7160"
                           : miscConfig.nfcReaderType == 2 ? "ST25R3916 (I2C)"
                                                           : "UNKNOWN";
  ESP_LOGI(TAG, "NFC reader type: %s (%u)", readerName, miscConfig.nfcReaderType);
  if (miscConfig.nfcReaderType == 1) {
    ESP_LOGI(TAG, "NFC IRQ pin: %d, VEN pin: %d", miscConfig.nfcIrqPin, miscConfig.nfcVenPin);
  } else if (miscConfig.nfcReaderType == 2) {
    ESP_LOGI(TAG, "NFC I2C pins: SDA=%d, SCL=%d", activeNfcPins[0], activeNfcPins[1]);
  }
  readerDataManager.begin();

  nfcManager = std::make_unique<NfcManager>(readerDataManager,
                              activeNfcPins,
                              miscConfig.nfcReaderType,
                              miscConfig.nfcIrqPin,
                              miscConfig.nfcVenPin,
                              miscConfig.hkAuthPrecomputeEnabled,
                              miscConfig.nfcFastPollingEnabled);
  nfcManager->begin();

  webServerManager.setNfcManager(nfcManager.get());
  webServerManager.setMqttManager(mqttManager.get());
  hardwareManager->begin();
  homekitLock->begin();
  lockManager->begin();
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
