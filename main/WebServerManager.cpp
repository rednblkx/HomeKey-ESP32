#include <event_manager.hpp>
#include "WebServerManager.hpp"
#include "ConfigManager.hpp"
#include "HomeSpan.h"
#include "ReaderDataManager.hpp"
#include "esp_log.h"
#include "eventStructs.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "eth_structs.hpp"
#include "fmt/base.h"
#include "fmt/ranges.h"
#include <LittleFS.h>
#include <esp_app_desc.h>
#include <string>
#include "cJSON.h"

const char* WebServerManager::TAG = "WebServerManager";

WebServerManager::WebServerManager(ConfigManager& configManager, ReaderDataManager& readerDataManager)
    : m_server(80),
      m_configManager(configManager),
      m_readerDataManager(readerDataManager)
{
  espp::EventManager::get().add_publisher("homekit/setupCodeChanged", "WebServerManager");
  espp::EventManager::get().add_publisher("homekit/btrPropChanged", "WebServerManager");
  espp::EventManager::get().add_publisher("mqtt/uidPublishChanged", "WebServerManager");
  espp::EventManager::get().add_publisher("hardware/gpioPinChanged", "WebServerManager");
}

void WebServerManager::begin() {
    ESP_LOGI(TAG, "Initializing...");
    
    if (!LittleFS.begin()) {
        ESP_LOGE(TAG, "Failed to mount LittleFS. Web server cannot start.");
        return;
    }

    setupRoutes();

    m_server.begin();
    ESP_LOGI(TAG, "Web server started.");
}

String WebServerManager::indexProcessor(const String& var) {
    if (var == "VERSION") {
        return esp_app_get_description()->version;
    }
    return "";
}

void WebServerManager::setupRoutes() {
    const espConfig::misc_config_t& miscConfig = m_configManager.getConfig<const espConfig::misc_config_t>();

    m_server.serveStatic("/assets", LittleFS, "/assets/").setFilter([](AsyncWebServerRequest* req){req->addInterestingHeader("ANY"); return true;});
    m_server.serveStatic("/fragment", LittleFS, "/routes").setFilter([](AsyncWebServerRequest* req){req->addInterestingHeader("ANY"); return true;});

    // Main web routes
    m_server.on("/", HTTP_GET, std::bind(&WebServerManager::handleRootOrHash, this, std::placeholders::_1));
    m_server.on("/#*", HTTP_GET, std::bind(&WebServerManager::handleRootOrHash, this, std::placeholders::_1));

    // API endpoints
    m_server.on("/config", HTTP_GET, std::bind(&WebServerManager::handleGetConfig, this, std::placeholders::_1));
    m_server.on("/config/clear", HTTP_POST, std::bind(&WebServerManager::handleClearConfig, this, std::placeholders::_1));
    m_server.on("/eth_get_config", HTTP_GET, std::bind(&WebServerManager::handleGetEthConfig, this, std::placeholders::_1));
    m_server.on("/reboot_device", HTTP_GET, std::bind(&WebServerManager::handleReboot, this, std::placeholders::_1));
    m_server.on("/reset_hk_pair", HTTP_GET, std::bind(&WebServerManager::handleHKReset, this, std::placeholders::_1));
    m_server.on("/reset_wifi_cred", HTTP_GET, std::bind(&WebServerManager::handleWifiReset, this, std::placeholders::_1));
    m_server.on("/start_config_ap", HTTP_GET, std::bind(&WebServerManager::handleStartConfigAP, this, std::placeholders::_1));
    m_server.on("/get_wifi_rssi", HTTP_GET, std::bind(&WebServerManager::handleGetWifiRssi, this, std::placeholders::_1));

    // Complex POST handler for saving config
    AsyncCallbackWebHandler* saveHandler = new AsyncCallbackWebHandler();
    saveHandler->setUri("/config/save");
    saveHandler->setMethod(HTTP_POST);
    saveHandler->onBody(std::bind(&WebServerManager::handleSaveConfigBody, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    saveHandler->onRequest(std::bind(&WebServerManager::processSaveConfigRequest, this, std::placeholders::_1));
    m_server.addHandler(saveHandler);

    m_server.onNotFound(std::bind(&WebServerManager::handleNotFound, this, std::placeholders::_1));

    if (miscConfig.webAuthEnabled) {
        ESP_LOGI(TAG, "Web authentication is enabled.");
        const char* user = miscConfig.webUsername.c_str();
        const char* pass = miscConfig.webPassword.c_str();
        // Protect all handlers except static assets
        m_server.on("/", HTTP_GET, std::bind(&WebServerManager::handleRootOrHash, this, std::placeholders::_1)).setAuthentication(user, pass);
        m_server.on("/#*", HTTP_GET, std::bind(&WebServerManager::handleRootOrHash, this, std::placeholders::_1)).setAuthentication(user, pass);
        m_server.on("/config", HTTP_GET, std::bind(&WebServerManager::handleGetConfig, this, std::placeholders::_1)).setAuthentication(user, pass);
        m_server.on("/config/clear", HTTP_POST, std::bind(&WebServerManager::handleClearConfig, this, std::placeholders::_1)).setAuthentication(user, pass);
        m_server.on("/reboot_device", HTTP_GET, std::bind(&WebServerManager::handleReboot, this, std::placeholders::_1)).setAuthentication(user, pass);
        m_server.on("/reset_hk_pair", HTTP_GET, std::bind(&WebServerManager::handleHKReset, this, std::placeholders::_1)).setAuthentication(user, pass);
        m_server.on("/reset_wifi_cred", HTTP_GET, std::bind(&WebServerManager::handleWifiReset, this, std::placeholders::_1)).setAuthentication(user, pass);
        m_server.on("/start_config_ap", HTTP_GET, std::bind(&WebServerManager::handleStartConfigAP, this, std::placeholders::_1)).setAuthentication(user, pass);
        m_server.on("/get_wifi_rssi", HTTP_GET, std::bind(&WebServerManager::handleGetWifiRssi, this, std::placeholders::_1)).setAuthentication(user, pass);
        saveHandler->setAuthentication(user, pass);
    }
}

// --- Request Handler Implementations ---
void WebServerManager::handleGetWifiRssi(AsyncWebServerRequest *request){
    std::string rssi_val = std::to_string(WiFi.RSSI());
    request->send(200, "text/plain", rssi_val.c_str());
}
void WebServerManager::handleRootOrHash(AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, std::bind(&WebServerManager::indexProcessor, this, std::placeholders::_1));
}

void WebServerManager::handleGetConfig(AsyncWebServerRequest *request) {
  if (!request->hasParam("type")) {
      request->send(400, "text/plain", "Missing 'type' parameter");
      return;
  }
  std::string type = request->getParam("type")->value().c_str();
  std::string responseJson;

  if (type == "mqtt") {
      responseJson = m_configManager.serializeToJson<espConfig::mqttConfig_t>();
  } else if (type == "misc" || type == "actions") {
      responseJson = m_configManager.serializeToJson<espConfig::misc_config_t>();
  } else if (type == "hkinfo") {
      const auto& readerData = m_readerDataManager.getReaderData();
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
    request->send(400, "text/plain", "Invalid 'type' parameter");
    return;
  }
  
  request->send(200, "application/json", responseJson.c_str());
}

void WebServerManager::handleGetEthConfig(AsyncWebServerRequest *request) {
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
    cJSON_AddBoolToObject(eth_config, "ethEnabled", m_configManager.getConfig<espConfig::misc_config_t>().ethernetEnabled);
    char *payload = cJSON_PrintUnformatted(eth_config);
    cJSON_Delete(eth_config);
    request->send(200, "application/json", payload);
    free(payload);
}

void WebServerManager::handleSaveConfigBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (len > 0) {
        cJSON *parsed = cJSON_ParseWithLength((const char*)data, len);
        if (!parsed) {
            ESP_LOGE(TAG, "Failed to parse JSON chunk.");
            request->_tempObject = nullptr;
            return;
        }
        request->_tempObject = parsed;
    } else request->_tempObject = nullptr;
}

std::string getJsonValueAsString(cJSON* value) {
  char *str = cJSON_PrintUnformatted(value);
  std::string s(str);
  free(str);
  return s;
}

bool WebServerManager::validateRequest(AsyncWebServerRequest *req, cJSON *currentData) {
  uint8_t propertiesProcessed = 0;
  cJSON *obj = (cJSON*)req->_tempObject;

  cJSON *it = obj->child;
  while(it) {
    std::string keyStr = it->string;

    cJSON *existingValue = cJSON_GetObjectItem(currentData, keyStr.c_str());
    if (!existingValue) {
      LOG(E, "\"%s\" does not exist in the current configuration!", keyStr.c_str());
      std::string msg = "\"" + std::string(keyStr) + "\" is not a valid configuration key.";
      req->send(400, "text/plain", msg.c_str());
      return false;
    }

    cJSON *incomingValue = it;

    bool typeOk = false;
    if (cJSON_IsString(existingValue)) {
      LOG(D, "VALUE FOR %s IS String",keyStr.c_str());
      typeOk = cJSON_IsString(incomingValue);
    } else if (cJSON_IsObject(existingValue)) {
      LOG(D, "VALUE FOR %s IS OBJECT",keyStr.c_str());
      typeOk = cJSON_IsObject(incomingValue);
    } else if (cJSON_IsArray(existingValue)) {
      LOG(D, "VALUE FOR %s IS ARRAY",keyStr.c_str());
      typeOk = cJSON_IsArray(incomingValue);
    } else if (cJSON_IsBool(existingValue)) {
      LOG(D, "VALUE FOR %s IS BOOL",keyStr.c_str());
      typeOk = cJSON_IsBool(incomingValue) ||
               (cJSON_IsNumber(incomingValue) && (incomingValue->valueint == 0 || incomingValue->valueint == 1));
    } else if (cJSON_IsNumber(existingValue)) {
      LOG(D, "VALUE FOR %s IS UINT",keyStr.c_str());
      typeOk = cJSON_IsNumber(incomingValue);
    } else {
      LOG(D, "RX VALUE NOT MATCH ANY EXPECTED TYPE");
      typeOk = false;
    } 

    if (!typeOk) {
      LOG(E, "Type mismatch for key \"%s\"!", keyStr.c_str());
      std::string valueStr = getJsonValueAsString(incomingValue);
      std::string msg = "Invalid type for key \"" + std::string(keyStr) + "\". Received value: " + valueStr;
      req->send(400, "text/plain", msg.c_str());
      return false;
    }

    // --- setupCode Validation ---
    if (keyStr == "setupCode") {
      if (!cJSON_IsString(incomingValue)) {
        LOG(E, "\"%s\" is not a string!", keyStr.c_str());
        std::string msg = "Value for \"" + std::string(keyStr) + "\" must be a string.";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
      std::string code = incomingValue->valuestring;
      if (code.length() != 8 || std::find_if(code.begin(), code.end(), [](unsigned char c) { return !std::isdigit(c); }) != code.end()) {
        LOG(E, "Invalid setupCode format for value: %s", code.c_str());
        std::string msg = "\"" + code + "\" is not a valid value for \"" + keyStr + "\". It must be an 8-digit number.";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
      if (homeSpan.controllerListBegin() != homeSpan.controllerListEnd() && code.compare(cJSON_GetStringValue(existingValue)) != 0) {
        LOG(E, "Attempted to change Setup Code while devices are paired.");
        req->send(400, "text/plain", "The Setup Code can only be set if no devices are paired, reset if any issues!");
        return false;
      }
    // --- Pin Validation ---
    } else if (keyStr.size() >= 3 && strcmp(keyStr.data() + (keyStr.size() - 3), "Pin") == 0) {
      if (!cJSON_IsNumber(incomingValue) || (incomingValue->valueint == 0)) {
        LOG(E, "Invalid type or value for GPIO pin \"%s\"", keyStr.c_str());
        std::string msg = getJsonValueAsString(incomingValue) + " is not a valid value for \"" + keyStr + "\".";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
      uint8_t pin = incomingValue->valueint;
      if (pin != 255 && !GPIO_IS_VALID_GPIO(pin) && !GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
        LOG(E, "GPIO pin %u for \"%s\" is not a valid pin.", pin, keyStr.c_str());
        std::string msg = std::to_string(pin) + " is not a valid GPIO Pin for \"" + keyStr + "\".";
        req->send(400, "text/plain", msg.c_str());
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
        LOG(E, "\"%s\" could not validate! Expected a boolean (or 0/1).\n", keyStr.c_str());
        std::string msg = getJsonValueAsString(incomingValue) + " is not a valid value for \"" + keyStr + "\".";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
    }

    propertiesProcessed++;
    it = it->next;
  }

  if (propertiesProcessed != cJSON_GetArraySize(obj)) {
    // This case should ideally not be reached if the loop returns on first error.
    // It's a fallback for unexpected issues.
    LOG(E, "Not all properties could be validated, cannot continue!");
    if (!req->client()->disconnected()) {
      req->send(500, "text/plain", "Something went wrong during validation!");
    }
    return false;
  }

  return true;
}

void merge(cJSON *dest, cJSON *src)
{
   cJSON *item = src->child;
   while(item) {
     cJSON_ReplaceItemInObject(dest, item->string, cJSON_Duplicate(item, true));
     item = item->next;
   }
}

void WebServerManager::processSaveConfigRequest(AsyncWebServerRequest *request) {
    if ((request->_tempObject == nullptr) || !request->hasParam("type")) {
        request->send(400, "text/plain", "Invalid request or missing type.");
        return;
    }
    String type = request->getParam("type")->value();
    
    cJSON *data = nullptr;
    cJSON *configSchema = nullptr;
    if (type == "mqtt") {
        std::string s = m_configManager.serializeToJson<espConfig::mqttConfig_t>();
        data = cJSON_Parse(s.c_str());
        configSchema = cJSON_Duplicate(data, true);
    } else if(type == "misc" || type == "actions") {
        std::string s = m_configManager.serializeToJson<espConfig::misc_config_t>();
        data = cJSON_Parse(s.c_str());
        configSchema = cJSON_Duplicate(data, true);
    } else {
      request->send(400, "text/plain", "Invalid type!");
    }
    if(!validateRequest(request, configSchema)){
      cJSON_Delete(data);
      cJSON_Delete(configSchema);
      return;
    }

    bool success = false;
    bool rebootNeeded = false;
    std::string rebootMsg;
    
    cJSON *obj = (cJSON*)request->_tempObject;

    cJSON *it = obj->child;
    while(it) {
      cJSON *configSchemaItem = cJSON_GetObjectItem(configSchema, it->string);
      if(cJSON_Compare(it, configSchemaItem, true)) {
        it = it->next;
        continue;
      }
      const char* keyStr = it->string;
      if(strcmp(keyStr, "nfcTagNoPublish") == 0){
        EventBinaryStatus s{.status = (bool)cJSON_IsTrue(it)};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "mqtt/uidPublishChanged",
            d);
      } else if(strcmp(keyStr, "setupCode") == 0){
        EventValueChanged s{.name = keyStr, .str = it->valuestring};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "homekit/setupCodeChanged",
            d);
      } else if(strcmp(keyStr, "nfcNeopixelPin") == 0){
        rebootNeeded = true;
        rebootMsg = "Pixel GPIO pin was changed, reboot needed to apply! Rebooting...";
      } else if(strcmp(keyStr, "nfcSuccessPin") == 0 || strcmp(keyStr, "nfcFailPin") == 0 || strcmp(keyStr, "gpioActionPin") == 0){
        EventValueChanged s{.name = keyStr, .oldValue = static_cast<uint8_t>(configSchemaItem->valueint), .newValue = static_cast<uint8_t>(it->valueint)};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "hardware/gpioPinChanged",
            d);
      } else if(strcmp(keyStr, "btrLowStatusThreshold") == 0){
        EventValueChanged s{.name = "btrLowThreshold", .newValue = static_cast<uint8_t>(it->valueint)};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "homekit/btrPropChanged",
            d);
      } else if(strcmp(keyStr, "neoPixelType") == 0) {
        rebootNeeded = true;
        rebootMsg = "Pixel Type was changed, reboot needed to apply! Rebooting...";
      } else if(strcmp(keyStr, "gpioActionPin") == 0 && it->valueint != 255){
        cJSON_SetBoolValue(cJSON_GetObjectItem(obj, "hkDumbSwitchMode"), false);
      }
      it = it->next;
    }

    if (type == "mqtt") {
      merge(data, obj);
      success = m_configManager.deserializeFromJson<espConfig::mqttConfig_t>(cJSON_PrintUnformatted(data));
      if(success){
        success = m_configManager.saveConfig<espConfig::mqttConfig_t>();
        rebootNeeded = true; // Always reboot on MQTT config change
        rebootMsg = "MQTT config saved, reboot needed to apply! Rebooting...";
      } else ESP_LOGE(TAG, "Could not deserialize from JSON!");
    } else if(type == "misc" || type == "actions") {
      merge(data, obj);
      success = m_configManager.deserializeFromJson<espConfig::misc_config_t>(cJSON_PrintUnformatted(data));
      if(success){
        success = m_configManager.saveConfig<espConfig::misc_config_t>();
        if(type == "misc"){
          rebootNeeded = true;
          rebootMsg = "Misc config saved, reboot needed to apply! Rebooting...";
        }
      } else ESP_LOGE(TAG, "Could not deserialize from JSON!");
    }

    cJSON_Delete(data);
    cJSON_Delete(configSchema);

    if (success) {
      request->send(200, "text/plain", rebootNeeded ? rebootMsg.c_str() : "Saved and applied!");
      if (rebootNeeded) {
          delay(1000);
          ESP.restart();
      }
    } else {
      request->send(500, "text/plain", "Failed to save configuration to NVS.");
    }
}
void WebServerManager::handleClearConfig(AsyncWebServerRequest *request) {
    if (!request->hasParam("type")) {
        request->send(400, "text/plain", "Missing 'type' parameter");
        return;
    }
    String type = request->getParam("type")->value();
    bool success = false;
    if (type == "mqtt") {
        success = m_configManager.deleteConfig<espConfig::mqttConfig_t>();
    } else if (type == "misc" || type == "actions") {
        success = m_configManager.deleteConfig<espConfig::misc_config_t>();
    }

    if (success) {
        request->send(200, "text/plain", "Cleared! Rebooting...");
        delay(1000);
        ESP.restart();
    } else {
        request->send(500, "text/plain", "Failed to clear configuration.");
    }
}

void WebServerManager::handleStartConfigAP(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Starting AP mode...");
    delay(1000);
    homeSpan.processSerialCommand("A");
}

void WebServerManager::handleReboot(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Rebooting...");
    delay(1000);
    ESP.restart();
}

void WebServerManager::handleHKReset(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Erasing HomeKit pairings and rebooting...");
    m_readerDataManager.deleteAllReaderData();
    homeSpan.processSerialCommand("H");
}

void WebServerManager::handleWifiReset(AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Erasing WiFi credentials and rebooting...");
    homeSpan.processSerialCommand("X");
}

void WebServerManager::handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}
