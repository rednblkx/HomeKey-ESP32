#include "logging.h"
#include <iomanip>

#define LOG(x, format, ...) ESP_LOG##x(TAG, "%s > " format, __FUNCTION__ __VA_OPT__(, ) __VA_ARGS__)
namespace red_log
{
  std::string bufToHexString(const uint8_t* buf, size_t len, bool ignoreLevel) {
    std::string result;
    if (buf == NULL || buf == nullptr) {
      return result;
    }
    if (esp_log_level_get("*") >= esp_log_level_t::ESP_LOG_INFO || ignoreLevel) {
      result.reserve(2 * len);
      for (size_t i = 0; i < len; ++i) {
        result.push_back("0123456789ABCDEF"[buf[i] >> 4]);
        result.push_back("0123456789ABCDEF"[buf[i] & 0xF]);
      }
      // ESP_LOGV("bufToHexString", "%s", result.c_str());
    }
    return result;
  }
  std::string bufToHexString(const uint16_t* buf, size_t len, bool ignoreLevel) {
    std::string result;
    if (esp_log_level_get("*") >= esp_log_level_t::ESP_LOG_INFO || ignoreLevel) {
      result.reserve(4 * len); // Reserve space for 4 characters per uint16_t
      for (size_t i = 0; i < len; ++i) {
        result.push_back("0123456789ABCDEF"[(buf[i] >> 12) & 0xF]);
        result.push_back("0123456789ABCDEF"[(buf[i] >> 8) & 0xF]);
        result.push_back("0123456789ABCDEF"[(buf[i] >> 4) & 0xF]);
        result.push_back("0123456789ABCDEF"[buf[i] & 0xF]);
      }
      // ESP_LOGV("bufToHexString", "%s", result.c_str());
    }

    return result;
  }

  std::string int_to_hex(int i) {
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(2)
      << std::hex << i;
    return stream.str();
  }
};
