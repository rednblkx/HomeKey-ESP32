#pragma once
#include "esp_http_server.h"
#include "cJSON.h"
#include "esp_timer.h"
#include <string>
#include <vector>
#include <mutex>

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
    // WebSocket endpoint handler
    static esp_err_t handleWebSocket(httpd_req_t *req);

    // --- Helper Methods ---
    static bool validateRequest(httpd_req_t *req, cJSON *currentData, const char* body);
    static WebServerManager* getInstance(httpd_req_t *req);
    static esp_err_t ws_send_text(httpd_handle_t server, int fd, const char* msg, size_t len);
    static void statusTimerCallback(void* arg){ WebServerManager* instance = static_cast<WebServerManager*>(arg); instance->broadcastToWebSocketClients(instance->getDeviceMetrics().c_str()); }
    std::string getDeviceMetrics();
    std::string getDeviceInfo();
    
    // --- WebSocket Management ---
    void addWebSocketClient(int fd);
    void removeWebSocketClient(int fd);
    void broadcastToWebSocketClients(const char* message);
    esp_err_t handleWebSocketMessage(httpd_req_t *req, const std::string& message);
    
    // --- Member Variables ---
    httpd_handle_t m_server;
    ConfigManager& m_configManager;
    ReaderDataManager& m_readerDataManager;
    esp_timer_handle_t m_statusTimer;
    
    // WebSocket client management
    std::vector<int> m_wsClients;
    std::mutex m_wsClientsMutex;

    static const char* TAG;
};


