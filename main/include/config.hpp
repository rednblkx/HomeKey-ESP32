#pragma once
#include "ETH.h"
#include <array>
#include <cstdint>
#include <string>
#include "defaults.h"
#include <nlohmann/json.hpp>

enum HK_COLOR
{
  TAN,
  GOLD,
  SILVER,
  BLACK
};

enum lockStates
{
  UNLOCKED,
  LOCKED,
  JAMMED,
  UNKNOWN,
  UNLOCKING,
  LOCKING
};

enum customLockStates
{
  C_LOCKED = 1,
  C_UNLOCKING = 2,
  C_UNLOCKED = 3,
  C_LOCKING = 4,
  C_JAMMED = 254,
  C_UNKNOWN = 255
};
// Custom Lock Actions to be used in MQTT_CUSTOM_STATE_TOPIC
enum customLockActions
{
  UNLOCK = 1,
  LOCK = 2
};

enum class gpioMomentaryStateStatus : uint8_t
{
  M_DISABLED = 0,
  M_HOME = 1 << 0,
  M_HK = 1 << 1,
  M_HOME_HK = (uint8_t)(M_HOME | M_HK)
};

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
    uint8_t pin_cs = SS;
    uint8_t pin_irq = A4;
    uint8_t pin_rst = A5;
    uint8_t pin_sck = SCK;
    uint8_t pin_miso = MISO;
    uint8_t pin_mosi = MOSI;
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
  extern std::map<eth_phy_type_t, eth_chip_desc_t> supportedChips;
  extern std::vector<eth_board_presets_t> boardPresets;
};

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
    std::map<std::string, int> customLockStates = { {"C_LOCKED", C_LOCKED}, {"C_UNLOCKING", C_UNLOCKING}, {"C_UNLOCKED", C_UNLOCKED}, {"C_LOCKING", C_LOCKING}, {"C_JAMMED", C_JAMMED}, {"C_UNKNOWN", C_UNKNOWN} };
    std::map<std::string, int> customLockActions = { {"UNLOCK", UNLOCK}, {"LOCK", LOCK} };
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(espConfig::mqttConfig_t, mqttBroker, mqttPort, mqttUsername, mqttPassword, mqttClientId, lwtTopic, hkTopic, lockStateTopic,
      lockStateCmd, lockCStateCmd, lockTStateCmd, lockCustomStateTopic, lockCustomStateCmd, lockEnableCustomState, hassMqttDiscoveryEnabled, customLockStates, customLockActions,
      nfcTagNoPublish, btrLvlCmdTopic, hkAltActionTopic)
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
    uint8_t ethPhyType = 0;
    std::array<int8_t, 5> ethRmiiConfig = {0, -1, -1, -1, 0};
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
        ethernetEnabled, ethActivePreset, ethPhyType, ethRmiiConfig,
        ethSpiConfig
    )
  };
}; // namespace espConfig
