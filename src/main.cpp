#include <hkAuthContext.h>
#include <HomeKey.h>
#include <utils.h>
#include "HomeSpan.h"
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include "HAP.h"
#include <PicoMQTT.h>
#include <chrono>
#include <HK_HomeKit.h>

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

#define PN532_SS (5)
PN532_SPI pn532spi(SPI, PN532_SS);
PN532 nfc(pn532spi);

nvs_handle savedData;
homeKeyReader::readerData_t readerData;
homeKeyReader::KeyFlow hkFlow = homeKeyReader::KeyFlow::kFlowFAST;

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
  const char* TAG = "LockManagement";

  LockManagement() : Service::LockManagement() {

    LOG(D,"Configuring LockManagement"); // initialization message

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
      MQTT_SET_STATE_TOPIC, [this](const char* payload) {
        LOG(D, "Received message in topic set_state: %s", payload);
        int state = atoi(payload);
        lockTargetState->setVal(state == 0 || state == 1 ? state : lockTargetState->getVal());
        lockCurrentState->setVal(state == 0 || state == 1 ? state : lockCurrentState->getVal()); },
      false);
    mqtt.subscribe(
      MQTT_SET_TARGET_STATE_TOPIC, [this](const char* payload) {
        LOG(D, "Received message in topic set_target_state: %s", payload);
        int state = atoi(payload);
        lockTargetState->setVal(state == 0 || state == 1 ? state : lockTargetState->getVal()); },
      false);
    mqtt.subscribe(
      MQTT_SET_CURRENT_STATE_TOPIC, [this](const char* payload) {
        LOG(D, "Received message in topic set_current_state: %s", payload);
        int state = atoi(payload);
        lockCurrentState->setVal(state == 0 || state == 1 ? state : lockCurrentState->getVal()); },
      false);
  } // end constructor

  boolean update(std::vector<char>* callback) {
    int targetState = lockTargetState->getNewVal();
    LOG(I, "New LockState=%d, Current LockState=%d", targetState, lockCurrentState->getVal());

    // lockCurrentState->setVal(targetState);
    mqtt.publish(MQTT_STATE_TOPIC, std::to_string(targetState).c_str());

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
        LOG(D, "*** SELECT HOMEKEY APPLET SUCCESSFUL ***");
        LOG(D, "Reader Private Key: %s", utils::bufToHexString((const uint8_t*)readerData.reader_private_key, sizeof(readerData.reader_private_key)).c_str());
        HKAuthenticationContext authCtx([](uint8_t* apdu, size_t apduLen, uint8_t* res, uint8_t* resLen) {  return nfc.inDataExchange(apdu, apduLen, res, resLen); }, readerData, savedData);
        auto authResult = authCtx.authenticate(hkFlow);
        if (std::get<2>(authResult) != homeKeyReader::kFlowFailed) {
          int newTargetState = lockTargetState->getNewVal();
          int targetState = lockTargetState->getVal();
          mqtt.publish(MQTT_STATE_TOPIC, std::to_string(newTargetState == targetState ? !lockCurrentState->getVal() : newTargetState).c_str());
          json payload;
          payload["issuerId"] = utils::bufToHexString(std::get<0>(authResult), 8, true);
          payload["endpointId"] = utils::bufToHexString(std::get<1>(authResult), 6, true);
          payload["homekey"] = true;
          mqtt.publish(MQTT_AUTH_TOPIC, payload.dump().c_str());
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
        mqtt.publish(MQTT_AUTH_TOPIC, payload.dump().c_str());
      }
      delay(500);
      nfc.inRelease();
      nfc.setPassiveActivationRetries(10);
      bool deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
      LOG(V, "Target still present: %d", deviceStillInField);
      while (deviceStillInField) {
        delay(300);
        nfc.inRelease();
        deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
        LOG(V, "Target still present: %d", deviceStillInField);
      }
      nfc.inRelease();
      nfc.setPassiveActivationRetries(0);
    }
    else {
      uint8_t response[2];
      uint8_t length = 2;
      nfc.writeRegister(0x633d, 0);
      nfc.inCommunicateThru(ecpData, sizeof(ecpData), response, &length, 100);
    }
  }
};

struct NFCAccess : Service::NFCAccess
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


  boolean update(std::vector<char>* callback) {
    LOG(D, "PROVISIONED READER KEY: %s", utils::bufToHexString(readerData.reader_private_key, sizeof(readerData.reader_private_key)).c_str());
    LOG(D, "READER GROUP IDENTIFIER: %s", utils::bufToHexString(readerData.reader_identifier, sizeof(readerData.reader_identifier)).c_str());
    LOG(D, "READER UNIQUE IDENTIFIER: %s", utils::bufToHexString(readerData.identifier, sizeof(readerData.identifier)).c_str());

    // char* dataConfState = configurationState->getNewString(); // Underlying functionality currently unknown
    char* dataNfcControlPoint = nfcControlPoint->getNewString();
    LOG(D, "NfcControlPoint Length: %d", strlen(dataNfcControlPoint));
    std::vector<uint8_t> decB64 = utils::decodeB64(dataNfcControlPoint);
    if (decB64.size() == 0)
      return false;
    LOG(D, "Decoded data: %s", utils::bufToHexString(decB64.data(), decB64.size()).c_str());
    LOG(D, "Decoded data length: %d", decB64.size());
    HK_HomeKit hkCtx(readerData, savedData, "READERDATA");
    std::vector<uint8_t> result = hkCtx.processResult(decB64);
    callback->insert(callback->end(), result.begin(), result.end());
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
        for (auto it=HAPClient::controllerList.begin();it!=HAPClient::controllerList.end();it++) {
          if ((*it).allocated) {
            std::vector<uint8_t> id = utils::getHashIdentifier((*it).LTPK, 32, true);
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
    for (auto it=HAPClient::controllerList.begin();it!=HAPClient::controllerList.end();it++) {
      if ((*it).allocated) {
        std::vector<uint8_t> id = utils::getHashIdentifier((*it).LTPK, 32, true);
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
          memcpy(issuer.publicKey, (*it).LTPK, 32);
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
    hkFlow = homeKeyReader::KeyFlow::kFlowFAST;
    Serial.println("FAST Flow");
    break;

  case '1':
    hkFlow = homeKeyReader::KeyFlow::kFlowSTANDARD;
    Serial.println("STANDARD Flow");
    break;
  case '2':
    hkFlow = homeKeyReader::KeyFlow::kFlowATTESTATION;
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

void wifiCallback() {
  mqtt.host = MQTT_HOST;
  mqtt.port = MQTT_PORT;
  mqtt.client_id = MQTT_CLIENTID;
  mqtt.username = MQTT_USERNAME;
  mqtt.password = MQTT_PASSWORD;
  mqtt.begin();
}

void setup() {
  Serial.begin(115200);
  size_t len;
  const char* TAG = "SETUP";
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData);
  if (!nvs_get_blob(savedData, "READERDATA", NULL, &len)) {
    uint8_t msgpack[len];
    nvs_get_blob(savedData, "READERDATA", msgpack, &len);
    LOG(V, "READERDATA - MSGPACK(%d): %s", len, utils::bufToHexString(msgpack, len).c_str());
    json data = json::from_msgpack(msgpack, msgpack + len);
    LOG(D, "READERDATA - JSON(%d): %s", len, data.dump(-1).c_str());
    homeKeyReader::readerData_t p = data.template get<homeKeyReader::readerData_t>();
    readerData = p;
  }
  homeSpan.setControlPin(CONTROL_PIN);
  homeSpan.setStatusPin(LED_PIN);
  homeSpan.setStatusAutoOff(15);
  homeSpan.reserveSocketConnections(2);
  homeSpan.setPairingCode(HK_CODE);
  homeSpan.setLogLevel(0);

  LOG(D, "READER GROUP ID (%d): %s", strlen((const char*)readerData.reader_identifier), utils::bufToHexString(readerData.reader_identifier, sizeof(readerData.reader_identifier)).c_str());
  LOG(D, "READER UNIQUE ID (%d): %s", strlen((const char*)readerData.identifier), utils::bufToHexString(readerData.identifier, sizeof(readerData.identifier)).c_str());

  LOG(I, "HOMEKEY ISSUERS: %d", readerData.issuers.size());
  for (auto& issuer : readerData.issuers) {
    LOG(D, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer.issuerId, sizeof(issuer.issuerId)).c_str(), utils::bufToHexString(issuer.publicKey, sizeof(issuer.publicKey)).c_str());
  }
  homeSpan.enableOTA(OTA_PWD);
  homeSpan.begin(Category::Locks, NAME);

  new SpanUserCommand('D', "Delete Home Key Data", deleteReaderData);
  new SpanUserCommand('L', "Set Log Level", setLogLevel);
  new SpanUserCommand('F', "Set HomeKey Flow", setFlow);
  new SpanUserCommand('I', "Add dummy Issuers and endpoints", insertDummyIssuers);
  new SpanUserCommand('P', "Print Issuers", print_issuers);
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
      char identifier[18];
      sprintf(identifier, "%.2s%.2s%.2s%.2s%.2s%.2s", HAPClient::accessory.ID, HAPClient::accessory.ID + 3, HAPClient::accessory.ID + 6, HAPClient::accessory.ID + 9, HAPClient::accessory.ID + 12, HAPClient::accessory.ID + 15);
      device["identifiers"] = { identifier }; // assign the identifiers array
      payload["device"] = device; // assign the device object to the payload object
      std::string bufferpub = payload.dump(-1, ' ', false, json::error_handler_t::strict); // use dump instead of dump_to
      mqtt.publish(("homeassistant/tag/" MQTT_CLIENTID "/rfid/config"), bufferpub.c_str(), true, 1);
      payload = json();
      payload["topic"] = MQTT_AUTH_TOPIC;
      payload["value_template"] = "{{ value_json.issuerId }}";
      payload["device"] = device; // reuse the device object for the second message
      bufferpub = payload.dump(-1, ' ', false, json::error_handler_t::strict); // use dump instead of dump_to
      mqtt.publish(("homeassistant/tag/" MQTT_CLIENTID "/hkIssuer/config"), bufferpub.c_str(), true, 1);
      payload = json();
      payload["topic"] = MQTT_AUTH_TOPIC;
      payload["value_template"] = "{{ value_json.endpointId }}";
      payload["device"] = device; // reuse the device object for the third message
      bufferpub = payload.dump(-1, ' ', false, json::error_handler_t::strict); // use dump instead of dump_to
      mqtt.publish(("homeassistant/tag/" MQTT_CLIENTID "/hkEndpoint/config"), bufferpub.c_str(), true, 1);
      LOG(D, "MQTT PUBLISHED DISCOVERY");
    }
    };
}

//////////////////////////////////////

void loop() {
  homeSpan.poll();
  mqtt.loop();
}
