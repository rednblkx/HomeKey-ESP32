#pragma once
#include "cJSON.h"
#include "esp_http_server.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_timer.h"
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// Forward declarations
class ConfigManager;
class ReaderDataManager;
class SystemManager;
class MqttManager;
namespace loggable {
class WebSocketLogSinker;
}

// ============================================================================
// WebSocket Frame Structures
// ============================================================================

struct WsFrame {
  int fd;
  httpd_ws_type_t type;
  size_t len;
  uint8_t *payload;
};

struct WsFrameDeleter {
  void operator()(WsFrame *frame) const { delete frame; }
};

using WsFramePtr = std::unique_ptr<WsFrame, WsFrameDeleter>;

// ============================================================================
// WebServerManager Class
// ============================================================================

class WebServerManager {
public:
  // ------------------------------------------------------------------------
  // Public Interface
  // ------------------------------------------------------------------------
  WebServerManager(ConfigManager &configManager,
                   ReaderDataManager &readerDataManager);
  ~WebServerManager();

  void begin();
  void setMqttManager(MqttManager *mqttManager) { m_mqttManager = mqttManager; }
  void broadcastWs(const uint8_t *payload, size_t len, httpd_ws_type_t type);
  void broadcastToWebSocketClients(const char *message);

private:
  friend class loggable::WebSocketLogSinker;

  // ------------------------------------------------------------------------
  // Internal Types & Enums
  // ------------------------------------------------------------------------
  enum class OTAUploadType { FIRMWARE, LITTLEFS };

  struct WsClient {
    int fd;
    std::mutex mutex;
    WsClient(int file_descriptor) : fd(file_descriptor) {}
  };

  struct OTAAsyncRequest {
    httpd_req_t *req;
    size_t contentLength;
    OTAUploadType uploadType;
    bool skipReboot = false;
  };

#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
  struct BroadcastMsg {
    httpd_ws_type_t type;
    size_t len;
    uint8_t payload[];
  };

  struct BroadcastMsgDeleter {
    void operator()(BroadcastMsg *msg) const { free(msg); }
  };

  using BroadcastMsgPtr = std::unique_ptr<BroadcastMsg, BroadcastMsgDeleter>;
#endif

  // ------------------------------------------------------------------------
  // Static Task Callbacks
  // ------------------------------------------------------------------------
  static void ws_send_task(void *arg);
  static void otaWorkerTask(void *parameter);
  static void statusTimerCallback(void *arg);

  // ------------------------------------------------------------------------
  // HTTP Route Handlers (Static)
  // ------------------------------------------------------------------------
  static esp_err_t handleGetConfig(httpd_req_t *req);
  static esp_err_t handleGetEthConfig(httpd_req_t *req);
  static esp_err_t handleClearConfig(httpd_req_t *req);
  static esp_err_t handleSaveConfig(httpd_req_t *req);
  static esp_err_t handleReboot(httpd_req_t *req);
  static esp_err_t handleHKReset(httpd_req_t *req);
  static esp_err_t handleWifiReset(httpd_req_t *req);
  static esp_err_t handleStartConfigAP(httpd_req_t *req);
  static esp_err_t handleRootOrHash(httpd_req_t *req);
  static esp_err_t handleStaticFiles(httpd_req_t *req);
  static esp_err_t handleWebSocket(httpd_req_t *req);
  static esp_err_t handleOTAUpload(httpd_req_t *req);
  static esp_err_t handleCertificateUpload(httpd_req_t *req);
  static esp_err_t handleCertificateStatus(httpd_req_t *req);
  static esp_err_t handleCertificateDelete(httpd_req_t *req);

  // ------------------------------------------------------------------------
  // Core Internal Methods
  // ------------------------------------------------------------------------

  // Server setup
  void setupRoutes();

  // WebSocket management
  void addWebSocketClient(int fd);
  void removeWebSocketClient(int fd);
  void queue_ws_frame(int fd, const uint8_t *payload, size_t len,
                      httpd_ws_type_t type);
  esp_err_t handleWebSocketMessage(httpd_req_t *req,
                                   const std::string &message);

  // Device info/status
  std::string getDeviceMetrics();
  std::string getDeviceInfo();
  std::string getOTAStatus();
  void broadcastOTAStatus();

  // OTA management
  void initializeOTAWorker();
  void cleanupOTAAsync();
  esp_err_t otaUploadAsync(httpd_req_t *req);
  static esp_err_t queueOTARequest(httpd_req_t *req);

  // Utility methods
  static bool validateRequest(httpd_req_t *req, cJSON *currentData,
                              const char *body);
  static WebServerManager *getInstance(httpd_req_t *req);
  static esp_err_t ws_send_frame(httpd_handle_t server, int fd,
                                 const uint8_t *payload, size_t len,
                                 httpd_ws_type_t type = HTTPD_WS_TYPE_TEXT);

  // ------------------------------------------------------------------------
  // Member Variables
  // ------------------------------------------------------------------------

  // HTTP Server
  httpd_handle_t m_server;
  static const char *TAG;

  // Dependencies
  ConfigManager &m_configManager;
  ReaderDataManager &m_readerDataManager;
  MqttManager *m_mqttManager;

  // WebSocket infrastructure
  QueueHandle_t m_wsQueue;
  TaskHandle_t m_wsTaskHandle;
  std::vector<std::unique_ptr<WsClient>> m_wsClients;
  std::mutex m_wsClientsMutex;
  esp_timer_handle_t m_statusTimer;

#ifdef WEBSERVER_USE_DEDICATED_BROADCAST_QUEUE
  QueueHandle_t m_wsBroadcastQueue = nullptr;
#endif

  // OTA state
  esp_ota_handle_t m_otaHandle = 0;
  const esp_partition_t *m_updatePartition = nullptr;
  const esp_partition_t *m_littlefsPartition = nullptr;
  size_t m_otaWrittenBytes = 0;
  size_t m_otaTotalBytes = 0;
  bool m_otaInProgress = false;
  std::string m_otaError;
  OTAUploadType m_currentUploadType = OTAUploadType::FIRMWARE;
  bool m_skipReboot = false;
  std::mutex m_otaMutex;

  // OTA async worker
  TaskHandle_t m_otaWorkerHandle = nullptr;
  QueueHandle_t m_otaRequestQueue = nullptr;
  SemaphoreHandle_t m_otaWorkerReady = nullptr;
};
