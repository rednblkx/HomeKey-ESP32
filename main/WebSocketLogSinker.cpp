#include "WebSocketLogSinker.h"
#include "WebServerManager.hpp"
#include "cJSON.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>

namespace loggable {

/**
 * @brief Constructs a WebSocketLogSinker that will broadcast formatted log messages.
 *
 * @param webServerManager WebServerManager used to broadcast messages to connected WebSocket clients.
 */
WebSocketLogSinker::WebSocketLogSinker(WebServerManager& webServerManager)
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
 * @brief Serialize one log message as a JSON object and append it to an array.
 *
 * The object contains the message timestamp (ms since epoch), uptime (ms),
 * type ("log"), level, tag, and msg.
 */
bool WebSocketLogSinker::append_entry(const LogMessage& message, cJSON* entries) {
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }

    cJSON_AddNumberToObject(root, "ts", std::chrono::duration_cast<std::chrono::milliseconds>(message.get_timestamp().time_since_epoch()).count());
    cJSON_AddNumberToObject(root, "uptime", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    cJSON_AddStringToObject(root, "type", "log");
    cJSON_AddStringToObject(root, "level", level_to_string(message.get_level()));
    cJSON_AddStringToObject(root, "tag", message.get_tag().c_str());
    cJSON_AddStringToObject(root, "msg", message.get_message().c_str());

    cJSON_AddItemToArray(entries, root);
    return true;
}

/**
 * @brief Broadcast a JSON array of log entries as one WebSocket frame.
 *
 * Wraps the array in {"type":"logs","entries":[...]} and sends it via the
 * associated WebServerManager. On serialization failure the entries are
 * dropped silently: logging must never block or throw.
 */
void WebSocketLogSinker::broadcast_entries(cJSON* entries) {
    if (entries == nullptr || cJSON_GetArraySize(entries) == 0) {
        return;
    }

    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }
    cJSON_AddStringToObject(root, "type", "logs");
    // Ownership of the array transfers to root; delete only root afterwards.
    cJSON_AddItemToObject(root, "entries", entries);

    char* json_string = cJSON_PrintUnformatted(root);
    if (json_string) {
        m_webServerManager.broadcastWs(reinterpret_cast<const uint8_t*>(json_string), strlen(json_string),
              HTTPD_WS_TYPE_TEXT);
        cJSON_free(json_string);
    }

    cJSON_Delete(root);
}

/**
 * @brief Serializes a single log message to JSON and broadcasts it.
 *
 * Used for one-off messages such as drop notices. Batched messages go
 * through consume_batch() instead.
 */
void WebSocketLogSinker::consume(const LogMessage& message) {
    cJSON* entries = cJSON_CreateArray();
    if (entries == nullptr) {
        return;
    }
    if (append_entry(message, entries)) {
        broadcast_entries(entries);
    } else {
        cJSON_Delete(entries);
    }
}

/**
 * @brief Serializes a batch of log messages into one JSON payload and
 * broadcasts it as a single WebSocket frame.
 */
void WebSocketLogSinker::consume_batch(const LogMessage* messages, size_t count) {
    cJSON* entries = cJSON_CreateArray();
    if (entries == nullptr) {
        return;
    }

    size_t appended = 0;
    for (size_t i = 0; i < count; ++i) {
        if (append_entry(messages[i], entries)) {
            ++appended;
        }
    }

    if (appended > 0) {
        broadcast_entries(entries);
    } else {
        cJSON_Delete(entries);
    }
}

} // namespace loggable
