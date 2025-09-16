#pragma once
#include "esp_http_server.h"
#include "cJSON.h"
#include <string>

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
    static esp_err_t handleGetConfig(httpd_req_t *req);
    static esp_err_t handleGetEthConfig(httpd_req_t *req);
    static esp_err_t handleClearConfig(httpd_req_t *req);
    static esp_err_t handleSaveConfig(httpd_req_t *req);
    static esp_err_t handleReboot(httpd_req_t *req);
    static esp_err_t handleHKReset(httpd_req_t *req);
    static esp_err_t handleWifiReset(httpd_req_t *req);
    static esp_err_t handleStartConfigAP(httpd_req_t *req);
    static esp_err_t handleGetWifiRssi(httpd_req_t *req);
    static esp_err_t handleRootOrHash(httpd_req_t *req);
    static esp_err_t handleStaticFiles(httpd_req_t *req);
    static esp_err_t handleNotFound(httpd_req_t *req);

    // --- Helper Methods ---
    static std::string indexProcessor(const std::string& var);
    static bool validateRequest(httpd_req_t *req, cJSON *currentData, const char* body);
    static WebServerManager* getInstance(httpd_req_t *req);
    
    // --- Member Variables ---
    httpd_handle_t m_server;
    ConfigManager& m_configManager;
    ReaderDataManager& m_readerDataManager;

    static const char* TAG;
};


