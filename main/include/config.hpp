#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <array>
#include "defaults.h"
#include "fmt/format.h"
#include "esp_mac.h"

enum HK_COLOR { TAN, GOLD, SILVER, BLACK };
enum class gpioMomentaryStateStatus : uint8_t {
  M_DISABLED = 0,
  M_HOME = 1 << 0,
  M_HK = 1 << 1,
  M_HOME_HK = (uint8_t)(M_HOME | M_HK)
};
struct gpioLockAction {
  enum { HOMEKIT = 1, HOMEKEY = 2, OTHER = 3 };
  uint8_t source;
  uint8_t action;
};

/**
 * @brief Create a platform identifier string derived from the device MAC address.
 *
 * Reads the device Bluetooth MAC address and formats an identifier using bytes 2–5.
 *
 * @return std::string Identifier in the form "ESP_{XX}{XX}{XX}{XX}" where each `XX`
 *         is the Bluetooth MAC byte starting from 2 rendered as two uppercase hexadecimal digits.
 */
static const std::string platform_create_id_string(void) {
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);
  const std::string macStr = fmt::format("ESP_{:02X}{:02X}{:02X}{:02X}", mac[2], mac[3], mac[4], mac[5]);
  return macStr;
}
namespace espConfig
{
  struct mqttConfig_t
  {
    /**
     * @brief Initialize MQTT configuration with defaults and compose topic names.
     *
     * Constructs an mqttConfig_t using a platform-generated client identifier, assigns
     * that identifier to the mqttClientId member, and appends standard MQTT topic
     * suffixes to the identifier to populate the various topic string members.
     */
    mqttConfig_t(){
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
    };
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
    bool nfcTagNoPublish = MQTT_NO_TAG_PUBLISH;
    /* SSL/TLS Settings */
    bool useSSL = MQTT_USE_SSL;
    bool allowInsecure = MQTT_ALLOW_INSECURE;
    std::map<std::string, uint8_t> customLockStates = { {"C_LOCKED", C_LOCKED}, {"C_UNLOCKING", C_UNLOCKING}, {"C_UNLOCKED", C_UNLOCKED}, {"C_LOCKING", C_LOCKING}, {"C_JAMMED", C_JAMMED}, {"C_UNKNOWN", C_UNKNOWN} };
    std::map<std::string, uint8_t> customLockActions = { {"UNLOCK", C_UNLOCK}, {"LOCK", C_LOCK} };
  };

  struct mqtt_ssl_t {
    std::string caCert = MQTT_CA_CERT;
    std::string clientCert = MQTT_CLIENT_CERT;
    std::string clientKey = MQTT_CLIENT_KEY;
  };

  struct misc_config_t
  {
    std::string deviceName = DEVICE_NAME;
    std::string otaPasswd = OTA_PWD;
    uint8_t hk_key_color = HOMEKEY_COLOR;
    std::string setupCode = SETUP_CODE;
    bool lockAlwaysUnlock = HOMEKEY_ALWAYS_UNLOCK;
    bool lockAlwaysLock = HOMEKEY_ALWAYS_LOCK;
    uint8_t controlPin = HS_PIN;
    uint8_t hsStatusPin = HS_STATUS_LED;
    bool webAuthEnabled = WEB_AUTH_ENABLED;
    std::string webUsername = WEB_AUTH_USERNAME;
    std::string webPassword = WEB_AUTH_PASSWORD;
    std::array<uint8_t, 4> nfcGpioPins{SS_PIN, SCK_PIN, MISO_PIN, MOSI_PIN};
    uint8_t btrLowStatusThreshold = BTR_PROX_BAT_LOW_THRESHOLD;
    bool proxBatEnabled = BTR_PROX_BAT_ENABLED;
    bool ethernetEnabled = ETH_ENABLED;
    uint8_t ethActivePreset = ETH_ACTIVE_PRESET;
    uint8_t ethPhyType = ETH_PHY_TYPE;
    std::array<uint8_t, 5> ethRmiiConfig = {ETH_RMII_CONF_PHY_ADDR, ETH_RMII_CONF_MDC_PIN, ETH_RMII_CONF_MDIO_PIN, ETH_RMII_CONF_POWER_PIN, ETH_RMII_CONF_RMII_CLOCK_MODE};
    std::array<uint8_t, 7> ethSpiConfig = {ETH_SPI_CONF_SPI_FREQ_MHZ, ETH_SPI_CONF_PIN_CS, ETH_SPI_CONF_PIN_IRQ, ETH_SPI_CONF_PIN_RST, ETH_SPI_CONF_PIN_SCK, ETH_SPI_CONF_PIN_MISO, ETH_SPI_CONF_PIN_MOSI};
  };
  struct actions_config_t {
    enum colorMap
    {
      R,
      G,
      B
    };
    uint8_t nfcNeopixelPin = NFC_NEOPIXEL_PIN;
    uint8_t neoPixelType = NEOPIXEL_TYPE;
    std::map<colorMap, uint8_t> neopixelSuccessColor = { {R, NEOPIXEL_SUCCESS_R}, {G, NEOPIXEL_SUCCESS_G}, {B, NEOPIXEL_SUCCESS_B} };
    std::map<colorMap, uint8_t> neopixelFailureColor = { {R, NEOPIXEL_FAIL_R}, {G, NEOPIXEL_FAIL_G}, {B, NEOPIXEL_FAIL_B} };
    std::map<colorMap, uint8_t> neopixelTagEventColor = { {R, NEOPIXEL_TAG_EVENT_R}, {G, NEOPIXEL_TAG_EVENT_G}, {B, NEOPIXEL_TAG_EVENT_B} };
    uint16_t neopixelTagEventTime = NEOPIXEL_TAG_EVENT_TIME;
    uint16_t neopixelSuccessTime = NEOPIXEL_SUCCESS_TIME;
    uint16_t neopixelFailTime = NEOPIXEL_FAIL_TIME;
    uint8_t nfcSuccessPin = NFC_SUCCESS_PIN;
    uint16_t nfcSuccessTime = NFC_SUCCESS_TIME;
    bool nfcSuccessHL = NFC_SUCCESS_HL;
    uint8_t nfcFailPin = NFC_FAIL_PIN;
    uint16_t nfcFailTime = NFC_FAIL_TIME;
    bool nfcFailHL = NFC_FAIL_HL;
    uint8_t tagEventPin = TAG_EVENT_PIN;
    uint16_t tagEventTimeout = TAG_EVENT_TIMEOUT;
    bool tagEventHL = TAG_EVENT_HL;
    uint8_t gpioActionPin = GPIO_ACTION_PIN;
    bool gpioActionLockState = GPIO_ACTION_LOCK_STATE;
    bool gpioActionUnlockState = GPIO_ACTION_UNLOCK_STATE;
    uint8_t gpioActionMomentaryEnabled = GPIO_ACTION_MOMENTARY_STATE;
    bool hkGpioControlledState = GPIO_HK_CONTROLLED_STATE;
    uint16_t gpioActionMomentaryTimeout = GPIO_ACTION_MOMENTARY_TIMEOUT;
    bool hkDumbSwitchMode = HK_DUMB_SWITCH_MODE;
    uint8_t hkAltActionPin = GPIO_HK_ALT_ACTION_PIN;
    uint16_t hkAltActionTimeout = GPIO_HK_ALT_ACTION_TIMEOUT;
    uint8_t hkAltActionGpioState = GPIO_HK_ALT_ACTION_GPIO_STATE;
    uint8_t hkAltActionInitPin = GPIO_HK_ALT_ACTION_INIT_PIN;
    uint8_t hkAltActionInitLedPin = GPIO_HK_ALT_ACTION_INIT_LED_PIN;
    uint16_t hkAltActionInitTimeout = GPIO_HK_ALT_ACTION_INIT_TIMEOUT;
  };
} // namespace espConfig
