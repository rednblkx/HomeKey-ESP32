#pragma once

#include "loggable.hpp"
#include <memory>

class WebServerManager;

namespace loggable {

/**
 * Log sink that forwards log messages to a WebServerManager for WebSocket delivery.
 *
 * Associates an ISink implementation with a WebServerManager to publish formatted
 * log messages to connected WebSocket clients.
 */
class WebSocketLogSinker : public ISink {
public:
    /**
     * Construct a WebSocketLogSinker bound to a WebServerManager.
     * @param webServerManager Pointer to the WebServerManager used to emit log messages; must remain valid for the sink's lifetime.
     */
    explicit WebSocketLogSinker(WebServerManager* webServerManager);
    ~WebSocketLogSinker() override = default;
    /**
     * Process and forward a log message to the associated WebServerManager.
     * @param message Log message to be forwarded.
     */
    void consume(const LogMessage& message) override;

private:
    WebServerManager* m_webServerManager;
    /**
     * Convert a LogLevel value to its null-terminated string representation.
     * @param level Log level to convert.
     * @returns C-string name corresponding to `level`.
     */
    const char* level_to_string(LogLevel level);
};

} // namespace loggable