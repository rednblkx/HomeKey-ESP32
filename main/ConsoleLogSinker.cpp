#include "ConsoleLogSinker.h"
#include "fmt/color.h"
#include <cstdio>
#include <ctime>

namespace loggable {

void ConsoleLogSinker::consume(const LogMessage& message) {
    fmt::detail::color_type c = fmt::color::white;
    const char *level = "NONE";
    switch (message.get_level()) {
        case LogLevel::Error:
            c = fmt::color::crimson;
            level = "E";
            break;
        case LogLevel::Warning:
            c = fmt::color::orange;
            level = "W";
            break;
        case LogLevel::Info:
            c = fmt::color::light_green;
            level = "I";
            break;
        case LogLevel::Debug:
            c = fmt::color::light_blue;
            level = "D";
            break;
        case LogLevel::Verbose:
            c = fmt::color::light_cyan;
            level = "V";
            break;
        default:
            break;
    }

    fmt::memory_buffer buf;
    fmt::format_to(fmt::appender(buf), fmt::fg(c), "[{}][{}][{}] {}\n", std::chrono::duration_cast<std::chrono::milliseconds>(message.get_timestamp().time_since_epoch()).count(), level, message.get_tag(), message.get_message());
    // The console (USB-Serial/JTAG) can fail or short-write when no host is
    // reading; fmt::print would abort() on that, so write directly instead.
    std::fwrite(buf.data(), 1, buf.size(), stdout);
}

} // namespace loggable
