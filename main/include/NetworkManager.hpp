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
  /// Retain configuration for later startup; configManager must outlive this manager.
  NetworkManager(ConfigManager &configManager);

  /**
   * Initialize networking, preferring Ethernet when enabled. Wait up to 10 seconds
   * for Ethernet IP acquisition, then try saved Wi-Fi credentials asynchronously
   * or enter AP mode if none exist. Checked ESP-IDF initialization errors abort.
   */
  void begin();

  /**
   * Initialize Ethernet from the retained ConfigManager; config is unused.
   * Once initialized, later calls return true without applying config changes.
   * Return false for rejected configuration or driver setup failures; true does
   * not guarantee a link or IP address. Event-handler registration errors abort.
   * Lifecycle changes are mirrored onto ETH_APP_EVENT.
   */
  bool startEthernet(const espConfig::misc_config_t &config);

  /**
   * Save credentials to NVS without connecting, truncating stored SSID/password
   * to 32/64 bytes. NVS open, write, or commit errors abort.
   */
  void saveCredentials(const std::string &ssid, const std::string &password);
  /** Attempt to erase stored credentials and reboot even if NVS operations fail. */
  void eraseCredentialsAndReboot();

  /**
   * Test credentials using initialized Wi-Fi in a mode that supports STA.
   * Wait up to timeoutMs milliseconds for a GOT_IP event. Success leaves STA
   * connected and defers the connected transition until commitConnection();
   * timeout disconnects and returns false. Credentials are not saved and the
   * previous connection is not restored. A configuration error aborts.
   */
  bool testConnect(const std::string &ssid, const std::string &password,
                   int timeoutMs = 15000);

  /**
   * Start the configuration AP in APSTA mode, disable automatic STA retries,
   * and call the connection callback with 0. Checked Wi-Fi errors abort.
   */
  void startApMode();

  /**
   * Keep the (already-established) test STA connection and switch the whole
   * system to the connected state. Called by the web save task after the
   * HTTP response has been delivered; fires callback (1) only for the first uplink.
   */
  void commitConnection();

  /** Disable the softAP if active; this does not stop the captive DNS server. */
  void tearDownAp();

  /** Single-shot connected transition: fires the callback on the first
   *  uplink only; later uplinks are additive and change nothing. */
  void handleUplinkUp();

  /** Ethernet failover hooks: start WiFi when ETH drops; drop WiFi when
   *  ETH (re)connects — Ethernet is the preferred uplink. */
  void onEthDown();
  void onEthUp();

  /** Return whether an uplink has ever been committed; disconnects do not clear it. */
  bool isConnected() const;
  /// Return the saved SSID, or "ETH" if Ethernet is up and no SSID is recorded.
  std::string ssid() const;
  /// Return the associated Wi-Fi AP signal strength in dBm, or 0 if unavailable.
  int rssi() const;
  /// Return a nonzero STA IPv4 address, then Ethernet IPv4, or empty if neither exists.
  std::string ip() const;

  /** AP-mode SSID: "HK_" plus eight uppercase hex digits from the BT MAC. */
  static std::string apSsid();

  /** conn_cb contract: 1 = connected, 0 = AP config mode. */
  void setConnectionCallback(std::function<void(int)> cb) { m_conn_cb = cb; }

private:
  /// Initialize STA networking and Wi-Fi event handlers; checked driver errors abort.
  void initWifi();
  /// Connect using saved credentials; do nothing if loading fails. Checked Wi-Fi errors abort.
  void startSta();
  /// Configure the AP in APSTA mode; dualMode is unused. Checked Wi-Fi errors abort.
  void startAp(bool dualMode);
  /// Signal connection-test success, or announce an uplink when no test is active.
  void handleStaGotIp(esp_event_base_t base, int32_t id, void *data);
  /// Retry STA up to eight times, then enter AP mode, unless STA retries are disabled or Ethernet is up.
  void handleStaDisconnected(esp_event_base_t base, int32_t id, void *data);

  /// Handle Ethernet lifecycle events, publish app events, and apply uplink preference.
  static void ethEventCb(void *arg, esp_event_base_t base, int32_t id, void *data);
  /// Acquire pins and configure Ethernet from saved settings; return false on setup failure.
  /// Checked event-handler registration errors abort; success does not wait for IP acquisition.
  bool initializeEth();
  /// Set the Ethernet hostname from the BT MAC when an Ethernet netif exists.
  void onEthStarted();

  /// Dispatch STA IP and disconnect events to the manager supplied in arg.
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
  static constexpr int kEthUplinkWaitMs = 10000;

  // AP netif is created lazily on first startAp() (see initWifi comment).
  bool m_apNetifCreated = false;

  // Ethernet state (valid only when ethernetEnabled)
  esp_netif_t *m_eth_netif = nullptr;
  esp_eth_handle_t m_eth_handle = nullptr;
  esp_eth_netif_glue_handle_t m_eth_glue = nullptr;
  uint8_t m_eth_mac[6] = {};

  static const char *TAG;
};
