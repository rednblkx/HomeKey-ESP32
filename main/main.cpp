#include <cstdint>
#include <memory>
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
#include "esp_app_desc.h"
#include "pins_arduino.h"
#include "NFC_SERV_CHARS.h"
#include <mbedtls/sha256.h>
#include <esp_mac.h>
#include <freertos/semphr.h>

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

struct DoorbellSensor;
extern DoorbellSensor* homekit_doorbell;

nvs_handle savedData;
SemaphoreHandle_t readerDataMutex = nullptr;
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

struct DoorbellSensor : Service::StatelessProgrammableSwitch {

    // This characteristic will store the event type.
    // We'll use SINGLE_PRESS for a doorbell ring.
    SpanCharacteristic *switchEvent;

    // A unique index for this switch if you had multiple stateless switches on one accessory.
    // For a single doorbell, 1 is fine.
    const int switchIndex = 1;

    DoorbellSensor() : Service::StatelessProgrammableSwitch() {
        LOG(I, "Configuring HomeKit DoorbellSensor Service");

        // Create the Programmable Switch Event characteristic.
        // The constructor takes the initial value (0 for no event) and a boolean
        // indicating if it supports notifications (true).
        switchEvent = new Characteristic::ProgrammableSwitchEvent(0, true);

        // Add the Service Label Index characteristic. This is required by Apple
        // if you have multiple services of the same type (though we only have one here).
        // It helps differentiate them in some contexts.
        new Characteristic::ServiceLabelIndex(switchIndex);

        // Optional: Add a Name characteristic to this service if you want to customize
        // its name in some third-party HomeKit apps (the Home app usually uses the
        // accessory name or allows you to rename it).
        // new Characteristic::Name("Physical Doorbell");

        LOG(I, "DoorbellSensor Service configured with Index %d", switchIndex);
    }

    // This method will be called by our ESP32 code when the physical doorbell is pressed.
    void triggerHomeKitEvent() {
        if (switchEvent) {
            LOG(I, "HomeKit: Triggering Doorbell SINGLE_PRESS event.");
            // Set the value to SINGLE_PRESS (0).
            // Other values are DOUBLE_PRESS (1) and LONG_PRESS (2),
            // but SINGLE_PRESS is most appropriate for a standard doorbell.
            switchEvent->setVal(Characteristic::ProgrammableSwitchEvent::SINGLE_PRESS);

            // Note: StatelessProgrammableSwitch events are "momentary".
            // HomeKit controllers expect the value to be set and then effectively clear.
            // HomeSpan handles notifying controllers. You don't need to set it back to 0 manually
            // for the event to register. The *next* event will overwrite it.
        } else {
            LOG(E, "HomeKit: switchEvent characteristic is null, cannot trigger event!");
        }
    }
};

// Instantiate the global pointer
DoorbellSensor* homekit_doorbell = nullptr;

std::string platform_create_id_string(void) {
  uint8_t mac[6];
  char id_string[13];
  esp_read_mac(mac, ESP_MAC_BT);
  sprintf(id_string, "ESP32_%02x%02X%02X", mac[3], mac[4], mac[5]);
  return std::string(id_string);
}

struct eth_chip_desc_t {
  std::string name;
  bool emac;
  eth_phy_type_t phy_type;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_ONLY_SERIALIZE(eth_chip_desc_t, name, emac, phy_type)
};

struct eth_board_presets_t {
  std::string name;
  eth_chip_desc_t ethChip;
  #if CONFIG_ETH_USE_ESP32_EMAC
  struct rmii_conf_t {
    int32_t phy_addr = 1;
    uint8_t pin_mcd = 23;
    uint8_t pin_mdio = 18;
    int8_t pin_power = -1;
    eth_clock_mode_t pin_rmii_clock = ETH_CLOCK_GPIO0_IN;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_ONLY_SERIALIZE(rmii_conf_t, phy_addr, pin_mcd, pin_mdio, pin_power, pin_rmii_clock)
  } rmii_conf;
  #endif
  struct spi_conf_t {
    uint8_t spi_freq_mhz = 20;
    uint8_t pin_cs = 0; //SS;
    uint8_t pin_irq = A4;
    uint8_t pin_rst = A5;
    uint8_t pin_sck = 5; //SCK;
    uint8_t pin_miso = 6; // MISO;
    uint8_t pin_mosi = 7; // MOSI;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_ONLY_SERIALIZE(spi_conf_t, spi_freq_mhz, pin_cs, pin_irq, pin_rst, pin_sck, pin_miso, pin_mosi)
  } spi_conf;
  friend void to_json(nlohmann ::json &nlohmann_json_j, const eth_board_presets_t &nlohmann_json_t) {
    nlohmann_json_j["name"] = nlohmann_json_t.name;
    nlohmann_json_j["ethChip"] = nlohmann_json_t.ethChip;
    if (nlohmann_json_t.ethChip.emac) {
    #if CONFIG_ETH_USE_ESP32_EMAC
      nlohmann_json_j["rmii_conf"] = nlohmann_json_t.rmii_conf;
    #endif
    } else {
      nlohmann_json_j["spi_conf"] = nlohmann_json_t.spi_conf;
    }
  }
};

namespace eth_config_ns {
  std::map<eth_phy_type_t, eth_chip_desc_t> supportedChips = {
  #if CONFIG_ETH_USE_ESP32_EMAC
      {ETH_PHY_LAN8720, eth_chip_desc_t{"LAN8720", true, ETH_PHY_LAN8720}},
      {ETH_PHY_TLK110, eth_chip_desc_t{"TLK110", true, ETH_PHY_TLK110}},
      {ETH_PHY_RTL8201, eth_chip_desc_t{"RTL8201", true, ETH_PHY_RTL8201}},
      {ETH_PHY_DP83848, eth_chip_desc_t{"DP83848", true, ETH_PHY_DP83848}},
      {ETH_PHY_KSZ8041, eth_chip_desc_t{"KSZ8041", true, ETH_PHY_KSZ8041}},
      {ETH_PHY_KSZ8081, eth_chip_desc_t{"KSZ8081", true, ETH_PHY_KSZ8081}},
  #endif
  #if CONFIG_ETH_SPI_ETHERNET_DM9051
      {ETH_PHY_DM9051, eth_chip_desc_t{"DM9051", false, ETH_PHY_DM9051}},
  #endif
  #if CONFIG_ETH_SPI_ETHERNET_W5500
      {ETH_PHY_W5500, eth_chip_desc_t{"W5500", false, ETH_PHY_W5500}},
  #endif
  #if CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL
      {ETH_PHY_KSZ8851, eth_chip_desc_t{"KSZ8851", false, ETH_PHY_KSZ8851}},
  #endif
  };
  std::vector<eth_board_presets_t> boardPresets = {
      eth_board_presets_t{.name = "Generic W5500",
                          .ethChip = supportedChips[ETH_PHY_W5500],
                          .spi_conf{20, SS, A3, A4, SCK, MISO, MOSI}},
      eth_board_presets_t{.name = "T-ETH-Lite-ESP32S3",
                          .ethChip = supportedChips[ETH_PHY_W5500],
                          .spi_conf{20, 9, 13, 14, 10, 11, 12}},
  #if CONFIG_ETH_USE_ESP32_EMAC
      eth_board_presets_t{.name = "WT32-ETH01",
                          .ethChip = supportedChips[ETH_PHY_LAN8720],
                          .rmii_conf{1, 23, 18, 16, ETH_CLOCK_GPIO0_IN}},
      eth_board_presets_t{.name = "Olimex ESP32-POE",
                          .ethChip = supportedChips[ETH_PHY_LAN8720],
                          .rmii_conf{0, 23, 18, 12, ETH_CLOCK_GPIO17_OUT}},
      eth_board_presets_t{.name = "EST-PoE-32",
                          .ethChip = supportedChips[ETH_PHY_LAN8720],
                          .rmii_conf{0, 23, 18, 12, ETH_CLOCK_GPIO17_OUT}},
      eth_board_presets_t{.name = "T-ETH-Lite-ESP32",
                          .ethChip = supportedChips[ETH_PHY_RTL8201],
                          .rmii_conf{0, 23, 18, 12, ETH_CLOCK_GPIO0_IN}}
  #endif
  };
};

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
    bool ethernetEnabled = false;
    uint8_t ethActivePreset = 255; // 255 for custom pins
    uint8_t ethPhyType = 0;
    #if CONFIG_ETH_USE_ESP32_EMAC
    std::array<int8_t, 5> ethRmiiConfig = {0, -1, -1, -1, 0};
    #endif
    std::array<int8_t, 7> ethSpiConfig = {20, -1, -1, -1, -1, -1, -1};
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        misc_config_t, deviceName, otaPasswd, hk_key_color, setupCode,
        lockAlwaysUnlock, lockAlwaysLock, controlPin, hsStatusPin,
        nfcSuccessPin, nfcSuccessTime, nfcNeopixelPin, neoPixelType,
        neopixelSuccessColor, neopixelFailureColor, neopixelSuccessTime,
        neopixelFailTime, nfcSuccessHL, nfcFailPin, nfcFailTime, nfcFailHL,
        gpioActionPin, gpioActionLockState, gpioActionUnlockState,
        gpioActionMomentaryEnabled, gpioActionMomentaryTimeout, webAuthEnabled,
        webUsername, webPassword, nfcGpioPins, btrLowStatusThreshold,
        proxBatEnabled, hkDumbSwitchMode, hkAltActionInitPin,
        hkAltActionInitLedPin, hkAltActionInitTimeout, hkAltActionPin,
        hkAltActionTimeout, hkAltActionGpioState, hkGpioControlledState,
        ethernetEnabled, ethActivePreset, ethPhyType,
#if CONFIG_ETH_USE_ESP32_EMAC
        ethRmiiConfig,
#endif
        ethSpiConfig
    )
  } miscConfig;
}; // namespace espConfig

KeyFlow hkFlow = KeyFlow::kFlowFAST;
bool hkAltActionActive = false;
SpanCharacteristic* lockCurrentState;
SpanCharacteristic* lockTargetState;
SpanCharacteristic* statusLowBtr;
SpanCharacteristic* btrLevel;
esp_mqtt_client_handle_t client = nullptr;

std::shared_ptr<Pixel> pixel;

// This internal version assumes the CALLER holds the mutex
bool save_to_nvs_internal() {
  // Check added just in case, but lock should be held by caller
  if (readerDataMutex == nullptr || xSemaphoreGetMutexHolder(readerDataMutex) != xTaskGetCurrentTaskHandle()) {
      LOG(E, "save_to_nvs_internal called without holding mutex!");
      return false;
  }
  std::vector<uint8_t> serialized = nlohmann::json::to_msgpack(readerData); // Read shared data
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", serialized.data(), serialized.size());
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG(D, "NVS SET STATUS: %s", esp_err_to_name(set_nvs));
  LOG(D, "NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
  return !set_nvs && !commit_nvs;
}


bool save_to_nvs() {
  if (readerDataMutex == nullptr) {
    LOG(E, "readerDataMutex not initialized in save_to_nvs wrapper!");
    return false;
}
bool success = false;
if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
   success = save_to_nvs_internal(); // Call the internal function
   xSemaphoreGive(readerDataMutex);
} else {
    LOG(E, "Failed to take readerDataMutex in save_to_nvs wrapper!");
}
return success;
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
  bool initial_state_set = false; // Flag for initial setup
  const TickType_t initial_delay_ticks = pdMS_TO_TICKS(1500); // Time for HomeSpan chars to init
  const TickType_t loop_delay_ticks = pdMS_TO_TICKS(100); // Poll interval

  LOG(I, "gpio_task started.");

  while (1) {
      // *** SET INITIAL GPIO STATE (runs only once after boot/task start) ***
      if (!initial_state_set && espConfig::miscConfig.gpioActionPin != 255) {
          // Wait a bit for HomeSpan characteristics to be ready
          LOG(D, "gpio_task: Delaying %dms for initial state check...", pdTICKS_TO_MS(initial_delay_ticks));
          vTaskDelay(initial_delay_ticks);

          // Check if the global characteristic pointers are valid
          if (lockCurrentState != nullptr && lockTargetState != nullptr) {
              int current_state = lockCurrentState->getVal();
              LOG(I, "gpio_task: Setting initial GPIO state based on LockCurrentState: %d", current_state);

              uint8_t initial_level;
              if (current_state == lockStates::LOCKED) {
                  initial_level = espConfig::miscConfig.gpioActionLockState;
              } else { // Assume UNLOCKED or other states use the unlock level initially
                  initial_level = espConfig::miscConfig.gpioActionUnlockState;
              }

              // Ensure pin is output
              pinMode(espConfig::miscConfig.gpioActionPin, OUTPUT);
              // Write initial state
              digitalWrite(espConfig::miscConfig.gpioActionPin, initial_level);
              initial_state_set = true;
              LOG(I, "gpio_task: Initial GPIO pin %d set to level %d", espConfig::miscConfig.gpioActionPin, initial_level);

              // Ensure target state matches initial current state if needed
               if(lockTargetState->getVal() != current_state) {
                    LOG(I, "gpio_task: Aligning initial target state to current state (%d)", current_state);
                    lockTargetState->setVal(current_state);
               }

          } else {
               LOG(E, "gpio_task: lockCurrentState/lockTargetState characteristic not valid for initial state set!");
               // Optionally retry or set a default safe state (e.g., locked)
               // pinMode(espConfig::miscConfig.gpioActionPin, OUTPUT);
               // digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
               // initial_state_set = true; // Mark as set even if fallback?
          }
      }
      // Reset flag if pin gets disabled via web config later
      if (espConfig::miscConfig.gpioActionPin == 255 && initial_state_set) {
           LOG(I, "gpio_task: gpioActionPin disabled, resetting initial_state_set flag.");
           initial_state_set = false;
      }


      // *** PROCESS QUEUE MESSAGES ***
      if (gpio_lock_handle != nullptr) {
          // Check queue without blocking (timeout 0)
          if (xQueueReceive(gpio_lock_handle, &status, 0) == pdPASS) {
              LOG(D, "gpio_task: Received action - source=%d action=%d", status.source, status.action);

              // Only process if GPIO pin is currently enabled
              if (espConfig::miscConfig.gpioActionPin == 255 && !espConfig::miscConfig.hkDumbSwitchMode) {
                  LOG(W, "gpio_task: Received action but gpioActionPin is disabled (and not dumb mode). Ignoring.");
                  // Skip rest of processing for this message
              }
              // Process action 0 (state change request)
              else if (status.action == 0) {
                  if(lockTargetState == nullptr || lockCurrentState == nullptr) {
                      LOG(E, "gpio_task: Characteristics pointers invalid, cannot process action 0.");
                      continue; // Skip to next loop iteration
                  }

                  int target_state_val = lockTargetState->getNewVal(); // Get intended state
                  uint8_t gpio_level_to_set;
                  bool isUnlockAction = (target_state_val == lockStates::UNLOCKED);

                  LOG(D, "GPIO Task Action: Target State=%d", target_state_val);

                  // Determine GPIO level based on target state
                  if (isUnlockAction) {
                      gpio_level_to_set = espConfig::miscConfig.gpioActionUnlockState;
                      // Set intermediate HomeKit state for feedback
                      if(lockCurrentState->getVal() != lockStates::UNLOCKING) {
                           lockCurrentState->setVal(lockStates::UNLOCKING);
                           if (client) esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKING).c_str(), 0, 1, true);
                      }
                  } else { // Target is LOCKED
                      gpio_level_to_set = espConfig::miscConfig.gpioActionLockState;
                       // Set intermediate HomeKit state for feedback
                      if(lockCurrentState->getVal() != lockStates::LOCKING) {
                          lockCurrentState->setVal(lockStates::LOCKING);
                           if (client) esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
                      }
                  }

                  // --- Perform Physical Action ---
                  if (espConfig::miscConfig.gpioActionPin != 255) { // Only write if pin is valid
                      LOG(D, "GPIO Task: Writing pin %d to level %d", espConfig::miscConfig.gpioActionPin, gpio_level_to_set);
                      digitalWrite(espConfig::miscConfig.gpioActionPin, gpio_level_to_set);
                  } else if (espConfig::miscConfig.hkDumbSwitchMode) {
                      // Dumb switch mode might just mean "toggle" regardless of target state
                      // Or maybe it simulates a momentary press? Define this behaviour.
                      // Example: Simple Toggle (assumes it needs a pulse)
                      LOG(D, "GPIO Task: Dumb Switch Mode - Simulating momentary toggle");
                      uint8_t pressLevel = espConfig::miscConfig.gpioActionUnlockState; // Or some dedicated pulse level?
                      uint8_t releaseLevel = espConfig::miscConfig.gpioActionLockState; // Or inverse of pressLevel?
                      // Assume hkDumbSwitchMode uses gpioActionPin if set, otherwise needs a pin? Error if 255?
                       if(espConfig::miscConfig.gpioActionPin != 255) {
                           digitalWrite(espConfig::miscConfig.gpioActionPin, pressLevel);
                           vTaskDelay(pdMS_TO_TICKS(espConfig::miscConfig.gpioActionMomentaryTimeout > 0 ? espConfig::miscConfig.gpioActionMomentaryTimeout : 200)); // Use timeout or default
                           digitalWrite(espConfig::miscConfig.gpioActionPin, releaseLevel);
                       } else {
                            LOG(E, "Dumb Switch Mode enabled but no valid gpioActionPin configured!");
                       }
                      // In dumb mode, HK state might just toggle immediately
                      lockCurrentState->setVal(target_state_val);
                      if (client) esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(target_state_val).c_str(), 0, 1, true);
                      continue; // Skip momentary logic below for dumb mode? Or adapt it?
                  }
                   // --- End Physical Action ---


                  // --- Momentary Logic (Only if NOT Dumb Mode and pin is valid) ---
                  if (espConfig::miscConfig.gpioActionPin != 255 && isUnlockAction && (static_cast<uint8_t>(espConfig::miscConfig.gpioActionMomentaryEnabled) & status.source)) {
                      LOG(D, "GPIO Task: Starting momentary delay (%d ms) for UNLOCK", espConfig::miscConfig.gpioActionMomentaryTimeout);
                      vTaskDelay(pdMS_TO_TICKS(espConfig::miscConfig.gpioActionMomentaryTimeout));

                      LOG(D, "GPIO Task: Momentary timeout, returning pin %d to LOCK level (%d).", espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);
                      digitalWrite(espConfig::miscConfig.gpioActionPin, espConfig::miscConfig.gpioActionLockState);

                      // Update HomeKit state AFTER momentary action completes
                      // Check if HomeKit still expects UNLOCKED (user might have changed target during delay)
                      if(lockTargetState->getVal() == lockStates::UNLOCKED) {
                          LOG(D,"GPIO Task: Momentary done, setting TargetState back to LOCKED");
                          lockTargetState->setVal(lockStates::LOCKED); // Re-target to Locked
                      }
                      lockCurrentState->setVal(lockStates::LOCKED); // Set current to Locked
                      if (client) esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);

                  } else if (espConfig::miscConfig.gpioActionPin != 255) {
                      // If NOT momentary, or if it was a LOCK action:
                      // Physical action is done, update HomeKit current state to match the final target state
                      LOG(D, "GPIO Task: Non-momentary action complete, setting CurrentState to %d", target_state_val);
                      // Optional short delay if physical lock needs time to settle before updating state
                      // vTaskDelay(pdMS_TO_TICKS(200));
                      lockCurrentState->setVal(target_state_val);
                      if (client) esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(target_state_val).c_str(), 0, 1, true);
                  }
                  // --- End Momentary Logic ---

              }
              // Process action 2 (stop task command)
              else if (status.action == 2) {
                  LOG(I, "gpio_task stopping command received.");
                  vTaskDelete(NULL); // Gracefully delete self
                  return; // Exit function
              }
          } // End if xQueueReceive successful
      } else {
          // Handle case where queue handle is somehow null (shouldn't happen if setup is correct)
          LOG(E, "gpio_task: gpio_lock_handle is NULL!");
          vTaskDelay(pdMS_TO_TICKS(1000)); // Delay longer before retrying
      }

      vTaskDelay(loop_delay_ticks); // Prevent task from hogging CPU
  } // End while(1)
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
            vTaskDelay(pdMS_TO_TICKS(espConfig::miscConfig.neopixelFailTime));
            pixel->off();
          }
          break;
        case 1:
          if (espConfig::miscConfig.nfcNeopixelPin && espConfig::miscConfig.nfcNeopixelPin != 255) {
            LOG(D, "FAIL PIXEL %d:%d,%d,%d", espConfig::miscConfig.nfcNeopixelPin, espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::R], espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::G], espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::B]);
            pixel->set(pixel->RGB(espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::R], espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::G], espConfig::miscConfig.neopixelSuccessColor[espConfig::misc_config_t::colorMap::B]));
            vTaskDelay(pdMS_TO_TICKS(espConfig::miscConfig.neopixelSuccessTime));
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
            vTaskDelay(pdMS_TO_TICKS(espConfig::miscConfig.nfcFailTime));
            digitalWrite(espConfig::miscConfig.nfcFailPin, !espConfig::miscConfig.nfcFailHL);
          }
          break;
        case 1:
          if (espConfig::miscConfig.nfcSuccessPin && espConfig::miscConfig.nfcSuccessPin != 255) {
            LOG(D, "SUCCESS LED %d:%d", espConfig::miscConfig.nfcSuccessPin, espConfig::miscConfig.nfcSuccessHL);
            digitalWrite(espConfig::miscConfig.nfcSuccessPin, espConfig::miscConfig.nfcSuccessHL);
            vTaskDelay(pdMS_TO_TICKS(espConfig::miscConfig.nfcSuccessTime));
            digitalWrite(espConfig::miscConfig.nfcSuccessPin, !espConfig::miscConfig.nfcSuccessHL);
          }
          break;
        case 2:
          if(hkAltActionActive){
            digitalWrite(espConfig::miscConfig.hkAltActionPin, espConfig::miscConfig.hkAltActionGpioState);
            vTaskDelay(pdMS_TO_TICKS(espConfig::miscConfig.hkAltActionTimeout));
            digitalWrite(espConfig::miscConfig.hkAltActionPin, !espConfig::miscConfig.hkAltActionGpioState);
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
    LOG(I, "Configuring LockMechanism");
    lockCurrentState = new Characteristic::LockCurrentState(1, true);
    lockTargetState = new Characteristic::LockTargetState(1, true);

    // --- Protect read of readerData.reader_gid ---
    if (readerDataMutex != nullptr) {
        if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
            if(readerData.reader_gid.size() >= 8) {
                memcpy(ecpData + 8, readerData.reader_gid.data(), 8); // Read shared data
                with_crc16(ecpData, 16, ecpData + 16);
            } else {
                LOG(W, "Reader GID not available/valid size in LockMechanism constructor.");
            }
            xSemaphoreGive(readerDataMutex);
        } else {
            LOG(E, "Failed to take readerDataMutex in LockMechanism constructor!");
        }
    } else {
         LOG(E, "readerDataMutex not initialized for LockMechanism constructor!");
    }
  } 

  boolean update() {
    int targetState = lockTargetState->getNewVal();
    LOG(I, "New LockState=%d, Current LockState=%d", targetState, lockCurrentState->getVal());

    // Sends the action to the gpio_task queue
    if ((espConfig::miscConfig.gpioActionPin != 255 && espConfig::miscConfig.hkGpioControlledState) || espConfig::miscConfig.hkDumbSwitchMode) {
      // Only send to queue if the pin is enabled OR dumb switch mode is on
      const gpioLockAction gpioAction{ .source = gpioLockAction::HOMEKIT, .action = 0 }; // action 0 = process state change
      LOG(D, "LockMechanism::update() sending action to gpio_task queue.");
      // Use a small timeout for the queue send in case the queue is full
      if (xQueueSend(gpio_lock_handle, &gpioAction, pdMS_TO_TICKS(50)) != pdPASS) {
           LOG(E, "Failed to send action to gpio_lock_handle queue!");
      }
    } else {
         LOG(D, "LockMechanism::update() - GPIO action pin disabled or not HK controlled, not sending to queue.");
    }

    int currentState = lockCurrentState->getNewVal(); // Use getNewVal as it reflects intermediate states maybe? Or getVal()? Test this.
    if (client != nullptr) {
        // Report intermediate states if possible (gpio_task might set these)
        if(currentState == lockStates::UNLOCKING || currentState == lockStates::LOCKING) {
             esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(currentState).c_str(), 1, 1, true);
        } else {
            // Report final target state once reached (gpio_task should update this)
             esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(targetState).c_str(), 1, 1, true);
        }

      if (espConfig::mqttData.lockEnableCustomState) {
        if (targetState == lockStates::UNLOCKED) {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]).c_str(), 0, 0, false);
        } else if (targetState == lockStates::LOCKED) {
          esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["LOCK"]).c_str(), 0, 0, false);
        }
      }
    } else LOG(W, "MQTT Client not initialized, cannot publish message");

    // HomeSpan expects update() to return true if the action is accepted.
    // Since we delegate to gpio_task, we should usually return true here.
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
    LOG(D,"NFCAccess::update() called.");
    boolean resultStatus = false; // To return status
    TLV8 ctrlData(NULL, 0);
    nfcControlPoint->getNewTLV(ctrlData); // Get data from HAP
    std::vector<uint8_t> tlvData(ctrlData.pack_size());
    ctrlData.pack(tlvData.data());

    if (tlvData.size() == 0) {
         LOG(W, "NFCAccess::update received empty control point data.");
         return false; // No data to process
    }

    LOG(D, "Decoded TLV data length: %d", tlvData.size());
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, tlvData.data(), tlvData.size(), ESP_LOG_VERBOSE);

    // --- Protect readerData modification and save ---
    if (readerDataMutex == nullptr) {
        LOG(E, "readerDataMutex not initialized in NFCAccess::update!");
        return false;
    }
    if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(2000)) == pdTRUE) { // Longer timeout maybe needed for processing+NVS
         // --- Start Critical Section ---
         LOG(D, "Processing NFC Control Point data (mutex acquired).");
         LOG(D, "Current Reader Key (before processing): %s", red_log::bufToHexString(readerData.reader_pk.data(), readerData.reader_pk.size()).c_str());

         // HK_HomeKit processes TLV and potentially modifies readerData
         HK_HomeKit hkCtx(readerData, savedData, "READERDATA", tlvData);
         std::vector<uint8_t> result = hkCtx.processResult();

         // Check if GID was updated by hkCtx before using it and saving
         LOG(D, "Reader GID (after processing): %s", red_log::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size()).c_str());
         if (readerData.reader_gid.size() >= 8) {
             memcpy(ecpData + 8, readerData.reader_gid.data(), 8); // Update ecpData based on potentially new GID
             with_crc16(ecpData, 16, ecpData + 16);
         }

         // Save the potentially modified readerData
         LOG(D, "Saving potentially modified readerData to NVS...");
         save_to_nvs_internal(); // Call internal save function

         // Set the response TLV for HomeKit
         TLV8 res(NULL, 0);
         res.unpack(result.data(), result.size());
         nfcControlPoint->setTLV(res, false);
         resultStatus = true; // Indicate success
         // --- End Critical Section ---
         xSemaphoreGive(readerDataMutex);
         LOG(D, "NFCAccess::update finished processing (mutex released).");
    } else {
         LOG(E, "Failed to take readerDataMutex in NFCAccess::update!");
         resultStatus = false;
         // Optionally set an error TLV?
         // TLV8 errRes; errRes.add(HAP_TLV_TYPE_ERROR, HAP_TLV_ERROR_BUSY);
         // nfcControlPoint->setTLV(errRes, false);
    }
    // --- End protection ---

    return resultStatus;
}

void deleteReaderData(const char* buf = "") {
  LOG(W, "Attempting to delete reader data...");
  // --- Protect readerData modification ---
  if (readerDataMutex == nullptr) {
      LOG(E, "readerDataMutex not initialized in deleteReaderData!");
      return;
  }
  if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
      // --- Start Critical Section ---
      esp_err_t erase_nvs = nvs_erase_key(savedData, "READERDATA");
      esp_err_t commit_nvs = nvs_commit(savedData); // Commit erase
      LOG(D, "*** NVS W STATUS");
      LOG(D, "ERASE: %s", esp_err_to_name(erase_nvs));
      LOG(D, "COMMIT: %s", esp_err_to_name(commit_nvs));
      LOG(D, "*** NVS W STATUS");

      // Clear in-memory data *after* successful erase/commit
      if (erase_nvs == ESP_OK && commit_nvs == ESP_OK) {
          LOG(I,"Clearing in-memory readerData structure.");
          readerData.issuers.clear();
          readerData.reader_gid.clear();
          readerData.reader_id.clear();
          readerData.reader_pk.clear();
          readerData.reader_pk_x.clear();
          readerData.reader_sk.clear();
      } else {
           LOG(E, "Failed to erase/commit readerData from NVS. In-memory data NOT cleared.");
      }
      // --- End Critical Section ---
      xSemaphoreGive(readerDataMutex);
  } else {
       LOG(E, "Failed to take readerDataMutex in deleteReaderData!");
  }
  // --- End protection ---
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
  LOG(I, "Pairing callback triggered.");
  // --- Protect readerData modification and save ---
  if (readerDataMutex == nullptr) {
      LOG(E, "readerDataMutex not initialized in pairCallback!");
      return;
  }
  if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
       // --- Start Critical Section ---
       if (HAPClient::nAdminControllers() == 0) {
           LOG(I, "pairCallback: No controllers paired. Clearing readerData.");
           // Clear in-memory data first
           readerData.issuers.clear();
           readerData.reader_gid.clear();
           readerData.reader_id.clear();
           readerData.reader_pk.clear();
           readerData.reader_pk_x.clear();
           readerData.reader_sk.clear();
           // Then erase from NVS
           nvs_erase_key(savedData, "READERDATA");
           nvs_commit(savedData); // Commit erase
       } else {
           LOG(I, "pairCallback: Processing %d controllers.", HAPClient::nAdminControllers());
           bool changed = false; // Track if we actually modify data
           for (auto it = homeSpan.controllerListBegin(); it != homeSpan.controllerListEnd(); ++it) {
               std::vector<uint8_t> id = getHashIdentifier(it->getLTPK(), 32);
               LOG(D, "Controller hash: %s", red_log::bufToHexString(id.data(), 8).c_str());
               bool issuer_found = false;
               for (auto&& issuer : readerData.issuers) { // Read shared data
                   if (issuer.issuer_id.size() == 8 && std::equal(issuer.issuer_id.begin(), issuer.issuer_id.end(), id.begin())) {
                       LOG(D, "Issuer %s already added.", red_log::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str());
                       issuer_found = true;
                       break;
                   }
               }
               if (!issuer_found) {
                   LOG(I, "Adding new issuer - ID: %s", red_log::bufToHexString(id.data(), 8).c_str());
                   hkIssuer_t newIssuer;
                   newIssuer.issuer_id = std::vector<uint8_t>{ id.begin(), id.begin() + 8 };
                   newIssuer.issuer_pk.assign(it->getLTPK(), it->getLTPK() + 32); // Use assign for clarity
                   readerData.issuers.emplace_back(newIssuer); // Modify shared data
                   changed = true; // Mark that data was changed
               }
           } // end for loop controllers

           // Save only if data actually changed
           if(changed) {
                LOG(I, "pairCallback: readerData modified, saving to NVS...");
                save_to_nvs_internal(); // Call internal save function
           } else {
                LOG(D, "pairCallback: No changes detected in readerData issuers.");
           }
       } // end else (controllers > 0)
       // --- End Critical Section ---
      xSemaphoreGive(readerDataMutex);
  } else {
      LOG(E, "Failed to take readerDataMutex in pairCallback!");
  }
  // --- End protection ---
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
  LOG(I, "--- Printing Issuers ---");
  // --- Protect readerData read ---
  if (readerDataMutex == nullptr) {
      LOG(E, "readerDataMutex not initialized in print_issuers!");
      return;
  }
  if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
       // --- Start Critical Section ---
       if(readerData.issuers.empty()) {
           LOG(I, "  (No issuers configured)");
       } else {
           for (auto&& issuer : readerData.issuers) { // Read shared data
               LOG(I, "  Issuer ID: %s, PK: %s...", red_log::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str(), red_log::bufToHexString(issuer.issuer_pk.data(), 8).c_str()); // Log only start of PK maybe
               if(!issuer.endpoints.empty()) {
                   LOG(I, "    Endpoints:");
                   for (auto&& endpoint : issuer.endpoints) { // Read shared data
                       LOG(I, "      Endpoint ID: %s, PK: %s...", red_log::bufToHexString(endpoint.endpoint_id.data(), endpoint.endpoint_id.size()).c_str(), red_log::bufToHexString(endpoint.endpoint_pk.data(), 8).c_str());
                   }
               } else {
                    LOG(I, "    (No endpoints for this issuer)");
               }
           }
       }
       // --- End Critical Section ---
       xSemaphoreGive(readerDataMutex);
  } else {
       LOG(E, "Failed to take readerDataMutex in print_issuers!");
  }
  // --- End protection ---
   LOG(I, "--- End Printing Issuers ---");
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
  // Ensure states that imply an ACTION (like UNLOCKING/LOCKING) primarily set the target state.
  // States that just REPORT status (like UNLOCKED/LOCKED/JAMMED) should set the current state.

  if (espConfig::mqttData.customLockStates["C_UNLOCKING"] == state) {
    LOG(I, "MQTT set_custom_state_handler: Received C_UNLOCKING. Setting target state.");
    lockTargetState->setVal(lockStates::UNLOCKED); // Use Target State to trigger action
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKING).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_LOCKING"] == state) {
    LOG(I, "MQTT set_custom_state_handler: Received C_LOCKING. Setting target state.");
    lockTargetState->setVal(lockStates::LOCKED); // Use Target State to trigger action
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKING).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_UNLOCKED"] == state) {
     LOG(I, "MQTT set_custom_state_handler: Received C_UNLOCKED. Updating current state.");
    // This reports the lock IS unlocked. Only update CurrentState.
    // Don't call digitalWrite here. If the GPIO needs setting, the source should ensure it happened.
    lockCurrentState->setVal(lockStates::UNLOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKED).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_LOCKED"] == state) {
     LOG(I, "MQTT set_custom_state_handler: Received C_LOCKED. Updating current state.");
    // This reports the lock IS locked. Only update CurrentState.
    lockCurrentState->setVal(lockStates::LOCKED);
    // Optional: Ensure TargetState matches if necessary?
    // if(lockTargetState->getVal() != lockStates::LOCKED) lockTargetState->setVal(lockStates::LOCKED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKED).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_JAMMED"] == state) {
    LOG(I, "MQTT set_custom_state_handler: Received C_JAMMED. Updating current state.");
    lockCurrentState->setVal(lockStates::JAMMED);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::JAMMED).c_str(), 0, 1, true);
    return;
  } else if (espConfig::mqttData.customLockStates["C_UNKNOWN"] == state) {
    LOG(I, "MQTT set_custom_state_handler: Received C_UNKNOWN. Updating current state.");
    lockCurrentState->setVal(lockStates::UNKNOWN);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNKNOWN).c_str(), 0, 1, true);
    return;
  }
  LOG(W, "MQTT set_custom_state_handler: Update state failed! Received invalid custom state value: %d", state);
}

void set_state_handler(esp_mqtt_client_handle_t client, int state) {
  switch (state) {
  case lockStates::UNLOCKED:
    // REMOVE: digitalWrite call block
    LOG(I, "MQTT set_state_handler: Received UNLOCK command. Setting target state.");
    lockTargetState->setVal(state); // Trigger HomeKit update -> LockMechanism::update -> gpio_task
    // lockCurrentState->setVal(state); // Don't set current state here, let gpio_task confirm it
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::UNLOCKING).c_str(), 0, 1, true); // Publish UNLOCKING state
    if (espConfig::mqttData.lockEnableCustomState) {
      esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]).c_str(), 0, 0, false);
    }
    break;
  case lockStates::LOCKED:
    LOG(I, "MQTT set_state_handler: Received LOCK command. Setting target state.");
    lockTargetState->setVal(state); // Trigger HomeKit update -> LockMechanism::update -> gpio_task
    // lockCurrentState->setVal(state); // Don't set current state here
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(lockStates::LOCKING).c_str(), 0, 1, true); // Publish LOCKING state
    if (espConfig::mqttData.lockEnableCustomState) {
      esp_mqtt_client_publish(client, espConfig::mqttData.lockCustomStateTopic.c_str(), std::to_string(espConfig::mqttData.customLockActions["LOCK"]).c_str(), 0, 0, false);
    }
    break;
  case lockStates::JAMMED: // These only update CURRENT state, no physical action triggered
  case lockStates::UNKNOWN:
    LOG(I, "MQTT set_state_handler: Received non-actionable state %d. Updating current state.", state);
    lockCurrentState->setVal(state);
    esp_mqtt_client_publish(client, espConfig::mqttData.lockStateTopic.c_str(), std::to_string(state).c_str(), 0, 1, true);
    break;
  default:
    LOG(W, "MQTT set_state_handler: Update state failed! Received invalid state value: %d", state);
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
  if (espConfig::mqttData.hassMqttDiscoveryEnabled) {
    json payload;
    payload["topic"] = espConfig::mqttData.hkTopic.c_str();
    payload["value_template"] = "{{ value_json.uid }}";
    json device;
    device["name"] = espConfig::miscConfig.deviceName.c_str();
    char identifier[18];
    std::string hap_id_str = HAPClient::accessory.ID;
    // sprintf(identifier, "%.2s%.2s%.2s%.2s%.2s%.2s", HAPClient::accessory.ID, HAPClient::accessory.ID + 3, HAPClient::accessory.ID + 6, HAPClient::accessory.ID + 9, HAPClient::accessory.ID + 12, HAPClient::accessory.ID + 15);
    std::string id = identifier;
    device["identifiers"].push_back(hap_id_str);     // Add real ID to device identifiers
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
    payload["unique_id"] = hap_id_str;
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
  esp_mqtt_client_config_t mqtt_cfg {};
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
    eth_config["ethEnabled"] = espConfig::miscConfig.ethernetEnabled;
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
              pixel = std::make_shared<Pixel>(it.value(), PixelType::GRB);
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
          uint8_t pixelType = it.value().template get<uint8_t>();
          if (pixelType != configData.at(it.key()).template get<uint8_t>()) {
            rebootNeeded = true;
            rebootMsg = "Pixel Type was changed, reboot needed! Rebooting...";
          }
        } else if (it.key() == std::string("gpioActionPin")) {
          if (espConfig::miscConfig.gpioActionPin == 255 && it.value() != 255 ) {
            LOG(D, "ENABLING HomeKit Trigger - Simple GPIO");
            pinMode(it.value(), OUTPUT);
            if(gpio_lock_task_handle == nullptr){
              xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, &gpio_lock_task_handle);
            }
            if(espConfig::miscConfig.hkDumbSwitchMode){
              serializedData->at("hkDumbSwitchMode") = false;
            }
          } else if (espConfig::miscConfig.gpioActionPin != 255 && it.value() == 255) {
            LOG(D, "DISABLING HomeKit Trigger - Simple GPIO");
            if( gpio_lock_task_handle != nullptr){
              gpioLockAction status{ .source = gpioLockAction::OTHER, .action = 2 };
              xQueueSend(gpio_lock_handle, &status, 0);
              gpio_lock_task_handle = nullptr;
            }
            gpio_reset_pin(gpio_num_t(espConfig::miscConfig.gpioActionPin));
          }
        } else if (it.key() == std::string("hkDumbSwitchMode") && gpio_lock_task_handle == nullptr) {
          xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, &gpio_lock_task_handle);
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
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP.restart();
    });
  webServer.addHandler(rebootDeviceHandle);
  auto startConfigAP = new AsyncCallbackWebHandler();
  startConfigAP->setUri("/start_config_ap");
  startConfigAP->setMethod(HTTP_GET);
  startConfigAP->onRequest([](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Starting the AP...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    webServer.end();
    homeSpan.processSerialCommand("A");
    });
  webServer.addHandler(startConfigAP);
  auto resetHkHandle = new AsyncCallbackWebHandler();
  resetHkHandle->setUri("/reset_hk_pair");
  resetHkHandle->setMethod(HTTP_GET);
  resetHkHandle->onRequest([](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Erasing HomeKit pairings and restarting...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    deleteReaderData();
    homeSpan.processSerialCommand("H");
    });
  webServer.addHandler(resetHkHandle);
  auto resetWifiHandle = new AsyncCallbackWebHandler();
  resetWifiHandle->setUri("/reset_wifi_cred");
  resetWifiHandle->setMethod(HTTP_GET);
  resetWifiHandle->onRequest([](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Erasing WiFi credentials and restarting, AP will start on boot...");
    vTaskDelay(pdMS_TO_TICKS(1000));
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
    startConfigAP->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
    ethSuppportConfig->setAuthentication(espConfig::miscConfig.webUsername.c_str(), espConfig::miscConfig.webPassword.c_str());
  }
  webServer.onNotFound(notFound);
  webServer.begin();
}

void mqttConfigReset(const char* buf) {
  nvs_erase_key(savedData, "MQTTDATA");
  nvs_commit(savedData);
  ESP.restart();
}

void wifiCallback(int status) {
  if (status == 1) {
    if (espConfig::mqttData.mqttBroker.size() >= 7 && espConfig::mqttData.mqttBroker.size() <= 16 && !std::equal(espConfig::mqttData.mqttBroker.begin(), espConfig::mqttData.mqttBroker.end(), "0.0.0.0")) {
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

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h> // Header for semaphores (mutexes)
// ... other includes like HAP.h, HKAuthContext.h, PN532.h, logging.h, etc.

// --- Assume these globals are declared elsewhere ---
extern PN532 *nfc;
extern readerData_t readerData;
extern SemaphoreHandle_t readerDataMutex; // Mutex handle must be created in setup()
extern QueueHandle_t gpio_led_handle;
extern QueueHandle_t neopixel_handle;
extern QueueHandle_t gpio_lock_handle;
extern TaskHandle_t nfc_reconnect_task;
extern esp_mqtt_client_handle_t client;
extern SpanCharacteristic* lockCurrentState;
extern SpanCharacteristic* lockTargetState;
extern KeyFlow hkFlow;
extern bool hkAltActionActive;
extern uint8_t ecpData[18]; // Used for communicateThru

// Function declarations needed if not already visible
extern std::string hex_representation(const std::vector<uint8_t>& v);
extern void mqtt_publish(std::string topic, std::string payload, uint8_t qos, bool retain);
extern void with_crc16(unsigned char* data, unsigned int size, unsigned char* result); // Assuming this is declared
extern void nfc_retry(void* arg); 
extern void trigger_nfc_reconnect(const char* reason);
// --- End Assume Globals/Declarations ---


// Stops NFC, ensures the retry task is created (if needed), and suspends the current task.
void trigger_nfc_reconnect(const char* reason) {
  const char* TAG_RECONNECT = "NFC_RECONNECT";
  ESP_LOGE(TAG_RECONNECT, "Triggering PN532 reconnect due to: %s", reason);

  if (nfc) {
      nfc->stop(); // Attempt to cleanly stop the NFC interface
  }

  // Check if the reconnect task handle is already valid (meaning task might exist)
  // A more robust check would involve querying task state if possible, but checking
  // the handle is a basic preventative measure against creating duplicates rapidly.
  if (nfc_reconnect_task == nullptr) {
      ESP_LOGI(TAG_RECONNECT, "Creating nfc_reconnect_task...");
      BaseType_t ret = xTaskCreate(nfc_retry, "nfc_reconnect_task", 8192, NULL, 5, &nfc_reconnect_task); // Increased priority maybe?
      if (ret != pdPASS) {
          ESP_LOGE(TAG_RECONNECT, "Failed to create nfc_reconnect_task! System may not recover NFC.");
          // Cannot suspend if task creation failed, maybe just loop with delay?
          // Or maybe attempt recovery again later? For now, log the error.
          // Ideally, we should not reach the suspend below if creation fails.
           nfc_reconnect_task = nullptr; // Ensure handle is null if creation failed
           // If creation failed, maybe just return and let the main loop try again?
           // return; // Decide on failure strategy here. For now, proceed to suspend.
      }
  } else {
      ESP_LOGW(TAG_RECONNECT, "nfc_reconnect_task handle already exists. Assuming task is running or pending.");
      // If the task exists but crashed, this won't restart it. A more advanced
      // system might delete the old task handle and create a new one.
  }

  ESP_LOGW(TAG_RECONNECT, "Suspending nfc_poll_task. nfc_reconnect_task will attempt recovery.");
  vTaskSuspend(NULL); // Suspend the current task (nfc_poll_task)
}

// =========================================================================
// ==               NFC Thread Entry Function (with Mutex)                ==
// =========================================================================
void nfc_thread_entry(void* arg) {
  const char* TAG_NFC = "NFC_TASK"; // Tag for logging within this task
  uint32_t versiondata = 0;
  bool nfc_initialized = false;

  // --- Initial PN532 Check & Setup ---
  ESP_LOGI(TAG_NFC, "Starting initial PN532 check...");
  versiondata = nfc->getFirmwareVersion();
  if (!versiondata) {
      ESP_LOGE(TAG_NFC, "Initial PN532 check failed. Triggering reconnect...");
      trigger_nfc_reconnect("Initial getFirmwareVersion failed");
      // Should not return here if suspended correctly
      return;
  } else {
      // Log firmware version etc.
      unsigned int model = (versiondata >> 24) & 0xFF;
      ESP_LOGI(TAG_NFC, "Found chip PN5%x", model);
      int maj = (versiondata >> 16) & 0xFF;
      int min = (versiondata >> 8) & 0xFF;
      ESP_LOGI(TAG_NFC, "Firmware ver. %d.%d", maj, min);

      // Configure the PN532
      if (!nfc->SAMConfig()) {
           ESP_LOGE(TAG_NFC, "Initial SAMConfig failed. Triggering reconnect...");
           trigger_nfc_reconnect("Initial SAMConfig failed");
           return; // Should not return
      }
      if (!nfc->setRFField(0x02, 0x01)) { // Turn RF field on
           ESP_LOGE(TAG_NFC, "Initial setRFField failed. Triggering reconnect...");
           trigger_nfc_reconnect("Initial setRFField failed");
           return; // Should not return
      }
      nfc->setPassiveActivationRetries(0); // Don't retry endlessly in readPassiveTargetID during polling
      nfc_initialized = true; // Mark as successfully initialized
      ESP_LOGI(TAG_NFC, "NFC Initialized. Waiting for an ISO14443A card...");
  }

  // ===========================//
  // === Main NFC Loop ===      //
  // ===========================//
  while (1) {
      if (!nfc_initialized) {
          // Should not happen if initial checks passed, but as a safeguard
          ESP_LOGE(TAG_NFC, "NFC not initialized at start of loop. Attempting reconnect.");
          trigger_nfc_reconnect("NFC initialization flag was false");
          continue; // Skip rest of loop, will suspend in helper
      }

      // --- Prepare ECP Data (Option 2: Inside loop, under mutex) ---
      bool ecp_prep_success = false;
      if (readerDataMutex != nullptr) {
          if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(50)) == pdTRUE) { // Short timeout
              // --- Start Critical Section (Read GID) ---
              if(readerData.reader_gid.size() >= 8) {
                  memcpy(ecpData + 8, readerData.reader_gid.data(), 8);
                  with_crc16(ecpData, 16, ecpData + 16);
                  ecp_prep_success = true;
              } else {
                  ESP_LOGD(TAG_NFC, "Reader GID invalid size (%d) for ECP data in loop.", readerData.reader_gid.size());
              }
              // --- End Critical Section ---
              xSemaphoreGive(readerDataMutex);
          } else {
              ESP_LOGW(TAG_NFC, "Failed mutex for ECP prep in loop (Timeout?). Skipping CommunicateThru.");
          }
      } else {
          ESP_LOGE(TAG_NFC, "Mutex invalid for ECP prep in loop!");
          // Cannot prepare ECP data safely
      }
      // --- End ECP Data Prep ---


      // --- Optional: Wakeup/CommunicateThru Command ---
      if (ecp_prep_success) { // Only run if ECP data was prepared successfully
          uint8_t res[4];
          uint16_t resLen = 4;
          // Write register (Check for failure -> indicates chip issue)
          if (!nfc->writeRegister(0x633d, 0, true)) {
              trigger_nfc_reconnect("writeRegister(0x633d) failed");
              continue; // Skip rest of loop, will suspend in helper
          }
          // Send ECP data (Failure here might be less critical for wakeup?)
          if (!nfc->inCommunicateThru(ecpData, sizeof(ecpData), res, &resLen, 100, true)) {
               ESP_LOGW(TAG_NFC, "inCommunicateThru failed. Card might not wake quickly.");
               // Decide if this warrants a full reconnect or just continue polling
               // For now, just log and continue polling
          }
      } else {
           ESP_LOGD(TAG_NFC, "Skipping CommunicateThru, ECP data prep failed.");
           // Maybe still do writeRegister? Depends on its purpose.
           // If writeRegister is essential even without ECP:
           // if (!nfc->writeRegister(0x633d, 0, true)) {
           //     trigger_nfc_reconnect("writeRegister(0x633d) failed (no ECP)");
           //     continue;
           // }
      }
      // --- End Optional Wakeup ---


      // --- Poll for Passive Target ---
      uint8_t uid[16];
      uint8_t uidLen = 0;
      uint8_t atqa[2];
      uint8_t sak[1];
      // The PN532 library's readPassiveTargetID often returns `false` on timeout,
      // but might return `false` for other communication errors too.
      // We treat timeout (no card) as normal, but need to consider other failures.
      // Unfortunately, the library might not distinguish error from timeout easily.
      // We rely on the writeRegister check above as the main indicator of chip health.
      bool passiveTarget = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, atqa, sak, 500, true, true);


      // --- Process if Target Found ---
      if (passiveTarget) {
          ESP_LOGI(TAG_NFC, "*** PASSIVE TARGET DETECTED (UID Len: %d) ***", uidLen);
          ESP_LOGD(TAG_NFC, "ATQA: %02x%02x, SAK: %02x", atqa[1], atqa[0], sak[0]);
          ESP_LOG_BUFFER_HEX_LEVEL(TAG_NFC, uid, uidLen, ESP_LOG_VERBOSE);

          nfc->setPassiveActivationRetries(5); // Increase retries for subsequent commands
          auto startTime = std::chrono::high_resolution_clock::now();

          // --- Select HomeKey Applet ---
          uint8_t selectApdu[] = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x00 };
          uint8_t selectCmdRes[64];
          uint16_t selectCmdResLength = sizeof(selectCmdRes);
          ESP_LOGI(TAG_NFC, "Selecting HomeKey Applet...");

          // Check return status of inDataExchange
          bool selectStatus = nfc->inDataExchange(selectApdu, sizeof(selectApdu), selectCmdRes, &selectCmdResLength);

          ESP_LOGD(TAG_NFC, "SELECT Applet Response (Status: %d, Len: %d)", selectStatus, selectCmdResLength);
          ESP_LOG_BUFFER_HEX_LEVEL(TAG_NFC, selectCmdRes, selectCmdResLength, ESP_LOG_VERBOSE);

          // --- Authenticate if Select Successful ---
          // Check selectStatus AND the SW1/SW2 bytes
          if (selectStatus && selectCmdResLength >= 2 && selectCmdRes[selectCmdResLength - 2] == 0x90 && selectCmdRes[selectCmdResLength - 1] == 0x00) {
              ESP_LOGD(TAG_NFC, "*** SELECT HOMEKEY APPLET SUCCESSFUL ***");

              // --- Authenticate (Protected Access to readerData) ---
              std::vector<uint8_t> issuerIdResult;
              std::vector<uint8_t> endpointIdResult;
              std::string readerIdHexResult;
              KeyFlow flowResult = kFlowFailed;
              bool authAttempted = false;

              if (readerDataMutex != nullptr) {
                  if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(2000)) == pdTRUE) { // Allow longer time for auth
                      authAttempted = true;
                      ESP_LOGD(TAG_NFC, "Mutex acquired for HomeKey authentication.");
                      // --- Start Critical Section ---
                      readerIdHexResult = hex_representation(readerData.reader_id);
                      // Create Auth Context - check if reader SK exists first?
                      if(readerData.reader_sk.empty()) {
                          ESP_LOGE(TAG_NFC,"Authentication impossible: Reader secret key is missing!");
                          flowResult = kFlowFailed; // Ensure failure state
                      } else {
                          HKAuthenticationContext authCtx(
                              [](uint8_t* s, uint8_t l, uint8_t* r, uint16_t* rl, bool il) -> bool {
                                  // Lambda for NFC exchange - add logging/error check here?
                                  bool nfc_status = nfc->inDataExchange(s, l, r, rl, il);
                                  if (!nfc_status) {
                                      ESP_LOGW("NFC_AUTH_LAMBDA", "inDataExchange failed during authentication step!");
                                      // Don't trigger reconnect here, let AuthCtx handle flow failure
                                  }
                                  return nfc_status;
                              },
                              readerData, savedData
                          );
                          ESP_LOGI(TAG_NFC, "Starting HomeKey authentication (Flow: %d)...", hkFlow);
                          auto authResultTuple = authCtx.authenticate(hkFlow);
                          issuerIdResult = std::get<0>(authResultTuple);
                          endpointIdResult = std::get<1>(authResultTuple);
                          flowResult = std::get<2>(authResultTuple);
                          ESP_LOGI(TAG_NFC, "HomeKey authentication finished (Result Flow: %d).", flowResult);
                      }
                      // --- End Critical Section ---
                      xSemaphoreGive(readerDataMutex);
                      ESP_LOGD(TAG_NFC, "Mutex released after HomeKey authentication attempt.");
                  } else {
                      ESP_LOGE(TAG_NFC, "Failed to take readerDataMutex for authentication!");
                      // flowResult remains kFlowFailed
                  }
              } else {
                  ESP_LOGE(TAG_NFC, "readerDataMutex not initialized for authentication!");
                  // flowResult remains kFlowFailed
              }
              // --- End Protected Authentication ---


              // --- Process Authentication Result (outside the lock) ---
              if (authAttempted && flowResult != kFlowFailed) {
                  ESP_LOGI(TAG_NFC, ">>> HomeKey Authentication Successful! <<<");
                  bool successStatus = true;
                  if (espConfig::miscConfig.nfcSuccessPin != 255) xQueueSend(gpio_led_handle, &successStatus, 0);
                  if (espConfig::miscConfig.nfcNeopixelPin != 255) xQueueSend(neopixel_handle, &successStatus, 0);

                  if ((espConfig::miscConfig.gpioActionPin != 255 && espConfig::miscConfig.hkGpioControlledState) || espConfig::miscConfig.hkDumbSwitchMode) {
                      ESP_LOGD(TAG_NFC,"Sending action to gpio_task queue due to successful HomeKey auth.");
                      const gpioLockAction action{ .source = gpioLockAction::HOMEKEY, .action = 0 };
                      if (gpio_lock_handle) xQueueSend(gpio_lock_handle, &action, pdMS_TO_TICKS(50));
                  }

                  if (espConfig::miscConfig.hkAltActionInitPin != 255 && espConfig::miscConfig.hkAltActionPin != 255 && hkAltActionActive) {
                       ESP_LOGI(TAG_NFC, "Alt Action is active, triggering related GPIO/MQTT.");
                       uint8_t alt_action_status = 2;
                       if(gpio_led_handle) xQueueSend(gpio_led_handle, &alt_action_status, 0);
                       mqtt_publish(espConfig::mqttData.hkAltActionTopic, "alt_action", 0, false);
                  }

                  json payload;
                  payload["issuerId"] = hex_representation(issuerIdResult);
                  payload["endpointId"] = hex_representation(endpointIdResult);
                  payload["readerId"] = readerIdHexResult;
                  payload["homekey"] = true;
                  mqtt_publish(espConfig::mqttData.hkTopic, payload.dump(), 0, false);

                  if (espConfig::miscConfig.lockAlwaysUnlock) {
                       ESP_LOGI(TAG_NFC, "Config lockAlwaysUnlock=true, setting TargetState to UNLOCKED.");
                       lockTargetState->setVal(lockStates::UNLOCKED);
                       if (espConfig::mqttData.lockEnableCustomState) mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["UNLOCK"]), 0, false);
                  } else if (espConfig::miscConfig.lockAlwaysLock) {
                       ESP_LOGI(TAG_NFC, "Config lockAlwaysLock=true, setting TargetState to LOCKED.");
                       lockTargetState->setVal(lockStates::LOCKED);
                       if (espConfig::mqttData.lockEnableCustomState) mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions["LOCK"]), 0, false);
                  } else {
                       if(lockCurrentState != nullptr && lockTargetState != nullptr) {
                            int current_state = lockCurrentState->getVal();
                            int new_target = (current_state == lockStates::LOCKED) ? lockStates::UNLOCKED : lockStates::LOCKED;
                            ESP_LOGI(TAG_NFC, "Config toggling state, setting TargetState to %d (opposite of current %d).", new_target, current_state);
                            lockTargetState->setVal(new_target);
                            if (espConfig::mqttData.lockEnableCustomState) {
                                 std::string customAction = (new_target == lockStates::UNLOCKED) ? "UNLOCK" : "LOCK";
                                 mqtt_publish(espConfig::mqttData.lockCustomStateTopic, std::to_string(espConfig::mqttData.customLockActions[customAction]), 0, false);
                            }
                       } else { ESP_LOGE(TAG_NFC, "Cannot toggle state, characteristics invalid!"); }
                  }
                  auto stopTime = std::chrono::high_resolution_clock::now();
                  ESP_LOGI(TAG_NFC, "Total Time (detection->auth->queue): %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
              } else {
                  ESP_LOGW(TAG_NFC, "--- HomeKey Authentication FAILED (AuthAttempted: %d, FlowResult: %d) ---", authAttempted, flowResult);
                  bool failStatus = false;
                  if (espConfig::miscConfig.nfcFailPin != 255) xQueueSend(gpio_led_handle, &failStatus, 0);
                  if (espConfig::miscConfig.nfcNeopixelPin != 255) xQueueSend(neopixel_handle, &failStatus, 0);
              }
              // --- End Process Authentication Result ---

          } // End if (Select Applet Successful)
          // --- Handle Non-HomeKey Tag (if select failed) ---
          else {
              // Select failed (either status false or bad SW1/SW2)
              ESP_LOGW(TAG_NFC, "Select HomeKey Applet failed (Status: %d, SW1: %02x, SW2: %02x). Assuming non-HomeKey tag.",
                       selectStatus,
                       (selectCmdResLength >= 2) ? selectCmdRes[selectCmdResLength - 2] : 0xFF,
                       (selectCmdResLength >= 1) ? selectCmdRes[selectCmdResLength - 1] : 0xFF);

              bool failStatus = false; // Signal failure locally
              if (espConfig::miscConfig.nfcFailPin != 255) xQueueSend(gpio_led_handle, &failStatus, 0);
              if (espConfig::miscConfig.nfcNeopixelPin != 255) xQueueSend(neopixel_handle, &failStatus, 0);

              if (!espConfig::mqttData.nfcTagNoPublish) {
                  // Publish UID etc. to MQTT if configured
                  json payload;
                  payload["atqa"] = hex_representation(std::vector<uint8_t>(atqa, atqa + 2));
                  payload["sak"] = hex_representation(std::vector<uint8_t>(sak, sak + 1));
                  payload["uid"] = hex_representation(std::vector<uint8_t>(uid, uid + uidLen));
                  payload["homekey"] = false;
                  mqtt_publish(espConfig::mqttData.hkTopic, payload.dump(), 0, false);
              } else {
                  ESP_LOGD(TAG_NFC, "Non-HK tag publishing is disabled.");
              }
          }

          // --- Card Removal / Cleanup for this interaction ---
          ESP_LOGD(TAG_NFC, "Processing complete for this target interaction.");
          // Release is implicitly handled by readPassiveTargetID with waitForCardRemoval=true
          nfc->setPassiveActivationRetries(0); // Reset retries for the next polling cycle

      } // End if (passiveTarget)

      // Short delay in the main loop to prevent busy-waiting and yield CPU
      vTaskDelay(pdMS_TO_TICKS(50));

  } // End while(1)

  // --- Cleanup (Should not be reached in normal operation) ---
  ESP_LOGE(TAG_NFC, "nfc_thread_entry exited main while loop unexpectedly!");
  if (nfc) nfc->stop(); // Try to clean up NFC state
  vTaskDelete(NULL); // Delete self
  return; // Should not be reached
}// =========================================================================
// ==            End of NFC Thread Entry Function                         ==
// =========================================================================

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
  readerDataMutex = xSemaphoreCreateMutex();
  LOG(I, "Setting up physical doorbell sense pin %d...", GPIO_DOORBELL_SENSE_PIN);
    pinMode(GPIO_DOORBELL_SENSE_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(GPIO_DOORBELL_SENSE_PIN), handle_doorbell_sense_interrupt, FALLING);
    LOG(I, "Interrupt attached to GPIO %d for doorbell sense (FALLING edge).", GPIO_DOORBELL_SENSE_PIN);

  size_t len;
  const char* TAG = "SETUP";
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData);
  if (readerDataMutex != nullptr) {
    if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(1000)) == pdTRUE) { 
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
        xSemaphoreGive(readerDataMutex); // Release mutex
      } else {
          LOG(E, "Failed to take readerDataMutex during NVS load in setup!");
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
    uint8_t initial_safe_level = espConfig::miscConfig.gpioActionLockState ? HIGH : LOW;

    digitalWrite(espConfig::miscConfig.gpioActionPin, initial_safe_level);
    LOG(I, "Setup: Setting initial safe state for Action Pin %d to %s (Locked Level)",
        espConfig::miscConfig.gpioActionPin, initial_safe_level == HIGH ? "HIGH" : "LOW");
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
    LOG(I, "An Error has occurred while mounting LITTLEFS");
    return;
  }
  listDir(LittleFS, "/", 0);
  LOG(I, "LittleFS used space: %d / %d", LittleFS.usedBytes(), LittleFS.totalBytes());
  if (espConfig::miscConfig.ethernetEnabled) {
    Network.onEvent(onEvent);
    if (espConfig::miscConfig.ethActivePreset != 255) {
      if (espConfig::miscConfig.ethActivePreset >= eth_config_ns::boardPresets.size()) {
        LOG(E, "Invalid preset index, not initializing ethernet!");
      } else {
        eth_board_presets_t ethPreset = eth_config_ns::boardPresets[espConfig::miscConfig.ethActivePreset];
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
    } else if (espConfig::miscConfig.ethActivePreset == 255) {
      eth_chip_desc_t chipType = eth_config_ns::supportedChips[eth_phy_type_t(espConfig::miscConfig.ethPhyType)];
      if (!chipType.emac) {
        ETH.begin(chipType.phy_type, 1, espConfig::miscConfig.ethSpiConfig[1], espConfig::miscConfig.ethSpiConfig[2], espConfig::miscConfig.ethSpiConfig[3], SPI2_HOST, espConfig::miscConfig.ethSpiConfig[4], espConfig::miscConfig.ethSpiConfig[5], espConfig::miscConfig.ethSpiConfig[6], espConfig::miscConfig.ethSpiConfig[0]);
      } else {
#if CONFIG_ETH_USE_ESP32_EMAC
        ETH.begin(chipType.phy_type, espConfig::miscConfig.ethRmiiConfig[0], espConfig::miscConfig.ethRmiiConfig[1], espConfig::miscConfig.ethRmiiConfig[2], espConfig::miscConfig.ethRmiiConfig[3], eth_clock_mode_t(espConfig::miscConfig.ethRmiiConfig[4]));
#endif
      }
    }
  }
  if (espConfig::miscConfig.controlPin != 255) {
    homeSpan.setControlPin(espConfig::miscConfig.controlPin);
  }
  if (espConfig::miscConfig.hsStatusPin != 255) {
    homeSpan.setStatusPin(espConfig::miscConfig.hsStatusPin);
  }
  homeSpan.setStatusAutoOff(15);
  homeSpan.setLogLevel(0);
  homeSpan.setSketchVersion(app_version.c_str());

  if (xSemaphoreTake(readerDataMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
    LOG(I, "READER GROUP ID (%d): %s", readerData.reader_gid.size(), red_log::bufToHexString(readerData.reader_gid.data(), readerData.reader_gid.size()).c_str());
    LOG(I, "READER UNIQUE ID (%d): %s", readerData.reader_id.size(), red_log::bufToHexString(readerData.reader_id.data(), readerData.reader_id.size()).c_str());

    LOG(I, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
    for (auto&& issuer : readerData.issuers) {
      LOG(D, "Issuer ID: %s, Public Key: %s", red_log::bufToHexString(issuer.issuer_id.data(), issuer.issuer_id.size()).c_str(), red_log::bufToHexString(issuer.issuer_pk.data(), issuer.issuer_pk.size()).c_str());
    }
    xSemaphoreGive(readerDataMutex);
  } else {
    LOG(E, "Failed to take readerDataMutex for setup logging!");
  }
  homeSpan.enableAutoStartAP();
  homeSpan.enableOTA(espConfig::miscConfig.otaPasswd.c_str());
  homeSpan.setPortNum(1201);
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);
  char macStr[9] = { 0 };
  sprintf(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
  homeSpan.setHostNameSuffix(macStr);
  homeSpan.begin(Category::Locks, espConfig::miscConfig.deviceName.c_str(), "HK-", "HomeKey-ESP32");

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
  new NFCAccessoryInformation();
  new Service::HAPProtocolInformation();
  new Characteristic::Version();
  new LockManagement();
  new LockMechanism();
  new NFCAccess();
  homekit_doorbell = new DoorbellSensor();
  if (espConfig::miscConfig.proxBatEnabled) {
    new PhysicalLockBattery();
  }
  homeSpan.setControllerCallback(pairCallback);
  homeSpan.setConnectionCallback(wifiCallback);
  if (espConfig::miscConfig.nfcNeopixelPin != 255) {
    pixel = std::make_shared<Pixel>(espConfig::miscConfig.nfcNeopixelPin, pixelTypeMap[espConfig::miscConfig.neoPixelType]);
    xTaskCreate(neopixel_task, "neopixel_task", 4096, NULL, 2, &neopixel_task_handle);
  }
  if (espConfig::miscConfig.nfcSuccessPin != 255 || espConfig::miscConfig.nfcFailPin != 255) {
    xTaskCreate(nfc_gpio_task, "nfc_gpio_task", 4096, NULL, 2, &gpio_led_task_handle);
  }
  if (espConfig::miscConfig.gpioActionPin != 255 || espConfig::miscConfig.hkDumbSwitchMode) {
    xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 2, &gpio_lock_task_handle);
  }
  if (espConfig::miscConfig.hkAltActionInitPin != 255) {
    xTaskCreate(alt_action_task, "alt_action_task", 2048, NULL, 2, &alt_action_task_handle);
  }
  xTaskCreate(nfc_thread_entry, "nfc_task", 8192, NULL, 1, &nfc_poll_task);
}

//////////////////////////////////////

#define GPIO_DOORBELL_SENSE_PIN   10  // Example GPIO, choose an available one

// --- Global state for interrupt safety ---
volatile bool physical_doorbell_pressed_flag = false;
unsigned long last_physical_press_event_time = 0;
const unsigned long SENSE_DEBOUNCE_MS = 500; // milliseconds

// --- Interrupt Service Routine (ISR) ---
void IRAM_ATTR handle_doorbell_sense_interrupt() {
    unsigned long current_time = millis();
    if (digitalRead(GPIO_DOORBELL_SENSE_PIN) == LOW) { // Active LOW (3.3V -> 0V on press)
        if (current_time - last_physical_press_event_time > SENSE_DEBOUNCE_MS) {
            physical_doorbell_pressed_flag = true;
            last_physical_press_event_time = current_time;
        }
    }
}

// --- Function to be called from the main loop to act on the interrupt flag ---
void check_and_notify_doorbell_press() {
    if (physical_doorbell_pressed_flag) {
        LOG(I, "Physical doorbell button was pressed!");

        // === YOUR NOTIFICATION LOGIC GOES HERE ===

        // 1. Trigger HomeKit Event
        if (homekit_doorbell != nullptr) {
            homekit_doorbell->triggerHomeKitEvent();
        } else {
            LOG(E, "homekit_doorbell service not initialized!");
        }

        // 2. Send an MQTT message (optional, if you still want this)
        if (client != nullptr) {
           mqtt_publish("your/doorbell/topic", "pressed", 0, false); // Change topic as needed
        }

        // 3. Other actions (flash LED, etc.)
        // ...

        // =========================================

        // Reset the flag so we don't process the same press again
        physical_doorbell_pressed_flag = false;
    }
}

void loop() {
  homeSpan.poll();
  check_and_notify_doorbell_press();
  vTaskDelay(5);
}
