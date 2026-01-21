// ============================================================================
// WebServerManager.cpp - ESP32 Web Server Implementation
// ============================================================================

#include "fmt/ranges.h"
#include "WebServerManager.hpp"
#include "ConfigManager.hpp"
#include "HomeSpan.h"
#include "MqttManager.hpp"
#include "ReaderDataManager.hpp"
#include "cJSON.h"
#include "config.hpp"
#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_log_level.h"
#include "eth_structs.hpp"
#include "eventStructs.hpp"
#include "freertos/idf_additions.h"
#include "loggable.hpp"
#include "sodium/randombytes.h"
#include <LittleFS.h>
#include <algorithm>
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

// ============================================================================
// Constants
// ============================================================================

const char *WebServerManager::TAG = "WebServerManager";
static EventBus::Bus& event_bus = EventBus::Bus::instance();
const size_t MAX_WS_PAYLOAD = 8192;
const size_t MAX_OTA_SIZE = 0x1E0000;
const size_t MAX_FS_SIZE = 0x20000;

// ============================================================================
// Helper Functions
// ============================================================================

static inline bool str_ends_with(const char *str, const char *suffix) {
  if (!str || !suffix)
    return false;
  size_t lenstr = strlen(str), lensuf = strlen(suffix);
  return lenstr >= lensuf && memcmp(str + lenstr - lensuf, suffix, lensuf) == 0;
}

static inline std::string cjson_to_string_and_free(cJSON *obj) {
  if (!obj)
    return "";
  char *raw = cJSON_PrintUnformatted(obj);
  std::string out = raw ? std::string(raw) : std::string{};
  free(raw);
  cJSON_Delete(obj);
  return out;
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
                                   ReaderDataManager &readerDataManager)
    : m_server(nullptr), m_configManager(configManager),
      m_readerDataManager(readerDataManager), m_mqttManager(nullptr) {
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
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  config.max_uri_handlers = 20;
  config.max_open_sockets = 6;
  config.stack_size = 8192;
  config.uri_match_fn = httpd_uri_match_wildcard;
  config.lru_purge_enable = true;

  if (httpd_start(&m_server, &config) != ESP_OK) {
    ESP_LOGE(TAG, "Error starting server");
    return;
  }

  m_wsQueue = xQueueCreate(20, sizeof(WsFrame *));
  if (!m_wsQueue) {
    ESP_LOGE(TAG, "Failed to create WebSocket queue");
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }

  if (xTaskCreate(ws_send_task, "ws_send_task", 4096, this, 5,
                  &m_wsTaskHandle) != pdPASS) {
    ESP_LOGE(TAG, "Failed to create WebSocket task");
    vQueueDelete(m_wsQueue);
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }

  setupRoutes();

  esp_timer_create_args_t timerArgs = {
      .callback = &statusTimerCallback, .arg = this, .name = "statusTimer"};
  if (esp_timer_create(&timerArgs, &m_statusTimer) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to create status timer");
  }

  ESP_LOGI(TAG, "Web server initialization complete");
}

void WebServerManager::stop() {
  ESP_LOGI(TAG, "Stopping WebServerManager");
  httpd_stop(m_server);
  m_server = nullptr;
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
  const std::string cred = fmt::format("{}:{}", m_configManager.getConfig<espConfig::misc_config_t>().webUsername, m_configManager.getConfig<espConfig::misc_config_t>().webPassword);
  size_t n = 0;
  esp_crypto_base64_encode(NULL, 0, &n, (const uint8_t*)cred.c_str(), cred.size());
  std::string digest = "Basic ";
  digest.resize(6+n);
  esp_crypto_base64_encode((uint8_t *)digest.data() + 6, digest.size(), &n, (const uint8_t *)cred.c_str(), cred.size());
  return authReq == digest;
}

// ============================================================================
// Route Setup
// ============================================================================

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
      {"/_app/*", HTTP_GET, handleStaticFiles, this},

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
      {"/certificates/upload", HTTP_POST, handleCertificateUpload, this},
      {"/certificates/status", HTTP_GET, handleCertificateStatus, this},
      {"/certificates/*", HTTP_DELETE, handleCertificateDelete, this},

      // Catch-all
      {"/*", HTTP_GET, handleRootOrHash, this}};

  for (auto &r : routes) {
    httpd_uri_t uri = {.uri = r.uri,
                       .method = r.method,
                       .handler = r.handler,
                       .user_ctx = r.ctx};
#ifdef CONFIG_HTTPD_WS_SUPPORT
    uri.is_websocket = r.is_ws;
#endif
    httpd_register_uri_handler(m_server, &uri);
  }
}

// ============================================================================
// Utility Methods
// ============================================================================

WebServerManager *WebServerManager::getInstance(httpd_req_t *req) {
  return static_cast<WebServerManager *>(req->user_ctx);
}

// ============================================================================
// Static File Handler
// ============================================================================

esp_err_t WebServerManager::handleStaticFiles(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  const char *filename = strrchr(req->uri, '/') + 1;
  if (strlen(filename) == 0)
    filename = "/index.html";

  std::string filepath = req->uri;
  bool use_compressed = false;

  // Check for gzip compressed version
  if (str_ends_with(filename, ".js") || str_ends_with(filename, ".css")) {
    size_t accept_len = httpd_req_get_hdr_value_len(req, "Accept-Encoding");
    if (accept_len > 0 && accept_len <= 512) {
      std::vector<char> hdr(accept_len + 1, 0);
      if (httpd_req_get_hdr_value_str(req, "Accept-Encoding", hdr.data(),
                                      hdr.size()) == ESP_OK) {
        if (std::string(hdr.data()).find("gzip") != std::string::npos) {
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
  httpd_resp_set_hdr(req, "Connection", "close");
  if (use_compressed)
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

  uint8_t *buffer = new uint8_t[4096];
  size_t bytes_read;
  while ((bytes_read = file.read(buffer, 4096)) > 0) {
    esp_err_t err = httpd_resp_send_chunk(req, (const char *)buffer, bytes_read);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to send chunk: %d", err);
      delete[] buffer;
      file.close();
      httpd_resp_send_chunk(req, NULL, 0);
      return ESP_FAIL;
    }
    taskYIELD();
  }
  delete[] buffer;
  file.close();
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
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
  char sessionId[65];
  size_t sessionIdLen = sizeof(sessionId);
  esp_err_t err = httpd_req_get_cookie_val(req, "sessionId", sessionId, &sessionIdLen);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  std::string sessionCookie;
  if(instance->m_sessionId.compare(sessionId) != 0 || err != ESP_OK){
    sessionCookie = fmt::format("sessionId={};", instance->m_sessionId);
    httpd_resp_set_hdr(req, "Set-Cookie", sessionCookie.c_str());
  }

  File file = LittleFS.open("/app.html", "r");
  if (!file) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Connection", "close");

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
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char query[256], type_param[64];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      httpd_query_key_value(query, "type", type_param, sizeof(type_param)) !=
          ESP_OK) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "400 Bad Request");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Missing 'type' parameter"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string type = type_param, responseJson;

  if (type == "mqtt") {
    responseJson =
        instance->m_configManager.serializeToJson<espConfig::mqttConfig_t>();
  } else if (type == "misc") {
    responseJson =
        instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
  } else if (type == "actions"){
    responseJson =
        instance->m_configManager.serializeToJson<espConfig::actions_config_t>();
  } else if (type == "hkinfo") {
    const auto readerData = instance->m_readerDataManager.getReaderDataCopy();
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
    responseJson = cjson_to_string_and_free(hkInfo);
  } else {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "400 Bad Request");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Invalid 'type' parameter"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(true));
  cJSON_AddItemToObject(res, "data", cJSON_Parse(responseJson.c_str()));
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

esp_err_t WebServerManager::handleGetNfcPresets(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  cJSON *response = cJSON_CreateObject();
  cJSON *presetsArray = cJSON_CreateArray();
  for (auto &&v : nfcGpioPinsPresets) {
    cJSON *preset = cJSON_CreateObject();
    cJSON_AddStringToObject(preset, "name", v.name.c_str());
    cJSON* gpioArray = cJSON_AddArrayToObject(preset, "gpioPins");
    for (auto &&pin : v.gpioPins) {
      cJSON_AddItemToArray(gpioArray, cJSON_CreateNumber(pin));
    }
    cJSON_AddItemToArray(presetsArray, preset);
  }
  cJSON *nfcPresets = cJSON_CreateObject();
  cJSON_AddItemToObject(nfcPresets, "presets", presetsArray);
  cJSON_AddItemToObject(response, "data", nfcPresets);
  cJSON_AddItemToObject(response, "success", cJSON_CreateBool(true));
  std::string resp = cjson_to_string_and_free(response);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

esp_err_t WebServerManager::handleGetEthConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  cJSON *eth_config = cJSON_CreateObject();

  // Supported chips
  cJSON *chipsArray = cJSON_CreateArray();
  for (auto &&v : eth_config_ns::supportedChips) {
    cJSON *chip = cJSON_CreateObject();
    cJSON_AddStringToObject(chip, "name", v.second.name.c_str());
    cJSON_AddBoolToObject(chip, "emac", v.second.emac);
    cJSON_AddNumberToObject(chip, "phy_type", v.second.phy_type);
    cJSON_AddItemToArray(chipsArray, chip);
  }
  cJSON_AddItemToObject(eth_config, "supportedChips", chipsArray);

  // Board presets
  cJSON *boardPresetsArray = cJSON_CreateArray();
  for (auto &&v : eth_config_ns::boardPresets) {
    cJSON *preset = cJSON_CreateObject();
    cJSON_AddStringToObject(preset, "name", v.name.c_str());

    cJSON *chip = cJSON_CreateObject();
    cJSON_AddStringToObject(chip, "name", v.ethChip.name.c_str());
    cJSON_AddBoolToObject(chip, "emac", v.ethChip.emac);
    cJSON_AddNumberToObject(chip, "phy_type", v.ethChip.phy_type);
    cJSON_AddItemToObject(preset, "ethChip", chip);
    if(v.ethChip.emac){
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
    } else {
      cJSON *spi_conf = cJSON_CreateObject();
      cJSON_AddNumberToObject(spi_conf, "spi_freq_mhz", v.spi_conf.spi_freq_mhz);
      cJSON_AddNumberToObject(spi_conf, "pin_cs", v.spi_conf.pin_cs);
      cJSON_AddNumberToObject(spi_conf, "pin_irq", v.spi_conf.pin_irq);
      cJSON_AddNumberToObject(spi_conf, "pin_rst", v.spi_conf.pin_rst);
      cJSON_AddNumberToObject(spi_conf, "pin_sck", v.spi_conf.pin_sck);
      cJSON_AddNumberToObject(spi_conf, "pin_miso", v.spi_conf.pin_miso);
      cJSON_AddNumberToObject(spi_conf, "pin_mosi", v.spi_conf.pin_mosi);
      cJSON_AddItemToObject(preset, "spi_conf", spi_conf);
    }
    cJSON_AddItemToArray(boardPresetsArray, preset);
  }
  cJSON_AddItemToObject(eth_config, "boardPresets", boardPresetsArray);
  cJSON_AddBoolToObject(
      eth_config, "ethEnabled",
      instance->m_configManager.getConfig<espConfig::misc_config_t>()
          .ethernetEnabled);

  cJSON_AddNumberToObject(eth_config, "numSpiBuses", SPI_HOST_MAX - 1);

  std::string payload = cjson_to_string_and_free(eth_config);
  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(true));
  cJSON_AddItemToObject(res, "data", cJSON_Parse(payload.c_str()));
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
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
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  // Parse query parameters
  char query[256], type_param[64];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      httpd_query_key_value(query, "type", type_param, sizeof(type_param)) !=
          ESP_OK) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Missing 'type' parameter"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  // Read request body
  const size_t max_content_size = 2048;
  if (req->content_len >= max_content_size) {
    httpd_resp_set_status(req, "413 Payload Too Large");
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Request body too large"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::vector<char> content(max_content_size, 0);
  int ret = httpd_req_recv(req, content.data(), content.size() - 1);
  if (ret <= 0) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Invalid request body"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }
  content[ret] = '\0';

  cJSON *obj = cJSON_Parse(content.data());
  if (!obj) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Invalid JSON"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  // Load current configuration
  std::string type = type_param;
  cJSON *configSchema = nullptr;

  if (type == "mqtt") {
    std::string s =
        instance->m_configManager.serializeToJson<espConfig::mqttConfig_t>();
    configSchema = cJSON_Parse(s.c_str());
  } else if (type == "misc") {
    std::string s =
        instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
    configSchema = cJSON_Parse(s.c_str());
  } else if (type == "actions") {
    std::string s =
        instance->m_configManager.serializeToJson<espConfig::actions_config_t>();
    configSchema = cJSON_Parse(s.c_str());
  } else {
    cJSON_Delete(obj);
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Invalid 'type' parameter"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  if (!validateRequest(req, configSchema, content.data())) {
    cJSON_Delete(configSchema);
    cJSON_Delete(obj);
    return ESP_FAIL;
  }

  bool success = false, rebootNeeded = false;
  std::string rebootMsg;

  // Process configuration changes and publish events
  cJSON *it = obj->child;
  if(it == NULL){
    cJSON_Delete(obj);
    httpd_resp_set_status(req, HTTPD_400);
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Received empty object, nothing to save"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }
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
      event_bus.publish({event_bus.get_topic(HK_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, event_data.data(), event_data.size()});
    } else if (keyStr == "nfcNeopixelPin") {
      rebootNeeded = true;
      rebootMsg = "Pixel GPIO pin changed, reboot needed! Rebooting...";
    } else if (keyStr == "nfcSuccessPin" || keyStr == "nfcFailPin" ||
               keyStr == "gpioActionPin") {
      EventValueChanged s{.name = keyStr,
                          .oldValue = (uint8_t)configSchemaItem->valueint,
                          .newValue = (uint8_t)it->valueint};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      event_bus.publish({event_bus.get_topic(HARDWARE_CONFIG_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, d.data(), d.size()});
      if (keyStr == "gpioActionPin" && it->valueint != 255 && cJSON_IsTrue(cJSON_GetObjectItem(configSchema, "hkDumbSwitchMode"))) {
        cJSON_AddBoolToObject(obj, "hkDumbSwitchMode",false);
      }
    } else if (keyStr == "btrLowStatusThreshold") {
      EventValueChanged s{.name = "btrLowThreshold",
                          .newValue = (uint8_t)it->valueint};
      std::vector<uint8_t> d;
      alpaca::serialize(s, d);
      HomekitEvent event{.type = HomekitEventType::BTR_PROP_CHANGED, .data = d};
      std::vector<uint8_t> event_data;
      alpaca::serialize(event, event_data);
      event_bus.publish({event_bus.get_topic(HK_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, event_data.data(), event_data.size()});
    } else if (keyStr == "neoPixelType") {
      rebootNeeded = true;
      rebootMsg = "Pixel Type changed, reboot needed! Rebooting...";
    } else if (keyStr == "useSSL") {
      esp_chip_info_t chip_info;
      esp_chip_info(&chip_info);
      if(chip_info.model == CHIP_ESP32) {
        httpd_resp_set_type(req, "application/json");
        cJSON *res = cJSON_CreateObject();
        cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
        cJSON_AddItemToObject(res, "error", cJSON_CreateString("TLS currently not available on the ESP32 chip model due to memory constraints"));
        httpd_resp_set_status(req, HTTPD_500);
        std::string response = cjson_to_string_and_free(res);
        httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
      }
    }
    it = it->next;
  }

  char *data_str = cJSON_PrintUnformatted(obj);
  std::string result;
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
      success =
          instance->m_configManager.saveConfig<espConfig::misc_config_t>();
      rebootNeeded = true;
      rebootMsg = "Misc config saved, reboot needed! Rebooting...";
    }
  } else if (type == "actions") {
    result = instance->m_configManager.updateFromJson<espConfig::actions_config_t>(data_str);
    if (!result.empty()) {
      success =
          instance->m_configManager.saveConfig<espConfig::actions_config_t>();
    }
  }

  cJSON_free(data_str);

  cJSON_Delete(configSchema);
  cJSON_Delete(obj);

  if (success) {
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(true));
    cJSON_AddItemToObject(res, "message", cJSON_CreateString(rebootNeeded ? rebootMsg.c_str() : "Saved and applied!"));
    cJSON_AddItemToObject(res, "data", cJSON_Parse(result.c_str()));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    if (rebootNeeded) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      esp_restart();
    }
    return ESP_OK;
  }
  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
  cJSON_AddItemToObject(res, "error", cJSON_CreateString("Unable to save config!"));
  httpd_resp_set_status(req, HTTPD_500);
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_FAIL;
}

bool WebServerManager::validateRequest(httpd_req_t *req, cJSON *currentData,
                                       const char *body) {
  cJSON *obj = cJSON_Parse(body);
  if (!obj) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "400 Bad Request");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Invalid JSON"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return false;
  }

  cJSON *it = obj->child;
  while (it) {
    std::string keyStr = it->string;
    cJSON *existingValue = cJSON_GetObjectItem(currentData, keyStr.c_str());

    if (!existingValue) {
      std::string msg = "\"" + keyStr + "\" is not a valid configuration key.";
      httpd_resp_set_type(req, "application/json");
      httpd_resp_set_status(req, "400 Bad Request");
      cJSON *res = cJSON_CreateObject();
      cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
      cJSON_AddItemToObject(res, "error", cJSON_CreateString(msg.c_str()));
      std::string response = cjson_to_string_and_free(res);
      httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
      return false;
    }

    // Type validation
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
      std::string msg = "Invalid type for key \"" + keyStr +
                        "\". Received: " + std::string(valueStr);
      free(valueStr);
      httpd_resp_set_type(req, "application/json");
      httpd_resp_set_status(req, "400 Bad Request");
      cJSON *res = cJSON_CreateObject();
      cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
      cJSON_AddItemToObject(res, "error", cJSON_CreateString(msg.c_str()));
      std::string response = cjson_to_string_and_free(res);
      httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
      return false;
    }

    // Setup code validation
    if (keyStr == "setupCode") {
      if (!cJSON_IsString(incomingValue)) {
        std::string msg = "Value for \"" + keyStr + "\" must be a string.";
        httpd_resp_set_type(req, "application/json");
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
        std::string msg =
            "\"" + code + "\" is not valid. Must be an 8-digit number.";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_status(req, "400 Bad Request");
        cJSON *res = cJSON_CreateObject();
        cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
        cJSON_AddItemToObject(res, "error", cJSON_CreateString(msg.c_str()));
        std::string response = cjson_to_string_and_free(res);
        httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
        return false;
      }
      if (homeSpan.controllerListBegin() != homeSpan.controllerListEnd() &&
          code.compare(cJSON_GetStringValue(existingValue)) != 0) {
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_status(req, "400 Bad Request");
        cJSON *res = cJSON_CreateObject();
        cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
        cJSON_AddItemToObject(res, "error", cJSON_CreateString("Setup Code can only be set if no devices are paired"));
        std::string response = cjson_to_string_and_free(res);
        httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
        return false;
      }
    }
    // Pin validation
    else if (str_ends_with(keyStr.c_str(), "Pin")) {
      if (!cJSON_IsNumber(incomingValue)) {
        std::string msg = "Value for \"" + keyStr + "\" must be a number.";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_status(req, "400 Bad Request");
        cJSON *res = cJSON_CreateObject();
        cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
        cJSON_AddItemToObject(res, "error", cJSON_CreateString(msg.c_str()));
        std::string response = cjson_to_string_and_free(res);
        httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
        return false;
      }
      if (uint8_t pin = incomingValue->valueint; pin != 255 && !GPIO_IS_VALID_GPIO(pin) &&
                                                 !GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
        std::string msg = std::to_string(pin) +
                          " is not a valid GPIO Pin for \"" + keyStr + "\".";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_status(req, "400 Bad Request");
        cJSON *res = cJSON_CreateObject();
        cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
        cJSON_AddItemToObject(res, "error", cJSON_CreateString(msg.c_str()));
        std::string response = cjson_to_string_and_free(res);
        httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
        return false;
      }
    } else if (keyStr == "ethSpiBus" && cJSON_IsNumber(incomingValue) && (incomingValue->valueint < SPI2_HOST || incomingValue->valueint >= SPI_HOST_MAX)){
        std::string msg = std::to_string(incomingValue->valueint) +
                      " is not a valid SPI Bus value";
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_status(req, "400 Bad Request");
        cJSON *res = cJSON_CreateObject();
        cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
        cJSON_AddItemToObject(res, "error", cJSON_CreateString(msg.c_str()));
        std::string response = cjson_to_string_and_free(res);
        httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
        return false;
    }

    // Boolean coercion
    if (cJSON_IsBool(existingValue)) {
      bool valueOk = cJSON_IsBool(incomingValue);
      if (!valueOk && cJSON_IsNumber(incomingValue)) {
        int val = incomingValue->valueint;
        if (val == 0 || val == 1) {
          cJSON_SetBoolValue(incomingValue, val);
          valueOk = true;
        }
      }
      if (!valueOk) {
        char *valueStr = cJSON_PrintUnformatted(incomingValue);
        std::string msg =
            std::string(valueStr) + " is not valid for \"" + keyStr + "\".";
        free(valueStr);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_status(req, "400 Bad Request");
        cJSON *res = cJSON_CreateObject();
        cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
        cJSON_AddItemToObject(res, "error", cJSON_CreateString(msg.c_str()));
        std::string response = cjson_to_string_and_free(res);
        httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
        return false;
      }
    }
    it = it->next;
  }
  cJSON_Delete(obj);
  return true;
}

esp_err_t WebServerManager::handleClearConfig(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  char query[256], type_param[64];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      httpd_query_key_value(query, "type", type_param, sizeof(type_param)) !=
          ESP_OK) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "400 Bad Request");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Missing 'type' parameter"));
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string type = type_param;
  bool success =
      (type == "mqtt")
          ? instance->m_configManager.deleteConfig<espConfig::mqttConfig_t>()
      : (type == "misc" || type == "actions")
          ? instance->m_configManager.deleteConfig<espConfig::misc_config_t>()
          : false;

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
  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req,
                     "{\"success\":\"true\",\"message\":\"Rebooting...\"}");
  vTaskDelay(pdMS_TO_TICKS(1000));
  esp_restart();
  return ESP_OK;
}

esp_err_t WebServerManager::handleHKReset(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(true));
  cJSON_AddItemToObject(res, "error", cJSON_CreateString("Erasing HomeKit pairings, device will reboot"));
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  instance->m_readerDataManager.deleteAllReaderData();
  homeSpan.processSerialCommand("H");
  return ESP_OK;
}

esp_err_t WebServerManager::handleWifiReset(httpd_req_t *req) {
  WebServerManager* instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(true));
  cJSON_AddItemToObject(res, "error", cJSON_CreateString("Erasing WiFi credentials, device will reboot"));
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  homeSpan.processSerialCommand("X");
  return ESP_OK;
}

esp_err_t WebServerManager::handleStartConfigAP(httpd_req_t *req) {
  WebServerManager* instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(true));
  cJSON_AddItemToObject(res, "error", cJSON_CreateString("Starting AP mode..."));
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  vTaskDelay(pdMS_TO_TICKS(1000));
  instance->stop();
  homeSpan.processSerialCommand("A");
  return ESP_OK;
}

// ============================================================================
// WebSocket Implementation
// ============================================================================

esp_err_t WebServerManager::ws_send_frame(httpd_handle_t server, int fd,
                                          const uint8_t *payload, size_t len,
                                          httpd_ws_type_t type) {
#ifdef CONFIG_HTTPD_WS_SUPPORT
  httpd_ws_frame_t ws_pkt = {.final = true,
                             .fragmented = false,
                             .type = type,
                             .payload = const_cast<uint8_t *>(payload),
                             .len = len};
  return httpd_ws_send_frame_async(server, fd, &ws_pkt);
#else
  return ESP_ERR_NOT_SUPPORTED;
#endif
}

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
    char *sessionId = new char[65];
    size_t sessionIdLen = 65;
    esp_err_t err = httpd_req_get_cookie_val(req, "sessionId", sessionId, &sessionIdLen);
    if(!instance->basicAuth(req) && (err != ESP_OK || strncmp(sessionId, instance->m_sessionId.c_str(), sessionIdLen) != 0)){
      ESP_LOGE(TAG, "HTTP Authorization failed!");
      httpd_resp_set_type(req, "application/json");
      httpd_resp_set_hdr(req, "Connection", "keep-alive");
      httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
      httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
      delete[] sessionId;
      return ESP_FAIL;
    }
    delete[] sessionId;
    int sockfd = httpd_req_to_sockfd(req);
    ESP_LOGI(TAG, "WebSocket client connected: fd=%d", sockfd);
    instance->addWebSocketClient(sockfd);
    if (!instance->m_wsBroadcastBuffer.empty()) {
      for (auto &c : instance->m_wsBroadcastBuffer) {
        instance->queue_ws_frame(sockfd, c.data(), c.size(), HTTPD_WS_TYPE_TEXT);
      }
      instance->m_wsBroadcastBuffer.clear();
    }

    std::string status = instance->getDeviceInfo();
    instance->queue_ws_frame(sockfd, (const uint8_t *)status.c_str(),
                             status.size(), HTTPD_WS_TYPE_TEXT);
    std::string metrics = instance->getDeviceMetrics();
    instance->queue_ws_frame(sockfd, (const uint8_t *)metrics.c_str(),
                             metrics.size(), HTTPD_WS_TYPE_TEXT);

    if (!esp_timer_is_active(instance->m_statusTimer))
      esp_timer_start_periodic(instance->m_statusTimer, 5000 * 1000);
    return ESP_OK;
  }

  // Receive WebSocket frame
  httpd_ws_frame_t ws_pkt = {};
  ws_pkt.type = HTTPD_WS_TYPE_TEXT;
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
    m_wsClients.emplace_back(std::make_unique<WsClient>(fd));
  }
}

void WebServerManager::removeWebSocketClient(int fd) {
  bool stopTimer = false;
  {
    std::scoped_lock lock(m_wsClientsMutex);
    auto it = std::find_if(
        m_wsClients.begin(), m_wsClients.end(),
        [fd](const std::unique_ptr<WsClient> &c) { return c->fd == fd; });
    if (it != m_wsClients.end()) {
      m_wsClients.erase(it);
      ESP_LOGI(TAG, "Removed WebSocket client fd=%d, remaining: %zu", fd,
               m_wsClients.size());
      stopTimer = m_wsClients.empty();
    }
  }
  if (stopTimer && m_statusTimer && esp_timer_is_active(m_statusTimer)) {
    esp_timer_stop(m_statusTimer);
  }
}

void WebServerManager::broadcastWs(const uint8_t *payload, size_t len,
                                   httpd_ws_type_t type) {
  std::vector<int> fds;
  {
    fds.reserve(m_wsClients.size());
    for (const auto &c : m_wsClients)
      fds.push_back(c->fd);
  }
  static const size_t max_buffer = 64;
  if (fds.empty()) {
    if(m_wsBroadcastBuffer.size() >= max_buffer){
       m_wsBroadcastBuffer.erase(m_wsBroadcastBuffer.begin());
    }
    m_wsBroadcastBuffer.push_back(std::vector<uint8_t>(payload, payload + len));
    return;
  }
  for (int fd : fds){
    queue_ws_frame(fd, payload, len, type);
  }
}

void WebServerManager::queue_ws_frame(int fd, const uint8_t *payload,
                                      size_t len, httpd_ws_type_t type) {
  WsFrame *frame = new WsFrame;
  if (!frame)
    return;

  frame->fd = fd;
  frame->type = type;
  frame->len = len;
  frame->payload = new uint8_t[len];
  memcpy(frame->payload, payload, len);

  if (xQueueSend(m_wsQueue, &frame, pdMS_TO_TICKS(100)) != pdTRUE) {
    delete frame;
  }
}

void WebServerManager::ws_send_task(void *arg) {
  WebServerManager *instance = static_cast<WebServerManager *>(arg);
  WsFrame *raw_frame = nullptr;

  while (true) {
    if (xQueueReceive(instance->m_wsQueue, &raw_frame, portMAX_DELAY) ==
        pdPASS) {
      if (!raw_frame)
        continue;

      WsFramePtr frame(raw_frame);

      int target_fd = -1;
      {
        std::scoped_lock<std::mutex> lock(instance->m_wsClientsMutex);
        auto it = std::find_if(
            instance->m_wsClients.begin(), instance->m_wsClients.end(),
            [fd = frame->fd](const std::unique_ptr<WsClient> &c) {
              return c->fd == fd;
            });
        if (it != instance->m_wsClients.end())
          target_fd = frame->fd;
      }

      if (target_fd != -1) {
        esp_err_t send_ret =
            instance->ws_send_frame(instance->m_server, target_fd,
                                    frame->payload, frame->len, frame->type);
        if (send_ret != ESP_OK) {
          const char *err = esp_err_to_name(send_ret);
          bool is_err =
              (err && (strstr(err, "masked") || strstr(err, "MASKED"))) ||
              send_ret == ESP_FAIL;
          if (is_err || send_ret == ESP_ERR_INVALID_STATE ||
              send_ret == ESP_ERR_INVALID_ARG) {
            instance->removeWebSocketClient(frame->fd);
          }
        }
      }
      delete[] frame->payload;
    }
  }
}

esp_err_t WebServerManager::handleWebSocketMessage(httpd_req_t *req,
                                                   const std::string &message) {
  cJSON *json = cJSON_Parse(message.c_str());
  if (!json) {
    cJSON *error = cJSON_CreateObject();
    cJSON_AddStringToObject(error, "type", "error");
    cJSON_AddStringToObject(error, "message", "Invalid JSON format");
    std::string err_str = cjson_to_string_and_free(error);
    queue_ws_frame(httpd_req_to_sockfd(req), (const uint8_t *)err_str.c_str(),
                   err_str.size(), HTTPD_WS_TYPE_TEXT);
    return ESP_OK;
  }

  cJSON *type_item = cJSON_GetObjectItem(json, "type");
  if (!type_item || !cJSON_IsString(type_item)) {
    cJSON_Delete(json);
    return ESP_OK;
  }

  std::string msg_type = type_item->valuestring;
  int sockfd = httpd_req_to_sockfd(req);
  std::string response;

  if (msg_type == "ping") {
    cJSON *pong = cJSON_CreateObject();
    cJSON_AddStringToObject(pong, "type", "pong");
    cJSON_AddNumberToObject(pong, "timestamp", static_cast<uint32_t>(esp_timer_get_time() / 1000));
    response = cjson_to_string_and_free(pong);
  } else if (msg_type == "metrics") {
    response = getDeviceMetrics();
  } else if (msg_type == "sysinfo") {
    response = getDeviceInfo();
  } else if (msg_type == "ota_info") {
    response = getOTAInfo();
  } else if (msg_type == "set_log_level") {  
    cJSON *level_item = cJSON_GetObjectItem(json, "data");
    if(level_item && cJSON_IsNumber(level_item)) {
      esp_log_level_t level = esp_log_level_t(level_item->valueint >= 0 && level_item->valueint < 6 ? level_item->valueint : ESP_LOG_WARN);
      esp_log_level_set("*", level);
      loggable::Sinker::instance().set_level(level_item->valueint >= 0 && level_item->valueint < 6 ? (loggable::LogLevel)level_item->valueint : loggable::LogLevel::Warning);
      m_configManager.updateFromJson<espConfig::misc_config_t>("{\"logLevel\":" + std::to_string(level) + "}");
      m_configManager.saveConfig<espConfig::misc_config_t>();
    }
    response = getDeviceInfo();
  } else {
    cJSON *unknown = cJSON_CreateObject();
    cJSON_AddStringToObject(unknown, "type", "error");
    cJSON_AddStringToObject(unknown, "message", "Unknown message type");
    cJSON_AddStringToObject(unknown, "received_type", msg_type.c_str());
    response = cjson_to_string_and_free(unknown);
  }

  queue_ws_frame(sockfd, (const uint8_t *)response.c_str(), response.size(),
                 HTTPD_WS_TYPE_TEXT);
  cJSON_Delete(json);
  return ESP_OK;
}

// ============================================================================
// Device Info/Status Methods
// ============================================================================

std::string WebServerManager::getDeviceMetrics() {
  cJSON *status = cJSON_CreateObject();
  cJSON_AddStringToObject(status, "type", "metrics");
  cJSON_AddNumberToObject(status, "uptime", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<int64_t, std::micro>(esp_timer_get_time())).count());
  cJSON_AddNumberToObject(status, "free_heap", esp_get_free_heap_size());
  cJSON_AddNumberToObject(status, "wifi_rssi", WiFi.RSSI());
  return cjson_to_string_and_free(status);
}

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
  cJSON_AddNumberToObject(info, "log_level", esp_log_level_get("*"));
  esp_chip_info_t chipInfo;
  esp_chip_info(&chipInfo);
  cJSON_AddNumberToObject(info, "chip_model", chipInfo.model);
  cJSON_AddNumberToObject(info, "log_level", esp_log_level_get("*"));
  return cjson_to_string_and_free(info);
}

void WebServerManager::statusTimerCallback(void *arg) {
  WebServerManager *instance = static_cast<WebServerManager *>(arg);
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
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
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

  if (uploadType == OTAUploadType::FIRMWARE && req->content_len > MAX_OTA_SIZE) {
    ESP_LOGE(TAG, "OTA size %zu > max %zu", req->content_len, MAX_OTA_SIZE);
    instance->m_otaInProgress = false;
    httpd_resp_set_status(req, "413 Payload Too Large");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"Firmware too large\"}");
    return ESP_OK;
  }

  if (uploadType == OTAUploadType::LITTLEFS && req->content_len > MAX_FS_SIZE) {
    ESP_LOGE(TAG, "OTA size %zu > max %zu", req->content_len, MAX_FS_SIZE);
    instance->m_otaInProgress = false;
    httpd_resp_set_status(req, "413 Payload Too Large");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":false,\"error\":\"LittleFS too large\"}");
    return ESP_OK;
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
    httpd_resp_sendstr(req, "\"success\":false,\"error\":\"Failed to start OTA\"");
    return ESP_OK;
  }

  OTAParams *params = new OTAParams{reqCopy, instance, uploadType, skipReboot, req->content_len, new OTAState()};
  params->state->inProgress = true;

  if (xTaskCreate(otaTask, "ota_task", 8192, params, 5, NULL) != pdPASS) {
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

  params->state->inProgress = false;
  instance->broadcastOTAStatus(*params->state);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, "{\"success\":true,\"message\":\"Update Complete\"}");
  httpd_req_async_handler_complete(req);
  instance->m_otaInProgress = false;
  
  if (!params->skipReboot) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      esp_restart();
  }
  
  if (buffer) free(buffer);
  delete params->state;
  delete params;
  vTaskDelete(NULL);
  return;

error:
  params->state->inProgress = false;
  instance->broadcastOTAStatus(*params->state);
  if (params->state->handle) esp_ota_abort(params->state->handle);
  if (buffer) free(buffer);
  
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_status(req, "500 Internal Server Error");
  cJSON *errResp = cJSON_CreateObject();
  cJSON_AddBoolToObject(errResp, "success", false);
  cJSON_AddStringToObject(errResp, "error", params->state->error.c_str());
  std::string errJson = cjson_to_string_and_free(errResp);
  httpd_resp_sendstr(req, errJson.c_str());
  httpd_req_async_handler_complete(req);
  instance->m_otaInProgress = false;
  delete params->state;
  delete params;
  vTaskDelete(NULL);
}

std::string WebServerManager::getOTAInfo() {
  cJSON *status = cJSON_CreateObject();
  cJSON_AddStringToObject(status, "type", "ota_info");
  
  cJSON_AddStringToObject(status, "current_version",
                          esp_app_get_description()->version);

  const esp_partition_t *running = esp_ota_get_running_partition();
  const esp_partition_t *next_update = esp_ota_get_next_update_partition(NULL);
  if (running)
    cJSON_AddStringToObject(status, "running_partition", running->label);
  if (next_update)
    cJSON_AddStringToObject(status, "next_update_partition",
                            next_update->label);
  return cjson_to_string_and_free(status);
}

void WebServerManager::broadcastOTAStatus(const OTAState& state) {
  cJSON *status = cJSON_CreateObject();
  cJSON_AddStringToObject(status, "type", "ota_status");
  if (!state.error.empty())
    cJSON_AddStringToObject(status, "error", state.error.c_str());
  cJSON_AddBoolToObject(status, "in_progress", state.inProgress);
  cJSON_AddNumberToObject(status, "bytes_written", state.writtenBytes);
  cJSON_AddStringToObject(status, "upload_type",
                          (state.currentUploadType == OTAUploadType::LITTLEFS)
                              ? "littlefs"
                              : "firmware");

  if (state.inProgress && state.totalBytes > 0) {
    cJSON_AddNumberToObject(status, "progress_percent",
                            (float)state.writtenBytes / state.totalBytes *
                                100.0f);
    cJSON_AddNumberToObject(status, "total_bytes", state.totalBytes);
  }
  
  std::string otaStatus = cjson_to_string_and_free(status);
  broadcastWs((const uint8_t *)otaStatus.c_str(), otaStatus.size(),
              HTTPD_WS_TYPE_TEXT);
}

// ============================================================================
// Certificate Handlers
// ============================================================================

esp_err_t WebServerManager::handleCertificateUpload(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  size_t content_len = req->content_len;
  if (content_len == 0 || content_len > 8192) {
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Invalid bundle content length"));
    httpd_resp_set_status(req, "400 Bad Request");
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  char query[256], type_param[128];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) !=
           ESP_OK &&
       (strcmp(type_param, "client") || strcmp(type_param, "ca") ||
        strcmp(type_param, "privateKey")))) {
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Missing 'type' parameter"));
    httpd_resp_set_status(req, "400 Bad Request");
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string certBuf;
  certBuf.reserve(content_len + 1);
  char buffer[1024];
  size_t remaining = content_len;

  while (remaining > 0) {
    size_t chunk_size = std::min(remaining, sizeof(buffer) - 1);
    int received = httpd_req_recv(req, buffer, chunk_size);
    if (received <= 0) {
      httpd_resp_set_type(req, "application/json");
      cJSON *res = cJSON_CreateObject();
      cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
      cJSON_AddItemToObject(res, "error", cJSON_CreateString("Failed to receive bundle data"));
      httpd_resp_set_status(req, "400 Bad Request");
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
      return ESP_FAIL;
    }
    buffer[received] = '\0';
    certBuf.append(buffer, received);
    remaining -= received;
  }

  bool success = instance->m_configManager.saveCertificate(
      std::string(type_param), certBuf);

  if (success) {
    cJSON *response = cJSON_CreateObject();
    cJSON_AddItemToObject(response, "success", cJSON_CreateBool(true));
    cJSON_AddStringToObject(
        response, "message",
        fmt::format("Certificate '{}' saved successfully!", type_param)
            .c_str());
    cJSON_AddNumberToObject(response, "size", content_len);
    std::string resp = cjson_to_string_and_free(response);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp.c_str(), resp.length());
    return ESP_OK;
  }

  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
  cJSON_AddItemToObject(res, "error", cJSON_CreateString("Failed to save certificate"));
  httpd_resp_set_status(req, HTTPD_500);
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_FAIL;
}

esp_err_t WebServerManager::handleCertificateStatus(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  cJSON *response = cJSON_CreateObject();
  cJSON *certificates = cJSON_CreateObject();

  std::vector<CertificateStatus> status =
      instance->m_configManager.getCertificatesStatus();
  for (auto cert : status) {
    cJSON *certInfo = cJSON_CreateObject();
    if (cert.type != "privateKey") {
      if (!cert.issuer.empty())
        cJSON_AddStringToObject(certInfo, "issuer", cert.issuer.c_str());
      if (!cert.subject.empty())
        cJSON_AddStringToObject(certInfo, "subject", cert.subject.c_str());
      if (!cert.expiration.from.empty() && !cert.expiration.to.empty()) {
        cJSON *expiration = cJSON_CreateObject();
        cJSON_AddStringToObject(expiration, "from",
                                cert.expiration.from.c_str());
        cJSON_AddStringToObject(expiration, "to", cert.expiration.to.c_str());
        cJSON_AddItemToObject(certInfo, "expiration", expiration);
      }
      if(cert.type == "client"){
        cJSON_AddBoolToObject(certInfo, "keyMatchesCert", cert.keyMatchesCert);
      }
    } else if (cert.type == "privateKey") {
      cJSON_AddBoolToObject(certInfo, "exists", true);
    }

    cJSON_AddItemToObject(certificates, cert.type.c_str(), certInfo);
  }

  cJSON_AddItemToObject(response, "data", certificates);
  cJSON_AddItemToObject(response, "success", cJSON_CreateBool(true));

  std::string resp = cjson_to_string_and_free(response);
  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());
  return ESP_OK;
}

esp_err_t WebServerManager::handleCertificateDelete(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  const char *type_start = strrchr(req->uri, '/');
  if (!type_start || strlen(type_start) <= 1) {
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Missing certificate type"));
    httpd_resp_set_status(req, "400 Bad Request");
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  std::string certType = type_start + 1;
  if (certType != "ca" && certType != "client" && certType != "privateKey") {
    httpd_resp_set_type(req, "application/json");
    cJSON *res = cJSON_CreateObject();
    cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
    cJSON_AddItemToObject(res, "error", cJSON_CreateString("Invalid certificate type"));
    httpd_resp_set_status(req, "400 Bad Request");
    std::string response = cjson_to_string_and_free(res);
    httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
    return ESP_FAIL;
  }

  bool success = instance->m_configManager.deleteCertificate(certType);

  if (success) {
    cJSON *response = cJSON_CreateObject();
    cJSON_AddItemToObject(response, "success", cJSON_CreateBool(true));
    cJSON_AddStringToObject(response, "message", fmt::format("Certificate '{}' deleted", certType).c_str());
    std::string resp = cjson_to_string_and_free(response);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp.c_str(), resp.length());
    return ESP_OK;
  }

  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(false));
  cJSON_AddItemToObject(res, "error", cJSON_CreateString("Failed to delete certificate"));
  httpd_resp_set_status(req, HTTPD_500);
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_FAIL;
}
