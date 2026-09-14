#include "EthernetDriver.hpp"

#include <string>
#include <vector>

#include "esp_log.h"
#include "eth_structs.hpp"
#include "app_event_loop.hpp"
#include "app_events.hpp"
#include "GPIOAllocator.hpp"
#include "Network.h"

namespace {
const char* TAG = "EthernetDriver";

std::atomic<bool> s_started{false};
std::atomic<bool> s_bringUpFailed{false};

/**
 * @brief Handle Arduino ethernet events, update hostname, and republish on the app event bus.
 *
 * ETH_START sets the hostname from the MAC. Every state change is logged and
 * mirrored onto ETH_APP_EVENT so other components (e.g. captive-portal
 * provisioning) can wait for link/IP without polling the driver.
 *
 * @param event The Arduino ethernet event identifier.
 * @param info  Event-specific data; provides the netif descriptor for GOT_IP logging.
 */
void ethEventHandler(arduino_event_id_t event, arduino_event_info_t info) {
  uint8_t mac[6] = { 0, 0, 0, 0, 0, 0 };
  std::string macStr;
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      ESP_LOGI(TAG, "ETH Started");
      ETH.macAddress(mac);
      macStr = fmt::format("ESP32_{:02X}{:02X}{:02X}", mac[0], mac[1], mac[2]);
      ETH.setHostname(macStr.c_str());
      AppEventLoop::publish(ETH_APP_EVENT, ETH_STARTED, nullptr, 0);
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      ESP_LOGI(TAG, "ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP: {
      ESP_LOGI(TAG, "ETH Got IP: '%s'", esp_netif_get_desc(info.got_ip.esp_netif));
      char ip[16];
      esp_ip4addr_ntoa(&info.got_ip.ip_info.ip, ip, sizeof(ip));
      AppEventLoop::publish(ETH_APP_EVENT, ETH_GOT_IP, ip, strlen(ip));
      break;
    }
    case ARDUINO_EVENT_ETH_LOST_IP:
      ESP_LOGI(TAG, "ETH Lost IP");
      AppEventLoop::publish(ETH_APP_EVENT, ETH_LOST_IP, nullptr, 0);
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      ESP_LOGI(TAG, "ETH Disconnected");
      AppEventLoop::publish(ETH_APP_EVENT, ETH_DISCONNECTED, nullptr, 0);
      break;
    case ARDUINO_EVENT_ETH_STOP:
      ESP_LOGI(TAG, "ETH Stopped");
      AppEventLoop::publish(ETH_APP_EVENT, ETH_STOPPED, nullptr, 0);
      break;
    default: break;
  }
}
} // namespace

namespace EthernetDriver {

bool start(const espConfig::misc_config_t& config) {
  if (s_started) {
    return true;
  }
  if (s_bringUpFailed) {
    ESP_LOGW(TAG, "Previous bring-up failed; retrying.");
  }

  if (!config.ethernetEnabled) {
    ESP_LOGI(TAG, "Ethernet is disabled. HomeSpan will manage Wi-Fi.");
    return false;
  }

  ESP_LOGI(TAG, "Ethernet is enabled. Initializing...");

  spi_host_device_t spiHost = SPI2_HOST;
  if (config.ethSpiBus < static_cast<unsigned char>(SPI_HOST_MAX)) {
    spiHost = static_cast<spi_host_device_t>(config.ethSpiBus);
  } else {
    ESP_LOGW(TAG, "ethSpiBus out of range (%u). Defaulting to SPI2_HOST.", static_cast<unsigned>(config.ethSpiBus));
  }

  uint8_t eth_sck = 255;
  uint8_t eth_miso = 255;
  uint8_t eth_mosi = 255;
  uint8_t eth_cs = 255;
  uint8_t eth_irq = 255;
  uint8_t eth_rst = 255;
  bool is_spi_ethernet = false;
  eth_phy_type_t phy_type;

  if (config.ethActivePreset != 255) {
    if (config.ethActivePreset >= eth_config_ns::boardPresets.size()) {
      ESP_LOGE(TAG, "Invalid ethActivePreset index (%d). Not initializing Ethernet.", config.ethActivePreset);
      return false;
    }
    const eth_board_presets_t& ethPreset = eth_config_ns::boardPresets[config.ethActivePreset];
    is_spi_ethernet = !ethPreset.ethChip.emac;
    phy_type = ethPreset.ethChip.phy_type;
    if (is_spi_ethernet) {
      eth_sck = ethPreset.spi_conf.pin_sck;
      eth_miso = ethPreset.spi_conf.pin_miso;
      eth_mosi = ethPreset.spi_conf.pin_mosi;
      eth_cs = ethPreset.spi_conf.pin_cs;
      eth_irq = ethPreset.spi_conf.pin_irq;
      eth_rst = ethPreset.spi_conf.pin_rst;
    }
  } else {
    phy_type = static_cast<eth_phy_type_t>(config.ethPhyType);
    if (eth_config_ns::supportedChips.count(phy_type) == 0) {
      ESP_LOGE(TAG, "Custom phy_type (%d) is not supported.", config.ethPhyType);
      return false;
    }
    const eth_chip_desc_t& chipType = eth_config_ns::supportedChips.at(phy_type);
    is_spi_ethernet = !chipType.emac;
    if (is_spi_ethernet) {
      eth_sck = config.ethSpiConfig[4];
      eth_miso = config.ethSpiConfig[5];
      eth_mosi = config.ethSpiConfig[6];
      eth_cs = config.ethSpiConfig[1];
      eth_irq = config.ethSpiConfig[2];
      eth_rst = config.ethSpiConfig[3];
    }
  }

  if (is_spi_ethernet) {
    static std::vector<GPIOAllocator::GPIOLease> eth_leases;
    eth_leases.clear();
    if (eth_sck == 255 || eth_miso == 255 || eth_mosi == 255 || eth_cs == 255) {
      ESP_LOGE(TAG, "One or more required GPIO Pins for SPI Ethernet are not "
                    "defined, cannot setup Ethernet.");
      return false;
    }

    auto is_nfc_bus_claim = [](const GPIOAllocator::PinStatus& status, GPIOAllocator::PinRole role) {
      return std::any_of(status.holders.begin(), status.holders.end(),
                         [&](const GPIOAllocator::PinHolder& h) {
                           return h.consumer == GPIOAllocator::PinConsumer::Nfc && h.role == role;
                         });
    };

    if (spiHost == SPI2_HOST) {
      auto sck_status = GPIOAllocator::instance().status_of(eth_sck);
      auto miso_status = GPIOAllocator::instance().status_of(eth_miso);
      auto mosi_status = GPIOAllocator::instance().status_of(eth_mosi);
      if (!is_nfc_bus_claim(sck_status, GPIOAllocator::PinRole::SpiSck) ||
          !is_nfc_bus_claim(miso_status, GPIOAllocator::PinRole::SpiMiso) ||
          !is_nfc_bus_claim(mosi_status, GPIOAllocator::PinRole::SpiMosi)) {
        ESP_LOGE(TAG, "Ethernet conflict: When using SPI2, Ethernet must share the exact same SCK/MISO/MOSI pins as NFC.");
        ESP_LOGE(TAG, "Current owners - SCK (%d): %s, MISO (%d): %s, MOSI (%d): %s",
                 eth_sck, GPIOAllocator::instance().owner_of(eth_sck).value_or("free").c_str(),
                 eth_miso, GPIOAllocator::instance().owner_of(eth_miso).value_or("free").c_str(),
                 eth_mosi, GPIOAllocator::instance().owner_of(eth_mosi).value_or("free").c_str());
        return false;
      }
    }

    auto acquire_bus = [&](uint8_t pin, GPIOAllocator::PinRole role, const char* tag) -> bool {
      auto lease = GPIOAllocator::instance().acquire(gpio_num_t(pin), GPIO_MODE_DISABLE, role,
                                                     GPIOAllocator::PinConsumer::Eth, tag);
      if (lease.has_value()) {
        eth_leases.push_back(std::move(lease.value()));
        return true;
      }
      ESP_LOGE(TAG, "Failed to allocate Ethernet bus pin %d for %s.", pin, tag);
      return false;
    };

    if (!acquire_bus(eth_sck, GPIOAllocator::PinRole::SpiSck, spiHost == SPI2_HOST ? "SPI2_SCK_ETH" : "ETH_SCK") ||
        !acquire_bus(eth_miso, GPIOAllocator::PinRole::SpiMiso, spiHost == SPI2_HOST ? "SPI2_MISO_ETH" : "ETH_MISO") ||
        !acquire_bus(eth_mosi, GPIOAllocator::PinRole::SpiMosi, spiHost == SPI2_HOST ? "SPI2_MOSI_ETH" : "ETH_MOSI")) {
      eth_leases.clear();
      return false;
    }
    ESP_LOGI(TAG, "Allocated Ethernet SPI Host pins (SCK: %d, MISO: %d, MOSI: %d)", eth_sck, eth_miso, eth_mosi);

    auto check_and_allocate = [&](uint8_t pin, const char* tag_name, GPIOAllocator::PinRole role, gpio_mode_t mode) -> bool {
      if (pin == 255) return true;
      auto lease = GPIOAllocator::instance().acquire(gpio_num_t(pin), mode, role,
                                                     GPIOAllocator::PinConsumer::Eth, tag_name);
      if (lease.has_value()) {
        eth_leases.push_back(std::move(lease.value()));
        return true;
      }
      ESP_LOGE(TAG, "Failed to allocate Pin %d for %s: %s.", pin, tag_name,
               GPIOAllocator::instance().owner_of(pin).value_or("acquire failed").c_str());
      return false;
    };

    if (!check_and_allocate(eth_cs, "ETH_SPI_CS", GPIOAllocator::PinRole::SpiCs, GPIO_MODE_DISABLE) ||
        !check_and_allocate(eth_irq, "ETH_SPI_IRQ", GPIOAllocator::PinRole::EthIrq, GPIO_MODE_INPUT) ||
        !check_and_allocate(eth_rst, "ETH_SPI_RST", GPIOAllocator::PinRole::EthRst, GPIO_MODE_OUTPUT)) {
      eth_leases.clear();
      return false;
    }
  }

  Network.onEvent(ethEventHandler);

  bool beginOk = false;
  // --- Preset-based Configuration ---
  if (config.ethActivePreset != 255) {
    const eth_board_presets_t& ethPreset = eth_config_ns::boardPresets[config.ethActivePreset];
    ESP_LOGI(TAG, "Initializing with preset: %s", ethPreset.name.c_str());

    if (!ethPreset.ethChip.emac) {
      const auto& spiConf = ethPreset.spi_conf;
      beginOk = ETH.begin(ethPreset.ethChip.phy_type, 1, spiConf.pin_cs, spiConf.pin_irq, spiConf.pin_rst,
                          spiHost, spiConf.pin_sck, spiConf.pin_miso, spiConf.pin_mosi, spiConf.spi_freq_mhz);
    } else {
#if CONFIG_ETH_USE_ESP32_EMAC
      const auto& rmiiConf = ethPreset.rmii_conf;
      beginOk = ETH.begin(ethPreset.ethChip.phy_type, rmiiConf.phy_addr, rmiiConf.pin_mcd, rmiiConf.pin_mdio,
                          rmiiConf.pin_power, rmiiConf.pin_rmii_clock);
#else
      ESP_LOGE(TAG, "Preset requires EMAC, but this board does not have a built-in Ethernet MAC.");
#endif
    }
  }
  // --- Custom Configuration ---
  else {
    ESP_LOGI(TAG, "Initializing with custom pin configuration.");
    const eth_chip_desc_t& chipType = eth_config_ns::supportedChips.at(phy_type);

    if (!chipType.emac) {
      const auto& spiConf = config.ethSpiConfig;
      beginOk = ETH.begin(chipType.phy_type, 1, spiConf[1], spiConf[2], spiConf[3],
                          spiHost, spiConf[4], spiConf[5], spiConf[6], spiConf[0]);
    } else {
#if CONFIG_ETH_USE_ESP32_EMAC
      const auto& rmiiConf = config.ethRmiiConfig;
      beginOk = ETH.begin(chipType.phy_type, rmiiConf[0], rmiiConf[1], rmiiConf[2], rmiiConf[3],
                          static_cast<eth_clock_mode_t>(rmiiConf[4]));
#else
      ESP_LOGE(TAG, "Custom config requires EMAC, but this board does not have a built-in Ethernet MAC.");
#endif
    }
  }

  if (!beginOk) {
    s_bringUpFailed = true;
    ESP_LOGE(TAG, "ETH.begin() failed.");
    return false;
  }

  s_started = true;
  s_bringUpFailed = false;
  return true;
}

} // namespace EthernetDriver
