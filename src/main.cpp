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
#include "util/x963kdf.h"
#include <mbedtls/hkdf.h>
#include "util/DigitalKeySecureContext.h"
#include "util/ndef.h"
#include <YACL.h>
#include <nlohmann/json.hpp>
#include "util/ISO18013SecureContext.h"
#include <list>
#include <util/BERTLV.h>

using nlohmann::literals::operator ""_json;
using namespace nlohmann::literals;
using namespace nlohmann::json_literals;
using namespace nlohmann::literals::json_literals;
using namespace nlohmann;

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

struct attestation_t
{
  uint32_t time;
  uint8_t payload;
};

struct issuerEnrollments_t
{
  uint32_t hap_at;
  uint8_t payload[56];
  attestation_t attestation;
};

struct issuerEndpoints_t
{
  uint8_t endpointId[6];
  uint32_t last_used_at;
  uint8_t counter;
  uint8_t key_type;
  uint8_t publicKey[65];
  uint8_t persistent_key[32];
  issuerEnrollments_t enrollments[1];
};

struct homeKeyIssuers_t
{
  uint8_t issuerId[8];
  uint8_t publicKey[64];
  issuerEndpoints_t endpoints[1];
};

struct readerData_t
{
  uint8_t reader_private_key[32];
  uint8_t reader_identifier[8];
  uint8_t identifier[8];
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
  kAuth0Status = 0x90
} AUTH0_RESPONSE;

typedef enum
{
  kNDEF_MESSAGE = 0x53,
  kEnv1Status = 0x90
} ENVELOPE_RESPONSE;

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
std::vector<unsigned char> simple_tlv(unsigned char tag, const unsigned char *value, size_t valLength, unsigned char *out = NULL, size_t* olen = NULL){
  uint8_t dataLen[2] = {(unsigned char)valLength};
  bool lenExt = false;
  if (valLength >= 128)
  {
    uint8_t fb = (1 << 7) + sizeof((unsigned char)valLength);
    dataLen[0] = fb;
    dataLen[1] = (unsigned char)valLength;
    lenExt = true;
  }
  size_t len = sizeof(tag) + valLength + (lenExt ? 2 : 1);
  std::vector<unsigned char> buf(len);
  memcpy(buf.data(), &tag, sizeof(tag));
  memcpy(buf.data()+sizeof(tag), dataLen, lenExt ? 2 : 1);
  memcpy(buf.data() +sizeof(tag) + (lenExt ? 2 : 1), value, valLength);
  printf("*** TLV TAG %x[%d]: \n", tag, valLength);
  nfc.PrintHex(buf.data()+(lenExt ? 3: 2), len-(lenExt ? 3: 2));
  if(out != NULL && olen != NULL){
    memcpy(out, buf.data(), len);
    size_t l = len + *olen;
    memcpy(olen, &l, sizeof(len));
  }
  return buf;
}

void pack(uint8_t *buf, size_t buflen, uint8_t *out, int* olen){
  memcpy(out + *olen, buf, buflen);
  size_t l = *olen + buflen;
  memcpy(olen, &l, 1);
}

int provision_device_cred(uint8_t *buf, size_t len)
{
  TLV<Device_Credential_Request, 5> tlv8;
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
  bool existent = false;
  int index_ex = 0;
  for (int i = 0; i < issuersCount; ++i)
  {
    if(!memcmp(readerData.issuers[i].issuerId, tlv8.buf(kDevice_Req_Issuer_Key_Identifier), 8)){
      existent = true;
      index_ex = i;
    }
  }
  if(existent) {
    memcpy(readerData.issuers[index_ex].publicKey, tlv8.buf(kDevice_Req_Public_Key), tlv8.len(kDevice_Req_Public_Key));
  } else {
    memcpy(readerData.issuers[issuersCount].issuerId, tlv8.buf(kDevice_Req_Issuer_Key_Identifier), tlv8.len(kDevice_Req_Issuer_Key_Identifier));
    memcpy(readerData.issuers[issuersCount].publicKey, tlv8.buf(kDevice_Req_Public_Key), tlv8.len(kDevice_Req_Public_Key));
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

std::vector<unsigned char> attestation_salt(std::vector<unsigned char> env1Data, std::vector<unsigned char> readerCmd){
    TLV<ENVELOPE_RESPONSE, 2> env1ResTlv;
  env1ResTlv.create(kNDEF_MESSAGE, 128, "NDEF.MESSAGE");
  env1ResTlv.create(kEnv1Status, 2, "STATUS");
  env1ResTlv.unpack(env1Data.data(), env1Data.size());
  env1ResTlv.print(1);
  NDEFMessage ndefEnv1Ctx = NDEFMessage(env1ResTlv.buf(kNDEF_MESSAGE), env1ResTlv.len(kNDEF_MESSAGE));
  auto ndefEnv1Data = ndefEnv1Ctx.unpack();
  auto ndefEnv1Pack = ndefEnv1Ctx.pack();
  for (int i = 0; i < ndefEnv1Data.size(); i++)
  {
    printf("\n%s\n", ndefEnv1Data.data()[i].id.data() );
  }
  NDEFRecord res_eng = ndefEnv1Ctx.findType("iso.org:18013:deviceengagement");
  Serial.printf("\n%s\n", res_eng.type.data());
  CBOR deviceEngagement = CBOR();

  deviceEngagement.encode(res_eng.data.data(), res_eng.data.size() - 1);

  CBOR readerNdef = CBOR();
  readerNdef.encode(readerCmd.data(), readerCmd.size());
  CBOR deviceNdef = CBOR();
  deviceNdef.encode(env1ResTlv.buf(kNDEF_MESSAGE), env1ResTlv.len(kNDEF_MESSAGE));

  CBOR tag1 = CBOR(24, deviceEngagement);
  json tagArray = {json::from_cbor(tag1.to_CBOR(), tag1.to_CBOR() + tag1.length(), true, true, json::cbor_tag_handler_t::store), {json::from_cbor(deviceNdef.to_CBOR(), deviceNdef.to_CBOR() + deviceNdef.length(), true, true, json::cbor_tag_handler_t::store), json::from_cbor(readerNdef.to_CBOR(), readerNdef.to_CBOR() + readerNdef.length(), true, true, json::cbor_tag_handler_t::store)}};
  std::cout << tagArray << std::endl;

  std::vector<uint8_t> cborArr = json::to_cbor(tagArray);

  CBOR arrBuf = CBOR();

  arrBuf.encode(cborArr.data(), cborArr.size());

  CBOR tag2 = CBOR();

  tag2.encode(24, arrBuf);

  unsigned char hash[32];
  mbedtls_sha256((const unsigned char *)tag2.get_buffer(), tag2.length(), hash, false);

  std::vector<uint8_t> salt(sizeof(hash));

  memcpy(&salt[0], hash, 32);
  printf("\n");

  std::cout << "salt:" << std::endl;
  for (auto& byte : salt)
  {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;

  return salt;
}

std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> envelope1Cmd(){
    uint8_t controlFlow[4] = {0x80, 0x3c, 0x40, 0xa0};
    uint8_t controlRes[8];
    uint8_t controlResLen = 8;
    nfc.inDataExchange(controlFlow, sizeof(controlFlow),  controlRes, &controlResLen);
    nfc.PrintHex(controlRes, controlResLen);

    // cla=0x00; ins=0xa4; p1=0x04; p2=0x00; lc=0x07(7); data=a0000008580102; le=0x00
    uint8_t data[] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x02, 0x0};
    uint8_t response[4];
    uint8_t responseLength = 4;
    nfc.inDataExchange(data, sizeof(data), response, &responseLength);
    nfc.PrintHex(response, responseLength);
    unsigned char payload[] = {0x15, 0x91, 0x02, 0x02, 0x63, 0x72, 0x01, 0x02, 0x51, 0x02, 0x11, 0x61, 0x63, 0x01, 0x03, 0x6e, 0x66, 0x63, 0x01, 0x0a, 0x6d, 0x64, 0x6f, 0x63, 0x72, 0x65, 0x61, 0x64, 0x65, 0x72};
    unsigned char payload1[] = {0x01};
    unsigned char payload2[] = {0xa2, 0x00, 0x63, 0x31, 0x2e, 0x30, 0x20, 0x81, 0x29};
    auto ndefMessage = NDEFMessage({
      NDEFRecord("", 0x01, "Hr", payload, sizeof(payload)),
      NDEFRecord("nfc", 0x04, "iso.org:18013:nfc", payload1, 1),
      NDEFRecord("mdocreader", 0x04, "iso.org:18013:readerengagement", payload2, sizeof(payload2))
    }).pack();
    auto envelope1Tlv = simple_tlv(0x53, ndefMessage.data(), ndefMessage.size(), NULL, NULL);
    for (int i = 0; i < envelope1Tlv.size();i++){
      printf("%02x ", envelope1Tlv.data()[i]);
    }
    printf("\n");
    uint8_t env1Apdu[envelope1Tlv.size() + 6] = {0x00, 0xc3, 0x00, 0x01, static_cast<uint8_t>(envelope1Tlv.size())};
    memcpy(env1Apdu + 5, envelope1Tlv.data(), envelope1Tlv.size());
    uint8_t env1Res[128];
    uint8_t env1ResLen = 128;
    nfc.inDataExchange(env1Apdu, sizeof(env1Apdu), env1Res, &env1ResLen);
    nfc.PrintHex(env1Res, env1ResLen);
    return std::make_tuple(std::vector<unsigned char> {env1Res, env1Res + env1ResLen}, ndefMessage);
}

void envelope2Cmd(std::vector<uint8_t> env1Data, std::vector<uint8_t> readerCmd, std::vector<uint8_t> shared_secret){
  std::cout << std::endl << "env1Data: " << std::endl;
  for (auto& byte : env1Data)
  {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;
  std::cout << "readerCmd: " << std::endl;
  for (auto& byte : readerCmd)
  {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;
  std::cout << "shared_secret: " << std::endl;
  for (auto& byte : shared_secret)
  {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;

  auto salt = attestation_salt(env1Data, readerCmd);

  ISO18013SecureContext secureCtx = ISO18013SecureContext(shared_secret, salt, 16);
  json ex1 = json::parse(R"({ "docType":"com.apple.HomeKit.1.credential","nameSpaces":{"com.apple.HomeKit":{"credential_id": false}}})");
  auto cbor = ex1.to_cbor(ex1);
  printf("\ncbor doc_type:");
  for (auto& byte: cbor)
  {
    printf("%02x", byte);
  }
  printf("\n");
  CBOR itemRequest = CBOR();
  itemRequest.encode(cbor.data(), cbor.size());
  CBOR itemRequestTag = CBOR();
  itemRequestTag.encode(24, itemRequest);
  json message_to_encrypt;
  message_to_encrypt["version"] = "1.0";
  message_to_encrypt["docRequests"] = {};
  message_to_encrypt["docRequests"].push_back({{"itemsRequest", 0}});
  message_to_encrypt["docRequests"][0]["itemsRequest"] = json::from_cbor(itemRequestTag.get_buffer(), itemRequestTag.get_buffer() + itemRequestTag.length(), true, true, json::cbor_tag_handler_t::store);
  std::vector<std::uint8_t> v = json::to_cbor(message_to_encrypt);
  std::cout << std::endl;
  for (auto& byte : v)
  {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;
  auto encrypted = secureCtx.encryptMessageToEndpoint(v);
  printf("\ncbor enc_docRequests: ");

  for (auto& byte : encrypted)
  {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;

  auto tlv = simple_tlv(0x53, encrypted.data(), encrypted.size());

  for (int i= 0; i < tlv.size();i++)
  {
    printf("%02x", tlv[i]);
  }
  std::cout << std::endl;

  unsigned char apdu[6 + tlv.size()] = {0x0, 0xC3, 0x0, 0x0, (unsigned char)tlv.size()};

  memcpy(apdu + 5, tlv.data(), tlv.size());
  for (int i= 0; i < sizeof(apdu);i++)
  {
    printf("%02x", apdu[i]);
  }
  std::cout << std::endl;
  uint8_t newLen = 255;
  uint8_t *env2Res = new unsigned char[255];
  std::vector<unsigned char> attestation_package;
  size_t totalLength = 0;
  uint8_t getData[5] = {0x0, 0xc0, 0x0, 0x0, 0x0};
  nfc.inDataExchangeT4(apdu, sizeof(apdu), env2Res, &newLen);
  attestation_package.insert(attestation_package.begin(), env2Res, env2Res + newLen);
  printf("\nLength chunk: %d\n", newLen);
  nfc.PrintHex(env2Res, newLen);
  while (newLen == 253)
  {
    newLen = 255;
    nfc.inDataExchangeT4(getData, sizeof(getData), env2Res, &newLen);
    attestation_package.insert(attestation_package.end(), env2Res, env2Res + newLen);
    printf("\nLength chunk: %d\n", newLen);
    nfc.PrintHex(env2Res, newLen);
  }
  delete[] env2Res;
  printf("\ntotal package length: %d\n", attestation_package.size());
  for (auto& data: attestation_package){
    printf("%02x ", data);
  }
  printf("\n");
  printf("\n");
  BERTLV data = BERTLV::unpack(attestation_package);
  for(auto& byte: data.value){
    printf("%02x ", byte);
  }
  printf("\n");
  auto decrypted_message = secureCtx.decryptMessageFromEndpoint(data.value);
  for (auto& data: decrypted_message){
    printf("%02x ", data);
  }
}

std::tuple<std::vector<unsigned char>, std::vector<unsigned char>> attestation_exchange(DigitalKeySecureContext context){
  uint8_t attestation_exchange_common_secret[32];
  esp_fill_random(attestation_exchange_common_secret, 32);
  auto attTlv = simple_tlv(0xC0, attestation_exchange_common_secret, 32, NULL, NULL);
  auto opAttTlv = simple_tlv(0x8E, attTlv.data(), attTlv.size(), NULL, NULL);
  uint8_t attComm[opAttTlv.size() + 1] = {0x0};
  memcpy(attComm + 1, opAttTlv.data(), opAttTlv.size());
  nfc.PrintHex(attComm, sizeof(attComm));
  auto encrypted = context.encrypt_command(attComm, sizeof(attComm));

  nfc.PrintHex(std::get<0>(encrypted), std::get<1>(encrypted));
  nfc.PrintHex(std::get<2>(encrypted), 8);
  uint8_t exchApdu[std::get<1>(encrypted) + 5] = {0x84, 0xc9, 0x0, 0x0, (uint8_t)std::get<1>(encrypted)};
  memcpy(exchApdu + 5, std::get<0>(encrypted), std::get<1>(encrypted));
  uint8_t exchRes[16];
  uint8_t exchResLen = 16;
  nfc.inDataExchange(exchApdu, sizeof(exchApdu), exchRes, &exchResLen);
  nfc.PrintHex(exchRes, exchResLen);
  return std::make_tuple(std::vector<unsigned char> {exchRes, exchRes + exchResLen}, std::vector<unsigned char>{attestation_exchange_common_secret, attestation_exchange_common_secret+32});
}

std::vector<uint8_t> signSharedInfo(uint8_t *stdTlv, size_t len){
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
  // uint8_t sigTlv[sizeof(sigPoint) + 2];
  // size_t sigTlvLen = 0;
  return simple_tlv(0x9E, sigPoint, sizeof(sigPoint));
}

void std_auth(uint8_t *readerIdentifier, uint8_t *transId, uint8_t *readerEphPrivKey, size_t readerEphLen, uint8_t *readerEphPubKey, size_t readerEphPubLen, uint8_t *endEphPublicKey, size_t endEphLen){
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
  uint8_t *stdTlv = new unsigned char[110];
  size_t len = 0;
  simple_tlv(0x4D, readerIdentifier, sizeof(readerData.reader_identifier) + sizeof(readerData.identifier), stdTlv, &len);
  simple_tlv(0x86, endpointEph_X, endpointEph_XLen, stdTlv+len, &len);
  simple_tlv(0x87, readerEph_X, readerEph_XLen, stdTlv+len, &len);
  simple_tlv(0x4C, transId, 16, stdTlv+len, &len);
  simple_tlv(0x93, bytes, 4, stdTlv+len, &len);
  Serial.println("*** Packed TLV: ");
  nfc.PrintHex(stdTlv, len);
  auto sigTlv = signSharedInfo(stdTlv, len);
  uint8_t apdu[sigTlv.size() + 5] = {0x80, 0x81, 0x0, 0x0, (uint8_t)sigTlv.size()};
  memcpy(apdu + 5, sigTlv.data(), sigTlv.size());
  uint8_t *response = new unsigned char[91];
  uint8_t responseLength = 91;
  nfc.inDataExchange(apdu, sizeof(apdu), response, &responseLength);
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
      auto att1_res = attestation_exchange(context);
      size_t att1Len = std::get<0>(att1_res).size();
      if (att1Len > 2 && std ::get<0>(att1_res).data()[att1Len - 2] == 0x90)
      {
        auto env1 = envelope1Cmd();
        size_t env1Len = std::get<0>(env1).size();
        if (env1Len > 2 && std::get<0>(env1).data()[env1Len - 2] == 0x90)
        {
          envelope2Cmd(std::get<0>(env1), std::get<1>(env1),std::get<1>(att1_res));
        }
      }
    }
  }
  delete[] stdTlv;
  delete[] response;
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

  uint8_t *fastTlv = new unsigned char[110];
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
  uint8_t *apdu = new unsigned char[113]{0x80, 0x80, 0x01, 0x01, (uint8_t)len};
  memcpy(apdu + 5, fastTlv, len);
  bool exchange;
  uint8_t *response = new unsigned char[90];
  uint8_t responseLength = 90;
  exchange = nfc.inDataExchange(apdu, 113, response, &responseLength);
  Serial.print("***RESPONSE: ");
  nfc.PrintHex(response, responseLength);


  if(response[responseLength - 2] == 0x90){
    TLV<AUTH0_RESPONSE, 10> Auth0Res;

    Auth0Res.create(kEndpoint_Public_Key, 65, "END.PUBKEY");
    Auth0Res.create(kAuth0_Cryptogram, 16, "END.PUBKEY");
    Auth0Res.create(kAuth0Status, 1, "STATUS");

    Auth0Res.unpack(response, responseLength);

    std_auth(identifier, transactionIdentifier, readerEphemeralPrivKey, ephPrivLen, readerEphemeralPublicKey, ephLen, Auth0Res.buf(kEndpoint_Public_Key), Auth0Res.len(kEndpoint_Public_Key));
  }
  delete[] apdu;
  delete[] fastTlv;
  delete[] response;
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

        if (strlen((const char *)readerData.reader_private_key) > 0)
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
          LOG1("b64_val: %s", resB64);
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
          resB64[out_len] = '\0';
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
        resB64[out_len] = '\0';
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
      std::fill(readerData.reader_identifier, readerData.reader_identifier + 8, 0);
      std::fill(readerData.reader_private_key, readerData.reader_private_key + 32, 0);
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

void deleteReaderData(const char* buf){
  uint8_t empty[64];
  std::fill(empty, empty + 64, 0);
  for (uint8_t i = 0; i < issuersCount; ++i)
  {
    for (int j = 0; j < sizeof(readerData.issuers[i].issuerId); j++)
      memcpy(readerData.issuers[i].issuerId, empty, 8);
      memcpy(readerData.issuers[i].publicKey, empty, 64);
  }
  issuersCount = 0;
  memcpy(readerData.identifier, empty, 8);
  memcpy(readerData.reader_identifier, empty, 8);
  memcpy(readerData.reader_private_key, empty, 32);
  esp_err_t set_issuers = nvs_set_u8(savedData, "ISSUERS", issuersCount);
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", &readerData, sizeof(readerData));
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG1("*** NVS W STATUS: \n");
  LOG1("ISSUERS: %s\n", esp_err_to_name(set_issuers));
  LOG1("READER DATA: %s\n", esp_err_to_name(set_nvs));
  LOG1("COMMIT: %s\n", esp_err_to_name(commit_nvs));
  LOG1("*** NVS W STATUS: \n");
}

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

  new SpanUserCommand('D',"Delete NFC Reader Data",deleteReaderData);

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
    exchange = nfc.inDataExchange(data, sizeof(data), response, &responseLength);
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
        unsigned char *buf = new unsigned char[MBEDTLS_ECP_MAX_BYTES];
        mbedtls_ecp_point_write_binary(&keypair.grp, &keypair.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olenPub, buf, MBEDTLS_ECP_MAX_BYTES);

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
        unsigned char *bufPub = new unsigned char[MBEDTLS_ECP_MAX_BYTES];
        size_t olen = 0;
        mbedtls_ecp_point_write_binary(&ephemeral.grp, &ephemeral.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, bufPub, MBEDTLS_ECP_MAX_BYTES);
        Serial.println("**Ephemeral Pub Key: ");
        for (int i = 0; i < olen; i++)
        {
          printf("%02x ", bufPub[i]);
        }
        printf("\n");
        fast_auth(buf, olenPub, bufPub, olen, bufPriv, sizeof(bufPriv));
        delete[] buf;
        delete[] bufPub;
      }
    }
  }
  else
  {
    // uint8_t data[18] = {0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0};
    // memcpy(data + 8, readerData.reader_identifier, sizeof(readerData.reader_identifier));
    // with_crc16(data, 16, data + 16);
    // uint8_t response[64];
    // uint8_t length = 64;
    // nfc.setPassiveActivationRetries(0);
    // nfc.writeRegister(0x633d, 0);
    // nfc.inCommunicateThru(data, sizeof(data), response, &length, 1000);
  }
}