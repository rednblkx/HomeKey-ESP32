#pragma once
#include <map>
#include <string>
#include "defaults.h"
#include "ETH.h"

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
  static const std::map<eth_phy_type_t, eth_chip_desc_t> supportedChips = {
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
  static const std::vector<eth_board_presets_t> boardPresets = {
      eth_board_presets_t{.name = "Generic W5500",
                          .ethChip = supportedChips.at(ETH_PHY_W5500),
                          .spi_conf{20, SS, A3, A4, SCK, MISO, MOSI}},
      eth_board_presets_t{.name = "T-ETH-Lite-ESP32S3",
                          .ethChip = supportedChips.at(ETH_PHY_W5500),
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
}; // namespace eth_config_ns
