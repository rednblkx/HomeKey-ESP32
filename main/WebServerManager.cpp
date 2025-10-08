#include "WebServerManager.hpp"
#include "ConfigManager.hpp"
#include "HomeSpan.h"
#include "MqttManager.hpp"
#include "ReaderDataManager.hpp"
#include "cJSON.h"
#include "config.hpp"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_http_server.h"
#include "esp_ota_ops.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "eth_structs.hpp"
#include "eventStructs.hpp"
#include "fmt/base.h"
#include "fmt/ranges.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <LittleFS.h>
#include <dirent.h>
#include <esp_app_desc.h>
#include <event_manager.hpp>
#include <string>
#include <unordered_map>
#include <vector>

const char *WebServerManager::TAG = "WebServerManager";

// WebSocket payload size limit
const size_t MAX_WS_PAYLOAD = 8192; // 8KB max payload

// Max OTA size
const size_t MAX_OTA_SIZE = 0x1E0000;

// Forward declaration
static void mergeJson(cJSON *target, cJSON *source);


/**
 * @brief Checks whether a C string ends with the given suffix.
 *
 * Compares the trailing characters of `str` against `suffix`.
 *
 * @param str Null-terminated string to test. If nullptr, treated as not matching.
 * @param suffix Null-terminated suffix to look for. If nullptr, treated as not matching.
 * @return true if `str` ends with `suffix`, false otherwise.
 */
static inline bool str_ends_with(const char *str, const char *suffix) {
  if (!str || !suffix)
    return false;
  const size_t lenstr = strlen(str);
  const size_t lensuf = strlen(suffix);
  return (lenstr >= lensuf) &&
         (memcmp(str + lenstr - lensuf, suffix, lensuf) == 0);
}

/**
 * @brief Serialize a cJSON value to a std::string and release the cJSON object.
 *
 * Converts the provided cJSON structure to a compact JSON string and frees all
 * resources associated with the cJSON object.
 *
 * @param obj Pointer to the cJSON object to serialize; ownership is consumed and
 *            the object will be deleted by this function.
 * @return std::string The serialized JSON text, or an empty string if `obj` is null
 *                     or serialization fails.
 */
static inline std::string cjson_to_string_and_free(cJSON *obj) {
  if (!obj)
    return {};
  char *raw = cJSON_PrintUnformatted(obj);
  std::string out = raw ? std::string(raw) : std::string{};
  if (raw)
    free(raw);
  cJSON_Delete(obj);
  return out;
}

/**
 * @brief Construct a WebServerManager and register required event publishers.
 *
 * Initializes the WebServerManager with references to the configuration and
 * reader-data managers and registers the "homekit/event" and
 * "hardware/gpioPinChanged" event publishers with the global EventManager.
 *
 * @param configManager Reference to the ConfigManager used for persistent config.
 * @param readerDataManager Reference to the ReaderDataManager that provides reader-related identifiers and data.
 */
WebServerManager::WebServerManager(ConfigManager &configManager,
                                   ReaderDataManager &readerDataManager)
    : m_server(nullptr), m_configManager(configManager),
      m_readerDataManager(readerDataManager), m_mqttManager(nullptr) {
  espp::EventManager::get().add_publisher("homekit/event", "WebServerManager");
  espp::EventManager::get().add_publisher("hardware/gpioPinChanged",
                                          "WebServerManager");
}

/**
 * @brief Cleanly shuts down the web server manager and releases its resources.
 *
 * Performs overall cleanup for the WebServerManager instance: finalizes any
 * pending OTA worker state, stops and clears the HTTP server, stops and
 * deletes the status timer, and deletes the dedicated WebSocket broadcast
 * queue when present.
 */
WebServerManager::~WebServerManager() {
  ESP_LOGI(TAG, "WebServerManager destructor called");

  // Clean up OTA resources
  cleanupOTAAsync();

  // Stop the web server
  if (m_server) {
    httpd_stop(m_server);
    m_server = nullptr;
  }

  // Stop status timer
  if (m_statusTimer) {
    esp_timer_stop(m_statusTimer);
    esp_timer_delete(m_statusTimer);
    m_statusTimer = nullptr;
  }
#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
  if (m_wsBroadcastQueue) {
    vQueueDelete(m_wsBroadcastQueue);
    m_wsBroadcastQueue = nullptr;
  }
#endif
}

/**
 * @brief Initializes and starts the web server, WebSocket infrastructure, OTA worker, and status timer.
 *
 * Performs the full startup sequence required for HTTP/WebSocket operation:
 * mounts LittleFS, starts the HTTP server, creates WebSocket queues and the send task, registers HTTP/WebSocket routes,
 * initializes the OTA worker, and creates a periodic status timer.
 *
 * On critical failures (e.g., LittleFS mount, HTTP server start, queue or task creation) the method aborts startup,
 * stops any partially started services and returns without leaving the server running. Failure to create the status
 * timer is treated as non-fatal and startup continues.
 */
void WebServerManager::begin() {
  ESP_LOGI(TAG, "Initializing...");

  // Step 1: Mount LittleFS with minimal logging
  ESP_LOGI(TAG, "Mounting LittleFS...");
  if (!LittleFS.begin()) {
    ESP_LOGE(TAG, "Failed to mount LittleFS. Web server cannot start.");
    return;
  }

  ESP_LOGI(TAG, "LittleFS mounted successfully");

  ESP_LOGI(TAG, "Filesystem usage: %d/%d bytes", LittleFS.usedBytes(), LittleFS.totalBytes());

  // List all files present on LittleFS after initialization
  ESP_LOGI(TAG, "Listing all files on LittleFS:");
  DIR *dir = opendir("/littlefs");
  if (dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) { // regular file
        struct stat st;
        std::string path = "/littlefs/" + std::string(entry->d_name);
        if (stat(path.c_str(), &st) == 0) {
          ESP_LOGI(TAG, "File: %s, size: %ld bytes", entry->d_name, st.st_size);
        }
      } else if (entry->d_type == DT_DIR) { // directory
        if (strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
          ESP_LOGI(TAG, "Directory: %s", entry->d_name);
        }
      }
    }
    closedir(dir);
  } else {
    ESP_LOGE(TAG, "Failed to open directory for listing");
  }

  // Step 2: Create HTTP server with minimal configuration
  ESP_LOGI(TAG, "Starting HTTP server...");
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  config.max_uri_handlers = 20;
  config.max_resp_headers = 8;
  config.max_open_sockets = 8;
  config.stack_size = 8192;
  config.uri_match_fn = httpd_uri_match_wildcard;
  config.recv_wait_timeout = 10;
  config.send_wait_timeout = 10;
  config.lru_purge_enable = true;

  esp_err_t ret = httpd_start(&m_server, &config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error starting server: %s",
             esp_err_to_name(ret));
    return;
  }
  ESP_LOGI(TAG, "HTTP server started on port %d",
           config.server_port);

  // Step 3: Create WebSocket infrastructure
  ESP_LOGI(TAG, "Creating WebSocket infrastructure...");
  m_wsQueue = xQueueCreate(10, sizeof(WsFrame *));
  if (!m_wsQueue) {
    ESP_LOGE(TAG, "Failed to create WebSocket queue");
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }

#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
  m_wsBroadcastQueue = xQueueCreate(8, sizeof(BroadcastMsg *));
  if (!m_wsBroadcastQueue) {
    ESP_LOGE(TAG, "Failed to create WebSocket broadcast queue");
    vQueueDelete(m_wsQueue);
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }
#endif

  BaseType_t task_result =
      xTaskCreate(ws_send_task, "ws_send_task", 4096, this, 5, &m_wsTaskHandle);
  if (task_result != pdPASS) {
    ESP_LOGE(TAG, "Failed to create WebSocket send task");
#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
    if (m_wsBroadcastQueue)
      vQueueDelete(m_wsBroadcastQueue);
#endif
    vQueueDelete(m_wsQueue);
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }

  setupRoutes();

  ESP_LOGI(TAG, "Creating status timer...");
  esp_timer_create_args_t statusTimerArgs = {
      .callback = &WebServerManager::statusTimerCallback,
      .arg = this,
      .name = "statusTimer"};
  esp_err_t timer_ret = esp_timer_create(&statusTimerArgs, &m_statusTimer);
  if (timer_ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to create status timer: %s",
             esp_err_to_name(timer_ret));
    // Continue without timer - not critical
  }

  ESP_LOGI(TAG, "Web server initialization complete");
}

/**
 * @brief Retrieve the WebServerManager instance associated with an HTTP request.
 *
 * Extracts and returns the pointer stored in the request's `user_ctx`.
 *
 * @param req HTTP request whose `user_ctx` is expected to hold a `WebServerManager*`.
 * @return WebServerManager* Pointer from `req->user_ctx`, or `nullptr` if none is set.
 */
WebServerManager *WebServerManager::getInstance(httpd_req_t *req) {
  return static_cast<WebServerManager *>(req->user_ctx);
}

/**
 * @brief Register all HTTP and WebSocket routes used by the web server.
 *
 * Sets up handlers for static assets, the root page, API endpoints (config, ethernet,
 * reboot, resets, start AP), the WebSocket endpoint (when enabled), and OTA upload endpoints.
 */
void WebServerManager::setupRoutes() {
  ESP_LOGI(TAG, "Setting up routes...");
  // Static file handlers
  httpd_uri_t static_uri = {.uri = "/static/*",
                            .method = HTTP_GET,
                            .handler = handleStaticFiles,
                            .user_ctx = (void *)"/static",
                            .is_websocket = false,
                            .handle_ws_control_frames = false};
  httpd_register_uri_handler(m_server, &static_uri);

  httpd_uri_t assets_uri = {.uri = "/_app/*",
                            .method = HTTP_GET,
                            .handler = handleStaticFiles,
                            .user_ctx = (void *)"/_app",
                            .is_websocket = false,
                            .handle_ws_control_frames = false};
  httpd_register_uri_handler(m_server, &assets_uri);

  // API endpoints
  httpd_uri_t config_get_uri = {.uri = "/config",
                                .method = HTTP_GET,
                                .handler = handleGetConfig,
                                .user_ctx = this};
  httpd_register_uri_handler(m_server, &config_get_uri);

  httpd_uri_t config_clear_uri = {.uri = "/config/clear",
                                  .method = HTTP_POST,
                                  .handler = handleClearConfig,
                                  .user_ctx = this};
  httpd_register_uri_handler(m_server, &config_clear_uri);

  httpd_uri_t config_save_uri = {.uri = "/config/save",
                                 .method = HTTP_POST,
                                 .handler = handleSaveConfig,
                                 .user_ctx = this};
  httpd_register_uri_handler(m_server, &config_save_uri);

  httpd_uri_t eth_config_uri = {.uri = "/eth_get_config",
                                .method = HTTP_GET,
                                .handler = handleGetEthConfig,
                                .user_ctx = this};
  httpd_register_uri_handler(m_server, &eth_config_uri);

  httpd_uri_t reboot_uri = {.uri = "/reboot_device",
                            .method = HTTP_POST,
                            .handler = handleReboot,
                            .user_ctx = this};
  httpd_register_uri_handler(m_server, &reboot_uri);

  httpd_uri_t hk_reset_uri = {.uri = "/reset_hk_pair",
                              .method = HTTP_GET,
                              .handler = handleHKReset,
                              .user_ctx = this};
  httpd_register_uri_handler(m_server, &hk_reset_uri);

  httpd_uri_t wifi_reset_uri = {.uri = "/reset_wifi_cred",
                                .method = HTTP_GET,
                                .handler = handleWifiReset,
                                .user_ctx = this};
  httpd_register_uri_handler(m_server, &wifi_reset_uri);

  httpd_uri_t config_ap_uri = {.uri = "/start_config_ap",
                               .method = HTTP_GET,
                               .handler = handleStartConfigAP,
                               .user_ctx = this};
  httpd_register_uri_handler(m_server, &config_ap_uri);

  // WebSocket endpoint
  httpd_uri_t ws_uri = {
      .uri = "/ws",
      .method = HTTP_GET,
      .handler = handleWebSocket,
      .user_ctx = this,
  };
#ifdef CONFIG_HTTPD_WS_SUPPORT
  ws_uri.is_websocket = true;
#endif
  httpd_register_uri_handler(m_server, &ws_uri);

  // OTA endpoints
  httpd_uri_t ota_upload_uri = {.uri = "/ota/upload",
                                .method = HTTP_POST,
                                .handler = handleOTAUpload,
                                .user_ctx = this,
                                .is_websocket = false};
  httpd_register_uri_handler(m_server, &ota_upload_uri);

  httpd_uri_t ota_littlefs_uri = {
      .uri = "/ota/littlefs",
      .method = HTTP_POST,
      .handler = handleOTAUpload, // Same handler, different endpoint
      .user_ctx = this,
      .is_websocket = false};
  httpd_register_uri_handler(m_server, &ota_littlefs_uri);

  // Certificate endpoints
  httpd_uri_t cert_upload_uri = {.uri = "/certificates/upload",
                                 .method = HTTP_POST,
                                 .handler = handleCertificateUpload,
                                 .user_ctx = this,
                                 .is_websocket = false};
  httpd_register_uri_handler(m_server, &cert_upload_uri);

  httpd_uri_t cert_status_uri = {.uri = "/certificates/status",
                                 .method = HTTP_GET,
                                 .handler = handleCertificateStatus,
                                 .user_ctx = this,
                                 .is_websocket = false};
  httpd_register_uri_handler(m_server, &cert_status_uri);

  httpd_uri_t cert_delete_uri = {.uri = "/certificates/*",
                                 .method = HTTP_DELETE,
                                 .handler = handleCertificateDelete,
                                 .user_ctx = this,
                                 .is_websocket = false};
  httpd_register_uri_handler(m_server, &cert_delete_uri);
  // Main web routes
  httpd_uri_t root_uri = {.uri = "/*",
                          .method = HTTP_GET,
                          .handler = handleRootOrHash,
                          .user_ctx = this};
  httpd_register_uri_handler(m_server, &root_uri);
}

/**
 * @brief Serves static files from LittleFS based on the request URI.
 *
 * Determines the filesystem path using req->user_ctx as the base path and the
 * request URI, sets an appropriate Content-Type, and streams the file to the
 * client. For `.js` and `.css` files it will prefer a gzip-compressed variant
 * (`.gz`) when the client advertises gzip support and will set
 * `Content-Encoding: gzip` when serving compressed content. If the file is not
 * found or cannot be opened, a 404 response is sent.
 *
 * @param req HTTP request whose `user_ctx` is treated as the base filesystem path
 *            and whose `uri` identifies the requested file.
 * @return esp_err_t `ESP_OK` if the file was successfully streamed; `ESP_FAIL`
 *         if the file was not found, could not be opened, or a send error
 *         occurred (a 404 response is sent when the file is missing or cannot
 *         be opened).
 */
esp_err_t WebServerManager::handleStaticFiles(httpd_req_t *req) {
  const char *base_path = (const char *)req->user_ctx;
  std::string filepath;
  std::string compressed_filepath;

  // Get the file path from URI
  const char *filename = req->uri + strlen(base_path);
  if (strlen(filename) == 0) {
    filename = "/index.html";
  }

  filepath = std::string(base_path) + filename;

  // Check for compressed version first for .js and .css files
  bool use_compressed = false;
  std::string actual_filepath = filepath;

  if (str_ends_with(filename, ".js") || str_ends_with(filename, ".css")) {
    // Check if client accepts gzip encoding
    size_t accept_len = httpd_req_get_hdr_value_len(req, "Accept-Encoding");
    if (accept_len > 0) {
      // Cap maximum header size to prevent large allocations
      const size_t cap = accept_len > 512 ? 512 : accept_len;
      std::vector<char> hdr(cap + 1, 0);
      if (httpd_req_get_hdr_value_str(req, "Accept-Encoding", hdr.data(),
                                      hdr.size()) == ESP_OK) {
        std::string accept_encoding(hdr.data());
        if (accept_encoding.find("gzip") != std::string::npos) {
          compressed_filepath = filepath + ".gz";
          if (LittleFS.exists(compressed_filepath.c_str())) {
            use_compressed = true;
            actual_filepath = compressed_filepath;
          }
        }
      }
    }
  }

  // Check if file exists
  if (!LittleFS.exists(actual_filepath.c_str())) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  File file = LittleFS.open(actual_filepath.c_str(), "r");
  if (!file) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  // Determine content type based on original filename (not compressed)
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

  // Set compression header if using compressed file
  if (use_compressed) {
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  }

  char buffer[1024];
  size_t bytes_read;
  while ((bytes_read = file.read((uint8_t *)buffer, sizeof(buffer))) > 0) {
    if (httpd_resp_send_chunk(req, buffer, bytes_read) != ESP_OK) {
      file.close();
      return ESP_FAIL;
    }
  }

  file.close();
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

/**
 * @brief Send a WebSocket frame to a connected client.
 *
 * @param server HTTP server handle associated with the connection.
 * @param fd File descriptor identifying the WebSocket client.
 * @param payload Pointer to the frame payload bytes.
 * @param len Length of the payload in bytes.
 * @param type WebSocket frame type (text, binary, close, ping, pong).
 * @return esp_err_t `ESP_OK` on success, or an ESP error code; `ESP_ERR_NOT_SUPPORTED` if WebSocket support is disabled.
 */
esp_err_t WebServerManager::ws_send_frame(httpd_handle_t server, int fd,
                                          const uint8_t *payload, size_t len,
                                          httpd_ws_type_t type) {
#ifdef CONFIG_HTTPD_WS_SUPPORT
  httpd_ws_frame_t ws_pkt = {};
  ws_pkt.final = true;
  ws_pkt.fragmented = false;
  ws_pkt.type = type;
  ws_pkt.payload = const_cast<uint8_t *>(payload);
  ws_pkt.len = len;
  return httpd_ws_send_frame_async(server, fd, &ws_pkt);
#else
  return ESP_ERR_NOT_SUPPORTED;
#endif
}

/**
 * @brief Handle WebSocket connections and incoming WebSocket frames for the server.
 *
 * Processes the WebSocket handshake (registers new clients and queues initial
 * device info), receives and validates incoming frames, dispatches text frames
 * to the message handler, and removes clients on terminal protocol or
 * connection errors.
 *
 * @param req HTTPD request context representing the WebSocket connection.
 * @return esp_err_t ESP_OK on successful handling of the request; an ESP error
 * code on failure (e.g., network/protocol errors encountered while receiving
 * frames).
 */
esp_err_t WebServerManager::handleWebSocket(httpd_req_t *req) {
#ifndef CONFIG_HTTPD_WS_SUPPORT
  httpd_resp_set_status(req, "501 Not Implemented");
  httpd_resp_send(req, "WebSocket not enabled in HTTPD", HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
#else
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    ESP_LOGE(TAG, "Failed to get WebServerManager instance");
    return ESP_FAIL;
  }

  if (req->method == HTTP_GET) {
    // This is the WebSocket handshake, add the client
    int sockfd = httpd_req_to_sockfd(req);
    instance->addWebSocketClient(sockfd);
    ESP_LOGI(TAG, "WebSocket client connected: fd=%d", sockfd);

    std::string status = instance->getDeviceInfo();
    instance->queue_ws_frame(sockfd, (const uint8_t *)status.c_str(),
                             status.size(), HTTPD_WS_TYPE_TEXT);
    // Do not send frames directly from the HTTP handler context
    if (!esp_timer_is_active(instance->m_statusTimer))
      esp_timer_start_periodic(instance->m_statusTimer, 5000 * 1000);

    return ESP_OK;
  }

  httpd_ws_frame_t ws_pkt;
  memset(&ws_pkt, 0, sizeof(ws_pkt));
  ws_pkt.type = HTTPD_WS_TYPE_TEXT;

  // First get frame length
  esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "ws recv frame len failed: %s (errno: %d)",
             esp_err_to_name(ret), errno);
    // Remove client on error - but be more careful about the error type
    int sockfd = httpd_req_to_sockfd(req);

    // Check for specific WebSocket protocol errors that indicate broken
    // connection
    bool is_protocol_error = false;

    // Check if the error message contains "masked" or "masking" (case
    // insensitive)
    const char *err_name = esp_err_to_name(ret);
    if (err_name &&
        (strstr(err_name, "masked") || strstr(err_name, "masking") ||
         strstr(err_name, "MASKED") || strstr(err_name, "MASKING"))) {
      is_protocol_error = true;
      ESP_LOGW(TAG,
               "WebSocket protocol error (frame masking issue) with client "
               "fd=%d, removing client",
               sockfd);
    }

    // Also check errno for connection reset/broken pipe
    if (errno == ECONNRESET || errno == EPIPE || errno == ENOTCONN) {
      is_protocol_error = true;
      ESP_LOGW(TAG,
               "Connection reset/broken with client fd=%d (errno: %d), "
               "removing client",
               sockfd, errno);
    }

    // Remove client on serious errors, including protocol errors and connection
    // issues
    if (ret == ESP_FAIL || ret == ESP_ERR_INVALID_STATE ||
        ret == ESP_ERR_INVALID_ARG || is_protocol_error) {
      instance->removeWebSocketClient(sockfd);
    } else {
      ESP_LOGW(TAG, "Temporary network issue with client fd=%d, keeping client",
               sockfd);
    }
    return ret;
  }

  // Validate payload size
  if (ws_pkt.len > MAX_WS_PAYLOAD) {
    ESP_LOGE(TAG, "WebSocket payload too large: %zu bytes (max: %zu)",
             ws_pkt.len, MAX_WS_PAYLOAD);
    int sockfd = httpd_req_to_sockfd(req);
    instance->removeWebSocketClient(sockfd);
    return ESP_FAIL;
  }

  std::string payload;
  if (ws_pkt.len) {
    payload.resize(ws_pkt.len);
    ws_pkt.payload = (uint8_t *)payload.data();
    ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "ws recv frame failed: %s (errno: %d)",
               esp_err_to_name(ret), errno);
      // Remove client on error - but be more careful about the error type
      int sockfd = httpd_req_to_sockfd(req);

      // Check for specific WebSocket protocol errors that indicate broken
      // connection
      bool is_protocol_error = false;

      // Check if the error message contains "masked" or "masking" (case
      // insensitive)
      const char *err_name = esp_err_to_name(ret);
      if (err_name &&
          (strstr(err_name, "masked") || strstr(err_name, "masking") ||
           strstr(err_name, "MASKED") || strstr(err_name, "MASKING"))) {
        is_protocol_error = true;
        ESP_LOGW(TAG,
                 "WebSocket protocol error (frame masking issue) with client "
                 "fd=%d during frame recv, removing client",
                 sockfd);
      }

      // Also check errno for connection reset/broken pipe
      if (errno == ECONNRESET || errno == EPIPE || errno == ENOTCONN) {
        is_protocol_error = true;
        ESP_LOGW(TAG,
                 "Connection reset/broken with client fd=%d (errno: %d) during "
                 "frame recv, removing client",
                 sockfd, errno);
      }

      // Remove client on serious errors, including protocol errors and
      // connection issues
      if (ret == ESP_FAIL || ret == ESP_ERR_INVALID_STATE ||
          ret == ESP_ERR_INVALID_ARG || is_protocol_error) {
        instance->removeWebSocketClient(sockfd);
      } else {
        ESP_LOGW(TAG,
                 "Temporary network issue with client fd=%d during frame recv, "
                 "keeping client",
                 sockfd);
      }
      return ret;
    }
  }

  // Handle different frame types
  if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {
    ESP_LOGD(TAG, "Received WebSocket message: %s", payload.c_str());
    return instance->handleWebSocketMessage(req, payload);
  } else if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
    ESP_LOGD(TAG, "WebSocket close frame received");
    int sockfd = httpd_req_to_sockfd(req);
    instance->removeWebSocketClient(sockfd);
    return ESP_OK;
  }

  return ESP_OK;
#endif
}

/**
 * @brief Serves the site's index page by streaming /index.html from LittleFS.
 *
 * Attempts to open /index.html from LittleFS and streams its contents to the HTTP client in chunks; if the file is missing or a send error occurs, responds with a 404 and fails.
 *
 * @param req HTTP request context used to send the response.
 * @return esp_err_t `ESP_OK` if the file was streamed and the response completed, `ESP_FAIL` if the file could not be opened or a chunk send failed.
 */
esp_err_t WebServerManager::handleRootOrHash(httpd_req_t *req) {
  File file = LittleFS.open("/app.html", "r");
  if (!file) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "text/html");

  char buffer[1024];
  size_t bytes_read;
  while ((bytes_read = file.read((uint8_t *)buffer, sizeof(buffer))) > 0) {
    if (httpd_resp_send_chunk(req, buffer, bytes_read) != ESP_OK) {
      file.close();
      return ESP_FAIL;
    }
  }

  file.close();
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

/**
 * @brief Handle incoming /config HTTP requests and return the requested configuration as JSON.
 *
 * Supports query parameter `type` with the following values:
 * - `"mqtt"`: returns MQTT configuration.
 * - `"misc"` or `"actions"`: returns misc/actions configuration.
 * - `"hkinfo"`: returns HomeKit-related reader information (group_identifier, unique_identifier, issuers with endpoints).
 *
 * If `type` is missing or invalid, the handler sends a 400 response with an explanatory message. On internal failures it sends a 500 response.
 *
 * @param req HTTP request object; must include a URL query string containing a `type` parameter.
 * @return esp_err_t `ESP_OK` if a JSON response was successfully sent, `ESP_FAIL` if an error response was sent. */
esp_err_t WebServerManager::handleGetConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  // Parse query parameters
  char query[256];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  char type_param[64];
  if (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) !=
      ESP_OK) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string type = type_param;
  std::string responseJson;

  if (type == "mqtt") {
    responseJson =
        instance->m_configManager.serializeToJson<espConfig::mqttConfig_t>();
  } else if (type == "misc" || type == "actions") {
    responseJson =
        instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
  } else if (type == "hkinfo") {
    const auto &readerData = instance->m_readerDataManager.getReaderData();
    cJSON *hkInfo = cJSON_CreateObject();
    cJSON_AddStringToObject(
        hkInfo, "group_identifier",
        fmt::format("{:02X}", fmt::join(readerData.reader_gid, "")).c_str());
    cJSON_AddStringToObject(
        hkInfo, "unique_identifier",
        fmt::format("{:02X}", fmt::join(readerData.reader_id, "")).c_str());
    cJSON *issuersArray = cJSON_CreateArray();
    for (const auto &issuer : readerData.issuers) {
      cJSON *issuerJson = cJSON_CreateObject();
      cJSON_AddStringToObject(
          issuerJson, "issuerId",
          fmt::format("{:02X}", fmt::join(issuer.issuer_id, "")).c_str());
      cJSON *endpointsArray = cJSON_CreateArray();
      for (const auto &endpoint : issuer.endpoints) {
        cJSON *ep = cJSON_CreateObject();
        cJSON_AddStringToObject(
            ep, "endpointId",
            fmt::format("{:02X}", fmt::join(endpoint.endpoint_id, "")).c_str());
        cJSON_AddItemToArray(endpointsArray, ep);
      }
      cJSON_AddItemToObject(issuerJson, "endpoints", endpointsArray);
      cJSON_AddItemToArray(issuersArray, issuerJson);
    }
    cJSON_AddItemToObject(hkInfo, "issuers", issuersArray);
    char *response_str = cJSON_PrintUnformatted(hkInfo);
    responseJson = response_str;
    free(response_str);
    cJSON_Delete(hkInfo);
  } else {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Invalid 'type' parameter", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, responseJson.c_str(), responseJson.length());
  return ESP_OK;
}

/**
 * @brief Builds and sends the device's Ethernet capabilities and board presets as JSON.
 *
 * The response contains an object with keys:
 *  - `supportedChips`: array of supported Ethernet chip descriptors (`name`, `emac`, `phy_type`),
 *  - `boardPresets`: array of board preset descriptors (each includes `name`, `ethChip`, optional `rmii_conf` when compiled with ESP32 EMAC, and `spi_conf`),
 *  - `ethEnabled`: boolean indicating whether Ethernet is enabled in the current misc config.
 *
 * @param req HTTP request context used to write the JSON response.
 * @return esp_err_t `ESP_OK` if the JSON payload was generated and sent successfully, `ESP_FAIL` if the server instance could not be retrieved or an internal error occurred.
 */
esp_err_t WebServerManager::handleGetEthConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  cJSON *eth_config = cJSON_CreateObject();
  cJSON *chipsArray = cJSON_CreateArray();
  for (auto &&v : eth_config_ns::supportedChips) {
    cJSON *chip = cJSON_CreateObject();
    cJSON_AddStringToObject(chip, "name", v.second.name.c_str());
    cJSON_AddBoolToObject(chip, "emac", v.second.emac);
    cJSON_AddNumberToObject(chip, "phy_type", v.second.phy_type);
    cJSON_AddItemToArray(chipsArray, chip);
  }
  cJSON_AddItemToObject(eth_config, "supportedChips", chipsArray);

  cJSON *boardPresetsArray = cJSON_CreateArray();
  for (auto &&v : eth_config_ns::boardPresets) {
    cJSON *preset = cJSON_CreateObject();
    cJSON_AddStringToObject(preset, "name", v.name.c_str());
    cJSON *chip = cJSON_CreateObject();
    cJSON_AddStringToObject(chip, "name", v.ethChip.name.c_str());
    cJSON_AddBoolToObject(chip, "emac", v.ethChip.emac);
    cJSON_AddNumberToObject(chip, "phy_type", v.ethChip.phy_type);
    cJSON_AddItemToObject(preset, "ethChip", chip);
#if CONFIG_ETH_USE_ESP32_EMAC
    cJSON *rmii_conf = cJSON_CreateObject();
    cJSON_AddNumberToObject(rmii_conf, "phy_addr", v.rmii_conf.phy_addr);
    cJSON_AddNumberToObject(rmii_conf, "pin_mcd", v.rmii_conf.pin_mcd);
    cJSON_AddNumberToObject(rmii_conf, "pin_mdio", v.rmii_conf.pin_mdio);
    cJSON_AddNumberToObject(rmii_conf, "pin_power", v.rmii_conf.pin_power);
    cJSON_AddNumberToObject(rmii_conf, "pin_rmii_clock",
                            v.rmii_conf.pin_rmii_clock);
    cJSON_AddItemToObject(preset, "rmii_conf", rmii_conf);
#endif
    cJSON *spi_conf = cJSON_CreateObject();
    cJSON_AddNumberToObject(spi_conf, "spi_freq_mhz", v.spi_conf.spi_freq_mhz);
    cJSON_AddNumberToObject(spi_conf, "pin_cs", v.spi_conf.pin_cs);
    cJSON_AddNumberToObject(spi_conf, "pin_irq", v.spi_conf.pin_irq);
    cJSON_AddNumberToObject(spi_conf, "pin_rst", v.spi_conf.pin_rst);
    cJSON_AddNumberToObject(spi_conf, "pin_sck", v.spi_conf.pin_sck);
    cJSON_AddNumberToObject(spi_conf, "pin_miso", v.spi_conf.pin_miso);
    cJSON_AddNumberToObject(spi_conf, "pin_mosi", v.spi_conf.pin_mosi);
    cJSON_AddItemToObject(preset, "spi_conf", spi_conf);
    cJSON_AddItemToArray(boardPresetsArray, preset);
  }
  cJSON_AddItemToObject(eth_config, "boardPresets", boardPresetsArray);
  cJSON_AddBoolToObject(
      eth_config, "ethEnabled",
      instance->m_configManager.getConfig<espConfig::misc_config_t>()
          .ethernetEnabled);
  char *payload = cJSON_PrintUnformatted(eth_config);
  cJSON_Delete(eth_config);

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, payload, strlen(payload));
  free(payload);
  return ESP_OK;
}

/**
 * @brief Handle HTTP request to save configuration changes.
 *
 * Validates and applies JSON configuration changes specified in the request
 * body for the configuration type indicated by the "type" query parameter
 * ("mqtt", "misc", or "actions"). On successful validation the handler merges
 * incoming values into the current configuration, persists the updated config,
 * publishes any relevant events (for HomeKit or GPIO changes), and may schedule
 * a device reboot if required by the applied changes.
 *
 * The handler sends appropriate HTTP responses for missing/invalid parameters or
 * payloads and for internal failures, and may trigger a soft restart when a
 * reboot is required to apply changes.
 *
 * @param req HTTP request containing the "type" query parameter and JSON body.
 * @return esp_err_t ESP_OK on successful save (and after any scheduled reboot
 *         has been initiated), ESP_FAIL on validation or processing failure.
 */
esp_err_t WebServerManager::handleSaveConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  // Parse query parameters for type
  char query[256];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  char type_param[64];
  if (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) !=
      ESP_OK) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  // Read request body
  const size_t max_content_size = 2048;
  if (req->content_len >= max_content_size) {
    httpd_resp_set_status(req, "413 Payload Too Large");
    httpd_resp_send(req, "Request body too large", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::vector<char> content(max_content_size, 0);
  int ret = httpd_req_recv(req, content.data(), content.size() - 1);
  if (ret <= 0) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Invalid request body", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }
  content[ret] = '\0';

  cJSON *obj = cJSON_Parse(content.data());
  if (!obj) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Invalid JSON", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string type = type_param;

  // Get current configuration for validation
  cJSON *data = nullptr;
  cJSON *configSchema = nullptr;
  if (type == "mqtt") {
    std::string s =
        instance->m_configManager.serializeToJson<espConfig::mqttConfig_t>();
    data = cJSON_Parse(s.c_str());
    configSchema = cJSON_Duplicate(data, true);
  } else if (type == "misc" || type == "actions") {
    std::string s =
        instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
    data = cJSON_Parse(s.c_str());
    configSchema = cJSON_Duplicate(data, true);
  } else {
    cJSON_Delete(obj);
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Invalid type!", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  if (!validateRequest(req, configSchema, content.data())) {
    cJSON_Delete(data);
    cJSON_Delete(configSchema);
    cJSON_Delete(obj);
    return ESP_FAIL;
  }

  // Process configuration changes and save
  bool success = false;
  bool rebootNeeded = false;
  std::string rebootMsg;

  // Process individual field changes and publish events
  cJSON *it = obj->child;
  while (it) {
    cJSON *configSchemaItem = cJSON_GetObjectItem(configSchema, it->string);
    if (cJSON_Compare(it, configSchemaItem, true)) {
      it = it->next;
      continue;
    }
    const std::string keyStr = it->string;
    if (keyStr == "setupCode") {
      EventValueChanged s{.name = keyStr, .str = it->valuestring};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      HomekitEvent event{.type = HomekitEventType::SETUP_CODE_CHANGED,
                         .data = d};
      std::vector<uint8_t> event_data;
      alpaca::serialize(event, event_data);
      espp::EventManager::get().publish("homekit/event", event_data);
    } else if (keyStr == "nfcNeopixelPin") {
      rebootNeeded = true;
      rebootMsg =
          "Pixel GPIO pin was changed, reboot needed to apply! Rebooting...";
    } else if (keyStr == "nfcSuccessPin" || keyStr == "nfcFailPin" ||
               keyStr == "gpioActionPin") {
      EventValueChanged s{.name = keyStr,
                          .oldValue =
                              static_cast<uint8_t>(configSchemaItem->valueint),
                          .newValue = static_cast<uint8_t>(it->valueint)};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      espp::EventManager::get().publish("hardware/gpioPinChanged", d);
    } else if (keyStr == "btrLowStatusThreshold") {
      EventValueChanged s{.name = "btrLowThreshold",
                          .newValue = static_cast<uint8_t>(it->valueint)};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      HomekitEvent event{.type = HomekitEventType::BTR_PROP_CHANGED, .data = d};
      std::vector<uint8_t> event_data;
      alpaca::serialize(event, event_data);
      espp::EventManager::get().publish("homekit/event", event_data);
    } else if (keyStr == "neoPixelType") {
      rebootNeeded = true;
      rebootMsg =
          "Pixel Type was changed, reboot needed to apply! Rebooting...";
    } else if (keyStr == "gpioActionPin" && it->valueint != 255) {
      cJSON_ReplaceItemInObject(obj, "hkDumbSwitchMode",
                                cJSON_CreateBool(false));
    }
    it = it->next;
  }

  // Merge changes and save configuration
  if (type == "mqtt") {
    mergeJson(data, obj);
    char *data_str = cJSON_PrintUnformatted(data);
    success =
        instance->m_configManager.deserializeFromJson<espConfig::mqttConfig_t>(
            data_str);
    free(data_str);
    if (success) {
      success = instance->m_configManager.saveConfig<espConfig::mqttConfig_t>();
      rebootNeeded = true; // Always reboot on MQTT config change
      rebootMsg = "MQTT config saved, reboot needed to apply! Rebooting...";
    } else
      ESP_LOGE(TAG, "Could not deserialize from JSON!");
  } else if (type == "misc" || type == "actions") {
    mergeJson(data, obj);
    char *data_str = cJSON_PrintUnformatted(data);
    success =
        instance->m_configManager.deserializeFromJson<espConfig::misc_config_t>(
            data_str);
    free(data_str);
    if (success) {
      success =
          instance->m_configManager.saveConfig<espConfig::misc_config_t>();
      if (type == "misc") {
        rebootNeeded = true;
        rebootMsg = "Misc config saved, reboot needed to apply! Rebooting...";
      }
    } else
      ESP_LOGE(TAG, "Could not deserialize from JSON!");
  }

  cJSON_Delete(data);
  cJSON_Delete(configSchema);
  cJSON_Delete(obj);

  if (success) {
    const char *response =
        rebootNeeded ? rebootMsg.c_str() : "Saved and applied!";
    httpd_resp_send(req, response, strlen(response));
    if (rebootNeeded) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      esp_restart();
    }
    return ESP_OK;
  } else {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
}

/**
 * @brief Convert a cJSON value to its compact JSON string representation.
 *
 * @param value Pointer to the cJSON value to stringify (not modified or freed).
 * @return std::string Compact (unformatted) JSON text representing the provided value.
 */
std::string getJsonValueAsString(cJSON *value) {
  char *str = cJSON_PrintUnformatted(value);
  std::string s(str);
  free(str);
  return s;
}

/**
 * @brief Merge top-level properties from one cJSON object into another.
 *
 * Copies each key from @p source into @p target: if a key already exists in
 * @p target its value is replaced with a deep duplicate of the value from
 * @p source, otherwise the key and a deep duplicate of its value are added to
 * @p target.
 *
 * @param target cJSON object to be modified and receive properties from @p source.
 * @param source cJSON object whose top-level properties will be copied.
 *
 * Notes:
 * - Only top-level keys of @p source are considered (no special merging of nested
 *   objects beyond duplicating them).
 * - Values are duplicated with cJSON_Duplicate(..., true), so @p source is not
 *   modified and ownership of the duplicated items is transferred to @p target.
 */
static void mergeJson(cJSON *target, cJSON *source) {
  cJSON *item = source->child;
  while (item) {
    cJSON *existing = cJSON_GetObjectItem(target, item->string);
    if (existing) {
      cJSON_ReplaceItemInObject(target, item->string,
                                cJSON_Duplicate(item, true));
    } else {
      cJSON_AddItemToObject(target, item->string, cJSON_Duplicate(item, true));
    }
    item = item->next;
  }
}

/**
 * @brief Validates a JSON configuration update against an existing schema and rejects invalid changes.
 *
 * Parses the provided JSON `body`, verifies each key exists in `currentData`, checks type compatibility
 * and value constraints for special keys (e.g., "setupCode" format and pairing, keys ending with "Pin",
 * boolean coercion from 0/1), and sends an appropriate HTTP error response on validation failure.
 *
 * @param req HTTP request context used to send error responses when validation fails.
 * @param currentData cJSON object containing the current configuration schema and values to validate against.
 * @param body JSON string with the proposed configuration changes.
 * @return true if all properties in `body` are valid for `currentData`; `false` if parsing fails or any validation rule fails
 *         (an HTTP error response will have been sent to the client in that case).
 */
bool WebServerManager::validateRequest(httpd_req_t *req, cJSON *currentData,
                                       const char *body) {
  uint8_t propertiesProcessed = 0;
  cJSON *obj = cJSON_Parse(body);
  if (!obj) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Invalid JSON", HTTPD_RESP_USE_STRLEN);
    return false;
  }
  WebServerManager *instance = getInstance(req);
  cJSON *it = obj->child;
  while (it) {
    std::string keyStr = it->string;

    cJSON *existingValue = cJSON_GetObjectItem(currentData, keyStr.c_str());
    if (!existingValue) {
      ESP_LOGE(TAG, "\"%s\" does not exist in the current configuration!",
               keyStr.c_str());
      std::string msg =
          "\"" + std::string(keyStr) + "\" is not a valid configuration key.";
      httpd_resp_set_status(req, "400 Bad Request");
      httpd_resp_send(req, msg.c_str(), msg.length());
      cJSON_Delete(obj);
      return false;
    }

    cJSON *incomingValue = it;

    bool typeOk = false;
    if (cJSON_IsString(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS String", keyStr.c_str());
      typeOk = cJSON_IsString(incomingValue);
    } else if (cJSON_IsObject(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS OBJECT", keyStr.c_str());
      typeOk = cJSON_IsObject(incomingValue);
    } else if (cJSON_IsArray(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS ARRAY", keyStr.c_str());
      typeOk = cJSON_IsArray(incomingValue);
    } else if (cJSON_IsBool(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS BOOL", keyStr.c_str());
      typeOk = cJSON_IsBool(incomingValue) ||
               (cJSON_IsNumber(incomingValue) &&
                (incomingValue->valueint == 0 || incomingValue->valueint == 1));
    } else if (cJSON_IsNumber(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS UINT", keyStr.c_str());
      typeOk = cJSON_IsNumber(incomingValue);
    } else {
      ESP_LOGD(TAG, "RX VALUE NOT MATCH ANY EXPECTED TYPE");
      typeOk = false;
    }

    if (!typeOk) {
      ESP_LOGE(TAG, "Type mismatch for key \"%s\"!", keyStr.c_str());
      std::string valueStr = getJsonValueAsString(incomingValue);
      std::string msg = "Invalid type for key \"" + std::string(keyStr) +
                        "\". Received value: " + valueStr;
      httpd_resp_set_status(req, "400 Bad Request");
      httpd_resp_send(req, msg.c_str(), msg.length());
      cJSON_Delete(obj);
      return false;
    }

    // --- setupCode Validation ---
    if (keyStr == "setupCode") {
      if (!cJSON_IsString(incomingValue)) {
        ESP_LOGE(TAG, "\"%s\" is not a string!", keyStr.c_str());
        std::string msg =
            "Value for \"" + std::string(keyStr) + "\" must be a string.";
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, msg.c_str(), msg.length());
        cJSON_Delete(obj);
        return false;
      }
      std::string code = incomingValue->valuestring;
      if (code.length() != 8 ||
          std::find_if(code.begin(), code.end(), [](unsigned char c) {
            return !std::isdigit(c);
          }) != code.end()) {
        ESP_LOGE(TAG, "Invalid setupCode format for value: %s", code.c_str());
        std::string msg = "\"" + code + "\" is not a valid value for \"" +
                          keyStr + "\". It must be an 8-digit number.";
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, msg.c_str(), msg.length());
        cJSON_Delete(obj);
        return false;
      }
      if (homeSpan.controllerListBegin() != homeSpan.controllerListEnd() &&
          code.compare(cJSON_GetStringValue(existingValue)) != 0) {
        ESP_LOGE(TAG,
                 "Attempted to change Setup Code while devices are paired.");
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req,
                        "The Setup Code can only be set if no devices are "
                        "paired, reset if any issues!",
                        HTTPD_RESP_USE_STRLEN);
        cJSON_Delete(obj);
        return false;
      }
      // --- Pin Validation ---
    } else if (str_ends_with(keyStr.c_str(), "Pin")) {
      if (!cJSON_IsNumber(incomingValue) || (incomingValue->valueint == 0)) {
        ESP_LOGE(TAG, "Invalid type or value for GPIO pin \"%s\"",
                 keyStr.c_str());
        std::string msg = getJsonValueAsString(incomingValue) +
                          " is not a valid value for \"" + keyStr + "\".";
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, msg.c_str(), msg.length());
        cJSON_Delete(obj);
        return false;
      }
      uint8_t pin = incomingValue->valueint;
      if (pin != 255 && !GPIO_IS_VALID_GPIO(pin) &&
          !GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
        ESP_LOGE(TAG, "GPIO pin %u for \"%s\" is not a valid pin.", pin,
                 keyStr.c_str());
        std::string msg = std::to_string(pin) +
                          " is not a valid GPIO Pin for \"" + keyStr + "\".";
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, msg.c_str(), msg.length());
        cJSON_Delete(obj);
        return false;
      }
    }

    // --- Boolean Validation ---
    if (cJSON_IsBool(existingValue)) {
      bool valueOk = false;
      if (cJSON_IsBool(incomingValue)) {
        valueOk = true;
      } else if (cJSON_IsNumber(incomingValue)) {
        int val = incomingValue->valueint;
        if (val == 0 || val == 1) {
          cJSON_SetBoolValue(incomingValue, val);
          valueOk = true;
        }
      }

      if (!valueOk) {
        ESP_LOGE(TAG,
                 "\"%s\" could not validate! Expected a boolean (or 0/1).\n",
                 keyStr.c_str());
        std::string msg = getJsonValueAsString(incomingValue) +
                          " is not a valid value for \"" + keyStr + "\".";
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, msg.c_str(), msg.length());
        cJSON_Delete(obj);
        return false;
      }
    }

    propertiesProcessed++;
    it = it->next;
  }

  if (propertiesProcessed != cJSON_GetArraySize(obj)) {
    // This case should ideally not be reached if the loop returns on first
    // error. It's a fallback for unexpected issues.
    ESP_LOGE(TAG, "Not all properties could be validated, cannot continue!");
    httpd_resp_set_status(req, "500 Internal Server Error");
    httpd_resp_send(req, "Something went wrong during validation!",
                    HTTPD_RESP_USE_STRLEN);
    cJSON_Delete(obj);
    return false;
  }

  cJSON_Delete(obj);
  return true;
}

/**
 * @brief Clears a stored configuration section specified by the request.
 *
 * Reads the "type" query parameter from the HTTP request and deletes the
 * corresponding configuration:
 * - "mqtt" -> deletes the MQTT configuration
 * - "misc" or "actions" -> deletes the misc/actions configuration
 *
 * On successful deletion the handler responds with "Cleared! Rebooting...",
 * delays briefly, and restarts the device. If the "type" parameter is missing
 * the handler responds with HTTP 400; if deletion fails it responds with
 * HTTP 500.
 *
 * @param req HTTP request containing the query parameter `type`.
 * @return esp_err_t `ESP_OK` if the requested configuration was deleted and restart was initiated, `ESP_FAIL` otherwise.
 */
esp_err_t WebServerManager::handleClearConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  // Parse query parameters for type
  char query[256];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  char type_param[64];
  if (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) !=
      ESP_OK) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string type = type_param;
  bool success = false;
  if (type == "mqtt") {
    success = instance->m_configManager.deleteConfig<espConfig::mqttConfig_t>();
  } else if (type == "misc" || type == "actions") {
    success =
        instance->m_configManager.deleteConfig<espConfig::misc_config_t>();
  }

  if (success) {
    httpd_resp_send(req, "Cleared! Rebooting...", HTTPD_RESP_USE_STRLEN);
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
    return ESP_OK;
  } else {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
}

/**
 * @brief Erase all HomeKit pairings and initiate a device reboot.
 *
 * Sends an HTTP response indicating HomeKit pairings will be erased, deletes all stored reader data, and issues the HomeSpan reset command to trigger HomeKit pairing removal and a reboot.
 *
 * @param req HTTP request context used to send the response.
 * @return esp_err_t `ESP_OK` if the handler executed and reset was initiated, `ESP_FAIL` if the WebServerManager instance could not be retrieved (a 500 response is sent in that case).
 */
esp_err_t WebServerManager::handleHKReset(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  httpd_resp_send(req, "Erasing HomeKit pairings and rebooting...",
                  HTTPD_RESP_USE_STRLEN);
  instance->m_readerDataManager.deleteAllReaderData();
  homeSpan.processSerialCommand("H");
  return ESP_OK;
}

/**
 * @brief Erase stored Wi‑Fi credentials and trigger a device reboot.
 *
 * Sends an HTTP response informing the client that Wi‑Fi credentials are being erased and a reboot will occur,
 * then issues the serial command that initiates credential erasure and reboot.
 *
 * @param req The HTTP request that triggered this handler; used to send the response.
 * @return esp_err_t `ESP_OK` on success.
 */
esp_err_t WebServerManager::handleWifiReset(httpd_req_t *req) {
  httpd_resp_send(req, "Erasing WiFi credentials and rebooting...",
                  HTTPD_RESP_USE_STRLEN);
  homeSpan.processSerialCommand("X");
  return ESP_OK;
}

/**
 * @brief Initiates device configuration access-point mode and responds to the HTTP client.
 *
 * Sends a short confirmation response to the requester, waits one second, then issues
 * the serial command that instructs the system to enter configuration AP mode.
 *
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t WebServerManager::handleStartConfigAP(httpd_req_t *req) {
  httpd_resp_send(req, "Starting AP mode...", HTTPD_RESP_USE_STRLEN);
  vTaskDelay(pdMS_TO_TICKS(1000));
  homeSpan.processSerialCommand("A");
  return ESP_OK;
}

/**
 * @brief Register a new WebSocket client identified by its socket file descriptor.
 *
 * Adds a WsClient for the given socket fd to the internal client list if one
 * does not already exist and logs the addition. This operation is thread-safe.
 *
 * @param fd Socket file descriptor of the connected WebSocket client.
 */
void WebServerManager::addWebSocketClient(int fd) {
  bool added = false;
  size_t total = 0;
  {
    std::lock_guard<std::mutex> lock(m_wsClientsMutex);
    // Check if client already exists
    auto it = std::find_if(m_wsClients.begin(), m_wsClients.end(),
                           [fd](const std::unique_ptr<WsClient> &client) {
                             return client->fd == fd;
                           });
    if (it == m_wsClients.end()) {
      m_wsClients.emplace_back(std::make_unique<WsClient>(fd));
      added = true;
    }
    total = m_wsClients.size();
  }
  if (added) {
    ESP_LOGI(TAG, "Added WebSocket client fd=%d, total clients: %zu", fd,
             total);
  }
}

/**
 * @brief Remove a WebSocket client identified by its file descriptor.
 *
 * Removes the client with the given socket file descriptor from the internal
 * WebSocket client list. If the client was present it will be erased and, when
 * this removal leaves no remaining clients, the shared status timer will be
 * stopped.
 *
 * This operation is thread-safe: the client list is modified under the
 * internal mutex and the timer stop is performed outside that lock to avoid
 * re-entrancy.
 *
 * @param fd Socket file descriptor identifying the WebSocket client to remove.
 */
void WebServerManager::removeWebSocketClient(int fd) {
  bool removed = false;
  size_t remaining = 0;
  bool stopStatusTimer = false;
  {
    std::lock_guard<std::mutex> lock(m_wsClientsMutex);
    auto it = std::find_if(m_wsClients.begin(), m_wsClients.end(),
                           [fd](const std::unique_ptr<WsClient> &client) {
                             return client->fd == fd;
                           });
    if (it != m_wsClients.end()) {
      ESP_LOGD(TAG, "Removing WebSocket client fd=%d from list", fd);
      m_wsClients.erase(it);
      removed = true;
      remaining = m_wsClients.size();
      stopStatusTimer = m_wsClients.empty();
    }
  }
  if (removed) {
    ESP_LOGI(TAG, "Removed WebSocket client fd=%d, remaining clients: %zu", fd,
             remaining);
  } else {
    ESP_LOGW(TAG, "Attempted to remove non-existent WebSocket client fd=%d",
             fd);
  }
  if (stopStatusTimer && m_statusTimer && esp_timer_is_active(m_statusTimer)) {
    // Stop timer outside the client list lock to avoid re-entrancy
    esp_timer_stop(m_statusTimer);
  }
}

/**
 * @brief Broadcasts a textual message to all connected WebSocket clients.
 *
 * Sends the provided null-terminated UTF-8 text as a WebSocket text frame to every registered client.
 *
 * @param message Null-terminated UTF-8 string to broadcast. The string length is determined with `strlen`.
 */
void WebServerManager::broadcastToWebSocketClients(const char *message) {
  broadcastWs(reinterpret_cast<const uint8_t *>(message), strlen(message),
              HTTPD_WS_TYPE_TEXT);
}

/**
 * @brief Broadcasts a WebSocket message to all connected clients.
 *
 * Enqueues the provided payload for delivery to every current WebSocket client;
 * if a dedicated broadcast queue is enabled the message is placed on that queue,
 * otherwise a snapshot of client connections is taken and a send frame is queued
 * for each client. If there are no connected clients the function returns
 * without sending.
 *
 * @param payload Pointer to the message bytes to broadcast.
 * @param len Length of the message in bytes.
 * @param type WebSocket frame type (e.g., `HTTPD_WS_TYPE_TEXT` or `HTTPD_WS_TYPE_BINARY`).
 */
void WebServerManager::broadcastWs(const uint8_t *payload, size_t len,
                                   httpd_ws_type_t type) {
#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
  // Producers enqueue a broadcast message; sender task will snapshot clients
  // and send.
  size_t total_size = sizeof(BroadcastMsg) + len;
  BroadcastMsg *msg = static_cast<BroadcastMsg *>(malloc(total_size));
  if (!msg) {
    ESP_LOGE(TAG, "Failed to allocate broadcast message (%u bytes)",
             static_cast<unsigned>(total_size));
    return;
  }
  msg->type = type;
  msg->len = len;
  memcpy(msg->payload, payload, len);
  if (!m_wsBroadcastQueue ||
      xQueueSend(m_wsBroadcastQueue, &msg, pdMS_TO_TICKS(10)) != pdPASS) {
    ESP_LOGE(TAG, "Failed to enqueue broadcast message");
    free(msg);
  }
  return;
#else
  // Snapshot client fds under mutex to avoid races
  std::vector<int> fds;
  {
    std::lock_guard<std::mutex> lock(m_wsClientsMutex);
    fds.reserve(m_wsClients.size());
    for (const auto &c : m_wsClients) {
      fds.push_back(c->fd);
    }
  }

  // Lightweight logging: show size and preview (for text)
  if (type == HTTPD_WS_TYPE_TEXT) {
    size_t preview_len = std::min<size_t>(len, 128);
    ESP_LOGD(
        TAG,
        "Broadcasting TEXT to %zu WebSocket clients (%zu bytes, preview): %.*s",
        fds.size(), len, static_cast<int>(preview_len),
        reinterpret_cast<const char *>(payload));
  } else {
    ESP_LOGD(TAG, "Broadcasting BINARY to %zu WebSocket clients (%zu bytes)",
             fds.size(), len);
  }

  if (fds.empty()) {
    return;
  }
  for (int fd : fds) {
    queue_ws_frame(fd, payload, len, type);
  }
#endif
}

/**
 * @brief Enqueues a WebSocket frame for asynchronous transmission by the send task.
 *
 * Allocates a contiguous WsFrame containing a copy of the provided payload and attempts
 * to push a pointer to that frame into the manager's WebSocket queue. If allocation
 * or queueing fails the allocated frame (if any) is freed and the function returns.
 *
 * @param fd Target WebSocket client file descriptor.
 * @param payload Pointer to the frame payload to copy.
 * @param len Number of bytes in `payload`.
 * @param type WebSocket frame type (e.g., HTTPD_WS_TYPE_TEXT or HTTPD_WS_TYPE_BINARY).
 */
void WebServerManager::queue_ws_frame(int fd, const uint8_t *payload,
                                       size_t len, httpd_ws_type_t type) {
   // Allocate memory for the struct and the payload
   size_t total_size = sizeof(WsFrame) + len;
   WsFrame* raw_frame = static_cast<WsFrame *>(malloc(total_size));
   if (!raw_frame) {
     ESP_LOGE(TAG, "Failed to allocate memory for WebSocket frame");
     return;
   }

   // Populate the struct
   raw_frame->fd = fd;
   raw_frame->type = type;
   raw_frame->len = len;
   memcpy(raw_frame->payload, payload, len);

   // Send the pointer to the queue
   if (xQueueSend(m_wsQueue, &raw_frame, pdMS_TO_TICKS(10)) != pdPASS) {
     ESP_LOGE(TAG, "Failed to queue WebSocket frame");
     free(raw_frame);
   }
}

/**
 * @brief Task loop that sends queued WebSocket frames and broadcasts to clients.
 *
 * Runs indefinitely: drains per-client frames and (when enabled) broadcast messages,
 * sends each payload to the appropriate connected clients, and removes clients
 * that exhibit protocol or connection errors. The task snapshots client fds
 * under a lock but never holds locks during network I/O. Allocated frame and
 * broadcast buffers are freed after processing.
 *
 * @param arg Pointer to the WebServerManager instance (task argument).
 */
void WebServerManager::ws_send_task(void *arg) {
  WebServerManager *instance = static_cast<WebServerManager *>(arg);
  WsFrame *frame = nullptr;
#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
  BroadcastMsg *bmsg = nullptr;
#endif

  while (true) {
#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
    // First, drain any queued per-fd frames without blocking
    while (xQueueReceive(instance->m_wsQueue, &frame, 0) == pdPASS) {
      if (!frame) {
        continue;
      }

      esp_err_t send_ret = ESP_FAIL;
      int target_fd = -1;
      {
        // Take the list lock to locate the client, then release before sending
        std::unique_lock<std::mutex> list_lock(instance->m_wsClientsMutex);
        auto it = std::find_if(
            instance->m_wsClients.begin(), instance->m_wsClients.end(),
            [fd = frame->fd](const std::unique_ptr<WsClient> &client) {
              return client->fd == fd;
            });
        if (it != instance->m_wsClients.end()) {
          target_fd = frame->fd; // Store fd for send operation
        }
        list_lock.unlock(); // Always unlock list lock before any potentially
                            // blocking operation
      }

      if (target_fd != -1) {
        // CRITICAL FIX: Do NOT hold any locks during network send operation
        // This prevents deadlock when network operations fail and ESP-IDF tries
        // to clean up
        ESP_LOGD(TAG, "Sending frame to client fd=%d, len=%zu", target_fd,
                 frame->len);
        send_ret =
            instance->ws_send_frame(instance->m_server, target_fd,
                                    frame->payload, frame->len, frame->type);
        ESP_LOGD(TAG, "Send frame result for fd=%d: %s", target_fd,
                 esp_err_to_name(send_ret));
      }

      if (send_ret != ESP_OK) {
        // Check if this is a protocol error (like frame masking issues) or
        // connection error
        bool is_connection_error = false;
        const char *err_name = esp_err_to_name(send_ret);

        if (err_name &&
            (strstr(err_name, "masked") || strstr(err_name, "masking") ||
             strstr(err_name, "MASKED") || strstr(err_name, "MASKING"))) {
          is_connection_error = true;
          ESP_LOGW(TAG,
                   "WebSocket protocol error (frame masking) for client fd=%d, "
                   "removing client",
                   frame->fd);
        } else if (send_ret == ESP_FAIL) {
          is_connection_error = true;
          ESP_LOGW(
              TAG,
              "Connection failed for WebSocket client fd=%d, removing client",
              frame->fd);
        }

        // Only remove if it's a connection/protocol error, not temporary issues
        if (is_connection_error || send_ret == ESP_ERR_INVALID_STATE ||
            send_ret == ESP_ERR_INVALID_ARG) {
          instance->removeWebSocketClient(frame->fd);
        } else {
          ESP_LOGD(TAG, "Temporary send issue for client fd=%d: %s, will retry",
                   frame->fd, esp_err_to_name(send_ret));
        }
      }

      free(frame); // Free the memory after processing
    }

    // Wait briefly for a broadcast; if none, loop back to drain direct frames
    // again
    if (xQueueReceive(instance->m_wsBroadcastQueue, &bmsg, pdMS_TO_TICKS(50)) ==
        pdPASS) {
      if (bmsg) {
        // Snapshot fds under the list lock
        std::vector<int> fds;
        {
          std::lock_guard<std::mutex> lock(instance->m_wsClientsMutex);
          fds.reserve(instance->m_wsClients.size());
          for (const auto &c : instance->m_wsClients)
            fds.push_back(c->fd);
        }
        if (!fds.empty()) {
          // Optionally log (not under list lock)
          if (bmsg->type == HTTPD_WS_TYPE_TEXT) {
            size_t preview_len = std::min<size_t>(bmsg->len, 128);
            ESP_LOGI(TAG,
                     "Broadcasting TEXT to %zu WebSocket clients (%zu bytes, "
                     "preview): %.*s",
                     fds.size(), bmsg->len, static_cast<int>(preview_len),
                     reinterpret_cast<const char *>(bmsg->payload));
          } else {
            ESP_LOGI(TAG,
                     "Broadcasting BINARY to %zu WebSocket clients (%zu bytes)",
                     fds.size(), bmsg->len);
          }
          // Send to each client without holding locks during network operations
          for (int fd : fds) {
            esp_err_t send_ret = ESP_FAIL;
            // CRITICAL: No locks held during network send to prevent deadlock
            send_ret = instance->ws_send_frame(
                instance->m_server, fd, bmsg->payload, bmsg->len, bmsg->type);

            if (send_ret != ESP_OK) {
              // Check if this is a protocol error (like frame masking issues)
              // or connection error
              bool is_connection_error = false;
              const char *err_name = esp_err_to_name(send_ret);

              if (err_name &&
                  (strstr(err_name, "masked") || strstr(err_name, "masking") ||
                   strstr(err_name, "MASKED") || strstr(err_name, "MASKING"))) {
                is_connection_error = true;
                ESP_LOGW(TAG,
                         "WebSocket protocol error (frame masking) for "
                         "broadcast client fd=%d, removing client",
                         fd);
              } else if (send_ret == ESP_FAIL) {
                is_connection_error = true;
                ESP_LOGW(TAG,
                         "Connection failed for broadcast client fd=%d, "
                         "removing client",
                         fd);
              }

              // Only remove if it's a connection/protocol error, not temporary
              // issues
              if (is_connection_error || send_ret == ESP_ERR_INVALID_STATE ||
                  send_ret == ESP_ERR_INVALID_ARG) {
                instance->removeWebSocketClient(fd);
              } else {
                ESP_LOGD(TAG,
                         "Temporary broadcast send issue for client fd=%d: %s, "
                         "will skip",
                         fd, esp_err_to_name(send_ret));
              }
            }
          }
        }
        free(bmsg);
      }
    }
#else
    if (xQueueReceive(instance->m_wsQueue, &frame, portMAX_DELAY) == pdPASS) {
      if (!frame) {
        continue;
      }

      esp_err_t send_ret = ESP_FAIL;
      int target_fd = -1;
      {
        // Take the list lock to locate the client, then release before sending
        std::unique_lock<std::mutex> list_lock(instance->m_wsClientsMutex);
        auto it = std::find_if(
            instance->m_wsClients.begin(), instance->m_wsClients.end(),
            [fd = frame->fd](const std::unique_ptr<WsClient> &client) {
              return client->fd == fd;
            });
        if (it != instance->m_wsClients.end()) {
          target_fd = frame->fd; // Store fd for send operation
        }
        list_lock.unlock(); // Always unlock list lock before any potentially
                            // blocking operation
      }

      if (target_fd != -1) {
        // CRITICAL FIX: Do NOT hold any locks during network send operation
        // This prevents deadlock when network operations fail and ESP-IDF tries
        // to clean up
        ESP_LOGD(TAG, "Sending frame to client fd=%d, len=%zu", target_fd,
                 frame->len);
        send_ret =
            instance->ws_send_frame(instance->m_server, target_fd,
                                    frame->payload, frame->len, frame->type);
        ESP_LOGD(TAG, "Send frame result for fd=%d: %s", target_fd,
                 esp_err_to_name(send_ret));
      }

      if (send_ret != ESP_OK) {
        // Check if this is a protocol error (like frame masking issues) or
        // connection error
        bool is_connection_error = false;
        const char *err_name = esp_err_to_name(send_ret);

        if (err_name &&
            (strstr(err_name, "masked") || strstr(err_name, "masking") ||
             strstr(err_name, "MASKED") || strstr(err_name, "MASKING"))) {
          is_connection_error = true;
          ESP_LOGW(TAG,
                   "WebSocket protocol error (frame masking) for client fd=%d, "
                   "removing client",
                   frame->fd);
        } else if (send_ret == ESP_FAIL) {
          is_connection_error = true;
          ESP_LOGW(
              TAG,
              "Connection failed for WebSocket client fd=%d, removing client",
              frame->fd);
        }

        // Only remove if it's a connection/protocol error, not temporary issues
        if (is_connection_error || send_ret == ESP_ERR_INVALID_STATE ||
            send_ret == ESP_ERR_INVALID_ARG) {
          instance->removeWebSocketClient(frame->fd);
        } else {
          ESP_LOGD(TAG, "Temporary send issue for client fd=%d: %s, will retry",
                   frame->fd, esp_err_to_name(send_ret));
        }
      }

      free(frame); // Free the memory after processing
    }
#endif
  }
}

/**
 * @brief Builds a JSON payload containing basic device metrics.
 *
 * The JSON includes the following fields:
 * - `type`: `"metrics"`
 * - `uptime`: device uptime in seconds
 * - `free_heap`: free heap memory in bytes
 * - `wifi_rssi`: Wi‑Fi RSSI value in dBm
 *
 * @return std::string JSON-encoded metrics object.
 */
std::string WebServerManager::getDeviceMetrics() {
  cJSON *status = cJSON_CreateObject();
  cJSON_AddStringToObject(status, "type", "metrics");
  cJSON_AddNumberToObject(status, "uptime",
                          esp_timer_get_time() / 1000000); // seconds
  cJSON_AddNumberToObject(status, "free_heap", esp_get_free_heap_size());
  cJSON_AddNumberToObject(status, "wifi_rssi", WiFi.RSSI());
  return cjson_to_string_and_free(status);
}

/**
 * @brief Builds a JSON object containing basic system information.
 *
 * The JSON includes the fields `type` ("sysinfo"), `version` (application version),
 * `eth_enabled` (boolean indicating if Ethernet is enabled in misc config), and
 * `wifi_ssid` (currently connected Wi‑Fi SSID).
 *
 * @return std::string JSON string with system information. The caller receives
 * the serialized JSON; memory for the underlying cJSON is freed by this call.
 */
std::string WebServerManager::getDeviceInfo() {
  cJSON *info = cJSON_CreateObject();
  cJSON_AddStringToObject(info, "type", "sysinfo");
  cJSON_AddStringToObject(
      info, "deviceName",
      m_configManager.getConfig<espConfig::misc_config_t>().deviceName.c_str());
  cJSON_AddStringToObject(info, "version", esp_app_get_description()->version);
  cJSON_AddBoolToObject(
      info, "eth_enabled",
      m_configManager.getConfig<espConfig::misc_config_t>().ethernetEnabled);
  cJSON_AddStringToObject(info, "wifi_ssid", WiFi.SSID().c_str());
  return cjson_to_string_and_free(info);
}

/**
 * @brief Process a JSON-formatted WebSocket text message and send an appropriate response to the originating client.
 *
 * This handler parses the provided JSON message, dispatches by the message's "type" field,
 * and queues a text WebSocket frame back to the same client containing the response.
 * Supported message types include heartbeat ping/pong, device metrics, system info, OTA status,
 * echo (returns provided data with timestamp and uptime), and error responses for invalid JSON or unknown types.
 *
 * @param req HTTP request context for the WebSocket connection; used to determine the client's socket.
 * @param message Raw JSON message payload received from the client.
 * @return esp_err_t `ESP_OK` if the message was processed and a response was queued; otherwise an appropriate ESP error code. */
esp_err_t WebServerManager::handleWebSocketMessage(httpd_req_t *req,
                                                   const std::string &message) {
  // Parse JSON message
  cJSON *json = cJSON_Parse(message.c_str());
  if (!json) {
    ESP_LOGE(TAG, "Invalid JSON in WebSocket message: %s", message.c_str());

    // Send error response
    cJSON *error = cJSON_CreateObject();
    cJSON_AddStringToObject(error, "type", "error");
    cJSON_AddStringToObject(error, "message", "Invalid JSON format");
    char *error_str = cJSON_PrintUnformatted(error);

    int sockfd = httpd_req_to_sockfd(req);
    queue_ws_frame(sockfd, (const uint8_t *)error_str, strlen(error_str),
                   HTTPD_WS_TYPE_TEXT);

    free(error_str);
    cJSON_Delete(error);
    return ESP_OK;
  }

  cJSON *type_item = cJSON_GetObjectItem(json, "type");
  if (!type_item || !cJSON_IsString(type_item)) {
    ESP_LOGE(TAG, "WebSocket message missing 'type' field");
    cJSON_Delete(json);
    return ESP_OK;
  }

  std::string msg_type = type_item->valuestring;
  int sockfd = httpd_req_to_sockfd(req);

  // Handle different message types
  if (msg_type == "ping") {
    // Respond to heartbeat ping with pong
    cJSON *pong = cJSON_CreateObject();
    cJSON_AddStringToObject(pong, "type", "pong");
    cJSON_AddNumberToObject(pong, "timestamp", esp_timer_get_time() / 1000);
    char *pong_char_ptr = cJSON_PrintUnformatted(pong);
    std::string pong_str(pong_char_ptr);
    free(pong_char_ptr);
    cJSON_Delete(pong);
    queue_ws_frame(httpd_req_to_sockfd(req), (const uint8_t *)pong_str.c_str(),
                   pong_str.size(), HTTPD_WS_TYPE_TEXT);
  } else if (msg_type == "metrics") {
    // Send system status
    std::string status = getDeviceMetrics();
    queue_ws_frame(sockfd, (const uint8_t *)status.c_str(), status.size(),
                   HTTPD_WS_TYPE_TEXT);
  } else if (msg_type == "sysinfo") {
    // Send system info
    std::string status = getDeviceInfo();
    queue_ws_frame(sockfd, (const uint8_t *)status.c_str(), status.size(),
                   HTTPD_WS_TYPE_TEXT);
  } else if (msg_type == "ota_status") {
    // Send OTA status
    std::string otaStatus = getOTAStatus();
    queue_ws_frame(sockfd, (const uint8_t *)otaStatus.c_str(), otaStatus.size(),
                   HTTPD_WS_TYPE_TEXT);
  } else if (msg_type == "echo") {
    // Echo the message back
    cJSON *echo = cJSON_CreateObject();
    cJSON_AddStringToObject(echo, "type", "echo");
    cJSON *data_item = cJSON_GetObjectItem(json, "data");
    if (data_item) {
      cJSON_AddItemToObject(echo, "data", cJSON_Duplicate(data_item, true));
    }
    cJSON_AddNumberToObject(echo, "timestamp", esp_timer_get_time() / 1000);
    cJSON_AddNumberToObject(echo, "uptime", esp_timer_get_time() / 1000000);
    char *echo_char_ptr = cJSON_PrintUnformatted(echo);
    std::string echo_str(echo_char_ptr);
    free(echo_char_ptr);
    cJSON_Delete(echo);
    queue_ws_frame(sockfd, (const uint8_t *)echo_str.c_str(), echo_str.size(),
                   HTTPD_WS_TYPE_TEXT);
  } else {
    ESP_LOGW(TAG, "Unknown WebSocket message type: %s", msg_type.c_str());

    // Send unknown type response
    cJSON *unknown = cJSON_CreateObject();
    cJSON_AddStringToObject(unknown, "type", "error");
    cJSON_AddStringToObject(unknown, "message", "Unknown message type");
    cJSON_AddStringToObject(unknown, "received_type", msg_type.c_str());
    char *unknown_char_ptr = cJSON_PrintUnformatted(unknown);
    std::string unknown_str(unknown_char_ptr);
    free(unknown_char_ptr);
    cJSON_Delete(unknown);
    queue_ws_frame(sockfd, (const uint8_t *)unknown_str.c_str(),
                   unknown_str.size(), HTTPD_WS_TYPE_TEXT);
  }

  cJSON_Delete(json);
  return ESP_OK;
}

/**
 * @brief Initialize and start the asynchronous OTA worker pipeline.
 *
 * Creates the synchronization primitive and queue used to submit OTA requests and
 * starts the OTA worker task that processes queued requests.
 *
 * On success the manager will have an OTA worker task handle, a binary semaphore
 * indicating worker readiness, and a single-slot request queue. On failure the
 * function releases any partially allocated resources and leaves the OTA
 * worker-related members null.
 */
void WebServerManager::initializeOTAWorker() {
  ESP_LOGI(TAG, "Initializing OTA worker");

  // Create semaphore to track worker availability
  m_otaWorkerReady = xSemaphoreCreateBinary();
  if (m_otaWorkerReady == nullptr) {
    ESP_LOGE(TAG, "Failed to create OTA worker semaphore");
    return;
  }

  // Create request queue
  m_otaRequestQueue = xQueueCreate(1, sizeof(OTAAsyncRequest));
  if (m_otaRequestQueue == nullptr) {
    ESP_LOGE(TAG, "Failed to create OTA request queue");
    vSemaphoreDelete(m_otaWorkerReady);
    m_otaWorkerReady = nullptr;
    return;
  }

  // Create worker task
  BaseType_t result = xTaskCreate(otaWorkerTask, "ota_worker",
                                  8192, // Stack size
                                  this,
                                  5, // Priority
                                  &m_otaWorkerHandle);

  if (result != pdPASS) {
    ESP_LOGE(TAG, "Failed to create OTA worker task");
    vQueueDelete(m_otaRequestQueue);
    vSemaphoreDelete(m_otaWorkerReady);
    m_otaRequestQueue = nullptr;
    m_otaWorkerReady = nullptr;
    return;
  }

  ESP_LOGI(TAG, "OTA worker initialized successfully");
}

/**
 * @brief OTA worker task loop that processes queued OTA requests.
 *
 * Signals readiness by giving the instance's m_otaWorkerReady semaphore, waits for an OTAAsyncRequest from m_otaRequestQueue, sets the instance upload state, calls otaUploadAsync to perform the upload, and completes the associated asynchronous HTTP request. Logs upload type, progress, and errors.
 *
 * @param parameter Pointer to the WebServerManager instance passed as the task parameter.
 */
void WebServerManager::otaWorkerTask(void *parameter) {
  ESP_LOGI(TAG, "OTA worker task started");
  WebServerManager *instance = static_cast<WebServerManager *>(parameter);

  while (true) {
    // Signal that worker is ready
    xSemaphoreGive(instance->m_otaWorkerReady);

    // Wait for OTA request
    OTAAsyncRequest otaReq;
    if (xQueueReceive(instance->m_otaRequestQueue, &otaReq, pdMS_TO_TICKS(5000)) ==
        pdTRUE) {
      const char *uploadTypeStr =
          (otaReq.uploadType == WebServerManager::OTAUploadType::LITTLEFS)
              ? "LittleFS"
              : "Firmware";
      ESP_LOGI(TAG, "Processing %s upload request for %s", uploadTypeStr,
               otaReq.req->uri);

      // Set the current upload type and reboot flag in the instance
      instance->m_currentUploadType = otaReq.uploadType;
      instance->m_skipReboot = otaReq.skipReboot;

      // Process the OTA upload
      esp_err_t result = instance->otaUploadAsync(otaReq.req);

      if(result == ESP_FAIL) {
        httpd_resp_set_status(otaReq.req, "500 Internal Server Error");
        httpd_resp_set_type(otaReq.req, "application/json");
        httpd_resp_sendstr(otaReq.req, "{\"status\":\"error\",\"message\":\"OTA upload failed\"}");
      } else {
        // Send immediate response
        httpd_resp_set_status(otaReq.req, "202 Accepted");
        httpd_resp_set_type(otaReq.req, "application/json");
        httpd_resp_sendstr(otaReq.req, "{\"status\":\"finished\",\"message\":\"OTA upload finished\"}");
      }

      // Complete the async request
      if (httpd_req_async_handler_complete(otaReq.req) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to complete async OTA request");
      }
      if (result != ESP_OK) {
        ESP_LOGE(TAG, "OTA upload failed");
      }
    } else {
      ESP_LOGW(TAG, "OTA worker task timed out waiting for request");
      break;
    }
  }

  ESP_LOGW(TAG, "OTA worker task stopped");
  instance->m_otaWorkerHandle = nullptr;
  vTaskDelete(NULL);
}

/**
 * @brief Accepts an OTA upload HTTP request and enqueues it for asynchronous processing.
 *
 * Validates that no OTA is currently active and that the OTA worker is available,
 * creates an async copy of the HTTP request, determines the upload type from the
 * request URI ("/ota/littlefs" selects LITTLEFS; otherwise firmware), parses the
 * optional "skipReboot" query parameter, and posts an OTAAsyncRequest to the OTA
 * worker queue. Sends an immediate HTTP JSON acceptance response on success or
 * an appropriate HTTP error response on failure.
 *
 * @param req Pointer to the incoming HTTP request initiating the OTA upload.
 * @return esp_err_t `ESP_OK` if the request was accepted and queued; `ESP_FAIL`
 *         if the request was rejected due to OTA in progress, worker busy, or
 *         queueing failure. May return a different `esp_err_t` if creating the
 *         async handler copy fails.
 */
esp_err_t WebServerManager::queueOTARequest(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  // Check if OTA is already in progress
  {
    std::lock_guard<std::mutex> lock(instance->m_otaMutex);
    if (instance->m_otaInProgress) {
      httpd_resp_set_status(req, "409 Conflict");
      httpd_resp_sendstr(req, "{\"status\":\"error\",\"message\":\"OTA update already in progress\"}");
      return ESP_FAIL;
    }
  }

  // Create async request copy
  httpd_req_t *reqCopy = nullptr;
  esp_err_t err = httpd_req_async_handler_begin(req, &reqCopy);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to begin async handler: %s", esp_err_to_name(err));
    return err;
  }

  // Check if worker is available (non-blocking)
  if (xSemaphoreTake(instance->m_otaWorkerReady, 0) == pdFALSE) {
    ESP_LOGE(TAG, "OTA worker is busy");
    httpd_resp_set_status(req, "503 Service Unavailable");
    httpd_resp_sendstr(req, "{\"status\":\"error\",\"message\":\"OTA worker is busy, try again later\"}");
    httpd_req_async_handler_complete(reqCopy);
    return ESP_FAIL;
  }

  // Determine upload type based on endpoint
  WebServerManager::OTAUploadType uploadType =
      WebServerManager::OTAUploadType::FIRMWARE;
  if (strcmp(req->uri, "/ota/littlefs") == 0) {
    uploadType = WebServerManager::OTAUploadType::LITTLEFS;
  }

  // Check for skipReboot parameter in query string
  bool skipReboot = false;
  char query[256];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
    char param[32];
    if (httpd_query_key_value(query, "skipReboot", param, sizeof(param)) ==
        ESP_OK) {
      skipReboot = (strcmp(param, "true") == 0);
    }
  }

  // Create OTA request
  OTAAsyncRequest otaReq;
  otaReq.req = reqCopy;
  otaReq.contentLength = req->content_len;
  otaReq.uploadType = uploadType;
  otaReq.skipReboot = skipReboot;

  // Queue the request
  if (xQueueSend(instance->m_otaRequestQueue, &otaReq, pdMS_TO_TICKS(100)) ==
      pdFALSE) {
    ESP_LOGE(TAG, "Failed to queue OTA request");
    httpd_resp_set_status(req, "503 Service Unavailable");
    httpd_resp_sendstr(req, "{\"status\":\"error\",\"message\":\"Failed to queue OTA request\"}");
    httpd_req_async_handler_complete(reqCopy);
    return ESP_FAIL;
  }

  return ESP_OK;
}

/**
 * @brief Process an asynchronous OTA upload from an HTTP request and apply it to the device.
 *
 * Reads the upload payload from the provided HTTP request and writes it either to the firmware
 * update partition or to the LittleFS data partition depending on the current upload type.
 * Progress and error state are reflected in the instance OTA status fields and are periodically
 * broadcast to connected clients. On successful firmware updates the next boot partition is set
 * and the device will reboot unless the upload was requested with reboot disabled.
 *
 * @param req HTTP request (async) supplying the OTA payload; data is read from this request.
 * @return esp_err_t ESP_OK on successful completion of the upload and finalization;
 *         ESP_FAIL on any error (in which case `m_otaError` is set with a human-readable message).
 */
esp_err_t WebServerManager::otaUploadAsync(httpd_req_t *req) {
  std::lock_guard<std::mutex> lock(m_otaMutex);

  if (esp_timer_is_active(m_statusTimer))
    esp_timer_stop(m_statusTimer);

  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = 10000,  // 10 seconds
      .idle_core_mask = 0,
      .trigger_panic = true
  };
  esp_task_wdt_reconfigure(&wdt_config);
  esp_task_wdt_add(NULL); // Add current task to watchdog

  std::unordered_map<esp_err_t, std::string> errMap = {
    {ESP_ERR_OTA_VALIDATE_FAILED, "Invalid OTA image"},
    {ESP_ERR_OTA_BASE, "OTA error"},
    {ESP_ERR_INVALID_ARG, "Invalid argument"},
    {ESP_ERR_INVALID_STATE, "Invalid state"},
    {ESP_ERR_INVALID_SIZE, "Invalid size"},
  };
  ESP_LOGI(TAG, "Starting async OTA upload processing");
  ESP_LOGI(TAG, "Free heap before OTA: %d bytes", esp_get_free_heap_size());

  size_t content_len = req->content_len;
  if (content_len == 0) {
    ESP_LOGE(TAG, "No upload data received");
    m_otaError = "No upload data received";
    esp_task_wdt_delete(NULL); // Remove from watchdog
    return ESP_FAIL;
  }

  if(content_len > MAX_OTA_SIZE) {
    ESP_LOGE(TAG, "OTA size too large: %d bytes", content_len);
    m_otaError = "OTA size too large. Make sure you are not using the merged firmware file. Max size is " + std::to_string(MAX_OTA_SIZE) + " bytes. Received: " + std::to_string(content_len) + " bytes";
    esp_task_wdt_delete(NULL); // Remove from watchdog
    return ESP_FAIL;
  }

  const char *uploadTypeStr = (m_currentUploadType == OTAUploadType::LITTLEFS)
                                  ? "LittleFS"
                                  : "Firmware";
  ESP_LOGI(TAG, "%s upload size: %d bytes", uploadTypeStr, content_len);

  // Check if we have enough free heap for the operation
  size_t free_heap = esp_get_free_heap_size();
  if (free_heap < 8192) { // Need at least 8KB free
    ESP_LOGE(TAG, "Insufficient heap memory: %d bytes free", free_heap);
    m_otaError = "Insufficient memory for upload";
    esp_task_wdt_delete(NULL); // Remove from watchdog
    return ESP_FAIL;
  }

  esp_err_t err = ESP_OK;

  if (m_currentUploadType == OTAUploadType::FIRMWARE) {
    // Initialize firmware OTA
    m_updatePartition = esp_ota_get_next_update_partition(NULL);
    if (m_updatePartition == NULL) {
      ESP_LOGE(TAG, "No OTA partition found");
      m_otaError = "No OTA partition found";
      esp_task_wdt_delete(NULL); // Remove from watchdog
      return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Writing firmware to partition subtype %d at offset 0x%lx",
             m_updatePartition->subtype, m_updatePartition->address);

    err = esp_ota_begin(m_updatePartition, content_len, &m_otaHandle);
    esp_task_wdt_reset();   // Reset watchdog timer
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "esp_ota_begin failed: %d", err);
      m_otaError =
          "Failed to begin firmware OTA: " + std::to_string(err);
      esp_task_wdt_delete(NULL); // Remove from watchdog
      return ESP_FAIL;
    }
  } else {
    // Initialize LittleFS upload
    m_littlefsPartition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "spiffs");
    if (m_littlefsPartition == NULL) {
      ESP_LOGE(TAG, "No LittleFS partition found");
      m_otaError = "No LittleFS partition found";
      esp_task_wdt_delete(NULL); // Remove from watchdog
      return ESP_FAIL;
    }

    ESP_LOGI(TAG,
             "Writing LittleFS to partition subtype %d at offset 0x%lx (size: "
             "%d bytes)",
             m_littlefsPartition->subtype, m_littlefsPartition->address,
             m_littlefsPartition->size);

    // Check if content fits in partition
    if (content_len > m_littlefsPartition->size) {
      ESP_LOGE(TAG,
               "LittleFS image too large: %d bytes, partition size: %d bytes",
               content_len, m_littlefsPartition->size);
      m_otaError = "LittleFS image too large for partition";
      esp_task_wdt_delete(NULL); // Remove from watchdog
      return ESP_FAIL;
    }

    // Erase the partition before writing
    ESP_LOGI(TAG, "Erasing LittleFS partition...");
    err = esp_partition_erase_range(m_littlefsPartition, 0,
                                    m_littlefsPartition->size);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to erase LittleFS partition: %d", err);
      m_otaError = "Failed to erase LittleFS partition: " +
                   std::to_string(err);
      esp_task_wdt_delete(NULL); // Remove from watchdog
      return ESP_FAIL;
    }
    ESP_LOGI(TAG, "LittleFS partition erased successfully");
  }

  m_otaInProgress = true;
  m_otaWrittenBytes = 0;
  m_otaTotalBytes = content_len;
  m_otaError.clear();

  // Log partition info for the current upload type
  if (m_currentUploadType == OTAUploadType::FIRMWARE && m_updatePartition) {
    ESP_LOGI(TAG, "Preparing to write %d bytes to firmware partition subtype %d at offset 0x%lx",
             content_len, m_updatePartition->subtype, m_updatePartition->address);
  } else if (m_currentUploadType == OTAUploadType::LITTLEFS && m_littlefsPartition) {
    ESP_LOGI(TAG, "Preparing to write %d bytes to LittleFS partition subtype %d at offset 0x%lx",
             content_len, m_littlefsPartition->subtype, m_littlefsPartition->address);
  }
  // Broadcast OTA start status
  broadcastOTAStatus();

  // Allocate buffer prioritizing PSRAM for large buffers, falling back to internal RAM
  const size_t preferred_size = 64 * 1024; // 64KB preferred buffer size
  size_t buffer_size = preferred_size;

  // Also ensure buffer doesn't exceed remaining content
  if (buffer_size > content_len) {
    buffer_size = content_len;
  }

  // Try to allocate from PSRAM first
  void *buffer = heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);
  if (buffer) {
    ESP_LOGI(TAG, "Allocated %d byte buffer from PSRAM", buffer_size);
  } else {
    // Fallback to internal RAM with reduced size based on largest free contiguous block
    size_t largest_free = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    buffer_size = (largest_free * 80) / 100;

    // Ensure minimum buffer size
    const size_t min_buffer_size = 4096; // 4KB minimum
    if (buffer_size < min_buffer_size) {
      buffer_size = min_buffer_size;
    }

    // Also ensure buffer doesn't exceed remaining content
    if (buffer_size > content_len) {
      buffer_size = content_len;
    }

    buffer = heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT);
    if (buffer) {
      ESP_LOGI(TAG, "Allocated %d byte buffer from internal RAM (largest free block: %d bytes)", buffer_size, largest_free);
    } else {
      ESP_LOGE(TAG, "Failed to allocate buffer from internal RAM");
      esp_ota_abort(m_otaHandle);
      m_otaInProgress = false;
      m_otaError = "Failed to allocate buffer for OTA";

      // Broadcast error status
      broadcastOTAStatus();

      esp_task_wdt_delete(NULL); // Remove from watchdog
      return ESP_FAIL;
    }
  }

  char *char_buffer = static_cast<char *>(buffer);

  size_t remaining = content_len;
  int consecutive_failures = 0;
  const int max_failures = 10;
  TickType_t last_progress_time = xTaskGetTickCount();
  const TickType_t progress_timeout = pdMS_TO_TICKS(30000); // 30 second timeout
  size_t last_broadcast_bytes = 0;              // Track last broadcast point
  const size_t broadcast_interval = 128 * 1024; // 128KB intervals

  ESP_LOGI(TAG,
           "Starting to read %d bytes of firmware data with %d byte buffer",
           content_len, buffer_size);

  while (remaining > 0) {
    // Check for timeout
    TickType_t current_time = xTaskGetTickCount();
    if ((current_time - last_progress_time) > progress_timeout) {
      ESP_LOGE(TAG, "OTA upload timeout - no progress for 30 seconds");
      esp_ota_abort(m_otaHandle);
      m_otaInProgress = false;
      m_otaError = "Upload timeout - no progress for 30 seconds";

      // Broadcast error status
      broadcastOTAStatus();

      heap_caps_free(buffer);
      esp_task_wdt_delete(NULL); // Remove from watchdog
      return ESP_FAIL;
    }

    size_t chunk_size = std::min(remaining, buffer_size);

    ESP_LOGD(TAG, "Attempting to receive %d bytes, %d remaining", chunk_size,
             remaining);

    int received = httpd_req_recv(req, char_buffer, chunk_size);

    // Reset watchdog after receive operation
    esp_task_wdt_reset();

    if (received <= 0) {
      consecutive_failures++;
      ESP_LOGW(TAG, "Failed to receive OTA data (attempt %d/%d), received: %d",
               consecutive_failures, max_failures, received);

      if (consecutive_failures >= max_failures) {
        ESP_LOGE(TAG, "Too many consecutive receive failures, aborting OTA");
        esp_ota_abort(m_otaHandle);
        m_otaInProgress = false;
        m_otaError = "Failed to receive firmware data after multiple attempts";

        // Broadcast error status
        broadcastOTAStatus();

        heap_caps_free(buffer);
        esp_task_wdt_delete(NULL); // Remove from watchdog
        return ESP_FAIL;
      }

      // Wait a bit before retrying
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }

    consecutive_failures = 0; // Reset failure counter on success
    last_progress_time = xTaskGetTickCount(); // Update progress time

    if (m_currentUploadType == OTAUploadType::FIRMWARE) {
      err = esp_ota_write(m_otaHandle, buffer, received);
      esp_task_wdt_reset();
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_write failed: %d", err);
        esp_ota_abort(m_otaHandle);
        m_otaInProgress = false;
        m_otaError =
            "Failed to write firmware: " + errMap[err];

        // Broadcast error status
        broadcastOTAStatus();

        heap_caps_free(buffer);
        esp_task_wdt_delete(NULL); // Remove from watchdog
        return ESP_FAIL;
      }
    } else {
      // Write to LittleFS partition
      err = esp_partition_write(m_littlefsPartition, m_otaWrittenBytes, buffer,
                                received);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_partition_write failed: %d", err);
        m_otaInProgress = false;
        m_otaError =
            "Failed to write LittleFS: " + std::to_string(err);

        // Broadcast error status
        broadcastOTAStatus();

        heap_caps_free(buffer);
        esp_task_wdt_delete(NULL); // Remove from watchdog
        return ESP_FAIL;
      }
    }

    m_otaWrittenBytes += received;
    remaining -= received;

    vTaskDelay(1);

    ESP_LOGD(TAG, "Received %d bytes, total: %d/%d (%.1f%%)", received,
             m_otaWrittenBytes, content_len,
             (float)m_otaWrittenBytes / content_len * 100.0);

    // Log and broadcast progress every 128KB or on final chunk
    if ((m_otaWrittenBytes - last_broadcast_bytes) >= broadcast_interval ||
        remaining == 0) {
      ESP_LOGI(TAG, "OTA progress: %d/%d bytes (%.1f%%)", m_otaWrittenBytes,
               content_len, (float)m_otaWrittenBytes / content_len * 100.0);

      // Broadcast progress update
      broadcastOTAStatus();

      last_broadcast_bytes = m_otaWrittenBytes; // Update last broadcast point
    }

    // Yield after each chunk to allow other tasks to run and monitor heap
    size_t current_heap = esp_get_free_heap_size();
    size_t largest_free = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    ESP_LOGD(TAG, "Free heap during OTA: %d bytes, largest free block: %d bytes", current_heap, largest_free);
    if (current_heap < 16384) {
        ESP_LOGW(TAG, "Warning: Free heap dropped below 16KB: %d bytes", current_heap);
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Slightly longer delay since chunks are much larger
  }

  ESP_LOGI(TAG, "OTA write completed: %d bytes", m_otaWrittenBytes);

  heap_caps_free(buffer);

  // Remove task from watchdog before finalizing
  esp_task_wdt_delete(NULL);

  if (m_currentUploadType == OTAUploadType::FIRMWARE) {
    // Finalize firmware OTA
    err = esp_ota_end(m_otaHandle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "esp_ota_end failed: %d", err);
      m_otaInProgress = false;
      m_otaError = "Failed to finalize firmware OTA: " + errMap[err];

      // Broadcast error status
      broadcastOTAStatus();

      return ESP_FAIL;
    }

    // Set boot partition
    err = esp_ota_set_boot_partition(m_updatePartition);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s",
               esp_err_to_name(err));
      m_otaInProgress = false;
      m_otaError =
          "Failed to set boot partition: " + std::string(esp_err_to_name(err));

      // Broadcast error status
      broadcastOTAStatus();

      return ESP_FAIL;
    }

    m_otaInProgress = false;

    if (m_skipReboot) {
      ESP_LOGI(TAG, "Firmware OTA update completed successfully. Reboot "
                    "skipped for sequential upload.");
    } else {
      ESP_LOGI(TAG, "Firmware OTA update completed successfully. Rebooting...");
    }

    // Broadcast completion status
    broadcastOTAStatus();

    if (!m_skipReboot) {
      // Reboot after a short delay
      vTaskDelay(pdMS_TO_TICKS(2000));
      esp_restart();
    }
  } else {
    // Finalize LittleFS upload
    m_otaInProgress = false;
    ESP_LOGI(TAG, "LittleFS upload completed successfully. %d bytes written.",
             m_otaWrittenBytes);

    // Broadcast completion status
    broadcastOTAStatus();

    // Note: No reboot needed for LittleFS, filesystem will be available on next
    // mount
  }

  return ESP_OK;
}

/**
 * @brief Release and stop the asynchronous OTA worker and its resources.
 *
 * Frees the OTA worker task, the OTA request queue, and the OTA-ready semaphore
 * if they exist, and clears the OTA in-progress indicator.
 */
void WebServerManager::cleanupOTAAsync() {
  // Clean up worker task
  if (m_otaWorkerHandle != nullptr) {
    vTaskDelete(m_otaWorkerHandle);
    m_otaWorkerHandle = nullptr;
  }

  // Clean up queue
  if (m_otaRequestQueue != nullptr) {
    vQueueDelete(m_otaRequestQueue);
    m_otaRequestQueue = nullptr;
  }

  // Clean up semaphore
  if (m_otaWorkerReady != nullptr) {
    vSemaphoreDelete(m_otaWorkerReady);
    m_otaWorkerReady = nullptr;
  }

  m_otaInProgress = false;
}

/**
 * @brief Accepts an incoming OTA HTTP upload request and queues it for asynchronous processing.
 *
 * The request is not processed synchronously by this handler; instead it is enqueued for the OTA
 * worker to perform the actual upload and flashing/partition work.
 *
 * @param req Pointer to the HTTP request representing the OTA upload.
 * @return esp_err_t `ESP_OK` if the request was accepted and queued, otherwise an error code indicating why queuing failed.
 */
esp_err_t WebServerManager::handleOTAUpload(httpd_req_t *req) {
  ESP_LOGI(TAG, "OTA upload request received");

  WebServerManager* instance = static_cast<WebServerManager*>(req->user_ctx);

  if(instance->m_otaWorkerHandle == nullptr) {
    instance->initializeOTAWorker();
  }

  // Queue the request for async processing
  return queueOTARequest(req);
}

/**
 * @brief Handle an HTTP request that triggers a short-delayed device reboot.
 *
 * Sends a JSON response informing the client that the device is rebooting,
 * waits briefly, and then restarts the device.
 *
 * @param req HTTP request used to send the JSON response before reboot.
 * @return esp_err_t `ESP_OK` if the response was sent and the reboot was initiated.
 */
esp_err_t WebServerManager::handleReboot(httpd_req_t *req) {
  ESP_LOGI(TAG, "OTA reboot request received");

  // Send response before rebooting
  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, "{\"status\":\"success\",\"message\":\"Rebooting device...\"}"); 

  // Reboot after a short delay
  vTaskDelay(pdMS_TO_TICKS(1000));
  esp_restart();

  return ESP_OK;
}

/**
 * Builds a JSON representation of the current OTA update status.
 *
 * The returned JSON contains the following fields:
 * - `type`: fixed value `"ota_status"`.
 * - `in_progress`: `true` if an OTA is active, `false` otherwise.
 * - `bytes_written`: number of bytes written so far.
 * - `upload_type`: `"littlefs"` or `"firmware"`.
 * - `error`: error message when present.
 * - `current_version`: currently running firmware version.
 * - `running_partition`: label of the running partition when available.
 * - `next_update_partition`: label of the partition selected for the next update when available.
 * - `progress_percent` and `total_bytes`: included when an OTA is in progress and total size is known.
 *
 * @return std::string JSON string describing the OTA status (ownership: caller).
 */
std::string WebServerManager::getOTAStatus() {
  cJSON *status = cJSON_CreateObject();
  cJSON_AddStringToObject(status, "type", "ota_status");
  cJSON_AddBoolToObject(status, "in_progress", m_otaInProgress);
  cJSON_AddNumberToObject(status, "bytes_written", m_otaWrittenBytes);

  // Add upload type information
  const char *uploadTypeStr = (m_currentUploadType == OTAUploadType::LITTLEFS)
                                  ? "littlefs"
                                  : "firmware";
  cJSON_AddStringToObject(status, "upload_type", uploadTypeStr);

  if (!m_otaError.empty()) {
    cJSON_AddStringToObject(status, "error", m_otaError.c_str());
  }

  // Add current firmware info
  const esp_app_desc_t *app_desc = esp_app_get_description();
  cJSON_AddStringToObject(status, "current_version", app_desc->version);

  // Add partition info
  const esp_partition_t *running = esp_ota_get_running_partition();
  const esp_partition_t *next_update = esp_ota_get_next_update_partition(NULL);

  if (running) {
    cJSON_AddStringToObject(status, "running_partition", running->label);
  }
  if (next_update) {
    cJSON_AddStringToObject(status, "next_update_partition",
                            next_update->label);
  }

  // Add progress percentage if OTA is in progress
  if (m_otaInProgress && m_otaTotalBytes > 0) {
    float progress = (float)m_otaWrittenBytes / m_otaTotalBytes * 100.0f;
    cJSON_AddNumberToObject(status, "progress_percent", progress);
    cJSON_AddNumberToObject(status, "total_bytes", m_otaTotalBytes);
  }

  return cjson_to_string_and_free(status);
}

/**
 * @brief Broadcasts the current OTA status to all connected WebSocket clients.
 *
 * Retrieves the OTA status payload produced by getOTAStatus() and sends it to
 * every connected WebSocket client as a text message. The payload is a JSON
 * string that includes the `"type": "ota_status"` object produced by
 * getOTAStatus().
 */
void WebServerManager::broadcastOTAStatus() {
  std::string otaStatus = getOTAStatus();
  broadcastToWebSocketClients(otaStatus.c_str());
}

/**
 * @brief Handles certificate bundle upload requests
 * @param req HTTP request containing certificate bundle content
 * @return ESP_OK on success, ESP_FAIL on error
 *
 * Expects:
 * - Request body: Certificate bundle content in PEM format containing CA, client cert, and private key
 * - Max content length: 8KB
 *
 * The bundle is parsed and individual certificates are saved using ConfigManager's centralized
 * certificate validation and storage methods. Certificate validation is handled by ConfigManager.
 *
 * Returns JSON response with status, message, size, and reconnection status
 */
esp_err_t WebServerManager::handleCertificateUpload(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  ESP_LOGI(TAG, "Certificate bundle upload request, content length: %d",
           req->content_len);

  // Read request body
  size_t content_len = req->content_len;
  if (content_len == 0 || content_len > 8192) { // Max 8KB
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Invalid bundle content length",
                    HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string bundleContent;
  bundleContent.reserve(content_len + 1);

  char buffer[1024];
  size_t remaining = content_len;
  size_t total_received = 0;

  while (remaining > 0) {
    size_t chunk_size = std::min(remaining, sizeof(buffer) - 1);
    int received = httpd_req_recv(req, buffer, chunk_size);

    if (received <= 0) {
      httpd_resp_set_status(req, "400 Bad Request");
      httpd_resp_send(req, "Failed to receive bundle data",
                      HTTPD_RESP_USE_STRLEN);
      return ESP_FAIL;
    }

    buffer[received] = '\0';
    bundleContent.append(buffer, received);
    remaining -= received;
    total_received += received;
  }

  // Save certificate bundle using ConfigManager
  bool success = instance->m_configManager.saveCertificateBundle(bundleContent);

  if (success) {
    ESP_LOGI(TAG, "Certificate bundle uploaded successfully, size: %zu bytes",
             bundleContent.length());

  } else {
    ESP_LOGE(TAG, "Failed to save certificate bundle");
  }

  if (success) {
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddStringToObject(response, "message",
                            "Certificate bundle uploaded successfully");
    cJSON_AddNumberToObject(response, "size", total_received);

    char *response_str = cJSON_PrintUnformatted(response);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response_str, strlen(response_str));

    free(response_str);
    cJSON_Delete(response);
    return ESP_OK;
  } else {
    httpd_resp_set_status(req, "500 Internal Server Error");
    httpd_resp_send(req, "Failed to save certificate bundle",
                    HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }
}

/**
 * @brief Handles certificate status requests
 * @param req HTTP request for certificate status information
 * @return ESP_OK on success, ESP_FAIL on error
 *
 * Returns JSON response with detailed status for each certificate type (CA, client, private key)
 * including existence, validity, size, issuer, subject, expiration, and key matching information.
 * Certificate validation is performed using ConfigManager's centralized validation methods.
 */
esp_err_t WebServerManager::handleCertificateStatus(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  ESP_LOGI(TAG, "Certificate status request received");

  cJSON *response = cJSON_CreateObject();
  cJSON *certificates = cJSON_CreateObject();

  // Check each certificate type using ConfigManager's centralized validation
  const char *certTypes[] = {"ca", "client", "privateKey"};
  for (const char *certType : certTypes) {
    cJSON *certInfo = cJSON_CreateObject();

    // Load certificate content from ConfigManager
    std::string certContent;
    bool exists = false;
    size_t contentSize = 0;

    certContent = instance->m_configManager.loadCertificate(certType);
    exists = !certContent.empty();
    contentSize = certContent.length();

    cJSON_AddBoolToObject(certInfo, "exists", exists);
    cJSON_AddNumberToObject(certInfo, "size", contentSize);

    if (exists) {
      // Validate certificate content and get detailed information
      bool isValid = instance->m_configManager.validateCertificateContent(
          certContent, certType);
      cJSON_AddBoolToObject(certInfo, "valid", isValid);

      // Get issuer and expiration information (only for certificates, not private key)
      if (strcmp(certType, "privateKey") != 0) {
        std::string issuer =
            instance->m_configManager.getCertificateIssuer(certContent);
        std::string subject =
            instance->m_configManager.getCertificateSubject(certContent);
        std::string expiration =
            instance->m_configManager.getCertificateExpiration(certContent);

        if (!issuer.empty()) {
          cJSON_AddStringToObject(certInfo, "issuer", issuer.c_str());
        }

        if (!subject.empty()) {
          cJSON_AddStringToObject(certInfo, "subject", subject.c_str());
        }

        if (!expiration.empty()) {
          cJSON_AddStringToObject(certInfo, "expiration", expiration.c_str());
        }
      }

      // For client certificates, check if private key matches
      if (strcmp(certType, "client") == 0) {
        std::string privateKeyContent = instance->m_configManager.getConfig<const espConfig::mqtt_ssl_t>().clientKey;
        if (!privateKeyContent.empty()) {
          bool keyMatches =
              instance->m_configManager.validatePrivateKeyMatchesCertificate(
                  privateKeyContent, certContent);
          cJSON_AddBoolToObject(certInfo, "keyMatches", keyMatches);
        }
      }

      // Add validation message
      std::string validationMsg =
          isValid ? !strcmp(certType, "privateKey") ? "Key is valid" : "Certificate is valid" : "Validation failed";
      cJSON_AddStringToObject(certInfo, "validationMessage",
                              validationMsg.c_str());
    } else {
      cJSON_AddBoolToObject(certInfo, "valid", false);
      cJSON_AddStringToObject(certInfo, "validationMessage",
                              strcmp(certType, "privateKey") == 0 ? "Private key not configured" : "Certificate not found");
    }

    cJSON_AddItemToObject(certificates, certType, certInfo);
  }

  cJSON_AddItemToObject(response, "certificates", certificates);
  cJSON_AddStringToObject(response, "status", "success");

  // Add timestamp
  cJSON_AddNumberToObject(response, "timestamp", time(nullptr));

  char *response_str = cJSON_PrintUnformatted(response);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, response_str, strlen(response_str));

  free(response_str);
  cJSON_Delete(response);
  return ESP_OK;
}

/**
 * @brief Handles certificate deletion requests
 * @param req HTTP request for certificate deletion
 * @return ESP_OK on success, ESP_FAIL on error
 *
 * Expects URI in format: /certificates/{type} where type is 'ca', 'client', or 'privateKey'
 * Uses ConfigManager's centralized certificate deletion method.
 *
 * Returns JSON response with status, message, and certificate type
 */
esp_err_t WebServerManager::handleCertificateDelete(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  ESP_LOGI(TAG, "Certificate delete request for URI: %s", req->uri);

  // Extract certificate type from URI path
  const char *uri = req->uri;
  const char *type_start = strrchr(uri, '/');
  if (!type_start || strlen(type_start) <= 1) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Missing certificate type in URL",
                    HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string certType = type_start + 1;

  // Validate certificate type
  if (certType != "ca" && certType != "client" && certType != "privateKey") {
    ESP_LOGE(TAG, "Invalid certificate type for deletion: %s",
             certType.c_str());
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(
        req,
        "Invalid certificate type. Must be 'ca', 'client', or 'privateKey'",
        HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  // Delete certificate using ConfigManager's centralized method
  bool success = instance->m_configManager.deleteCertificate(certType);

  if (success) {
    ESP_LOGI(TAG, "Certificate deleted successfully for type: %s",
             certType.c_str());
  } else {
    ESP_LOGE(TAG, "Failed to delete certificate for type: %s",
             certType.c_str());
  }

  if (success) {
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddStringToObject(response, "message",
                            "Certificate deleted successfully");
    cJSON_AddStringToObject(response, "type", certType.c_str());

    char *response_str = cJSON_PrintUnformatted(response);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response_str, strlen(response_str));

    free(response_str);
    cJSON_Delete(response);
    return ESP_OK;
  } else {
    httpd_resp_set_status(req, "500 Internal Server Error");
    httpd_resp_send(req, "Failed to delete certificate", HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }
}

/**
 * @brief Responds to unknown or unmatched HTTP requests with a 404 Not Found.
 *
 * Sends an HTTP 404 response for the given request.
 *
 * @param req HTTP request context to respond to.
 * @return esp_err_t `ESP_FAIL`.
 */
esp_err_t WebServerManager::handleNotFound(httpd_req_t *req) {
  httpd_resp_send_404(req);
  return ESP_FAIL;
}
