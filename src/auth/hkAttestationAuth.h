#include <util/CommonCryptoUtils.h>
#include <tuple>
#include "HomeKey.h"
#include <util/BERTLV.h>
#include <util/DigitalKeySecureContext.h>
#include <util/x963kdf.h>
#include <util/ndef.h>
#include <util/ISO18013SecureContext.h>

class HKAttestationAuth
{
private:
  const char *TAG = "HKAttestAuth";
  bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen){};
  std::vector<uint8_t> attestation_exchange_common_secret;
  DigitalKeySecureContext &DKSContext;
  std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> envelope1Cmd();
  std::vector<unsigned char> envelope2Cmd(std::vector<uint8_t> env1Data, std::vector<uint8_t> readerCmd, std::vector<uint8_t> shared_secret);

public:
  HKAttestationAuth(DigitalKeySecureContext &context, bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen)) : nfcInDataExchange(nfcInDataExchange), DKSContext(context){};
  homeKeyReader::KeyFlow attest();
};
