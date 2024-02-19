#include <util/CommonCryptoUtils.h>
#include <tuple>
#include "HomeKey.h"
#include <util/BERTLV.h>
#include <util/DigitalKeySecureContext.h>
#include <util/x963kdf.h>

class HKStdAuth : CommonCryptoUtils
{
private:
  const char *TAG = "HKStdAuth";
  uint8_t &reader_private_key;
  std::vector<uint8_t> readerEphPrivKey;
  std::list<homeKeyIssuer::issuer_t> &issuers;
  std::vector<uint8_t> &readerEphX;
  std::vector<uint8_t> &endpointEphPubKey;
  std::vector<uint8_t> &endpointEphX;
  std::vector<uint8_t> &transactionIdentifier;
  std::vector<uint8_t> &readerIdentifier;
  bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen){};
  void Auth1_keys_generator(uint8_t *persistentKey, uint8_t *volatileKey);
  void Auth1_keying_material(uint8_t *keyingMaterial, const char *context, uint8_t *out, size_t outLen);

public:
  HKStdAuth(bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen), uint8_t &reader_private_key, std::vector<uint8_t> &readerEphPrivKey, std::list<homeKeyIssuer::issuer_t> &issuers, std::vector<uint8_t> &readerEphX, std::vector<uint8_t> &endpointEphPubKey, std::vector<uint8_t> &endpointEphX, std::vector<uint8_t> &transactionIdentifier, std::vector<uint8_t> &readerIdentifier) : reader_private_key(reader_private_key), readerEphPrivKey(readerEphPrivKey), issuers(issuers), readerEphX(readerEphX), endpointEphPubKey(endpointEphPubKey), endpointEphX(endpointEphX), transactionIdentifier(transactionIdentifier), readerIdentifier(readerIdentifier), nfcInDataExchange(nfcInDataExchange) {};
  std::tuple<homeKeyIssuer::issuer_t *, homeKeyEndpoint::endpoint_t *, DigitalKeySecureContext, std::vector<uint8_t>, homeKeyReader::KeyFlow> attest();
};