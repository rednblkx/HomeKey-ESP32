#include "ConsoleLogSinker.h"
#include "fmt/color.h"
#include <ctime>

#ifdef CONFIG_IDF_TARGET_ESP32C3
#define LOCAL static
#else
#define LOCAL thread_local
#endif

namespace loggable {

void ConsoleLogSinker::consume(const LogMessage& message) {

    LOCAL bool flag = false;
    if (flag) return;

    struct Guard {
        bool& flag;
        explicit Guard(bool& f) : flag(f) { flag = true; }
        ~Guard() { flag = false; }
    } guard(flag);

    fmt::detail::color_type c = fmt::color::white;
    std::string level = "NONE";
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

    fmt::print(fmt::fg(c), "[{}][{}][{}] {}\n", std::chrono::duration_cast<std::chrono::milliseconds>(message.get_timestamp().time_since_epoch()).count(), level, message.get_tag(), message.get_message());
}

} // namespace loggable
