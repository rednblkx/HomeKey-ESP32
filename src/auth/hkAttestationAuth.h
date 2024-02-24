#include <util/CommonCryptoUtils.h>
#include <tuple>
#include "HomeKey.h"
#include <util/BERTLV.h>
#include <util/DigitalKeySecureContext.h>
#include <util/x963kdf.h>
#include <util/ndef.h>
#include <util/ISO18013SecureContext.h>

class HKAttestationAuth : CommonCryptoUtils
{
private:
  const char *TAG = "HKAttestAuth";
  std::list<homeKeyIssuer::issuer_t> &issuers;
  bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen){};
  std::vector<uint8_t> attestation_exchange_common_secret;
  DigitalKeySecureContext &DKSContext;
  std::vector<unsigned char> attestation_salt(std::vector<unsigned char> &env1Data, std::vector<unsigned char> &readerCmd);
  std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> envelope1Cmd();
  std::vector<unsigned char> envelope2Cmd(std::vector<uint8_t> &salt);
  std::tuple<homeKeyIssuer::issuer_t*, std::vector<uint8_t>, std::vector<uint8_t>> verify(std::vector<uint8_t> &decryptedCbor);

public:
  HKAttestationAuth(std::list<homeKeyIssuer::issuer_t> &issuers, DigitalKeySecureContext &context, bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen)) : issuers(issuers), nfcInDataExchange(nfcInDataExchange), DKSContext(context){};
  std::tuple<std::tuple<homeKeyIssuer::issuer_t *, std::vector<uint8_t>, std::vector<uint8_t>>, homeKeyReader::KeyFlow> attest();
};
