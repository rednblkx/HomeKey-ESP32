#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <array>
#include <cstdint>

#include "esp_eth.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "hal/spi_types.h"

class ConfigManager;
namespace espConfig {
struct misc_config_t;
}

/**
 * @brief Native ESP-IDF replacement for every Arduino networking API the app used.
 *
 * Owns WiFi STA + softAP lifecycle, Ethernet bring-up, WiFi credential storage in
 * NVS (namespace SAVED_DATA, key WIFICREDS), and connection status reporting.
 * The status callback contract matches the old HomeSpan connection callback:
 * 1 = station/GOT_IP, 0 = AP configuration mode.
 */
class NetworkManager {
public:
  NetworkManager(ConfigManager &configManager);

  /** Initialize netif/eventloop/wifi, start STA if credentials exist, else AP mode. */
  void begin();

  /**
   * Bring up Ethernet per the given config. Idempotent: a later call with
   * the same config is a no-op that reports success, so the captive-portal
   * flow can start the driver to verify the module and the normal boot path
   * will not re-initialize it. Lifecycle changes are mirrored onto
   * ETH_APP_EVENT (ETH_STARTED / ETH_GOT_IP / ...) for other components.
   */
  bool startEthernet(const espConfig::misc_config_t &config);

  /** Save credentials to NVS (does not connect; caller decides next step). */
  void saveCredentials(const std::string &ssid, const std::string &password);
  /** Erase stored credentials and reboot (old HomeSpan "X" behavior). */
  void eraseCredentialsAndReboot();

  /** Blocking connect attempt used by the web UI test-then-save flow. */
  bool testConnect(const std::string &ssid, const std::string &password,
                   int timeoutMs = 15000);

  /** Switch to AP-only configuration mode (old HomeSpan "A" / autoStartAP). */
  void startApMode();

  /**
   * Keep the (already-established) test STA connection and switch the whole
   * system to the connected state. Called by the web save task after the
   * HTTP response has been delivered; fires the connection callback (1).
   */
  void commitConnection();

  /** Status queries for the web UI (old WiFi.RSSI()/SSID()/isConnected()). */
  bool isConnected() const;
  std::string ssid() const;
  int rssi() const;
  std::string ip() const;

  /** AP-mode SSID derived from the BT MAC ("HK_XXXX"). */
  static std::string apSsid();

  /** conn_cb contract: 1 = connected, 0 = AP config mode. */
  void setConnectionCallback(std::function<void(int)> cb) { m_conn_cb = cb; }

private:
  void initWifi();
  void startSta();
  void startAp(bool dualMode);
  void handleStaGotIp(esp_event_base_t base, int32_t id, void *data);
  void handleStaDisconnected(esp_event_base_t base, int32_t id, void *data);

  static void ethEventCb(void *arg, esp_event_base_t base, int32_t id, void *data);
  bool initializeEth();
  void onEthStarted();

  static void eventHandler(void *arg, esp_event_base_t base, int32_t id, void *data);

  ConfigManager &m_configManager;
  std::function<void(int)> m_conn_cb;
  EventGroupHandle_t m_wifiEvents = nullptr;
  std::atomic<bool> m_staStarted{false};
  std::atomic<bool> m_apMode{false};
  std::atomic<bool> m_ethUp{false};
  std::atomic<bool> m_ethStarted{false};
  std::atomic<bool> m_ethBringUpFailed{false};
  std::atomic<bool> m_anyUp{false};
  std::atomic<int> m_retryCount{0};
  // True while testConnect() owns the radio; suppresses conn_cb(1) on the
  // transient test GOT_IP so the connected flow starts only on commitConnection().
  std::atomic<bool> m_testInProgress{false};
  std::string m_staSsid;
  static constexpr int kMaxRetry = 8;

  // AP netif is created lazily on first startAp() (see initWifi comment).
  bool m_apNetifCreated = false;

  // Ethernet state (valid only when ethernetEnabled)
  esp_netif_t *m_eth_netif = nullptr;
  esp_eth_handle_t m_eth_handle = nullptr;
  esp_eth_netif_glue_handle_t m_eth_glue = nullptr;
  uint8_t m_eth_mac[6] = {};

  static const char *TAG;
};
