#pragma once
#include <memory>
#include <atomic>
#include "esp_http_server.h"
#include "cJSON.h"
#include "esp_timer.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include <string>
#include <vector>
#include <mutex>
#include "esp_log.h"

class ConfigManager;
class ReaderDataManager;
class SystemManager;
class MqttManager;
namespace loggable {
    class WebSocketLogSinker;
}

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
     * @brief Sets the MQTT manager instance for certificate reconnection handling.
     * @param mqttManager Pointer to the MQTT manager instance.
     */
    void setMqttManager(MqttManager* mqttManager);

    /**
     * @brief Destructor - cleans up OTA resources.
     */
    ~WebServerManager();

    /**
     * @brief Initializes the web server and attaches all handlers.
     */
    void begin();
    
    void broadcastWs(const uint8_t * payload, size_t len, httpd_ws_type_t type);
    void broadcastToWebSocketClients(const char* message);

private:
    friend class loggable::WebSocketLogSinker;

    // WebSocket sending task
    static void ws_send_task(void* arg);
    void queue_ws_frame(int fd, const uint8_t* payload, size_t len, httpd_ws_type_t type);
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
    // OTA endpoint handlers
    static esp_err_t handleOTAUpload(httpd_req_t *req);
    static esp_err_t handleOTAReboot(httpd_req_t *req);
    
    // Certificate endpoint handlers
    static esp_err_t handleCertificateUpload(httpd_req_t *req);
    static esp_err_t handleCertificateStatus(httpd_req_t *req);
    static esp_err_t handleCertificateDelete(httpd_req_t *req);

    // --- Helper Methods ---
    static bool validateRequest(httpd_req_t *req, cJSON *currentData, const char* body);
    static WebServerManager* getInstance(httpd_req_t *req);
    static esp_err_t ws_send_frame(httpd_handle_t server, int fd, const uint8_t* payload, size_t len, httpd_ws_type_t type = HTTPD_WS_TYPE_TEXT);
    /**
     * @brief Timer callback that collects device metrics and broadcasts them to all WebSocket clients.
     *
     * Invoked by the status timer to obtain the current device metrics and send that data to connected
     * WebSocket clients via the manager's broadcast mechanism.
     *
     * @param arg Pointer to the WebServerManager instance that will produce and broadcast metrics; must be a valid WebServerManager*.
     */
    static void statusTimerCallback(void* arg){
        WebServerManager* instance = static_cast<WebServerManager*>(arg);
        // Ensure the string outlives c_str() usage during the call
        auto metrics = instance->getDeviceMetrics();
        instance->broadcastToWebSocketClients(metrics.c_str());
    }
    std::string getDeviceMetrics();
    std::string getDeviceInfo();
    std::string getOTAStatus();
    void broadcastOTAStatus();
    
    // --- WebSocket Management ---
    void addWebSocketClient(int fd);
    void removeWebSocketClient(int fd);
    esp_err_t handleWebSocketMessage(httpd_req_t *req, const std::string& message);
    
    // --- Member Variables ---
    httpd_handle_t m_server;
    QueueHandle_t m_wsQueue;
    TaskHandle_t m_wsTaskHandle;
    ConfigManager& m_configManager;
    ReaderDataManager& m_readerDataManager;
    esp_timer_handle_t m_statusTimer;
    MqttManager* m_mqttManager; // Pointer to MQTT manager for certificate reconnection
    
    // WebSocket client management
    struct WsClient {
        int fd;
        std::mutex mutex;
        /**
         * @brief Construct a WsClient for the specified client socket descriptor.
         *
         * @param file_descriptor File descriptor identifying the WebSocket client connection.
         */
        WsClient(int file_descriptor) : fd(file_descriptor) {}
    };
    std::vector<std::unique_ptr<WsClient>> m_wsClients;

    // Locking rules for m_wsClientsMutex:
    //  - Only protect structural access to m_wsClients (add/remove/iterate/find).
    //  - NEVER call ESP_LOG* or esp_timer_* while holding this mutex.
    //  - NEVER perform network I/O (httpd_ws_send_frame_async) while holding this mutex.
    //  - Take at most for short critical sections, and release before any callbacks.
    std::mutex m_wsClientsMutex;

    // Optional dedicated broadcast queue; when enabled via WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE,
    // broadcastWs enqueues messages and ws_send_task snapshots clients and sends.
#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
    struct BroadcastMsg {
        httpd_ws_type_t type;
        size_t len;
        uint8_t payload[];
    };
    QueueHandle_t m_wsBroadcastQueue = nullptr;
#endif
    
    // OTA upload types
    enum class OTAUploadType {
        FIRMWARE,
        LITTLEFS
    };
    
    // OTA update management
    esp_ota_handle_t m_otaHandle = 0;
    const esp_partition_t* m_updatePartition = nullptr;
    const esp_partition_t* m_littlefsPartition = nullptr;
    size_t m_otaWrittenBytes = 0;
    size_t m_otaTotalBytes = 0;
    bool m_otaInProgress = false;
    std::string m_otaError = "";
    OTAUploadType m_currentUploadType = OTAUploadType::FIRMWARE;
    bool m_skipReboot = false;
    std::mutex m_otaMutex;
    
    // Async OTA support
    TaskHandle_t m_otaWorkerHandle = nullptr;
    QueueHandle_t m_otaRequestQueue = nullptr;
    SemaphoreHandle_t m_otaWorkerReady = nullptr;
    static void otaWorkerTask(void* parameter);
    esp_err_t otaUploadAsync(httpd_req_t *req);
    static esp_err_t queueOTARequest(httpd_req_t *req);
    void initializeOTAWorker();
    void cleanupOTAAsync();
    
    struct WsFrame {
        int fd;
        httpd_ws_type_t type;
        size_t len;
        uint8_t payload[]; // Flexible array member
    };

    struct OTAAsyncRequest {
        httpd_req_t* req;
        size_t contentLength;
        OTAUploadType uploadType;
        bool skipReboot = false; // For sequential uploads
    };

    static const char* TAG;
};


