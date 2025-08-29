#pragma once
#include "ETH.h"
#include "HomeKey.h"
#include "structs.hpp"
#include <ArduinoJson.hpp>

using namespace ArduinoJson;

struct gpioLockAction {
  enum { HOMEKIT = 1, HOMEKEY = 2, OTHER = 3 };
  uint8_t source;
  uint8_t action;
};

// --- ArduinoJSON Converter Specializations ---

namespace ArduinoJson {

  template <typename T>
  struct Converter<std::vector<T> > {
    static void toJson(const std::vector<T>& src, JsonVariant dst) {
      JsonArray array = dst.to<JsonArray>();
      for (T item : src)
        array.add(item);
    }

    static std::vector<T> fromJson(JsonVariantConst src) {
      std::vector<T> dst;
      for (T item : src.as<JsonArrayConst>())
        dst.push_back(item);
      return dst;
    }

    static bool checkJson(JsonVariantConst src) {
      JsonArrayConst array = src;
      bool result = array;
      for (JsonVariantConst item : array)
        result &= item.is<T>();
      return result;
    }
  };

  template <typename T>
  struct Converter<std::map<std::string, T> > {
    static void toJson(const std::map<std::string, T>& src, JsonVariant dst) {
      JsonObject obj = dst.to<JsonObject>();
      for (const auto& item : src)
        obj[item.first] = item.second;
    }

    static std::map<std::string, T> fromJson(JsonVariantConst src) {
      std::map<std::string, T> dst;
      for (JsonPairConst item : src.as<JsonObjectConst>())
        dst[item.key().c_str()] = item.value().as<T>();
      return dst;
    }

    static bool checkJson(JsonVariantConst src) {
      JsonObjectConst obj = src;
      bool result = obj;
      for (JsonPairConst item : obj)
        result &= item.value().is<T>();
      return result;
    }
  };

  template <typename T, size_t N>
  struct Converter<std::array<T, N> > {
    static void toJson(const std::array<T, N>& src, JsonVariant dst) {
      JsonArray array = dst.to<JsonArray>();
      for (T item : src)
        array.add(item);
    }

    static std::array<T, N> fromJson(JsonVariantConst src) {
      std::array<T, N> dst;
      size_t idx = 0;
      for (T item : src.as<JsonArrayConst>())
        dst[idx++] = item;
      return dst;
    }

    static bool checkJson(JsonVariantConst src) {
      JsonArrayConst array = src;
      bool result = array;
      size_t size = 0;
      for (JsonVariantConst item : array) {
        result &= item.is<T>();
        size++;
      }
      return result && size == N;
    }
  };

 // hkEndpoint_t
  void convertToJson(const hkEndpoint_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, hkEndpoint_t& dst);

  // hkIssuer_t
  void convertToJson(const hkIssuer_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, hkIssuer_t& dst);

  // readerData_t
  void convertToJson(const readerData_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, readerData_t& dst);

  // eth_chip_desc_t
  void convertToJson(const eth_chip_desc_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, eth_chip_desc_t& dst);

#if CONFIG_ETH_USE_ESP32_EMAC
  // rmii_conf_t
  void convertToJson(const eth_board_presets_t::rmii_conf_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, eth_board_presets_t::rmii_conf_t& dst);
#endif

  // eth_board_presets_t::spi_conf_t
  void convertToJson(const eth_board_presets_t::spi_conf_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, eth_board_presets_t::spi_conf_t& dst);

  // eth_board_presets_t
  void convertToJson(const eth_board_presets_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, eth_board_presets_t& dst);

  // espConfig::mqttConfig_t
  void convertToJson(const espConfig::mqttConfig_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, espConfig::mqttConfig_t& dst);

  // std::map<espConfig::misc_config_t::colorMap, int>
  void convertToJson(const std::map<espConfig::misc_config_t::colorMap, int>& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, std::map<espConfig::misc_config_t::colorMap, int>& dst);

  // espConfig::misc_config_t
  void convertToJson(const espConfig::misc_config_t& src, JsonVariant dst);

  void convertFromJson(JsonVariantConst src, espConfig::misc_config_t& dst);
} // namespace ARDUINOJSON_NAMESPACE

struct ByteWriter {
  public:
    // Writes one byte, returns the number of bytes written (0 or 1)
    size_t write(uint8_t c);
    // Writes several bytes, returns the number of bytes written
    size_t write(const uint8_t *buffer, size_t length);
    void clear(){data.clear();};
    operator std::vector<uint8_t>&(){
      return data;
    }
    operator uint8_t*(){
      return data.data();
    }
    operator size_t(){
      return data.size();
    }
  private:
    std::vector<uint8_t> data;
};

struct ByteReader {
public:
  ByteReader(const uint8_t* buf, size_t len);
  int read();
  size_t readBytes(char* buffer, size_t length);
private:
  std::vector<uint8_t>::iterator it;
  std::vector<uint8_t> data;
};
