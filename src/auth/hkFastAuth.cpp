#include <auth/hkFastAuth.h>

/**
 * The function `Auth0_keying_material` generates keying material using the HKDF algorithm based on
 * various input parameters.
 *
 * @param context The `context` parameter is a pointer to a character array that represents the context
 * for the keying material generation. It is used as input to the HKDF (HMAC-based Key Derivation
 * Function) algorithm.
 * @param ePub_X ePub_X is a pointer to a uint8_t array that represents the public key of the entity
 * being authenticated. It has a length of 32 bytes.
 * @param keyingMaterial The `keyingMaterial` parameter is a pointer to a buffer where the input keying material to be used with HKDF is stored.
 * It should have a size of at least 32 bytes.
 * @param out The `out` parameter is a pointer to the buffer where the output keying material will be
 * stored. The size of the buffer is specified by the `outLen` parameter.
 * @param outLen The parameter `outLen` represents the length of the output buffer `out`. It specifies
 * the maximum number of bytes that can be written to the `out` buffer.
 */
void HKFastAuth::Auth0_keying_material(const char *context, const uint8_t *ePub_X, const uint8_t *keyingMaterial, uint8_t *out, size_t outLen)
{
  uint8_t interface = 0x5E;
  uint8_t flags[2] = {0x01, 0x01};
  uint8_t prot_ver[4] = {0x5c, 0x02, 0x02, 0x0};
  uint8_t supported_vers[6] = {0x5c, 0x04, 0x02, 0x0, 0x01, 0x0};
  uint8_t dataMaterial[32 + strlen(context) + readerIdentifier.size() + 32 + 1 + sizeof(supported_vers) + sizeof(prot_ver) + readerEphX.size() + 16 + 2 + endpointEphX.size()];
  size_t olen = 0;
  utils::pack(&reader_key_X, 32, dataMaterial, &olen);
  utils::pack((uint8_t *)context, strlen(context), dataMaterial, &olen);
  utils::pack(readerIdentifier.data(), readerIdentifier.size(), dataMaterial, &olen);
  utils::pack(ePub_X, 32, dataMaterial, &olen);
  utils::pack(&interface, 1, dataMaterial, &olen);
  utils::pack(supported_vers, sizeof(supported_vers), dataMaterial, &olen);
  utils::pack(prot_ver, sizeof(prot_ver), dataMaterial, &olen);
  utils::pack(readerEphX.data(), readerEphX.size(), dataMaterial, &olen);
  utils::pack(transactionIdentifier.data(), 16, dataMaterial, &olen);
  utils::pack(flags, 2, dataMaterial, &olen);
  utils::pack(endpointEphX.data(), endpointEphX.size(), dataMaterial, &olen);
  LOG(D, "Auth0 HKDF Material: %s", utils::bufToHexString(dataMaterial, sizeof(dataMaterial)).c_str());
  int ret = mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), NULL, 0, keyingMaterial, 32, dataMaterial, sizeof(dataMaterial), out, outLen);
  LOG(V, "HKDF Status: %d", ret);
}

/**
 * The function `find_endpoint_by_cryptogram` searches for an endpoint in a list of issuers based on a
 * given cryptogram.
 *
 * @param cryptogram The parameter "cryptogram" is a vector of uint8_t, which represents a cryptogram.
 *
 * @return a pointer to an object of type `homeKeyEndpoint::endpoint_t`.
 */
std::tuple<homeKeyIssuer::issuer_t *, homeKeyEndpoint::endpoint_t *> HKFastAuth::find_endpoint_by_cryptogram(std::vector<uint8_t> &cryptogram)
{
  homeKeyEndpoint::endpoint_t *foundEndpoint = nullptr;
  homeKeyIssuer::issuer_t *foundIssuer = nullptr;
  for (auto &&issuer : issuers)
  {
    LOG(V, "Issuer: %s, Endpoints: %d", utils::bufToHexString(issuer.issuerId, sizeof(issuer.issuerId)).c_str(), issuer.endpoints.size());
    for (auto &&endpoint : issuer.endpoints)
    {
      LOG(V, "Endpoint: %s, Persistent Key: %s", utils::bufToHexString(endpoint.endpointId, sizeof(endpoint.endpointId)).c_str(), utils::bufToHexString(endpoint.persistent_key, sizeof(endpoint.persistent_key)).c_str());
      std::vector<uint8_t> hkdf(58, 0);
      Auth0_keying_material("VolatileFast", endpoint.endpoint_key_x, endpoint.persistent_key, hkdf.data(), hkdf.size());
      LOG(V, "HKDF Derived Key: %s", utils::bufToHexString(hkdf.data(), hkdf.size()).c_str());
      if (!memcmp(hkdf.data(), cryptogram.data(), 16))
      {
        LOG(D, "Endpoint %s matches cryptogram", utils::bufToHexString(endpoint.endpointId, sizeof(endpoint.endpointId)).c_str());
        foundEndpoint = &endpoint;
        foundIssuer = &issuer;
        break;
      }
    }
    if (foundEndpoint != nullptr)
    {
      break;
    }
  }
  return std::make_tuple(foundIssuer, foundEndpoint);
}

/**
 * Performs a fast authentication process using the given data and returns the
 * issuer, endpoint, and key flow status.
 *
 * @param data A pointer to an array of uint8_t (unsigned 8-bit integers) representing a TLV object
 * that should contain the endpoint's public key and a cryptogram
 * @param dataLen Length of the `data` array
 *
 * @return a tuple containing three elements: a pointer to the issuer, a pointer to the endpoint, and a
 * value of the enum type `homeKeyReader::KeyFlow`
 */
std::tuple<homeKeyIssuer::issuer_t *, homeKeyEndpoint::endpoint_t *, homeKeyReader::KeyFlow> HKFastAuth::attest(uint8_t *data, size_t dataLen)
{
  homeKeyIssuer::issuer_t *issuer = nullptr;
  homeKeyEndpoint::endpoint_t *endpoint = nullptr;
  if (data[dataLen - 2] == 0x90 && data[0] == 0x86)
  {
    auto Auth0Res = BERTLV::unpack_array(data, dataLen);
    auto endpointPubKey = BERTLV::findTag(kEndpoint_Public_Key, Auth0Res);
    endpointEphPubKey = endpointPubKey.value;
    auto encryptedMessage = BERTLV::findTag(kAuth0_Cryptogram, Auth0Res);
    endpointEphX = std::move(get_x(endpointEphPubKey));
    auto foundData = find_endpoint_by_cryptogram(encryptedMessage.value);
    endpoint = std::get<1>(foundData);
    issuer = std::get<0>(foundData);
    if (endpoint != nullptr)
    {
      LOG(D, "Endpoint %s Authenticated via FAST Flow", utils::bufToHexString(endpoint->endpointId, sizeof(endpoint->endpointId), true).c_str());
      return std::make_tuple(issuer, endpoint, homeKeyReader::kFlowFAST);
    }
    else
    {
      LOG(W, "FAST Flow failed!");
      return std::make_tuple(issuer, endpoint, homeKeyReader::kFlowSTANDARD);
    }
  }
  LOG(E, "Response not valid, something went wrong!");
  return std::make_tuple(issuer, endpoint, homeKeyReader::kFlowFailed);
}