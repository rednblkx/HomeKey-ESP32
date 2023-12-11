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
#include <SPI.h>
#include <Adafruit_PN532.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (D5)
#define PN532_MOSI (D7)
#define PN532_SS   (D8)
#define PN532_MISO (D6)
#define RESET_PIN  (D3)
// Use this line for a breakout with a SPI connection:

Adafruit_PN532 nfc(PN532_SS);

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
  memcpy(readerData.issuers[issuersCount > 0 ? issuersCount : 0].publicKey, publicKey, tlv8.len(kDevice_Req_Public_Key));
  memcpy(readerData.issuers[issuersCount > 0 ? issuersCount : 0].issuerId, issuerIdentifier, tlv8.len(kDevice_Req_Issuer_Key_Identifier));
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

  // boolean update(){
  //   return (true);
  // }

  // void loop(){

  //   // if(nfcControlPoint.timeVal()>5000)                        // here we simulate an actual sensor by generating a random pressure reading every 5 seconds
  //   //   pressure.setVal((double)random(900,1100));

  // } // end loop

}; // end LockManagement

struct LockMechanism : Service::LockMechanism
{
  SpanCharacteristic *lockCurrentState;
  SpanCharacteristic *lockTargetState;

  LockMechanism() : Service::LockMechanism()
  {
    // const char *string = "key-identifier";
    // char hashable[48];
    // strcpy(hashable, string);
    // memcpy(hashable + strlen(string), readerData.reader_private_key, sizeof(readerData.reader_private_key));
    // unsigned char hash[32];
    // char identifier[8];
    // mbedtls_sha256_ret((const unsigned char *)hashable, strlen(string) + sizeof(readerData.reader_private_key), hash, 0);
    // memcpy(identifier, (const char *)hash, 8);
    // size_t lenIdent = sprintf(NULL, "%x", identifier);
    // Serial.printf("%x", identifier);
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
    }
    // strncpy(callback, (const char *)resB64, out_len);
    // memcpy(callbackLen, &out_len, sizeof(out_len));
    return (true);
  }

  void loop()
  {
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
  uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  // nfc.setPassiveActivationRetries(0xFF);
  // success = nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);

  // if (success) {
  //   Serial.println("Found a card!");
  //   nfc.readDetectedPassiveTargetID(uid, &uidLength);
  //   Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
  //   Serial.print("UID Value: ");
  //   for (uint8_t i=0; i < uidLength; i++)
  //   {
  //     Serial.print(" 0x");Serial.print(uid[i], HEX);
  //   }
  //   Serial.println("");
  //   // // Wait 1 second before continuing
  //   // delay(1000);
  // } 
  // else {


  // Serial.println(bitFraming);
  // }

  } // end loop

}; // end NFCAccess

//////////////////////////////////////

void setup()
{

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
    for (int j = 0; j < sizeof(readerData.issuers[i].publicKey);j++)
      Serial.printf("%02x", readerData.issuers[i].publicKey[j]);
  }
  Serial.printf("\n");

  homeSpan.begin(Category::Locks, "Test NFC Lock");
  // pinMode(RESET_PIN, OUTPUT);
  // digitalWrite(RESET_PIN, LOW);
  // delay(1); // min 20ns
  // digitalWrite(RESET_PIN, HIGH);
  // delay(2); // max 2ms

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    // while (1); // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0);

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

void loop()
{
  homeSpan.poll();
  // boolean success;
  // boolean bitFraming;
  // else {
  //   // nfc.setPassiveActivationRetries(0);
  // // }
  // bitFraming = nfc.setBitFraming();
  // nfc.startEcpBroadcast(readerData.reader_identifier, sizeof(readerData.reader_identifier));
  // delay(1);
  // uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
  // uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // success = nfc.inListPassiveTarget();
  // if (success)
  // {
  //   Serial.println("Found something");
  // }
}