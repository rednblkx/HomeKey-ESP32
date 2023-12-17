/*********************************************************************************
 *  MIT License
 *
 *  Copyright (c) 2022 Gregg E. Berman
 *
 *  https://github.com/HomeSpan/HomeSpan
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 ********************************************************************************/

#include "HomeSpan.h"
#include "TLV.h"
#include <mbedtls/sha256.h>
#include <mbedtls/ecp.h>
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <sys/random.h>
#include <mbedtls/ecdh.h>
#include "x963kdf.h"
#include <mbedtls/hkdf.h>
#include "DigitalKeySecureContext.h"

// // If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK (D5)
#define PN532_MOSI (D7)
#define PN532_SS (D8)
#define PN532_MISO (D6)
#define RESET_PIN (D3)

PN532_SPI pn532spi(SPI, PN532_SS);
PN532 nfc(pn532spi);
// Use this line for a breakout with a SPI connection:

// Adafruit_PN532 nfc(PN532_SS);

struct attestation
{
  uint32_t time;
  uint8_t payload;
};

struct issuerEnrollments_t
{
  uint32_t hap_at;
  char payload[56];
  boolean attestation;
};

struct issuerEndpoints_t
{
  char endpointId[6];
  uint32_t last_used_at;
  uint8_t counter;
  uint8_t key_type;
  char publicKey[65];
  char persistent_key[32];
  issuerEnrollments_t enrollments[2];
};

struct homeKeyIssuers_t
{
  char issuerId[8];
  char publicKey[32];
  issuerEndpoints_t endpoints[2];
};

struct readerData_t
{
  char reader_private_key[32];
  char reader_identifier[8];
  char identifier[8];
  homeKeyIssuers_t issuers[4];
} readerData;

uint8_t issuersCount = 0;

typedef enum
{
  kTLVType1_Operation = 0x01,
  kTLVType1_Device_Credential_Request = 0x04,
  kTLVType1_Device_Credential_Response = 0x05,
  kTLVType1_Reader_Key_Request = 0x06,
  kTLVType1_Reader_Key_Response = 0x07,
} Reader_Tags;

typedef enum
{
  kReader_Req_Key_Type = 0x01,
  kReader_Req_Reader_Private_Key = 0x02,
  kReader_Req_Identifier = 0x03,
  kReader_Req_Key_Identifier = 0x04, // This is only relevant for "remove" operation
  kRequest_Reader_Key_Request = 0x06
} Reader_Key_Request;

typedef enum
{
  kReader_Res_Key_Identifier = 0x01,
  kReader_Res_Status = 0x02,
  kReader_Res_Reader_Key_Response = 0x07
} Reader_Key_Response;

typedef enum
{
  kDevice_Req_Key_Type = 0x01,
  kDevice_Req_Public_Key = 0x02,
  kDevice_Req_Issuer_Key_Identifier = 0x03,
  kDevice_Req_Key_State = 0x04,
  kDevice_Req_Key_Identifier = 0x05 // This is only relevant for "remove" operation
} Device_Credential_Request;

typedef enum
{
  kDevice_Res_Key_Identifier = 0x01,
  kDevice_Res_Issuer_Key_Identifier = 0x02,
  kDevice_Res_Status = 0x03,
  kDevice_Credential_Response = 0x05
} Device_Credential_Response;

typedef enum
{
  kEndpoint_Public_Key = 0x86,
  kAuth0_Cryptogram = 0x9D,
  kStatus = 0x90
} AUTH0_RESPONSE;

nvs_handle savedData;

void printBuffer(const uint8_t *buffer, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    Serial.printf("%#02x ", buffer[i]);
  }
  Serial.println();
}

size_t decodeB64(unsigned char *dst, const char *src, size_t len)
{
  std::string msgCy = src;
  msgCy.erase(std::remove(msgCy.begin(), msgCy.end(), '\\'), msgCy.end());
  size_t out_len1 = 0;
  int ret = mbedtls_base64_decode(dst, len, &out_len1, (const unsigned char *)msgCy.c_str(), msgCy.length());
  if (dst == NULL)
  {
    LOG1("*** B64 DEC LEN: %d", out_len1);
    return (out_len1);
  }
  LOG1("\n");
  LOG1("*** DECODED B64: \n");
  for (int cnt = 0; cnt < len; cnt++)
  {
    LOG1("%#x ", dst[cnt]);
  }
  LOG1("\n\n");
  if (ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
    LOG0("\n*** WARNING:  Destination buffer is too small (%d out of %d bytes needed)\n\n", len, out_len1);
  else if (ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER)
    LOG0("\n*** WARNING:  Data is not in base-64 format\n\n");

  return out_len1;
}

int set_reader_key(uint8_t *buf, size_t len)
{
  TLV<Reader_Key_Request, 10> tlv8;
  printBuffer(buf, len);
  tlv8.create(kReader_Req_Key_Type, 1, "KEY.TYPE");
  tlv8.create(kReader_Req_Reader_Private_Key, 32, "READER.PRIV.KEY");
  tlv8.create(kReader_Req_Identifier, 8, "IDENTIFIER");
  // tlv8.create(kRequest_Reader_Key_Request, 64, "READER.REQ");
  // tlv8.create(kReader_Req_Key_Identifier, 64, "KEY.IDENTIFIER");

  LOG1("*** TLV DECODE STATE: ");
  LOG1(tlv8.unpack(buf, len));
  LOG1("\n");
  tlv8.print(1);
  uint8_t *readerKey = tlv8.buf(kReader_Req_Reader_Private_Key);
  uint8_t *unknownIdentifier = tlv8.buf(kReader_Req_Identifier);
  printBuffer(readerKey, tlv8.len(kReader_Req_Reader_Private_Key));
  printBuffer(unknownIdentifier, tlv8.len(kReader_Req_Identifier));
  memcpy(readerData.reader_private_key, readerKey, tlv8.len(kReader_Req_Reader_Private_Key));
  memcpy(readerData.identifier, unknownIdentifier, tlv8.len(kReader_Req_Identifier));
  const char *string = "key-identifier";
  char hashable[48];
  strcpy(hashable, string);
  memcpy(hashable + strlen(string), readerData.reader_private_key, sizeof(readerData.reader_private_key));
  unsigned char hash[32];
  uint8_t readeridentifier[8];
  mbedtls_sha256_ret((const unsigned char *)hashable, strlen(string) + sizeof(readerData.reader_private_key), hash, 0);
  memcpy(readeridentifier, (const char *)hash, 8);
  memcpy(readerData.reader_identifier, readeridentifier, 8);
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", &readerData, sizeof(readerData));
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG1("*** NVS W STATUS: \n");
  LOG1("SET: %s\n", esp_err_to_name(set_nvs));
  LOG1("COMMIT: %s\n", esp_err_to_name(commit_nvs));
  LOG1("*** NVS W STATUS: \n");
  tlv8.clear();
  if (set_nvs == 0 && commit_nvs == 0)
  {
    return 0;
  }
  else
    return 1;
}
std::tuple<uint8_t*, size_t> simple_tlv(uint8_t tag, const uint8_t *value, size_t valLength, uint8_t *out, size_t* olen){
  size_t len = sizeof(tag) + valLength + 1;
  uint8_t *buf = new unsigned char[len];
  memcpy(buf, &tag, sizeof(tag));
  memcpy(buf+sizeof(tag), (uint8_t *)&valLength, 1);
  memcpy(buf +sizeof(tag) + 1, value, valLength);
  printf("*** TLV TAG %x[%d]: \n", tag, valLength);
  nfc.PrintHex(buf+2, len-2);
  if(out != NULL && olen != NULL){
    memcpy(out, buf, len);
    size_t l = len + *olen;
    memcpy(olen, &l, sizeof(len));
  }
  return std::make_tuple(buf, len);
}

void pack(uint8_t *buf, size_t buflen, uint8_t *out, int* olen){
  memcpy(out + *olen, buf, buflen);
  size_t l = *olen + buflen;
  memcpy(olen, &l, 1);
}

int provision_device_cred(uint8_t *buf, size_t len)
{
  TLV<Device_Credential_Request, 10> tlv8;
  printBuffer(buf, len);
  tlv8.create(kDevice_Req_Key_Type, 1, "KEY.TYPE");
  tlv8.create(kDevice_Req_Public_Key, 64, "PUBLIC.KEY");
  tlv8.create(kDevice_Req_Issuer_Key_Identifier, 8, "ISSUER.IDENTIFIER");
  tlv8.create(kDevice_Req_Key_State, 1, "KEY.STATE");
  tlv8.create(kDevice_Req_Key_Identifier, 8, "KEY.IDENTIFIER");

  LOG1("*** TLV DECODE STATE: ");
  LOG1(tlv8.unpack(buf, len));
  LOG1("\n");
  tlv8.print(1);
  uint8_t *publicKey = tlv8.buf(kDevice_Req_Public_Key);
  uint8_t *issuerIdentifier = tlv8.buf(kDevice_Req_Issuer_Key_Identifier);
  printBuffer(publicKey, tlv8.len(kDevice_Req_Public_Key));
  printBuffer(issuerIdentifier, tlv8.len(kDevice_Req_Issuer_Key_Identifier));
  bool existent = false;
  int index_ex = 0;
  for (int i = 0; i < issuersCount; ++i)
  {
    if(!memcmp(readerData.issuers[i].issuerId, issuerIdentifier, 8)){
      existent = true;
      index_ex = 0;
    }
  }
  if(existent) {
    memcpy(readerData.issuers[index_ex].publicKey, publicKey, tlv8.len(kDevice_Req_Public_Key));
  } else {
    memcpy(readerData.issuers[issuersCount > 0 ? issuersCount : 0].issuerId, issuerIdentifier, tlv8.len(kDevice_Req_Issuer_Key_Identifier));
    memcpy(readerData.issuers[issuersCount > 0 ? issuersCount : 0].publicKey, publicKey, tlv8.len(kDevice_Req_Public_Key));
  }
  issuersCount++;
  esp_err_t set_issuers = nvs_set_u8(savedData, "ISSUERS", issuersCount);
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", &readerData, sizeof(readerData));
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG1("*** NVS W STATUS: \n");
  LOG1("ISSUERS: %s\n", esp_err_to_name(set_issuers));
  LOG1("SET: %s\n", esp_err_to_name(set_nvs));
  LOG1("COMMIT: %s\n", esp_err_to_name(commit_nvs));
  LOG1("*** NVS W STATUS: \n");
  tlv8.clear();
  return 0;
}

void get_public_points(uint8_t *pubKey, size_t pubLen, uint8_t *xBuf, size_t *xLen, uint8_t *yBuf, size_t *yLen){
  mbedtls_ecp_group grp;
  mbedtls_ecp_point point;
  mbedtls_ecp_point_init(&point);
  mbedtls_ecp_group_init(&grp);
  mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
  int ret = mbedtls_ecp_point_read_binary(&grp, &point, pubKey, pubLen);
  size_t buffer_size_x = mbedtls_mpi_size(&point.X);
  size_t buffer_size_y = mbedtls_mpi_size(&point.Y);
  uint8_t X[buffer_size_x];
  uint8_t Y[buffer_size_y];
  // Write X and Y coordinates into the buffer
  mbedtls_mpi_write_binary(&point.X, X, mbedtls_mpi_size(&point.X));
  mbedtls_mpi_write_binary(&point.Y, Y, mbedtls_mpi_size(&point.Y));
  if(xBuf != NULL){
    memcpy(xBuf, X, buffer_size_x);
    memcpy(xLen, &buffer_size_x, sizeof(buffer_size_x));
  }
  if(yBuf != NULL){
    memcpy(yBuf, Y, buffer_size_y);
    memcpy(yLen, &buffer_size_y, sizeof(buffer_size_y));
  }
  mbedtls_ecp_group_free(&grp);
  mbedtls_ecp_point_free(&point);
}

void get_keying_material(uint8_t *rEph_X, size_t rEph_XLen, uint8_t *eEph_X, size_t eEph_XLen, uint8_t *transId, uint8_t *derivedKey, const char *context, uint8_t *out, size_t outLen){
  uint8_t interface = static_cast<uint8_t>(0x5E);
  uint8_t flags[] = {0x01, 0x01};
  uint8_t prot_ver[] = {0x5c, 0x02, 0x02, 0x0};
  uint8_t supported_vers[] = {0x5c, 0x04, 0x02, 0x0, 0x01, 0x0};
  uint8_t dataMaterial[rEph_XLen + eEph_XLen + strlen(context) + 29];
  int olen = 0;
  pack(rEph_X, rEph_XLen, dataMaterial, &olen);
  pack(eEph_X, eEph_XLen, dataMaterial, &olen);
  pack(transId, 16, dataMaterial, &olen);
  pack(&interface, 1, dataMaterial, &olen);
  pack(flags, 2, dataMaterial, &olen);
  pack((uint8_t *)context, strlen(context), dataMaterial, &olen);
  pack(prot_ver, sizeof(prot_ver), dataMaterial, &olen);
  pack(supported_vers, sizeof(supported_vers), dataMaterial, &olen);
  Serial.print("\nDATA MATERIAL: ");
  nfc.PrintHex(dataMaterial, sizeof(dataMaterial));
  mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), NULL, 0, derivedKey, 32, dataMaterial, sizeof(dataMaterial), out, outLen);
}

void get_shared_key(uint8_t *reader_priv_key, size_t privLen, uint8_t *endpoint_pub_key, size_t pubLen, uint8_t *out, size_t buflen){
    mbedtls_ecp_group grp;
    mbedtls_mpi reader_ephemeral_private_key, shared_key;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ecp_point endpoint_ephemeral_public_key;
    mbedtls_entropy_context entropy;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&reader_ephemeral_private_key);
    mbedtls_ecp_point_init(&endpoint_ephemeral_public_key);
    mbedtls_mpi_init(&shared_key);

    // Initialize the elliptic curve group (e.g., SECP256R1)
    mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);

    // Set the reader's ephemeral private key
    mbedtls_mpi_read_binary(&reader_ephemeral_private_key, reader_priv_key, privLen);

    // Set the endpoint's ephemeral public key
    mbedtls_ecp_point_read_binary(&grp, &endpoint_ephemeral_public_key, endpoint_pub_key, pubLen);

    // Perform key exchange
    mbedtls_ecdh_compute_shared(&grp, &shared_key, &endpoint_ephemeral_public_key, &reader_ephemeral_private_key,
                                mbedtls_ctr_drbg_random, &ctr_drbg);

    // Now, shared_key contains the shared secret

    mbedtls_mpi_write_binary(&shared_key, out, buflen);

    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&reader_ephemeral_private_key);
    mbedtls_mpi_free(&shared_key);
    mbedtls_ecp_point_free(&endpoint_ephemeral_public_key);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

std::tuple<uint8_t*, uint8_t*, uint8_t*> key_material_generator(uint8_t *rEphPrivKey, size_t rEphLen, uint8_t *rEph_X, size_t rEph_XLen, uint8_t *eEphPubKey, size_t eEphLen, uint8_t *eEph_X, size_t eEph_XLen, uint8_t *transId /*0x5E interface, 5c020200 sel ver, 5c0402000100 supported vers*/){ 
  try
  {
    mbedtls_md_type_t algorithm = MBEDTLS_MD_SHA256;

    uint8_t sharedKey[32];

    get_shared_key(rEphPrivKey, rEphLen, eEphPubKey, eEphLen, sharedKey, sizeof(sharedKey));
    Serial.print("\nSHARED KEY: ");
    nfc.PrintHex(sharedKey, 32);

    X963KDF kdf(algorithm, 32, transId, 16);

    // Derive the key using X963KDF
    uint8_t derivedKey[32];
    kdf.derive(sharedKey, sizeof(sharedKey), derivedKey);
    Serial.print("DERIVED KEY: ");
    nfc.PrintHex(derivedKey, 32);
    uint8_t PersistentKey[32];
    uint8_t VolatileKey[48];
    get_keying_material(rEph_X, rEph_XLen, eEph_X, eEph_XLen, transId, derivedKey, "Persistent", PersistentKey, 32);
    get_keying_material(rEph_X, rEph_XLen, eEph_X, eEph_XLen, transId, derivedKey, "Volatile", VolatileKey, 48);
    Serial.print("PERSISTENT KEY: ");
    nfc.PrintHex(PersistentKey, 32);
    Serial.print("VOLATILE KEY: ");
    nfc.PrintHex(VolatileKey, 48);
    uint8_t kenc[16];
    uint8_t kmac[16];
    uint8_t krmac[16];

    memcpy(kenc, VolatileKey, 16);
    memcpy(kmac, VolatileKey+16, 16);
    memcpy(krmac, VolatileKey+32, 16);
    Serial.print("KENC: ");
    nfc.PrintHex(kenc, 16);
    Serial.print("KMAC: ");
    nfc.PrintHex(kmac, 16);
    Serial.print("KRMAC: ");
    nfc.PrintHex(krmac, 16);
    return std::make_tuple(kenc, kmac, krmac);
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

}

void std_auth(uint8_t *readerIdentifier, uint8_t *transId, uint8_t *readerEphPrivKey, size_t readerEphLen, uint8_t *readerEphPubKey, size_t readerEphPubLen, uint8_t *endEphPublicKey, size_t endEphLen){
try
{
    uint8_t readerEph_X[readerEphPubLen];
  size_t readerEph_XLen = 0;
  uint8_t endpointEph_X[endEphLen];
  size_t endpointEph_XLen = 0;
  int value = 1096652137;
  uint8_t bytes[4];

  bytes[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
  bytes[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
  bytes[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
  bytes[3] = static_cast<uint8_t>(value & 0xFF);

  get_public_points(readerEphPubKey, readerEphPubLen, readerEph_X, &readerEph_XLen, NULL, NULL);
  get_public_points(endEphPublicKey, endEphLen, endpointEph_X, &endpointEph_XLen, NULL, NULL);
  printBuffer(readerEph_X, readerEph_XLen);
  printBuffer(endpointEph_X, endpointEph_XLen);
  uint8_t stdTlv[110];
  size_t len = 0;
  simple_tlv(0x4D, readerIdentifier, sizeof(readerData.reader_identifier) + sizeof(readerData.identifier), stdTlv, &len);
  simple_tlv(0x86, endpointEph_X, endpointEph_XLen, stdTlv+len, &len);
  simple_tlv(0x87, readerEph_X, readerEph_XLen, stdTlv+len, &len);
  simple_tlv(0x4C, transId, 16, stdTlv+len, &len);
  simple_tlv(0x93, bytes, 4, stdTlv+len, &len);
  Serial.println("*** Packed TLV: ");
  nfc.PrintHex(stdTlv, len);
  mbedtls_ecp_keypair keypair;
  mbedtls_ecp_keypair_init(&keypair);

  mbedtls_mpi sigMpi1;
  mbedtls_mpi sigMpi2;
  mbedtls_mpi_init(&sigMpi1);
  mbedtls_mpi_init(&sigMpi2);
  
  mbedtls_ctr_drbg_context dbrg;
  mbedtls_ctr_drbg_init(&dbrg);
  mbedtls_entropy_context entropy;
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_seed(&dbrg, mbedtls_entropy_func, &entropy, NULL, 0);

  const mbedtls_md_info_t *mdInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

  uint8_t hash[32];

  mbedtls_md(mdInfo, stdTlv, len, hash);

  Serial.println("** SHA256 Hashed TLV:");
  nfc.PrintHex(hash, 32);

  mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &keypair, (uint8_t *)readerData.reader_private_key, sizeof(readerData.reader_private_key));

  mbedtls_ecdsa_sign_det_ext(&keypair.grp, &sigMpi1, &sigMpi2, &keypair.d, hash, 32, MBEDTLS_MD_SHA256, mbedtls_ctr_drbg_random, &dbrg);
  uint8_t sigPoint[mbedtls_mpi_size(&sigMpi1) + mbedtls_mpi_size(&sigMpi2)];
  mbedtls_mpi_write_binary(&sigMpi1, sigPoint, mbedtls_mpi_size(&sigMpi1));
  mbedtls_mpi_write_binary(&sigMpi2, sigPoint+mbedtls_mpi_size(&sigMpi1), mbedtls_mpi_size(&sigMpi2));
  mbedtls_ecp_keypair_free(&keypair);
  mbedtls_mpi_free(&sigMpi1);
  mbedtls_mpi_free(&sigMpi2);
  mbedtls_ctr_drbg_free(&dbrg);
  mbedtls_entropy_free(&entropy);
  uint8_t sigTlv[sizeof(sigPoint) + 2];
  size_t sigTlvLen = 0;
  simple_tlv(0x9E, sigPoint, sizeof(sigPoint), sigTlv, &sigTlvLen);
  uint8_t apdu[sigTlvLen + 5] = {0x80, 0x81, 0x0, 0x0, (uint8_t)sigTlvLen};
  memcpy(apdu + 5, sigTlv, sigTlvLen);
  uint8_t response[91];
  uint8_t responseLength = 91;
  nfc.inDataExchange(apdu, sizeof(apdu), response, &responseLength, true);
  // nfc.inCommunicateThru(apdu, sizeof(apdu), response, &responseLength, 1000, true);
  nfc.PrintHex(response, responseLength);
  if(responseLength > 2 && response[responseLength - 2] == 0x90){
    auto keys = key_material_generator(readerEphPrivKey, readerEphLen, readerEph_X, readerEph_XLen, endEphPublicKey, endEphLen, endpointEph_X, readerEph_XLen, transId);
    DigitalKeySecureContext context(std::get<0>(keys), std::get<1>(keys), std::get<2>(keys));
    auto response_result = context.decrypt_response(response, responseLength - 2);
    printf("\ndecrypted data: ");
    for (size_t i = 0; i < std::get<1>(response_result); i++)
    {
        printf("%02X ", std::get<0>(response_result)[i]);
    }
    printf("\ndecrypted length: %d\n", std::get<1>(response_result));
    if(std::get<0>(response_result)[0] == 0){
        std::cout << std::endl;
        printf("\nSOMETHING WENT WRONG\n");
        return;
    }
    if(std::get<0>(response_result)[0] == 0x4e){
      uint8_t attestation_exchange_common_secret[32];
      esp_fill_random(attestation_exchange_common_secret, 32);
      auto attTlv = simple_tlv(0xC0, attestation_exchange_common_secret, 32, NULL, NULL);
      auto opAttTlv = simple_tlv(0x8E, std::get<0>(attTlv), std::get<1>(attTlv), NULL, NULL);
      uint8_t attComm[std::get<1>(opAttTlv) + 1] = {0x0};
      memcpy(attComm + 1, std::get<0>(opAttTlv), std::get<1>(opAttTlv));
      nfc.PrintHex(attComm, sizeof(attComm));
      auto encrypted = context.encrypt_command(attComm, sizeof(attComm));

      nfc.PrintHex(std::get<0>(encrypted), std::get<1>(encrypted));
      nfc.PrintHex(std::get<2>(encrypted), 8);
      uint8_t exchApdu[std::get<1>(encrypted) + 5] = {0x84, 0xc9, 0x0, 0x0, (uint8_t)std::get<1>(encrypted)};
      memcpy(exchApdu + 5, std::get<0>(encrypted), std::get<1>(encrypted));
      uint8_t exchRes[16];
      uint8_t exchResLen = 16;
      nfc.inDataExchange(exchApdu, sizeof(exchApdu), exchRes, &exchResLen, true);
      nfc.PrintHex(exchRes, exchResLen);
      if(exchResLen > 2 && exchRes[exchResLen - 2] == 0x90){
        uint8_t controlFlow[4] = {0x80, 0x3c, 0x01, 0x0};
        uint8_t controlRes[8];
        uint8_t controlResLen = 8;
        nfc.inDataExchange(controlFlow, sizeof(controlFlow),  controlRes, &controlResLen, true);
        nfc.PrintHex(controlRes, controlResLen);
      }
    }
  }

}
catch(const std::exception& e)
{
  std::cerr << e.what() << '\n';
}

}

void fast_auth(uint8_t *readerPublicKey, size_t pubLen, uint8_t *readerEphemeralPublicKey, size_t ephLen, uint8_t *readerEphemeralPrivKey, size_t ephPrivLen){
  uint8_t prot_v_data[] = {0x02, 0x0};
  uint8_t transactionIdentifier[16];
  esp_fill_random(transactionIdentifier, 16);

  Serial.println("*** Transaction Identifier: ");
  for (int i = 0; i < 16;i++){
    Serial.printf("%02x ", transactionIdentifier[i]);
  }

  Serial.println();

  uint8_t fastTlv[110];
  size_t len = 0;
  simple_tlv(0x5C, prot_v_data, sizeof(prot_v_data), fastTlv, &len);

  simple_tlv(0x87, readerEphemeralPublicKey, ephLen, fastTlv+len, &len);
  
  simple_tlv(0x4C, transactionIdentifier, sizeof(transactionIdentifier), fastTlv+len, &len);
  
  uint8_t identifier[sizeof(readerData.reader_identifier) + sizeof(readerData.identifier)];
  memcpy(identifier, readerData.reader_identifier, sizeof(readerData.reader_identifier));
  memcpy(identifier+sizeof(readerData.reader_identifier), readerData.identifier, sizeof(readerData.identifier));
  Serial.println("*** READER IDENTIFIER: ");
  for (int i = 0; i < 16;i++){
    Serial.printf("%02x ", identifier[i]);
  } 
  Serial.println();
  simple_tlv(0x4D, identifier, sizeof(identifier), fastTlv+len, &len);
  Serial.println("*** TLV LENGTH: ");
  Serial.println(len);
  uint8_t apdu[113] = {0x80, 0x80, 0x01, 0x01, (uint8_t)len};
  memcpy(apdu + 5, fastTlv, len);
  bool exchange;
  uint8_t response[90];
  uint8_t responseLength = 90;
  exchange = nfc.inDataExchange(apdu, sizeof(apdu), response, &responseLength, true);
  Serial.print("***RESPONSE: ");
  nfc.PrintHex(response, responseLength);

  if(response[responseLength - 2] == 0x90){
    TLV<AUTH0_RESPONSE, 10> Auth0Res;

    Auth0Res.create(kEndpoint_Public_Key, 65, "END.PUBKEY");
    Auth0Res.create(kAuth0_Cryptogram, 16, "END.PUBKEY");
    Auth0Res.create(kStatus, 1, "STATUS");

    Auth0Res.unpack(response, responseLength);

    std_auth(identifier, transactionIdentifier, readerEphemeralPrivKey, ephPrivLen, readerEphemeralPublicKey, ephLen, Auth0Res.buf(kEndpoint_Public_Key), Auth0Res.len(kEndpoint_Public_Key));
  }

}

struct LockManagement : Service::LockManagement
{
  SpanCharacteristic *lockControlPoint;
  SpanCharacteristic *version;

  LockManagement() : Service::LockManagement()
  {

    Serial.print("Configuring LockManagement"); // initialization message
    Serial.print("\n");
    new Characteristic::Name("Lock Management");

    lockControlPoint = new Characteristic::LockControlPoint();
    version = new Characteristic::Version();

  } // end constructor

}; // end LockManagement

struct LockMechanism : Service::LockMechanism
{
  SpanCharacteristic *lockCurrentState;
  SpanCharacteristic *lockTargetState;

  LockMechanism() : Service::LockMechanism()
  {
    Serial.print("Configuring LockMechanism"); // initialization message
    Serial.print("\n");
    new Characteristic::Name("NFC Lock");
    lockCurrentState = new Characteristic::LockCurrentState();
    lockTargetState = new Characteristic::LockTargetState();

  } // end constructor

  boolean update(char *callback, int *callbackLen)
  {
    int targetState = lockTargetState->getNewVal();
    LOG0("\tCurrent LockState=");
    LOG0(lockCurrentState->getVal());
    LOG0("New Power=");
    LOG0(targetState);
    LOG0("\n");

    lockCurrentState->setVal(targetState);

    return (true);
  }

  void loop()
  {

    // if(nfcControlPoint.timeVal()>5000)                        // here we simulate an actual sensor by generating a random pressure reading every 5 seconds
    //   pressure.setVal((double)random(900,1100));

  } // end loop

}; // end LockMechanism

struct NFCAccess : Service::NFCAccess
{ // A standalone Air Pressure Sensor
  SpanCharacteristic *configurationState;
  SpanCharacteristic *nfcControlPoint;
  SpanCharacteristic *nfcSupportedConfiguration;

  NFCAccess() : Service::NFCAccess()
  {
    Serial.print("Configuring NFCAccess"); // initialization message
    Serial.print("\n");
    new Characteristic::Name("NFC Access");
    configurationState = new Characteristic::ConfigurationState();
    nfcControlPoint = new Characteristic::NFCAccessControlPoint();
    nfcSupportedConfiguration = new Characteristic::NFCAccessSupportedConfiguration();
  } // end constructor

  boolean update(char *callback, int *callbackLen)
  {
    if (readerData.reader_private_key)
      LOG1("*** EXISTENT READER KEY:");

    for (int cnt = 0; cnt < sizeof(readerData.reader_private_key); cnt++)
    {
      LOG1("%02x", readerData.reader_private_key[cnt]);
    }
    LOG1("\n");
    if (readerData.reader_private_key)
      LOG1("*** EXISTENT READER IDENTIFIER:");
    for (int cnt = 0; cnt < sizeof(readerData.reader_identifier); cnt++)
    {
      LOG1("%02x", readerData.reader_identifier[cnt]);
    }
    LOG1("\n");

    TLV<Reader_Tags, 10> tlv8;

    tlv8.create(kTLVType1_Operation, 1, "OPERATION");
    tlv8.create(kTLVType1_Device_Credential_Request, 82, "DEV.REQ");
    tlv8.create(kTLVType1_Device_Credential_Response, 82, "DEV.RES");
    tlv8.create(kTLVType1_Reader_Key_Request, 64, "KEY.REQ");
    tlv8.create(kTLVType1_Reader_Key_Response, 64, "KEY.RES");

    char *dataConfState = configurationState->getNewString();
    char *dataNfcControlPoint = nfcControlPoint->getNewString();
    LOG1("*** New Value ConfState: ");
    LOG1(dataConfState);
    LOG1("*** New Value NfcControlPoint: ");
    LOG1(dataNfcControlPoint);
    LOG1("\n");
    LOG1("*** Value Length: %d \n", strlen(dataNfcControlPoint));
    size_t lTlv = decodeB64(NULL, dataNfcControlPoint, 0);
    unsigned char dTlv[lTlv];
    decodeB64(dTlv, dataNfcControlPoint, lTlv);
    tlv8.unpack(dTlv, lTlv);
    tlv8.print(1);
    if (tlv8.val(kTLVType1_Operation) == 1)
    {
      if (tlv8.buf(kTLVType1_Reader_Key_Request) != NULL)
      {
        LOG1("*** REQ TO GET READER KEY\n");

        if (strlen(readerData.reader_private_key) > 0)
        {
          size_t out_len = 0;
          TLV<Reader_Key_Response, 10> tlv8;
          tlv8.create(kReader_Res_Key_Identifier, 8, "KEY.IDENTIFIER");
          memcpy(tlv8.buf(kReader_Res_Key_Identifier, 8), readerData.reader_identifier, 8);
          size_t lenSubTlv = tlv8.pack(NULL);
          LOG1("SUB-TLV LENGTH: %d\n", lenSubTlv);
          uint8_t subTlv[lenSubTlv];
          tlv8.pack(subTlv);
          tlv8.clear();
          tlv8.create(kReader_Res_Reader_Key_Response, lenSubTlv, "READER.RESPONSE");
          memcpy(tlv8.buf(kReader_Res_Reader_Key_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = tlv8.pack(NULL);
          LOG1("TLV LENGTH: %d\n", lenTlv);
          uint8_t tlv[lenTlv];
          tlv8.pack(tlv);
          for (int i = 0; i < lenTlv; i++)
          {
            LOG1("%02x", tlv[i]);
          }
          LOG1("\n");
          int ret = mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          uint8_t resB64[out_len];
          int ret1 = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          LOG1("\nb64_ret1: %d\n", ret1);
          LOG1("b64_len: %d\n", out_len);
          // LOG1("b64_val: %s", resB64);
          LOG1("\n*** RES LENGTH: %d\n", out_len);
          memcpy(callback, (const char *)resB64, out_len);
          memcpy(callbackLen, &out_len, sizeof(out_len));
        }
      }
    }
    else if (tlv8.val(kTLVType1_Operation) == 2)
    {
      if (tlv8.val(kTLVType1_Reader_Key_Request) != -1)
      {
        LOG1("*** REQ TO SET READER KEY\n");
        int ret = set_reader_key(tlv8.buf(kTLVType1_Reader_Key_Request), tlv8.len(kTLVType1_Reader_Key_Request));
        if (ret == 0)
        {
          LOG1("*** SUCCESS, SENDING RESPONSE");
          size_t out_len = 0;
          TLV<Reader_Key_Response, 10> tlv8;
          tlv8.create(kReader_Res_Status, 1, "STATUS");
          tlv8.val(kReader_Res_Status, 0);
          size_t lenSubTlv = tlv8.pack(NULL);
          LOG1("SUB-TLV LENGTH: %d\n", lenSubTlv);
          uint8_t subTlv[lenSubTlv];
          tlv8.pack(subTlv);
          tlv8.clear();
          tlv8.create(kReader_Res_Reader_Key_Response, lenSubTlv, "READER.RESPONSE");
          memcpy(tlv8.buf(kReader_Res_Reader_Key_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = tlv8.pack(NULL);
          LOG1("TLV LENGTH: %d\n", lenTlv);
          uint8_t tlv[lenTlv];
          tlv8.pack(tlv);
          for (int i = 0; i < lenTlv; i++)
          {
            LOG1("%#x ", tlv[i]);
          }
          int ret = mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          unsigned char resB64[out_len];
          int ret1 = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
          // resB64[out_len] = (unsigned char)'\0';
          LOG1("\nb64_ret1: %d\n", ret1);
          LOG1("b64_len: %d\n", out_len);
          LOG1("b64_val: %s", resB64);
          strncpy(callback, (const char *)resB64, out_len);
          memcpy(callbackLen, &out_len, sizeof(out_len));
        }
      }
      else if (tlv8.val(kTLVType1_Device_Credential_Request) != -1)
      {
        LOG1("*** REQ TO PROVISION DEVICE CREDENTIAL\n");
        provision_device_cred(tlv8.buf(kTLVType1_Device_Credential_Request), tlv8.len(kTLVType1_Device_Credential_Request));

        size_t out_len = 0;
        TLV<Device_Credential_Response, 10> tlv8;
        tlv8.create(kDevice_Res_Key_Identifier, 8, "KEY.IDENTIFIER");
        tlv8.create(kDevice_Res_Issuer_Key_Identifier, 8, "ISSUER.IDENTIFIER");
        tlv8.create(kDevice_Res_Status, 1, "STATUS");
        memcpy(tlv8.buf(kDevice_Res_Issuer_Key_Identifier, 8), readerData.issuers[issuersCount - 1].issuerId, 8);
        tlv8.val(kDevice_Res_Status, 0);
        size_t lenSubTlv = tlv8.pack(NULL);
        LOG1("SUB-TLV LENGTH: %d\n", lenSubTlv);
        uint8_t subTlv[lenSubTlv];
        tlv8.pack(subTlv);
        tlv8.clear();
        tlv8.print(1);
        tlv8.create(kDevice_Credential_Response, lenSubTlv, "DEV.RESPONSE");
        memcpy(tlv8.buf(kDevice_Credential_Response, lenSubTlv), subTlv, lenSubTlv);
        size_t lenTlv = tlv8.pack(NULL);
        LOG1("TLV LENGTH: %d\n", lenTlv);
        uint8_t tlv[lenTlv];
        tlv8.pack(tlv);
        for (int i = 0; i < lenTlv; i++)
        {
          LOG1("%#x ", tlv[i]);
        }
        int ret = mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
        unsigned char resB64[out_len];
        int ret1 = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
        // resB64[out_len] = (unsigned char)'\0';
        LOG1("\nb64_ret1: %d\n", ret1);
        LOG1("b64_len: %d\n", out_len);
        LOG1("b64_val: %s", resB64);
        strncpy(callback, (const char *)resB64, out_len);
        memcpy(callbackLen, &out_len, sizeof(out_len));
      }
    }
    else if (tlv8.val(kTLVType1_Operation) == 3)
    {
      LOG1("*** REQ TO REMOVE READER KEY\n");
      strcpy(readerData.reader_identifier, "");
      strcpy(readerData.reader_private_key, "");
      esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", &readerData, sizeof(readerData));
      esp_err_t commit_nvs = nvs_commit(savedData);
      LOG1("*** NVS W STATUS: \n");
      LOG1("SET: %s\n", esp_err_to_name(set_nvs));
      LOG1("COMMIT: %s\n", esp_err_to_name(commit_nvs));
      LOG1("*** NVS W STATUS: \n");
      const char res[] = "BwMCAQA=";
      size_t resLen = strlen(res);
      strncpy(callback, res, resLen);
      memcpy(callbackLen, &resLen, sizeof(resLen));
    }
    return (true);
  }

  void loop()
  {

  } // end loop

}; // end NFCAccess

//////////////////////////////////////

void setup()
{
  // pinMode(RESET_PIN, OUTPUT);
  // digitalWrite(RESET_PIN, LOW);
  // delay(1); // min 20ns
  // digitalWrite(RESET_PIN, HIGH);
  // delay(2); // max 2ms

  Serial.begin(115200);

  size_t len;
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData); // open a new namespace called SAVED_DATA in the NVS

  if (!nvs_get_blob(savedData, "READERDATA", NULL, &len))     // if LIGHTDATA data found
    nvs_get_blob(savedData, "READERDATA", &readerData, &len); // retrieve data

  nvs_get_u8(savedData, "ISSUERS", &issuersCount);

  homeSpan.setLogLevel(1);

  Serial.printf("\n*** READER KEY IDENTIFIER: ");
  for (int i = 0; i < sizeof(readerData.reader_identifier); i++)
  {
    Serial.printf("%x", readerData.reader_identifier[i]);
  }

  Serial.printf("\n*** HOMEKEY ISSUERS: %d\n", issuersCount);
  for (int i = 0; i < issuersCount; ++i)
  {
    Serial.printf("**Issuer #%d\n", i);
    Serial.printf("\t- Identifier: ");
    for (int j = 0; j < sizeof(readerData.issuers[i].issuerId); j++)
      Serial.printf("%02x", readerData.issuers[i].issuerId[j]);
    Serial.printf("\n\t- Public Key: ");
    for (int j = 0; j < sizeof(readerData.issuers[i].publicKey); j++)
      Serial.printf("%02x", readerData.issuers[i].publicKey[j]);
    Serial.println();
  }
  Serial.printf("\n");

  homeSpan.begin(Category::Locks, "Test NFC Lock");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
    // while (1); // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A card");

  new SpanAccessory();                 // Begin by creating a new Accessory using SpanAccessory(), no arguments needed
  new Service::AccessoryInformation(); // HAP requires every Accessory to implement an AccessoryInformation Service, with the required Identify Characteristic
  new Characteristic::Identify();
  new Characteristic::Manufacturer();
  new Characteristic::Model();
  new Characteristic::Name("NFC Lock");
  new Characteristic::SerialNumber();
  new Characteristic::FirmwareRevision();
  new Characteristic::HardwareFinish();

  new LockManagement();
  new LockMechanism();
  new NFCAccess();
  new Service::HAPProtocolInformation();
  new Characteristic::Version();
}

//////////////////////////////////////

// Function to calculate CRC16
void crc16a(unsigned char *data, unsigned int size, unsigned char *result)
{
  unsigned short w_crc = 0x6363;

  for (unsigned int i = 0; i < size; ++i)
  {
    unsigned char byte = data[i];
    byte = (byte ^ (w_crc & 0x00FF));
    byte = ((byte ^ (byte << 4)) & 0xFF);
    w_crc = ((w_crc >> 8) ^ (byte << 8) ^ (byte << 3) ^ (byte >> 4)) & 0xFFFF;
  }

  result[0] = static_cast<unsigned char>(w_crc & 0xFF);
  result[1] = static_cast<unsigned char>((w_crc >> 8) & 0xFF);
}

// Function to append CRC16 to data
void with_crc16(unsigned char *data, unsigned int size, unsigned char *result)
{
  crc16a(data, size, result);
}

void loop()
{
  homeSpan.poll();
  if (nfc.inListPassiveTarget())
  {
    bool exchange;
    Serial.println("Found something");
    uint8_t data[] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x0};
    uint8_t response[32];
    uint8_t responseLength = 32;
    exchange = nfc.inDataExchange(data, sizeof(data), response, &responseLength, true);
    if (exchange)
    {
      Serial.print("responseLength: ");
      Serial.println(responseLength);
      if (response[responseLength - 2] == 0x90 && response[responseLength - 1] == 0x00)
      {
        Serial.println("\nSuccess!");
        nfc.PrintHex(response, responseLength);
        nfc.PrintHex((const uint8_t *)readerData.reader_private_key, sizeof(readerData.reader_private_key));
        mbedtls_ecp_keypair keypair;
        mbedtls_ecp_keypair_init(&keypair);
        mbedtls_entropy_context entropy;
        mbedtls_ctr_drbg_context ctr_drbg;
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);

        int ecp_key = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &keypair, (const uint8_t *)readerData.reader_private_key, sizeof(readerData.reader_private_key));
        int ret = mbedtls_ecp_mul(&keypair.grp, &keypair.Q, &keypair.d, &keypair.grp.G, mbedtls_ctr_drbg_random, &ctr_drbg);

        size_t olenPub = 0;
        unsigned char buf[MBEDTLS_ECP_MAX_BYTES];
        mbedtls_ecp_point_write_binary(&keypair.grp, &keypair.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olenPub, buf, sizeof(buf));

        // Set buffer size for coordinates
        size_t buffer_size_x = mbedtls_mpi_size(&keypair.Q.X);
        size_t buffer_size_y = mbedtls_mpi_size(&keypair.Q.Y);
        uint8_t x_buffer[buffer_size_x];
        uint8_t y_buffer[buffer_size_y];

        // Write X and Y coordinates into the buffer
        mbedtls_mpi_write_binary(&keypair.Q.X, x_buffer, mbedtls_mpi_size(&keypair.Q.X));
        mbedtls_mpi_write_binary(&keypair.Q.Y, y_buffer, mbedtls_mpi_size(&keypair.Q.Y));

        // Cleanup
        mbedtls_ecp_keypair_free(&keypair);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);

        mbedtls_ecp_keypair ephemeral;
        mbedtls_ecp_keypair_init(&ephemeral);
        mbedtls_entropy_context eph_entropy;
        mbedtls_ctr_drbg_context eph_ctr_drbg;
        mbedtls_entropy_init(&eph_entropy);
        mbedtls_ctr_drbg_init(&eph_ctr_drbg);
        mbedtls_ctr_drbg_seed(&eph_ctr_drbg, mbedtls_entropy_func, &eph_entropy, NULL, 0);
        mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, &ephemeral, mbedtls_ctr_drbg_random, &eph_ctr_drbg);
        unsigned char bufPriv[mbedtls_mpi_size(&ephemeral.d)];
        mbedtls_mpi_write_binary(&ephemeral.d, bufPriv, sizeof(bufPriv));
        Serial.println("**Ephemeral Priv Key: ");
        for (int i = 0; i < sizeof(bufPriv); i++)
        {
          printf("%02x ", bufPriv[i]);
        }
        printf("\n");
        unsigned char bufPub[MBEDTLS_ECP_MAX_BYTES];
        size_t olen = 0;
        mbedtls_ecp_point_write_binary(&ephemeral.grp, &ephemeral.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, bufPub, sizeof(bufPub));
        Serial.println("**Ephemeral Pub Key: ");
        for (int i = 0; i < olen; i++)
        {
          printf("%02x ", bufPub[i]);
        }
        printf("\n");
        fast_auth(buf, olenPub, bufPub, olen, bufPriv, sizeof(bufPriv));
      }
    }
  }
  else
  {
    uint8_t data[18] = {0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0};
    memcpy(data + 8, readerData.reader_identifier, sizeof(readerData.reader_identifier));
    with_crc16(data, 16, data + 16);
    uint8_t response[64];
    uint8_t length = 64;
    nfc.setPassiveActivationRetries(0);
    uint16_t bitFraming = 0x633d;
    uint8_t val = 0;
    nfc.writeRegister(&bitFraming, &val, 1);
    nfc.inCommunicateThru(data, sizeof(data), response, &length, 100, false);
  }
}