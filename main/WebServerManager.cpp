// ============================================================================
// WebServerManager.cpp - ESP32 Web Server Implementation
// ============================================================================

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
#include "esp_log.h"
#include "esp_log_level.h"
#include "esp_task_wdt.h"
#include "eth_structs.hpp"
#include "eventStructs.hpp"
#include "fmt/base.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "freertos/projdefs.h"
#include <LittleFS.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <esp_app_desc.h>
#include <event_manager.hpp>
#include <future>
#include <mutex>
#include <esp_tls_crypto.h>
#include <stdbool.h>
#include <string>

// ============================================================================
// Constants
// ============================================================================

const char *WebServerManager::TAG = "WebServerManager";
const size_t MAX_WS_PAYLOAD = 8192;
const size_t MAX_OTA_SIZE = 0x1E0000;

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
// ============================================================================

WebServerManager::WebServerManager(ConfigManager &configManager,
                                   ReaderDataManager &readerDataManager)
    : m_server(nullptr), m_configManager(configManager),
      m_readerDataManager(readerDataManager), m_mqttManager(nullptr) {
  espp::EventManager::get().add_publisher("homekit/event", "WebServerManager");
  espp::EventManager::get().add_publisher("hardware/gpioPinChanged",
                                          "WebServerManager");
}

WebServerManager::~WebServerManager() {
  ESP_LOGI(TAG, "WebServerManager destructor called");
  cleanupOTAAsync();
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
// ============================================================================

void WebServerManager::begin() {
  ESP_LOGI(TAG, "Initializing...");

  // Mount filesystem
  if (!LittleFS.begin()) {
    ESP_LOGE(TAG, "Failed to mount LittleFS");
    return;
  }
  ESP_LOGI(TAG, "LittleFS mounted: %d/%d bytes", LittleFS.usedBytes(),
           LittleFS.totalBytes());

  // Configure and start HTTP server
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  config.max_uri_handlers = 20;
  config.max_open_sockets = 8;
  config.stack_size = 8192;
  config.uri_match_fn = httpd_uri_match_wildcard;
  config.lru_purge_enable = true;

  if (httpd_start(&m_server, &config) != ESP_OK) {
    ESP_LOGE(TAG, "Error starting server");
    return;
  }

  // Create WebSocket queue
  m_wsQueue = xQueueCreate(10, sizeof(WsFrame *));
  if (!m_wsQueue) {
    ESP_LOGE(TAG, "Failed to create WebSocket queue");
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }

  // Create WebSocket send task
  if (xTaskCreate(ws_send_task, "ws_send_task", 4096, this, 5,
                  &m_wsTaskHandle) != pdPASS) {
    ESP_LOGE(TAG, "Failed to create WebSocket task");
    vQueueDelete(m_wsQueue);
    httpd_stop(m_server);
    m_server = nullptr;
    return;
  }

  setupRoutes();

  // Create status timer
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
    ESP_LOGD(TAG, "HTTP Auth not enabled, skipping...");
    return true;
  }
  size_t hdr_len = httpd_req_get_hdr_value_len(req, "Authorization");
  if(!(hdr_len > 0)){
    ESP_LOGE(TAG, "Authorization data not provided");
    return false;
  }
  std::string authReq; authReq.resize(hdr_len + 1);
  if(httpd_req_get_hdr_value_str(req, "Authorization", authReq.data(), authReq.size()) != ESP_OK){
    ESP_LOGE(TAG, "Invalid HTTP Header, authorization failed");
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
  if (use_compressed)
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

  // Stream file
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

esp_err_t WebServerManager::handleRootOrHash(httpd_req_t *req) {
  WebServerManager* instance = getInstance(req);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
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

  std::string payload = cjson_to_string_and_free(eth_config);
  httpd_resp_set_type(req, "application/json");
  cJSON *res = cJSON_CreateObject();
  cJSON_AddItemToObject(res, "success", cJSON_CreateBool(true));
  cJSON_AddItemToObject(res, "data", cJSON_Parse(payload.c_str()));
  std::string response = cjson_to_string_and_free(res);
  httpd_resp_send(req, response.c_str(), HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

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
    httpd_resp_set_status(req, "400 Bad Request");
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
      espp::EventManager::get().publish("homekit/event", event_data);
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
      espp::EventManager::get().publish("hardware/gpioPinChanged", d);
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
      espp::EventManager::get().publish("homekit/event", event_data);
    } else if (keyStr == "neoPixelType") {
      rebootNeeded = true;
      rebootMsg = "Pixel Type changed, reboot needed! Rebooting...";
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
      if (!cJSON_IsNumber(incomingValue) || incomingValue->valueint == 0) {
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
      uint8_t pin = incomingValue->valueint;
      if (pin != 255 && !GPIO_IS_VALID_GPIO(pin) &&
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

  // Handle WebSocket handshake
  if (req->method == HTTP_GET) {
    if(!instance->basicAuth(req)){
      ESP_LOGE(TAG, "HTTP Authorization failed!");
      httpd_resp_set_type(req, "application/json");
      httpd_resp_set_hdr(req, "Connection", "keep-alive");
      httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
      httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
      return ESP_FAIL;
    }
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
  if (fds.empty()) {
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
  } else if (msg_type == "ota_status") {
    response = getOTAStatus();
  } else if (msg_type == "set_log_level") {  
    cJSON *level_item = cJSON_GetObjectItem(json, "data");
    if(level_item && cJSON_IsNumber(level_item)) {
      esp_log_level_t level = esp_log_level_t(level_item->valueint > 0 && level_item->valueint < 6 ? level_item->valueint : ESP_LOG_WARN);
      esp_log_level_set("*", level);
    }
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

void WebServerManager::initializeOTAWorker() {
  ESP_LOGI(TAG, "Initializing OTA worker");

  m_otaWorkerReady = xSemaphoreCreateBinary();
  if (!m_otaWorkerReady) {
    ESP_LOGE(TAG, "Failed to create semaphore");
    return;
  }

  m_otaRequestQueue = xQueueCreate(1, sizeof(OTAAsyncRequest));
  if (!m_otaRequestQueue) {
    vSemaphoreDelete(m_otaWorkerReady);
    m_otaWorkerReady = nullptr;
    return;
  }

  if (xTaskCreate(otaWorkerTask, "ota_worker", 8192, this, 5,
                  &m_otaWorkerHandle) != pdPASS) {
    vQueueDelete(m_otaRequestQueue);
    vSemaphoreDelete(m_otaWorkerReady);
    m_otaRequestQueue = m_otaWorkerReady = nullptr;
  }
}

void WebServerManager::otaWorkerTask(void *parameter) {
  WebServerManager *instance = static_cast<WebServerManager *>(parameter);

  while (true) {
    xSemaphoreGive(instance->m_otaWorkerReady);

    OTAAsyncRequest otaReq;
    if (xQueueReceive(instance->m_otaRequestQueue, &otaReq,
                      pdMS_TO_TICKS(5000)) == pdTRUE) {
      instance->m_currentUploadType = otaReq.uploadType;
      instance->m_skipReboot = otaReq.skipReboot;

      esp_err_t result = instance->otaUploadAsync(otaReq.req);

      httpd_resp_set_type(otaReq.req, "application/json");
      if (result == ESP_FAIL) {
        httpd_resp_set_type(otaReq.req, "application/json");
        httpd_resp_set_status(otaReq.req, "500 Internal Server Error");
        httpd_resp_sendstr(
            otaReq.req,
            "{\"success\":\"false\",\"error\":\"OTA upload failed\"}");
      } else {
        httpd_resp_set_type(otaReq.req, "application/json");
        httpd_resp_set_status(otaReq.req, "202 Accepted");
        httpd_resp_sendstr(
            otaReq.req,
            "{\"success\":\"true\",\"message\":\"OTA upload finished\"}");
      }

      httpd_req_async_handler_complete(otaReq.req);
      instance->m_otaInProgress = false;
      instance->m_otaWrittenBytes = instance->m_otaTotalBytes = 0;
      instance->m_otaError.clear();
      instance->m_otaHandle = 0;
      if(!instance->m_skipReboot){
        vTaskDelay(pdMS_TO_TICKS(2000));
        esp_restart();
      }
    } else
      break;
  }

  instance->m_otaWorkerHandle = nullptr;
  vTaskDelete(NULL);
}

esp_err_t WebServerManager::queueOTARequest(httpd_req_t *req) {
  WebServerManager *instance = getInstance(req);

  {
    std::lock_guard<std::mutex> lock(instance->m_otaMutex);
    if (instance->m_otaInProgress) {
      httpd_resp_set_type(req, "application/json");
      httpd_resp_set_status(req, "409 Conflict");
      httpd_resp_sendstr(
          req,
          "{\"success\":\"false\",\"error\":\"OTA already in progress\"}");
      return ESP_FAIL;
    }
  }

  httpd_req_t *reqCopy = nullptr;
  esp_err_t err = httpd_req_async_handler_begin(req, &reqCopy);
  if (err != ESP_OK)
    return err;

  if (xSemaphoreTake(instance->m_otaWorkerReady, 0) == pdFALSE) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "503 Service Unavailable");
    httpd_resp_sendstr(
        req, "{\"success\":\"false\",\"error\":\"OTA worker busy\"}");
    httpd_req_async_handler_complete(reqCopy);
    return ESP_FAIL;
  }
  char * type = strrchr(req->uri, '/');
  OTAUploadType uploadType = (strncmp(type+1, "littlefs", strlen("littlefs")) == 0)
                                 ? OTAUploadType::LITTLEFS
                                 : OTAUploadType::FIRMWARE;

  bool skipReboot = false;
  char query[256], param[32];
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK &&
      httpd_query_key_value(query, "skipReboot", param, sizeof(param)) ==
          ESP_OK) {
    skipReboot = (strcmp(param, "true") == 0);
  }

  OTAAsyncRequest otaReq = {reqCopy, req->content_len, uploadType, skipReboot};
  if (xQueueSend(instance->m_otaRequestQueue, &otaReq, pdMS_TO_TICKS(100)) ==
      pdFALSE) {
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "503 Service Unavailable");
    httpd_resp_sendstr(
        req, "{\"success\":\"false\",\"error\":\"Failed to queue request\"}");
    httpd_req_async_handler_complete(reqCopy);
    return ESP_FAIL;
  }
  return ESP_OK;
}

esp_err_t WebServerManager::otaUploadAsync(httpd_req_t *req) {
  std::lock_guard<std::mutex> lock(m_otaMutex);

  if (esp_timer_is_active(m_statusTimer))
    esp_timer_stop(m_statusTimer);

  esp_task_wdt_config_t wdt_config = {
      .timeout_ms = 10000, .idle_core_mask = 0, .trigger_panic = true};
  esp_task_wdt_reconfigure(&wdt_config);
  esp_task_wdt_add(NULL);

  size_t content_len = req->content_len;
  if (content_len == 0 || content_len > MAX_OTA_SIZE) {
    m_otaError = content_len == 0
                     ? "No data"
                     : "OTA size too large (max " +
                           std::to_string(MAX_OTA_SIZE) + " bytes)";
    esp_task_wdt_delete(NULL);
    return ESP_FAIL;
  }

  esp_err_t err = ESP_OK;

  // Initialize OTA partition
  if (m_currentUploadType == OTAUploadType::FIRMWARE) {
    m_updatePartition = esp_ota_get_next_update_partition(NULL);
    if (!m_updatePartition) {
      m_otaError = "No OTA partition found";
      esp_task_wdt_delete(NULL);
      return ESP_FAIL;
    }
    err = esp_ota_begin(m_updatePartition, content_len, &m_otaHandle);
    esp_task_wdt_reset();
    if (err != ESP_OK) {
      m_otaError = "Failed to begin OTA";
      esp_task_wdt_delete(NULL);
      return ESP_FAIL;
    }
  } else {
    m_littlefsPartition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "spiffs");
    if (!m_littlefsPartition) {
      m_otaError = "No LittleFS partition found";
      esp_task_wdt_delete(NULL);
      return ESP_FAIL;
    }
    if (content_len > m_littlefsPartition->size) {
      m_otaError = "LittleFS image too large";
      esp_task_wdt_delete(NULL);
      return ESP_FAIL;
    }
    err = esp_partition_erase_range(m_littlefsPartition, 0,
                                    m_littlefsPartition->size);
    if (err != ESP_OK) {
      m_otaError = "Failed to erase partition";
      esp_task_wdt_delete(NULL);
      return ESP_FAIL;
    }
  }

  m_otaInProgress = true;
  m_otaWrittenBytes = 0;
  m_otaTotalBytes = content_len;
  m_otaError.clear();
  auto f = std::async(std::launch::async, [this]() { broadcastOTAStatus(); });

  const size_t preferred_size = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL) * 0.7;
  size_t buffer_size = std::min(preferred_size, content_len);
  std::unique_ptr<void, decltype(&heap_caps_free)> buffer(heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL), heap_caps_free);

  if (!buffer) {
    size_t largest = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    buffer_size = std::min((largest * 60) / 100, content_len);
    buffer.reset(
        heap_caps_malloc(std::max(buffer_size, size_t(4096)), MALLOC_CAP_8BIT));
    if (!buffer) {
      esp_ota_abort(m_otaHandle);
      m_otaInProgress = false;
      m_otaError = "Failed to allocate buffer";
      broadcastOTAStatus();
      esp_task_wdt_delete(NULL);
      return ESP_FAIL;
    }
  }

  // Upload loop
  size_t remaining = content_len, last_broadcast = 0;
  int consecutive_failures = 0;
  TickType_t last_progress = xTaskGetTickCount();
  const TickType_t timeout = pdMS_TO_TICKS(30000);
  const size_t broadcast_interval = 128 * 1024;

  while (remaining > 0) {
    if ((xTaskGetTickCount() - last_progress) > timeout) {
      esp_ota_abort(m_otaHandle);
      m_otaInProgress = false;
      m_otaError = "Upload timeout";
      broadcastOTAStatus();
      esp_task_wdt_delete(NULL);
      return ESP_FAIL;
    }

    size_t chunk_size = std::min(remaining, buffer_size);
    int received = httpd_req_recv(req, (char *)buffer.get(), chunk_size);
    esp_task_wdt_reset();

    if (received <= 0) {
      if (++consecutive_failures >= 10) {
        esp_ota_abort(m_otaHandle);
        m_otaInProgress = false;
        m_otaError = "Failed to receive data";
        broadcastOTAStatus();
        esp_task_wdt_delete(NULL);
        return ESP_FAIL;
      }
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }

    consecutive_failures = 0;
    last_progress = xTaskGetTickCount();

    if (m_currentUploadType == OTAUploadType::FIRMWARE) {
      err = esp_ota_write(m_otaHandle, buffer.get(), received);
      esp_task_wdt_reset();
      if (err != ESP_OK) {
        esp_ota_abort(m_otaHandle);
        m_otaInProgress = false;
        m_otaError = "Failed to write firmware";
        broadcastOTAStatus();
        esp_task_wdt_delete(NULL);
        return ESP_FAIL;
      }
    } else {
      err = esp_partition_write(m_littlefsPartition, m_otaWrittenBytes,
                                buffer.get(), received);
      if (err != ESP_OK) {
        m_otaInProgress = false;
        m_otaError = "Failed to write LittleFS";
        broadcastOTAStatus();
        esp_task_wdt_delete(NULL);
        return ESP_FAIL;
      }
    }

    m_otaWrittenBytes += received;
    remaining -= received;
    vTaskDelay(1);

    if ((m_otaWrittenBytes - last_broadcast) >= broadcast_interval ||
        remaining == 0) {
      auto f =
          std::async(std::launch::async, [this]() { broadcastOTAStatus(); });
      last_broadcast = m_otaWrittenBytes;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }

  esp_task_wdt_delete(NULL);

  // Finalize OTA
  if (m_currentUploadType == OTAUploadType::FIRMWARE) {
    err = esp_ota_end(m_otaHandle);
    if (err != ESP_OK) {
      m_otaInProgress = false;
      m_otaError = "Failed to finalize OTA";
      broadcastOTAStatus();
      return ESP_FAIL;
    }
    err = esp_ota_set_boot_partition(m_updatePartition);
    if (err != ESP_OK) {
      m_otaInProgress = false;
      m_otaError = "Failed to set boot partition";
      broadcastOTAStatus();
      return ESP_FAIL;
    }
    m_otaInProgress = false;
    broadcastOTAStatus();
  } else {
    m_otaInProgress = false;
    broadcastOTAStatus();
  }
  return ESP_OK;
}

void WebServerManager::cleanupOTAAsync() {
  if (m_otaWorkerHandle) {
    vTaskDelete(m_otaWorkerHandle);
    m_otaWorkerHandle = nullptr;
  }
  if (m_otaRequestQueue) {
    vQueueDelete(m_otaRequestQueue);
    m_otaRequestQueue = nullptr;
  }
  if (m_otaWorkerReady) {
    vSemaphoreDelete(m_otaWorkerReady);
    m_otaWorkerReady = nullptr;
  }
  m_otaInProgress = false;
}

esp_err_t WebServerManager::handleOTAUpload(httpd_req_t *req) {
  WebServerManager *instance = static_cast<WebServerManager *>(req->user_ctx);
  if(!instance->basicAuth(req)){
    ESP_LOGE(TAG, "HTTP Authorization failed!");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Polaris\"");
    httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, NULL);
    return ESP_FAIL;
  }
  if (!instance->m_otaWorkerHandle)
    instance->initializeOTAWorker();
  return queueOTARequest(req);
}

std::string WebServerManager::getOTAStatus() {
  cJSON *status = cJSON_CreateObject();
  cJSON_AddStringToObject(status, "type", "ota_status");
  cJSON_AddBoolToObject(status, "in_progress", m_otaInProgress);
  cJSON_AddNumberToObject(status, "bytes_written", m_otaWrittenBytes);
  cJSON_AddStringToObject(status, "upload_type",
                          (m_currentUploadType == OTAUploadType::LITTLEFS)
                              ? "littlefs"
                              : "firmware");

  if (!m_otaError.empty())
    cJSON_AddStringToObject(status, "error", m_otaError.c_str());
  cJSON_AddStringToObject(status, "current_version",
                          esp_app_get_description()->version);

  const esp_partition_t *running = esp_ota_get_running_partition();
  const esp_partition_t *next_update = esp_ota_get_next_update_partition(NULL);
  if (running)
    cJSON_AddStringToObject(status, "running_partition", running->label);
  if (next_update)
    cJSON_AddStringToObject(status, "next_update_partition",
                            next_update->label);

  if (m_otaInProgress && m_otaTotalBytes > 0) {
    cJSON_AddNumberToObject(status, "progress_percent",
                            (float)m_otaWrittenBytes / m_otaTotalBytes *
                                100.0f);
    cJSON_AddNumberToObject(status, "total_bytes", m_otaTotalBytes);
  }
  return cjson_to_string_and_free(status);
}

void WebServerManager::broadcastOTAStatus() {
  std::string otaStatus = getOTAStatus();
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
