#include <event_manager.hpp>
#include "WebServerManager.hpp"
#include "ConfigManager.hpp"
#include "HomeSpan.h"
#include "ReaderDataManager.hpp"
#include "eventStructs.hpp"
#include "utils.hpp"
#include "config.hpp"
#include <format.hpp>
#include <LittleFS.h>
#include <esp_app_desc.h>
#include <string>

const char* WebServerManager::TAG = "WebServerManager";

using namespace ArduinoJson;

WebServerManager::WebServerManager(ConfigManager& configManager, ReaderDataManager& readerDataManager)
    : m_server(80),
      m_configManager(configManager),
      m_readerDataManager(readerDataManager)
{
  espp::EventManager::get().add_publisher("homekit/setupCodeChanged", "WebServerManager");
  espp::EventManager::get().add_publisher("homekit/btrLowThresholdChanged", "WebServerManager");
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
    const auto& miscConfig = m_configManager.getMiscConfig();

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
    String type = request->getParam("type")->value();
    JsonDocument responseJson;

    if (type == "mqtt") {
        responseJson = m_configManager.getMqttConfig();
    } else if (type == "misc" || type == "actions") {
        responseJson = m_configManager.getMiscConfig();
    } else if (type == "hkinfo") {
        const auto& readerData = m_readerDataManager.getReaderData();
        JsonDocument hkInfo;
        hkInfo["group_identifier"] = fmt::format("{:02X}", fmt::join(readerData.reader_gid, ""));
        hkInfo["unique_identifier"] = fmt::format("{:02X}", fmt::join(readerData.reader_id, ""));
        JsonArray issuers = hkInfo["issuers"].to<JsonArray>();
        for(const auto& issuer : readerData.issuers) {
            JsonDocument issuerJson;
            issuerJson["issuerId"] = fmt::format("{:02X}", fmt::join(issuer.issuer_id, ""));
            JsonArray endpoints = issuerJson["endpoints"].to<JsonArray>();
            for (const auto& endpoint : issuer.endpoints) {
              JsonDocument ep;
              ep["endpointId"] = fmt::format("{:02X}", fmt::join(endpoint.endpoint_id, "")); 
              endpoints.add(ep);
            }
            issuers.add(issuerJson);
        }
        responseJson = hkInfo;
    } else {
        request->send(400, "text/plain", "Invalid 'type' parameter");
        return;
    }
    
    std::string payload;
    serializeJson(responseJson, payload);
    request->send(200, "application/json", payload.c_str());
}

void WebServerManager::handleGetEthConfig(AsyncWebServerRequest *request) {
    JsonDocument eth_config;
    JsonArray chips = eth_config["supportedChips"].to<JsonArray>();
    for (auto &&v : eth_config_ns::supportedChips) {
      chips.add(v.second);
    }
    eth_config["boardPresets"] = eth_config_ns::boardPresets;
    eth_config["ethEnabled"] = m_configManager.getMiscConfig().ethernetEnabled;
    std::string payload;
    serializeJson(eth_config, payload);
    request->send(200, "application/json", payload.c_str());
}

void WebServerManager::handleSaveConfigBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (len > 0) {
        JsonDocument *parsed = new JsonDocument;
        std::vector<uint8_t> d(data, data + len);
        deserializeJson(*parsed, d);
        if (parsed->isNull()) {
            ESP_LOGE(TAG, "Failed to parse JSON chunk.");
            request->_tempObject = nullptr;
            return;
        }
        request->_tempObject = parsed;
    } else request->_tempObject = nullptr;
}

std::string getJsonValueAsString(JsonVariantConst value) {
  std::string valueStr;
  serializeJson(value, valueStr);
  return valueStr;
}

bool WebServerManager::validateRequest(AsyncWebServerRequest *req, JsonObjectConst &currentData) {
  uint8_t propertiesProcessed = 0;
  JsonObjectConst obj = static_cast<JsonDocument*>(req->_tempObject)->as<JsonObjectConst>();

  for (JsonPairConst it : obj) {
    std::string keyStr = it.key().c_str();

    if (currentData[keyStr].isNull()) {
      LOG(E, "\"%s\" does not exist in the current configuration!", keyStr.c_str());
      std::string msg = "\"" + keyStr + "\" is not a valid configuration key.";
      req->send(400, "text/plain", msg.c_str());
      return false;
    }

    JsonVariantConst incomingValue = it.value();
    JsonVariantConst existingValue = currentData[keyStr];

    bool typeOk = false;
    if (existingValue.is<JsonString>()) {
      LOG(D, "VALUE FOR %s IS String",keyStr.c_str());
      typeOk = incomingValue.is<JsonString>();
    } else if (existingValue.is<JsonObjectConst>()) {
      LOG(D, "VALUE FOR %s IS OBJECT",keyStr.c_str());
      typeOk = incomingValue.is<JsonObjectConst>();
    } else if (existingValue.is<JsonArrayConst>()) {
      LOG(D, "VALUE FOR %s IS ARRAY",keyStr.c_str());
      typeOk = incomingValue.is<JsonArrayConst>();
    } else if (existingValue.is<bool>()) {
      LOG(D, "VALUE FOR %s IS BOOL",keyStr.c_str());
      typeOk = incomingValue.is<bool>() ||
               (incomingValue.is<int>() && (incomingValue.as<int>() == 0 || incomingValue.as<int>() == 1)) ||
               (incomingValue.is<unsigned int>() && (incomingValue.as<unsigned int>() <= 1));
    } else if (existingValue.is<unsigned int>()) {
      LOG(D, "VALUE FOR %s IS UINT",keyStr.c_str());
      typeOk = incomingValue.is<unsigned int>();
    } else {
      LOG(D, "RX VALUE NOT MATCH ANY EXPECTED TYPE");
      typeOk = false;
    } 

    if (!typeOk) {
      LOG(E, "Type mismatch for key \"%s\"!", keyStr.c_str());
      std::string valueStr;
      serializeJson(incomingValue, valueStr);
      std::string msg = "Invalid type for key \"" + std::string(keyStr) + "\". Received value: " + valueStr;
      req->send(400, "text/plain", msg.c_str());
      return false;
    }

    // --- setupCode Validation ---
    if (it.key() == "setupCode") {
      if (!it.value().is<const char *>()) {
        LOG(E, "\"%s\" is not a string!", keyStr.c_str());
        std::string msg = "Value for \"" + keyStr + "\" must be a string.";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
      std::string code = it.value().as<const char *>();
      if (code.length() != 8 || std::find_if(code.begin(), code.end(), [](unsigned char c) { return !std::isdigit(c); }) != code.end()) {
        LOG(E, "Invalid setupCode format for value: %s", code.c_str());
        std::string msg = "\"" + code + "\" is not a valid value for \"setupCode\". It must be an 8-digit number.";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
      if (homeSpan.controllerListBegin() != homeSpan.controllerListEnd() && code.compare(currentData[keyStr].as<const char *>()) != 0) {
        LOG(E, "Attempted to change Setup Code while devices are paired.");
        req->send(400, "text/plain", "The Setup Code can only be set if no devices are paired, reset if any issues!");
        return false;
      }
    // --- Pin Validation ---
    } else if (it.key().size() >= 3 && strcmp(keyStr.data() + (it.key().size() - 3), "Pin") == 0) {
      if (!it.value().is<uint8_t>() || (it.value().as<uint8_t>() == 0)) {
        LOG(E, "Invalid type or value for GPIO pin \"%s\"", keyStr.c_str());
        std::string msg = getJsonValueAsString(it.value()) + " is not a valid value for \"" + keyStr + "\".";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
      uint8_t pin = it.value().as<uint8_t>();
      if (pin != 255 && !GPIO_IS_VALID_GPIO(pin) && !GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
        LOG(E, "GPIO pin %u for \"%s\" is not a valid pin.", pin, keyStr.c_str());
        std::string msg = std::to_string(pin) + " is not a valid GPIO Pin for \"" + keyStr + "\".";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
    }

    // --- Boolean Validation ---
    if (currentData[keyStr].is<bool>()) {
      bool valueOk = false;
      if (it.value().is<bool>()) {
        valueOk = true;
      } else if (it.value().is<uint8_t>()) {
        uint8_t val = it.value().as<uint8_t>();
        if (val == 0 || val == 1) {
          (*static_cast<JsonDocument*>(req->_tempObject))[keyStr] = static_cast<bool>(val);
          valueOk = true;
        }
      }

      if (!valueOk) {
        LOG(E, "\"%s\" could not validate! Expected a boolean (or 0/1).", keyStr.c_str());
        std::string msg = getJsonValueAsString(it.value()) + " is not a valid value for \"" + keyStr + "\".";
        req->send(400, "text/plain", msg.c_str());
        return false;
      }
    }

    propertiesProcessed++;
  }

  if (propertiesProcessed != obj.size()) {
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

void merge(JsonObject dest, JsonObjectConst src)
{
   for (JsonPairConst kvp : src)
   {
     dest[kvp.key()] = kvp.value();
   }
}

void WebServerManager::processSaveConfigRequest(AsyncWebServerRequest *request) {
    if ((request->_tempObject != nullptr && static_cast<JsonDocument*>(request->_tempObject)->isNull()) || !request->hasParam("type")) {
        request->send(400, "text/plain", "Invalid request or missing type.");
        return;
    }
    String type = request->getParam("type")->value();
    
    JsonDocument data;
    JsonObjectConst configSchema;
    if (type == "mqtt") {
        data = m_configManager.getMqttConfig();
        configSchema = data.as<JsonObject>();
    } else {
        data = m_configManager.getMiscConfig();
        configSchema = data.as<JsonObject>();
    }
    if(!validateRequest(request, configSchema)){
      return;
    }

    bool success = false;
    bool rebootNeeded = false;
    std::string rebootMsg;
    
    JsonObjectConst obj = static_cast<JsonDocument*>(request->_tempObject)->as<JsonObjectConst>();

    for (JsonPairConst it : obj) {
      if(it.value() == configSchema[it.key()]) continue;
      const char* keyStr = it.key().c_str();
      if(it.key() == "nfcTagNoPublish"){
        EventBinaryStatus s{it.value().as<bool>()};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "mqtt/uidPublishChanged",
            d);
      } else if(it.key() == "setupCode"){
        EventValueChanged s{.name = keyStr, .str = it.value().as<const char *>()};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "homekit/setupCodeChanged",
            d);
      } else if(it.key() == "nfcNeopixelPin"){
        rebootNeeded = true;
        rebootMsg = "Pixel GPIO pin was changed, reboot needed to apply! Rebooting...";
      } else if(it.key() == "nfcSuccessPin" || it.key() == "nfcFailPin" || it.key() == "gpioActionPin"){
        EventValueChanged s{.name = keyStr, .oldValue = configSchema[keyStr], .newValue = it.value().as<uint8_t>()};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "hardware/gpioPinChanged",
            d);
      } else if(it.key() == "btrLowStatusThreshold"){
        EventValueChanged s{.newValue = it.value().as<uint8_t>()};
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        espp::EventManager::get().publish(
            "homekit/btrLowThresholdChanged",
            d);
      } else if(it.key() == "neoPixelType") {
        rebootNeeded = true;
        rebootMsg = "Pixel Type was changed, reboot needed to apply! Rebooting...";
      }
    }

    if (type == "mqtt") {
      merge(data.as<JsonObject>(), obj);
      espConfig::mqttConfig_t s = data.as<espConfig::mqttConfig_t>();
      success = m_configManager.saveMqttConfig(s);
      rebootNeeded = true; // Always reboot on MQTT config change
      rebootMsg = "MQTT config saved, reboot needed to apply! Rebooting...";
    } else if(type == "misc" || type == "actions") {
      merge(data.as<JsonObject>(), obj);
      espConfig::misc_config_t s = data.as<espConfig::misc_config_t>();
      success = m_configManager.saveMiscConfig(s);
      if(type == "misc"){
        rebootNeeded = true;
        rebootMsg = "Misc config saved, reboot needed to apply! Rebooting...";
      }
    }

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
        success = m_configManager.deleteMqttConfig();
    } else if (type == "misc" || type == "actions") {
        success = m_configManager.deleteMiscConfig();
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
