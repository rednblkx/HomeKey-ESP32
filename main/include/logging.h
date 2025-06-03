#pragma once
#include <string>
#include <cstdint>
#include <esp_log.h>

#define LOG(x, format, ...) ESP_LOG##x(TAG, "%s:%d > " format, __FUNCTION__ , __LINE__ __VA_OPT__(, ) __VA_ARGS__)
namespace red_log
{
  std::string bufToHexString(const uint8_t* buf, size_t len, bool ignoreLevel = true);
  std::string bufToHexString(const uint16_t* buf, size_t len, bool ignoreLevel = true);
  std::string int_to_hex(int i);
};