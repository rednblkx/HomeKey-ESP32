#pragma once
#include <cstdint>
#include <string>
#include <logging.hpp>
#include <mbedtls/base64.h>
#include <algorithm>
#include <vector>
#include <mbedtls/sha256.h>
#include <mbedtls/sha1.h>
#include <string.h>
#include <mbedtls/error.h>
class utils {
private:
  /* data */
public:
  static void pack(uint8_t *buf, size_t buflen, uint8_t *out, int *olen);
  static std::string bufToHexString(const uint8_t *buf, size_t len);
  static std::vector<uint8_t> encodeB64(const uint8_t *src, size_t len);
  static std::vector<uint8_t> decodeB64(const char *src);
  static std::vector<uint8_t> getHashIdentifier(uint8_t *key, size_t len, bool keyIdentifier);
  static std::vector<unsigned char> simple_tlv(unsigned char tag, const unsigned char *value, size_t valLength, unsigned char *out = NULL, size_t *olen = NULL);
};