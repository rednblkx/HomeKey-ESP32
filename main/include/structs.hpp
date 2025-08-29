#pragma once
#include <map>
#include <string>
#include <array>
#include "defaults.h"
#include "ETH.h"

enum HK_COLOR { TAN, GOLD, SILVER, BLACK };
enum class gpioMomentaryStateStatus : uint8_t {
  M_DISABLED = 0,
  M_HOME = 1 << 0,
  M_HK = 1 << 1,
  M_HOME_HK = (uint8_t)(M_HOME | M_HK)
};

struct eth_chip_desc_t {
  std::string name;
  bool emac;
  eth_phy_type_t phy_type;
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
  } rmii_conf;
#endif
  struct spi_conf_t {
    uint8_t spi_freq_mhz = 20;
    uint8_t pin_cs = SS;
    uint8_t pin_irq = A4;
    uint8_t pin_rst = A5;
    uint8_t pin_sck = SCK;
    uint8_t pin_miso = MISO;
    uint8_t pin_mosi = MOSI;
  } spi_conf;
};

namespace eth_config_ns {
extern std::map<eth_phy_type_t, eth_chip_desc_t> supportedChips;
extern std::vector<eth_board_presets_t> boardPresets;
}; // namespace eth_config_ns
namespace espConfig
{
  struct mqttConfig_t
  {
    mqttConfig_t();
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
    std::map<std::string, uint8_t> customLockStates = { {"C_LOCKED", C_LOCKED}, {"C_UNLOCKING", C_UNLOCKING}, {"C_UNLOCKED", C_UNLOCKED}, {"C_LOCKING", C_LOCKING}, {"C_JAMMED", C_JAMMED}, {"C_UNKNOWN", C_UNKNOWN} };
    std::map<std::string, uint8_t> customLockActions = { {"UNLOCK", C_UNLOCK}, {"LOCK", C_LOCK} };
  };

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
    uint8_t ethPhyType = 1;
    std::array<int8_t, 5> ethRmiiConfig = {0, -1, -1, -1, 0};
    std::array<int8_t, 7> ethSpiConfig = {20, -1, -1, -1, -1, -1, -1};
  };
} // namespace espConfig
