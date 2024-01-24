#include <auth/authContext.h>
#include <HomeKey.h>
#include <util/utils.h>
#include "HomeSpan.h"
#include "TLV.h"
#include <mbedtls/sha256.h>
#include <mbedtls/ecp.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <list>
#include <util/BERTLV.h>
#include "HAP.h"
#include <mbedtls/sha1.h>
#include <mbedtls/error.h>
#include <mbedtls/asn1write.h>
#include <logging.hpp>
#include <util/uart-appender.h>

using namespace esp32m;
using namespace nlohmann;

#define PN532_SS (D8)
PN532_SPI pn532spi(SPI, PN532_SS);
PN532 nfc(pn532spi);

nvs_handle savedData;
homeKeyReader::readerData_t readerData;
bool defaultToStd = false;

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

struct LockMechanism : Service::LockMechanism, public SimpleLoggable
{
  SpanCharacteristic *lockCurrentState;
  SpanCharacteristic *lockTargetState;

  LockMechanism() : Service::LockMechanism(), SimpleLoggable("LockMechanism")
  {
    Serial.print("Configuring LockMechanism"); // initialization message
    Serial.print("\n");
    new Characteristic::Name("NFC Lock");
    lockCurrentState = new Characteristic::LockCurrentState(0, true);
    lockTargetState = new Characteristic::LockTargetState();

  } // end constructor

  boolean update(std::vector<char> *callback, int *callbackLen)
  {
    int targetState = lockTargetState->getNewVal();
    logI("New LockState=%d, Current LockState=%d", targetState, lockCurrentState->getVal());

    lockCurrentState->setVal(targetState);

    return (true);
  }

  void loop()
  {
    uint8_t uid[16];
    uint8_t uidLen = 0;
    uint16_t atqa[2];
    uint8_t sak[1];
    bool passiveTarget = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, atqa, sak, 1000, true);
    if (passiveTarget)
    {
      logV("ATQA: %s", utils::bufToHexString(atqa, 2).c_str());
      logV("SAK: %s", utils::bufToHexString(sak, 1).c_str());
      logD("UID: %s", utils::bufToHexString(uid, uidLen).c_str());
      unsigned long startTime = millis();
      bool exchange;
      this->logger().logf(LogLevel::None, "*** PASSIVE TARGET DETECTED ***");
      uint8_t data[13] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x0};
      uint8_t selectCmdRes[32];
      uint8_t selectCmdResLength = 32;
      logD("SELECT HomeKey Applet, APDU: %s", utils::bufToHexString(data, sizeof(data)).c_str());
      exchange = nfc.inDataExchange(data, sizeof(data), selectCmdRes, &selectCmdResLength);
      logI("SELECT HomeKey Applet, Response: %s, Length: %d", utils::bufToHexString(selectCmdRes, selectCmdResLength).c_str(), selectCmdResLength);
      if (exchange)
      {
        if (selectCmdRes[selectCmdResLength - 2] == 0x90 && selectCmdRes[selectCmdResLength - 1] == 0x00)
        {
          logI("*** SELECT HOMEKEY APPLET SUCCESSFUL ***");
          logD("Reader Private Key: %s", utils::bufToHexString((const uint8_t *)readerData.reader_private_key, sizeof(readerData.reader_private_key)).c_str());
          AuthenticationContext flow(&nfc, &readerData);
          auto auth = flow.fast_auth(defaultToStd);
          if (std::get<0>(auth) != nullptr && std::get<1>(auth) != 99)
          {
            unsigned long stopTime = millis();
            this->logger().logf(LogLevel::None, "Transaction took %lu ms", stopTime - startTime);
            Serial.println("Device has been authenticated, toggling lock state");
            lockTargetState->setVal(!lockCurrentState->getVal());
            lockCurrentState->setVal(lockTargetState->getVal());
          }
          else
          {
            auto auth1 = flow.std_auth();
            issuerEndpoints::issuerEndpoints_t *foundEndpoint = std::get<0>(auth1);
            if (foundEndpoint != nullptr && std::get<3>(auth1) == homeKeyReader::kFlowSTANDARD)
            {
              unsigned long stopTime = millis();
              this->logger().logf(LogLevel::None, "Transaction took %lu ms", stopTime - startTime);
              Serial.println("Device has been authenticated, toggling lock state");
              lockTargetState->setVal(!lockCurrentState->getVal());
              lockCurrentState->setVal(lockTargetState->getVal());
              std::vector<uint8_t> persistentKey = std::get<2>(auth1);
              memcpy(foundEndpoint->persistent_key, persistentKey.data(), 32);
              json data = readerData;
              auto msgpack = json::to_msgpack(data);
              esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
              esp_err_t commit_nvs = nvs_commit(savedData);
              logD("NVS SET: %s\n", esp_err_to_name(set_nvs));
              logD("NVS COMMIT: %s\n", esp_err_to_name(commit_nvs));
            }
          }
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
      nfc.writeRegister(0x633d, 0);
      nfc.inCommunicateThru(data, sizeof(data), response, &length, 1000);
    }
  } // end loop

}; // end LockMechanism

struct NFCAccess : Service::NFCAccess, public SimpleLoggable
{ // A standalone Air Pressure Sensor
  SpanCharacteristic *configurationState;
  SpanCharacteristic *nfcControlPoint;
  SpanCharacteristic *nfcSupportedConfiguration;

  NFCAccess() : Service::NFCAccess(), SimpleLoggable("NFCAccess")
  {
    logI("Configuring NFCAccess"); // initialization message
    new Characteristic::Name("NFC Access");
    configurationState = new Characteristic::ConfigurationState();
    nfcControlPoint = new Characteristic::NFCAccessControlPoint();
    nfcSupportedConfiguration = new Characteristic::NFCAccessSupportedConfiguration();
  } // end constructor
  std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> get_public_points(std::vector<uint8_t> pubKey)
  {
    mbedtls_ecp_group grp;
    mbedtls_ecp_point point;
    mbedtls_ecp_point_init(&point);
    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    int ret = mbedtls_ecp_point_read_binary(&grp, &point, pubKey.data(), pubKey.size());
    logV("mbedtls_ecp_point_read_binary status: %d", ret);
    size_t buffer_size_x = mbedtls_mpi_size(&point.X);
    size_t buffer_size_y = mbedtls_mpi_size(&point.Y);
    uint8_t X[buffer_size_x];
    uint8_t Y[buffer_size_y];
    // Write X and Y coordinates into the buffer
    mbedtls_mpi_write_binary(&point.X, X, mbedtls_mpi_size(&point.X));
    mbedtls_mpi_write_binary(&point.Y, Y, mbedtls_mpi_size(&point.Y));
    logV("PublicKey: %s, X Coordinate: %s, Y Coordinate: %s", utils::bufToHexString(pubKey.data(), pubKey.size()).c_str(), utils::bufToHexString(X, sizeof(X)).c_str(), utils::bufToHexString(Y, sizeof(Y)).c_str());
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&point);
    return std::make_tuple(std::vector<uint8_t>{X, X + buffer_size_x}, std::vector<uint8_t>{Y, Y + buffer_size_y});
  }
  std::tuple<uint8_t *, int> provision_device_cred(uint8_t *buf, size_t len)
  {
    for (size_t i = 0; i < 16; i++)
    {
      if (HAPClient::controllers[i].allocated)
      {
        std::vector<uint8_t> id = utils::getHashIdentifier(HAPClient::controllers[i].LTPK, 32, true);
        logD("Found allocated controller - ID: %s", utils::bufToHexString(id.data(), 8).c_str());
        Issuers::homeKeyIssuers_t *foundIssuer = 0;
        for (auto &issuer : readerData.issuers)
        {
          nfc.PrintHex(issuer.issuerId, 8);
          HAPClient::hexPrintRow(HAPClient::controllers[i].LTPK, 32);
          if (!memcmp(issuer.issuerId, id.data(), 8))
          {
            logD("Issuer %s already added, skipping", utils::bufToHexString(issuer.issuerId, 8).c_str());
            foundIssuer = &issuer;
            break;
          }
        }
        if (foundIssuer == 0)
        {
          logI("Adding new issuer - ID: %s", utils::bufToHexString(id.data(), 8).c_str());
          Issuers::homeKeyIssuers_t issuer;
          memcpy(issuer.issuerId, id.data(), 8);
          memcpy(issuer.publicKey, HAPClient::controllers[i].LTPK, 32);
          readerData.issuers.emplace_back(issuer);
          json serializedData = readerData;
          auto msgpack = json::to_msgpack(serializedData);
          esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
          esp_err_t commit_nvs = nvs_commit(savedData);
          logD("NVS SET STATUS: %s", esp_err_to_name(set_nvs));
          logD("NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
        }
      }
    }
    TLV<Device_Credential_Request, 5> tlv8;
    logD("DCR Buffer length: %d, data: %s", len, utils::bufToHexString(buf, len).c_str());
    tlv8.create(kDevice_Req_Key_Type, 1, "KEY.TYPE");
    tlv8.create(kDevice_Req_Public_Key, 65, "PUBLIC.KEY");
    tlv8.create(kDevice_Req_Issuer_Key_Identifier, 8, "ISSUER.IDENTIFIER");
    tlv8.create(kDevice_Req_Key_State, 1, "KEY.STATE");
    tlv8.create(kDevice_Req_Key_Identifier, 8, "KEY.IDENTIFIER");

    logV("DCR TLV DECODE STATE: %d", tlv8.unpack(buf, len));
    tlv8.print(1);
    Issuers::homeKeyIssuers_t *foundIssuer = 0;
    for (auto &issuer : readerData.issuers)
    {
      if (!memcmp(issuer.issuerId, tlv8.buf(kDevice_Req_Issuer_Key_Identifier), 8))
      {
        logD("Found issuer - ID: %s", utils::bufToHexString(issuer.issuerId, 8).c_str());
        foundIssuer = &issuer;
      }
    }
    if (foundIssuer != 0)
    {
      issuerEndpoints::issuerEndpoints_t *foundEndpoint = 0;
      uint8_t endEphPubKey[tlv8.len(kDevice_Req_Public_Key) + 1] = {0x04};
      memcpy(endEphPubKey + 1, tlv8.buf(kDevice_Req_Public_Key), tlv8.len(kDevice_Req_Public_Key));
      std::vector<uint8_t> endpointId = utils::getHashIdentifier(endEphPubKey, sizeof(endEphPubKey), false);
      for (auto &endpoint : foundIssuer->endpoints)
      {
        if (!memcmp(endpoint.endpointId, endpointId.data(), 6))
        {
          logD("Found endpoint - ID: %s", utils::bufToHexString(endpoint.endpointId, 6).c_str());
          foundEndpoint = &endpoint;
        }
      }
      if (foundEndpoint == 0)
      {
        logI("Adding new endpoint - ID: %s , PublicKey: %s", utils::bufToHexString(endpointId.data(), 6).c_str(), utils::bufToHexString(endEphPubKey, sizeof(endEphPubKey)).c_str());
        issuerEndpoints::issuerEndpoints_t endpoint;
        endpointEnrollment::enrollment_t hap;
        hap.unixTime = std::time(nullptr);
        uint8_t encoded[128];
        size_t olen = 0;
        mbedtls_base64_encode(encoded, 128, &olen, buf, len);
        hap.payload.insert(hap.payload.begin(), encoded, encoded + olen);
        std::vector<uint8_t> x_coordinate = std::get<0>(get_public_points(std::vector<uint8_t>(endEphPubKey, endEphPubKey + sizeof(endEphPubKey))));

        endpoint.counter = 0;
        endpoint.key_type = tlv8.buf(kDevice_Req_Key_Type)[0];
        endpoint.last_used_at = 0;
        endpoint.enrollments.hap = hap;
        uint8_t persistentKey[32];
        esp_fill_random(persistentKey, 32);
        memcpy(endpoint.endpointId, endpointId.data(), 6);
        memcpy(endpoint.publicKey, endEphPubKey, sizeof(endEphPubKey));
        memcpy(endpoint.endpoint_key_x, x_coordinate.data(), x_coordinate.size());
        memcpy(endpoint.persistent_key, persistentKey, sizeof(persistentKey));
        foundIssuer->endpoints.emplace_back(endpoint);
        return std::make_tuple(foundIssuer->issuerId, homeKeyReader::SUCCESS);
      }
      else
      {
        logI("Endpoint already exists - ID: %s", utils::bufToHexString(foundEndpoint->endpointId, 6).c_str());
        return std::make_tuple(readerData.reader_identifier, homeKeyReader::DUPLICATE);
      }
      json serializedData = readerData;
      auto msgpack = json::to_msgpack(serializedData);
      esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
      esp_err_t commit_nvs = nvs_commit(savedData);
      logD("NVS SET STATUS: %s", esp_err_to_name(set_nvs));
      logD("NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
      tlv8.clear();
    }
    else
    {
      logI("Issuer does not exist - ID: %s", utils::bufToHexString(tlv8.buf(kDevice_Req_Issuer_Key_Identifier), 8).c_str());
      return std::make_tuple(readerData.reader_identifier, homeKeyReader::DOES_NOT_EXIST);
    }
    return std::make_tuple(readerData.reader_identifier, homeKeyReader::DOES_NOT_EXIST);
  }

  std::vector<uint8_t> getPublicKey(std::vector<uint8_t> privKey)
  {
    mbedtls_ecp_keypair keypair;
    mbedtls_ecp_keypair_init(&keypair);
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);

    int ecp_key = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &keypair, (const uint8_t *)privKey.data(), privKey.size());
    int ret = mbedtls_ecp_mul(&keypair.grp, &keypair.Q, &keypair.d, &keypair.grp.G, mbedtls_ctr_drbg_random, &ctr_drbg);

    size_t olenPub = 0;
    unsigned char readerPublicKey[MBEDTLS_ECP_MAX_BYTES];
    mbedtls_ecp_point_write_binary(&keypair.grp, &keypair.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olenPub, readerPublicKey, sizeof(readerPublicKey));

    // Cleanup
    mbedtls_ecp_keypair_free(&keypair);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    return std::vector<uint8_t>(readerPublicKey, readerPublicKey + olenPub);
  }

  int set_reader_key(uint8_t *buf, size_t len)
  {
    logI("Setting reader key: %s", utils::bufToHexString(buf, len).c_str());
    TLV<Reader_Key_Request, 3> tlv8;
    tlv8.create(kReader_Req_Key_Type, 1, "KEY.TYPE");
    tlv8.create(kReader_Req_Reader_Private_Key, 32, "READER.PRIV.KEY");
    tlv8.create(kReader_Req_Identifier, 8, "IDENTIFIER");
    // tlv8.create(kRequest_Reader_Key_Request, 64, "READER.REQ");
    // tlv8.create(kReader_Req_Key_Identifier, 64, "KEY.IDENTIFIER");

    logV("RKR TLV DECODE STATE: %d", tlv8.unpack(buf, len));
    tlv8.print(1);
    uint8_t *readerKey = tlv8.buf(kReader_Req_Reader_Private_Key);
    uint8_t *uniqueIdentifier = tlv8.buf(kReader_Req_Identifier);
    logI("Reader Key: %s", utils::bufToHexString(readerKey, tlv8.len(kReader_Req_Reader_Private_Key)).c_str());
    logI("UniqueIdentifier: %s", utils::bufToHexString(uniqueIdentifier, tlv8.len(kReader_Req_Identifier)).c_str());
    std::vector<uint8_t> pubKey = getPublicKey(std::vector<uint8_t>{readerKey, readerKey + tlv8.len(kReader_Req_Reader_Private_Key)});
    logD("Got reader public key: %s", utils::bufToHexString(pubKey.data(), pubKey.size()).c_str());
    std::vector<uint8_t> x_coordinate = std::get<0>(get_public_points(pubKey));
    logD("Got X coordinate: %s", utils::bufToHexString(x_coordinate.data(), x_coordinate.size()).c_str());
    memcpy(readerData.reader_key_x, x_coordinate.data(), x_coordinate.size());
    memcpy(readerData.reader_public_key, pubKey.data(), pubKey.size());
    memcpy(readerData.reader_private_key, readerKey, tlv8.len(kReader_Req_Reader_Private_Key));
    memcpy(readerData.identifier, uniqueIdentifier, tlv8.len(kReader_Req_Identifier));
    std::vector<uint8_t> readeridentifier = utils::getHashIdentifier(readerData.reader_private_key, sizeof(readerData.reader_private_key), true);
    logI("Reader GroupIdentifier: %s", utils::bufToHexString(readeridentifier.data(), 8).c_str());
    memcpy(readerData.reader_identifier, readeridentifier.data(), 8);
    json serializedData = readerData;
    auto msgpack = json::to_msgpack(serializedData);
    esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
    esp_err_t commit_nvs = nvs_commit(savedData);
    logD("NVS SET STATUS: %s", esp_err_to_name(set_nvs));
    logD("NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
    tlv8.clear();
    if (set_nvs == 0 && commit_nvs == 0)
    {
      return 0;
    }
    else
      return 1;
  }

  boolean update(std::vector<char> *callback, int *callbackLen)
  {
    logI("PROVISIONED READER KEY: %s", utils::bufToHexString(readerData.reader_private_key, sizeof(readerData.reader_private_key)).c_str());
    logI("READER GROUP IDENTIFIER: %s", utils::bufToHexString(readerData.reader_identifier, sizeof(readerData.reader_identifier)).c_str());
    logI("READER UNIQUE IDENTIFIER: %s", utils::bufToHexString(readerData.identifier, sizeof(readerData.identifier)).c_str());

    char *dataConfState = configurationState->getNewString();
    char *dataNfcControlPoint = nfcControlPoint->getNewString();
    logD("New Value ConfState:");
    logD(dataConfState);
    logD("New Value NfcControlPoint:");
    logD(dataNfcControlPoint);
    logD("NfcControlPoint Length: %d \n", strlen(dataNfcControlPoint));
    std::vector<uint8_t> decB64 = utils::decodeB64(dataNfcControlPoint);
    if (decB64.size() == 0)
      return false;
    logD("Decoded data: %s", utils::bufToHexString(decB64.data(), decB64.size()).c_str());
    logD("Decoded data length: %d \n", decB64.size());
    std::vector<BERTLV> tlv = BERTLV::unpack_array(decB64);
    BERTLV operation = BERTLV::findTag(kTLVType1_Operation, tlv);
    logD("Request Operation: %d\n", operation.value.data()[0]);
    BERTLV RKR = BERTLV::findTag(kTLVType1_Reader_Key_Request, tlv);
    BERTLV DCR = BERTLV::findTag(kTLVType1_Device_Credential_Request, tlv);
    if (operation.value.data()[0] == 1)
    {
      if (RKR.tag.data.size() > 0)
      {
        logI("GET READER KEY REQUEST");
        if (strlen((const char *)readerData.reader_private_key) > 0)
        {
          size_t out_len = 0;
          TLV<Reader_Key_Response, 2> readerKeyResTlv;
          readerKeyResTlv.create(kReader_Res_Key_Identifier, 8, "KEY.IDENTIFIER");
          memcpy(readerKeyResTlv.buf(kReader_Res_Key_Identifier, 8), readerData.reader_identifier, 8);
          size_t lenSubTlv = readerKeyResTlv.pack(NULL);
          uint8_t subTlv[lenSubTlv];
          readerKeyResTlv.pack(subTlv);
          logD("SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          readerKeyResTlv.clear();
          readerKeyResTlv.create(kReader_Res_Reader_Key_Response, lenSubTlv, "READER.RESPONSE");
          memcpy(readerKeyResTlv.buf(kReader_Res_Reader_Key_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = readerKeyResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          readerKeyResTlv.pack(tlv);
          logD("TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          uint8_t resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, sizeof(resB64), &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          logD("B64 ENC STATUS: %d\n", ret);
          logI("RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
    }
    else if (operation.value.data()[0] == 2)
    {
      if (RKR.tag.data.size() > 0)
      {
        logI("SET READER KEY REQUEST\n");
        int ret = set_reader_key(RKR.value.data(), RKR.value.size());
        if (ret == 0)
        {
          logI("KEY SAVED TO NVS, COMPOSING RESPONSE");
          size_t out_len = 0;
          TLV<Reader_Key_Response, 2> readerKeyResTlv;
          readerKeyResTlv.create(kReader_Res_Status, 1, "STATUS");
          readerKeyResTlv.val(kReader_Res_Status, 0);
          size_t lenSubTlv = readerKeyResTlv.pack(NULL);
          uint8_t subTlv[lenSubTlv];
          readerKeyResTlv.pack(subTlv);
          logD("SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          readerKeyResTlv.clear();
          readerKeyResTlv.create(kReader_Res_Reader_Key_Response, lenSubTlv, "READER.RESPONSE");
          memcpy(readerKeyResTlv.buf(kReader_Res_Reader_Key_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = readerKeyResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          readerKeyResTlv.pack(tlv);
          logD("TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          unsigned char resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          logD("B64 ENC STATUS: %d\n", ret);
          logI("RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
      else if (DCR.tag.data.size() > 0)
      {
        logI("PROVISION DEVICE CREDENTIAL REQUEST\n");
        std::tuple<uint8_t *, int> state = provision_device_cred(DCR.value.data(), DCR.value.size());
        if (std::get<1>(state) != 99 && std::get<0>(state) != NULL)
        {
          size_t out_len = 0;
          TLV<Device_Credential_Response, 4> devCredResTlv;
          devCredResTlv.create(kDevice_Res_Key_Identifier, 8, "KEY.IDENTIFIER");
          devCredResTlv.create(kDevice_Res_Issuer_Key_Identifier, 8, "ISSUER.IDENTIFIER");
          devCredResTlv.create(kDevice_Res_Status, 1, "STATUS");
          memcpy(devCredResTlv.buf(kDevice_Res_Issuer_Key_Identifier, 8), std::get<0>(state), 8);
          devCredResTlv.val(kDevice_Res_Status, std::get<1>(state));
          size_t lenSubTlv = devCredResTlv.pack(NULL);
          uint8_t subTlv[lenSubTlv];
          devCredResTlv.pack(subTlv);
          logI("SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          devCredResTlv.clear();
          devCredResTlv.print(1);
          devCredResTlv.create(kDevice_Credential_Response, lenSubTlv, "DEV.RESPONSE");
          memcpy(devCredResTlv.buf(kDevice_Credential_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = devCredResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          devCredResTlv.pack(tlv);
          logI("TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          unsigned char resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          logD("B64 ENC STATUS: %d\n", ret);
          logI("RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
    }
    else if (operation.value.data()[0] == 3)
    {
      logI("REMOVE READER KEY REQUEST");
      std::fill(readerData.reader_identifier, readerData.reader_identifier + 8, 0);
      std::fill(readerData.reader_private_key, readerData.reader_private_key + 32, 0);
      json serializedData = readerData;
      auto msgpack = json::to_msgpack(serializedData);
      esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
      esp_err_t commit_nvs = nvs_commit(savedData);
      logD("NVS SET: %s\n", esp_err_to_name(set_nvs));
      logD("NVS COMMIT: %s\n", esp_err_to_name(commit_nvs));
      const char *res = "BwMCAQA=";
      size_t resLen = 9;
      logI("RESPONSE LENGTH: %d, DATA: %s", resLen, res);
      callback->insert(callback->end(), res, res + resLen);
    }
    return (true);
  }

}; // end NFCAccess

//////////////////////////////////////

void deleteReaderData(const char *buf)
{
  uint8_t empty[64];
  readerData.issuers.clear();
  std::fill(readerData.identifier, readerData.identifier + 8, 0);
  std::fill(readerData.reader_identifier, readerData.reader_identifier + 8, 0);
  std::fill(readerData.reader_private_key, readerData.reader_private_key + 32, 0);
  esp_err_t erase_nvs = nvs_erase_key(savedData, "READERDATA");
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG1("*** NVS W STATUS: \n");
  LOG1("ERASE: %s\n", esp_err_to_name(erase_nvs));
  LOG1("COMMIT: %s\n", esp_err_to_name(commit_nvs));
  LOG1("*** NVS W STATUS: \n");
}

void pairCallback(bool isPaired)
{
  if (!isPaired && HAPClient::nAdminControllers() == 0)
  {
    deleteReaderData(NULL);
  }
}

void setFlow(const char *buf)
{
  switch (buf[1])
  {
  case '0':
    defaultToStd = false;
    Serial.println("FAST Flow");
    break;

  case '1':
    defaultToStd = true;
    Serial.println("STANDARD Flow");
    break;

  default:
    Serial.println("0 = FAST flow, 1 = STANDARD Flow");
    break;
  }
}

void setLogLevel(const char *buf)
{
  LogLevel level = LogLevel::Info;
  if (strncmp(buf + 1, "E", 1) == 0)
  {
    level = LogLevel::Error;
    Serial.println("ERROR");
  }
  else if (strncmp(buf + 1, "W", 1) == 0)
  {
    level = LogLevel::Warning;
    Serial.println("WARNING");
  }
  else if (strncmp(buf + 1, "I", 1) == 0)
  {
    level = LogLevel::Info;
    Serial.println("INFO");
  }
  else if (strncmp(buf + 1, "D", 1) == 0)
  {
    level = LogLevel::Debug;
    Serial.println("DEBUG");
  }
  else if (strncmp(buf + 1, "V", 1) == 0)
  {
    level = LogLevel::Verbose;
    Serial.println("VERBOSE");
  }
  else if (strncmp(buf + 1, "N", 1) == 0)
  {
    level = LogLevel::None;
    Serial.println("NONE");
  }

  Logging::setLevel(level);
}

void setup()
{
  Logging::addAppender(&UARTAppender::instance());
  Logging::setLevel(LogLevel::Info);
  Serial.begin(115200);
  SimpleLoggable *setupLog = new SimpleLoggable("SETUP");
  size_t len;
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData);
  if (!nvs_get_blob(savedData, "READERDATA", NULL, &len))
  {
    uint8_t msgpack[len];
    nvs_get_blob(savedData, "READERDATA", msgpack, &len);
    setupLog->logger().logf(LogLevel::Info, "READERDATA - MSGPACK(%d): %s", len, utils::bufToHexString(msgpack, len).c_str());
    json data = json::from_msgpack(msgpack, msgpack + len);
    setupLog->logger().logf(LogLevel::Info, "READERDATA - JSON(%d): %s", len, data.dump(-1).c_str());
    homeKeyReader::readerData_t p = data.template get<homeKeyReader::readerData_t>();
    readerData = p;
  }
  // homeSpan.setStatusPin(2);
  // homeSpan.setStatusAutoOff(5);
  homeSpan.setLogLevel(0);

  setupLog->logger().logf(LogLevel::Info, "READER GROUP ID (%d): %s", strlen((const char *)readerData.reader_identifier), utils::bufToHexString(readerData.reader_identifier, sizeof(readerData.reader_identifier)).c_str());
  setupLog->logger().logf(LogLevel::Info, "READER UNIQUE ID (%d): %s", strlen((const char *)readerData.identifier), utils::bufToHexString(readerData.identifier, sizeof(readerData.identifier)).c_str());

  setupLog->logger().logf(LogLevel::Info, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto &issuer : readerData.issuers)
  {
    setupLog->logger().logf(LogLevel::Info, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuerId, sizeof(issuer.issuerId)).c_str(), utils::bufToHexString(issuer.publicKey, sizeof(issuer.publicKey)).c_str());
  }
  homeSpan.begin(Category::Locks, "Test NFC Lock");

  new SpanUserCommand('D', "Delete NFC Reader Data", deleteReaderData);
  new SpanUserCommand('L', "Set Log Level", setLogLevel);
  new SpanUserCommand('F', "Set HomeKey Flow", setFlow);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  SimpleLoggable *nfcLog = new SimpleLoggable("NFC_SETUP");
  if (!versiondata)
  {
    nfcLog->logger().logf(LogLevel::Error, "NFC_SETUP", "Didn't find PN53x board");
  }
  else
  {
    nfcLog->logger().logf(LogLevel::Info, "Found chip PN5%x", (versiondata >> 24) & 0xFF);
    nfcLog->logger().logf(LogLevel::Info, "Firmware ver. %d.%d", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(0);
    nfcLog->logger().logf(LogLevel::Info, "Waiting for an ISO14443A card");
  }

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
  homeSpan.setPairCallback(pairCallback);
}

//////////////////////////////////////

void loop()
{
  homeSpan.poll();
}