#include <auth/authContext.h>
#include "authContext.h"

/**
 * The function `get_public_points` takes a public key as input, reads the X and Y coordinates of the
 * corresponding elliptic curve point, and returns them as separate vectors.
 * 
 * @param pubKey The `pubKey` parameter is a vector of `uint8_t` representing the public key.
 * 
 * @return The function `get_public_points` returns a tuple containing two vectors of `uint8_t` values.
 * The first vector contains the X coordinates of the public key point, and the second vector contains
 * the Y coordinates of the public key point.
 */
std::vector<uint8_t> AuthenticationContext::get_x(std::vector<uint8_t> pubKey)
{
  mbedtls_ecp_group grp;
  mbedtls_ecp_point point;
  mbedtls_ecp_point_init(&point);
  mbedtls_ecp_group_init(&grp);
  mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
  int ret = mbedtls_ecp_point_read_binary(&grp, &point, pubKey.data(), pubKey.size());
  logV("mbedtls_ecp_point_read_binary status: %d", ret);
  size_t buffer_size_x = mbedtls_mpi_size(&point.X);
  std::vector<uint8_t> X;
  X.resize(buffer_size_x);
  X.reserve(buffer_size_x);
  mbedtls_mpi_write_binary(&point.X, X.data(), mbedtls_mpi_size(&point.X));
  logV("PublicKey: %s, X Coordinate: %s", utils::bufToHexString(pubKey.data(), pubKey.size()).c_str(), utils::bufToHexString(X.data(), sizeof(X)).c_str());
  mbedtls_ecp_group_free(&grp);
  mbedtls_ecp_point_free(&point);
  return X;
}

int esp_rng(void *, uint8_t *buf, size_t len){
  esp_fill_random(buf, len);
  return 0;
}

/**
 * The function generates an ephemeral key pair using the secp256r1 elliptic curve and returns the
 * private and public keys as vectors of uint8_t.
 * 
 * @return The function `generateEphemeralKey` returns a `std::tuple` containing two elements:
 */
std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> AuthenticationContext::generateEphemeralKey()
{
  mbedtls_ecp_keypair ephemeral;
  mbedtls_ecp_keypair_init(&ephemeral);
  mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, &ephemeral, esp_rng, NULL);
  unsigned char bufPriv[mbedtls_mpi_size(&ephemeral.d)];
  mbedtls_mpi_write_binary(&ephemeral.d, bufPriv, sizeof(bufPriv));
  unsigned char bufPub[MBEDTLS_ECP_MAX_BYTES];
  size_t olen = 0;
  mbedtls_ecp_point_write_binary(&ephemeral.grp, &ephemeral.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, bufPub, sizeof(bufPub));
  mbedtls_ecp_keypair_free(&ephemeral);
  logD("Ephemeral Key generated");
  return std::make_tuple(std::vector<uint8_t>(bufPriv, bufPriv + sizeof(bufPriv)), std::vector<uint8_t>(bufPub, bufPub + olen));
}
/**
 * The function signSharedInfo signs the given data using the private key and returns the signature in
 * TLV format.
 * 
 * @param stdTlv A pointer to an array of uint8_t values representing a TLV (Tag-Length-Value)
 * structure.
 * @param len The `len` parameter represents the length of the `stdTlv` array.
 * 
 * @return a std::vector<uint8_t> containing a TLV (Tag-Length-Value) structure. The TLV structure
 * consists of a tag (0x9E) and the signature point (sigPoint) as the value.
 */
std::vector<uint8_t> AuthenticationContext::signSharedInfo(uint8_t *stdTlv, size_t len)
{
  mbedtls_ecp_keypair keypair;
  mbedtls_ecp_keypair_init(&keypair);

  mbedtls_mpi sigMpi1;
  mbedtls_mpi sigMpi2;
  mbedtls_mpi_init(&sigMpi1);
  mbedtls_mpi_init(&sigMpi2);

  uint8_t hash[32];

  mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), stdTlv, len, hash);

  mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &keypair, readerData->reader_private_key, 32);

  mbedtls_ecdsa_sign_det_ext(&keypair.grp, &sigMpi1, &sigMpi2, &keypair.d, hash, 32, MBEDTLS_MD_SHA256, esp_rng, NULL);
  uint8_t sigPoint[mbedtls_mpi_size(&sigMpi1) + mbedtls_mpi_size(&sigMpi2)];
  mbedtls_mpi_write_binary(&sigMpi1, sigPoint, mbedtls_mpi_size(&sigMpi1));
  mbedtls_mpi_write_binary(&sigMpi2, sigPoint + mbedtls_mpi_size(&sigMpi1), mbedtls_mpi_size(&sigMpi2));
  mbedtls_ecp_keypair_free(&keypair);
  mbedtls_mpi_free(&sigMpi1);
  mbedtls_mpi_free(&sigMpi2);
  return utils::simple_tlv(0x9E, sigPoint, sizeof(sigPoint));
}

/**
 * The function performs an elliptic curve Diffie-Hellman key exchange to compute a shared key between
 * the reader and the endpoint.
 * 
 * @param outBuf The `outBuf` parameter is a pointer to a buffer where the computed shared key will be
 * stored. It should be allocated with enough space to hold `oLen` bytes.
 * @param oLen oLen is the length of the output buffer (outBuf) where the shared key will be written.
 */
void AuthenticationContext::get_shared_key(uint8_t *outBuf, size_t oLen)
{
  mbedtls_ecp_group grp;
  mbedtls_mpi reader_ephemeral_private_key, shared_key;
  mbedtls_ecp_point endpoint_ephemeral_public_key;

  mbedtls_ecp_group_init(&grp);
  mbedtls_mpi_init(&reader_ephemeral_private_key);
  mbedtls_mpi_init(&shared_key);
  mbedtls_ecp_point_init(&endpoint_ephemeral_public_key);

  // Initialize the elliptic curve group (e.g., SECP256R1)
  mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);

  // Set the reader's ephemeral private key
  mbedtls_mpi_read_binary(&reader_ephemeral_private_key, readerEphPrivKey.data(), readerEphPrivKey.size());

  // Set the endpoint's ephemeral public key
  mbedtls_ecp_point_read_binary(&grp, &endpoint_ephemeral_public_key, endpointEphPubKey.data(), endpointEphPubKey.size());

  // Perform key exchange
  mbedtls_ecdh_compute_shared(&grp, &shared_key, &endpoint_ephemeral_public_key, &reader_ephemeral_private_key,
                              esp_rng, NULL);

  mbedtls_mpi_write_binary(&shared_key, outBuf, oLen);

  mbedtls_ecp_group_free(&grp);
  mbedtls_mpi_free(&reader_ephemeral_private_key);
  mbedtls_mpi_free(&shared_key);
  mbedtls_ecp_point_free(&endpoint_ephemeral_public_key);
}

AuthenticationContext::AuthenticationContext(const AuthenticationContext &ctx) : SimpleLoggable("HKAuthFlow")
{
  readerData = ctx.readerData;
  readerEphX = ctx.readerEphX;
  readerEphPrivKey = ctx.readerEphPrivKey;
  readerEphPubKey = ctx.readerEphPubKey;
  endpointEphPubKey = ctx.endpointEphPubKey;
  endpointEphX = ctx.endpointEphX;
  nfc = ctx.nfc;
  transactionIdentifier = ctx.transactionIdentifier;
  readerIdentifier = ctx.readerIdentifier;
}

/**
 * The function `AuthenticationContext::AuthenticationContext` initializes the AuthenticationContext
 * object by generating an ephemeral key, filling the transaction identifier with random data, and
 * copying reader identifiers.
 * 
 * @param nfc A pointer to an instance of the PN532 class, which is used for NFC communication.
 * @param readerData readerData is a pointer to a structure of type readerData_t.
 */
AuthenticationContext::AuthenticationContext(PN532 *nfc, homeKeyReader::readerData_t *readerData) : SimpleLoggable("HKAuthFlow"), nfc(nfc), readerData(readerData)
{
  auto readerEphKey = generateEphemeralKey();
  readerEphPrivKey = std::get<0>(readerEphKey);
  readerEphPubKey = std::get<1>(readerEphKey);
  transactionIdentifier.resize(16);
  transactionIdentifier.reserve(16);
  esp_fill_random(transactionIdentifier.data(), 16);
  readerIdentifier.insert(readerIdentifier.begin(), readerData->reader_identifier, readerData->reader_identifier + sizeof(readerData->reader_identifier));
  readerIdentifier.insert(readerIdentifier.end(), readerData->identifier, readerData->identifier + sizeof(readerData->identifier));
  readerEphX = get_x(readerEphPubKey);
}

/**
 * The function `Auth0_keying_material` generates keying material using the HKDF algorithm based on
 * various input parameters.
 * 
 * @param context The `context` parameter is a pointer to a character array that represents the context
 * for the keying material generation. It is used as input to the HKDF (HMAC-based Key Derivation
 * Function) algorithm.
 * @param ePub_X ePub_X is a pointer to a uint8_t array that represents the public key of the entity
 * being authenticated. It has a length of 32 bytes.
 * @param keyingMaterial The `keyingMaterial` parameter is a pointer to a buffer where the generated
 * keying material will be stored. It should have a size of at least 32 bytes.
 * @param out The `out` parameter is a pointer to the buffer where the output keying material will be
 * stored. The size of the buffer is specified by the `outLen` parameter.
 * @param outLen The parameter `outLen` represents the length of the output buffer `out`. It specifies
 * the maximum number of bytes that can be written to the `out` buffer.
 */
void AuthenticationContext::Auth0_keying_material(const char *context, uint8_t ePub_X[32], uint8_t *keyingMaterial, uint8_t *out, size_t outLen)
{
  uint8_t interface = 0x5E;
  uint8_t flags[2] = {0x01, 0x01};
  uint8_t prot_ver[4] = {0x5c, 0x02, 0x02, 0x0};
  uint8_t supported_vers[6] = {0x5c, 0x04, 0x02, 0x0, 0x01, 0x0};
  uint8_t dataMaterial[32 + strlen(context) + readerIdentifier.size() + 32 + 1 + sizeof(supported_vers) + sizeof(prot_ver) + readerEphX.size() + 16 + 2 + endpointEphX.size()];
  int olen = 0;
  utils::pack(readerData->reader_key_x, 32, dataMaterial, &olen);
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
  logD("Auth0 HKDF Material: %s", utils::bufToHexString(dataMaterial, sizeof(dataMaterial)).c_str());
  int ret = mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), NULL, 0, keyingMaterial, 32, dataMaterial, sizeof(dataMaterial), out, outLen);
  logV("HKDF Status: %d", ret);
}

/**
 * The function `Auth1_keying_material` generates keying material using various input data and the HKDF
 * algorithm.
 * 
 * @param keyingMaterial A pointer to the buffer where the generated keying material will be stored.
 * @param context The "context" parameter is a string that represents the context or additional
 * information for the authentication process. It is used as input to generate the keying material.
 * @param out The `out` parameter is a pointer to a buffer where the generated keying material will be
 * stored. The size of this buffer is specified by the `outLen` parameter.
 * @param outLen The parameter `outLen` represents the length of the output buffer `out` where the
 * generated keying material will be stored.
 */
void AuthenticationContext::Auth1_keying_material(uint8_t *keyingMaterial, const char *context, uint8_t *out, size_t outLen)
{
  uint8_t interface = 0x5E;
  uint8_t flags[2] = {0x01, 0x01};
  uint8_t prot_ver[4] = {0x5c, 0x02, 0x02, 0x0};
  uint8_t supported_vers[6] = {0x5c, 0x04, 0x02, 0x0, 0x01, 0x0};
  // std::list<uint8_t> dataMaterial;
  uint8_t dataMaterial[readerEphX.size() + endpointEphX.size() + transactionIdentifier.size() + 1 + sizeof(flags) + strlen(context) + sizeof(prot_ver) + sizeof(supported_vers)];
  int olen = 0;
  utils::pack(readerEphX.data(), readerEphX.size(), dataMaterial, &olen);
  utils::pack(endpointEphX.data(), endpointEphX.size(), dataMaterial, &olen);
  utils::pack(transactionIdentifier.data(), 16, dataMaterial, &olen);
  utils::pack(&interface, 1, dataMaterial, &olen);
  utils::pack(flags, 2, dataMaterial, &olen);
  utils::pack((uint8_t *)context, strlen(context), dataMaterial, &olen);
  utils::pack(prot_ver, sizeof(prot_ver), dataMaterial, &olen);
  utils::pack(supported_vers, sizeof(supported_vers), dataMaterial, &olen);
  logD("DATA Material Length: %d, Data: %s", sizeof(dataMaterial), utils::bufToHexString(dataMaterial, sizeof(dataMaterial)).c_str());
  mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), NULL, 0, keyingMaterial, 32, dataMaterial, olen, out, outLen);
}

/**
 * The function `find_endpoint_by_cryptogram` searches for an endpoint in a list of issuers based on a
 * given cryptogram.
 * 
 * @param cryptogram The parameter "cryptogram" is a vector of uint8_t, which represents a cryptogram.
 * 
 * @return a pointer to an object of type `issuerEndpoints::issuerEndpoints_t`.
 */
issuerEndpoints::issuerEndpoints_t * AuthenticationContext::find_endpoint_by_cryptogram(std::vector<uint8_t> cryptogram)
{
  issuerEndpoints::issuerEndpoints_t *foundEndpoint = nullptr;
  for (auto &&issuer : readerData->issuers)
  {
    logV("Issuer: %s, Endpoints: %d", utils::bufToHexString(issuer.issuerId, sizeof(issuer.issuerId)).c_str(), issuer.endpoints.size());
    for (auto &&endpoint : issuer.endpoints)
    {
      logV("Endpoint: %s, Persistent Key: %s", utils::bufToHexString(endpoint.endpointId, sizeof(endpoint.endpointId)).c_str(), utils::bufToHexString(endpoint.persistent_key, sizeof(endpoint.persistent_key)).c_str());
      std::vector<uint8_t> endpointPubX = get_x(std::vector<uint8_t>{endpoint.publicKey, endpoint.publicKey + 65});
      uint8_t hkdf[58];
      Auth0_keying_material("VolatileFast", endpoint.endpoint_key_x, endpoint.persistent_key, hkdf, sizeof(hkdf));
      logD("HKDF Derived Key: %s", utils::bufToHexString(hkdf, sizeof(hkdf)).c_str());
      if (!memcmp(hkdf, cryptogram.data(), 16))
      {
        logI("Endpoint %s matches cryptogram", utils::bufToHexString(endpoint.endpointId, sizeof(endpoint.endpointId)).c_str());
        foundEndpoint = &endpoint;
        break;
      }
    }
    if(foundEndpoint != nullptr){
      break;
    }
  }
  return foundEndpoint;
}

/**
 * The function `Auth1_keys_generator` generates persistent and volatile keys using a shared key and
 * X963KDF algorithm.
 * 
 * @param persistentKey The `persistentKey` parameter is a pointer to a buffer where the generated
 * persistent key will be stored. It should have a size of at least 32 bytes.
 * @param volatileKey The `volatileKey` parameter is a pointer to a buffer that will store the volatile
 * key. It is expected to be an array of `uint8_t` with a size of 48 bytes.
 */
void AuthenticationContext::Auth1_keys_generator(uint8_t *persistentKey, uint8_t *volatileKey)
{
  uint8_t sharedKey[32];

  get_shared_key(sharedKey, sizeof(sharedKey));
  logD("Shared Key: %s", utils::bufToHexString(sharedKey, 32).c_str());

  X963KDF kdf(MBEDTLS_MD_SHA256, 32, transactionIdentifier.data(), 16);

  // Derive the key using X963KDF
  uint8_t derivedKey[32];
  kdf.derive(sharedKey, sizeof(sharedKey), derivedKey);
  logD("X963KDF Derived Key: %s", utils::bufToHexString(derivedKey, 32).c_str());
  Auth1_keying_material(derivedKey, "Persistent", persistentKey, 32);
  Auth1_keying_material(derivedKey, "Volatile", volatileKey, 48);
  logD("Persistent Key: %s", utils::bufToHexString(persistentKey, 32).c_str());
  logD("Volatile Key: %s", utils::bufToHexString(volatileKey, 48).c_str());
}


/**
 * The function `fast_auth` performs a fast authentication process and returns the endpoint and
 * authentication flow type.
 * 
 * @param fallbackToStd The `fallbackToStd` parameter is a boolean flag that indicates whether to
 * fallback to the standard authentication flow if the fast authentication flow fails. If
 * `fallbackToStd` is `true`, the function will return a tuple with `endpoint` set to `nullptr` and the
 * second value set to `
 * 
 * @return a std::tuple containing a pointer to an issuerEndpoints_t object and an integer value.
 */
std::tuple<issuerEndpoints::issuerEndpoints_t *, homeKeyReader::KeyFlow> AuthenticationContext::fast_auth(bool fallbackToStd)
{
  uint8_t prot_v_data[] = {0x02, 0x0};

  std::vector<uint8_t> fastTlv(sizeof(prot_v_data) + readerEphPubKey.size() + transactionIdentifier.size() + readerIdentifier.size() + 8);
  size_t len = 0;
  utils::simple_tlv(0x5C, prot_v_data, sizeof(prot_v_data), fastTlv.data(), &len);

  utils::simple_tlv(0x87, readerEphPubKey.data(), readerEphPubKey.size(), fastTlv.data() + len, &len);

  utils::simple_tlv(0x4C, transactionIdentifier.data(), transactionIdentifier.size(), fastTlv.data() + len, &len);

  utils::simple_tlv(0x4D, readerIdentifier.data(), readerIdentifier.size(), fastTlv.data() + len, &len);
  std::vector<uint8_t> apdu{0x80, 0x80, 0x01, 0x01, (uint8_t)len};
  apdu.insert(apdu.begin() + 5, fastTlv.begin(), fastTlv.end());
  bool exchange;
  uint8_t response[128];
  uint8_t responseLength = 128;
  logD("Auth0 APDU Length: %d, DATA: %s", apdu.size(), utils::bufToHexString(apdu.data(), apdu.size()).c_str());
  exchange = nfc->inDataExchange(apdu.data(), apdu.size(), response, &responseLength);
  logD("Auth0 Response Length: %d, DATA: %s", responseLength, utils::bufToHexString(response, responseLength).c_str());
  issuerEndpoints::issuerEndpoints_t *endpoint = nullptr;
  if (response[responseLength - 2] == 0x90 && response[0] == 0x86)
  {
    auto Auth0Res = BERTLV::unpack_array(response, responseLength);
    auto endpointPubKey = BERTLV::findTag(kEndpoint_Public_Key, Auth0Res);
    endpointEphPubKey = endpointPubKey.value;
    auto encryptedMessage = BERTLV::findTag(kAuth0_Cryptogram, Auth0Res);
    endpointEphX = get_x(endpointEphPubKey);
    if(fallbackToStd){
      return std::make_tuple(endpoint, homeKeyReader::kFlowFailed);
    } else {
      endpoint = find_endpoint_by_cryptogram(encryptedMessage.value);
      if (endpoint != nullptr)
      {
        uint8_t apdu1[4] = {0x80, 0x3c, 0x01, 0x0};
        uint8_t response1[4];
        uint8_t responseLength = 4;
        logI("Endpoint Authenticated, CONTROL FLOW SUCCESS");
        logD("APDU: %s, Length: %d", utils::bufToHexString(apdu1, sizeof(apdu1)).c_str(), sizeof(apdu1));
        exchange = nfc->inDataExchange(apdu1, sizeof(apdu1), response1, &responseLength);
        logD("RESPONSE: %s, Length: %d", utils::bufToHexString(response1, responseLength).c_str(), responseLength);
        if(response1[0] == 0x90){
          logI("AUTHENTICATED VIA FAST FLOW");
          return std::make_tuple(endpoint, homeKeyReader::kFlowFAST);
        }
      }
      else
      {
        return std::make_tuple(endpoint, homeKeyReader::kFlowFailed);
      }
    }
  }
  return std::make_tuple(endpoint, homeKeyReader::kFlowFailed);
}

/**
 * The function `std_auth()` performs a standard authentication process using various cryptographic
 * operations and NFC communication.
 * 
 * @return a tuple containing four elements:
 * 1. A pointer to an object of type `issuerEndpoints::issuerEndpoints_t*`
 * 2. An object of type `DigitalKeySecureContext`
 * 3. A vector of type `std::vector<uint8_t>`
 * 4. An object of type `homeKeyReader::KeyFlow`
 */
std::tuple<issuerEndpoints::issuerEndpoints_t*, DigitalKeySecureContext, std::vector<uint8_t>, homeKeyReader::KeyFlow> AuthenticationContext::std_auth()
{
  int readerContext = 1096652137;
  int deviceContext = 1317567308;
  uint8_t readerCtx[4]{0x41,0x5d,0x95,0x69};
  uint8_t deviceCtx[4]{0x4e,0x88,0x7b,0x4c};

  std::vector<uint8_t> stdTlv(16 + endpointEphX.size() + readerEphX.size() + 30);
  size_t len = 0;
  utils::simple_tlv(0x4D, readerIdentifier.data(), 16, stdTlv.data(), &len);
  utils::simple_tlv(0x86, endpointEphX.data(), endpointEphX.size(), stdTlv.data() + len, &len);
  utils::simple_tlv(0x87, readerEphX.data(), readerEphX.size(), stdTlv.data() + len, &len);
  utils::simple_tlv(0x4C, transactionIdentifier.data(), 16, stdTlv.data() + len, &len);
  utils::simple_tlv(0x93, readerCtx, 4, stdTlv.data() + len, &len);
  auto sigTlv = signSharedInfo(stdTlv.data(), len);
  std::vector<uint8_t> apdu{0x80, 0x81, 0x0, 0x0, (uint8_t)sigTlv.size()};
  apdu.insert(apdu.begin()+5,sigTlv.begin(), sigTlv.end());
  uint8_t response[128];
  uint8_t responseLength = 128;
  logI("Auth1 APDU Length: %d, DATA: %s", apdu.size(), utils::bufToHexString(apdu.data(), apdu.size()).c_str());
  nfc->inDataExchange(apdu.data(), apdu.size(), response, &responseLength);
  logI("Auth1 Response Length: %d, DATA: %s", responseLength, utils::bufToHexString(response, responseLength).c_str());
  issuerEndpoints::issuerEndpoints_t *foundEndpoint = nullptr;
  if (responseLength > 2 && response[responseLength - 2] == 0x90)
  {
    uint8_t persistentKey[32];
    uint8_t volatileKey[48];
    Auth1_keys_generator(persistentKey, volatileKey);
    DigitalKeySecureContext context(volatileKey);
    auto response_result = context.decrypt_response(response, responseLength - 2);
    logD("Decrypted Length: %d, Data: %s", std::get<0>(response_result).size(), utils::bufToHexString(std::get<0>(response_result).data(), std::get<0>(response_result).size()).c_str());
    if (std::get<1>(response_result) > 0)
    {
      std::vector<BERTLV> decryptedTlv = BERTLV::unpack_array(std::vector<unsigned char>{std::get<0>(response_result).data(), std::get<0>(response_result).data()+std::get<1>(response_result)});
      BERTLV *signature = nullptr;
      BERTLV *device_identifier = nullptr;
      for (auto &data : decryptedTlv)
      {
        if(data.tag.data.data()[0] == 0x4E){
          device_identifier = &data;
        }
        if(data.tag.data.data()[0] == 0x9E){
          signature = &data;
        }
      }
      if(device_identifier == nullptr){
        uint8_t apdu[4] = {0x80, 0x3c, 0x00, 0x0};
        uint8_t response[4];
        uint8_t responseLength = 4;
        nfc->inDataExchange(apdu, sizeof(apdu), response, &responseLength);
        return std::make_tuple(nullptr, context, std::vector<uint8_t>{}, homeKeyReader::kFlowFailed);
      }
      for (auto &issuer : readerData->issuers)
      {
        for (auto &endpoint : issuer.endpoints)
        {
          if(!memcmp(endpoint.endpointId, device_identifier->value.data(), 6)){
            logD("STD_AUTH: Found Matching Endpoint, ID: %s", utils::bufToHexString(endpoint.endpointId, sizeof(endpoint.endpointId)).c_str());
            foundEndpoint = &endpoint;
          }
        }
      }
      if(foundEndpoint != nullptr){
        std::vector<uint8_t> verification_hash_input_material(sizeof(readerData->reader_identifier) + sizeof(readerData->identifier) + endpointEphX.size() + readerEphX.size() + 30);
        size_t olen = 0;

        utils::simple_tlv(0x4D, readerIdentifier.data(), sizeof(readerData->reader_identifier) + sizeof(readerData->identifier), verification_hash_input_material.data(), &olen);
        utils::simple_tlv(0x86, endpointEphX.data(), endpointEphX.size(), verification_hash_input_material.data() + olen, &olen);
        utils::simple_tlv(0x87, readerEphX.data(), readerEphX.size(), verification_hash_input_material.data() + olen, &olen);
        utils::simple_tlv(0x4C, transactionIdentifier.data(), 16, verification_hash_input_material.data() + olen, &olen);
        utils::simple_tlv(0x93, deviceCtx, 4, verification_hash_input_material.data() + olen, &olen);
        mbedtls_ecp_keypair keypair;
        mbedtls_ecp_keypair_init(&keypair);

        uint8_t hash[32];

        mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), verification_hash_input_material.data(), olen, hash);

        logD("verification_hash_input_material: %s", utils::bufToHexString(hash, 32).c_str());
        mbedtls_mpi r;
        mbedtls_mpi s;

        mbedtls_mpi_init( &r );
        mbedtls_mpi_init( &s );
        mbedtls_ecp_group_load(&keypair.grp, MBEDTLS_ECP_DP_SECP256R1);
        int pubImport = mbedtls_ecp_point_read_binary(&keypair.grp, &keypair.Q, foundEndpoint->publicKey, sizeof(foundEndpoint->publicKey));
        logV("public key import result: %d");

        mbedtls_mpi_read_binary(&r, signature->value.data(), signature->value.size()/2);
        mbedtls_mpi_read_binary(&s, signature->value.data() + (signature->value.size() / 2), signature->value.size()/2);

        int result = mbedtls_ecdsa_verify(&keypair.grp, hash, 32, &keypair.Q, &r, &s);

        mbedtls_ecp_keypair_free(&keypair);

        logV("signature verification result: %d", result);

        if(result == 0){
          uint8_t apdu[4] = {0x80, 0x3c, 0x01, 0x0};
          uint8_t cmdFlow[4];
          uint8_t cmdFlowLength = 4;
          nfc->inDataExchange(apdu, sizeof(apdu), cmdFlow, &cmdFlowLength);
          if(cmdFlow[0] == 0x90){
            logI("AUTHENTICATED VIA STD FLOW");
          }
          return std::make_tuple(foundEndpoint, context, std::vector<uint8_t>{persistentKey, persistentKey + 32}, homeKeyReader::kFlowSTANDARD);
        } else
        if (device_identifier != nullptr)
        {
          return std::make_tuple(foundEndpoint, context, std::vector<uint8_t>{persistentKey, persistentKey + 32}, homeKeyReader::kFlowATTESTATION);
        } else return std::make_tuple(nullptr, context, std::vector<uint8_t>{}, homeKeyReader::kFlowFailed);
      }
    }
    else
    {
      std::cout << std::endl;
      logE("SOMETHING WENT WRONG");
      uint8_t apdu[4] = {0x80, 0x3c, 0x00, 0x0};
      uint8_t response[4];
      uint8_t responseLength = 4;
      nfc->inDataExchange(apdu, sizeof(apdu), response, &responseLength);
      std::make_tuple(nullptr, context, std::vector<uint8_t>{}, homeKeyReader::kFlowFailed);
    }
  }
  return std::make_tuple(foundEndpoint, DigitalKeySecureContext(), std::vector<uint8_t>{}, homeKeyReader::kFlowFailed);
}