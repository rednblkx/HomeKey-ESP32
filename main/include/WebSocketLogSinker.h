#pragma once

#include "cJSON.h"
#include "loggable.hpp"

class WebServerManager;

namespace loggable {

/**
 * Log sink that forwards log messages to a WebServerManager for WebSocket delivery.
 *
 * Runs on its own async consumer task (registered with Delivery::Async).
 * consume_batch() serializes a whole batch of log messages into a single
 * JSON payload of the form {"type":"logs","entries":[{...},{...},...]} and
 * broadcasts it as one WebSocket frame, so bursts cost one frame instead of
 * one frame per message.
 */
class WebSocketLogSinker : public ISink {
public:
    /**
     * Construct a WebSocketLogSinker bound to a WebServerManager.
     * @param webServerManager Server used to emit log messages; must remain valid for the sink's lifetime.
     */
    explicit WebSocketLogSinker(WebServerManager& webServerManager);
    ~WebSocketLogSinker() override = default;

    /**
     * Process and forward a single log message (also used for drop notices).
     * @param message Log message to be forwarded.
     */
    void consume(const LogMessage& message) override;

    /**
     * Process a batch of log messages as a single broadcast frame.
     * @param messages Array of log messages.
     * @param count Number of messages in the array.
     */
    void consume_batch(const LogMessage* messages, size_t count) override;

private:
    WebServerManager& m_webServerManager;
    /**
     * Convert a LogLevel value to its null-terminated string representation.
     * @param level Log level to convert.
     * @returns C-string name corresponding to `level`.
     */
    static const char* level_to_string(LogLevel level);

    /**
     * Serialize one message into a JSON object and append it to an array.
     * @param message Log message to serialize.
     * @param entries JSON array the new object is appended to.
     * @returns true if the object was created and appended.
     */
    static bool append_entry(const LogMessage& message, cJSON* entries);

    /**
     * Broadcast a JSON array of log entries as one {"type":"logs"} frame.
     * @param entries JSON array of log entry objects.
     */
    void broadcast_entries(cJSON* entries);
};

} // namespace loggable
