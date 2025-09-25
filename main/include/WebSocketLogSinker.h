#pragma once

#include "loggable.hpp"
#include <memory>

class WebServerManager; // Forward declaration

namespace loggable {

class WebSocketLogSinker : public ISink {
public:
    explicit WebSocketLogSinker(WebServerManager* webServerManager);
    ~WebSocketLogSinker() override = default;

    void consume(const LogMessage& message) override;

private:
    WebServerManager* m_webServerManager;
    const char* level_to_string(LogLevel level);
};

} // namespace loggable