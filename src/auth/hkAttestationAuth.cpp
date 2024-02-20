#include <auth/hkAttestationAuth.h>

std::vector<unsigned char> attestation_salt(std::vector<unsigned char> env1Data, std::vector<unsigned char> readerCmd)
{
  auto env1ResTlv = BERTLV::unpack_array(env1Data);
  auto env1Ndef = BERTLV::findTag(kNDEF_MESSAGE, env1ResTlv);
  NDEFMessage ndefEnv1Ctx = NDEFMessage(env1Ndef.value.data(), env1Ndef.value.size());
  auto ndefEnv1Data = ndefEnv1Ctx.unpack();
  auto ndefEnv1Pack = ndefEnv1Ctx.pack();
  for (int i = 0; i < ndefEnv1Data.size(); i++)
  {
    printf("\n%s\n", ndefEnv1Data.data()[i].id.data());
  }
  NDEFRecord res_eng = ndefEnv1Ctx.findType("iso.org:18013:deviceengagement");
  // Serial.printf("\n%s\n", res_eng.type.data());
  CBOR deviceEngagement = CBOR();

  deviceEngagement.encode(res_eng.data.data(), res_eng.data.size() - 1);

  CBOR readerNdef = CBOR();
  readerNdef.encode(readerCmd.data(), readerCmd.size());
  CBOR deviceNdef = CBOR();
  deviceNdef.encode(env1Ndef.value.data(), env1Ndef.value.size());

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
  for (auto &byte : salt)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;

  return salt;
}

std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> HKAttestationAuth::envelope1Cmd()
{
  uint8_t controlFlow[4] = {0x80, 0x3c, 0x40, 0xa0};
  uint8_t controlRes[8];
  uint8_t controlResLen = 8;
  nfcInDataExchange(controlFlow, sizeof(controlFlow), controlRes, &controlResLen);
  LOG(D, "ENV1.1 RES LENGTH: %d, DATA: %s", controlResLen, utils::bufToHexString(controlRes, controlResLen).c_str());  
  // nfc.PrintHex(controlRes, controlResLen);

  // cla=0x00; ins=0xa4; p1=0x04; p2=0x00; lc=0x07(7); data=a0000008580102; le=0x00
  uint8_t data[] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x02, 0x0};
  uint8_t response[4];
  uint8_t responseLength = 4;
  nfcInDataExchange(data, sizeof(data), response, &responseLength);
  LOG(D, "ENV1.2 RES LENGTH: %d, DATA: %s", responseLength, utils::bufToHexString(response, responseLength).c_str());  
  // nfc.PrintHex(response, responseLength);
  unsigned char payload[] = {0x15, 0x91, 0x02, 0x02, 0x63, 0x72, 0x01, 0x02, 0x51, 0x02, 0x11, 0x61, 0x63, 0x01, 0x03, 0x6e, 0x66, 0x63, 0x01, 0x0a, 0x6d, 0x64, 0x6f, 0x63, 0x72, 0x65, 0x61, 0x64, 0x65, 0x72};
  unsigned char payload1[] = {0x01};
  unsigned char payload2[] = {0xa2, 0x00, 0x63, 0x31, 0x2e, 0x30, 0x20, 0x81, 0x29};
  auto ndefMessage = NDEFMessage({NDEFRecord("", 0x01, "Hr", payload, sizeof(payload)),
                                  NDEFRecord("nfc", 0x04, "iso.org:18013:nfc", payload1, 1),
                                  NDEFRecord("mdocreader", 0x04, "iso.org:18013:readerengagement", payload2, sizeof(payload2))})
                         .pack();
  LOG(D, "NDEF CMD LENGTH: %d, DATA: %s", ndefMessage.size(), utils::bufToHexString(ndefMessage.data(), ndefMessage.size()).c_str());  
  auto envelope1Tlv = utils::simple_tlv(0x53, ndefMessage.data(), ndefMessage.size(), NULL, NULL);
  uint8_t env1Apdu[envelope1Tlv.size() + 6] = {0x00, 0xc3, 0x00, 0x01, static_cast<uint8_t>(envelope1Tlv.size())};
  memcpy(env1Apdu + 5, envelope1Tlv.data(), envelope1Tlv.size());
  LOG(D, "APDU CMD LENGTH: %d, DATA: %s", sizeof(env1Apdu), utils::bufToHexString(env1Apdu, sizeof(env1Apdu)).c_str());  
  uint8_t env1Res[128];
  uint8_t env1ResLen = 128;
  nfcInDataExchange(env1Apdu, sizeof(env1Apdu), env1Res, &env1ResLen);
  LOG(D, "APDU RES LENGTH: %d, DATA: %s", env1ResLen, utils::bufToHexString(env1Res, env1ResLen).c_str());  
  // nfc.PrintHex(env1Res, env1ResLen);
  return std::make_tuple(std::vector<unsigned char>{env1Res, env1Res + env1ResLen}, ndefMessage);
}

std::vector<unsigned char> HKAttestationAuth::envelope2Cmd(std::vector<uint8_t> env1Data, std::vector<uint8_t> readerCmd, std::vector<uint8_t> shared_secret)
{
  std::cout << std::endl
            << "env1Data: " << std::endl;
  for (auto &byte : env1Data)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;
  std::cout << "readerCmd: " << std::endl;
  for (auto &byte : readerCmd)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;
  std::cout << "shared_secret: " << std::endl;
  for (auto &byte : shared_secret)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;

  auto salt = attestation_salt(env1Data, readerCmd);

  ISO18013SecureContext secureCtx = ISO18013SecureContext(shared_secret, salt, 16);
  json ex1 = json::parse(R"({ "docType":"com.apple.HomeKit.1.credential","nameSpaces":{"com.apple.HomeKit":{"credential_id": false}}})");
  auto cbor = ex1.to_cbor(ex1);
  printf("\ncbor doc_type:");
  for (auto &byte : cbor)
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
  for (auto &byte : v)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;
  auto encrypted = secureCtx.encryptMessageToEndpoint(v);
  printf("\ncbor enc_docRequests: ");

  for (auto &byte : encrypted)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  }
  std::cout << std::endl;

  auto tlv = utils::simple_tlv(0x53, encrypted.data(), encrypted.size());

  for (int i = 0; i < tlv.size(); i++)
  {
    printf("%02x", tlv[i]);
  }
  std::cout << std::endl;

  unsigned char apdu[6 + tlv.size()] = {0x0, 0xC3, 0x0, 0x0, (unsigned char)tlv.size()};

  memcpy(apdu + 5, tlv.data(), tlv.size());
  for (int i = 0; i < sizeof(apdu); i++)
  {
    printf("%02x", apdu[i]);
  }
  std::cout << std::endl;
  uint8_t newLen = 255;
  uint8_t *env2Res = new unsigned char[255];
  std::vector<unsigned char> attestation_package;
  uint8_t getData[5] = {0x0, 0xc0, 0x0, 0x0, 0x0};
  LOG(D, "ENV2 APDU Len: %d, Data: %s", sizeof(apdu), utils::bufToHexString(apdu, sizeof(apdu)).c_str());
  nfcInDataExchange(apdu, sizeof(apdu), env2Res, &newLen);
  attestation_package.insert(attestation_package.begin(), env2Res + 1, env2Res + newLen - 1);
  printf("\nLength chunk: %d\n", newLen);
  LOG(D, "env2Res Len: %d, Data: %s", newLen, utils::bufToHexString(env2Res, newLen).c_str());
  // nfc.PrintHex(env2Res, newLen);
  while (env2Res[newLen - 1] == 0xFD)
  {
    nfcInDataExchange(getData, sizeof(getData), env2Res, &newLen);
    attestation_package.insert(attestation_package.end(), env2Res + (newLen == 255 ? 1 : 0), env2Res + newLen - (newLen == 255 ? 1 : 0));
    printf("\nLength chunk: %d\n", newLen);
    LOG(D, "env2Res Len: %d, Data: %s", newLen, utils::bufToHexString(env2Res, newLen).c_str());
    // nfc.PrintHex(env2Res, newLen);
  }
  delete[] env2Res;
  printf("\ntotal package length: %d\n", attestation_package.size());
  for (auto &data : attestation_package)
  {
    printf("%02x ", data);
  }
  printf("\n");
  BERTLV data = BERTLV::unpack(attestation_package);
  for (auto &byte : data.value)
  {
    printf("%02x ", byte);
  }
  printf("\n");
  auto decrypted_message = secureCtx.decryptMessageFromEndpoint(data.value);
  for (auto &data : decrypted_message)
  {
    printf("%02x ", data);
  }
  printf("\n");
  return decrypted_message;
}

homeKeyReader::KeyFlow HKAttestationAuth::attest(){
  attestation_exchange_common_secret.resize(32);
  attestation_exchange_common_secret.reserve(32);
  esp_fill_random(attestation_exchange_common_secret.data(), 32);
  auto attTlv = utils::simple_tlv(0xC0, attestation_exchange_common_secret.data(), 32, NULL, NULL);
  auto opAttTlv = utils::simple_tlv(0x8E, attTlv.data(), attTlv.size(), NULL, NULL);
  uint8_t attComm[opAttTlv.size() + 1] = {0x0};
  memcpy(attComm + 1, opAttTlv.data(), opAttTlv.size());
  LOG(D, "attComm: %s", utils::bufToHexString(attComm, sizeof(attComm)).c_str());
  // nfc.PrintHex(attComm, sizeof(attComm));
  auto encrypted = DKSContext.encrypt_command(attComm, sizeof(attComm));

  LOG(D, "encrypted_command: %s", utils::bufToHexString(std::get<0>(encrypted).data(), std::get<0>(encrypted).size()).c_str());
  LOG(D, "calculated_rmac: %s", utils::bufToHexString(std::get<1>(encrypted).data(), std::get<1>(encrypted).size()).c_str());
  // nfc.PrintHex(std::get<0>(encrypted), std::get<1>(encrypted));
  // nfc.PrintHex(std::get<2>(encrypted), 8);
  uint8_t exchApdu[std::get<0>(encrypted).size() + 6] = {0x84, 0xc9, 0x0, 0x0, (uint8_t)std::get<0>(encrypted).size()};
  memcpy(exchApdu + 5, std::get<0>(encrypted).data(), std::get<0>(encrypted).size());
  LOG(D, "APDU CMD LENGTH: %d, DATA: %s", sizeof(exchApdu), utils::bufToHexString(exchApdu, sizeof(exchApdu)).c_str());
  uint8_t exchRes[16];
  uint8_t exchResLen = 16;
  nfcInDataExchange(exchApdu, sizeof(exchApdu), exchRes, &exchResLen);
  LOG(D, "APDU RES LENGTH: %d, DATA: %s", exchResLen, utils::bufToHexString(exchRes, exchResLen).c_str());

  if (exchResLen > 2 && exchRes[exchResLen - 2] == 0x90)
  {
    auto env1 = envelope1Cmd();
    std::vector<uint8_t> env1Res = std::get<0>(env1);
    if (env1Res.size() > 2 && env1Res.data()[env1Res.size() - 2] == 0x90)
    {
      auto env2Decrypted = envelope2Cmd(std::get<0>(env1), std::get<1>(env1), attestation_exchange_common_secret);
      if(env2Decrypted.size() > 0){
        return homeKeyReader::kFlowATTESTATION;
        // Serial.print("\nAuth Attestation, storing persistent key");
        // nfc.PrintHex(std::get<3>(keys).data(), 32);
        // memcpy(foundEndpoint->persistent_key, std::get<3>(keys).data(), 32);
        // json data = homeKeyReader::readerData;
        // auto bson = json::to_bson(data);
        // esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", bson.data(), bson.size());
        // esp_err_t commit_nvs = nvs_commit(savedData);
        // LOG1("*** NVS W STATUS: \n");
        // LOG1("SET: %s\n", esp_err_to_name(set_nvs));
        // LOG1("COMMIT: %s\n", esp_err_to_name(commit_nvs));
        // LOG1("*** NVS W STATUS: \n");
        // return std::make_tuple(foundEndpoint, std::get<3>(keys));
      } else return homeKeyReader::kFlowFailed;
    } else return homeKeyReader::kFlowFailed;
  }else return homeKeyReader::kFlowFailed;
  // nfc.PrintHex(exchRes, exchResLen);
  // return std::make_tuple(std::vector<unsigned char> {exchRes, exchRes + exchResLen}, std::vector<unsigned char>{attestation_exchange_common_secret, attestation_exchange_common_secret+32});
  
}