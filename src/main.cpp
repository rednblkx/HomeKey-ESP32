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
#include <sstream>
#include <PicoMQTT.h>

#if __has_include("mqtt.h")
#include <mqtt.h>
#else
#define MQTT_HOST "0.0.0.0"
#define MQTT_PORT 1883
#define MQTT_CLIENTID "homekey_mqtt"
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"
#define MQTT_AUTH_TOPIC "topic/auth"
#define MQTT_SET_STATE_TOPIC "topic/set_state"
#define MQTT_SET_TARGET_STATE_TOPIC "topic/set_target_state"
#define MQTT_SET_CURRENT_STATE_TOPIC "topic/set_current_state"
#define MQTT_STATE_TOPIC "topic/state"
#endif

using namespace nlohmann;

int currentState = 0;

PicoMQTT::Client mqtt;

#define PN532_SS (5)
PN532_SPI pn532spi(SPI, PN532_SS);
PN532 nfc(pn532spi);

nvs_handle savedData;
homeKeyReader::readerData_t readerData;
bool defaultToStd = false;

bool save_to_nvs()
{
  json serializedData = readerData;
  auto msgpack = json::to_msgpack(serializedData);
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
  esp_err_t commit_nvs = nvs_commit(savedData);
  ESP_LOGV("save_to_nvs", "NVS SET STATUS: %s", esp_err_to_name(set_nvs));
  ESP_LOGV("save_to_nvs", "NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
  return !set_nvs && !commit_nvs;
}

struct LockManagement : Service::LockManagement
{
  SpanCharacteristic *lockControlPoint;
  SpanCharacteristic *version;

  LockManagement() : Service::LockManagement()
  {

    Serial.print("Configuring LockManagement\n"); // initialization message
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

struct LockMechanism : Service::LockMechanism
{
  SpanCharacteristic *lockCurrentState;
  SpanCharacteristic *lockTargetState;
  const char *TAG = "LockMechanism";

  LockMechanism() : Service::LockMechanism()
  {
    ESP_LOGI(TAG, "Configuring LockMechanism"); // initialization message
    new Characteristic::Name("NFC Lock");
    lockCurrentState = new Characteristic::LockCurrentState(1, true);
    lockTargetState = new Characteristic::LockTargetState(1, true);
    mqtt.subscribe(
        MQTT_SET_STATE_TOPIC, [this](const char *payload)
        {
        ESP_LOGD(TAG, "Received message in topic set_state: %s", payload);
        int state = atoi(payload);
        lockTargetState->setVal(state == 0 || state == 1 ? state : lockTargetState->getVal());
        lockCurrentState->setVal(state == 0 || state == 1 ? state : lockCurrentState->getVal()); },
        false);
    mqtt.subscribe(
        MQTT_SET_TARGET_STATE_TOPIC, [this](const char *payload)
        {
        ESP_LOGD(TAG, "Received message in topic set_target_state: %s", payload);
        int state = atoi(payload);
        lockTargetState->setVal(state == 0 || state == 1 ? state : lockTargetState->getVal()); },
        false);
    mqtt.subscribe(
        MQTT_SET_CURRENT_STATE_TOPIC, [this](const char *payload)
        {
        ESP_LOGD(TAG, "Received message in topic set_current_state: %s", payload);
        int state = atoi(payload);
        lockCurrentState->setVal(state == 0 || state == 1 ? state : lockCurrentState->getVal()); },
        false);
  } // end constructor

  boolean update(std::vector<char> *callback, int *callbackLen)
  {
    int targetState = lockTargetState->getNewVal();
    ESP_LOGI(TAG, "New LockState=%d, Current LockState=%d", targetState, lockCurrentState->getVal());

    // lockCurrentState->setVal(targetState);
    mqtt.publish(MQTT_STATE_TOPIC, std::to_string(targetState).c_str());

    return (true);
  }

  void loop()
  {
    uint8_t uid[16];
    uint8_t uidLen = 0;
    uint16_t atqa[1];
    uint8_t sak[1];
    bool passiveTarget = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, atqa, sak, 1000, true);
    if (passiveTarget)
    {
      ESP_LOGD(TAG, "ATQA: %s", utils::bufToHexString(atqa, 1).c_str());
      ESP_LOGD(TAG, "SAK: %s", utils::bufToHexString(sak, 1).c_str());
      ESP_LOGD(TAG, "UID: %s", utils::bufToHexString(uid, uidLen).c_str());
      ESP_LOGI(TAG, "*** PASSIVE TARGET DETECTED ***");
      unsigned long startTime = millis();
      uint8_t data[13] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x0};
      uint8_t selectCmdRes[32];
      uint8_t selectCmdResLength = 32;
      ESP_LOGD(TAG, "SELECT HomeKey Applet, APDU: %s", utils::bufToHexString(data, sizeof(data)).c_str());
      bool exchange = nfc.inDataExchange(data, sizeof(data), selectCmdRes, &selectCmdResLength);
      ESP_LOGD(TAG, "SELECT HomeKey Applet, Response: %s, Length: %d", utils::bufToHexString(selectCmdRes, selectCmdResLength).c_str(), selectCmdResLength);
      if (exchange)
      {
        if (selectCmdRes[selectCmdResLength - 2] == 0x90 && selectCmdRes[selectCmdResLength - 1] == 0x00)
        {
          ESP_LOGI(TAG, "*** SELECT HOMEKEY APPLET SUCCESSFUL ***");
          ESP_LOGD(TAG, "Reader Private Key: %s", utils::bufToHexString((const uint8_t *)readerData.reader_private_key, sizeof(readerData.reader_private_key)).c_str());
          HKAuthenticationContext authCtx([](uint8_t *apdu, size_t apduLen, uint8_t *res, uint8_t *resLen)
                                          {  return nfc.inDataExchange(apdu, apduLen, res, resLen); },
                                          readerData);
          auto authResult = authCtx.authenticate(defaultToStd, savedData);
          if (std::get<2>(authResult) != homeKeyReader::kFlowFailed)
          {
            int newTargetState = lockTargetState->getNewVal();
            int targetState = lockTargetState->getVal();
            mqtt.publish(MQTT_STATE_TOPIC, std::to_string(newTargetState == targetState ? !lockCurrentState->getVal() : newTargetState).c_str());
            json payload;
            payload["issuerId"] = utils::bufToHexString(std::get<0>(authResult), 8, true);
            payload["endpointId"] = utils::bufToHexString(std::get<1>(authResult), 6, true);
            payload["homekey"] = true;
            mqtt.publish(MQTT_AUTH_TOPIC, payload.dump().c_str());
            unsigned long stopTime = millis();
            ESP_LOGI(TAG, "Total time: %lu ms", stopTime - startTime);
          }
        }
        else
        {
          json payload;
          payload["atqa"] = utils::bufToHexString(atqa, 1, true);
          payload["sak"] = utils::bufToHexString(sak, 1, true);
          payload["uid"] = utils::bufToHexString(uid, uidLen, true);
          payload["homekey"] = false;
          mqtt.publish(MQTT_AUTH_TOPIC, payload.dump().c_str());
        }
      }
      bool deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
      while (deviceStillInField)
      {
        nfc.inRelease();
        deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
        vTaskDelay(100 / portTICK_PERIOD_MS);
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
      nfc.inCommunicateThru(data, sizeof(data), response, &length, 100);
    }
  }
};

struct NFCAccess : Service::NFCAccess
{
  SpanCharacteristic *configurationState;
  SpanCharacteristic *nfcControlPoint;
  SpanCharacteristic *nfcSupportedConfiguration;
  const char *TAG = "NFCAccess";

  NFCAccess() : Service::NFCAccess()
  {
    ESP_LOGI(TAG, "Configuring NFCAccess"); // initialization message
    new Characteristic::Name("NFC Access");
    configurationState = new Characteristic::ConfigurationState();
    nfcControlPoint = new Characteristic::NFCAccessControlPoint();
    nfcSupportedConfiguration = new Characteristic::NFCAccessSupportedConfiguration();
  }
  std::vector<uint8_t> get_x(std::vector<uint8_t> pubKey)
  {
    mbedtls_ecp_group grp;
    mbedtls_ecp_point point;
    mbedtls_ecp_point_init(&point);
    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    int ret = mbedtls_ecp_point_read_binary(&grp, &point, pubKey.data(), pubKey.size());
    ESP_LOGV(TAG, "mbedtls_ecp_point_read_binary status: %d", ret);
    size_t buffer_size_x = mbedtls_mpi_size(&point.X);
    std::vector<uint8_t> X;
    X.resize(buffer_size_x);
    X.reserve(buffer_size_x);
    mbedtls_mpi_write_binary(&point.X, X.data(), buffer_size_x);
    ESP_LOGV(TAG, "PublicKey: %s, X Coordinate: %s", utils::bufToHexString(pubKey.data(), pubKey.size()).c_str(), utils::bufToHexString(X.data(), X.size()).c_str());
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&point);
    return X;
  }
  std::tuple<uint8_t *, int> provision_device_cred(uint8_t *buf, size_t len)
  {
    for (size_t i = 0; i < 16; i++)
    {
      if (HAPClient::controllers[i].allocated)
      {
        std::vector<uint8_t> id = utils::getHashIdentifier(HAPClient::controllers[i].LTPK, 32, true);
        ESP_LOGD(TAG, "Found allocated controller - ID: %s", utils::bufToHexString(id.data(), 8).c_str());
        homeKeyIssuer::issuer_t *foundIssuer = nullptr;
        for (auto &issuer : readerData.issuers)
        {
          if (!memcmp(issuer.issuerId, id.data(), 8))
          {
            ESP_LOGD(TAG, "Issuer %s already added, skipping", utils::bufToHexString(issuer.issuerId, 8).c_str());
            foundIssuer = &issuer;
            break;
          }
        }
        if (foundIssuer == nullptr)
        {
          ESP_LOGD(TAG, "Adding new issuer - ID: %s", utils::bufToHexString(id.data(), 8).c_str());
          homeKeyIssuer::issuer_t issuer;
          memcpy(issuer.issuerId, id.data(), 8);
          memcpy(issuer.publicKey, HAPClient::controllers[i].LTPK, 32);
          readerData.issuers.emplace_back(issuer);
        }
      }
    }
    TLV<Device_Credential_Request, 5> tlv8;
    ESP_LOGD(TAG, "DCR Buffer length: %d, data: %s", len, utils::bufToHexString(buf, len).c_str());
    tlv8.create(kDevice_Req_Key_Type, 1, "KEY.TYPE");
    tlv8.create(kDevice_Req_Public_Key, 65, "PUBLIC.KEY");
    tlv8.create(kDevice_Req_Issuer_Key_Identifier, 8, "ISSUER.IDENTIFIER");
    tlv8.create(kDevice_Req_Key_State, 1, "KEY.STATE");
    tlv8.create(kDevice_Req_Key_Identifier, 8, "KEY.IDENTIFIER");

    ESP_LOGV(TAG, "DCR TLV DECODE STATE: %d", tlv8.unpack(buf, len));
    tlv8.print(1);
    homeKeyIssuer::issuer_t *foundIssuer = nullptr;
    for (auto &issuer : readerData.issuers)
    {
      if (!memcmp(issuer.issuerId, tlv8.buf(kDevice_Req_Issuer_Key_Identifier), 8))
      {
        ESP_LOGD(TAG, "Found issuer - ID: %s", utils::bufToHexString(issuer.issuerId, 8).c_str());
        foundIssuer = &issuer;
      }
    }
    if (foundIssuer != nullptr)
    {
      homeKeyEndpoint::endpoint_t *foundEndpoint = 0;
      uint8_t endEphPubKey[tlv8.len(kDevice_Req_Public_Key) + 1] = {0x04};
      memcpy(endEphPubKey + 1, tlv8.buf(kDevice_Req_Public_Key), tlv8.len(kDevice_Req_Public_Key));
      std::vector<uint8_t> endpointId = utils::getHashIdentifier(endEphPubKey, sizeof(endEphPubKey), false);
      for (auto &endpoint : foundIssuer->endpoints)
      {
        if (!memcmp(endpoint.endpointId, endpointId.data(), 6))
        {
          ESP_LOGD(TAG, "Found endpoint - ID: %s", utils::bufToHexString(endpoint.endpointId, 6).c_str());
          foundEndpoint = &endpoint;
        }
      }
      if (foundEndpoint == 0)
      {
        ESP_LOGD(TAG, "Adding new endpoint - ID: %s , PublicKey: %s", utils::bufToHexString(endpointId.data(), 6).c_str(), utils::bufToHexString(endEphPubKey, sizeof(endEphPubKey)).c_str());
        homeKeyEndpoint::endpoint_t endpoint;
        endpointEnrollment::enrollment_t hap;
        hap.unixTime = std::time(nullptr);
        uint8_t encoded[128];
        size_t olen = 0;
        mbedtls_base64_encode(encoded, 128, &olen, buf, len);
        hap.payload.insert(hap.payload.begin(), encoded, encoded + olen);
        std::vector<uint8_t> x_coordinate = get_x(std::vector<uint8_t>(endEphPubKey, endEphPubKey + sizeof(endEphPubKey)));

        endpoint.counter = 0;
        endpoint.key_type = tlv8.buf(kDevice_Req_Key_Type)[0];
        endpoint.last_used_at = 0;
        endpoint.enrollments.hap = hap;
        std::fill(endpoint.persistent_key, endpoint.persistent_key + 32, 0);
        memcpy(endpoint.endpointId, endpointId.data(), 6);
        memcpy(endpoint.publicKey, endEphPubKey, sizeof(endEphPubKey));
        memcpy(endpoint.endpoint_key_x, x_coordinate.data(), x_coordinate.size());
        foundIssuer->endpoints.emplace_back(endpoint);
        save_to_nvs();
        return std::make_tuple(foundIssuer->issuerId, homeKeyReader::SUCCESS);
      }
      else
      {
        ESP_LOGD(TAG, "Endpoint already exists - ID: %s", utils::bufToHexString(foundEndpoint->endpointId, 6).c_str());
        save_to_nvs();
        return std::make_tuple(foundEndpoint->endpointId, homeKeyReader::DUPLICATE);
      }
      tlv8.clear();
    }
    else
    {
      ESP_LOGD(TAG, "Issuer does not exist - ID: %s", utils::bufToHexString(tlv8.buf(kDevice_Req_Issuer_Key_Identifier), 8).c_str());
      save_to_nvs();
      return std::make_tuple(tlv8.buf(kDevice_Req_Issuer_Key_Identifier), homeKeyReader::DOES_NOT_EXIST);
    }
    return std::make_tuple(readerData.reader_identifier, homeKeyReader::DOES_NOT_EXIST);
  }

  std::vector<uint8_t> getPublicKey(uint8_t *privKey, size_t len)
  {
    mbedtls_ecp_keypair keypair;
    mbedtls_ecp_keypair_init(&keypair);
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);

    int ecp_key = mbedtls_ecp_read_key(MBEDTLS_ECP_DP_SECP256R1, &keypair, privKey, len);
    int ret = mbedtls_ecp_mul(&keypair.grp, &keypair.Q, &keypair.d, &keypair.grp.G, mbedtls_ctr_drbg_random, &ctr_drbg);

    size_t olenPub = 0;
    std::vector<uint8_t> readerPublicKey(MBEDTLS_ECP_MAX_BYTES);
    mbedtls_ecp_point_write_binary(&keypair.grp, &keypair.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olenPub, readerPublicKey.data(), readerPublicKey.capacity());
    readerPublicKey.resize(olenPub);

    // Cleanup
    mbedtls_ecp_keypair_free(&keypair);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    return readerPublicKey;
  }

  int set_reader_key(uint8_t *buf, size_t len)
  {
    ESP_LOGD(TAG, "Setting reader key: %s", utils::bufToHexString(buf, len).c_str());
    TLV<Reader_Key_Request, 3> tlv8;
    tlv8.create(kReader_Req_Key_Type, 1, "KEY.TYPE");
    tlv8.create(kReader_Req_Reader_Private_Key, 32, "READER.PRIV.KEY");
    tlv8.create(kReader_Req_Identifier, 8, "IDENTIFIER");
    // tlv8.create(kRequest_Reader_Key_Request, 64, "READER.REQ");
    // tlv8.create(kReader_Req_Key_Identifier, 64, "KEY.IDENTIFIER");

    ESP_LOGV(TAG, "RKR TLV DECODE STATE: %d", tlv8.unpack(buf, len));
    tlv8.print(1);
    uint8_t *readerKey = tlv8.buf(kReader_Req_Reader_Private_Key);
    uint8_t *uniqueIdentifier = tlv8.buf(kReader_Req_Identifier);
    ESP_LOGD(TAG, "Reader Key: %s", utils::bufToHexString(readerKey, tlv8.len(kReader_Req_Reader_Private_Key)).c_str());
    ESP_LOGD(TAG, "UniqueIdentifier: %s", utils::bufToHexString(uniqueIdentifier, tlv8.len(kReader_Req_Identifier)).c_str());
    std::vector<uint8_t> pubKey = getPublicKey(readerKey, tlv8.len(kReader_Req_Reader_Private_Key));
    ESP_LOGD(TAG, "Got reader public key: %s", utils::bufToHexString(pubKey.data(), pubKey.size()).c_str());
    std::vector<uint8_t> x_coordinate = get_x(pubKey);
    ESP_LOGD(TAG, "Got X coordinate: %s", utils::bufToHexString(x_coordinate.data(), x_coordinate.size()).c_str());
    memcpy(readerData.reader_key_x, x_coordinate.data(), x_coordinate.size());
    memcpy(readerData.reader_public_key, pubKey.data(), pubKey.size());
    memcpy(readerData.reader_private_key, readerKey, tlv8.len(kReader_Req_Reader_Private_Key));
    memcpy(readerData.identifier, uniqueIdentifier, tlv8.len(kReader_Req_Identifier));
    std::vector<uint8_t> readeridentifier = utils::getHashIdentifier(readerData.reader_private_key, sizeof(readerData.reader_private_key), true);
    ESP_LOGD(TAG, "Reader GroupIdentifier: %s", utils::bufToHexString(readeridentifier.data(), 8).c_str());
    memcpy(readerData.reader_identifier, readeridentifier.data(), 8);
    bool nvs = save_to_nvs();
    tlv8.clear();
    if (nvs)
    {
      return 0;
    }
    else
      return 1;
  }

  boolean update(std::vector<char> *callback, int *callbackLen)
  {
    ESP_LOGD(TAG, "PROVISIONED READER KEY: %s", utils::bufToHexString(readerData.reader_private_key, sizeof(readerData.reader_private_key)).c_str());
    ESP_LOGD(TAG, "READER GROUP IDENTIFIER: %s", utils::bufToHexString(readerData.reader_identifier, sizeof(readerData.reader_identifier)).c_str());
    ESP_LOGD(TAG, "READER UNIQUE IDENTIFIER: %s", utils::bufToHexString(readerData.identifier, sizeof(readerData.identifier)).c_str());

    char *dataConfState = configurationState->getNewString();
    char *dataNfcControlPoint = nfcControlPoint->getNewString();
    ESP_LOGD(TAG, "NfcControlPoint Length: %d", strlen(dataNfcControlPoint));
    std::vector<uint8_t> decB64 = utils::decodeB64(dataNfcControlPoint);
    if (decB64.size() == 0)
      return false;
    ESP_LOGD(TAG, "Decoded data: %s", utils::bufToHexString(decB64.data(), decB64.size()).c_str());
    ESP_LOGD(TAG, "Decoded data length: %d", decB64.size());
    std::vector<BERTLV> tlv = BERTLV::unpack_array(decB64);
    BERTLV operation = BERTLV::findTag(kTLVType1_Operation, tlv);
    ESP_LOGD(TAG, "Request Operation: %d", operation.value.data()[0]);
    BERTLV RKR = BERTLV::findTag(kTLVType1_Reader_Key_Request, tlv);
    BERTLV DCR = BERTLV::findTag(kTLVType1_Device_Credential_Request, tlv);
    if (operation.value.data()[0] == 1)
    {
      if (RKR.tag.size() > 0)
      {
        ESP_LOGI(TAG, "GET READER KEY REQUEST");
        if (strlen((const char *)readerData.reader_private_key) > 0)
        {
          size_t out_len = 0;
          TLV<Reader_Key_Response, 2> readerKeyResTlv;
          readerKeyResTlv.create(kReader_Res_Key_Identifier, 8, "KEY.IDENTIFIER");
          memcpy(readerKeyResTlv.buf(kReader_Res_Key_Identifier, 8), readerData.reader_identifier, 8);
          size_t lenSubTlv = readerKeyResTlv.pack(NULL);
          uint8_t subTlv[lenSubTlv];
          readerKeyResTlv.pack(subTlv);
          ESP_LOGD(TAG, "SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          readerKeyResTlv.clear();
          readerKeyResTlv.create(kReader_Res_Reader_Key_Response, lenSubTlv, "READER.RESPONSE");
          memcpy(readerKeyResTlv.buf(kReader_Res_Reader_Key_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = readerKeyResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          readerKeyResTlv.pack(tlv);
          ESP_LOGD(TAG, "TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          uint8_t resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, sizeof(resB64), &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          ESP_LOGD(TAG, "B64 ENC STATUS: %d", ret);
          ESP_LOGI(TAG, "RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
    }
    else if (operation.value.data()[0] == 2)
    {
      if (RKR.tag.size() > 0)
      {
        ESP_LOGI(TAG, "SET READER KEY REQUEST");
        int ret = set_reader_key(RKR.value.data(), RKR.value.size());
        if (ret == 0)
        {
          ESP_LOGI(TAG, "KEY SAVED TO NVS, COMPOSING RESPONSE");
          size_t out_len = 0;
          TLV<Reader_Key_Response, 2> readerKeyResTlv;
          readerKeyResTlv.create(kReader_Res_Status, 1, "STATUS");
          readerKeyResTlv.val(kReader_Res_Status, 0);
          size_t lenSubTlv = readerKeyResTlv.pack(NULL);
          uint8_t subTlv[lenSubTlv];
          readerKeyResTlv.pack(subTlv);
          ESP_LOGD(TAG, "SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          readerKeyResTlv.clear();
          readerKeyResTlv.create(kReader_Res_Reader_Key_Response, lenSubTlv, "READER.RESPONSE");
          memcpy(readerKeyResTlv.buf(kReader_Res_Reader_Key_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = readerKeyResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          readerKeyResTlv.pack(tlv);
          ESP_LOGD(TAG, "TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          unsigned char resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          ESP_LOGD(TAG, "B64 ENC STATUS: %d", ret);
          ESP_LOGI(TAG, "RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
      else if (DCR.tag.size() > 0)
      {
        ESP_LOGI(TAG, "PROVISION DEVICE CREDENTIAL REQUEST");
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
          ESP_LOGD(TAG, "SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          devCredResTlv.clear();
          devCredResTlv.print(1);
          devCredResTlv.create(kDevice_Credential_Response, lenSubTlv, "DEV.RESPONSE");
          memcpy(devCredResTlv.buf(kDevice_Credential_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = devCredResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          devCredResTlv.pack(tlv);
          ESP_LOGD(TAG, "TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          unsigned char resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          ESP_LOGD(TAG, "B64 ENC STATUS: %d", ret);
          ESP_LOGI(TAG, "RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
    }
    else if (operation.value.data()[0] == 3)
    {
      ESP_LOGI(TAG, "REMOVE READER KEY REQUEST");
      std::fill(readerData.reader_identifier, readerData.reader_identifier + 8, 0);
      std::fill(readerData.reader_private_key, readerData.reader_private_key + 32, 0);
      json serializedData = readerData;
      auto msgpack = json::to_msgpack(serializedData);
      esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
      esp_err_t commit_nvs = nvs_commit(savedData);
      ESP_LOGD(TAG, "NVS SET: %s", esp_err_to_name(set_nvs));
      ESP_LOGD(TAG, "NVS COMMIT: %s", esp_err_to_name(commit_nvs));
      const char *res = "BwMCAQA=";
      size_t resLen = 9;
      ESP_LOGI(TAG, "RESPONSE LENGTH: %d, DATA: %s", resLen, res);
      callback->insert(callback->end(), res, res + resLen);
    }
    return true;
  }

}; // end NFCAccess

//////////////////////////////////////

void deleteReaderData(const char *buf)
{
  readerData.issuers.clear();
  std::fill(readerData.identifier, readerData.identifier + 8, 0);
  std::fill(readerData.reader_identifier, readerData.reader_identifier + 8, 0);
  std::fill(readerData.reader_private_key, readerData.reader_private_key + 32, 0);
  esp_err_t erase_nvs = nvs_erase_key(savedData, "READERDATA");
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG1("*** NVS W STATUS");
  LOG1("ERASE: %s", esp_err_to_name(erase_nvs));
  LOG1("COMMIT: %s", esp_err_to_name(commit_nvs));
  LOG1("*** NVS W STATUS");
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
  esp_log_level_t level = esp_log_level_get("*");
  if (strncmp(buf + 1, "E", 1) == 0)
  {
    level = ESP_LOG_ERROR;
    Serial.println("ERROR");
  }
  else if (strncmp(buf + 1, "W", 1) == 0)
  {
    level = ESP_LOG_WARN;
    Serial.println("WARNING");
  }
  else if (strncmp(buf + 1, "I", 1) == 0)
  {
    level = ESP_LOG_INFO;
    Serial.println("INFO");
  }
  else if (strncmp(buf + 1, "D", 1) == 0)
  {
    level = ESP_LOG_DEBUG;
    Serial.println("DEBUG");
  }
  else if (strncmp(buf + 1, "V", 1) == 0)
  {
    level = ESP_LOG_VERBOSE;
    Serial.println("VERBOSE");
  }
  else if (strncmp(buf + 1, "N", 1) == 0)
  {
    level = ESP_LOG_NONE;
    Serial.println("NONE");
  }

  esp_log_level_set("*", level);
}

void insertDummyIssuers(const char *buf)
{
  mbedtls_entropy_context entropy;
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_context drbg;
  mbedtls_ctr_drbg_init(&drbg);
  mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &entropy, NULL, 0);
  std::stringstream strVal;
  strVal << buf[1];
  unsigned int iterations;
  strVal >> iterations;
  if (iterations > 64)
  {
    Serial.print("\nInvalid Argument\n");
    return;
  }
  for (size_t i = 0; i < iterations; i++)
  {
    mbedtls_ecp_keypair ephemeral;
    mbedtls_ecp_keypair_init(&ephemeral);
    mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, &ephemeral, mbedtls_ctr_drbg_random, &drbg);
    std::vector<uint8_t> bufPub;
    bufPub.resize(MBEDTLS_ECP_MAX_BYTES);
    bufPub.reserve(MBEDTLS_ECP_MAX_BYTES);
    size_t olen = 0;
    mbedtls_ecp_point_write_binary(&ephemeral.grp, &ephemeral.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, bufPub.data(), bufPub.capacity());
    bufPub.resize(olen);
    mbedtls_ecp_keypair_free(&ephemeral);

    size_t buffer_size_x = mbedtls_mpi_size(&ephemeral.Q.X);
    std::vector<uint8_t> X;
    X.resize(buffer_size_x);
    X.reserve(buffer_size_x);
    mbedtls_mpi_write_binary(&ephemeral.Q.X, X.data(), buffer_size_x);

    homeKeyIssuer::issuer_t issuer;
    memcpy(issuer.issuerId, utils::getHashIdentifier(bufPub.data(), 32, true).data(), 8);
    memcpy(issuer.issuer_key_x, X.data(), X.size());
    memcpy(issuer.publicKey, bufPub.data(), bufPub.size());
    homeKeyEndpoint::endpoint_t endpoint;
    endpoint.counter = 0;
    memcpy(endpoint.endpoint_key_x, X.data(), X.size());
    memcpy(endpoint.endpointId, utils::getHashIdentifier(bufPub.data(), 32, false).data(), 6);
    endpoint.key_type = 0;
    endpoint.last_used_at = 0;
    endpoint.enrollments.attestation.payload.resize(64);
    endpoint.enrollments.attestation.unixTime = 0;
    endpoint.enrollments.hap.payload.resize(64);
    endpoint.enrollments.hap.unixTime = 0;
    esp_fill_random(endpoint.persistent_key, 32);
    memcpy(endpoint.publicKey, bufPub.data(), bufPub.size());
    issuer.endpoints.emplace_back(endpoint);
    issuer.endpoints.emplace_back(endpoint);
    issuer.endpoints.emplace_back(endpoint);
    issuer.endpoints.emplace_back(endpoint);

    readerData.issuers.emplace_front(issuer);
  }
  mbedtls_entropy_free(&entropy);
  mbedtls_ctr_drbg_free(&drbg);
}

void print_issuers(const char *buf)
{
  const char *TAG = "print_issuers";
  ESP_LOGI(TAG, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto &issuer : readerData.issuers)
  {
    ESP_LOGD(TAG, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuerId, sizeof(issuer.issuerId)).c_str(), utils::bufToHexString(issuer.publicKey, sizeof(issuer.publicKey)).c_str());
    for (auto &endpoint : issuer.endpoints)
    {
      ESP_LOGD(TAG, "Endpoint ID: %s, Public Key: %s", utils::bufToHexString(endpoint.endpointId, sizeof(endpoint.endpointId)).c_str(), utils::bufToHexString(endpoint.publicKey, sizeof(endpoint.publicKey)).c_str());
    }
  }
}

void wifiCallback()
{
  mqtt.host = MQTT_HOST;
  mqtt.port = MQTT_PORT;
  mqtt.client_id = MQTT_CLIENTID;
  mqtt.username = MQTT_USERNAME;
  mqtt.password = MQTT_PASSWORD;
  mqtt.begin();
}

void setup()
{
  Serial.begin(115200);
  size_t len;
  const char *TAG = "SETUP";
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData);
  if (!nvs_get_blob(savedData, "READERDATA", NULL, &len))
  {
    uint8_t msgpack[len];
    nvs_get_blob(savedData, "READERDATA", msgpack, &len);
    ESP_LOGV(TAG, "READERDATA - MSGPACK(%d): %s", len, utils::bufToHexString(msgpack, len).c_str());
    json data = json::from_msgpack(msgpack, msgpack + len);
    ESP_LOGD(TAG, "READERDATA - JSON(%d): %s", len, data.dump(-1).c_str());
    homeKeyReader::readerData_t p = data.template get<homeKeyReader::readerData_t>();
    readerData = p;
  }
  // homeSpan.setStatusPin(2);
  // homeSpan.setStatusAutoOff(5);
  homeSpan.reserveSocketConnections(2);
  homeSpan.setLogLevel(0);

  ESP_LOGD(TAG, "READER GROUP ID (%d): %s", strlen((const char *)readerData.reader_identifier), utils::bufToHexString(readerData.reader_identifier, sizeof(readerData.reader_identifier)).c_str());
  ESP_LOGD(TAG, "READER UNIQUE ID (%d): %s", strlen((const char *)readerData.identifier), utils::bufToHexString(readerData.identifier, sizeof(readerData.identifier)).c_str());

  ESP_LOGI(TAG, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto &issuer : readerData.issuers)
  {
    ESP_LOGD(TAG, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuerId, sizeof(issuer.issuerId)).c_str(), utils::bufToHexString(issuer.publicKey, sizeof(issuer.publicKey)).c_str());
  }
  homeSpan.enableOTA();
  homeSpan.begin(Category::Locks, "Test NFC Lock");

  new SpanUserCommand('D', "Delete Home Key Data", deleteReaderData);
  new SpanUserCommand('L', "Set Log Level", setLogLevel);
  new SpanUserCommand('F', "Set HomeKey Flow", setFlow);
  new SpanUserCommand('I', "Add dummy Issuers and endpoints", insertDummyIssuers);
  new SpanUserCommand('P', "Print Issuers", print_issuers);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    ESP_LOGE("NFC_SETUP", "Didn't find PN53x board");
  }
  else
  {
    ESP_LOGI("NFC_SETUP", "Found chip PN5%x", (versiondata >> 24) & 0xFF);
    ESP_LOGI("NFC_SETUP", "Firmware ver. %d.%d", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(0);
    ESP_LOGI("NFC_SETUP", "Waiting for an ISO14443A card");
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
  homeSpan.setWifiCallback(wifiCallback);
}

//////////////////////////////////////

void loop()
{
  homeSpan.poll();
  mqtt.loop();
}