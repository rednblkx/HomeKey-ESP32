// ============================================================================
// WebServerManager.cpp - ESP32 Web Server Implementation
// ============================================================================

#include "GPIOAllocator.hpp"
#include "esp_https_server.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "app_event_loop.hpp"
#include "app_events.hpp"
#include "EthernetDriver.hpp"
#include "fmt/ranges.h"
#include "WebServerManager.hpp"
#include "ConfigManager.hpp"
#include "HomeSpan.h"
#include "MqttManager.hpp"
#include "NfcManager.hpp"
#include "ReaderDataManager.hpp"
#include "cJSON.h"
#include "config.hpp"
#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_log_level.h"
#include "esp_wifi.h"
#include "eth_structs.hpp"
#include "eventStructs.hpp"
#include "freertos/idf_additions.h"
#include "loggable.hpp"
#include "portmacro.h"
#include "sodium/randombytes.h"
#include <LittleFS.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <esp_app_desc.h>
#include <mutex>
#include <esp_tls_crypto.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include "JsonGuard.hpp"

// ============================================================================
// Constants
// ============================================================================

const char *WebServerManager::TAG = "WebServerManager";
const size_t MAX_WS_PAYLOAD = 1024;
const size_t HEAP_UPPER_THRESHOLD = 70 * 1000;
const size_t HEAP_LOWER_THRESHOLD = 50 * 1000;

// ============================================================================
// Helper Functions
// ============================================================================

static inline bool str_ends_with(const char *str, const char *suffix) {
  if (!str || !suffix)
    return false;
  size_t lenstr = strlen(str), lensuf = strlen(suffix);
  return lenstr >= lensuf && memcmp(str + lenstr - lensuf, suffix, lensuf) == 0;
}

inline std::optional<std::string> check_pin_reassignment(uint8_t incoming_pin,
                                                         uint8_t current_pin,
                                                         const std::string& key,
                                                         int array_index,
                                                         bool override_strapping,
                                                         uint8_t nfcReaderType) {
    if (incoming_pin == current_pin || incoming_pin == 255) return std::nullopt;

    GPIOAllocator::PinRole role = GPIOAllocator::PinRole::GpioOut;
    GPIOAllocator::PinConsumer consumer = GPIOAllocator::PinConsumer::Hardware;
    GPIOAllocator::PinConsumer ignore_consumer = GPIOAllocator::PinConsumer::None;
    bool output_capable = true;
    if (key == "nfcGpioPins") {
      consumer = GPIOAllocator::PinConsumer::Nfc;
      ignore_consumer = GPIOAllocator::PinConsumer::Nfc;
      if (nfcReaderType == 2) { // ST25R3916: I2C bus
        switch (array_index) {
          case 0: role = GPIOAllocator::PinRole::I2cSda; break;
          case 1: role = GPIOAllocator::PinRole::I2cScl; break;
        }
      } else {
        switch (array_index) {
          case 0: role = GPIOAllocator::PinRole::SpiCs;   break; // SS / SDA
          case 1: role = GPIOAllocator::PinRole::SpiSck;  break; // SCK / SCL
          case 2: role = GPIOAllocator::PinRole::SpiMiso; break;
          case 3: role = GPIOAllocator::PinRole::SpiMosi; break;
        }
      }
    } else if (key == "nfcIrqPin") {
      consumer = GPIOAllocator::PinConsumer::Nfc;
      ignore_consumer = GPIOAllocator::PinConsumer::Nfc;
      role = GPIOAllocator::PinRole::NfcIrq;
    } else if (key == "nfcVenPin") {
      consumer = GPIOAllocator::PinConsumer::Nfc;
      ignore_consumer = GPIOAllocator::PinConsumer::Nfc;
      role = GPIOAllocator::PinRole::NfcVen;
    } else if (key == "ethSpiConfig") {
      consumer = GPIOAllocator::PinConsumer::Eth;
      switch (array_index) {
        case 1: role = GPIOAllocator::PinRole::SpiCs;   break; // CS
        case 2: role = GPIOAllocator::PinRole::EthIrq;  break; // IRQ
        case 3: role = GPIOAllocator::PinRole::EthRst;  break; // RST
        case 4: role = GPIOAllocator::PinRole::SpiSck;  break; // SCK
        case 5: role = GPIOAllocator::PinRole::SpiMiso; break;
        case 6: role = GPIOAllocator::PinRole::SpiMosi; break;
      }
    } else if (key == "controlPin") {
      consumer = GPIOAllocator::PinConsumer::HomeKit;
      role = GPIOAllocator::PinRole::GpioIn;
      output_capable = false;
    } else if (key == "hsStatusPin") {
      consumer = GPIOAllocator::PinConsumer::HomeKit;
      role = GPIOAllocator::PinRole::Led;
    } else if (key == "nfcSuccessPin" || key == "nfcFailPin" ||
               key == "tagEventPin" || key == "hkAltActionInitLedPin") {
      consumer = GPIOAllocator::PinConsumer::Hardware;
      role = GPIOAllocator::PinRole::Led;
    } else if (key == "hkAltActionInitPin") {
      consumer = GPIOAllocator::PinConsumer::Hardware;
      role = GPIOAllocator::PinRole::Irq;
      output_capable = false;
    }

    auto status = GPIOAllocator::instance().status_of(incoming_pin);
    if (status.strapping && status.holders.empty()) {
      if (override_strapping) return std::nullopt;
      return std::string("is a strapping pin and strapping override is disabled");
    }
    auto verdict = GPIOAllocator::instance().validate(
        gpio_num_t(incoming_pin),
        output_capable ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT,
        role, consumer, ignore_consumer);
    if (verdict) return std::nullopt;
    return std::string(GPIOAllocator::error_str(verdict.error())) +
           " (currently held by: " +
           GPIOAllocator::instance().owner_of(incoming_pin).value_or("unknown") + ")";
}

// ============================================================================
// Constructor & Destructor
/**
 * @brief Construct a WebServerManager and initialize internal references and defaults.
 *
 * Stores references to the provided ConfigManager and ReaderDataManager, initializes
 * the internal HTTP server pointer and MQTT manager pointer to null.
 *
 * @param configManager Reference to the configuration manager used for reading and saving settings.
 * @param readerDataManager Reference to the reader data manager used for accessing reader-related state.
 */

WebServerManager::WebServerManager(ConfigManager &configManager,
                                   NvsCredentialStore &readerDataManager)
    : m_server(nullptr), m_configManager(configManager),
      m_readerDataManager(readerDataManager), m_mqttManager(nullptr), m_nfcManager(nullptr) {
}

/**
 * @brief Clean up WebServerManager resources on destruction.
 *
 * Performs orderly shutdown of server-related subsystems and frees associated resources.
 *
 * @details Calls the OTA cleanup routine, stops the HTTP server if it is running, and
 * stops and deletes the periodic status timer.
 */
WebServerManager::~WebServerManager() {
  ESP_LOGI(TAG, "WebServerManager destructor called");

  if (m_server) {
    httpd_stop(m_server);
    m_server = nullptr;
  }
  if (m_statusTimer) {
    esp_timer_stop(m_statusTimer);
    esp_timer_delete(m_statusTimer);
    m_statusTimer = nullptr;
  }
}

bool WebServerManager::shouldEnableHttps() const {
    const auto& miscConfig = m_configManager.getConfig<espConfig::misc_config_t>();
    if (!miscConfig.webHttpsEnabled) {
        return false;
    }

    bool isMqttSslEnabled = m_configManager.getConfig<espConfig::mqttConfig_t>().useSSL;

    size_t freeHeap = esp_get_free_heap_size();

    if (isMqttSslEnabled && freeHeap < HEAP_UPPER_THRESHOLD) {
        ESP_LOGW(TAG, "HTTPS Web UI degraded to HTTP: Free heap (%zu bytes) insufficient for both HTTPS and MQTT SSL.", freeHeap);
        return false;
    }

    if (freeHeap < HEAP_LOWER_THRESHOLD) {
        ESP_LOGW(TAG, "HTTPS Web UI degraded to HTTP: Free heap (%zu bytes) below minimum threshold.", freeHeap);
        return false;
    }

    return true;
}

esp_err_t WebServerManager::sendJsonError(httpd_req_t *req, const std::string &msg,
                                      const char *status) {
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_status(req, status);
  std::string response = JsonBuilder::object()
      .addBool("success", false)
      .addString("error", msg.c_str())
      .toStringUnformatted();
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

std::string ownerConflictMsg(int pin, const std::string &key, const std::string &owner) {
  return std::to_string(pin) + " for \"" + key + "\" already owned by \"" + owner + "\".";
}

bool WebServerManager::heapGuardOk(httpd_req_t *req, bool otherActive,
                                    const char *thisName, const char *otherName) {
  size_t freeHeap = esp_get_free_heap_size();
  if (otherActive && freeHeap < HEAP_UPPER_THRESHOLD) {
    sendJsonError(req, std::string(thisName) + " cannot be enabled while " + otherName + " is active (low RAM).");
    return false;
  }
  if (freeHeap < HEAP_LOWER_THRESHOLD) {
    sendJsonError(req, std::string(thisName) + " cannot be enabled due to insufficient free heap memory.");
    return false;
  }
  return true;
}

// ============================================================================
// Initialization
/**
 * @brief Initialize and start the web server, WebSocket subsystem, and related resources.
 *
 * Initializes a new session identifier, mounts LittleFS, configures and starts the HTTP server,
 * creates the WebSocket send queue and worker task, registers HTTP/WebSocket routes, and
 * creates a periodic status timer. On failure of critical steps the method logs the error
 * and aborts initialization (server, queue, or task pointers may remain null).
 */

void WebServerManager::begin() {
  ESP_LOGI(TAG, "Initializing...");

  std::vector<uint8_t> sessionIdBytes(32);
  randombytes_buf(sessionIdBytes.data(), sessionIdBytes.size());
  m_sessionId = fmt::format("{:02x}", fmt::join(sessionIdBytes, ""));

  if (!LittleFS.begin()) {
    ESP_LOGW(TAG, "Failed to mount LittleFS");
  } else{
    ESP_LOGI(TAG, "LittleFS mounted: %d/%d bytes", LittleFS.usedBytes(),
            LittleFS.totalBytes());
  }
  wifi_mode_t currentMode;
  esp_err_t wifiErr = esp_wifi_get_mode(&currentMode);
  bool isApMode = (wifiErr == ESP_OK && (currentMode == WIFI_MODE_AP || currentMode == WIFI_MODE_APSTA));
  bool isHttpsActive = !isApMode && shouldEnableHttps();

  httpd_ssl_config_t ssl_config = HTTPD_SSL_CONFIG_DEFAULT();
  ssl_config.httpd.max_uri_handlers = 22;
  ssl_config.httpd.max_open_sockets = 4;
  ssl_config.httpd.stack_size = 8192;
  ssl_config.httpd.uri_match_fn = httpd_uri_match_wildcard;
  ssl_config.httpd.lru_purge_enable = true;
  ssl_config.httpd.backlog_conn = 4;

  if (!isHttpsActive) {
    ssl_config.transport_mode = HTTPD_SSL_TRANSPORT_INSECURE;
    ESP_LOGI(TAG, "Starting Web Server in HTTP mode");
  } else {
    ESP_LOGI(TAG, "Starting Web Server in HTTPS mode");
  }

  if (isHttpsActive) {
    const auto& httpsCerts = m_configManager.getHttpsCertsConfig();
    if (!httpsCerts.serverCert.empty() && !httpsCerts.privateKey.empty()) {
      ssl_config.servercert = reinterpret_cast<const uint8_t *>(httpsCerts.serverCert.c_str());
      ssl_config.servercert_len = httpsCerts.serverCert.length();
      ssl_config.prvtkey_pem = reinterpret_cast<const uint8_t *>(httpsCerts.privateKey.c_str());
      ssl_config.prvtkey_len = httpsCerts.privateKey.length();
      if (!httpsCerts.caCert.empty()) {
        ssl_config.cacert_len = httpsCerts.caCert.length();
        ssl_config.cacert_pem = reinterpret_cast<const uint8_t *>(httpsCerts.caCert.c_str());
      }
      ESP_LOGI(TAG, "Loaded user HTTPS certificates (%d bytes cert, %d bytes key)",
              httpsCerts.serverCert.length(), httpsCerts.privateKey.length());
    } else ESP_LOGI(TAG, "No user HTTPS certificates found");
  }

  if (httpd_ssl_start(&m_server, &ssl_config) == ESP_OK) {
    ESP_LOGI(TAG, "HTTP server started, free heap: %zu", esp_get_free_heap_size());
  } else {
    ESP_LOGE(TAG, "Failed to start HTTP server");
    ssl_config.transport_mode = HTTPD_SSL_TRANSPORT_INSECURE;
    if (httpd_ssl_start(&m_server, &ssl_config) == ESP_OK) {
      ESP_LOGI(TAG, "HTTP server started (INSECURE)");
    } else {
      ESP_LOGE(TAG, "Failed to start HTTP server in INSECURE mode as well!");
      return;
    }
  }
  m_wsQueue = xQueueCreate(128, sizeof(WsFrame *));
  if (!m_wsQueue) {
    ESP_LOGE(TAG, "Failed to create WebSocket queue");
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }
BaseType_t task;
#ifndef CONFIG_FREERTOS_UNICORE
    task = xTaskCreatePinnedToCore(ws_send_task, "ws_send_task", 4096, this, 2,
                      &m_wsTaskHandle, 1);
#else
    task = xTaskCreate(ws_send_task, "ws_send_task", 4096, this, 2,
                      &m_wsTaskHandle);
#endif
  if (task != pdPASS) {
    ESP_LOGE(TAG, "Failed to create WebSocket task");
    vQueueDelete(m_wsQueue);
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }

  if (isApMode) {
    setupCaptivePortalRoutes();
  } else {
    setupRoutes();
  }

  esp_timer_create_args_t timerArgs = {
      .callback = &statusTimerCallback, .arg = this, .name = "statusTimer"};
  if (esp_timer_create(&timerArgs, &m_statusTimer) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to create status timer");
  }

  ESP_LOGI(TAG, "Web server initialization complete");

  m_isInitialized = true;
}

/**
 * @brief Stops the web server and cleans up all resources.
 *
 * Performs a complete shutdown of the web server by stopping the HTTP server,
 * deleting the WebSocket task and queue, and stopping/deleting the status timer.
 */
void WebServerManager::end() {
  ESP_LOGI(TAG, "Ending WebServerManager...");

  if(!m_isInitialized) return;

  if (m_server) {
    httpd_ssl_stop(m_server);
    ESP_LOGI(TAG, "HTTP Server stopped!");
    m_server = nullptr;
  }

  if (m_wsTaskHandle) {
    WsFrame *sentinel = new WsFrame{};
    sentinel->fd = -1;
    if (xQueueSend(m_wsQueue, &sentinel, pdMS_TO_TICKS(100)) == pdTRUE) {
      if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) == 0) {
        ESP_LOGW(TAG, "ws_send_task did not exit in time; forcing deletion");
        vTaskDelete(m_wsTaskHandle);
      }
    } else {
      ESP_LOGW(TAG, "Failed to enqueue shutdown sentinel; forcing deletion");
      vTaskDelete(m_wsTaskHandle);
    }
    m_wsTaskHandle = nullptr;
  }

  if (m_wsQueue) {
    WsFrame* frame = nullptr;
    while (xQueueReceive(m_wsQueue, &frame, 0) == pdPASS) {
      delete frame; // payload lifetime is managed by sharedData/inline buffer
    }
    vQueueDelete(m_wsQueue);
    m_wsQueue = nullptr;
  }

  if (m_statusTimer) {
    esp_timer_stop(m_statusTimer);
    esp_timer_delete(m_statusTimer);
    m_statusTimer = nullptr;
  }

  ESP_LOGI(TAG, "WebServerManager ended");
}

bool WebServerManager::basicAuth(httpd_req_t* req){
  if(!m_configManager.getConfig<espConfig::misc_config_t>().webAuthEnabled){
    return true;
  }
  size_t hdr_len = httpd_req_get_hdr_value_len(req, "Authorization");
  if(!(hdr_len > 0)){
    ESP_LOGD(TAG, "Authorization data not provided");
    return false;
  }
  std::string authReq; authReq.resize(hdr_len + 1);
  if(httpd_req_get_hdr_value_str(req, "Authorization", authReq.data(), authReq.size()) != ESP_OK){
    ESP_LOGD(TAG, "Invalid HTTP Header, authorization failed");
    return false;
  }
  const auto& cred = m_configManager.getConfig<espConfig::misc_config_t>();
  std::scoped_lock lock(m_authDigestMutex);
  if (!m_authDigestValid || m_authDigestUser != cred.webUsername || m_authDigestPass != cred.webPassword) {
    const std::string userpass = fmt::format("{}:{}", cred.webUsername, cred.webPassword);
    size_t n = 0;
    esp_crypto_base64_encode(NULL, 0, &n, (const uint8_t*)userpass.c_str(), userpass.size());
    m_authDigest = "Basic ";
    m_authDigest.resize(6+n);
    esp_crypto_base64_encode((uint8_t *)m_authDigest.data() + 6, m_authDigest.size(), &n, (const uint8_t *)userpass.c_str(), userpass.size());
    m_authDigestUser = cred.webUsername;
    m_authDigestPass = cred.webPassword;
    m_authDigestValid = true;
  }
  return authReq == m_authDigest;
}

// ============================================================================
// Route Setup
// ============================================================================

esp_err_t WebServerManager::ws_post_handshake_cb(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    return ESP_FAIL;
  }

  int sockfd = httpd_req_to_sockfd(req);
  ESP_LOGI(TAG, "WebSocket connection established: fd=%d", sockfd);
  instance->addWebSocketClient(sockfd);

  // Send initial device status & metrics
  std::string status = instance->getDeviceInfo();
  instance->queue_ws_frame(sockfd, (const uint8_t *)status.c_str(),
                           status.size(), HTTPD_WS_TYPE_TEXT);
  std::string metrics = instance->getDeviceMetrics();
  instance->queue_ws_frame(sockfd, (const uint8_t *)metrics.c_str(),
                           metrics.size(), HTTPD_WS_TYPE_TEXT);

  if (!esp_timer_is_active(instance->m_statusTimer)) {
    esp_timer_start_periodic(instance->m_statusTimer, 5000 * 1000);
  }

  if(!instance->m_wsBroadcastBuffer.empty()){
    std::vector<std::vector<uint8_t>> backlog;
    {
      std::scoped_lock lock(instance->m_wsBroadcastMutex);
      backlog.assign(std::make_move_iterator(instance->m_wsBroadcastBuffer.begin()),
                     std::make_move_iterator(instance->m_wsBroadcastBuffer.end()));
      instance->m_wsBroadcastBuffer.clear();
      instance->m_wsBroadcastBytes = 0;
    }
    for (auto &v : backlog) {
      instance->queue_ws_frame(sockfd, v.data(), v.size(), HTTPD_WS_TYPE_TEXT);
    }
  }

  return ESP_OK;
}

void WebServerManager::setupRoutes() {
  ESP_LOGI(TAG, "Setting up routes...");

  struct RouteConfig {
    const char *uri;
    httpd_method_t method;
    esp_err_t (*handler)(httpd_req_t *);
    void *ctx;
    bool is_ws = false;
  };

  RouteConfig routes[] = {
      // Static files
      {"/static/*", HTTP_GET, handleStaticFiles, this},
      {"/assets/*", HTTP_GET, handleStaticFiles, this},

      // Configuration endpoints
      {"/config", HTTP_GET, handleGetConfig, this},
      {"/config/clear", HTTP_POST, handleClearConfig, this},
      {"/config/save", HTTP_POST, handleSaveConfig, this},
      {"/eth_get_config", HTTP_GET, handleGetEthConfig, this},
      {"/nfc_get_presets", HTTP_GET, handleGetNfcPresets, this},

      // Action endpoints
      {"/reboot_device", HTTP_POST, handleReboot, this},
      {"/reset_hk_pair", HTTP_GET, handleHKReset, this},
      {"/reset_wifi_cred", HTTP_GET, handleWifiReset, this},
      {"/start_config_ap", HTTP_GET, handleStartConfigAP, this},

      // WebSocket
      {"/ws", HTTP_GET, handleWebSocket, this, true},

      // OTA endpoints
      {"/ota/*", HTTP_POST, handleOTAUpload, this},

      // Certificate endpoints
      {"/certificates", HTTP_POST, handleCertificateUpload, this},
      {"/certificates", HTTP_GET, handleCertificateStatus, this},
      {"/certificates", HTTP_DELETE, handleCertificateDelete, this},

      // Catch-all (must be last)
      {"/*", HTTP_GET, handleRootOrHash, this}};

  for (auto &r : routes) {
    httpd_uri_t uri = {.uri = r.uri,
                       .method = r.method,
                       .handler = r.handler,
                       .user_ctx = r.ctx};
#ifdef CONFIG_HTTPD_WS_SUPPORT
    uri.is_websocket = r.is_ws;
    if (r.is_ws) {
      uri.ws_post_handshake_cb = ws_post_handshake_cb;
    }
#endif
    esp_err_t err = httpd_register_uri_handler(m_server, &uri);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to register URI %s: %s", r.uri, esp_err_to_name(err));
    }
  }
  ESP_LOGI(TAG, "Routes setup complete");
}

// ============================================================================
// Captive Portal Route Setup
// ============================================================================

void WebServerManager::setupCaptivePortalRoutes() {
  ESP_LOGI(TAG, "Setting up captive portal routes...");

  struct RouteConfig {
    const char *uri;
    httpd_method_t method;
    esp_err_t (*handler)(httpd_req_t *);
    void *ctx;
  };

  RouteConfig routes[] = {
      // Captive portal API endpoints
      {"/captive_portal_config", HTTP_GET, handleGetCaptivePortalConfig, this},
      {"/captive_portal_config", HTTP_POST, handleSaveCaptivePortalConfig, this},
      {"/nfc_get_presets", HTTP_GET, handleGetNfcPresets, this},
      {"/eth_get_config", HTTP_GET, handleGetEthConfig, this},
      {"/wifi_scan", HTTP_GET, handleWifiScan, this},
      {"/reboot_device", HTTP_POST, handleReboot, this},
      // Static files needed for the captive portal UI
      {"/static/*", HTTP_GET, handleStaticFiles, this},
      {"/assets/*", HTTP_GET, handleStaticFiles, this},

      // The captive portal page itself
      {"/captive-portal", HTTP_GET, handleRootOrHash, this},

      // Catch-all redirect to captive portal (must be last)
      {"/*", HTTP_GET, handleCaptivePortal, this}};

  for (auto &r : routes) {
    httpd_uri_t uri = {.uri = r.uri,
                       .method = r.method,
                       .handler = r.handler,
                       .user_ctx = r.ctx,
                       .is_websocket = false,
                       .handle_ws_control_frames = false,
                       .supported_subprotocol = nullptr};
    esp_err_t err = httpd_register_uri_handler(m_server, &uri);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to register captive portal URI %s: %s", r.uri, esp_err_to_name(err));
    }
  }
  ESP_LOGI(TAG, "Captive portal routes setup complete");
}

// ============================================================================
// Utility Methods
// ============================================================================

WebServerManager *WebServerManager::getInstance(httpd_req_t *req) {
  return static_cast<WebServerManager *>(req->user_ctx);
}

esp_err_t WebServerManager::sendAuthFailure(httpd_req_t *req) {
  ESP_LOGE(TAG, "HTTP Authorization failed!");
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Connection", "keep-alive");
  httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
  httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
  return ESP_OK;
}

// ============================================================================
// Static File Handler
// ============================================================================

esp_err_t WebServerManager::handleStaticFiles(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  const char *last_slash = strrchr(req->uri, '/');
  const char *filename = last_slash ? last_slash + 1 : req->uri;
  std::string filepath = req->uri;
  bool use_compressed = false;
  if (strlen(filename) == 0){
    filename = "/index.html.gz";
    use_compressed = true;
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  } else {
    httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=31536000, immutable");
  }

  // Check for gzip compressed version
  if (str_ends_with(filename, ".js") || str_ends_with(filename, ".css")) {
    size_t accept_len = httpd_req_get_hdr_value_len(req, "Accept-Encoding");
    if (accept_len > 0 && accept_len < 256) {
      char hdr[256];
      if (httpd_req_get_hdr_value_str(req, "Accept-Encoding", hdr, sizeof(hdr)) == ESP_OK) {
        if (strstr(hdr, "gzip") != nullptr) {
          std::string compressed = filepath + ".gz";
          if (LittleFS.exists(compressed.c_str())) {
            use_compressed = true;
            filepath = compressed;
          }
        }
      }
    }
  }

  if (!LittleFS.exists(filepath.c_str())) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  File file = LittleFS.open(filepath.c_str(), "r");
  if (!file) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  // Determine content type
  const char *content_type = "text/plain";
  if (str_ends_with(filename, ".html"))
    content_type = "text/html";
  else if (str_ends_with(filename, ".css"))
    content_type = "text/css";
  else if (str_ends_with(filename, ".js"))
    content_type = "application/javascript";
  else if (str_ends_with(filename, ".json"))
    content_type = "application/json";
  else if (str_ends_with(filename, ".png"))
    content_type = "image/png";
  else if (str_ends_with(filename, ".jpg") || str_ends_with(filename, ".jpeg"))
    content_type = "image/jpeg";
  else if (str_ends_with(filename, ".ico"))
    content_type = "image/x-icon";
  else if (str_ends_with(filename, ".webp"))
    content_type = "image/webp";

  httpd_resp_set_type(req, content_type);
  httpd_resp_set_hdr(req, "Connection", "keep-alive");
  if (use_compressed)
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

  char buffer[4096];

  size_t bytes_read;
  esp_err_t err = ESP_OK;
  while ((bytes_read = file.read((uint8_t*)buffer, sizeof(buffer))) > 0) {
      err = httpd_resp_send_chunk(req, buffer, bytes_read);
      vTaskDelay(pdMS_TO_TICKS(5));
      if (err != ESP_OK) break;
  }

  file.close();
  if (err != ESP_OK) {
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "TLS send error");
      return err;
  }
  return httpd_resp_send_chunk(req, NULL, 0); // End chunked stream cleanly
}

/**
 * @brief Serve the single-page application entry (app.html) for root/hash requests.
 *
 * Authenticates the request, issues a sessionId cookie when different from the server's session,
 * sets appropriate response headers, and streams /app.html from LittleFS in chunks.
 *
 * @param req The HTTP request to handle.
 * @return esp_err_t ESP_OK on successful send; ESP_FAIL if authentication fails, the file is missing, or a send error occurs.
 */
esp_err_t WebServerManager::handleRootOrHash(httpd_req_t *req) {
  WebServerManager* instance = getInstance(req);
  char sessionId[65] = {};
  size_t sessionIdLen = sizeof(sessionId);
  esp_err_t err = httpd_req_get_cookie_val(req, "sessionId", sessionId, &sessionIdLen);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  std::string sessionCookie;
  if(err != ESP_OK || instance->m_sessionId.compare(sessionId) != 0){
    sessionCookie = fmt::format("sessionId={};", instance->m_sessionId);
    httpd_resp_set_hdr(req, "Set-Cookie", sessionCookie.c_str());
  }

  File file = LittleFS.open("/index.html.gz", "r");
  if (!file) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
  httpd_resp_set_hdr(req, "Connection", "keep-alive");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

  char buffer[1024];
  size_t bytes_read;
  while ((bytes_read = file.read((uint8_t *)buffer, sizeof(buffer))) > 0) {
    if (httpd_resp_send_chunk(req, buffer, bytes_read) != ESP_OK) {
      file.close();
      httpd_resp_send_chunk(req, NULL, 0);
      return ESP_FAIL;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  file.close();
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

// ============================================================================
// Configuration Handlers
// ============================================================================

esp_err_t WebServerManager::handleGetConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_OK;
  }

  char query[256], type_param[64];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      httpd_query_key_value(query, "type", type_param, sizeof(type_param)) !=
          ESP_OK) {
    return sendJsonError(req, "Missing 'type' parameter");
  }

  std::string type = type_param;
  JsonGuard dataGuard(nullptr);

  if (type == "mqtt") {
    std::string s = instance->m_configManager.serializeToJson<espConfig::mqttConfig_t>();
    dataGuard.reset(cJSON_Parse(s.c_str()));
  } else if (type == "misc") {
    std::string s = instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
    dataGuard.reset(cJSON_Parse(s.c_str()));
  } else if (type == "actions"){
    std::string s = instance->m_configManager.serializeToJson<espConfig::actions_config_t>();
    dataGuard.reset(cJSON_Parse(s.c_str()));
  } else if (type == "hkinfo") {
    const auto readerData = instance->m_readerDataManager.snapshot();
    JsonGuard hkInfo(cJSON_CreateObject());
    cJSON_AddStringToObject(hkInfo.get(), "group_identifier", fmt::format("{:02X}", fmt::join(readerData.identity.group_identifier, "")).c_str());
    cJSON_AddStringToObject(hkInfo.get(), "unique_identifier", fmt::format("{:02X}", fmt::join(readerData.identity.sub_identifier, "")).c_str());

    JsonGuard issuersArray(cJSON_CreateArray());
    for (const auto &issuer : readerData.issuers) {
      JsonGuard issuerJson(cJSON_CreateObject());
      cJSON_AddStringToObject(issuerJson.get(), "issuerId", fmt::format("{:02X}", fmt::join(issuer.id, "")).c_str());
      
      JsonGuard endpointsArray(cJSON_CreateArray());
      for (const auto &endpoint : issuer.endpoints) {
        JsonGuard ep(cJSON_CreateObject());
        cJSON_AddStringToObject(ep.get(), "endpointId", fmt::format("{:02X}", fmt::join(endpoint.id, "")).c_str());
        cJSON_AddItemToArray(endpointsArray.get(), ep.release());
      }
      cJSON_AddItemToObject(issuerJson.get(), "endpoints", endpointsArray.release());
      cJSON_AddItemToArray(issuersArray.get(), issuerJson.release());
    }
    cJSON_AddItemToObject(hkInfo.get(), "issuers", issuersArray.release());
    dataGuard = std::move(hkInfo);
  } else {
    return sendJsonError(req, "Invalid 'type' parameter");
  }

  httpd_resp_set_type(req, "application/json");
  std::string response = JsonBuilder::object()
      .addBool("success", true)
      .addItem("data", std::move(dataGuard))
      .toStringUnformatted();
  
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

esp_err_t WebServerManager::handleGetNfcPresets(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  JsonBuilder presets = JsonBuilder::object();
  presets.withArray("presets", [&](JsonBuilder& presetsArray) {
    for (auto &&v : nfcGpioPinsPresets) {
      JsonBuilder preset = JsonBuilder::object();
      preset.addString("name", v.name.c_str());
      preset.addNumber("type", v.type);
      preset.withArray("gpioPins", [&](JsonBuilder& gpioArray) {
        for (auto &&pin : v.gpioPins) {
          gpioArray.addItemToArray(JsonGuard(cJSON_CreateNumber(pin)));
        }
      });
      preset.addNumber("irqPin", v.irqPin);
      preset.addNumber("venPin", v.venPin);
      presetsArray.addItemToArray(std::move(preset).release());
    }
  });
  JsonBuilder response = JsonBuilder::object();
  response.addItem("data", std::move(presets).release());
  response.addBool("success", true);

  std::string resp = response.toStringUnformatted();
  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

esp_err_t WebServerManager::handleGetEthConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  JsonBuilder eth_config = JsonBuilder::object();

  // Supported chips
  eth_config.withArray("supportedChips", [&](JsonBuilder& chipsArray) {
    for (auto &&v : eth_config_ns::supportedChips) {
      JsonBuilder chip = JsonBuilder::object();
      chip.addString("name", v.second.name.c_str());
      chip.addBool("emac", v.second.emac);
      chip.addNumber("phy_type", v.second.phy_type);
      chipsArray.addItemToArray(std::move(chip).release());
    }
  });

  // Board presets
  eth_config.withArray("boardPresets", [&](JsonBuilder& boardPresetsArray) {
    for (auto &&v : eth_config_ns::boardPresets) {
      JsonBuilder preset = JsonBuilder::object();
      preset.addString("name", v.name.c_str());

      preset.withObject("ethChip", [&](JsonBuilder& chip) {
        chip.addString("name", v.ethChip.name.c_str());
        chip.addBool("emac", v.ethChip.emac);
        chip.addNumber("phy_type", v.ethChip.phy_type);
      });

      if(v.ethChip.emac){
#if CONFIG_ETH_USE_ESP32_EMAC
        preset.withObject("rmii_conf", [&](JsonBuilder& rmii_conf) {
          rmii_conf.addNumber("phy_addr", v.rmii_conf.phy_addr);
          rmii_conf.addNumber("pin_mcd", v.rmii_conf.pin_mcd);
          rmii_conf.addNumber("pin_mdio", v.rmii_conf.pin_mdio);
          rmii_conf.addNumber("pin_power", v.rmii_conf.pin_power);
          rmii_conf.addNumber("pin_rmii_clock", v.rmii_conf.pin_rmii_clock);
        });
#endif
      } else {
        preset.withObject("spi_conf", [&](JsonBuilder& spi_conf) {
          spi_conf.addNumber("spi_freq_mhz", v.spi_conf.spi_freq_mhz);
          spi_conf.addNumber("pin_cs", v.spi_conf.pin_cs);
          spi_conf.addNumber("pin_irq", v.spi_conf.pin_irq);
          spi_conf.addNumber("pin_rst", v.spi_conf.pin_rst);
          spi_conf.addNumber("pin_sck", v.spi_conf.pin_sck);
          spi_conf.addNumber("pin_miso", v.spi_conf.pin_miso);
          spi_conf.addNumber("pin_mosi", v.spi_conf.pin_mosi);
        });
      }
      boardPresetsArray.addItemToArray(std::move(preset).release());
    }
  });

  eth_config.addBool("ethEnabled", instance->m_configManager.getConfig<espConfig::misc_config_t>().ethernetEnabled);
  eth_config.addNumber("numSpiBuses", SPI_HOST_MAX - 1);

  httpd_resp_set_type(req, "application/json");
  JsonBuilder response = JsonBuilder::object();
  response.addBool("success", true);
  response.addItem("data", std::move(eth_config).release());
  
  std::string resp = response.toStringUnformatted();
  httpd_resp_send(req, resp.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

/**
 * @brief Handle an HTTP request to save a configuration object for a given config type.
 *
 * Processes the request's "type" query parameter and JSON body, validates the payload
 * against the current configuration schema, applies updates, persists the configuration,
 * publishes relevant configuration change events, and sends an appropriate JSON HTTP response.
 * May trigger a device reboot when certain configuration keys change.
 *
 * @param req The HTTP request containing the query parameter `type=<mqtt|misc|actions>` and
 *            a JSON body with the configuration fields to update.
 * @return esp_err_t `ESP_OK` if the configuration was saved and applied; `ESP_FAIL` otherwise.
 */
esp_err_t WebServerManager::handleSaveConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_OK;
  }

  char query[256], type_param[64];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      httpd_query_key_value(query, "type", type_param, sizeof(type_param)) != ESP_OK) {
    return sendJsonError(req, "Missing 'type' parameter");
  }

  const size_t max_content_size = 2048;
  if (req->content_len >= max_content_size) {
    return sendJsonError(req, "Request body too large", "413 Payload Too Large");
  }

  std::vector<char> content(max_content_size, 0);
  int ret = httpd_req_recv(req, content.data(), content.size() - 1);
  if (ret <= 0) {
    return ESP_FAIL;
  }
  content[ret] = '\0';

  JsonGuard obj(cJSON_Parse(content.data()));
  if (!obj) {
    instance->sendJsonError(req, "Invalid JSON");
    return ESP_OK;
  }

  std::string type = type_param;
  JsonGuard configSchema(nullptr);

  if (type == "mqtt") {
    std::string s = instance->m_configManager.serializeToJson<espConfig::mqttConfig_t>();
    configSchema.reset(cJSON_Parse(s.c_str()));
  } else if (type == "misc") {
    std::string s = instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
    configSchema.reset(cJSON_Parse(s.c_str()));
  } else if (type == "actions") {
    std::string s = instance->m_configManager.serializeToJson<espConfig::actions_config_t>();
    configSchema.reset(cJSON_Parse(s.c_str()));
  } else {
    return sendJsonError(req, "Invalid 'type' parameter");
  }

  if (!validateRequest(req, configSchema.get(), obj.get())) {
    return ESP_OK; // validateRequest already sent a full error response
  }

  bool success = false, rebootNeeded = false;
  std::string rebootMsg, errorMsg;

  cJSON *it = obj.get()->child;
  if (it == NULL) {
    return sendJsonError(req, "Received empty object, nothing to save");
  }
  
  // Safe string representation that cleans itself up
  std::string data_str = to_string_unformatted(obj);
  std::string result;
  
  while (it) {
    cJSON *configSchemaItem = cJSON_GetObjectItem(configSchema.get(), it->string);
    if (cJSON_Compare(it, configSchemaItem, true)) {
      it = it->next;
      continue;
    }

    const std::string keyStr = it->string;

    if (keyStr == "setupCode") {
      EventValueChanged s{.name = keyStr, .str = it->valuestring};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      HomekitEvent event{.type = HomekitEventType::SETUP_CODE_CHANGED, .data = d};
      std::vector<uint8_t> event_data;
      alpaca::serialize(event, event_data);
      AppEventLoop::publish(HK_EVENT, HK_INTERNAL_EVENT, event_data.data(), event_data.size());
    } else if (keyStr == "nfcNeopixelPin") {
      rebootNeeded = true;
      rebootMsg = "Pixel GPIO pin changed, reboot needed! Rebooting...";
    } else if (str_ends_with(keyStr.c_str(), "Pin")) {
      EventValueChanged s{.name = keyStr, .oldValue = (uint8_t)configSchemaItem->valueint, .newValue = (uint8_t)it->valueint};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      AppEventLoop::publish(HW_EVENT, HW_CONFIG_CHANGED, d.data(), d.size());
      
      if (keyStr == "gpioActionPin" && it->valueint != 255) {
        cJSON* dumbSwitch = cJSON_GetObjectItem(obj.get(), "hkDumbSwitchMode");
        if (dumbSwitch && cJSON_IsTrue(dumbSwitch)) {
          cJSON_SetBoolValue(dumbSwitch, false); // Mutates in-place safely
        }
      }
    } else if (keyStr == "btrLowStatusThreshold") {
      EventValueChanged s{.name = "btrLowThreshold", .newValue = (uint8_t)it->valueint};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      HomekitEvent event{.type = HomekitEventType::BTR_PROP_CHANGED, .data = d};
      std::vector<uint8_t> event_data;
      alpaca::serialize(event, event_data);
      AppEventLoop::publish(HK_EVENT, HK_INTERNAL_EVENT, event_data.data(), event_data.size());
    } else if (keyStr == "neoPixelType") {
      rebootNeeded = true;
      rebootMsg = "Pixel Type changed, reboot needed! Rebooting...";
    }
    it = it->next;
  }

  if (type == "mqtt") {
    result = instance->m_configManager.updateFromJson<espConfig::mqttConfig_t>(data_str);
    if (!result.empty()) {
      success = instance->m_configManager.saveConfig<espConfig::mqttConfig_t>();
      rebootNeeded = true;
      rebootMsg = "MQTT config saved, reboot needed! Rebooting...";
    }
  } else if (type == "misc") {
    result = instance->m_configManager.updateFromJson<espConfig::misc_config_t>(data_str);
    if (!result.empty()) {
      success = instance->m_configManager.saveConfig<espConfig::misc_config_t>();
      rebootNeeded = true;
      rebootMsg = "Misc config saved, reboot needed! Rebooting...";
    }
  } else if (type == "actions") {
    result = instance->m_configManager.updateFromJson<espConfig::actions_config_t>(data_str);
    if (!result.empty()) {
      success = instance->m_configManager.saveConfig<espConfig::actions_config_t>();
    }
  }

  httpd_resp_set_type(req, "application/json");
  if (success) {
    JsonBuilder res = JsonBuilder::object();
    res.addBool("success", true);
    res.addString("message", rebootNeeded ? rebootMsg.c_str() : "Saved and applied!");
    JsonGuard dataPtr(cJSON_Parse(result.c_str()));
    res.addItem("data", std::move(dataPtr));
    
    std::string response = res.toStringUnformatted();
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    if (rebootNeeded) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      esp_restart();
    }
  } else {
    JsonBuilder res = JsonBuilder::object();
    res.addBool("success", false);
    res.addString("error", errorMsg.empty() ? "Unable to save config!" : errorMsg.c_str());
    httpd_resp_set_status(req, HTTPD_500);
    std::string response = res.toStringUnformatted();
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  }
  return ESP_OK;
}

bool WebServerManager::validateRequest(httpd_req_t *req, cJSON *currentData, cJSON *obj) {
  bool overrideStrapping = false;
  cJSON *ovrStrItem = cJSON_GetObjectItem(obj, "overrideStrappingRestriction");
  if (!ovrStrItem) ovrStrItem = cJSON_GetObjectItem(currentData, "overrideStrappingRestriction");

  if (ovrStrItem) {
    overrideStrapping = cJSON_IsBool(ovrStrItem) && cJSON_IsTrue(ovrStrItem);
  } else {
    overrideStrapping = getInstance(req)->m_configManager.getConfig<espConfig::misc_config_t>().overrideStrappingRestriction;
  }

  cJSON *readerTypeItem = cJSON_GetObjectItem(obj, "nfcReaderType");
  const uint8_t effectiveReaderType =
      (readerTypeItem && cJSON_IsNumber(readerTypeItem))
          ? static_cast<uint8_t>(readerTypeItem->valueint)
          : getInstance(req)->m_configManager.getConfig<espConfig::misc_config_t>().nfcReaderType;

  cJSON *it = obj->child;
  while (it) {
    std::string keyStr = it->string;
    cJSON *existingValue = cJSON_GetObjectItem(currentData, keyStr.c_str());

    if (!existingValue) {
      std::string msg = "\"" + keyStr + "\" is not a valid configuration key.";
      sendJsonError(req, msg);
      return false;
    }

    cJSON *incomingValue = it;
    bool typeOk = false;
    if (cJSON_IsString(existingValue))
      typeOk = cJSON_IsString(incomingValue);
    else if (cJSON_IsObject(existingValue))
      typeOk = cJSON_IsObject(incomingValue);
    else if (cJSON_IsArray(existingValue))
      typeOk = cJSON_IsArray(incomingValue);
    else if (cJSON_IsBool(existingValue))
      typeOk = cJSON_IsBool(incomingValue) ||
               (cJSON_IsNumber(incomingValue) &&
                (incomingValue->valueint == 0 || incomingValue->valueint == 1));
    else if (cJSON_IsNumber(existingValue))
      typeOk = cJSON_IsNumber(incomingValue);

    if (!typeOk) {
      char *valueStr = cJSON_PrintUnformatted(incomingValue);
      std::unique_ptr<char, decltype(&cJSON_free)> valueStrGuard(valueStr, &cJSON_free);
      std::string msg = "Invalid type for key \"" + keyStr + "\". Received: " + std::string(valueStr ? valueStr : "null");
      sendJsonError(req, msg);
      return false;
    }

    // Setup code validation
    if (keyStr == "setupCode") {
      // Type check above already guarantees cJSON_IsString
      std::string code = incomingValue->valuestring;
      if (code.length() != 8 ||
          std::find_if(code.begin(), code.end(), [](unsigned char c) {
            return !std::isdigit(c);
          }) != code.end()) {
        std::string msg =
            "\"" + code + "\" is not valid. Must be an 8-digit number.";
        sendJsonError(req, msg);
        return false;
      }
      static constexpr std::array<const char*, 12> kWeakCodes = {
        "00000000","11111111","22222222","33333333","44444444","55555555",
        "66666666","77777777","88888888","99999999","12345678","87654321"
      };
      if (std::find(kWeakCodes.begin(), kWeakCodes.end(), code) != kWeakCodes.end()) {
        sendJsonError(req, "\"" + code + "\" is too simple to use as a Setup Code.");
        return false;
      }
      if (homeSpan.controllerListBegin() != homeSpan.controllerListEnd() &&
          code.compare(cJSON_GetStringValue(existingValue)) != 0) {
        sendJsonError(req, "Setup Code can only be set if no devices are paired");
        return false;
      }
    }
    // Pin validation
    else if (str_ends_with(keyStr.c_str(), "Pin")) {
      // IRQ/VEN only exist on the PN7161 reader; for other reader types the
      // values are meaningless and must not fail validation.
      const bool nfcReaderPins = keyStr == "nfcIrqPin" || keyStr == "nfcVenPin";
      if (nfcReaderPins && effectiveReaderType != 1) {
        it = it->next;
        continue;
      }
      // Reject anything outside uint8_t range BEFORE truncating, so a value
      // like 256 can't wrap to a valid-looking pin (0) and slip past both
      // the GPIO-validity check and the ownership check below.
      if (incomingValue->valueint < 0 || incomingValue->valueint > 255) {
        std::string msg = std::to_string(incomingValue->valueint) +
                          " is not a valid GPIO Pin for \"" + keyStr + "\".";
        sendJsonError(req, msg);
        return false;
      }

      const uint8_t incomingPin = static_cast<uint8_t>(incomingValue->valueint);

      // Fix: Should use || instead of && because output is a strict subset of input.
      if (incomingPin != 255 && (!GPIO_IS_VALID_GPIO(incomingPin) ||
                                 !GPIO_IS_VALID_OUTPUT_GPIO(incomingPin))) {
        std::string msg = std::to_string(incomingPin) +
                          " is not a valid GPIO Pin for \"" + keyStr + "\".";
        sendJsonError(req, msg);
        return false;
      }

      if (nfcReaderPins && effectiveReaderType == 1) {
        cJSON *gpioPinsItem = cJSON_GetObjectItem(obj, "nfcGpioPins");
        if (gpioPinsItem && cJSON_IsArray(gpioPinsItem)) {
          bool dup = false;
          cJSON *gp = NULL;
          cJSON_ArrayForEach(gp, gpioPinsItem) {
            if (cJSON_IsNumber(gp) && gp->valueint >= 0 && gp->valueint <= 255 &&
                static_cast<uint8_t>(gp->valueint) == incomingPin) { dup = true; break; }
          }
          if (dup) {
            std::string msg = std::to_string(incomingPin) + " for \"" + keyStr +
                              "\" duplicates a pin in \"nfcGpioPins\".";
            sendJsonError(req, msg);
            return false;
          }
        }
      }

      const uint8_t currentPin   = (cJSON_IsNumber(existingValue) &&
                                    existingValue->valueint >= 0 &&
                                    existingValue->valueint <= 255)
                                        ? static_cast<uint8_t>(existingValue->valueint)
                                        : uint8_t{255};
      if (auto error = check_pin_reassignment(incomingPin, currentPin, keyStr, -1, overrideStrapping, effectiveReaderType)) {
        std::string msg = std::to_string(incomingPin) + " for \"" + keyStr + "\" " + *error + ".";
        sendJsonError(req, msg);
        return false;
      }
    } else if (keyStr == "ethSpiBus" && cJSON_IsNumber(incomingValue) && (incomingValue->valueint < SPI2_HOST || incomingValue->valueint >= SPI_HOST_MAX)){
        std::string msg = std::to_string(incomingValue->valueint) +
                      " is not a valid SPI Bus value";
        sendJsonError(req, msg);
        return false;
    } else if ((str_ends_with(keyStr.c_str(), "Pins") || str_ends_with(keyStr.c_str(), "SpiConfig")) && cJSON_IsArray(incomingValue)){
      cJSON *currentArr = cJSON_GetObjectItem(currentData, keyStr.c_str());
      std::array<bool, 256> seenPins{};
      bool checkDuplicates = keyStr == "nfcGpioPins";
      cJSON *el = NULL;
      int idx = 0;
      cJSON_ArrayForEach(el, incomingValue) {
        if (cJSON_IsNumber(el)) {
          if (idx == 0 && keyStr == "ethSpiConfig") { idx++; continue; }

          // Reject out-of-range values before truncating so a wrapped value
          // can't slip past the ownership lookup below.
          if (el->valueint < 0 || el->valueint > 255) {
            std::string msg = std::to_string(el->valueint) +
                              " is not a valid GPIO Pin for \"" + keyStr + "\".";
            sendJsonError(req, msg);
            return false;
          }
          const uint8_t elPin = static_cast<uint8_t>(el->valueint);

          if (checkDuplicates) {
            if (elPin != 255 && seenPins[elPin]) {
              std::string msg = std::to_string(elPin) +
                                " is assigned more than once in \"" + keyStr + "\".";
              sendJsonError(req, msg);
              return false;
            }
            seenPins[elPin] = true;
          }

          uint8_t currentPin = 255;
          if (currentArr && cJSON_IsArray(currentArr)) {
            cJSON *ce = cJSON_GetArrayItem(currentArr, idx);
            if (ce && cJSON_IsNumber(ce) && ce->valueint >= 0 && ce->valueint <= 255)
              currentPin = static_cast<uint8_t>(ce->valueint);
          }
          if (auto error = check_pin_reassignment(elPin, currentPin, keyStr, idx, overrideStrapping, effectiveReaderType)) {
            std::string msg = std::to_string(elPin) + " for \"" + keyStr + "\" " + *error + ".";
            sendJsonError(req, msg);
            return false;
          }
        }
        idx++;
      }
    }
    // --- Heap Memory Guard Checks ---
    if (keyStr == "webHttpsEnabled" && cJSON_IsTrue(it)) {
      bool mqttSsl = getInstance(req)->m_configManager.getConfig<espConfig::mqttConfig_t>().useSSL;
      if (!heapGuardOk(req, mqttSsl, "HTTPS", "MQTT SSL")) { return false; }
    } else if (keyStr == "useSSL" && cJSON_IsTrue(it)) {
      bool https = getInstance(req)->m_configManager.getConfig<espConfig::misc_config_t>().webHttpsEnabled;
      if (!heapGuardOk(req, https, "MQTT SSL", "HTTPS")) { return false; }
    }

    if (cJSON_IsBool(existingValue) && cJSON_IsNumber(incomingValue)) {
      cJSON_SetBoolValue(incomingValue, incomingValue->valueint);
    }
    
    it = it->next;
  }
  
  return true;
}

esp_err_t WebServerManager::handleClearConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char query[256], type_param[64];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      httpd_query_key_value(query, "type", type_param, sizeof(type_param)) != ESP_OK) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "400 Bad Request");
    std::string response = JsonBuilder::object()
        .addBool("success", false)
        .addString("error", "Missing 'type' parameter")
        .toStringUnformatted();
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string type = type_param;
  bool success = false;
  if (type == "mqtt")        success = instance->m_configManager.deleteConfig<espConfig::mqttConfig_t>();
  else if (type == "misc")   success = instance->m_configManager.deleteConfig<espConfig::misc_config_t>();
  else if (type == "actions")success = instance->m_configManager.deleteConfig<espConfig::actions_config_t>();

  if (success) {
    httpd_resp_send(req, "Cleared! Rebooting...", HTTPD_RESP_USE_STRLEN);
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
    return ESP_OK;
  }
  httpd_resp_send_500(req);
  return ESP_FAIL;
}

// ============================================================================
// Action Handlers
// ============================================================================

esp_err_t WebServerManager::handleReboot(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (instance && !instance->basicAuth(req)) {
    return sendAuthFailure(req);
  }
  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, "{\"success\":\"true\",\"message\":\"Rebooting...\"}");
  vTaskDelay(pdMS_TO_TICKS(1000));
  esp_restart();
  return ESP_OK;
}

esp_err_t WebServerManager::handleHKReset(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "application/json");
  std::string response = JsonBuilder::object()
      .addBool("success", true)
      .addString("message", "Erasing HomeKit pairings, device will reboot")
      .toStringUnformatted();
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  instance->m_readerDataManager.deleteAllReaderData();
  homeSpan.processSerialCommand("H");
  return ESP_OK;
}

esp_err_t WebServerManager::handleWifiReset(httpd_req_t *req) {
  WebServerManager* instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  httpd_resp_set_type(req, "application/json");
  std::string response = JsonBuilder::object()
      .addBool("success", true)
      .addString("message", "Erasing WiFi credentials, device will reboot")
      .toStringUnformatted();
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  homeSpan.processSerialCommand("X");
  return ESP_OK;
}

esp_err_t WebServerManager::handleStartConfigAP(httpd_req_t *req) {
  WebServerManager* instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  httpd_resp_set_type(req, "application/json");
  std::string response = JsonBuilder::object()
      .addBool("success", true)
      .addString("message", "Starting AP mode...")
      .toStringUnformatted();
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  vTaskDelay(pdMS_TO_TICKS(1000));
  auto run = [](void* p){ homeSpan.processSerialCommand("A"); vTaskDelete(nullptr); };
  xTaskCreate(run, "hs_cmd", 4096, NULL, 5, nullptr);
  return ESP_OK;
}

// ============================================================================
// Captive Portal
// ============================================================================

esp_err_t WebServerManager::handleCaptivePortal(httpd_req_t *req) {
  httpd_resp_set_status(req, "302 Found");
  httpd_resp_set_hdr(req, "Location", "/captive-portal");
  httpd_resp_send(req, NULL, 0);
  return ESP_OK;
}

esp_err_t WebServerManager::handleGetCaptivePortalConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const auto &miscConfig = instance->m_configManager.getConfig<espConfig::misc_config_t>();

  JsonBuilder config = JsonBuilder::object();
  config.addString("setupCode", miscConfig.setupCode.c_str());
  config.addNumber("hk_key_color", miscConfig.hk_key_color);
  config.addNumber("nfcPinsPreset", miscConfig.nfcPinsPreset);
  
  config.withArray("nfcGpioPins", [&](JsonBuilder& arr) {
    for (auto &&pin : miscConfig.nfcGpioPins) {
      arr.addItemToArray(JsonGuard(cJSON_CreateNumber(pin)));
    }
  });
  
  config.addNumber("nfcReaderType", miscConfig.nfcReaderType);
  config.addNumber("nfcIrqPin", miscConfig.nfcIrqPin);
  config.addNumber("nfcVenPin", miscConfig.nfcVenPin);
  config.addBool("ethernetEnabled", miscConfig.ethernetEnabled);
  config.addNumber("ethActivePreset", miscConfig.ethActivePreset);
  config.addNumber("ethPhyType", miscConfig.ethPhyType);
  config.addNumber("ethSpiBus", miscConfig.ethSpiBus);

  config.withArray("ethRmiiConfig", [&](JsonBuilder& arr) {
    for (auto &&val : miscConfig.ethRmiiConfig) {
      arr.addItemToArray(JsonGuard(cJSON_CreateNumber(val)));
    }
  });

  config.withArray("ethSpiConfig", [&](JsonBuilder& arr) {
    for (auto &&val : miscConfig.ethSpiConfig) {
      arr.addItemToArray(JsonGuard(cJSON_CreateNumber(val)));
    }
  });

  config.addBool("overrideStrappingRestriction", miscConfig.overrideStrappingRestriction);
  config.addBool("nfcFastPollingEnabled", miscConfig.nfcFastPollingEnabled);

  httpd_resp_set_type(req, "application/json");
  std::string response = JsonBuilder::object()
      .addBool("success", true)
      .addItem("data", std::move(config).release())
      .toStringUnformatted();
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

static bool connectWiFi(const char* ssid, const char* password, int timeoutMs = 30000) {
  ESP_LOGI("WiFiTest", "Testing connection to SSID: %s", ssid);

  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  
  int elapsed = 0;
  const int checkInterval = 1000;
  bool connected = false;
  
  while (elapsed < timeoutMs) {
    vTaskDelay(pdMS_TO_TICKS(checkInterval));
    elapsed += checkInterval;
    
    if (WiFi.isConnected()) {
      ESP_LOGI("WiFiTest", "Connected to %s, RSSI: %d", ssid, WiFi.RSSI());
      connected = true;
      break;
    } else {
      ESP_LOGW("WiFiTest", "Not yet connected!");
    }
  }
  
  if (!connected) {
    ESP_LOGE("WiFiTest", "Connection timeout after %d ms", timeoutMs);
    WiFi.disconnect();
  }
  
  return connected;
}

struct WifiSaveParams {
    httpd_req_t* req;
    WebServerManager* instance;
    std::string ssid;
    std::string password;
    std::string setupCode;
    bool hasSetupCode;
    std::string cleaned_body_str;
};

struct EthSaveParams {
    httpd_req_t* req;
    WebServerManager* instance;
    std::string setupCode;
    bool hasSetupCode;
    std::string cleaned_body_str;
};

static constexpr int ETH_IP_WAIT_MS = 3000;

/**
 * @brief Persist the captive-portal submission, start the ethernet driver, and
 *        report whether it came up.
 *
 * Runs off the HTTPD task (the request is completed asynchronously). The
 * ETH_GOT_IP subscription is registered before the driver starts so the event
 * cannot be missed. Outcomes:
 * - driver failed to start -> 400, error message; the portal lets the user fix
 *   the ethernet settings and resubmit.
 * - driver started + IP within ETH_IP_WAIT_MS -> success with the real IP.
 * - driver started, no IP -> success with 0.0.0.0 and an explanatory message;
 *   the device reports 0.0.0.0 until the link/DHCP comes up after reboot.
 */
void WebServerManager::captivePortalEthSaveTask(void *pvParameters) {
  EthSaveParams *params = static_cast<EthSaveParams *>(pvParameters);

  if (params->hasSetupCode) {
    homeSpan.setPairingCode(params->setupCode.c_str(), false);
  }

  params->instance->m_configManager.updateFromJson<espConfig::misc_config_t>(
      params->cleaned_body_str);
  params->instance->m_configManager.saveConfig<espConfig::misc_config_t>();

  EventGroupHandle_t ethEvents = xEventGroupCreate();
  auto gotIpSub = AppEventLoop::subscribe(ETH_APP_EVENT, ETH_GOT_IP,
      [ethEvents](const uint8_t* data, size_t size){
        if (ethEvents) xEventGroupSetBits(ethEvents, BIT0);
      });

  const auto miscConfig =
      params->instance->m_configManager.getConfig<espConfig::misc_config_t>();
  const bool driverStarted = EthernetDriver::start(miscConfig);

  bool gotIp = false;
  std::string ipAddr = "0.0.0.0";
  if (driverStarted) {
    if (ethEvents && gotIpSub.is_valid()) {
      gotIp = (xEventGroupWaitBits(ethEvents, BIT0, pdFALSE, pdFALSE,
                                   pdMS_TO_TICKS(ETH_IP_WAIT_MS)) & BIT0) != 0;
    }
    if (gotIp) {
      ipAddr = ETH.localIP().toString().c_str();
    }
  }

  if (gotIpSub.is_valid()) gotIpSub.reset();
  if (ethEvents) vEventGroupDelete(ethEvents);

  httpd_resp_set_type(params->req, "application/json");
  JsonBuilder res = JsonBuilder::object();
  std::string message;
  if (!driverStarted) {
    httpd_resp_set_status(params->req, "400 Bad Request");
    res.addBool("success", false);
    message = "Ethernet driver failed to start. Please check your Ethernet "
              "module settings and try again.";
    res.addString("error", message.c_str());
  } else {
    res.addBool("success", true);
    if (gotIp) {
      message = "Configuration saved. Device will now reboot.";
    } else {
      message = "Configuration saved. The Ethernet driver started, but no IP "
                "address was assigned within 3 seconds. Device will now reboot.";
    }
    res.withObject("data", [&](JsonBuilder &data) {
      data.addString("ip_addr", ipAddr.c_str());
    });
    res.addString("message", message.c_str());
  }

  std::string response = res.toStringUnformatted();
  httpd_resp_send(params->req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  httpd_req_async_handler_complete(params->req);
  delete params;
  vTaskDelete(NULL);
}

void WebServerManager::captivePortalSaveTask(void *pvParameters) {
  WifiSaveParams *params = static_cast<WifiSaveParams *>(pvParameters);

  bool connected =
      connectWiFi(params->ssid.c_str(), params->password.c_str(), 15000);

  if (connected) {
    homeSpan.setWifiCredentials(params->ssid.c_str(), params->password.c_str());

    if (params->hasSetupCode) {
      homeSpan.setPairingCode(params->setupCode.c_str(), false);
    }

    params->instance->m_configManager.updateFromJson<espConfig::misc_config_t>(
        params->cleaned_body_str);
    params->instance->m_configManager.saveConfig<espConfig::misc_config_t>();

    std::string ipAddr = WiFi.localIP().toString().c_str();

    httpd_resp_set_type(params->req, "application/json");
    JsonBuilder res = JsonBuilder::object();
    res.addBool("success", true);
    res.addString("message", "Configuration saved successfully.");
    res.withObject("data", [&](JsonBuilder &data) {
      data.addString("ip_addr", ipAddr.c_str());
    });

    std::string response = res.toStringUnformatted();
    httpd_resp_send(params->req, response.c_str(), HTTPD_RESP_USE_STRLEN);

    httpd_req_async_handler_complete(params->req);

    delete params;
  } else {
    httpd_resp_set_status(params->req, "400 Bad Request");
    httpd_resp_set_type(params->req, "application/json");
    std::string response =
        JsonBuilder::object()
            .addBool("success", false)
            .addString("error", "Failed to connect to WiFi network. Please "
                                "check your credentials and try again.")
            .toStringUnformatted();
    httpd_resp_send(params->req, response.c_str(), HTTPD_RESP_USE_STRLEN);

    httpd_req_async_handler_complete(params->req);

    delete params;
  }
  vTaskDelete(NULL);
}

esp_err_t WebServerManager::handleSaveCaptivePortalConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const size_t max_content_size = 2048;
  if (req->content_len >= max_content_size) {
    httpd_resp_set_status(req, "413 Payload Too Large");
    httpd_resp_set_type(req, "application/json");
    std::string response = JsonBuilder::object()
        .addBool("success", false)
        .addString("error", "Request body too large")
        .toStringUnformatted();
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::vector<char> content(max_content_size, 0);
  int ret = httpd_req_recv(req, content.data(), content.size() - 1);
  if (ret <= 0) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    std::string response = JsonBuilder::object()
        .addBool("success", false)
        .addString("error", "Invalid request body")
        .toStringUnformatted();
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }
  content[ret] = '\0';

  JsonGuard obj(cJSON_Parse(content.data()));
  if (!obj) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    std::string response = JsonBuilder::object()
        .addBool("success", false)
        .addString("error", "Invalid JSON")
        .toStringUnformatted();
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string ssid;
  std::string password;
  std::string setupCode;
  bool wifiProvided = false;
  bool hasSetupCode = false;

  cJSON *ssidItem = cJSON_GetObjectItem(obj.get(), "wifiSsid");
  cJSON *passwordItem = cJSON_GetObjectItem(obj.get(), "wifiPassword");
  if (ssidItem && cJSON_IsString(ssidItem)) {
    ssid = ssidItem->valuestring;
    if (!ssid.empty()) wifiProvided = true;
  }
  if (passwordItem && cJSON_IsString(passwordItem)) {
    password = passwordItem->valuestring;
  }

  cJSON_DeleteItemFromObject(obj.get(), "wifiSsid");
  cJSON_DeleteItemFromObject(obj.get(), "wifiPassword");

  cJSON *ethEnabledItem = cJSON_GetObjectItem(obj.get(), "ethernetEnabled");
  bool ethernetEnabled = (ethEnabledItem && cJSON_IsBool(ethEnabledItem) && cJSON_IsTrue(ethEnabledItem));

  if (!ethernetEnabled && !wifiProvided) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    std::string response = JsonBuilder::object()
        .addBool("success", false)
        .addString("error", "WiFi SSID and password are required (or enable Ethernet)")
        .toStringUnformatted();
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  if (wifiProvided) {
    if (ssid.length() > 32 || password.length() < 8 || password.length() > 64) {
      httpd_resp_set_status(req, "400 Bad Request");
      httpd_resp_set_type(req, "application/json");
      std::string response = JsonBuilder::object()
          .addBool("success", false)
          .addString("error", "Invalid WiFi credentials length")
          .toStringUnformatted();
      httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
      return ESP_FAIL;
    }
  }

  cJSON *colorItem = cJSON_GetObjectItem(obj.get(), "hk_key_color");
  if (colorItem && cJSON_IsNumber(colorItem) && colorItem->valueint > 3) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"Invalid hk_key_color (must be <= 3)\"}");
    return ESP_FAIL;
  }

  cJSON *nfcReaderTypeItem = cJSON_GetObjectItem(obj.get(), "nfcReaderType");
  if (nfcReaderTypeItem && cJSON_IsNumber(nfcReaderTypeItem) && (nfcReaderTypeItem->valueint < 0 || nfcReaderTypeItem->valueint > 2)) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"Invalid nfcReaderType\"}");
    return ESP_FAIL;
  }

  cJSON *setupCodeItem = cJSON_GetObjectItem(obj.get(), "setupCode");
  if (setupCodeItem && cJSON_IsString(setupCodeItem)) {
    setupCode = setupCodeItem->valuestring;
    hasSetupCode = true;
  }

  std::string currentConfigJson = instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
  JsonGuard currentConfigData(cJSON_Parse(currentConfigJson.c_str()));
  if (!currentConfigData) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  bool isValid = instance->validateRequest(req, currentConfigData.get(), obj.get());
  if (!isValid) {
    return ESP_FAIL; 
  }
  
  std::string cleaned_body_str = to_string_unformatted(obj);

  if (wifiProvided) {
    httpd_req_t* reqCopy = nullptr;
    if (httpd_req_async_handler_begin(req, &reqCopy) != ESP_OK) {
      return sendJsonError(req, "Failed to start save operation");
    }

    WifiSaveParams* params = new WifiSaveParams{
      .req = reqCopy,
      .instance = instance,
      .ssid = ssid,
      .password = password,
      .setupCode = setupCode,
      .hasSetupCode = hasSetupCode,
      .cleaned_body_str = cleaned_body_str
    };

BaseType_t task;
#ifndef CONFIG_FREERTOS_UNICORE
    task = xTaskCreatePinnedToCore(captivePortalSaveTask, "wifi_save_task", 8192, params, 5, nullptr, 1);
#else
    task = xTaskCreate(captivePortalSaveTask, "wifi_save_task", 8192, params, 5, nullptr);
#endif
    if (task != pdPASS) {
      ESP_LOGE(TAG, "Failed to create WiFi save task");
      delete params;
      httpd_req_async_handler_complete(reqCopy);
      return sendJsonError(req, "Failed to create save task");
    }

    return ESP_OK;
  }

  if (ethernetEnabled) {
    httpd_req_t* reqCopy = nullptr;
    if (httpd_req_async_handler_begin(req, &reqCopy) != ESP_OK) {
      return sendJsonError(req, "Failed to start save operation");
    }

    EthSaveParams* params = new EthSaveParams{
      .req = reqCopy,
      .instance = instance,
      .setupCode = setupCode,
      .hasSetupCode = hasSetupCode,
      .cleaned_body_str = cleaned_body_str
    };

    BaseType_t task;
#ifndef CONFIG_FREERTOS_UNICORE
    task = xTaskCreatePinnedToCore(captivePortalEthSaveTask, "eth_save_task", 8192, params, 5, nullptr, 1);
#else
    task = xTaskCreate(captivePortalEthSaveTask, "eth_save_task", 8192, params, 5, nullptr);
#endif
    if (task != pdPASS) {
      ESP_LOGE(TAG, "Failed to create Ethernet save task");
      delete params;
      httpd_req_async_handler_complete(reqCopy);
      return sendJsonError(req, "Failed to create save task");
    }

    return ESP_OK;
  }

  httpd_resp_set_type(req, "application/json");
  std::string response = JsonBuilder::object()
      .addBool("success", true)
      .addString("message", "Configuration saved successfully")
      .toStringUnformatted();
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

esp_err_t WebServerManager::handleWifiScan(httpd_req_t *req) {
  ESP_LOGI(TAG, "Starting WiFi scan...");

  wifi_mode_t current_mode;
  esp_wifi_get_mode(&current_mode);

  bool need_restore_mode = false;
  if (current_mode == WIFI_MODE_AP) {
    ESP_LOGI(TAG, "Temporarily enabling APSTA mode for scanning");
    esp_err_t mode_err = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if (mode_err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to set APSTA mode: %s", esp_err_to_name(mode_err));
      httpd_resp_set_status(req, "500 Internal Server Error");
      httpd_resp_set_type(req, "application/json");
      std::string response = JsonBuilder::object()
          .addBool("success", false)
          .addString("error", "Failed to enable scan mode")
          .toStringUnformatted();
      httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
      return ESP_OK;
    }
    need_restore_mode = true;
  }

  wifi_scan_config_t scan_config = {};
  scan_config.channel = 0;
  scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
  scan_config.scan_time.active.min = 100;
  scan_config.scan_time.active.max = 300;

  esp_err_t err = esp_wifi_scan_start(&scan_config, true);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "WiFi scan failed to start: %s", esp_err_to_name(err));
    if (need_restore_mode) {
      esp_wifi_set_mode(current_mode);
    }
    httpd_resp_set_status(req, "500 Internal Server Error");
    httpd_resp_set_type(req, "application/json");
    std::string response = JsonBuilder::object()
        .addBool("success", false)
        .addString("error", "WiFi scan failed to start")
        .toStringUnformatted();
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
  }

  uint16_t ap_count = 0;
  esp_wifi_scan_get_ap_num(&ap_count);
  constexpr uint16_t MAX_AP_COUNT = 20;
  if (ap_count > MAX_AP_COUNT) {
    ESP_LOGW(TAG, "Capping AP scan results from %d to %d", ap_count, MAX_AP_COUNT);
    ap_count = MAX_AP_COUNT;
  }
  ESP_LOGI(TAG, "Found %d access points", ap_count);

  wifi_ap_record_t ap_records[MAX_AP_COUNT];
  esp_wifi_scan_get_ap_records(&ap_count, ap_records);

  if (need_restore_mode) {
    esp_wifi_set_mode(current_mode);
    ESP_LOGI(TAG, "Restored WiFi mode to AP");
  }

  JsonBuilder res = JsonBuilder::object();
  res.addBool("success", true);
  res.withArray("data", [&](JsonBuilder& networks) {
    for (int i = 0; i < ap_count; i++) {
      JsonBuilder network = JsonBuilder::object();
      network.addString("ssid", (char*)ap_records[i].ssid);
      network.addNumber("rssi", ap_records[i].rssi);
      network.addNumber("channel", ap_records[i].primary);

      const char* auth_mode;
      switch (ap_records[i].authmode) {
        case WIFI_AUTH_OPEN: auth_mode = "OPEN"; break;
        case WIFI_AUTH_WEP: auth_mode = "WEP"; break;
        case WIFI_AUTH_WPA_PSK: auth_mode = "WPA_PSK"; break;
        case WIFI_AUTH_WPA2_PSK: auth_mode = "WPA2_PSK"; break;
        case WIFI_AUTH_WPA_WPA2_PSK: auth_mode = "WPA_WPA2_PSK"; break;
        case WIFI_AUTH_WPA2_ENTERPRISE: auth_mode = "WPA2_ENTERPRISE"; break;
        case WIFI_AUTH_WPA3_PSK: auth_mode = "WPA3_PSK"; break;
        case WIFI_AUTH_WPA2_WPA3_PSK: auth_mode = "WPA2_WPA3_PSK"; break;
        default: auth_mode = "UNKNOWN"; break;
      }
      network.addString("auth", auth_mode);
      networks.addItemToArray(std::move(network).release());
    }
  });
  res.addString("message", "WiFi scan complete");

  httpd_resp_set_type(req, "application/json");
  std::string response = res.toStringUnformatted();
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

// ============================================================================
// WebSocket Implementation
// ============================================================================

struct AsyncWsData {
  httpd_handle_t server;
  int fd;
  httpd_ws_type_t type;
  std::vector<uint8_t> payload;
};

esp_err_t WebServerManager::handleWebSocket(httpd_req_t *req) {
#ifndef CONFIG_HTTPD_WS_SUPPORT
  httpd_resp_set_status(req, "501 Not Implemented");
  httpd_resp_send(req, "WebSocket not enabled", HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
#else
  WebServerManager *instance = getInstance(req);
  if (!instance)
    return ESP_FAIL;

  if (req->method == HTTP_GET) {
    char sessionId[65] = {};
    size_t sessionIdLen = sizeof(sessionId);
    esp_err_t err = httpd_req_get_cookie_val(req, "sessionId", sessionId, &sessionIdLen);
    if(!instance->basicAuth(req) && (err != ESP_OK || strncmp(sessionId, instance->m_sessionId.c_str(), sessionIdLen) != 0)){
      return sendAuthFailure(req);
    }

    // Handshake check succeeded. Returning ESP_OK completes the handshake.
    // The server will invoke WebServerManager::ws_post_handshake_cb immediately after.
    return ESP_OK;
  }

  // Receive WebSocket frame
  httpd_ws_frame_t ws_pkt = {};
  esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);

  if (ret != ESP_OK) {
    int sockfd = httpd_req_to_sockfd(req);
    const char *err_name = esp_err_to_name(ret);
    bool is_protocol_error =
        (err_name &&
         (strstr(err_name, "masked") || strstr(err_name, "MASKED"))) ||
        errno == ECONNRESET || errno == EPIPE || errno == ENOTCONN;
    if (ret == ESP_FAIL || ret == ESP_ERR_INVALID_STATE ||
        ret == ESP_ERR_INVALID_ARG || is_protocol_error) {
      instance->removeWebSocketClient(sockfd);
    }
    return ret;
  }

  if (ws_pkt.len > MAX_WS_PAYLOAD) {
    ESP_LOGE(TAG, "Payload too large: %zu", ws_pkt.len);
    instance->removeWebSocketClient(httpd_req_to_sockfd(req));
    return ESP_FAIL;
  }

  std::string payload;
  if (ws_pkt.len) {
    payload.resize(ws_pkt.len);
    ws_pkt.payload = (uint8_t *)payload.data();
    ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
    if (ret != ESP_OK) {
      int sockfd = httpd_req_to_sockfd(req);
      const char *err_name = esp_err_to_name(ret);
      bool is_protocol_error =
          (err_name &&
           (strstr(err_name, "masked") || strstr(err_name, "MASKED"))) ||
          errno == ECONNRESET || errno == EPIPE || errno == ENOTCONN;
      if (ret == ESP_FAIL || ret == ESP_ERR_INVALID_STATE ||
          ret == ESP_ERR_INVALID_ARG || is_protocol_error) {
        instance->removeWebSocketClient(sockfd);
      }
      return ret;
    }
  }

  if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {
    return instance->handleWebSocketMessage(req, payload);
  } else if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
    instance->removeWebSocketClient(httpd_req_to_sockfd(req));
  }
  return ESP_OK;
#endif
}

void WebServerManager::addWebSocketClient(int fd) {
  std::scoped_lock lock(m_wsClientsMutex);
  auto it = std::find_if(
      m_wsClients.begin(), m_wsClients.end(),
      [fd](const std::unique_ptr<WsClient> &c) { return c->fd == fd; });
  if (it == m_wsClients.end()) {
    m_wsClients.emplace_back(std::make_unique<WsClient>(
        fd, m_wsClientGeneration.load(std::memory_order_relaxed)));
  }
}

void WebServerManager::removeWebSocketClient(int fd) {
  bool stopTimer = false;
  size_t remaining = 0;
  bool removed = false;
  {
    std::scoped_lock lock(m_wsClientsMutex);
    auto it = std::find_if(
        m_wsClients.begin(), m_wsClients.end(),
        [fd](const std::unique_ptr<WsClient> &c) { return c->fd == fd; });
    if (it != m_wsClients.end()) {
      m_wsClients.erase(it);
      remaining = m_wsClients.size();
      stopTimer = m_wsClients.empty();
      removed = true;
      m_wsClientGeneration.fetch_add(1, std::memory_order_relaxed);
    }
  }
  if (removed) {
    ESP_LOGI(TAG, "Removed WebSocket client fd=%d, remaining: %zu", fd,
             remaining);
  }
  if (stopTimer && m_statusTimer && esp_timer_is_active(m_statusTimer)) {
    esp_timer_stop(m_statusTimer);
  }
}

void WebServerManager::setWSBackLogSize(const uint16_t size){
  wsBacklogSize = size > kMaxBacklogFrames ? kMaxBacklogFrames : size;
}

void WebServerManager::broadcastWs(const uint8_t *payload, size_t len,
                                   httpd_ws_type_t type) {
  std::vector<int> fds;
  {
    std::scoped_lock lock(m_wsClientsMutex); 
    fds.reserve(m_wsClients.size());
    for (const auto &c : m_wsClients)
      fds.push_back(c->fd);
  }
  if (fds.empty() && wsBacklogSize > 0) {
    if (len > kMaxBacklogBytes) {
      return;
    }
    std::scoped_lock lock(m_wsBroadcastMutex);
    while (!m_wsBroadcastBuffer.empty() &&
           (m_wsBroadcastBuffer.size() >= wsBacklogSize ||
            m_wsBroadcastBytes + len > kMaxBacklogBytes)) {
      m_wsBroadcastBytes -= m_wsBroadcastBuffer.front().size();
      m_wsBroadcastBuffer.pop_front();
    }
    m_wsBroadcastBytes += len;
    m_wsBroadcastBuffer.emplace_back(payload, payload + len);
    return;
  }
  std::shared_ptr<std::vector<uint8_t>> shared;
  if (len > WsFrame::INLINE_SIZE) {
    shared = std::make_shared<std::vector<uint8_t>>(payload, payload + len);
  }
  for (int fd : fds){
    queue_ws_frame(fd, payload, len, type, shared);
  }
}

void WebServerManager::queue_ws_frame(int fd, const uint8_t *payload,
                                      size_t len, httpd_ws_type_t type,
                                      const std::shared_ptr<std::vector<uint8_t>>& shared) {
  uint32_t generation = 0;
  bool known = false;
  {
    std::scoped_lock lock(m_wsClientsMutex);
    auto it = std::find_if(
        m_wsClients.begin(), m_wsClients.end(),
        [fd](const std::unique_ptr<WsClient> &c) { return c->fd == fd; });
    if (it != m_wsClients.end()) {
      generation = (*it)->generation;
      known = true;
    }
  }
  if (!known)
    return; // client already gone; nothing to deliver to

  WsFrame *frame = new WsFrame;
  if (!frame)
    return;

  frame->fd = fd;
  frame->type = type;
  frame->len = len;
  frame->generation = generation;
  if (len <= WsFrame::INLINE_SIZE) {
    memcpy(frame->inlinePayload, payload, len);
    frame->payload = frame->inlinePayload;
  } else if (shared) {
    frame->payload = shared->data();
    frame->sharedData = std::move(shared);
  } else {
    frame->sharedData = std::make_shared<std::vector<uint8_t>>(payload, payload + len);
    frame->payload = frame->sharedData->data();
  }

  // Never block the caller (this runs on the log sink task): drop and count
  // instead. A blocked enqueue here would stall all log dispatch.
  if (xQueueSend(m_wsQueue, &frame, 0) != pdTRUE) {
    m_wsFrameDropped.fetch_add(1, std::memory_order_relaxed);
    delete frame;
  }
}

void WebServerManager::ws_send_task(void *arg) {
  WebServerManager *instance = static_cast<WebServerManager *>(arg);
  WsFrame *raw_frame = nullptr;

  while (true) {
    if (xQueueReceive(instance->m_wsQueue, &raw_frame, portMAX_DELAY) !=
        pdPASS) {
      continue;
    }
    if (!raw_frame)
      continue;

    if (raw_frame->fd == -1) {
      delete raw_frame;
      xTaskNotifyGive(instance->m_wsTaskHandle);
      vTaskDelete(NULL);
      return;
    }

    // Drain everything already queued before waiting again so a burst of
    // frames costs one wake-up instead of one per frame.
    do {
      WsFramePtr frame(raw_frame);

      {
        std::scoped_lock<std::mutex> lock(instance->m_wsClientsMutex);
        auto it = std::find_if(
            instance->m_wsClients.begin(), instance->m_wsClients.end(),
            [fd = frame->fd](const std::unique_ptr<WsClient> &c) {
              return c->fd == fd;
            });
        if (it == instance->m_wsClients.end() ||
            (*it)->generation != frame->generation)
          continue;
      }

      httpd_ws_frame_t ws_pkt = {};
      ws_pkt.final = true;
      ws_pkt.fragmented = false;
      ws_pkt.type = frame->type;
      ws_pkt.len = frame->len;
      ws_pkt.payload = const_cast<uint8_t*>(frame->payload);

      esp_err_t send_ret =
          httpd_ws_send_data(instance->m_server, frame->fd, &ws_pkt);
      bool remove = false;
      if (send_ret != ESP_OK) {
        const char *err = esp_err_to_name(send_ret);
        bool is_err =
            (err && (strstr(err, "masked") || strstr(err, "MASKED"))) ||
            send_ret == ESP_FAIL;
        remove = is_err || send_ret == ESP_ERR_INVALID_STATE ||
                 send_ret == ESP_ERR_INVALID_ARG;
      }
      if (remove) {
        instance->removeWebSocketClient(frame->fd);
      }
    } while (xQueueReceive(instance->m_wsQueue, &raw_frame, 0) == pdPASS &&
             raw_frame != nullptr && raw_frame->fd != -1);
  }
}

esp_err_t WebServerManager::handleWebSocketMessage(httpd_req_t *req, const std::string &message) {
  JsonGuard json(cJSON_Parse(message.c_str()));
  if (!json) {
    std::string err_str = JsonBuilder::object()
        .addString("type", "error")
        .addString("message", "Invalid JSON format")
        .toStringUnformatted();
    queue_ws_frame(httpd_req_to_sockfd(req), (const uint8_t *)err_str.c_str(), err_str.size(), HTTPD_WS_TYPE_TEXT);
    return ESP_OK;
  }

  cJSON *type_item = cJSON_GetObjectItem(json.get(), "type");
  if (!type_item || !cJSON_IsString(type_item)) {
    return ESP_OK;
  }

  std::string msg_type = type_item->valuestring;
  int sockfd = httpd_req_to_sockfd(req);
  std::string response;

  if (msg_type == "ping") {
    response = JsonBuilder::object()
        .addString("type", "pong")
        .addNumber("timestamp", static_cast<uint32_t>(esp_timer_get_time() / 1000))
        .toStringUnformatted();
  } else if (msg_type == "metrics") {
    response = getDeviceMetrics();
  } else if (msg_type == "sysinfo") {
    response = getDeviceInfo();
  } else if (msg_type == "ota_info") {
    response = getOTAInfo();
  } else if (msg_type == "set_log_level") {  
    cJSON *level_item = cJSON_GetObjectItem(json.get(), "data");
    if(level_item && cJSON_IsNumber(level_item)) {
      esp_log_level_t level = esp_log_level_t(level_item->valueint >= 0 && level_item->valueint < 6 ? level_item->valueint : ESP_LOG_WARN);
      esp_log_level_set("*", level);
      loggable::Sinker::instance().set_level(level_item->valueint >= 0 && level_item->valueint < 6 ? (loggable::LogLevel)level_item->valueint : loggable::LogLevel::Warning);
      m_configManager.setNVSLogLevel(level);
    }
    response = getDeviceInfo();
  } else if (msg_type == "set_backlog_max_size") {
    cJSON *item = cJSON_GetObjectItem(json.get(), "data");
    if(item && cJSON_IsNumber(item)) {
      if(item->valueint >= 0 && item->valueint <= kMaxBacklogFrames){
        wsBacklogSize = item->valueint;
        m_configManager.setBacklogMaxSize(item->valueint);
      } else ESP_LOGE(TAG, "Number outside of range for 'set_backlog_max_size'");
    }
    response = getDeviceInfo();
  } else {
    response = JsonBuilder::object()
        .addString("type", "error")
        .addString("message", "Unknown message type")
        .addString("received_type", msg_type.c_str())
        .toStringUnformatted();
  }

  queue_ws_frame(sockfd, (const uint8_t *)response.c_str(), response.size(), HTTPD_WS_TYPE_TEXT);
  return ESP_OK;
}

// ============================================================================
// Device Info/Status Methods
// ============================================================================

std::string WebServerManager::getDeviceMetrics() {
  JsonBuilder status = JsonBuilder::object();
  status.addString("type", "metrics");
  status.addNumber("uptime", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<int64_t, std::micro>(esp_timer_get_time())).count());
  status.addNumber("free_heap", esp_get_free_heap_size());
  status.addNumber("wifi_rssi", WiFi.RSSI());
  status.addBool("nfc_connected", m_nfcManager ? m_nfcManager->isConnected() : false);
  status.addNumber("nfc_reader_type", m_configManager.getConfig<espConfig::misc_config_t>().nfcReaderType);
  status.addBool("mqtt_connected", m_mqttManager ? m_mqttManager->isConnected() : false);
  status.addNumber("mqtt_error_code", m_mqttManager ? static_cast<uint8_t>(m_mqttManager->getLastErrorCode()) : 0);
  status.addNumber("ws_frames_dropped", static_cast<uint64_t>(getWsFrameDropCount()));
  if (m_mqttManager && !m_mqttManager->getLastErrorMessage().empty()) {
    status.addString("mqtt_error_message", m_mqttManager->getLastErrorMessage().c_str());
  }
  return status.toStringUnformatted();
}

std::string WebServerManager::getDeviceInfo() {
  JsonBuilder info = JsonBuilder::object();
  info.addString("type", "sysinfo");
  info.addString("deviceName", m_configManager.getConfig<espConfig::misc_config_t>().deviceName.c_str());
  info.addString("version", esp_app_get_description()->version);
  info.addBool("eth_enabled", m_configManager.getConfig<espConfig::misc_config_t>().ethernetEnabled);
  info.addString("wifi_ssid", WiFi.SSID().c_str());
  info.addNumber("log_level", esp_log_level_get("*"));
  esp_chip_info_t chipInfo;
  esp_chip_info(&chipInfo);
  info.addNumber("chip_model", chipInfo.model);
  info.addNumber("backlog_max_size", wsBacklogSize);
  return info.toStringUnformatted();
}

void WebServerManager::statusTimerCallback(void *arg) {
  WebServerManager *instance = static_cast<WebServerManager *>(arg);
  const uint64_t dropped = instance->getWsFrameDropCount();
  if (dropped > instance->m_lastReportedWsFrameDropped) {
    ESP_LOGW(TAG, "Dropped %llu WebSocket frames (send queue full)",
             dropped - instance->m_lastReportedWsFrameDropped);
    instance->m_lastReportedWsFrameDropped = dropped;
  }
  auto metrics = instance->getDeviceMetrics();
  instance->broadcastWs((const uint8_t *)(metrics.c_str()), metrics.size(),
                        HTTPD_WS_TYPE_TEXT);
}

// ============================================================================
// OTA Implementation
// ============================================================================

esp_err_t WebServerManager::handleOTAUpload(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance->basicAuth(req)) {
    return sendAuthFailure(req);
  }
  
  if (req->content_len == 0) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"Invalid request\"}");
    return ESP_OK;
  }

  bool expected = false;
  if (!instance->m_otaInProgress.compare_exchange_strong(expected, true)) {
    httpd_resp_set_status(req, "409 Conflict");
    httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"OTA in progress\"}");
    return ESP_OK;
  }

  char *type = strrchr(req->uri, '/');
  OTAUploadType uploadType = (type && strncmp(type + 1, "littlefs", 8) == 0)
                                 ? OTAUploadType::LITTLEFS
                                 : OTAUploadType::FIRMWARE;

 auto app_part =  esp_ota_get_running_partition();
  if (uploadType == OTAUploadType::FIRMWARE && req->content_len > app_part->size) {
    ESP_LOGE(TAG, "OTA size %zu > max %zu", req->content_len, app_part->size);
    instance->m_otaInProgress = false;
    httpd_resp_set_status(req, "413 Payload Too Large");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"Firmware too large\"}");
    return ESP_OK;
  }
  auto fs_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "spiffs");
  if (uploadType == OTAUploadType::LITTLEFS) {
    if (!fs_part) {
      ESP_LOGE(TAG, "No LittleFS (spiffs) partition found");
      instance->m_otaInProgress = false;
      httpd_resp_set_status(req, "500 Internal Server Error");
      httpd_resp_set_type(req, "application/json");
      httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"No LittleFS partition\"}");
      return ESP_OK;
    }
    if (req->content_len > fs_part->size) {
      ESP_LOGE(TAG, "OTA size %zu > max %zu", req->content_len, fs_part->size);
      instance->m_otaInProgress = false;
      httpd_resp_set_status(req, "413 Payload Too Large");
      httpd_resp_set_type(req, "application/json");
      httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"LittleFS too large\"}");
      return ESP_OK;
    }
  }

  bool skipReboot = false;
  char query[256], param[32];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK &&
      httpd_query_key_value(query, "skipReboot", param, sizeof(param)) ==
          ESP_OK) {
    skipReboot = (strcmp(param, "true") == 0);
  }

  httpd_req_t *reqCopy = nullptr;
  if (httpd_req_async_handler_begin(req, &reqCopy) != ESP_OK) {
    instance->m_otaInProgress = false;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "500 Internal Server Error");
    httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"Failed to start OTA\"}");
    return ESP_OK;
  }

  OTAParams *params = new OTAParams{reqCopy, instance, uploadType, skipReboot, req->content_len, new OTAState()};
  params->state->inProgress = true;

BaseType_t task;
#ifndef CONFIG_FREERTOS_UNICORE
    task = xTaskCreatePinnedToCore(otaTask, "ota_task", 8192, params, 5, NULL, 1);
#else
    task = xTaskCreate(otaTask, "ota_task", 8192, params, 5, NULL);
#endif
  if (task != pdPASS) {
    ESP_LOGE(TAG, "Failed to create OTA task");
    delete params->state;
    delete params;
    httpd_resp_set_type(reqCopy, "application/json");
    httpd_resp_set_status(reqCopy, "500 Internal Server Error");
    httpd_resp_sendstr(reqCopy, "{\"success\":false,\"error\":\"Failed to create OTA task\"}");
    httpd_req_async_handler_complete(reqCopy);
    instance->m_otaInProgress = false;
    return ESP_FAIL; 
  }

  return ESP_OK;
}

void WebServerManager::otaTask(void *pvParameters) {
  OTAParams *params = static_cast<OTAParams *>(pvParameters);
  WebServerManager *instance = params->instance;
  httpd_req_t *req = params->req;
  
  params->state->currentUploadType = params->uploadType;
  params->state->skipReboot = params->skipReboot;
  params->state->totalBytes = params->contentLength;
  params->state->writtenBytes = 0;
  params->state->error.clear();

  params->state->handle = 0;
  params->state->updatePartition = nullptr;
  params->state->littlefsPartition = nullptr;

  ESP_LOGI(TAG, "Starting OTA task. Type: %d, Size: %zu", (int)params->uploadType, params->contentLength);

  const size_t buffer_size = 4096;
  char *buffer = (char *)heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  if (!buffer) {
    params->state->error = "Buffer allocation failed";
    goto error;
  }

  if (params->uploadType == OTAUploadType::FIRMWARE) {
    params->state->updatePartition = esp_ota_get_next_update_partition(NULL);
    if (!params->state->updatePartition) {
       params->state->error = "No OTA partition";
       goto error;
    }
    if (esp_ota_begin(params->state->updatePartition, params->contentLength, &params->state->handle) != ESP_OK) {
       params->state->error = "OTA begin failed";
       goto error;
    }
  } else {
    params->state->littlefsPartition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "spiffs");
    if (!params->state->littlefsPartition) {
      params->state->error = "No LittleFS partition";
      goto error;
    }
    if (params->contentLength > params->state->littlefsPartition->size) {
      params->state->error = "Image too large";
      goto error;
    }
    LittleFS.end();
    if (esp_partition_erase_range(params->state->littlefsPartition, 0, params->state->littlefsPartition->size) != ESP_OK) {
      params->state->error = "Erase failed";
      goto error;
    }
  }

  {
    size_t remaining = params->contentLength;
    size_t last_broadcast = 0;
    int received;
    while (remaining > 0) {
        received = httpd_req_recv(req, buffer, std::min(remaining, buffer_size));
        if (received < 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            params->state->error = "Receive error";
            goto error;
        }
        
        if (received > 0) {
            if (params->uploadType == OTAUploadType::FIRMWARE) {
                if (esp_ota_write(params->state->handle, buffer, received) != ESP_OK) {
                    params->state->error = "Write error";
                    goto error;
                }
            } else {
                if (esp_partition_write(params->state->littlefsPartition, params->state->writtenBytes, buffer, received) != ESP_OK) {
                    params->state->error = "Write error";
                    goto error;
                }
            }
            params->state->writtenBytes += received;
            remaining -= received;
            
            if ((params->state->writtenBytes - last_broadcast) >= std::max(params->contentLength / 20, (size_t)1) || remaining == 0) {
                instance->broadcastOTAStatus(*params->state);
                last_broadcast = params->state->writtenBytes;
            }
        } else {
            params->state->error = "Received empty payload, aborting";
            goto error;
        }
    }
  }

  if (params->uploadType == OTAUploadType::FIRMWARE) {
    if (esp_ota_end(params->state->handle) != ESP_OK || esp_ota_set_boot_partition(params->state->updatePartition) != ESP_OK) {
        params->state->error = "End/SetBoot failed";
        goto error;
    }
  } else if (params->uploadType == OTAUploadType::LITTLEFS) {
    if(!LittleFS.begin()) {
      ESP_LOGE(TAG, "Failed to remount LittleFS after OTA");
      params->state->error = "LittleFS remount failed after OTA";
      goto error;
    }
  }

  {
    bool shouldReboot = !params->skipReboot;
    params->state->inProgress = false;
    instance->broadcastOTAStatus(*params->state);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":true,\"message\":\"Update Complete\"}");
    httpd_req_async_handler_complete(req);
    instance->m_otaInProgress = false;

    if (buffer) free(buffer);
    delete params->state;
    delete params;

    if (shouldReboot) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_restart();
    }

    vTaskDelete(NULL);
    return;
  }

error:
  params->state->inProgress = false;
  instance->broadcastOTAStatus(*params->state);
  if (params->state->handle) esp_ota_abort(params->state->handle);
  if (buffer) free(buffer);
  
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_status(req, "500 Internal Server Error");
  std::string errJson = JsonBuilder::object()
      .addBool("success", false)
      .addString("error", params->state->error.c_str())
      .toStringUnformatted();
  httpd_resp_sendstr(req, errJson.c_str());
  httpd_req_async_handler_complete(req);
  instance->m_otaInProgress = false;
  delete params->state;
  delete params;
  vTaskDelete(NULL);
}

std::string WebServerManager::getOTAInfo() {
  JsonBuilder status = JsonBuilder::object();
  status.addString("type", "ota_info");
  
  status.addString("current_version",
                          esp_app_get_description()->version);

  const esp_partition_t *running = esp_ota_get_running_partition();
  const esp_partition_t *next_update = esp_ota_get_next_update_partition(NULL);
  if (running)
    status.addString("running_partition", running->label);
  if (next_update)
    status.addString("next_update_partition",
                            next_update->label);
  return status.toStringUnformatted();
}

void WebServerManager::broadcastOTAStatus(const OTAState& state) {
  JsonBuilder status = JsonBuilder::object();
  status.addString("type", "ota_status");
  if (!state.error.empty()) {
    status.addString("error", state.error.c_str());
  }
  status.addBool("in_progress", state.inProgress);
  status.addNumber("bytes_written", state.writtenBytes);
  status.addString("upload_type", (state.currentUploadType == OTAUploadType::LITTLEFS) ? "littlefs" : "firmware");

  if (state.inProgress && state.totalBytes > 0) {
    status.addNumber("progress_percent", (float)state.writtenBytes / state.totalBytes * 100.0f);
    status.addNumber("total_bytes", state.totalBytes);
  }
  
  std::string otaStatus = status.toStringUnformatted();
  broadcastWs((const uint8_t *)otaStatus.c_str(), otaStatus.size(), HTTPD_WS_TYPE_TEXT);
}

// ============================================================================
// Certificate Handlers
// ============================================================================

esp_err_t WebServerManager::handleCertificateUpload(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  char query[256], type_param[8];
  if(httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) != ESP_OK )) {
    return sendJsonError(req, "Missing 'type' parameter", "400 Bad Request");
  }
  
  char* end = nullptr;
  long type_int = strtol(type_param, &end, 10);
  if (end == type_param || *end != '\0' || type_int < 0 || type_int >= static_cast<long>(espConfig::CertType::MAX)) {
    return sendJsonError(req, "Invalid 'type' parameter", "400 Bad Request");
  }
  const espConfig::CertType type = static_cast<espConfig::CertType>(type_int);
  
  const size_t content_len = req->content_len;
  if (content_len == 0 || content_len > 8192) {
    return sendJsonError(req, "Invalid bundle content length", "400 Bad Request");
  }

  std::string certBuf;
  certBuf.reserve(content_len + 1);
  char buffer[1024];
  size_t remaining = content_len;

  while (remaining > 0) {
    size_t chunk_size = std::min(remaining, sizeof(buffer) - 1);
    int received = httpd_req_recv(req, buffer, chunk_size);
    if (received <= 0) {
      return sendJsonError(req, "Failed to receive bundle data", "400 Bad Request");
    }
    buffer[received] = '\0';
    certBuf.append(buffer, received);
    remaining -= received;
  }

  bool success = instance->m_configManager.saveCertificate(type, certBuf);

  if (success) {
    httpd_resp_set_type(req, "application/json");
    std::string response = JsonBuilder::object()
        .addBool("success", true)
        .addString("message", "Certificate saved successfully!")
        .addNumber("size", content_len)
        .toStringUnformatted();
    httpd_resp_send(req, response.c_str(), response.length());
    return ESP_OK;
  }

  return sendJsonError(req, "Failed to save certificate", HTTPD_500);
}

esp_err_t WebServerManager::handleCertificateStatus(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  JsonBuilder response = JsonBuilder::object();
  response.withObject("data", [&](JsonBuilder& certificates) {
    std::vector<CertificateStatus> status = instance->m_configManager.getCertificatesStatus();
    for (auto cert : status) {
      JsonBuilder certInfo = JsonBuilder::object();
      bool isPrivateKey = (cert.type == espConfig::CertType::MQTT_PRIVATE_KEY || cert.type == espConfig::CertType::HTTPS_PRIVATE_KEY);
      bool isCA = (cert.type == espConfig::CertType::MQTT_CA || cert.type == espConfig::CertType::HTTPS_CA_CERT);

      if (!isPrivateKey) {
        if (!cert.issuer.empty()) certInfo.addString("issuer", cert.issuer.c_str());
        if (!cert.subject.empty()) certInfo.addString("subject", cert.subject.c_str());
        if (!cert.serial.empty()) certInfo.addString("serial", cert.serial.c_str());
        if (!cert.fingerprint.empty()) certInfo.addString("fingerprint", cert.fingerprint.c_str());
        if (!cert.expiration.from.empty() && !cert.expiration.to.empty()) {
          certInfo.withObject("expiration", [&](JsonBuilder& exp) {
            exp.addString("from", cert.expiration.from.c_str());
            exp.addString("to", cert.expiration.to.c_str());
          });
        }
        if(!isCA){
          certInfo.addBool("keyMatchesCert", cert.keyMatchesCert);
        }
      } else {
        certInfo.addBool("exists", true);
        certInfo.addString("keyType", cert.keyType.c_str());
      }
      certificates.addItem(std::to_string(static_cast<uint8_t>(cert.type)).c_str(), std::move(certInfo).release());
    }
  });
  response.addBool("success", true);

  std::string resp = response.toStringUnformatted();
  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());
  return ESP_OK;
}


esp_err_t WebServerManager::handleCertificateDelete(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    return sendAuthFailure(req);
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  
  char query[256], type_param[8];
  if(httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) != ESP_OK )) {
    return sendJsonError(req, "Missing 'type' parameter", "400 Bad Request");
  }
  
  char* end = nullptr;
  long type_int = strtol(type_param, &end, 10);
  if (end == type_param || *end != '\0' || type_int < 0 || type_int >= static_cast<long>(espConfig::CertType::MAX)) {
    return sendJsonError(req, "Invalid 'type' parameter", "400 Bad Request");
  }
  const espConfig::CertType type = static_cast<espConfig::CertType>(type_int);

  bool success = instance->m_configManager.deleteCertificate(type);

  if (success) {
    httpd_resp_set_type(req, "application/json");
    std::string response = JsonBuilder::object()
        .addBool("success", true)
        .addString("message", "Certificate deleted")
        .toStringUnformatted();
    httpd_resp_send(req, response.c_str(), response.length());
    return ESP_OK;
  }

  return sendJsonError(req, "Failed to delete certificate", HTTPD_500);
}
