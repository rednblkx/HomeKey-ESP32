#pragma once
#include "cJSON.h"
#include "esp_http_server.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_timer.h"
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <atomic>
#include <string>
#include <vector>

// Forward declarations
class ConfigManager;
class NvsCredentialStore;
class SystemManager;
class MqttManager;
class NfcManager;
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
  const uint8_t *payload;
  std::shared_ptr<std::vector<uint8_t>> sharedData;
  uint32_t generation{0};
  static constexpr size_t INLINE_SIZE = 128;
  uint8_t inlinePayload[INLINE_SIZE];
};

struct WsFrameDeleter {
  void operator()(WsFrame *frame) const {
    delete frame; // payload is owned by inlinePayload or sharedData
  }
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
                   NvsCredentialStore &readerDataManager);
  ~WebServerManager();

  void begin();

  /**
   * @brief Stops the web server and cleans up all resources.
   *
   * Performs a complete shutdown of the web server by stopping the HTTP server,
   * deleting the WebSocket task and queue, and stopping/deleting the status timer.
   */
  void end();
  bool basicAuth(httpd_req_t* req);
  void setMqttManager(MqttManager *mqttManager) { m_mqttManager = mqttManager; }
  void setNfcManager(NfcManager *nfcManager) { m_nfcManager = nfcManager; }
  void broadcastWs(const uint8_t *payload, size_t len, httpd_ws_type_t type);
  void setWSBackLogSize(const uint16_t size);

  /**
   * @brief Number of WebSocket frames dropped because the send queue was full.
   *
   * Frames are dropped without blocking when the queue is full; this counter
   * makes that backpressure observable (e.g. in the metrics broadcast).
   */
  [[nodiscard]] uint64_t getWsFrameDropCount() const {
    return m_wsFrameDropped.load(std::memory_order_relaxed);
  }

private:
  // ------------------------------------------------------------------------
  // Internal Types & Enums
  // ------------------------------------------------------------------------

  struct WsClient {
    int fd;
    uint32_t generation;
    WsClient(int file_descriptor, uint32_t gen) : fd(file_descriptor), generation(gen) {}
  };

  enum class OTAUploadType { FIRMWARE, LITTLEFS };

  struct OTAState {
    esp_ota_handle_t handle = 0;
    const esp_partition_t *updatePartition = nullptr;
    const esp_partition_t *littlefsPartition = nullptr;
    size_t writtenBytes = 0;
    size_t totalBytes = 0;
    bool skipReboot = false;
    bool inProgress = false;
    std::string error;
    OTAUploadType currentUploadType = OTAUploadType::FIRMWARE;
  };

  struct OTAParams {
    httpd_req_t *req;
    WebServerManager *instance;
    OTAUploadType uploadType;
    bool skipReboot;
    size_t contentLength;
    OTAState *state;
  };

  // ------------------------------------------------------------------------
  // Static Task Callbacks
  // ------------------------------------------------------------------------
  static void ws_send_task(void *arg);
  static void otaTask(void *pvParameters);
  static void statusTimerCallback(void *arg);

  // ------------------------------------------------------------------------
  // HTTP Route Handlers (Static)
  // ------------------------------------------------------------------------
  static esp_err_t handleGetConfig(httpd_req_t *req);
  static esp_err_t handleGetEthConfig(httpd_req_t *req);
  static esp_err_t handleGetNfcPresets(httpd_req_t *req);
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

  static void captivePortalSaveTask(void* pvParameters);
  static void captivePortalEthSaveTask(void* pvParameters);
  static esp_err_t handleCaptivePortal(httpd_req_t *req);
  static esp_err_t handleGetCaptivePortalConfig(httpd_req_t *req);
  static esp_err_t handleSaveCaptivePortalConfig(httpd_req_t *req);
  static esp_err_t handleWifiScan(httpd_req_t *req);

  // ------------------------------------------------------------------------
  // Core Internal Methods
  // ------------------------------------------------------------------------

  // Server setup
  void setupRoutes();
  void setupCaptivePortalRoutes();

  // WebSocket management
  void addWebSocketClient(int fd);
  void removeWebSocketClient(int fd);
  void queue_ws_frame(int fd, const uint8_t *payload, size_t len,
                      httpd_ws_type_t type,
                      const std::shared_ptr<std::vector<uint8_t>>& shared = nullptr);
  esp_err_t handleWebSocketMessage(httpd_req_t *req,
                                   const std::string &message);

  // Device info/status
  std::string getDeviceMetrics();
  std::string getDeviceInfo();
  std::string getOTAInfo();
  // OTA management
  void broadcastOTAStatus(const OTAState& state);

  // Utility methods
  static bool validateRequest(httpd_req_t *req, cJSON *currentData,
                              cJSON *obj);
  static WebServerManager *getInstance(httpd_req_t *req);
  static esp_err_t sendAuthFailure(httpd_req_t *req);
  static esp_err_t ws_post_handshake_cb(httpd_req_t *req);
  static esp_err_t sendJsonError(httpd_req_t *req, const std::string &msg, 
                            const char *status = "400 Bad Request");
  static bool heapGuardOk(httpd_req_t *req, bool otherActive,
                                    const char *thisName, const char *otherName);
  bool shouldEnableHttps() const;

  // ------------------------------------------------------------------------
  // Member Variables
  // ------------------------------------------------------------------------

  // HTTP Server
  httpd_handle_t m_server;
  static const char *TAG;
  std::string m_sessionId;

  // Dependencies
  ConfigManager &m_configManager;
  NvsCredentialStore &m_readerDataManager;
  MqttManager *m_mqttManager;
  NfcManager *m_nfcManager;

  // WebSocket infrastructure
  QueueHandle_t m_wsQueue;
  TaskHandle_t m_wsTaskHandle;
  std::vector<std::unique_ptr<WsClient>> m_wsClients;
  std::mutex m_wsClientsMutex;
  std::atomic<uint32_t> m_wsClientGeneration{0};
  esp_timer_handle_t m_statusTimer;
  std::deque<std::vector<uint8_t>> m_wsBroadcastBuffer;
  std::mutex m_wsBroadcastMutex;
  static constexpr size_t kMaxBacklogBytes = 16 * 1024;
  static constexpr uint16_t kMaxBacklogFrames = 100;
  size_t m_wsBroadcastBytes{0};
  std::atomic<uint16_t> wsBacklogSize{0};
  std::atomic<uint64_t> m_wsFrameDropped{0};
  uint64_t m_lastReportedWsFrameDropped{0};
  std::atomic<bool> m_otaInProgress{false};
  bool m_isInitialized{false};

  std::mutex m_authDigestMutex;
  std::string m_authDigest;
  std::string m_authDigestUser;
  std::string m_authDigestPass;
  bool m_authDigestValid{false};
};
