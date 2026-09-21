#include "WebSocketLogSinker.h"
#include "WebServerManager.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fmt/core.h>
#include <fmt/format.h>

namespace loggable {

/**
 * @brief Constructs a WebSocketLogSinker that will broadcast formatted log messages.
 *
 * @param webServerManager WebServerManager used to emit log messages; must remain valid for the sink's lifetime.
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
static const char* level_to_string(LogLevel level) {
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
 * @brief Append @p s to @p out as a JSON string literal with escaping.
 *
 * Escapes the JSON-mandated characters (", \, control chars) and emits
 * control bytes as \u00XX. Tag/message strings come from the device itself,
 * but a malformed or hostile value must still not break the frame.
 */
static void append_json_string(fmt::memory_buffer& out, std::string_view s) {
    out.push_back('"');
    for (char c : s) {
        const unsigned char u = static_cast<unsigned char>(c);
        switch (c) {
        case '"':  out.push_back('\\'); out.push_back('"'); break;
        case '\\': out.push_back('\\'); out.push_back('\\'); break;
        case '\b': fmt::format_to(std::back_inserter(out), "\\b"); break;
        case '\f': fmt::format_to(std::back_inserter(out), "\\f"); break;
        case '\n': fmt::format_to(std::back_inserter(out), "\\n"); break;
        case '\r': fmt::format_to(std::back_inserter(out), "\\r"); break;
        case '\t': fmt::format_to(std::back_inserter(out), "\\t"); break;
        default:
            if (u < 0x20) {
                fmt::format_to(std::back_inserter(out), "\\u{:04x}", u);
            } else {
                out.push_back(c);
            }
        }
    }
    out.push_back('"');
}

/**
 * @brief Serialize one log entry into @p out as a JSON object.
 *
 * The object contains the message timestamp (ms since epoch), uptime (ms),
 * type ("log"), level, tag, and msg.
 */
static void append_entry(fmt::memory_buffer& out, const LogMessage& message) {
    fmt::format_to(std::back_inserter(out),
        "{{\"ts\":{},\"uptime\":{},\"type\":\"log\",\"level\":\"{}\",\"tag\":",
        std::chrono::duration_cast<std::chrono::milliseconds>(message.get_timestamp().time_since_epoch()).count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
        level_to_string(message.get_level()));
    append_json_string(out, message.get_tag());
    fmt::format_to(std::back_inserter(out), ",\"msg\":");
    append_json_string(out, message.get_message());
    out.push_back('}');
}

/**
 * @brief Broadcast a batch of log entries as one {"type":"logs"} frame.
 *
 * Builds the whole payload in a single fmt buffer and sends it via the
 * associated WebServerManager. On failure the entries are dropped silently:
 * logging must never block or throw.
 */
void WebSocketLogSinker::broadcast_entries(const LogMessage* messages, size_t count) {
    if (count == 0) {
        return;
    }

    fmt::memory_buffer out;
    fmt::format_to(std::back_inserter(out), "{{\"type\":\"logs\",\"entries\":[");
    for (size_t i = 0; i < count; ++i) {
        if (i > 0) {
            out.push_back(',');
        }
        append_entry(out, messages[i]);
    }
    fmt::format_to(std::back_inserter(out), "]}}");

    m_webServerManager.broadcastWs(reinterpret_cast<const uint8_t*>(out.data()), out.size(),
          HTTPD_WS_TYPE_TEXT);
}

/**
 * @brief Serializes a single log message to JSON and broadcasts it.
 *
 * Used for one-off messages such as drop notices. Batched messages go
 * through consume_batch() instead.
 */
void WebSocketLogSinker::consume(const LogMessage& message) {
    broadcast_entries(&message, 1);
}

/**
 * @brief Serializes a batch of log messages into one JSON payload and
 * broadcasts it as a single WebSocket frame.
 */
void WebSocketLogSinker::consume_batch(const LogMessage* messages, size_t count) {
    broadcast_entries(messages, count);
}

} // namespace loggable
