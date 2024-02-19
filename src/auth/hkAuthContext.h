#pragma once
#include <tuple>
#include <esp_random.h>
#include <util/utils.h>
#include <mbedtls/ecp.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include "PN532.h"
#include <util/BERTLV.h>
#include <mbedtls/hkdf.h>
#include <mbedtls/md.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/ecdh.h>
#include "HomeKey.h"
#include <util/DigitalKeySecureContext.h>
#include <util/x963kdf.h>
#include <esp32-hal.h>
#include <nvs.h>
#include <util/CommonCryptoUtils.h>
#include <chrono>
#include <auth/hkFastAuth.h>

class HKAuthenticationContext : CommonCryptoUtils
{
private:
  const char *TAG = "HKAuthCtx";
  homeKeyReader::readerData_t &readerData;
  std::vector<uint8_t> readerEphX;
  std::vector<uint8_t> readerEphPrivKey;
  std::vector<uint8_t> readerEphPubKey;
  std::vector<uint8_t> endpointEphPubKey;
  std::vector<uint8_t> endpointEphX;
  bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen){};
  std::vector<uint8_t> transactionIdentifier;
  std::vector<uint8_t> readerIdentifier;
  void Auth1_keys_generator(uint8_t *persistentKey, uint8_t *volatileKey);
  void Auth1_keying_material(uint8_t *keyingMaterial, const char *context, uint8_t *out, size_t outLen);
  std::vector<uint8_t> commandFlow(homeKeyReader::CommandFlowStatus status);
  std::tuple<homeKeyIssuer::issuer_t *, homeKeyEndpoint::endpoint_t *, DigitalKeySecureContext, std::vector<uint8_t>, homeKeyReader::KeyFlow> std_auth();

public:
  HKAuthenticationContext(bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen), homeKeyReader::readerData_t &readerData);
  std::tuple<uint8_t *, uint8_t *, homeKeyReader::KeyFlow> authenticate(bool, nvs_handle &);
};