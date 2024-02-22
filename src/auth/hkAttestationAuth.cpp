#include <auth/hkAttestationAuth.h>

std::vector<unsigned char> HKAttestationAuth::attestation_salt(std::vector<unsigned char> &env1Data, std::vector<unsigned char> &readerCmd)
{
  auto env1ResTlv = BERTLV::unpack_array(env1Data);
  auto env1Ndef = BERTLV::findTag(kNDEF_MESSAGE, env1ResTlv);
  NDEFMessage ndefEnv1Ctx = NDEFMessage(env1Ndef.value.data(), env1Ndef.value.size());
  auto ndefEnv1Data = ndefEnv1Ctx.unpack();
  auto ndefEnv1Pack = ndefEnv1Ctx.pack();
  NDEFRecord *res_eng = ndefEnv1Ctx.findType("iso.org:18013:deviceengagement");
  CBOR deviceEngagement = CBOR();
  deviceEngagement.encode(res_eng->data.data(), res_eng->data.size() - 1);
  LOG(V, "DEV ENG CBOR: %s", utils::bufToHexString(deviceEngagement.to_CBOR(), deviceEngagement.length()).c_str());
  CBOR readerNdef = CBOR();
  readerNdef.encode(readerCmd.data(), readerCmd.size());
  LOG(V, "READER NDEF CBOR: %s", utils::bufToHexString(readerNdef.to_CBOR(), readerNdef.length()).c_str());
  CBOR deviceNdef = CBOR();
  deviceNdef.encode(env1Ndef.value.data(), env1Ndef.value.size());
  LOG(V, "DEVICE NDEF CBOR: %s", utils::bufToHexString(deviceNdef.to_CBOR(), deviceNdef.length()).c_str());

  CBOR tag1 = CBOR(24, deviceEngagement);
  json tagArray = {json::from_cbor(tag1.to_CBOR(), tag1.to_CBOR() + tag1.length(), true, true, json::cbor_tag_handler_t::store), {json::from_cbor(deviceNdef.to_CBOR(), deviceNdef.to_CBOR() + deviceNdef.length(), true, true, json::cbor_tag_handler_t::store), json::from_cbor(readerNdef.to_CBOR(), readerNdef.to_CBOR() + readerNdef.length(), true, true, json::cbor_tag_handler_t::store)}};
  std::vector<uint8_t> cborArr = json::to_cbor(tagArray);
  LOG(V, "CBOR ARRAY: %s", utils::bufToHexString(cborArr.data(), cborArr.size()).c_str());

  CBOR arrBuf = CBOR();

  arrBuf.encode(cborArr.data(), cborArr.size());

  CBOR tag2 = CBOR();

  tag2.encode(24, arrBuf);

  LOG(D, "CBOR MATERIAL DATA: %s", utils::bufToHexString(tag2.get_buffer(), tag2.length()).c_str());

  std::vector<uint8_t> salt(32);
  int shaRet = mbedtls_sha256_ret((const unsigned char *)tag2.get_buffer(), tag2.length(), salt.data(), false);

  if (shaRet != 0)
  {
      LOG(E, "SHA256 Failed - %s", mbedtls_high_level_strerr(shaRet));
      return std::vector<unsigned char>();
  }

  LOG(D, "ATTESTATION SALT: %s", utils::bufToHexString(salt.data(), salt.size()).c_str());

  return salt;
}

std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> HKAttestationAuth::envelope1Cmd()
{
  uint8_t ctrlFlow[4] = {0x80, 0x3c, 0x40, 0xa0};
  uint8_t ctrlFlowRes[8];
  uint8_t ctrlFlowResLen = 8;
  nfcInDataExchange(ctrlFlow, sizeof(ctrlFlow), ctrlFlowRes, &ctrlFlowResLen);
  LOG(D, "CTRL FLOW RES LENGTH: %d, DATA: %s", ctrlFlowResLen, utils::bufToHexString(ctrlFlowRes, ctrlFlowResLen).c_str());
  if (ctrlFlowRes[0] == 0x90 && ctrlFlowRes[1] == 0x0)
  { // cla=0x00; ins=0xa4; p1=0x04; p2=0x00; lc=0x07(7); data=a0000008580102; le=0x00
    uint8_t data[] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x02, 0x0};
    uint8_t response[4];
    uint8_t responseLength = 4;
    nfcInDataExchange(data, sizeof(data), response, &responseLength);
    LOG(D, "ENV1.2 RES LENGTH: %d, DATA: %s", responseLength, utils::bufToHexString(response, responseLength).c_str());
    if (response[0] == 0x90 && response[1] == 0x0){
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
      if (env1Res[env1ResLen - 2] == 0x90 && env1Res[env1ResLen - 1] == 0x0){
        return std::make_tuple(std::vector<unsigned char>{env1Res, env1Res + env1ResLen}, ndefMessage);
      }
    }
  }
  return std::make_tuple(std::vector<uint8_t>(), std::vector<uint8_t>());
}

std::vector<unsigned char> HKAttestationAuth::envelope2Cmd(std::vector<uint8_t> &salt)
{
  ISO18013SecureContext secureCtx = ISO18013SecureContext(attestation_exchange_common_secret, salt, 16);
  json ex1 = json::parse(R"({ "docType":"com.apple.HomeKit.1.credential","nameSpaces":{"com.apple.HomeKit":{"credential_id": false}}})");
  auto cbor = ex1.to_cbor(ex1);
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
  LOG(V, "CBOR2ENC: %s", utils::bufToHexString(v.data(), v.size()).c_str());
  auto encrypted = secureCtx.encryptMessageToEndpoint(v);
  if(encrypted.size() > 0){
    LOG(D, "ENC DATA: %s", utils::bufToHexString(encrypted.data(), encrypted.size()).c_str());

    auto tlv = utils::simple_tlv(0x53, encrypted.data(), encrypted.size());

    unsigned char apdu[6 + tlv.size()] = {0x0, 0xC3, 0x0, 0x0, (unsigned char)tlv.size()};

    memcpy(apdu + 5, tlv.data(), tlv.size());
    LOG(D, "ENV2 APDU - LENGTH: %d, DATA: %s", sizeof(apdu), utils::bufToHexString(apdu, sizeof(apdu)).c_str());
    uint8_t newLen = 255;
    uint8_t *env2Res = new unsigned char[255];
    std::vector<unsigned char> attestation_package;
    uint8_t getData[5] = {0x0, 0xc0, 0x0, 0x0, 0x0};
    LOG(D, "ENV2 APDU Len: %d, Data: %s", sizeof(apdu), utils::bufToHexString(apdu, sizeof(apdu)).c_str());
    nfcInDataExchange(apdu, sizeof(apdu), env2Res, &newLen);
    attestation_package.insert(attestation_package.begin(), env2Res + 1, env2Res + newLen - 1);
    LOG(D, "env2Res Len: %d, Data: %s", newLen, utils::bufToHexString(env2Res + 1, newLen).c_str());
    while (env2Res[newLen - 1] == 0xFD)
    {
      nfcInDataExchange(getData, sizeof(getData), env2Res, &newLen);
      attestation_package.insert(attestation_package.end(), env2Res + (newLen == 255 ? 1 : 0), env2Res + newLen - (newLen == 255 ? 1 : 0));
      LOG(D, "env2Res Len: %d, Data: %s", newLen, utils::bufToHexString(env2Res + 1, newLen).c_str());
    }
    delete[] env2Res;
    LOG(V, "ATT PKG LENGTH: %d - DATA: %s", attestation_package.size(), utils::bufToHexString(attestation_package.data(), attestation_package.size()).c_str());
    std::vector<BERTLV> data = BERTLV::unpack_array(attestation_package);
    if(BERTLV::findTag(0x90, data).tag.size() > 0){
      auto decrypted_message = secureCtx.decryptMessageFromEndpoint(BERTLV::findTag(0x53, data).value);
      if(decrypted_message.size() > 0){
        return decrypted_message;
      }
    }
  }
  return std::vector<uint8_t>();
}

homeKeyReader::KeyFlow HKAttestationAuth::attest()
{
  attestation_exchange_common_secret.resize(32);
  attestation_exchange_common_secret.reserve(32);
  esp_fill_random(attestation_exchange_common_secret.data(), 32);
  auto attTlv = utils::simple_tlv(0xC0, attestation_exchange_common_secret.data(), 32, NULL, NULL);
  auto opAttTlv = utils::simple_tlv(0x8E, attTlv.data(), attTlv.size(), NULL, NULL);
  uint8_t attComm[opAttTlv.size() + 1] = {0x0};
  memcpy(attComm + 1, opAttTlv.data(), opAttTlv.size());
  LOG(D, "attComm: %s", utils::bufToHexString(attComm, sizeof(attComm)).c_str());
  auto encryptedCmd = DKSContext.encrypt_command(attComm, sizeof(attComm));

  LOG(V, "encrypted_command: %s", utils::bufToHexString(std::get<0>(encryptedCmd).data(), std::get<0>(encryptedCmd).size()).c_str());
  LOG(V, "calculated_rmac: %s", utils::bufToHexString(std::get<1>(encryptedCmd).data(), std::get<1>(encryptedCmd).size()).c_str());
  uint8_t xchApdu[std::get<0>(encryptedCmd).size() + 6] = {0x84, 0xc9, 0x0, 0x0, (uint8_t)std::get<0>(encryptedCmd).size()};
  memcpy(xchApdu + 5, std::get<0>(encryptedCmd).data(), std::get<0>(encryptedCmd).size());
  LOG(V, "APDU CMD LENGTH: %d, DATA: %s", sizeof(xchApdu), utils::bufToHexString(xchApdu, sizeof(xchApdu)).c_str());
  uint8_t xchRes[16];
  uint8_t xchResLen = 16;
  nfcInDataExchange(xchApdu, sizeof(xchApdu), xchRes, &xchResLen);
  LOG(D, "APDU RES LENGTH: %d, DATA: %s", xchResLen, utils::bufToHexString(xchRes, xchResLen).c_str());

  if (xchResLen > 2 && xchRes[xchResLen - 2] == 0x90)
  {
    auto env1Data = envelope1Cmd();
    std::vector<uint8_t> env1Res = std::get<0>(env1Data);
    if (env1Res.size() > 2 && env1Res.data()[env1Res.size() - 2] == 0x90)
    {
      auto salt = attestation_salt(std::get<0>(env1Data), std::get<1>(env1Data));
      if(salt.size() > 0){
        auto env2DataDec = envelope2Cmd(salt);
        if (env2DataDec.size() > 0)
        {
          return homeKeyReader::kFlowATTESTATION;
        }
      }
    }
  }
  return homeKeyReader::kFlowFailed;
}