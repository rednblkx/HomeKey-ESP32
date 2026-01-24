#pragma once

#include "loggable.hpp"

class WebServerManager;

namespace loggable {

class ConsoleLogSinker : public ISink {
public:
    ConsoleLogSinker() = default;

    void consume(const LogMessage& message) override;
};

} // namespace loggable
