#include <auth/hkAuthContext.h>
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
#include <chrono>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>


#if __has_include("config.h")
#include <config.h>
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
#define HK_CODE "46637726"
#define LED_PIN 2
#define OTA_PWD "homespan-ota"
#define NAME "HK Lock"
#define DISCOVERY "1"
#define CONTROL_PIN 26
#endif

using namespace nlohmann;

int currentState = 0;

const char* TAG = "MAIN";

PicoMQTT::Client mqtt;
AsyncWebServer webServer(80);

#define PN532_SS (5)
PN532_SPI pn532spi(SPI, PN532_SS);
PN532 nfc(pn532spi);

nvs_handle_t savedData;
nvs_handle_t mqttDataHandle;
homeKeyReader::readerData_t readerData;
namespace mqttConfig{
  struct mqttConfig_t
  {
    std::string mqttBroker = "";
    uint16_t mqttPort = 1883;
    std::string mqttUsername = "";
    std::string mqttPassword = "";
    std::string mqttClientId = "";
    std::string hkTopic = "";
    std::string lockStateTopic = "";
    std::string lockStateCmd = "";
    std::string lockCStateCmd = "";
    std::string lockTStateCmd = "";
  } mqttData;
  inline void to_json(json &j, const mqttConfig_t &p)
  {
    j = json{{"broker", p.mqttBroker}, {"username", p.mqttUsername}, {"password", p.mqttPassword}, {"client_id", p.mqttClientId}, {"HKTopic", p.hkTopic}, {"lockStateTopic", p.lockStateTopic}, {"lockStateCmd", p.lockStateCmd}, {"lockCStateCmd", p.lockCStateCmd}, {"lockTStateCmd", p.lockTStateCmd}};
  }

  inline void from_json(const json &j, mqttConfig_t &p)
  {
    j.at("broker").get_to(p.mqttBroker);
    j.at("username").get_to(p.mqttUsername);
    j.at("password").get_to(p.mqttPassword);
    j.at("client_id").get_to(p.mqttClientId);
    j.at("HKTopic").get_to(p.hkTopic);
    j.at("lockStateTopic").get_to(p.lockStateTopic);
    j.at("lockStateCmd").get_to(p.lockStateCmd);
    j.at("lockCStateCmd").get_to(p.lockCStateCmd);
    j.at("lockTStateCmd").get_to(p.lockTStateCmd);
  }
}

int hkFlow = 0;

bool save_to_nvs() {
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
  SpanCharacteristic* lockControlPoint;
  SpanCharacteristic* version;

  LockManagement() : Service::LockManagement() {

    Serial.print("Configuring LockManagement\n"); // initialization message

    lockControlPoint = new Characteristic::LockControlPoint();
    version = new Characteristic::Version();

  } // end constructor

}; // end LockManagement

// Function to calculate CRC16
void crc16a(unsigned char* data, unsigned int size, unsigned char* result) {
  unsigned short w_crc = 0x6363;

  for (unsigned int i = 0; i < size; ++i) {
    unsigned char byte = data[i];
    byte = (byte ^ (w_crc & 0x00FF));
    byte = ((byte ^ (byte << 4)) & 0xFF);
    w_crc = ((w_crc >> 8) ^ (byte << 8) ^ (byte << 3) ^ (byte >> 4)) & 0xFFFF;
  }

  result[0] = static_cast<unsigned char>(w_crc & 0xFF);
  result[1] = static_cast<unsigned char>((w_crc >> 8) & 0xFF);
}

// Function to append CRC16 to data
void with_crc16(unsigned char* data, unsigned int size, unsigned char* result) {
  crc16a(data, size, result);
}

struct LockMechanism : Service::LockMechanism
{
  SpanCharacteristic* lockCurrentState;
  SpanCharacteristic* lockTargetState;
  const char* TAG = "LockMechanism";
  uint8_t ecpData[18] = { 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 };

  LockMechanism() : Service::LockMechanism() {
    memcpy(ecpData + 8, readerData.reader_identifier, sizeof(readerData.reader_identifier));
    with_crc16(ecpData, 16, ecpData + 16);
    LOG(I, "Configuring LockMechanism"); // initialization message
    lockCurrentState = new Characteristic::LockCurrentState(1, true);
    lockTargetState = new Characteristic::LockTargetState(1, true);
    mqtt.subscribe(
        mqttConfig::mqttData.lockStateCmd.c_str(), [this](const char *payload)
        {
        LOG(D, "Received message in topic set_state: %s", payload);
        int state = atoi(payload);
        lockTargetState->setVal(state == 0 || state == 1 ? state : lockTargetState->getVal());
        lockCurrentState->setVal(state == 0 || state == 1 ? state : lockCurrentState->getVal()); },
      false);
    mqtt.subscribe(
        mqttConfig::mqttData.lockTStateCmd.c_str(), [this](const char *payload)
        {
        LOG(D, "Received message in topic set_target_state: %s", payload);
        int state = atoi(payload);
        lockTargetState->setVal(state == 0 || state == 1 ? state : lockTargetState->getVal()); },
      false);
    mqtt.subscribe(
        mqttConfig::mqttData.lockCStateCmd.c_str(), [this](const char *payload)
        {
        LOG(D, "Received message in topic set_current_state: %s", payload);
        int state = atoi(payload);
        lockCurrentState->setVal(state == 0 || state == 1 ? state : lockCurrentState->getVal()); },
      false);
  } // end constructor

  boolean update(std::vector<char>* callback, int* callbackLen) {
    int targetState = lockTargetState->getNewVal();
    LOG(I, "New LockState=%d, Current LockState=%d", targetState, lockCurrentState->getVal());

    // lockCurrentState->setVal(targetState);
    mqtt.publish(mqttConfig::mqttData.lockStateTopic.c_str(), std::to_string(targetState).c_str());

    return (true);
  }

  void loop() {
    uint8_t uid[16];
    uint8_t uidLen = 0;
    uint16_t atqa[1];
    uint8_t sak[1];
    bool passiveTarget = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, atqa, sak, 500, true);
    if (passiveTarget) {
      LOG(D, "ATQA: %s", utils::bufToHexString(atqa, 1).c_str());
      LOG(D, "SAK: %s", utils::bufToHexString(sak, 1).c_str());
      LOG(D, "UID: %s", utils::bufToHexString(uid, uidLen).c_str());
      LOG(I, "*** PASSIVE TARGET DETECTED ***");
      auto startTime = std::chrono::high_resolution_clock::now();
      uint8_t data[13] = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x0 };
      uint8_t selectCmdRes[32];
      uint8_t selectCmdResLength = 32;
      LOG(D, "SELECT HomeKey Applet, APDU: %s", utils::bufToHexString(data, sizeof(data)).c_str());
      bool exchange = nfc.inDataExchange(data, sizeof(data), selectCmdRes, &selectCmdResLength);
      LOG(D, "SELECT HomeKey Applet, Response: %s, Length: %d", utils::bufToHexString(selectCmdRes, selectCmdResLength).c_str(), selectCmdResLength);
      if (selectCmdRes[selectCmdResLength - 2] == 0x90 && selectCmdRes[selectCmdResLength - 1] == 0x00) {
        LOG(I, "*** SELECT HOMEKEY APPLET SUCCESSFUL ***");
        LOG(D, "Reader Private Key: %s", utils::bufToHexString((const uint8_t*)readerData.reader_private_key, sizeof(readerData.reader_private_key)).c_str());
        HKAuthenticationContext authCtx([](uint8_t* apdu, size_t apduLen, uint8_t* res, uint8_t* resLen) {  return nfc.inDataExchange(apdu, apduLen, res, resLen); },
          readerData);
        auto authResult = authCtx.authenticate(hkFlow, savedData);
        if (std::get<2>(authResult) != homeKeyReader::kFlowFailed) {
          int newTargetState = lockTargetState->getNewVal();
          int targetState = lockTargetState->getVal();
          mqtt.publish(mqttConfig::mqttData.lockStateTopic.c_str(), std::to_string(newTargetState == targetState ? !lockCurrentState->getVal() : newTargetState).c_str());
          json payload;
          payload["issuerId"] = utils::bufToHexString(std::get<0>(authResult), 8, true);
          payload["endpointId"] = utils::bufToHexString(std::get<1>(authResult), 6, true);
          payload["homekey"] = true;
          mqtt.publish(mqttConfig::mqttData.hkTopic.c_str(), payload.dump().c_str());
          auto stopTime = std::chrono::high_resolution_clock::now();
          LOG(I, "Total Time (from detection to mqtt publish): %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
        }
        else {
          LOG(W, "We got status FlowFailed, mqtt untouched!");
        }
      }
      else {
        json payload;
        payload["atqa"] = utils::bufToHexString(atqa, 1, true);
        payload["sak"] = utils::bufToHexString(sak, 1, true);
        payload["uid"] = utils::bufToHexString(uid, uidLen, true);
        payload["homekey"] = false;
        mqtt.publish(mqttConfig::mqttData.hkTopic.c_str(), payload.dump().c_str());
      }
      delay(1000);
      nfc.inRelease();
      bool deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
      while (deviceStillInField) {
        LOG(V, "Target still present: %d", deviceStillInField);
        delay(1000);
        nfc.inRelease();
        deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
      }
    }
    else {
      uint8_t response[2];
      uint8_t length = 2;
      nfc.writeRegister(0x633d, 0);
      nfc.inCommunicateThru(ecpData, sizeof(ecpData), response, &length, 100);
    }
  }
};

struct NFCAccess : Service::NFCAccess, CommonCryptoUtils
{
  SpanCharacteristic* configurationState;
  SpanCharacteristic* nfcControlPoint;
  SpanCharacteristic* nfcSupportedConfiguration;
  const char* TAG = "NFCAccess";

  NFCAccess() : Service::NFCAccess() {
    LOG(I, "Configuring NFCAccess"); // initialization message
    configurationState = new Characteristic::ConfigurationState();
    nfcControlPoint = new Characteristic::NFCAccessControlPoint();
    nfcSupportedConfiguration = new Characteristic::NFCAccessSupportedConfiguration();
  }
  std::tuple<uint8_t*, int> provision_device_cred(uint8_t* buf, size_t len) {
    TLV<Device_Credential_Request, 5> tlv8;
    LOG(D, "DCR Buffer length: %d, data: %s", len, utils::bufToHexString(buf, len).c_str());
    tlv8.create(kDevice_Req_Key_Type, 1, "KEY.TYPE");
    tlv8.create(kDevice_Req_Public_Key, 65, "PUBLIC.KEY");
    tlv8.create(kDevice_Req_Issuer_Key_Identifier, 8, "ISSUER.IDENTIFIER");
    tlv8.create(kDevice_Req_Key_State, 1, "KEY.STATE");
    tlv8.create(kDevice_Req_Key_Identifier, 8, "KEY.IDENTIFIER");

    LOG(V, "DCR TLV DECODE STATE: %d", tlv8.unpack(buf, len));
    tlv8.print(1);
    homeKeyIssuer::issuer_t* foundIssuer = nullptr;
    for (auto& issuer : readerData.issuers) {
      if (!memcmp(issuer.issuerId, tlv8.buf(kDevice_Req_Issuer_Key_Identifier), 8)) {
        LOG(D, "Found issuer - ID: %s", utils::bufToHexString(issuer.issuerId, 8).c_str());
        foundIssuer = &issuer;
      }
    }
    if (foundIssuer != nullptr) {
      homeKeyEndpoint::endpoint_t* foundEndpoint = 0;
      uint8_t endEphPubKey[tlv8.len(kDevice_Req_Public_Key) + 1] = { 0x04 };
      memcpy(endEphPubKey + 1, tlv8.buf(kDevice_Req_Public_Key), tlv8.len(kDevice_Req_Public_Key));
      std::vector<uint8_t> endpointId = utils::getHashIdentifier(endEphPubKey, sizeof(endEphPubKey), false);
      for (auto& endpoint : foundIssuer->endpoints) {
        if (!memcmp(endpoint.endpointId, endpointId.data(), 6)) {
          LOG(D, "Found endpoint - ID: %s", utils::bufToHexString(endpoint.endpointId, 6).c_str());
          foundEndpoint = &endpoint;
        }
      }
      if (foundEndpoint == 0) {
        LOG(D, "Adding new endpoint - ID: %s , PublicKey: %s", utils::bufToHexString(endpointId.data(), 6).c_str(), utils::bufToHexString(endEphPubKey, sizeof(endEphPubKey)).c_str());
        homeKeyEndpoint::endpoint_t endpoint;
        endpointEnrollment::enrollment_t hap;
        hap.unixTime = std::time(nullptr);
        uint8_t encoded[128];
        size_t olen = 0;
        mbedtls_base64_encode(encoded, 128, &olen, buf, len);
        hap.payload.insert(hap.payload.begin(), encoded, encoded + olen);
        std::vector<uint8_t> x_coordinate = get_x(endEphPubKey, sizeof(endEphPubKey));

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
      else {
        LOG(D, "Endpoint already exists - ID: %s", utils::bufToHexString(foundEndpoint->endpointId, 6).c_str());
        save_to_nvs();
        return std::make_tuple(tlv8.buf(kDevice_Req_Issuer_Key_Identifier), homeKeyReader::DUPLICATE);
      }
      tlv8.clear();
    }
    else {
      LOG(D, "Issuer does not exist - ID: %s", utils::bufToHexString(tlv8.buf(kDevice_Req_Issuer_Key_Identifier), 8).c_str());
      save_to_nvs();
      return std::make_tuple(tlv8.buf(kDevice_Req_Issuer_Key_Identifier), homeKeyReader::DOES_NOT_EXIST);
    }
    return std::make_tuple(readerData.reader_identifier, homeKeyReader::DOES_NOT_EXIST);
  }

  int set_reader_key(uint8_t* buf, size_t len) {
    LOG(D, "Setting reader key: %s", utils::bufToHexString(buf, len).c_str());
    TLV<Reader_Key_Request, 3> tlv8;
    tlv8.create(kReader_Req_Key_Type, 1, "KEY.TYPE");
    tlv8.create(kReader_Req_Reader_Private_Key, 32, "READER.PRIV.KEY");
    tlv8.create(kReader_Req_Identifier, 8, "IDENTIFIER");
    // tlv8.create(kRequest_Reader_Key_Request, 64, "READER.REQ");
    // tlv8.create(kReader_Req_Key_Identifier, 64, "KEY.IDENTIFIER");

    LOG(V, "RKR TLV DECODE STATE: %d", tlv8.unpack(buf, len));
    tlv8.print(1);
    uint8_t* readerKey = tlv8.buf(kReader_Req_Reader_Private_Key);
    uint8_t* uniqueIdentifier = tlv8.buf(kReader_Req_Identifier);
    LOG(D, "Reader Key: %s", utils::bufToHexString(readerKey, tlv8.len(kReader_Req_Reader_Private_Key)).c_str());
    LOG(D, "UniqueIdentifier: %s", utils::bufToHexString(uniqueIdentifier, tlv8.len(kReader_Req_Identifier)).c_str());
    std::vector<uint8_t> pubKey = getPublicKey(readerKey, tlv8.len(kReader_Req_Reader_Private_Key));
    LOG(D, "Got reader public key: %s", utils::bufToHexString(pubKey.data(), pubKey.size()).c_str());
    std::vector<uint8_t> x_coordinate = get_x(pubKey.data(), pubKey.size());
    LOG(D, "Got X coordinate: %s", utils::bufToHexString(x_coordinate.data(), x_coordinate.size()).c_str());
    memcpy(readerData.reader_key_x, x_coordinate.data(), x_coordinate.size());
    memcpy(readerData.reader_public_key, pubKey.data(), pubKey.size());
    memcpy(readerData.reader_private_key, readerKey, tlv8.len(kReader_Req_Reader_Private_Key));
    memcpy(readerData.identifier, uniqueIdentifier, tlv8.len(kReader_Req_Identifier));
    std::vector<uint8_t> readeridentifier = utils::getHashIdentifier(readerData.reader_private_key, sizeof(readerData.reader_private_key), true);
    LOG(D, "Reader GroupIdentifier: %s", utils::bufToHexString(readeridentifier.data(), 8).c_str());
    memcpy(readerData.reader_identifier, readeridentifier.data(), 8);
    bool nvs = save_to_nvs();
    tlv8.clear();
    if (nvs) {
      return 0;
    }
    else
      return 1;
  }

  boolean update(std::vector<char>* callback, int* callbackLen) {
    LOG(D, "PROVISIONED READER KEY: %s", utils::bufToHexString(readerData.reader_private_key, sizeof(readerData.reader_private_key)).c_str());
    LOG(D, "READER GROUP IDENTIFIER: %s", utils::bufToHexString(readerData.reader_identifier, sizeof(readerData.reader_identifier)).c_str());
    LOG(D, "READER UNIQUE IDENTIFIER: %s", utils::bufToHexString(readerData.identifier, sizeof(readerData.identifier)).c_str());

    char* dataConfState = configurationState->getNewString();
    char* dataNfcControlPoint = nfcControlPoint->getNewString();
    LOG(D, "NfcControlPoint Length: %d", strlen(dataNfcControlPoint));
    std::vector<uint8_t> decB64 = utils::decodeB64(dataNfcControlPoint);
    if (decB64.size() == 0)
      return false;
    LOG(D, "Decoded data: %s", utils::bufToHexString(decB64.data(), decB64.size()).c_str());
    LOG(D, "Decoded data length: %d", decB64.size());
    std::vector<BERTLV> tlv = BERTLV::unpack_array(decB64);
    BERTLV operation = BERTLV::findTag(kTLVType1_Operation, tlv);
    LOG(D, "Request Operation: %d", operation.value.data()[0]);
    BERTLV RKR = BERTLV::findTag(kTLVType1_Reader_Key_Request, tlv);
    BERTLV DCR = BERTLV::findTag(kTLVType1_Device_Credential_Request, tlv);
    if (operation.value.data()[0] == 1) {
      if (RKR.tag.size() > 0) {
        LOG(I, "GET READER KEY REQUEST");
        if (strlen((const char*)readerData.reader_private_key) > 0) {
          size_t out_len = 0;
          TLV<Reader_Key_Response, 2> readerKeyResTlv;
          readerKeyResTlv.create(kReader_Res_Key_Identifier, 8, "KEY.IDENTIFIER");
          memcpy(readerKeyResTlv.buf(kReader_Res_Key_Identifier, 8), readerData.reader_identifier, 8);
          size_t lenSubTlv = readerKeyResTlv.pack(NULL);
          uint8_t subTlv[lenSubTlv];
          readerKeyResTlv.pack(subTlv);
          LOG(D, "SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          readerKeyResTlv.clear();
          readerKeyResTlv.create(kReader_Res_Reader_Key_Response, lenSubTlv, "READER.RESPONSE");
          memcpy(readerKeyResTlv.buf(kReader_Res_Reader_Key_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = readerKeyResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          readerKeyResTlv.pack(tlv);
          LOG(D, "TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          uint8_t resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, sizeof(resB64), &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          LOG(D, "B64 ENC STATUS: %d", ret);
          LOG(I, "RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
    }
    else if (operation.value.data()[0] == 2) {
      if (RKR.tag.size() > 0) {
        LOG(I, "SET READER KEY REQUEST");
        int ret = set_reader_key(RKR.value.data(), RKR.value.size());
        if (ret == 0) {
          LOG(I, "KEY SAVED TO NVS, COMPOSING RESPONSE");
          size_t out_len = 0;
          TLV<Reader_Key_Response, 2> readerKeyResTlv;
          readerKeyResTlv.create(kReader_Res_Status, 1, "STATUS");
          readerKeyResTlv.val(kReader_Res_Status, 0);
          size_t lenSubTlv = readerKeyResTlv.pack(NULL);
          uint8_t subTlv[lenSubTlv];
          readerKeyResTlv.pack(subTlv);
          LOG(D, "SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          readerKeyResTlv.clear();
          readerKeyResTlv.create(kReader_Res_Reader_Key_Response, lenSubTlv, "READER.RESPONSE");
          memcpy(readerKeyResTlv.buf(kReader_Res_Reader_Key_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = readerKeyResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          readerKeyResTlv.pack(tlv);
          LOG(D, "TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          unsigned char resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          LOG(D, "B64 ENC STATUS: %d", ret);
          LOG(I, "RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
      else if (DCR.tag.size() > 0) {
        LOG(I, "PROVISION DEVICE CREDENTIAL REQUEST");
        std::tuple<uint8_t*, int> state = provision_device_cred(DCR.value.data(), DCR.value.size());
        if (std::get<1>(state) != 99 && std::get<0>(state) != NULL) {
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
          LOG(D, "SUB-TLV LENGTH: %d, DATA: %s", lenSubTlv, utils::bufToHexString(subTlv, lenSubTlv).c_str());
          devCredResTlv.clear();
          devCredResTlv.print(1);
          devCredResTlv.create(kDevice_Credential_Response, lenSubTlv, "DEV.RESPONSE");
          memcpy(devCredResTlv.buf(kDevice_Credential_Response, lenSubTlv), subTlv, lenSubTlv);
          size_t lenTlv = devCredResTlv.pack(NULL);
          uint8_t tlv[lenTlv];
          devCredResTlv.pack(tlv);
          LOG(D, "TLV LENGTH: %d, DATA: %s", lenTlv, utils::bufToHexString(tlv, lenTlv).c_str());
          mbedtls_base64_encode(NULL, 0, &out_len, tlv, lenTlv);
          unsigned char resB64[out_len + 1];
          int ret = mbedtls_base64_encode(resB64, out_len, &out_len, tlv, lenTlv);
          resB64[out_len] = '\0';
          LOG(D, "B64 ENC STATUS: %d", ret);
          LOG(I, "RESPONSE LENGTH: %d, DATA: %s", out_len, resB64);
          callback->insert(callback->end(), resB64, resB64 + sizeof(resB64));
        }
      }
    }
    else if (operation.value.data()[0] == 3) {
      LOG(I, "REMOVE READER KEY REQUEST");
      std::fill(readerData.reader_identifier, readerData.reader_identifier + 8, 0);
      std::fill(readerData.reader_private_key, readerData.reader_private_key + 32, 0);
      json serializedData = readerData;
      auto msgpack = json::to_msgpack(serializedData);
      esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
      esp_err_t commit_nvs = nvs_commit(savedData);
      LOG(D, "NVS SET: %s", esp_err_to_name(set_nvs));
      LOG(D, "NVS COMMIT: %s", esp_err_to_name(commit_nvs));
      const char* res = "BwMCAQA=";
      size_t resLen = 9;
      LOG(I, "RESPONSE LENGTH: %d, DATA: %s", resLen, res);
      callback->insert(callback->end(), res, res + resLen);
    }
    return true;
  }

}; // end NFCAccess

//////////////////////////////////////

void deleteReaderData(const char* buf) {
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

void pairCallback(bool isPaired) {
  if (!isPaired && HAPClient::nAdminControllers() == 0) {
    deleteReaderData(NULL);
  }
  else if (!isPaired) {
    readerData.issuers.erase(std::remove_if(readerData.issuers.begin(), readerData.issuers.end(),
      [](homeKeyIssuer::issuer_t x) {
        for (size_t i = 0; i < 16; i++) {
          if (HAPClient::controllers[i].allocated) {
            std::vector<uint8_t> id = utils::getHashIdentifier(HAPClient::controllers[i].LTPK, 32, true);
            LOG(D, "Found allocated controller - Hash: %s", utils::bufToHexString(id.data(), 8).c_str());
            if (!memcmp(x.publicKey, id.data(), 8)) {
              return false;
            }
          }
        }
        LOG(D, "Issuer ID: %s - Associated controller was removed from Home, erasing from reader data.", utils::bufToHexString(x.issuerId, 8).c_str());
        return true;
      }),
      readerData.issuers.end());
  }
  if (isPaired) {
    for (size_t i = 0; i < 16; i++) {
      if (HAPClient::controllers[i].allocated) {
        std::vector<uint8_t> id = utils::getHashIdentifier(HAPClient::controllers[i].LTPK, 32, true);
        LOG(D, "Found allocated controller - Hash: %s", utils::bufToHexString(id.data(), 8).c_str());
        homeKeyIssuer::issuer_t* foundIssuer = nullptr;
        for (auto& issuer : readerData.issuers) {
          if (!memcmp(issuer.issuerId, id.data(), 8)) {
            LOG(D, "Issuer %s already added, skipping", utils::bufToHexString(issuer.issuerId, 8).c_str());
            foundIssuer = &issuer;
            break;
          }
        }
        if (foundIssuer == nullptr) {
          LOG(D, "Adding new issuer - ID: %s", utils::bufToHexString(id.data(), 8).c_str());
          homeKeyIssuer::issuer_t issuer;
          memcpy(issuer.issuerId, id.data(), 8);
          memcpy(issuer.publicKey, HAPClient::controllers[i].LTPK, 32);
          readerData.issuers.emplace_back(issuer);
        }
      }
    }
    save_to_nvs();
  }
}

void setFlow(const char* buf) {
  switch (buf[1]) {
  case '0':
    hkFlow = 0;
    Serial.println("FAST Flow");
    break;

  case '1':
    hkFlow = 1;
    Serial.println("STANDARD Flow");
    break;
  case '2':
    hkFlow = 2;
    Serial.println("ATTESTATION Flow");
    break;

  default:
    Serial.println("0 = FAST flow, 1 = STANDARD Flow, 2 = ATTESTATION Flow");
    break;
  }
}

void setLogLevel(const char* buf) {
  esp_log_level_t level = esp_log_level_get("*");
  if (strncmp(buf + 1, "E", 1) == 0) {
    level = ESP_LOG_ERROR;
    Serial.println("ERROR");
  }
  else if (strncmp(buf + 1, "W", 1) == 0) {
    level = ESP_LOG_WARN;
    Serial.println("WARNING");
  }
  else if (strncmp(buf + 1, "I", 1) == 0) {
    level = ESP_LOG_INFO;
    Serial.println("INFO");
  }
  else if (strncmp(buf + 1, "D", 1) == 0) {
    level = ESP_LOG_DEBUG;
    Serial.println("DEBUG");
  }
  else if (strncmp(buf + 1, "V", 1) == 0) {
    level = ESP_LOG_VERBOSE;
    Serial.println("VERBOSE");
  }
  else if (strncmp(buf + 1, "N", 1) == 0) {
    level = ESP_LOG_NONE;
    Serial.println("NONE");
  }

  esp_log_level_set("*", level);
}

void insertDummyIssuers(const char* buf) {
  mbedtls_entropy_context entropy;
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_context drbg;
  mbedtls_ctr_drbg_init(&drbg);
  mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &entropy, NULL, 0);
  std::stringstream strVal;
  strVal << buf[1];
  unsigned int iterations;
  strVal >> iterations;
  if (iterations > 64) {
    Serial.print("\nInvalid Argument\n");
    return;
  }
  for (size_t i = 0; i < iterations; i++) {
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

void print_issuers(const char* buf) {
  const char* TAG = "print_issuers";
  LOG(I, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto& issuer : readerData.issuers) {
    LOG(D, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuerId, sizeof(issuer.issuerId)).c_str(), utils::bufToHexString(issuer.publicKey, sizeof(issuer.publicKey)).c_str());
    for (auto& endpoint : issuer.endpoints) {
      LOG(D, "Endpoint ID: %s, Public Key: %s", utils::bufToHexString(endpoint.endpointId, sizeof(endpoint.endpointId)).c_str(), utils::bufToHexString(endpoint.publicKey, sizeof(endpoint.publicKey)).c_str());
    }
  }
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

String processor(const String& var){
  String result = "";
  if(var == "READERGID"){
    return String(utils::bufToHexString(readerData.reader_identifier, 8, true).c_str());
  } else if(var == "READERID"){
    return String(utils::bufToHexString(readerData.identifier, 8, true).c_str());
  } else if(var == "ISSUERSNO"){
    return String(readerData.issuers.size());
  } else if(var == "ISSUERSLIST"){
    for (auto &&issuer : readerData.issuers)
    {
      char issuerBuff[21 + 8];
      result += "<li>";
      snprintf(issuerBuff, sizeof(issuerBuff), "Issuer ID: %s", utils::bufToHexString(issuer.issuerId, 8, true).c_str());
      result += issuerBuff;
      result += "</li>\n";
      result += "\t\t<ul>";
      for (auto &&endpoint : issuer.endpoints)
      {
        char endBuff[23 + 6];
        result += "\n\t\t\t<li>";
        snprintf(endBuff, sizeof(endBuff), "Endpoint ID: %s", utils::bufToHexString(endpoint.endpointId, 6, true).c_str());
        result += endBuff;
        result += "</li>\n";
      }
      result += "\t\t</ul>";
    }
    return result;
  } else if(var == "MQTTBROKER"){
    return String(mqttConfig::mqttData.mqttBroker.c_str());
  } else if (var == "MQTTPORT"){
    return String(mqttConfig::mqttData.mqttPort);
  } else if(var == "MQTTCLIENTID"){
    return String(mqttConfig::mqttData.mqttClientId.c_str());
  } else if(var == "MQTTUSERNAME"){
    return String(mqttConfig::mqttData.mqttUsername.c_str());
  }else if(var == "MQTTPASSWORD"){
    return String(mqttConfig::mqttData.mqttPassword.c_str());
  }else if(var == "HKTOPIC"){
    return String(mqttConfig::mqttData.hkTopic.c_str());
  }else if(var == "STATETOPIC"){
    return String(mqttConfig::mqttData.lockStateTopic.c_str());
  }else if(var == "STATECMD"){
    return String(mqttConfig::mqttData.lockStateCmd.c_str());
  }else if(var == "CSTATECMD"){
    return String(mqttConfig::mqttData.lockCStateCmd.c_str());
  }else if(var == "TSTATECMD"){
    return String(mqttConfig::mqttData.lockTStateCmd.c_str());
  }
  return result;
}

void setupWeb(){
  
  Serial.printf("Starting HK Server Hub \n");
  webServer.on("/info", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/info.html","text/html", false, processor);
  });  
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html","text/html", false, processor);
  });  
  webServer.on("/mqtt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/mqtt.html","text/html", false, processor);
  });
  webServer.on("/mqttconfig", HTTP_POST, [](AsyncWebServerRequest *request)
               {
    const char *TAG = "mqttconfig";
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      LOG(V,"POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      if(!strcmp(p->name().c_str(), "mqtt-broker")){
        mqttConfig::mqttData.mqttBroker = p->value().c_str();
      } else if(!strcmp(p->name().c_str(), "mqtt-port")){
        int port = atoi(p->value().c_str());
        if(port > 0 && port < 65535){
          mqttConfig::mqttData.mqttPort = port;
        }
      } else if(!strcmp(p->name().c_str(), "mqtt-clientid")){
        mqttConfig::mqttData.mqttClientId = p->value().c_str();
      } else if(!strcmp(p->name().c_str(), "mqtt-username")){
        mqttConfig::mqttData.mqttUsername = p->value().c_str();
      } else if(!strcmp(p->name().c_str(), "mqtt-password")){
        mqttConfig::mqttData.mqttPassword = p->value().c_str();
      } else if(!strcmp(p->name().c_str(), "mqtt-hktopic")){
        mqttConfig::mqttData.hkTopic = p->value().c_str();
      } else if(!strcmp(p->name().c_str(), "mqtt-statetopic")){
        mqttConfig::mqttData.lockStateTopic = p->value().c_str();
      } else if(!strcmp(p->name().c_str(), "mqtt-statecmd")){
        mqttConfig::mqttData.lockStateCmd = p->value().c_str();
      } else if(!strcmp(p->name().c_str(), "mqtt-cstatecmd")){
        mqttConfig::mqttData.lockCStateCmd = p->value().c_str();
      } else if(!strcmp(p->name().c_str(), "mqtt-tstatecmd")){
        mqttConfig::mqttData.lockTStateCmd = p->value().c_str();
      }
    }
    json serializedData = mqttConfig::mqttData;
    auto msgpack = json::to_msgpack(serializedData);
    esp_err_t set_nvs = nvs_set_blob(mqttDataHandle, "MQTTDATA", msgpack.data(), msgpack.size());
    esp_err_t commit_nvs = nvs_commit(mqttDataHandle);
    LOG(V, "SET_STATUS: %s", esp_err_to_name(set_nvs));
    LOG(V, "COMMIT_STATUS: %s", esp_err_to_name(commit_nvs));
    request->send(200, "text/plain", "Received Config, Restarting..."); 
    ESP.restart();
  });
  webServer.onNotFound(notFound);
  webServer.begin();
}

void mqttConfigReset(const char* buf){
  nvs_erase_key(mqttDataHandle, "MQTTDATA");
  nvs_commit(mqttDataHandle);
  ESP.restart();
}

void wifiCallback()
{
  mqtt.host = mqttConfig::mqttData.mqttBroker.c_str();
  mqtt.port = mqttConfig::mqttData.mqttPort;
  mqtt.client_id = mqttConfig::mqttData.mqttClientId.c_str();
  mqtt.username = mqttConfig::mqttData.mqttUsername.c_str();
  mqtt.password = mqttConfig::mqttData.mqttPassword.c_str();
  mqtt.begin();
  setupWeb();
}

void setup() {
  Serial.begin(115200);
  size_t len;
  size_t mqttLen;
  const char* TAG = "SETUP";
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData);
  nvs_open("MQTTDATA", NVS_READWRITE, &mqttDataHandle);
  if (!nvs_get_blob(savedData, "READERDATA", NULL, &len))
  {
    uint8_t msgpack[len];
    nvs_get_blob(savedData, "READERDATA", msgpack, &len);
    LOG(V, "READERDATA - MSGPACK(%d): %s", len, utils::bufToHexString(msgpack, len).c_str());
    json data = json::from_msgpack(msgpack, msgpack + len, true, false);
    LOG(D, "READERDATA - JSON(%d): %s", len, data.dump(-1).c_str());
    if(!data.is_discarded()){
      readerData = data.template get<homeKeyReader::readerData_t>();
    }
  }
  homeSpan.setControlPin(CONTROL_PIN);
  homeSpan.setStatusPin(LED_PIN);
  homeSpan.setStatusAutoOff(15);
  if(!nvs_get_blob(mqttDataHandle, "MQTTDATA", NULL, &mqttLen)){
    uint8_t msgpack[len];
    nvs_get_blob(mqttDataHandle, "MQTTDATA", msgpack, &len);
    LOG(V, "MQTTDATA - MSGPACK(%d): %s", len, utils::bufToHexString(msgpack, len).c_str());
    json data = json::from_msgpack(msgpack, msgpack + len, true, false);
    LOG(D, "MQTTDATA - JSON(%d): %s", len, data.dump(-1).c_str());
    if(!data.is_discarded()){
      mqttConfig::mqttData = data.template get<mqttConfig::mqttConfig_t>();
    }
  }
  if(!LittleFS.begin(true)){
    Serial.println("An Error has occurred while mounting LITTLEFS");
    return;
  }
  listDir(LittleFS, "/", 0);
  // homeSpan.setStatusPin(2);
  // homeSpan.setStatusAutoOff(5);
  homeSpan.reserveSocketConnections(2);
  homeSpan.setPairingCode(HK_CODE);
  homeSpan.setLogLevel(0);

  LOG(D, "READER GROUP ID (%d): %s", strlen((const char*)readerData.reader_identifier), utils::bufToHexString(readerData.reader_identifier, sizeof(readerData.reader_identifier)).c_str());
  LOG(D, "READER UNIQUE ID (%d): %s", strlen((const char*)readerData.identifier), utils::bufToHexString(readerData.identifier, sizeof(readerData.identifier)).c_str());

  LOG(D, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto& issuer : readerData.issuers) {
    LOG(D, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuerId, sizeof(issuer.issuerId)).c_str(), utils::bufToHexString(issuer.publicKey, sizeof(issuer.publicKey)).c_str());
  }
  homeSpan.enableOTA(OTA_PWD);
  homeSpan.begin(Category::Locks, NAME);
  homeSpan.setPortNum(1201);
  homeSpan.setPairCallback(pairCallback);
  homeSpan.setWifiCallback(wifiCallback);
  homeSpan.begin(Category::Locks, "HK Lock");

  new SpanUserCommand('D', "Delete Home Key Data", deleteReaderData);
  new SpanUserCommand('L', "Set Log Level", setLogLevel);
  new SpanUserCommand('F', "Set HomeKey Flow", setFlow);
  new SpanUserCommand('I', "Add dummy Issuers and endpoints", insertDummyIssuers);
  new SpanUserCommand('P', "Print Issuers", print_issuers);
  new SpanUserCommand('M', "Erase MQTT Config and restart", mqttConfigReset);
  new SpanUserCommand('R', "Remove Endpoints", [](const char*) {
    for (auto&& issuer : readerData.issuers) {
      issuer.endpoints.clear();
    }
    });

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    ESP_LOGE("NFC_SETUP", "Didn't find PN53x board");
  }
  else {
    ESP_LOGI("NFC_SETUP", "Found chip PN5%x", (versiondata >> 24) & 0xFF);
    ESP_LOGI("NFC_SETUP", "Firmware ver. %d.%d", (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(0);
    ESP_LOGI("NFC_SETUP", "Waiting for an ISO14443A card");
  }

  new SpanAccessory();                 // Begin by creating a new Accessory using SpanAccessory(), no arguments needed
  new Service::AccessoryInformation(); // HAP requires every Accessory to implement an AccessoryInformation Service, with the required Identify Characteristic
  new Characteristic::Identify();
  new Characteristic::Manufacturer("Kodeative");
  new Characteristic::Model("HomeKey-ESP32");
  new Characteristic::Name(NAME);
  new Characteristic::SerialNumber("HK-360");
  new Characteristic::FirmwareRevision("0.1");
  new Characteristic::HardwareFinish();

  new LockManagement();
  new LockMechanism();
  new NFCAccess();
  new Service::HAPProtocolInformation();
  new Characteristic::Version();
  homeSpan.setPairCallback(pairCallback);
  homeSpan.setWifiCallback(wifiCallback);
  mqtt.connected_callback = [] {
    const char* TAG = "MQTT::connected_callback";
    LOG(D, "MQTT connected");
    if (!strcmp(DISCOVERY, "1")) {
      json payload;
      payload["topic"] = MQTT_AUTH_TOPIC;
      payload["value_template"] = "{{ value_json.uid }}";
      json device; // create a JSON object for the device
      device["name"] = NAME; // assign the name value
      device["identifiers"] = { NAME }; // assign the identifiers array
      payload["device"] = device; // assign the device object to the payload object
      std::string bufferpub = payload.dump(-1, ' ', false, json::error_handler_t::strict); // use dump instead of dump_to
      mqtt.publish(("homeassistant/tag/hk-lock/rfid/config"), bufferpub.c_str(), true, 1);
      payload = json();
      payload["topic"] = MQTT_AUTH_TOPIC;
      payload["value_template"] = "{{ value_json.issuerId }}";
      payload["device"] = device; // reuse the device object for the second message
      bufferpub = payload.dump(-1, ' ', false, json::error_handler_t::strict); // use dump instead of dump_to
      mqtt.publish(("homeassistant/tag/hk-lock/hkIssuer/config"), bufferpub.c_str(), true, 1);
      payload = json();
      payload["topic"] = MQTT_AUTH_TOPIC;
      payload["value_template"] = "{{ value_json.endpointId }}";
      payload["device"] = device; // reuse the device object for the third message
      bufferpub = payload.dump(-1, ' ', false, json::error_handler_t::strict); // use dump instead of dump_to
      mqtt.publish(("homeassistant/tag/hk-lock/hkEndpoint/config"), bufferpub.c_str(), true, 1);
      LOG(D, "MQTT PUBLISHED DISCOVERY");
    }
    };
}

//////////////////////////////////////

void loop() {
  homeSpan.poll();
  mqtt.loop();
}
