#include "esp_mac.h"
#include <cstdint>
#include <string>
#include "config.hpp"

std::string platform_create_id_string(void) {
  uint8_t mac[6];
  char id_string[13];
  esp_read_mac(mac, ESP_MAC_BT);
  sprintf(id_string, "ESP32_%02x%02X%02X", mac[3], mac[4], mac[5]);
  return std::string(id_string);
}

espConfig::mqttConfig_t::mqttConfig_t() {
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
}

namespace ArduinoJson {
 // hkEndpoint_t
  void convertToJson(const hkEndpoint_t& src, JsonVariant dst) {
    JsonObject obj = dst.to<JsonObject>();
    obj["endpointId"] = src.endpoint_id;
    obj["last_used_at"] = src.last_used_at;
    obj["counter"] = src.counter;
    obj["key_type"] = src.key_type;
    obj["publicKey"] = src.endpoint_pk;
    obj["endpoint_key_x"] = src.endpoint_pk_x;
    obj["persistent_key"] = src.endpoint_prst_k;
  }

  void convertFromJson(JsonVariantConst src, hkEndpoint_t& dst) {
    JsonObjectConst obj = src;
    dst.endpoint_id = obj["endpointId"].as<std::vector<uint8_t>>();
    dst.last_used_at = obj["last_used_at"].isNull() ? 0 : obj["last_used_at"].as<uint8_t>();
    dst.counter = obj["counter"].isNull() ? 0 : obj["counter"].as<uint8_t>();
    dst.key_type = obj["key_type"].isNull() ? 0 : obj["key_type"].as<uint8_t>();
    dst.endpoint_pk = obj["publicKey"].as<std::vector<uint8_t>>();
    dst.endpoint_pk_x = obj["endpoint_key_x"].as<std::vector<uint8_t>>();
    dst.endpoint_prst_k = obj["persistent_key"].as<std::vector<uint8_t>>();
  }

  // hkIssuer_t
  void convertToJson(const hkIssuer_t& src, JsonVariant dst) {
    JsonObject obj = dst.to<JsonObject>();
    obj["issuerId"] = src.issuer_id;
    obj["publicKey"] = src.issuer_pk;
    obj["issuer_key_x"] = src.issuer_pk_x;
    obj["endpoints"] = src.endpoints;
  }

  void convertFromJson(JsonVariantConst src, hkIssuer_t& dst) {
    JsonObjectConst obj = src;
    dst.issuer_id = obj["issuerId"].as<std::vector<uint8_t>>();
    dst.issuer_pk = obj["publicKey"].as<std::vector<uint8_t>>();
    dst.issuer_pk_x = obj["issuer_key_x"].as<std::vector<uint8_t>>();
    dst.endpoints = obj["endpoints"].as<std::vector<hkEndpoint_t>>();
  }

  // readerData_t
  void convertToJson(const readerData_t& src, JsonVariant dst) {
    JsonObject obj = dst.to<JsonObject>();
    obj["reader_private_key"] = src.reader_sk;
    obj["reader_public_key"] = src.reader_pk;
    obj["reader_key_x"] = src.reader_pk_x;
    obj["group_identifier"] = src.reader_gid;
    obj["unique_identifier"] = src.reader_id;
    obj["issuers"] = src.issuers;
  }

  void convertFromJson(JsonVariantConst src, readerData_t& dst) {
    JsonObjectConst obj = src;
    dst.reader_sk = obj["reader_private_key"].as<std::vector<uint8_t>>();
    dst.reader_pk = obj["reader_public_key"].as<std::vector<uint8_t>>();
    dst.reader_pk_x = obj["reader_key_x"].as<std::vector<uint8_t>>();
    dst.reader_gid = obj["group_identifier"].as<std::vector<uint8_t>>();
    dst.reader_id = obj["unique_identifier"].as<std::vector<uint8_t>>();
    dst.issuers = obj["issuers"].as<std::vector<hkIssuer_t>>();
  }

  // eth_chip_desc_t
  void convertToJson(const eth_chip_desc_t& src, JsonVariant dst) {
    JsonObject obj = dst.to<JsonObject>();
    obj["name"] = src.name;
    obj["emac"] = src.emac;
    obj["phy_type"] = src.phy_type;
  }

  void convertFromJson(JsonVariantConst src, eth_chip_desc_t& dst) {
    JsonObjectConst obj = src;
    dst.name = obj["name"].isNull() ? "" : obj["name"].as<const char *>();
    dst.emac = obj["emac"].isNull() ? false : obj["emac"].as<bool>();
    dst.phy_type = obj["phy_type"].as<eth_phy_type_t>();
  }

#if CONFIG_ETH_USE_ESP32_EMAC
  // rmii_conf_t
  void convertToJson(const eth_board_presets_t::rmii_conf_t& src, JsonVariant dst) {
      JsonObject obj = dst.to<JsonObject>();
      obj["phy_addr"] = src.phy_addr;
      obj["pin_mcd"] = src.pin_mcd;
      obj["pin_mdio"] = src.pin_mdio;
      obj["pin_power"] = src.pin_power;
      obj["pin_rmii_clock"] = src.pin_rmii_clock;
  }

  void convertFromJson(JsonVariantConst src, eth_board_presets_t::rmii_conf_t& dst) {
      JsonObjectConst obj = src;
      dst.phy_addr = obj["phy_addr"].isNull() ? 1 : obj["phy_addr"].as<uint8_t>();
      dst.pin_mcd = obj["pin_mcd"].isNull() ? 23 : obj["pin_mcd"].as<uint8_t>();
      dst.pin_mdio = obj["pin_mdio"].isNull() ? 18 : obj["pin_mdio"].as<uint8_t>();
      dst.pin_power = obj["pin_power"].isNull() ? -1 : obj["pin_power"].as<uint8_t>();
      dst.pin_rmii_clock = obj["pin_rmii_clock"].as<eth_clock_mode_t>();
  }
#endif

  // eth_board_presets_t::spi_conf_t
  void convertToJson(const eth_board_presets_t::spi_conf_t& src, JsonVariant dst) {
      JsonObject obj = dst.to<JsonObject>();
      obj["spi_freq_mhz"] = src.spi_freq_mhz;
      obj["pin_cs"] = src.pin_cs;
      obj["pin_irq"] = src.pin_irq;
      obj["pin_rst"] = src.pin_rst;
      obj["pin_sck"] = src.pin_sck;
      obj["pin_miso"] = src.pin_miso;
      obj["pin_mosi"] = src.pin_mosi;
  }

  void convertFromJson(JsonVariantConst src, eth_board_presets_t::spi_conf_t& dst) {
      JsonObjectConst obj = src;
      dst.spi_freq_mhz = obj["spi_freq_mhz"].isNull() ? 20 : obj["spi_freq_mhz"].as<uint8_t>();
      dst.pin_cs = obj["pin_cs"].isNull() ? 0 : obj["pin_cs"].as<uint8_t>();
      dst.pin_irq = obj["pin_irq"].isNull() ? 18 : obj["pin_irq"].as<uint8_t>();
      dst.pin_rst = obj["pin_rst"].isNull() ? 19 : obj["pin_rst"].as<uint8_t>();
      dst.pin_sck = obj["pin_sck"].isNull() ? 23 : obj["pin_sck"].as<uint8_t>();
      dst.pin_miso = obj["pin_miso"].isNull() ? 21 : obj["pin_miso"].as<uint8_t>();
      dst.pin_mosi = obj["pin_mosi"].isNull() ? 22 : obj["pin_mosi"].as<uint8_t>();
  }

  // eth_board_presets_t
  void convertToJson(const eth_board_presets_t& src, JsonVariant dst) {
    JsonObject obj = dst.to<JsonObject>();
    obj["name"] = src.name;
    obj["ethChip"] = src.ethChip;
#if CONFIG_ETH_USE_ESP32_EMAC
    obj["rmii_conf"] = src.rmii_conf;
#endif
    obj["spi_conf"] = src.spi_conf;
  }

  void convertFromJson(JsonVariantConst src, eth_board_presets_t& dst) {
    JsonObjectConst obj = src;
    dst.name = obj["name"].isNull() ? "" : obj["name"].as<const char *>();
    dst.ethChip = obj["ethChip"].as<eth_chip_desc_t>();
#if CONFIG_ETH_USE_ESP32_EMAC
    dst.rmii_conf = obj["rmii_conf"].as<eth_board_presets_t::rmii_conf_t>();
#endif
    dst.spi_conf = obj["spi_conf"].as<eth_board_presets_t::spi_conf_t>();
  }

  // espConfig::mqttConfig_t
  void convertToJson(const espConfig::mqttConfig_t& src, JsonVariant dst) {
    JsonObject obj = dst.to<JsonObject>();
    obj["mqttBroker"] = src.mqttBroker;
    obj["mqttPort"] = src.mqttPort;
    obj["mqttUsername"] = src.mqttUsername;
    obj["mqttPassword"] = src.mqttPassword;
    obj["mqttClientId"] = src.mqttClientId;
    obj["lwtTopic"] = src.lwtTopic;
    obj["hkTopic"] = src.hkTopic;
    obj["lockStateTopic"] = src.lockStateTopic;
    obj["lockStateCmd"] = src.lockStateCmd;
    obj["lockCStateCmd"] = src.lockCStateCmd;
    obj["lockTStateCmd"] = src.lockTStateCmd;
    obj["btrLvlCmdTopic"] = src.btrLvlCmdTopic;
    obj["hkAltActionTopic"] = src.hkAltActionTopic;
    obj["lockCustomStateTopic"] = src.lockCustomStateTopic;
    obj["lockCustomStateCmd"] = src.lockCustomStateCmd;
    obj["lockEnableCustomState"] = src.lockEnableCustomState;
    obj["hassMqttDiscoveryEnabled"] = src.hassMqttDiscoveryEnabled;
    obj["nfcTagNoPublish"] = src.nfcTagNoPublish;
    obj["customLockStates"] = src.customLockStates;
    obj["customLockActions"] = src.customLockActions;
  }

  void convertFromJson(JsonVariantConst src, espConfig::mqttConfig_t& dst) {
    JsonObjectConst obj = src;
    dst.mqttBroker = obj["mqttBroker"].isNull() ? "" : obj["mqttBroker"].as<const char *>();
    dst.mqttPort = obj["mqttPort"].isNull() ? 1883 : obj["mqttPort"].as<uint16_t>();
    dst.mqttUsername = obj["mqttUsername"].isNull() ? "" : obj["mqttUsername"].as<const char *>();
    dst.mqttPassword = obj["mqttPassword"].isNull() ? "" : obj["mqttPassword"].as<const char *>();
    dst.mqttClientId = obj["mqttClientId"].isNull() ? "" : obj["mqttClientId"].as<const char *>();
    dst.lwtTopic = obj["lwtTopic"].isNull() ? "" : obj["lwtTopic"].as<const char *>();
    dst.hkTopic = obj["hkTopic"].isNull() ? "" : obj["hkTopic"].as<const char *>();
    dst.lockStateTopic = obj["lockStateTopic"].isNull() ? "" : obj["lockStateTopic"].as<const char *>();
    dst.lockStateCmd = obj["lockStateCmd"].isNull() ? "" : obj["lockStateCmd"].as<const char *>();
    dst.lockCStateCmd = obj["lockCStateCmd"].isNull() ? "" : obj["lockCStateCmd"].as<const char *>();
    dst.lockTStateCmd = obj["lockTStateCmd"].isNull() ? "" : obj["lockTStateCmd"].as<const char *>();
    dst.btrLvlCmdTopic = obj["btrLvlCmdTopic"].isNull() ? "" : obj["btrLvlCmdTopic"].as<const char *>();
    dst.hkAltActionTopic = obj["hkAltActionTopic"].isNull() ? "" : obj["hkAltActionTopic"].as<const char *>();
    dst.lockCustomStateTopic = obj["lockCustomStateTopic"].isNull() ? "" : obj["lockCustomStateTopic"].as<const char *>();
    dst.lockCustomStateCmd = obj["lockCustomStateCmd"].isNull() ? "" : obj["lockCustomStateCmd"].as<const char *>();
    dst.lockEnableCustomState = obj["lockEnableCustomState"].isNull() ? false : obj["lockEnableCustomState"].as<bool>();
    dst.hassMqttDiscoveryEnabled = obj["hassMqttDiscoveryEnabled"].isNull() ? false : obj["hassMqttDiscoveryEnabled"].as<bool>();
    dst.nfcTagNoPublish = obj["nfcTagNoPublish"].isNull() ? false : obj["nfcTagNoPublish"].as<bool>();
    dst.customLockStates = obj["customLockStates"].as<std::map<std::string, uint8_t>>();
    dst.customLockActions = obj["customLockActions"].as<std::map<std::string, uint8_t>>();
  }

  // std::map<espConfig::misc_config_t::colorMap, int>
  void convertToJson(const std::map<espConfig::misc_config_t::colorMap, int>& src, JsonVariant dst) {
    JsonArray obj = dst.to<JsonArray>();
    for (const auto& pair : src) {
      JsonArray arr = obj.add<JsonArray>();
      arr.add(pair.first);
      arr.add(pair.second);
    }
  }

  void convertFromJson(JsonVariantConst src, std::map<espConfig::misc_config_t::colorMap, int>& dst) {
    dst.clear();
    JsonArrayConst obj = src.as<JsonArrayConst>();
    for (auto v : obj) {
      if(v.is<JsonArrayConst>()){
        switch (v[0].as<uint8_t>()) {
          case espConfig::misc_config_t::R:
            dst[espConfig::misc_config_t::R] = v[1].as<uint8_t>();
          break;
          case espConfig::misc_config_t::G:
            dst[espConfig::misc_config_t::G] = v[1].as<uint8_t>();
          break;
          case espConfig::misc_config_t::B: 
            dst[espConfig::misc_config_t::B] = v[1].as<uint8_t>();
          break;
        }
      }
    }
  }

  // espConfig::misc_config_t
  void convertToJson(const espConfig::misc_config_t& src, JsonVariant dst) {
    JsonObject obj = dst.to<JsonObject>();
  
    // Core Config
    obj["deviceName"] = src.deviceName;
    obj["otaPasswd"] = src.otaPasswd;
    obj["hk_key_color"] = src.hk_key_color;
    obj["setupCode"] = src.setupCode;

    // Lock Behavior
    obj["lockAlwaysUnlock"] = src.lockAlwaysUnlock;
    obj["lockAlwaysLock"] = src.lockAlwaysLock;
    obj["controlPin"] = src.controlPin;
    obj["hsStatusPin"] = src.hsStatusPin;
    obj["hkGpioControlledState"] = src.hkGpioControlledState;
    
    // Neopixel/NFC Status
    obj["nfcNeopixelPin"] = src.nfcNeopixelPin;
    obj["neoPixelType"] = src.neoPixelType;
    obj["neopixelSuccessColor"] = src.neopixelSuccessColor;
    obj["neopixelFailureColor"] = src.neopixelFailureColor;
    obj["neopixelSuccessTime"] = src.neopixelSuccessTime;
    obj["neopixelFailTime"] = src.neopixelFailTime;
    
    // NFC Binary Status Pins
    obj["nfcSuccessPin"] = src.nfcSuccessPin;
    obj["nfcSuccessTime"] = src.nfcSuccessTime;
    obj["nfcSuccessHL"] = src.nfcSuccessHL;
    obj["nfcFailPin"] = src.nfcFailPin;
    obj["nfcFailTime"] = src.nfcFailTime;
    obj["nfcFailHL"] = src.nfcFailHL;

    // GPIO Actions
    obj["gpioActionPin"] = src.gpioActionPin;
    obj["gpioActionLockState"] = src.gpioActionLockState;
    obj["gpioActionUnlockState"] = src.gpioActionUnlockState;
    obj["gpioActionMomentaryEnabled"] = src.gpioActionMomentaryEnabled;
    obj["gpioActionMomentaryTimeout"] = src.gpioActionMomentaryTimeout;
    
    // Web/Auth
    obj["webAuthEnabled"] = src.webAuthEnabled;
    obj["webUsername"] = src.webUsername;
    obj["webPassword"] = src.webPassword;
    obj["nfcGpioPins"] = src.nfcGpioPins; // std::array

    // Misc
    obj["btrLowStatusThreshold"] = src.btrLowStatusThreshold;
    obj["proxBatEnabled"] = src.proxBatEnabled;
    obj["hkDumbSwitchMode"] = src.hkDumbSwitchMode;
    
    // Alt Action
    obj["hkAltActionInitPin"] = src.hkAltActionInitPin;
    obj["hkAltActionInitLedPin"] = src.hkAltActionInitLedPin;
    obj["hkAltActionInitTimeout"] = src.hkAltActionInitTimeout;
    obj["hkAltActionPin"] = src.hkAltActionPin;
    obj["hkAltActionTimeout"] = src.hkAltActionTimeout;
    obj["hkAltActionGpioState"] = src.hkAltActionGpioState;

    // Ethernet
    obj["ethernetEnabled"] = src.ethernetEnabled;
    obj["ethActivePreset"] = src.ethActivePreset;
    obj["ethPhyType"] = src.ethPhyType;
    obj["ethRmiiConfig"] = src.ethRmiiConfig; // std::array
    obj["ethSpiConfig"] = src.ethSpiConfig;   // std::array
  }

  void convertFromJson(JsonVariantConst src, espConfig::misc_config_t& dst) {
    JsonObjectConst obj = src;
    dst.deviceName = obj["deviceName"].isNull() ? "" : obj["deviceName"].as<const char *>();
    dst.otaPasswd = obj["otaPasswd"].isNull() ? "" : obj["otaPasswd"].as<const char *>();
    dst.hk_key_color = obj["hk_key_color"].isNull() ? 0 : obj["hk_key_color"].as<uint8_t>();
    dst.setupCode = obj["setupCode"].isNull() ? "" : obj["setupCode"].as<const char *>();

    // Lock Behavior
    dst.lockAlwaysUnlock = obj["lockAlwaysUnlock"].isNull() ? false : obj["lockAlwaysUnlock"].as<bool>();
    dst.lockAlwaysLock = obj["lockAlwaysLock"].isNull() ? false : obj["lockAlwaysLock"].as<bool>();
    dst.controlPin = obj["controlPin"].isNull() ? 0 : obj["controlPin"].as<uint8_t>();
    dst.hsStatusPin = obj["hsStatusPin"].isNull() ? 0 : obj["hsStatusPin"].as<uint8_t>();
    dst.hkGpioControlledState = obj["hkGpioControlledState"].isNull() ? true : obj["hkGpioControlledState"].as<bool>();

    // Neopixel/NFC Status
    dst.nfcNeopixelPin = obj["nfcNeopixelPin"].isNull() ? 0 : obj["nfcNeopixelPin"].as<uint8_t>();
    dst.neoPixelType = obj["neoPixelType"].isNull() ? 5 : obj["neoPixelType"].as<uint8_t>();
    dst.neopixelSuccessColor = obj["neopixelSuccessColor"].as<std::map<espConfig::misc_config_t::colorMap, int>>();
    dst.neopixelFailureColor = obj["neopixelFailureColor"].as<std::map<espConfig::misc_config_t::colorMap, int>>();
    dst.neopixelSuccessTime = obj["neopixelSuccessTime"].isNull() ? 0 : obj["neopixelSuccessTime"].as<uint8_t>();
    dst.neopixelFailTime = obj["neopixelFailTime"].isNull() ? 0 : obj["neopixelFailTime"].as<uint8_t>();

    // NFC Binary Status Pins
    dst.nfcSuccessPin = obj["nfcSuccessPin"].isNull() ? 0 : obj["nfcSuccessPin"].as<uint8_t>();
    dst.nfcSuccessTime = obj["nfcSuccessTime"].isNull() ? 0 : obj["nfcSuccessTime"].as<uint8_t>();
    dst.nfcSuccessHL = obj["nfcSuccessHL"].isNull() ? false : obj["nfcSuccessHL"].as<bool>();
    dst.nfcFailPin = obj["nfcFailPin"].isNull() ? 0 : obj["nfcFailPin"].as<uint8_t>();
    dst.nfcFailTime = obj["nfcFailTime"].isNull() ? 0 : obj["nfcFailTime"].as<uint8_t>();
    dst.nfcFailHL = obj["nfcFailHL"].isNull() ? false : obj["nfcFailHL"].as<bool>();

    // GPIO Actions
    dst.gpioActionPin = obj["gpioActionPin"].isNull() ? 0 : obj["gpioActionPin"].as<uint8_t>();
    dst.gpioActionLockState = obj["gpioActionLockState"].isNull() ? false : obj["gpioActionLockState"].as<bool>();
    dst.gpioActionUnlockState = obj["gpioActionUnlockState"].isNull() ? false : obj["gpioActionUnlockState"].as<bool>();
    dst.gpioActionMomentaryEnabled = obj["gpioActionMomentaryEnabled"].isNull() ? 0 : obj["gpioActionMomentaryEnabled"].as<uint8_t>();
    dst.gpioActionMomentaryTimeout = obj["gpioActionMomentaryTimeout"].isNull() ? 0 : obj["gpioActionMomentaryTimeout"].as<uint8_t>();

    // Web/Auth
    dst.webAuthEnabled = obj["webAuthEnabled"].isNull() ? false : obj["webAuthEnabled"].as<bool>();
    dst.webUsername = obj["webUsername"].isNull() ? "" : obj["webUsername"].as<const char *>();
    dst.webPassword = obj["webPassword"].isNull() ? "" : obj["webPassword"].as<const char *>();
    dst.nfcGpioPins = obj["nfcGpioPins"].as<std::array<uint8_t, 4>>();

    // Misc
    dst.btrLowStatusThreshold = obj["btrLowStatusThreshold"].isNull() ? 10 : obj["btrLowStatusThreshold"].as<uint8_t>();
    dst.proxBatEnabled = obj["proxBatEnabled"].isNull() ? false : obj["proxBatEnabled"].as<bool>();
    dst.hkDumbSwitchMode = obj["hkDumbSwitchMode"].isNull() ? false : obj["hkDumbSwitchMode"].as<bool>();

    // Alt Action
    dst.hkAltActionInitPin = obj["hkAltActionInitPin"].isNull() ? 0 : obj["hkAltActionInitPin"].as<uint8_t>();
    dst.hkAltActionInitLedPin = obj["hkAltActionInitLedPin"].isNull() ? 0 : obj["hkAltActionInitLedPin"].as<uint8_t>();
    dst.hkAltActionInitTimeout = obj["hkAltActionInitTimeout"].isNull() ? 0 : obj["hkAltActionInitTimeout"].as<uint8_t>();
    dst.hkAltActionPin = obj["hkAltActionPin"].isNull() ? 0 : obj["hkAltActionPin"].as<uint8_t>();
    dst.hkAltActionTimeout = obj["hkAltActionTimeout"].isNull() ? 0 : obj["hkAltActionTimeout"].as<uint8_t>();
    dst.hkAltActionGpioState = obj["hkAltActionGpioState"].isNull() ? 0 : obj["hkAltActionGpioState"].as<uint8_t>();

    // Ethernet
    dst.ethernetEnabled = obj["ethernetEnabled"].isNull() ? false : obj["ethernetEnabled"].as<bool>();
    dst.ethActivePreset = obj["ethActivePreset"].isNull() ? 255 : obj["ethActivePreset"].as<uint8_t>();
    dst.ethPhyType = obj["ethPhyType"].isNull() ? 1 : obj["ethPhyType"].as<uint8_t>();
    dst.ethRmiiConfig = obj["ethRmiiConfig"].as<std::array<int8_t, 5>>();
    dst.ethSpiConfig = obj["ethSpiConfig"].as<std::array<int8_t, 7>>();
  }
}

size_t ByteWriter::write(uint8_t c){
  data.push_back(c);
  return 1;
}

size_t ByteWriter::write(const uint8_t *buf, size_t len){
  data.reserve(len);
  data.insert(data.end(), buf, buf + len);
  return len;
}

ByteReader::ByteReader(const uint8_t *buf, size_t len) : data(buf, buf + len){
  it = data.begin();
}

int ByteReader::read(){
  return *it++;
}

size_t ByteReader::readBytes(char *buf, size_t len){
  int c = 0;
  if(len <= data.size()){
    for (int i = 0; i < len; i++) {
      buf[i] = *it++;
      c++;
    }
  }
  return c;
}
