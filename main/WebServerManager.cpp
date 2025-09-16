#include <cstring>
#include <event_manager.hpp>
#include "WebServerManager.hpp"
#include "ConfigManager.hpp"
#include "HomeSpan.h"
#include "ReaderDataManager.hpp"
#include "esp_log.h"
#include "config.hpp"
#include "eth_structs.hpp"
#include "eventStructs.hpp"
#include "fmt/base.h"
#include "fmt/ranges.h"
#include <LittleFS.h>
#include <esp_app_desc.h>
#include <string>
#include "cJSON.h"
#include "esp_http_server.h"
#include "alpaca/alpaca.h"

const char* WebServerManager::TAG = "WebServerManager";

// Forward declaration
static void mergeJson(cJSON *target, cJSON *source);

WebServerManager::WebServerManager(ConfigManager& configManager, ReaderDataManager& readerDataManager)
    : m_server(nullptr),
      m_configManager(configManager),
      m_readerDataManager(readerDataManager)
{
  espp::EventManager::get().add_publisher("homekit/event", "WebServerManager");
  espp::EventManager::get().add_publisher("hardware/gpioPinChanged", "WebServerManager");
}

void WebServerManager::begin() {
    ESP_LOGI(TAG, "Initializing...");
    
    if (!LittleFS.begin()) {
        ESP_LOGE(TAG, "Failed to mount LittleFS. Web server cannot start.");
        return;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.max_uri_handlers = 20;
    config.max_resp_headers = 8;
    config.max_open_sockets = 7;
    config.stack_size = 8192;
    config.uri_match_fn = httpd_uri_match_wildcard;

    esp_err_t ret = httpd_start(&m_server, &config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
        setupRoutes();
        ESP_LOGI(TAG, "Web server started.");
    } else {
        ESP_LOGE(TAG, "Error starting server: %s", esp_err_to_name(ret));
    }
}

std::string WebServerManager::indexProcessor(const std::string& var) {
    if (var == "VERSION") {
        return esp_app_get_description()->version;
    }
    return "";
}

WebServerManager* WebServerManager::getInstance(httpd_req_t *req) {
    return static_cast<WebServerManager*>(req->user_ctx);
}

void WebServerManager::setupRoutes() {
    // Static file handlers
    httpd_uri_t static_uri = {
        .uri       = "/static/*",
        .method    = HTTP_GET,
        .handler   = handleStaticFiles,
        .user_ctx  = (void*)"/static"
    };
    httpd_register_uri_handler(m_server, &static_uri);

    httpd_uri_t assets_uri = {
        .uri       = "/assets/*",
        .method    = HTTP_GET,
        .handler   = handleStaticFiles,
        .user_ctx  = (void*)"/assets"
    };
    httpd_register_uri_handler(m_server, &assets_uri);

    httpd_uri_t fragment_uri = {
        .uri       = "/fragment/*",
        .method    = HTTP_GET,
        .handler   = handleStaticFiles,
        .user_ctx  = (void*)"/routes"
    };
    httpd_register_uri_handler(m_server, &fragment_uri);

    // Main web routes
    httpd_uri_t root_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = handleRootOrHash,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &root_uri);

    // API endpoints
    httpd_uri_t config_get_uri = {
        .uri       = "/config",
        .method    = HTTP_GET,
        .handler   = handleGetConfig,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &config_get_uri);

    httpd_uri_t config_clear_uri = {
        .uri       = "/config/clear",
        .method    = HTTP_POST,
        .handler   = handleClearConfig,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &config_clear_uri);

    httpd_uri_t config_save_uri = {
        .uri       = "/config/save",
        .method    = HTTP_POST,
        .handler   = handleSaveConfig,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &config_save_uri);

    httpd_uri_t eth_config_uri = {
        .uri       = "/eth_get_config",
        .method    = HTTP_GET,
        .handler   = handleGetEthConfig,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &eth_config_uri);

    httpd_uri_t reboot_uri = {
        .uri       = "/reboot_device",
        .method    = HTTP_GET,
        .handler   = handleReboot,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &reboot_uri);

    httpd_uri_t hk_reset_uri = {
        .uri       = "/reset_hk_pair",
        .method    = HTTP_GET,
        .handler   = handleHKReset,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &hk_reset_uri);

    httpd_uri_t wifi_reset_uri = {
        .uri       = "/reset_wifi_cred",
        .method    = HTTP_GET,
        .handler   = handleWifiReset,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &wifi_reset_uri);

    httpd_uri_t config_ap_uri = {
        .uri       = "/start_config_ap",
        .method    = HTTP_GET,
        .handler   = handleStartConfigAP,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &config_ap_uri);

    httpd_uri_t wifi_rssi_uri = {
        .uri       = "/get_wifi_rssi",
        .method    = HTTP_GET,
        .handler   = handleGetWifiRssi,
        .user_ctx  = this
    };
    httpd_register_uri_handler(m_server, &wifi_rssi_uri);
}

// Static file handler implementation
esp_err_t WebServerManager::handleStaticFiles(httpd_req_t *req) {
    const char* base_path = (const char*)req->user_ctx;
    char filepath[256];
    char compressed_filepath[260];
    
    // Get the file path from URI
    const char* filename = req->uri + strlen(base_path);
    if (strlen(filename) == 0) {
        filename = "/index.html";
    }
    
    snprintf(filepath, sizeof(filepath), "%s%s", base_path, filename);
    
    // Check for compressed version first for .js and .css files
    bool use_compressed = false;
    const char* actual_filepath = filepath;
    
    if (strstr(filename, ".js") || strstr(filename, ".css")) {
        // Check if client accepts gzip encoding
        size_t accept_encoding_len = httpd_req_get_hdr_value_len(req, "Accept-Encoding");
        if (accept_encoding_len > 0) {
            char* accept_encoding = (char*)malloc(accept_encoding_len + 1);
            if (accept_encoding && httpd_req_get_hdr_value_str(req, "Accept-Encoding", accept_encoding, accept_encoding_len + 1) == ESP_OK) {
                if (strstr(accept_encoding, "gzip")) {
                    snprintf(compressed_filepath, sizeof(compressed_filepath), "%s.gz", filepath);
                    if (LittleFS.exists(compressed_filepath)) {
                        use_compressed = true;
                        actual_filepath = compressed_filepath;
                    }
                }
                free(accept_encoding);
            }
        }
    }
    
    // Check if file exists
    if (!LittleFS.exists(actual_filepath)) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    
    File file = LittleFS.open(actual_filepath, "r");
    if (!file) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    
    // Determine content type based on original filename (not compressed)
    const char* content_type = "text/plain";
    if (strstr(filename, ".html")) content_type = "text/html";
    else if (strstr(filename, ".css")) content_type = "text/css";
    else if (strstr(filename, ".js")) content_type = "application/javascript";
    else if (strstr(filename, ".json")) content_type = "application/json";
    else if (strstr(filename, ".png")) content_type = "image/png";
    else if (strstr(filename, ".jpg") || strstr(filename, ".jpeg")) content_type = "image/jpeg";
    else if (strstr(filename, ".ico")) content_type = "image/x-icon";
    else if (strstr(filename, ".webp")) content_type = "image/webp";
    
    httpd_resp_set_type(req, content_type);
    
    // Set compression header if using compressed file
    if (use_compressed) {
        httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    }
    
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = file.read((uint8_t*)buffer, sizeof(buffer))) > 0) {
        if (httpd_resp_send_chunk(req, buffer, bytes_read) != ESP_OK) {
            file.close();
            return ESP_FAIL;
        }
    }
    
    file.close();
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

// Root/Index handler
esp_err_t WebServerManager::handleRootOrHash(httpd_req_t *req) {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    
    httpd_resp_set_type(req, "text/html");
    
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = file.read((uint8_t*)buffer, sizeof(buffer))) > 0) {
        // Simple template processing for VERSION
        std::string content(buffer, bytes_read);
        size_t pos = content.find("{{VERSION}}");
        if (pos != std::string::npos) {
            content.replace(pos, 11, esp_app_get_description()->version);
        }
        
        if (httpd_resp_send_chunk(req, content.c_str(), content.length()) != ESP_OK) {
            file.close();
            return ESP_FAIL;
        }
    }
    
    file.close();
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

// WiFi RSSI handler
esp_err_t WebServerManager::handleGetWifiRssi(httpd_req_t *req) {
    std::string rssi_val = std::to_string(WiFi.RSSI());
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, rssi_val.c_str(), rssi_val.length());
    return ESP_OK;
}

// Get configuration handler
esp_err_t WebServerManager::handleGetConfig(httpd_req_t *req) {
    WebServerManager* instance = getInstance(req);
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
    if (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) != ESP_OK) {
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    std::string type = type_param;
    std::string responseJson;

    if (type == "mqtt") {
        responseJson = instance->m_configManager.serializeToJson<espConfig::mqttConfig_t>();
    } else if (type == "misc" || type == "actions") {
        responseJson = instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
    } else if (type == "hkinfo") {
        const auto& readerData = instance->m_readerDataManager.getReaderData();
        cJSON *hkInfo = cJSON_CreateObject();
        cJSON_AddStringToObject(hkInfo, "group_identifier", fmt::format("{:02X}", fmt::join(readerData.reader_gid, "")).c_str());
        cJSON_AddStringToObject(hkInfo, "unique_identifier", fmt::format("{:02X}", fmt::join(readerData.reader_id, "")).c_str());
        cJSON *issuersArray = cJSON_CreateArray();
        for(const auto& issuer : readerData.issuers) {
            cJSON *issuerJson = cJSON_CreateObject();
            cJSON_AddStringToObject(issuerJson, "issuerId", fmt::format("{:02X}", fmt::join(issuer.issuer_id, "")).c_str());
            cJSON *endpointsArray = cJSON_CreateArray();
          for (const auto& endpoint : issuer.endpoints) {
            cJSON *ep = cJSON_CreateObject();
            cJSON_AddStringToObject(ep, "endpointId", fmt::format("{:02X}", fmt::join(endpoint.endpoint_id, "")).c_str()); 
            cJSON_AddItemToArray(endpointsArray, ep);
          }
          cJSON_AddItemToObject(issuerJson, "endpoints", endpointsArray);
          cJSON_AddItemToArray(issuersArray, issuerJson);
      }
        cJSON_AddItemToObject(hkInfo, "issuers", issuersArray);
        responseJson = cJSON_PrintUnformatted(hkInfo);
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

// Get Ethernet configuration handler
esp_err_t WebServerManager::handleGetEthConfig(httpd_req_t *req) {
    WebServerManager* instance = getInstance(req);
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
        cJSON_AddNumberToObject(rmii_conf, "pin_rmii_clock", v.rmii_conf.pin_rmii_clock);
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
    cJSON_AddBoolToObject(eth_config, "ethEnabled", instance->m_configManager.getConfig<espConfig::misc_config_t>().ethernetEnabled);
    char *payload = cJSON_PrintUnformatted(eth_config);
    cJSON_Delete(eth_config);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, payload, strlen(payload));
    free(payload);
    return ESP_OK;
}

// Save configuration handler
esp_err_t WebServerManager::handleSaveConfig(httpd_req_t *req) {
    WebServerManager* instance = getInstance(req);
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
    if (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) != ESP_OK) {
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    // Read request body
    char content[2048];
    int ret = httpd_req_recv(req, content, sizeof(content) - 1);
    if (ret <= 0) {
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, "Invalid request body", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }
    content[ret] = '\0';

    cJSON *obj = cJSON_Parse(content);
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
        std::string s = instance->m_configManager.serializeToJson<espConfig::mqttConfig_t>();
        data = cJSON_Parse(s.c_str());
        configSchema = cJSON_Duplicate(data, true);
    } else if(type == "misc" || type == "actions") {
        std::string s = instance->m_configManager.serializeToJson<espConfig::misc_config_t>();
        data = cJSON_Parse(s.c_str());
        configSchema = cJSON_Duplicate(data, true);
    } else {
        cJSON_Delete(obj);
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, "Invalid type!", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    if (!validateRequest(req, configSchema, content)) {
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
    while(it) {
      cJSON *configSchemaItem = cJSON_GetObjectItem(configSchema, it->string);
      if(cJSON_Compare(it, configSchemaItem, true)) {
        it = it->next;
        continue;
      }
      const std::string keyStr = it->string;
      if(keyStr == "setupCode"){
        EventValueChanged s{.name = keyStr, .str = it->valuestring};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        HomekitEvent event{.type = HomekitEventType::SETUP_CODE_CHANGED, .data = d};
        std::vector<uint8_t> event_data;
        alpaca::serialize(event, event_data);
        espp::EventManager::get().publish("homekit/event", event_data);
      } else if(keyStr == "nfcNeopixelPin"){
        rebootNeeded = true;
        rebootMsg = "Pixel GPIO pin was changed, reboot needed to apply! Rebooting...";
      } else if(keyStr == "nfcSuccessPin" || keyStr == "nfcFailPin" || keyStr == "gpioActionPin"){
        EventValueChanged s{.name = keyStr, .oldValue = static_cast<uint8_t>(configSchemaItem->valueint), .newValue = static_cast<uint8_t>(it->valueint)};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "hardware/gpioPinChanged",
            d);
      } else if(keyStr == "btrLowStatusThreshold"){
        EventValueChanged s{.name = "btrLowThreshold", .newValue = static_cast<uint8_t>(it->valueint)};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        HomekitEvent event{.type = HomekitEventType::BTR_PROP_CHANGED, .data = d};
        std::vector<uint8_t> event_data;
        alpaca::serialize(event, event_data);
        espp::EventManager::get().publish("homekit/event", event_data);
      } else if(keyStr == "neoPixelType") {
        rebootNeeded = true;
        rebootMsg = "Pixel Type was changed, reboot needed to apply! Rebooting...";
      } else if(keyStr == "gpioActionPin" && it->valueint != 255){
        cJSON_AddFalseToObject(obj, "hkDumbSwitchMode");
      }
      it = it->next;
    }

    // Merge changes and save configuration
    if (type == "mqtt") {
      mergeJson(data, obj);
      success = instance->m_configManager.deserializeFromJson<espConfig::mqttConfig_t>(cJSON_PrintUnformatted(data));
      if(success){
        success = instance->m_configManager.saveConfig<espConfig::mqttConfig_t>();
        rebootNeeded = true; // Always reboot on MQTT config change
        rebootMsg = "MQTT config saved, reboot needed to apply! Rebooting...";
      } else ESP_LOGE(TAG, "Could not deserialize from JSON!");
    } else if(type == "misc" || type == "actions") {
      mergeJson(data, obj);
      success = instance->m_configManager.deserializeFromJson<espConfig::misc_config_t>(cJSON_PrintUnformatted(data));
      if(success){
        success = instance->m_configManager.saveConfig<espConfig::misc_config_t>();
        if(type == "misc"){
          rebootNeeded = true;
          rebootMsg = "Misc config saved, reboot needed to apply! Rebooting...";
        }
      } else ESP_LOGE(TAG, "Could not deserialize from JSON!");
    }

    cJSON_Delete(data);
    cJSON_Delete(configSchema);
    cJSON_Delete(obj);

    if (success) {
        const char* response = rebootNeeded ? rebootMsg.c_str() : "Saved and applied!";
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

std::string getJsonValueAsString(cJSON* value) {
  char *str = cJSON_PrintUnformatted(value);
  std::string s(str);
  free(str);
  return s;
}

static void mergeJson(cJSON *target, cJSON *source) {
    cJSON *item = source->child;
    while (item) {
        cJSON *existing = cJSON_GetObjectItem(target, item->string);
        if (existing) {
            cJSON_ReplaceItemInObject(target, item->string, cJSON_Duplicate(item, true));
        } else {
            cJSON_AddItemToObject(target, item->string, cJSON_Duplicate(item, true));
        }
        item = item->next;
    }
}

bool WebServerManager::validateRequest(httpd_req_t *req, cJSON *currentData, const char* body) {
  uint8_t propertiesProcessed = 0;
  cJSON *obj = cJSON_Parse(body);
  if (!obj) {
    httpd_resp_set_status(req, "400 Bad Request");
    httpd_resp_send(req, "Invalid JSON", HTTPD_RESP_USE_STRLEN);
    return false;
  }

  cJSON *it = obj->child;
  while(it) {
    std::string keyStr = it->string;

    cJSON *existingValue = cJSON_GetObjectItem(currentData, keyStr.c_str());
    if (!existingValue) {
      ESP_LOGE(TAG, "\"%s\" does not exist in the current configuration!", keyStr.c_str());
      std::string msg = "\"" + std::string(keyStr) + "\" is not a valid configuration key.";
      httpd_resp_set_status(req, "400 Bad Request");
      httpd_resp_send(req, msg.c_str(), msg.length());
      cJSON_Delete(obj);
      return false;
    }

    cJSON *incomingValue = it;

    bool typeOk = false;
    if (cJSON_IsString(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS String",keyStr.c_str());
      typeOk = cJSON_IsString(incomingValue);
    } else if (cJSON_IsObject(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS OBJECT",keyStr.c_str());
      typeOk = cJSON_IsObject(incomingValue);
    } else if (cJSON_IsArray(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS ARRAY",keyStr.c_str());
      typeOk = cJSON_IsArray(incomingValue);
    } else if (cJSON_IsBool(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS BOOL",keyStr.c_str());
      typeOk = cJSON_IsBool(incomingValue) ||
               (cJSON_IsNumber(incomingValue) && (incomingValue->valueint == 0 || incomingValue->valueint == 1));
    } else if (cJSON_IsNumber(existingValue)) {
      ESP_LOGD(TAG, "VALUE FOR %s IS UINT",keyStr.c_str());
      typeOk = cJSON_IsNumber(incomingValue);
    } else {
      ESP_LOGD(TAG, "RX VALUE NOT MATCH ANY EXPECTED TYPE");
      typeOk = false;
    } 

    if (!typeOk) {
      ESP_LOGE(TAG, "Type mismatch for key \"%s\"!", keyStr.c_str());
      std::string valueStr = getJsonValueAsString(incomingValue);
      std::string msg = "Invalid type for key \"" + std::string(keyStr) + "\". Received value: " + valueStr;
      httpd_resp_set_status(req, "400 Bad Request");
      httpd_resp_send(req, msg.c_str(), msg.length());
      cJSON_Delete(obj);
      return false;
    }

    // --- setupCode Validation ---
    if (keyStr == "setupCode") {
      if (!cJSON_IsString(incomingValue)) {
        ESP_LOGE(TAG, "\"%s\" is not a string!", keyStr.c_str());
        std::string msg = "Value for \"" + std::string(keyStr) + "\" must be a string.";
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, msg.c_str(), msg.length());
        cJSON_Delete(obj);
        return false;
      }
      std::string code = incomingValue->valuestring;
      if (code.length() != 8 || std::find_if(code.begin(), code.end(), [](unsigned char c) { return !std::isdigit(c); }) != code.end()) {
        ESP_LOGE(TAG, "Invalid setupCode format for value: %s", code.c_str());
        std::string msg = "\"" + code + "\" is not a valid value for \"" + keyStr + "\". It must be an 8-digit number.";
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, msg.c_str(), msg.length());
        cJSON_Delete(obj);
        return false;
      }
      if (homeSpan.controllerListBegin() != homeSpan.controllerListEnd() && code.compare(cJSON_GetStringValue(existingValue)) != 0) {
        ESP_LOGE(TAG, "Attempted to change Setup Code while devices are paired.");
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, "The Setup Code can only be set if no devices are paired, reset if any issues!", HTTPD_RESP_USE_STRLEN);
        cJSON_Delete(obj);
        return false;
      }
    // --- Pin Validation ---
    } else if (keyStr.size() >= 3 && strcmp(keyStr.data() + (keyStr.size() - 3), "Pin") == 0) {
      if (!cJSON_IsNumber(incomingValue) || (incomingValue->valueint == 0)) {
        ESP_LOGE(TAG, "Invalid type or value for GPIO pin \"%s\"", keyStr.c_str());
        std::string msg = getJsonValueAsString(incomingValue) + " is not a valid value for \"" + keyStr + "\".";
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, msg.c_str(), msg.length());
        cJSON_Delete(obj);
        return false;
      }
      uint8_t pin = incomingValue->valueint;
      if (pin != 255 && !GPIO_IS_VALID_GPIO(pin) && !GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
        ESP_LOGE(TAG, "GPIO pin %u for \"%s\" is not a valid pin.", pin, keyStr.c_str());
        std::string msg = std::to_string(pin) + " is not a valid GPIO Pin for \"" + keyStr + "\".";
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
        ESP_LOGE(TAG, "\"%s\" could not validate! Expected a boolean (or 0/1).\n", keyStr.c_str());
        std::string msg = getJsonValueAsString(incomingValue) + " is not a valid value for \"" + keyStr + "\".";
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
    // This case should ideally not be reached if the loop returns on first error.
    // It's a fallback for unexpected issues.
    ESP_LOGE(TAG, "Not all properties could be validated, cannot continue!");
    httpd_resp_set_status(req, "500 Internal Server Error");
    httpd_resp_send(req, "Something went wrong during validation!", HTTPD_RESP_USE_STRLEN);
    cJSON_Delete(obj);
    return false;
  }

  cJSON_Delete(obj);
  return true;
}

// Clear configuration handler
esp_err_t WebServerManager::handleClearConfig(httpd_req_t *req) {
    WebServerManager* instance = getInstance(req);
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
    if (httpd_query_key_value(query, "type", type_param, sizeof(type_param)) != ESP_OK) {
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_send(req, "Missing 'type' parameter", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    std::string type = type_param;
    bool success = false;
    if (type == "mqtt") {
        success = instance->m_configManager.deleteConfig<espConfig::mqttConfig_t>();
    } else if (type == "misc" || type == "actions") {
        success = instance->m_configManager.deleteConfig<espConfig::misc_config_t>();
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

// Reboot handler
esp_err_t WebServerManager::handleReboot(httpd_req_t *req) {
    httpd_resp_send(req, "Rebooting...", HTTPD_RESP_USE_STRLEN);
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
    return ESP_OK;
}

// HomeKit reset handler
esp_err_t WebServerManager::handleHKReset(httpd_req_t *req) {
    WebServerManager* instance = getInstance(req);
    if (!instance) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_send(req, "Erasing HomeKit pairings and rebooting...", HTTPD_RESP_USE_STRLEN);
    instance->m_readerDataManager.deleteAllReaderData();
    homeSpan.processSerialCommand("H");
    return ESP_OK;
}

// WiFi reset handler
esp_err_t WebServerManager::handleWifiReset(httpd_req_t *req) {
    httpd_resp_send(req, "Erasing WiFi credentials and rebooting...", HTTPD_RESP_USE_STRLEN);
    homeSpan.processSerialCommand("X");
    return ESP_OK;
}

// Start config AP handler
esp_err_t WebServerManager::handleStartConfigAP(httpd_req_t *req) {
    httpd_resp_send(req, "Starting AP mode...", HTTPD_RESP_USE_STRLEN);
    vTaskDelay(pdMS_TO_TICKS(1000));
    homeSpan.processSerialCommand("A");
    return ESP_OK;
}

// Not found handler
esp_err_t WebServerManager::handleNotFound(httpd_req_t *req) {
    httpd_resp_send_404(req);
    return ESP_FAIL;
}
