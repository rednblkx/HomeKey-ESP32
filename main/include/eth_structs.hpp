#pragma once
#include <map>
#include <string>
#include <vector>
#include "defaults.h"
#include "esp_eth.h"
#include "sdkconfig.h"

// Native replacement for the Arduino ETH.h types the old eth_structs.hpp used.
// Arduino's eth_phy_type_t/eth_clock_mode_t mapped 1:1 onto IDF enums; on IDF 6
// the external PHY/SPI drivers live in the espressif/esp-eth-drivers registry
// components and EMAC clock config moved into eth_mac_clock_config_t.

struct eth_chip_desc_t {
  std::string name;
  bool emac;
  int phy_type; // matches esp-eth-drivers chip selection; cast at use sites
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
    int pin_rmii_clock = 0; // old eth_clock_mode_t / EMAC_CLK_EXT_IN default
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

// Old Arduino eth_phy_type_t values (kept for NVS/config compatibility).
enum ArduinoEthPhyType {
  ETH_PHY_LAN8720 = 0,
  ETH_PHY_TLK110 = 1,
  ETH_PHY_RTL8201 = 2,
  ETH_PHY_DP83848 = 3,
  ETH_PHY_KSZ8041 = 4,
  ETH_PHY_KSZ8081 = 5,
  ETH_PHY_DM9051 = 6,
  ETH_PHY_W5500 = 7,
  ETH_PHY_KSZ8851 = 8,
  ETH_PHY_IP101 = 9,
};

// Old Arduino eth_clock_mode_t values.
enum ArduinoEthClockMode {
  ETH_CLOCK_GPIO0_IN = 0,
  ETH_CLOCK_GPIO0_OUT = 1,
  ETH_CLOCK_GPIO16_OUT = 2,
  ETH_CLOCK_GPIO17_OUT = 3,
};

namespace eth_config_ns {
inline const std::map<int, eth_chip_desc_t> supportedChips = {
#if CONFIG_ETH_USE_ESP32_EMAC
    {ETH_PHY_LAN8720, eth_chip_desc_t{"LAN8720", true, ETH_PHY_LAN8720}},
    {ETH_PHY_TLK110, eth_chip_desc_t{"TLK110", true, ETH_PHY_TLK110}},
    {ETH_PHY_RTL8201, eth_chip_desc_t{"RTL8201", true, ETH_PHY_RTL8201}},
    {ETH_PHY_DP83848, eth_chip_desc_t{"DP83848", true, ETH_PHY_DP83848}},
    {ETH_PHY_KSZ8041, eth_chip_desc_t{"KSZ8041", true, ETH_PHY_KSZ8041}},
    {ETH_PHY_KSZ8081, eth_chip_desc_t{"KSZ8081", true, ETH_PHY_KSZ8081}},
#endif
#if CONFIG_ETH_USE_SPI_ETHERNET
    {ETH_PHY_DM9051, eth_chip_desc_t{"DM9051", false, ETH_PHY_DM9051}},
    {ETH_PHY_W5500, eth_chip_desc_t{"W5500", false, ETH_PHY_W5500}},
    {ETH_PHY_KSZ8851, eth_chip_desc_t{"KSZ8851", false, ETH_PHY_KSZ8851}},
#endif
};
/// Build named Ethernet presets for this target.
/// @throws std::out_of_range if a required chip is absent from supportedChips.
inline std::vector<eth_board_presets_t> makeBoardPresets() {
  std::vector<eth_board_presets_t> presets;
  {
    eth_board_presets_t p{};
    p.name = "Generic W5500";
    p.ethChip = supportedChips.at(ETH_PHY_W5500);
    p.spi_conf = {20, SS, A3, A4, SCK, MISO, MOSI};
    presets.push_back(p);
  }
  {
    eth_board_presets_t p{};
    p.name = "T-ETH-Lite-ESP32S3";
    p.ethChip = supportedChips.at(ETH_PHY_W5500);
    p.spi_conf = {20, 9, 13, 14, 10, 11, 12};
    presets.push_back(p);
  }
#if CONFIG_ETH_USE_ESP32_EMAC
  {
    eth_board_presets_t p{};
    p.name = "WT32-ETH01";
    p.ethChip = supportedChips.at(ETH_PHY_LAN8720);
    p.rmii_conf = {1, 23, 18, 16, ETH_CLOCK_GPIO0_IN};
    presets.push_back(p);
  }
  {
    eth_board_presets_t p{};
    p.name = "Olimex ESP32-POE";
    p.ethChip = supportedChips.at(ETH_PHY_LAN8720);
    p.rmii_conf = {0, 23, 18, 12, ETH_CLOCK_GPIO17_OUT};
    presets.push_back(p);
  }
  {
    eth_board_presets_t p{};
    p.name = "EST-PoE-32";
    p.ethChip = supportedChips.at(ETH_PHY_LAN8720);
    p.rmii_conf = {0, 23, 18, 12, ETH_CLOCK_GPIO17_OUT};
    presets.push_back(p);
  }
  {
    eth_board_presets_t p{};
    p.name = "T-ETH-Lite-ESP32";
    p.ethChip = supportedChips.at(ETH_PHY_RTL8201);
    p.rmii_conf = {0, 23, 18, 12, ETH_CLOCK_GPIO0_IN};
    presets.push_back(p);
  }
  {
    eth_board_presets_t p{};
    p.name = "CASmo-NFC-MB-ETH";
    p.ethChip = supportedChips.at(ETH_PHY_LAN8720);
    p.rmii_conf = {0, 23, 18, -1, ETH_CLOCK_GPIO17_OUT};
    presets.push_back(p);
  }
#endif
  return presets;
}
inline const std::vector<eth_board_presets_t> boardPresets = makeBoardPresets();
} // namespace eth_config_ns
