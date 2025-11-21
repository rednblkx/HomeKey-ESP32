#include "WebSocketLogSinker.h"
#include "WebServerManager.hpp"
#include "cJSON.h"
#include "esp_timer.h"
#include <chrono>
#include <cstdint>
#include <ctime>
#include <ratio>

namespace loggable {

/**
 * @brief Constructs a WebSocketLogSinker that will broadcast formatted log messages.
 *
 * @param webServerManager Pointer to the WebServerManager used to broadcast messages to connected WebSocket clients. If `nullptr`, broadcasting is disabled.
 */
WebSocketLogSinker::WebSocketLogSinker(WebServerManager* webServerManager)
    : m_webServerManager(webServerManager) {
}

/**
 * @brief Convert a LogLevel value to its corresponding uppercase string label.
 *
 * @param level Log level to convert.
 * @return const char* One of `ERROR`, `WARN`, `INFO`, `DEBUG`, `VERBOSE`, or `NONE` when the level is unrecognized.
 */
const char* WebSocketLogSinker::level_to_string(LogLevel level) {
    switch (level) {
    case LogLevel::Error: return "ERROR";
    case LogLevel::Warning: return "WARN";
    case LogLevel::Info: return "INFO";
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Verbose: return "VERBOSE";
    default: return "NONE";
    }
}

/**
 * @brief Serializes a log message to JSON and broadcasts it to connected WebSocket clients.
 *
 * Creates a JSON object containing the message timestamp (seconds since epoch), type ("log"),
 * level, tag, and msg, then sends the serialized JSON to all WebSocket clients via the
 * associated WebServerManager. Safeguards against null web server manager and prevents
 * re-entrant broadcasts triggered by logging during a broadcast.
 *
 * @param message Log message to serialize and broadcast.
 */
void WebSocketLogSinker::consume(const LogMessage& message) {
    if (!m_webServerManager) return;

    // Re-entrancy guard to prevent nested broadcasts triggered by logging during a broadcast.
    static thread_local bool in_append = false;
    if (in_append) return; 

    struct Guard {
        bool& flag;
        explicit Guard(bool& f) : flag(f) { flag = true; }
        ~Guard() { flag = false; }
    } guard(in_append);

    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return; // Failed to create JSON object
    }
    
    cJSON_AddNumberToObject(root, "ts", std::chrono::duration_cast<std::chrono::milliseconds>(message.get_timestamp().time_since_epoch()).count());
    cJSON_AddNumberToObject(root, "uptime", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    cJSON_AddStringToObject(root, "type", "log");
    cJSON_AddStringToObject(root, "level", level_to_string(message.get_level()));
    cJSON_AddStringToObject(root, "tag", message.get_tag().c_str());
    cJSON_AddStringToObject(root, "msg", message.get_message().c_str());

    char* json_string = cJSON_PrintUnformatted(root);
    if (json_string) {
        m_webServerManager->broadcastWs(reinterpret_cast<const uint8_t *>(json_string), strlen(json_string),
              HTTPD_WS_TYPE_TEXT);
        cJSON_free(json_string);
    }

    cJSON_Delete(root);
}

} // namespace loggable
