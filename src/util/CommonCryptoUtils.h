#pragma once
#include <vector>
#include <tuple>
#include <mbedtls/ecp.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/hkdf.h>
#include <mbedtls/md.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/error.h>
#include <esp_log.h>
#include <util/utils.h>
#include <esp_random.h>

class CommonCryptoUtils
{
private:
  const char *TAG = "CCUtils";

public:
  CommonCryptoUtils();
  std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> generateEphemeralKey();
  std::vector<uint8_t> signSharedInfo(uint8_t *stdTlv, size_t len, uint8_t *privateKey, size_t keyLen);
  std::vector<uint8_t> get_x(std::vector<uint8_t> &pubKey);
  std::vector<uint8_t> get_x(uint8_t *pubKey, size_t len);
  std::vector<uint8_t> getPublicKey(uint8_t *privKey, size_t len);
  static int esp_rng(void *, uint8_t *buf, size_t len);
};