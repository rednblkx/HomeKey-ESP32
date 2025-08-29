#ifndef WEBSERVER_MANAGER_HPP
#define WEBSERVER_MANAGER_HPP
#include "ArduinoJson.hpp"
#pragma once
#include "ESPAsyncWebServer.h"

class ConfigManager;
class ReaderDataManager;
class SystemManager;

/**
 * @class WebServerManager
 * @brief Manages the web-based configuration portal.
 *
 * This class initializes and configures the AsyncWebServer, sets up all API
 * and action endpoints, serves static files, and handles requests to view or
 * modify the device's configuration.
 */
class WebServerManager {
public:
    /**
     * @brief Constructs the WebServerManager.
     * @param configManager Reference to the configuration manager.
     * @param readerDataManager Reference to the HomeKey reader data manager.
     */
    WebServerManager(ConfigManager& configManager, ReaderDataManager& readerDataManager);

    /**
     * @brief Initializes the web server and attaches all handlers.
     */
    void begin();

private:
    // --- Route Setup ---
    void setupRoutes();

    // --- Request Handlers ---
    void handleGetConfig(AsyncWebServerRequest *request);
    void handleGetEthConfig(AsyncWebServerRequest *request);
    void handleClearConfig(AsyncWebServerRequest *request);
    void handleSaveConfigBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void processSaveConfigRequest(AsyncWebServerRequest *request);
    void handleReboot(AsyncWebServerRequest *request);
    void handleHKReset(AsyncWebServerRequest *request);
    void handleWifiReset(AsyncWebServerRequest *request);
    void handleStartConfigAP(AsyncWebServerRequest *request);
    void handleGetWifiRssi(AsyncWebServerRequest *request);
    void handleRootOrHash(AsyncWebServerRequest *request);
    void handleNotFound(AsyncWebServerRequest *request);

    // --- Helper Methods ---
    String indexProcessor(const String& var);
    bool validateRequest(AsyncWebServerRequest *request, ArduinoJson::JsonObjectConst &currentData);
    
    // --- Member Variables ---
    AsyncWebServer m_server;
    ConfigManager& m_configManager;
    ReaderDataManager& m_readerDataManager;

    // To temporarily store the body during multipart uploads
    // ArduinoJson::JsonDocument m_requestBodyJson;

    static const char* TAG;
};

#endif // WEBSERVER_MANAGER_HPP
