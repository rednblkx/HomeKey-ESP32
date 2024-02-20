#pragma once
#include <tuple>
#include <util/BERTLV.h>
#include "HomeKey.h"
#include <util/DigitalKeySecureContext.h>
#include <nvs.h>
#include <util/CommonCryptoUtils.h>
#include <chrono>
#include <auth/hkFastAuth.h>
#include <auth/hkStdAuth.h>
#include <auth/hkAttestationAuth.h>

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
  std::vector<uint8_t> commandFlow(homeKeyReader::CommandFlowStatus status);

public:
  HKAuthenticationContext(bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen), homeKeyReader::readerData_t &readerData);
  std::tuple<uint8_t *, uint8_t *, homeKeyReader::KeyFlow> authenticate(int, nvs_handle &);
};