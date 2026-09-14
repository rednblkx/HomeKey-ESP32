#include "NetworkManager.hpp"
#include "ConfigManager.hpp"
#include "app_events.hpp"
#include "app_event_loop.hpp"
#include "GPIOAllocator.hpp"
#include "config.hpp"
#include "eth_structs.hpp"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "fmt/ranges.h"
#include "hal/spi_types.h"
#include "lwip/inet.h"
#include "nvs.h"
#include "nvs_flash.h"

#include <algorithm>
#include <cstring>
#include <esp_eth.h>
#include <esp_event.h>
#include <esp_netif.h>
#if CONFIG_ETH_USE_SPI_ETHERNET
#include "esp_eth_mac_w5500.h"
#include "esp_eth_phy_w5500.h"
#include "esp_eth_mac_dm9051.h"
#include "esp_eth_phy_dm9051.h"
#include "esp_eth_mac_ksz8851snl.h"
#include "esp_eth_phy_ksz8851snl.h"
#endif
#if CONFIG_ETH_USE_ESP32_EMAC
#include "esp_eth_mac_esp.h"
#include "esp_eth_phy_lan87xx.h"
#include "esp_eth_phy_rtl8201.h"
#include "esp_eth_phy_dp83848.h"
#include "esp_eth_phy_ksz80xx.h"
#endif

const char *NetworkManager::TAG = "Network";

namespace {
constexpr const char *NVS_NAMESPACE = "SAVED_DATA";
constexpr const char *WIFI_CREDS_KEY = "WIFICREDS";

// Matches HomeSpan's WIFIDATA blob layout: char ssid[33] + char pwd[65].
struct WifiCreds {
  char ssid[33];
  char pwd[65];
};

bool loadCreds(WifiCreds &out) {
  nvs_handle_t h;
  if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK) return false;
  size_t len = sizeof(out);
  esp_err_t err = nvs_get_blob(h, WIFI_CREDS_KEY, &out, &len);
  nvs_close(h);
  return err == ESP_OK && len == sizeof(out);
}

void storeCreds(const WifiCreds &creds) {
  nvs_handle_t h;
  ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h));
  ESP_ERROR_CHECK(nvs_set_blob(h, WIFI_CREDS_KEY, &creds, sizeof(creds)));
  ESP_ERROR_CHECK(nvs_commit(h));
  nvs_close(h);
}
} // namespace

NetworkManager::NetworkManager(ConfigManager &configManager)
    : m_configManager(configManager) {}

std::string NetworkManager::apSsid() {
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);
  return fmt::format("HK_{:02X}{:02X}{:02X}{:02X}", mac[2], mac[3], mac[4], mac[5]);
}

void NetworkManager::begin() {
  ESP_ERROR_CHECK(esp_netif_init());
  // setup() already creates the default event loop; tolerate either order.
  esp_err_t loop_err = esp_event_loop_create_default();
  if (loop_err != ESP_OK && loop_err != ESP_ERR_INVALID_STATE) {
    ESP_ERROR_CHECK(loop_err);
  }

  const auto &miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();
  if (miscConfig.ethernetEnabled) {
    startEthernet(miscConfig);
  }

  initWifi();

  WifiCreds creds{};
  if (loadCreds(creds) && creds.ssid[0] != '\0') {
    m_staSsid = creds.ssid;
    startSta();
  } else {
    ESP_LOGW(TAG, "No WiFi credentials stored, starting AP config mode");
    startApMode();
  }
}

void NetworkManager::initWifi() {
  m_wifiEvents = xEventGroupCreate();

  // Only the STA netif is created up front. The AP netif is created lazily
  // in startAp() — the access point must never come up just because the
  // driver supports it.
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             &NetworkManager::eventHandler, this));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             &NetworkManager::eventHandler, this));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());
}

void NetworkManager::eventHandler(void *arg, esp_event_base_t base, int32_t id,
                                  void *data) {
  auto *self = static_cast<NetworkManager *>(arg);
  if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
    self->handleStaGotIp(base, id, data);
  } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
    self->handleStaDisconnected(base, id, data);
  }
}

void NetworkManager::startSta() {
  WifiCreds creds{};
  if (!loadCreds(creds)) {
    ESP_LOGE(TAG, "startSta without credentials");
    return;
  }

  wifi_config_t wifi_config = {};
  strncpy(reinterpret_cast<char *>(wifi_config.sta.ssid), creds.ssid,
          sizeof(wifi_config.sta.ssid) - 1);
  strncpy(reinterpret_cast<char *>(wifi_config.sta.password), creds.pwd,
          sizeof(wifi_config.sta.password) - 1);
  // WPA3 must stay on; PMF required for it (matches previous WPA2/WPA3 setup).
  wifi_config.sta.pmf_cfg.capable = true;
  wifi_config.sta.pmf_cfg.required = false;

  m_retryCount = 0;
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  // The driver was already started in initWifi() (WIFI_MODE_NULL); switching
  // mode on a started driver is legal, but calling esp_wifi_start() again is
  // not (ESP_ERR_INVALID_STATE). Kick the connection explicitly — the STA
  // does not auto-connect just because a config is set.
  esp_wifi_connect();
  m_staStarted = true;
}

void NetworkManager::handleStaGotIp(esp_event_base_t, int32_t, void *data) {
  auto *event = static_cast<ip_event_got_ip_t *>(data);
  char ip[16];
  inet_ntoa_r(event->ip_info.ip, ip, sizeof(ip));
  ESP_LOGI(TAG, "Got IP: %s", ip);
  m_anyUp = true;
  m_retryCount = 0;
  if (m_wifiEvents) xEventGroupSetBits(m_wifiEvents, BIT0);
  if (m_testInProgress) return; // commitConnection() fires the callback later
  if (m_conn_cb) m_conn_cb(1);
}

void NetworkManager::commitConnection() {
  m_testInProgress = false;
  m_apMode = false;
  // Drop the AP: the save flow only keeps it alive long enough to deliver
  // the HTTP response over the portal association. Connected operation is
  // STA-only.
  esp_wifi_set_mode(WIFI_MODE_STA);
  if (m_conn_cb) m_conn_cb(1);
}

void NetworkManager::handleStaDisconnected(esp_event_base_t, int32_t, void *) {
  if (!m_staStarted) return;
  if (m_apMode) return; // AP config mode owns the radio now

  int retries = ++m_retryCount;
  if (retries <= kMaxRetry) {
    ESP_LOGW(TAG, "WiFi lost connection, retry %d/%d", retries, kMaxRetry);
    esp_wifi_connect();
  } else {
    ESP_LOGE(TAG, "WiFi connect failed after %d retries, starting AP config mode",
             kMaxRetry);
    startApMode();
  }
}

void NetworkManager::startAp(bool dualMode) {
  const auto &miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();
  const std::string ssid = apSsid();

  // The AP netif must exist before the driver is switched into an AP mode.
  if (!m_apNetifCreated) {
    esp_netif_create_default_wifi_ap();
    m_apNetifCreated = true;
  }

  wifi_config_t ap_config = {};
  strncpy(reinterpret_cast<char *>(ap_config.ap.ssid), ssid.c_str(),
          sizeof(ap_config.ap.ssid) - 1);
  ap_config.ap.ssid_len = ssid.size();
  strncpy(reinterpret_cast<char *>(ap_config.ap.password),
          miscConfig.accessPointPassword.c_str(),
          sizeof(ap_config.ap.password) - 1);
  ap_config.ap.channel = 11;
  ap_config.ap.max_connection = 2;
  ap_config.ap.ssid_hidden = 0;
  ap_config.ap.authmode = WIFI_AUTH_WPA2_WPA3_PSK;
  ap_config.ap.pmf_cfg.capable = true;
  ap_config.ap.pmf_cfg.required = true;

  // Always APSTA: the STA interface exists (idle) so a later testConnect()
  // can associate WITHOUT any mode transition — switching a live AP-only
  // interface to APSTA restarts the AP and kicks every client.
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
  m_apMode = true;
  ESP_LOGI(TAG, "AP config mode started: SSID %s", ssid.c_str());
}

void NetworkManager::startApMode() {
  m_staStarted = false;
  // Switch the radio to AP mode BEFORE firing the callback: the web server
  // checks esp_wifi_get_mode() to decide between the regular UI and the
  // captive-portal routes, so the mode must already be AP when it starts.
  startAp(false);
  if (m_conn_cb) m_conn_cb(0);
}

void NetworkManager::saveCredentials(const std::string &ssid,
                                     const std::string &password) {
  WifiCreds creds{};
  strncpy(creds.ssid, ssid.c_str(), sizeof(creds.ssid) - 1);
  strncpy(creds.pwd, password.c_str(), sizeof(creds.pwd) - 1);
  storeCreds(creds);
  m_staSsid = ssid;
}

void NetworkManager::eraseCredentialsAndReboot() {
  nvs_handle_t h;
  if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h) == ESP_OK) {
    nvs_erase_key(h, WIFI_CREDS_KEY);
    nvs_commit(h);
    nvs_close(h);
  }
  esp_restart();
}

bool NetworkManager::testConnect(const std::string &ssid,
                                 const std::string &password, int timeoutMs) {
  ESP_LOGI(TAG, "Testing connection to SSID: %s", ssid.c_str());

  wifi_config_t wifi_config = {};
  strncpy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid.c_str(),
          sizeof(wifi_config.sta.ssid) - 1);
  strncpy(reinterpret_cast<char *>(wifi_config.sta.password), password.c_str(),
          sizeof(wifi_config.sta.password) - 1);
  wifi_config.sta.pmf_cfg.capable = true;
  wifi_config.sta.pmf_cfg.required = false;

  m_testInProgress = true;

  // The AP is already in APSTA mode (startAp guarantees it), so this is just
  // a STA connect — no mode transition, no AP reconfiguration. The driver
  // moves the softAP to the STA's channel with a CSA announcement when the
  // association completes; portal clients follow it and stay associated.
  xEventGroupClearBits(m_wifiEvents, BIT0);
  esp_wifi_disconnect();
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  esp_wifi_connect();

  EventBits_t bits = xEventGroupWaitBits(m_wifiEvents, BIT0, pdFALSE, pdFALSE,
                                         pdMS_TO_TICKS(timeoutMs));
  bool connected = (bits & BIT0) != 0;

  if (connected) {
    ESP_LOGI(TAG, "Connected to %s, RSSI: %d", ssid.c_str(), rssi());
    // Keep the STA association; commitConnection() switches to STA-only
    // after the HTTP response has been delivered over the (still-up) AP.
    return true;
  }

  ESP_LOGE(TAG, "Connection timeout after %d ms", timeoutMs);
  esp_wifi_disconnect();
  m_testInProgress = false;
  // AP untouched — the portal client never noticed anything.
  return false;
}

bool NetworkManager::isConnected() const { return m_anyUp; }

std::string NetworkManager::ssid() const {
  if (m_ethUp && m_staSsid.empty()) return "ETH";
  return m_staSsid;
}

int NetworkManager::rssi() const {
  wifi_ap_record_t ap;
  if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) return ap.rssi;
  return 0;
}

std::string NetworkManager::ip() const {
  esp_netif_ip_info_t info;
  esp_netif_t *sta = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  if (sta && esp_netif_get_ip_info(sta, &info) == ESP_OK && info.ip.addr != 0) {
    char ip[16];
    inet_ntoa_r(info.ip.addr, ip, sizeof(ip));
    return ip;
  }
  esp_netif_t *eth = esp_netif_get_handle_from_ifkey("ETH_DEF");
  if (eth && esp_netif_get_ip_info(eth, &info) == ESP_OK && info.ip.addr != 0) {
    char ip[16];
    inet_ntoa_r(info.ip.addr, ip, sizeof(ip));
    return ip;
  }
  return "";
}

// ---------------------------------------------------------------------------
// Ethernet (ported 1:1 from HomeKitLock::initializeETH; Arduino ETH -> esp_eth)
// ---------------------------------------------------------------------------

void NetworkManager::ethEventCb(void *arg, esp_event_base_t base, int32_t id,
                                void *data) {
  auto *self = static_cast<NetworkManager *>(arg);
  if (base == ETH_EVENT) {
    switch (id) {
    case ETHERNET_EVENT_CONNECTED:
      ESP_LOGI(TAG, "ETH Connected");
      if (self->m_eth_handle) {
        esp_eth_ioctl(self->m_eth_handle, ETH_CMD_G_MAC_ADDR, self->m_eth_mac);
      }
      esp_netif_set_default_netif(self->m_eth_netif);
      break;
    case ETHERNET_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "ETH Disconnected");
      self->m_ethUp = false;
      AppEventLoop::publish(ETH_APP_EVENT, ETH_DISCONNECTED, nullptr, 0);
      break;
    case ETHERNET_EVENT_START:
      ESP_LOGI(TAG, "ETH Started");
      self->onEthStarted();
      AppEventLoop::publish(ETH_APP_EVENT, ETH_STARTED, nullptr, 0);
      break;
    case ETHERNET_EVENT_STOP:
      ESP_LOGI(TAG, "ETH Stopped");
      AppEventLoop::publish(ETH_APP_EVENT, ETH_STOPPED, nullptr, 0);
      break;
    default:
      break;
    }
  } else if (base == IP_EVENT && id == IP_EVENT_ETH_GOT_IP) {
    auto *event = static_cast<ip_event_got_ip_t *>(data);
    char ip[16];
    inet_ntoa_r(event->ip_info.ip, ip, sizeof(ip));
    ESP_LOGI(TAG, "ETH Got IP: %s", ip);
    self->m_ethUp = true;
    self->m_anyUp = true;
    AppEventLoop::publish(ETH_APP_EVENT, ETH_GOT_IP, ip, strlen(ip));
    if (self->m_conn_cb) self->m_conn_cb(1);
  }
}

void NetworkManager::onEthStarted() {
  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_BT);
  std::string macStr = fmt::format("ESP32_{:02X}{:02X}{:02X}", mac[0], mac[1], mac[2]);
  if (m_eth_netif) esp_netif_set_hostname(m_eth_netif, macStr.c_str());
}

bool NetworkManager::startEthernet(const espConfig::misc_config_t &config) {
  if (m_ethStarted) {
    return true; // already up (or previously started); idempotent
  }
  if (m_ethBringUpFailed) {
    ESP_LOGW(TAG, "Previous Ethernet bring-up failed; retrying.");
  }
  if (!initializeEth()) {
    m_ethBringUpFailed = true;
    return false;
  }
  m_ethStarted = true;
  m_ethBringUpFailed = false;
  return true;
}

bool NetworkManager::initializeEth() {
  const auto &miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();

  spi_host_device_t spiHost = SPI2_HOST;
  if (miscConfig.ethSpiBus < static_cast<unsigned char>(SPI_HOST_MAX)) {
    spiHost = static_cast<spi_host_device_t>(miscConfig.ethSpiBus);
  } else {
    ESP_LOGW(TAG, "ethSpiBus out of range (%u). Defaulting to SPI2_HOST.",
             static_cast<unsigned>(miscConfig.ethSpiBus));
  }

  uint8_t eth_sck = 255;
  uint8_t eth_miso = 255;
  uint8_t eth_mosi = 255;
  uint8_t eth_cs = 255;
  uint8_t eth_irq = 255;
  uint8_t eth_rst = 255;
  bool is_spi_ethernet = false;
  int phy_type = 0;

  if (miscConfig.ethActivePreset != 255) {
    if (miscConfig.ethActivePreset >= eth_config_ns::boardPresets.size()) {
      ESP_LOGE(TAG, "Invalid ethActivePreset index (%d). Not initializing Ethernet.",
               miscConfig.ethActivePreset);
      return false;
    }
    const eth_board_presets_t &ethPreset = eth_config_ns::boardPresets[miscConfig.ethActivePreset];
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
    phy_type = miscConfig.ethPhyType;
    if (eth_config_ns::supportedChips.count(phy_type) == 0) {
      ESP_LOGE(TAG, "Custom phy_type (%d) is not supported.", miscConfig.ethPhyType);
      return false;
    }
    const eth_chip_desc_t &chipType = eth_config_ns::supportedChips.at(phy_type);
    is_spi_ethernet = !chipType.emac;
    if (is_spi_ethernet) {
      eth_sck = miscConfig.ethSpiConfig[4];
      eth_miso = miscConfig.ethSpiConfig[5];
      eth_mosi = miscConfig.ethSpiConfig[6];
      eth_cs = miscConfig.ethSpiConfig[1];
      eth_irq = miscConfig.ethSpiConfig[2];
      eth_rst = miscConfig.ethSpiConfig[3];
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

    auto is_nfc_bus_claim = [](const GPIOAllocator::PinStatus &status,
                               GPIOAllocator::PinRole role) {
      return std::any_of(status.holders.begin(), status.holders.end(),
                         [&](const GPIOAllocator::PinHolder &h) {
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
        ESP_LOGE(TAG, "Ethernet conflict: When using SPI2, Ethernet must share the "
                      "exact same SCK/MISO/MOSI pins as NFC.");
        ESP_LOGE(TAG, "Current owners - SCK (%d): %s, MISO (%d): %s, MOSI (%d): %s",
                 eth_sck, GPIOAllocator::instance().owner_of(eth_sck).value_or("free").c_str(),
                 eth_miso, GPIOAllocator::instance().owner_of(eth_miso).value_or("free").c_str(),
                 eth_mosi, GPIOAllocator::instance().owner_of(eth_mosi).value_or("free").c_str());
        return false;
      }
    }

    auto acquire_bus = [&](uint8_t pin, GPIOAllocator::PinRole role, const char *tag) -> bool {
      auto lease = GPIOAllocator::instance().acquire(gpio_num_t(pin), GPIO_MODE_DISABLE,
                                                     role, GPIOAllocator::PinConsumer::Eth, tag);
      if (lease.has_value()) {
        eth_leases.push_back(std::move(lease.value()));
        return true;
      }
      ESP_LOGE(TAG, "Failed to allocate Ethernet bus pin %d for %s.", pin, tag);
      return false;
    };

    if (!acquire_bus(eth_sck, GPIOAllocator::PinRole::SpiSck,
                     spiHost == SPI2_HOST ? "SPI2_SCK_ETH" : "ETH_SCK") ||
        !acquire_bus(eth_miso, GPIOAllocator::PinRole::SpiMiso,
                     spiHost == SPI2_HOST ? "SPI2_MISO_ETH" : "ETH_MISO") ||
        !acquire_bus(eth_mosi, GPIOAllocator::PinRole::SpiMosi,
                     spiHost == SPI2_HOST ? "SPI2_MOSI_ETH" : "ETH_MOSI")) {
      eth_leases.clear();
      return false;
    }
    ESP_LOGI(TAG, "Allocated Ethernet SPI Host pins (SCK: %d, MISO: %d, MOSI: %d)",
             eth_sck, eth_miso, eth_mosi);

    auto check_and_allocate = [&](uint8_t pin, const char *tag_name,
                                  GPIOAllocator::PinRole role, gpio_mode_t mode) -> bool {
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

  ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID,
                                             &NetworkManager::ethEventCb, this));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP,
                                             &NetworkManager::ethEventCb, this));

  // netif (with default DHCP client) + glue must exist before driver install.
  esp_netif_config_t eth_netif_cfg = ESP_NETIF_DEFAULT_ETH();
  m_eth_netif = esp_netif_new(&eth_netif_cfg);

  eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

  esp_eth_mac_t *mac = nullptr;
  esp_eth_phy_t *phy = nullptr;

  // --- Preset-based Configuration ---
  if (miscConfig.ethActivePreset != 255) {
    const eth_board_presets_t &ethPreset = eth_config_ns::boardPresets[miscConfig.ethActivePreset];
    ESP_LOGI(TAG, "Initializing with preset: %s", ethPreset.name.c_str());

    if (!ethPreset.ethChip.emac) {
#if CONFIG_ETH_USE_SPI_ETHERNET
      const auto &spiConf = ethPreset.spi_conf;
      spi_device_interface_config_t spi_devcfg = {};
      spi_devcfg.mode = 0;
      spi_devcfg.clock_speed_hz = spiConf.spi_freq_mhz * 1000 * 1000;
      spi_devcfg.spics_io_num = spiConf.pin_cs;
      spi_devcfg.queue_size = 20;
      phy_config.reset_gpio_num = spiConf.pin_rst;

      switch (ethPreset.ethChip.phy_type) {
      case ETH_PHY_W5500: {
        eth_w5500_config_t w5500_cfg = ETH_W5500_DEFAULT_CONFIG(spiHost, &spi_devcfg);
        w5500_cfg.base.int_gpio_num = spiConf.pin_irq;
        mac = esp_eth_mac_new_w5500(&w5500_cfg, &mac_config);
        phy = esp_eth_phy_new_w5500(&phy_config);
        break;
      }
      case ETH_PHY_DM9051: {
        eth_dm9051_config_t dm9051_cfg = ETH_DM9051_DEFAULT_CONFIG(spiHost, &spi_devcfg);
        dm9051_cfg.int_gpio_num = spiConf.pin_irq;
        mac = esp_eth_mac_new_dm9051(&dm9051_cfg, &mac_config);
        phy = esp_eth_phy_new_dm9051(&phy_config);
        break;
      }
      case ETH_PHY_KSZ8851: {
        eth_ksz8851snl_config_t ksz_cfg = ETH_KSZ8851SNL_DEFAULT_CONFIG(spiHost, &spi_devcfg);
        ksz_cfg.int_gpio_num = spiConf.pin_irq;
        mac = esp_eth_mac_new_ksz8851snl(&ksz_cfg, &mac_config);
        phy = esp_eth_phy_new_ksz8851snl(&phy_config);
        break;
      }
      default:
        ESP_LOGE(TAG, "Unsupported SPI Ethernet chip type %d.", ethPreset.ethChip.phy_type);
        esp_netif_destroy(m_eth_netif);
        m_eth_netif = nullptr;
        return false;
      }
#else
      ESP_LOGE(TAG, "SPI Ethernet support not compiled in (CONFIG_ETH_USE_SPI_ETHERNET).");
      esp_netif_destroy(m_eth_netif);
      m_eth_netif = nullptr;
      return false;
#endif
    } else {
#if CONFIG_ETH_USE_ESP32_EMAC
      const auto &rmiiConf = ethPreset.rmii_conf;
      eth_esp32_emac_config_t emac_cfg = ETH_ESP32_EMAC_DEFAULT_CONFIG();
      emac_cfg.smi_gpio.mdc_num = rmiiConf.pin_mcd;
      emac_cfg.smi_gpio.mdio_num = rmiiConf.pin_mdio;
      emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN; // placeholder, set below
      // Old Arduino eth_clock_mode_t: 0=GPIO0_IN, 1=GPIO0_OUT, 2=GPIO16_OUT, 3=GPIO17_OUT
      switch (rmiiConf.pin_rmii_clock) {
      case ETH_CLOCK_GPIO0_IN:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN;
        emac_cfg.clock_config.rmii.clock_gpio = 0;
        break;
      case ETH_CLOCK_GPIO0_OUT:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_cfg.clock_config.rmii.clock_gpio = 0;
        break;
      case ETH_CLOCK_GPIO16_OUT:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_cfg.clock_config.rmii.clock_gpio = 16;
        break;
      case ETH_CLOCK_GPIO17_OUT:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_cfg.clock_config.rmii.clock_gpio = 17;
        break;
      default:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN;
        emac_cfg.clock_config.rmii.clock_gpio = 0;
        break;
      }
      phy_config.phy_addr = rmiiConf.phy_addr;
      phy_config.reset_gpio_num = rmiiConf.pin_power >= 0 ? rmiiConf.pin_power : -1;

      mac = esp_eth_mac_new_esp32(&emac_cfg, &mac_config);
      switch (ethPreset.ethChip.phy_type) {
      case ETH_PHY_LAN8720:
        phy = esp_eth_phy_new_lan87xx(&phy_config);
        break;
      case ETH_PHY_TLK110:
        phy = esp_eth_phy_new_lan87xx(&phy_config);
        break;
      case ETH_PHY_RTL8201:
        phy = esp_eth_phy_new_rtl8201(&phy_config);
        break;
      case ETH_PHY_DP83848:
        phy = esp_eth_phy_new_dp83848(&phy_config);
        break;
      case ETH_PHY_KSZ8041:
      case ETH_PHY_KSZ8081:
        phy = esp_eth_phy_new_ksz80xx(&phy_config);
        break;
      default:
        ESP_LOGE(TAG, "Unsupported EMAC PHY type %d.", ethPreset.ethChip.phy_type);
        esp_netif_destroy(m_eth_netif);
        m_eth_netif = nullptr;
        return false;
      }
#else
      ESP_LOGE(TAG, "Preset requires EMAC, but this board does not have a built-in Ethernet MAC.");
      esp_netif_destroy(m_eth_netif);
      m_eth_netif = nullptr;
      return false;
#endif
    }
  }
  // --- Custom Configuration ---
  else {
    ESP_LOGI(TAG, "Initializing with custom pin configuration.");
    auto it = eth_config_ns::supportedChips.find(miscConfig.ethPhyType);
    if (it == eth_config_ns::supportedChips.end()) {
      ESP_LOGE(TAG, "Custom phy_type (%d) is not supported.", miscConfig.ethPhyType);
      esp_netif_destroy(m_eth_netif);
      m_eth_netif = nullptr;
      return false;
    }
    const eth_chip_desc_t &chipType = it->second;

    if (!chipType.emac) {
#if CONFIG_ETH_USE_SPI_ETHERNET
      const auto &spiConf = miscConfig.ethSpiConfig;
      spi_device_interface_config_t spi_devcfg = {};
      spi_devcfg.mode = 0;
      spi_devcfg.clock_speed_hz = spiConf[0] * 1000 * 1000;
      spi_devcfg.spics_io_num = spiConf[1];
      spi_devcfg.queue_size = 20;
      phy_config.reset_gpio_num = spiConf[3] == 255 ? -1 : spiConf[3];

      switch (chipType.phy_type) {
      case ETH_PHY_W5500: {
        eth_w5500_config_t w5500_cfg = ETH_W5500_DEFAULT_CONFIG(spiHost, &spi_devcfg);
        w5500_cfg.base.int_gpio_num = spiConf[2] == 255 ? -1 : spiConf[2];
        mac = esp_eth_mac_new_w5500(&w5500_cfg, &mac_config);
        phy = esp_eth_phy_new_w5500(&phy_config);
        break;
      }
      case ETH_PHY_DM9051: {
        eth_dm9051_config_t dm9051_cfg = ETH_DM9051_DEFAULT_CONFIG(spiHost, &spi_devcfg);
        dm9051_cfg.int_gpio_num = spiConf[2] == 255 ? -1 : spiConf[2];
        mac = esp_eth_mac_new_dm9051(&dm9051_cfg, &mac_config);
        phy = esp_eth_phy_new_dm9051(&phy_config);
        break;
      }
      case ETH_PHY_KSZ8851: {
        eth_ksz8851snl_config_t ksz_cfg = ETH_KSZ8851SNL_DEFAULT_CONFIG(spiHost, &spi_devcfg);
        ksz_cfg.int_gpio_num = spiConf[2] == 255 ? -1 : spiConf[2];
        mac = esp_eth_mac_new_ksz8851snl(&ksz_cfg, &mac_config);
        phy = esp_eth_phy_new_ksz8851snl(&phy_config);
        break;
      }
      default:
        ESP_LOGE(TAG, "Unsupported SPI Ethernet chip type %d.", chipType.phy_type);
        esp_netif_destroy(m_eth_netif);
        m_eth_netif = nullptr;
        return false;
      }
#else
      ESP_LOGE(TAG, "SPI Ethernet support not compiled in (CONFIG_ETH_USE_SPI_ETHERNET).");
      esp_netif_destroy(m_eth_netif);
      m_eth_netif = nullptr;
      return false;
#endif
    } else {
#if CONFIG_ETH_USE_ESP32_EMAC
      const auto &rmiiConf = miscConfig.ethRmiiConfig;
      eth_esp32_emac_config_t emac_cfg = ETH_ESP32_EMAC_DEFAULT_CONFIG();
      emac_cfg.smi_gpio.mdc_num = rmiiConf[1];
      emac_cfg.smi_gpio.mdio_num = rmiiConf[2];
      switch (rmiiConf[4]) {
      case ETH_CLOCK_GPIO0_IN:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN;
        emac_cfg.clock_config.rmii.clock_gpio = 0;
        break;
      case ETH_CLOCK_GPIO0_OUT:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_cfg.clock_config.rmii.clock_gpio = 0;
        break;
      case ETH_CLOCK_GPIO16_OUT:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_cfg.clock_config.rmii.clock_gpio = 16;
        break;
      case ETH_CLOCK_GPIO17_OUT:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_cfg.clock_config.rmii.clock_gpio = 17;
        break;
      default:
        emac_cfg.clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN;
        emac_cfg.clock_config.rmii.clock_gpio = 0;
        break;
      }
      phy_config.phy_addr = rmiiConf[0];
      phy_config.reset_gpio_num = rmiiConf[3] == 255 ? -1 : rmiiConf[3];

      mac = esp_eth_mac_new_esp32(&emac_cfg, &mac_config);
      switch (chipType.phy_type) {
      case ETH_PHY_LAN8720:
      case ETH_PHY_TLK110:
        phy = esp_eth_phy_new_lan87xx(&phy_config);
        break;
      case ETH_PHY_RTL8201:
        phy = esp_eth_phy_new_rtl8201(&phy_config);
        break;
      case ETH_PHY_DP83848:
        phy = esp_eth_phy_new_dp83848(&phy_config);
        break;
      case ETH_PHY_KSZ8041:
      case ETH_PHY_KSZ8081:
        phy = esp_eth_phy_new_ksz80xx(&phy_config);
        break;
      default:
        ESP_LOGE(TAG, "Unsupported EMAC PHY type %d.", chipType.phy_type);
        esp_netif_destroy(m_eth_netif);
        m_eth_netif = nullptr;
        return false;
      }
#else
      ESP_LOGE(TAG, "Custom config requires EMAC, but this board does not have a built-in Ethernet MAC.");
      esp_netif_destroy(m_eth_netif);
      m_eth_netif = nullptr;
      return false;
#endif
    }
  }

  if (!mac || !phy) {
    ESP_LOGE(TAG, "Failed to create Ethernet MAC/PHY instances.");
    esp_netif_destroy(m_eth_netif);
    m_eth_netif = nullptr;
    return false;
  }

  esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
  esp_eth_handle_t eth_handle = nullptr;
  if (esp_eth_driver_install(&eth_config, &eth_handle) != ESP_OK || !eth_handle) {
    ESP_LOGE(TAG, "Ethernet driver install failed.");
    mac->del(mac);
    phy->del(phy);
    esp_netif_destroy(m_eth_netif);
    m_eth_netif = nullptr;
    return false;
  }
  m_eth_handle = eth_handle;

  // Attach the netif glue and start.
  m_eth_glue = esp_eth_new_netif_glue(eth_handle);
  if (esp_netif_attach(m_eth_netif, m_eth_glue) != ESP_OK) {
    ESP_LOGE(TAG, "esp_netif_attach failed.");
    esp_eth_del_netif_glue(m_eth_glue);
    m_eth_glue = nullptr;
    esp_eth_driver_uninstall(m_eth_handle);
    m_eth_handle = nullptr;
    esp_netif_destroy(m_eth_netif);
    m_eth_netif = nullptr;
    return false;
  }
  esp_eth_start(eth_handle);
  return true;
}
