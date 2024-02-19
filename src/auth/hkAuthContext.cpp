#include <auth/hkAuthContext.h>
/**
 * The HKAuthenticationContext constructor initializes various member variables and generates an
 * ephemeral key for the reader.
 * 
 * @param nfcInDataExchange nfcInDataExchange is a function pointer that points to a function with the
 * following signature: bool (*)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen)
 * @param readerData readerData is a reference to an object of type homeKeyReader::readerData_t.
 */
HKAuthenticationContext::HKAuthenticationContext(bool (*nfcInDataExchange)(uint8_t *data, size_t lenData, uint8_t *res, uint8_t *resLen), homeKeyReader::readerData_t &readerData) : readerData(readerData), nfcInDataExchange(nfcInDataExchange)
{
  auto startTime = std::chrono::high_resolution_clock::now();
  auto readerEphKey = generateEphemeralKey();
  readerEphPrivKey = std::move(std::get<0>(readerEphKey));
  readerEphPubKey = std::move(std::get<1>(readerEphKey));
  transactionIdentifier.resize(16);
  transactionIdentifier.reserve(16);
  esp_fill_random(transactionIdentifier.data(), 16);
  readerIdentifier.reserve(sizeof(readerData.reader_identifier) + sizeof(readerData.identifier));
  readerIdentifier.insert(readerIdentifier.begin(), readerData.reader_identifier, readerData.reader_identifier + sizeof(readerData.reader_identifier));
  readerIdentifier.insert(readerIdentifier.end(), readerData.identifier, readerData.identifier + sizeof(readerData.identifier));
  readerEphX = std::move(get_x(readerEphPubKey));
  auto stopTime = std::chrono::high_resolution_clock::now();
  endpointEphX = std::vector<uint8_t>();
  endpointEphPubKey = std::vector<uint8_t>();
  LOG(I, "Initialization Time: %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
}

/**
 * The function `HKAuthenticationContext::authenticate` performs authentication using various
 * parameters and returns the result along with the authentication flow type.
 * 
 * @param defaultToStd The parameter "defaultToStd" is a boolean flag that determines whether the
 * authentication process should default to the standard flow.
 * @param savedData The parameter `savedData` is a reference to an `nvs_handle` object. It is used to
 * store and retrieve data in the Non-Volatile Storage (NVS) of the device. The NVS is a key-value
 * storage system that allows persistent storage of data even when the device is
 * 
 * @return a tuple containing three elements:
 * 1. A pointer to a uint8_t array.
 * 2. A pointer to a uint8_t array.
 * 3. An enum value of type `homeKeyReader::KeyFlow`.
 */
std::tuple<uint8_t *, uint8_t *, homeKeyReader::KeyFlow> HKAuthenticationContext::authenticate(int hkFlow, nvs_handle &savedData){
  auto startTime = std::chrono::high_resolution_clock::now();
  uint8_t prot_v_data[2] = {0x02, 0x0};

  std::vector<uint8_t> fastTlv(sizeof(prot_v_data) + readerEphPubKey.size() + transactionIdentifier.size() + readerIdentifier.size() + 8);
  size_t len = 0;
  utils::simple_tlv(0x5C, prot_v_data, sizeof(prot_v_data), fastTlv.data(), &len);

  utils::simple_tlv(0x87, readerEphPubKey.data(), readerEphPubKey.size(), fastTlv.data() + len, &len);

  utils::simple_tlv(0x4C, transactionIdentifier.data(), transactionIdentifier.size(), fastTlv.data() + len, &len);

  utils::simple_tlv(0x4D, readerIdentifier.data(), readerIdentifier.size(), fastTlv.data() + len, &len);
  std::vector<uint8_t> apdu{0x80, 0x80, 0x01, 0x01, (uint8_t)len};
  apdu.insert(apdu.begin() + 5, fastTlv.begin(), fastTlv.end());
  uint8_t response[128];
  uint8_t responseLength = 128;
  LOG(D, "Auth0 APDU Length: %d, DATA: %s", apdu.size(), utils::bufToHexString(apdu.data(), apdu.size()).c_str());
  nfcInDataExchange(apdu.data(), apdu.size(), response, &responseLength);
  LOG(D, "Auth0 Response Length: %d, DATA: %s", responseLength, utils::bufToHexString(response, responseLength).c_str());
  auto Auth0Res = BERTLV::unpack_array(response, responseLength);
  auto endpointPubKey = BERTLV::findTag(kEndpoint_Public_Key, Auth0Res);
  endpointEphPubKey = std::move(endpointPubKey.value);
  endpointEphX = std::move(get_x(endpointEphPubKey));
  homeKeyIssuer::issuer_t *foundIssuer = nullptr;
  homeKeyEndpoint::endpoint_t *foundEndpoint = nullptr;
  std::vector<uint8_t> persistentKey;
  homeKeyReader::KeyFlow flowUsed = homeKeyReader::kFlowFailed;
  if(hkFlow == 0){
    auto fastAuth = HKFastAuth(*readerData.reader_key_x, readerData.issuers, readerEphX, endpointEphPubKey, endpointEphX, transactionIdentifier, readerIdentifier).attest(response, responseLength);
    if (std::get<1>(fastAuth) != nullptr && std::get<2>(fastAuth) != homeKeyReader::kFlowFailed)
    {
      foundIssuer = std::get<0>(fastAuth);
      foundEndpoint = std::get<1>(fastAuth);
      flowUsed = std::get<2>(fastAuth);
      LOG(I, "Authentication complete, transaction took %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count());
      LOG(D, "Endpoint %s Authenticated via FAST Flow", utils::bufToHexString(foundEndpoint->endpointId, sizeof(foundEndpoint->endpointId), true).c_str());
    }
  }
  if(foundEndpoint == nullptr){
    auto stdAuth = HKStdAuth(nfcInDataExchange, *readerData.reader_private_key, readerEphPrivKey, readerData.issuers, readerEphX, endpointEphPubKey, endpointEphX, transactionIdentifier, readerIdentifier).attest();
    if (std::get<1>(stdAuth) != nullptr && std::get<4>(stdAuth) != homeKeyReader::kFlowFailed)
    {
      foundIssuer = std::get<0>(stdAuth);
      foundEndpoint = std::get<1>(stdAuth);
      flowUsed = std::get<4>(stdAuth);
      LOG(I, "Authentication complete, transaction took %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count());
      LOG(D, "Endpoint %s Authenticated via STANDARD Flow", utils::bufToHexString(foundEndpoint->endpointId, sizeof(foundEndpoint->endpointId), true).c_str());
      persistentKey = std::get<3>(stdAuth);
      memcpy(foundEndpoint->persistent_key, persistentKey.data(), 32);
      LOG(D, "New Persistent Key: %s", utils::bufToHexString(foundEndpoint->persistent_key, 32).c_str());
    }
  }
  if(foundEndpoint != nullptr && flowUsed != homeKeyReader::kFlowFailed) {
    std::vector<uint8_t> cmdFlowStatus = commandFlow(homeKeyReader::kCmdFlowSuccess);
    LOG(D, "CONTROL FLOW RESPONSE: %s, Length: %d", utils::bufToHexString(cmdFlowStatus.data(), cmdFlowStatus.size()).c_str(), cmdFlowStatus.size());
    if (cmdFlowStatus.data()[0] == 0x90)
    {
      if(flowUsed == homeKeyReader::kFlowSTANDARD && persistentKey.size() > 0){
        json serializedData = readerData;
        auto msgpack = json::to_msgpack(serializedData);
        esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", msgpack.data(), msgpack.size());
        esp_err_t commit_nvs = nvs_commit(savedData);
        LOG(V, "NVS SET STATUS: %s", esp_err_to_name(set_nvs));
        LOG(V, "NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
      }
      return std::make_tuple(foundIssuer->issuerId, foundEndpoint->endpointId, flowUsed);
    } else {
      LOG(E, "Control Flow Response not 0x90!, %s", utils::bufToHexString(cmdFlowStatus.data(), cmdFlowStatus.size()).c_str());
      return std::make_tuple(foundIssuer->issuerId, foundEndpoint->endpointId, homeKeyReader::kFlowFailed);
    }
  }
  return std::make_tuple(foundIssuer->issuerId, foundEndpoint->endpointId, homeKeyReader::kFlowFailed);
}

/**
 * The function `HKAuthenticationContext::commandFlow` sends the command flow status APDU command
 * and returns the response.
 * 
 * @param status The parameter "status" is of type "homeKeyReader::CommandFlowStatus"
 * 
 * @return a std::vector<uint8_t> object, which contains the received response
 */
std::vector<uint8_t> HKAuthenticationContext::commandFlow(homeKeyReader::CommandFlowStatus status)
{
  uint8_t apdu[4] = {0x80, 0x3c, status, status == homeKeyReader::kCmdFlowAttestation ? (uint8_t)0xa0 : (uint8_t)0x0};
  std::vector<uint8_t> cmdFlowRes(4);
  uint8_t cmdFlowResLen = cmdFlowRes.size();
  LOG(D, "APDU: %s, Length: %d", utils::bufToHexString(apdu, sizeof(apdu)).c_str(), sizeof(apdu));
  nfcInDataExchange(apdu, sizeof(apdu), cmdFlowRes.data(), &cmdFlowResLen);
  return cmdFlowRes;
}