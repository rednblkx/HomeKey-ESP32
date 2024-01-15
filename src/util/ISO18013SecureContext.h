/*
  Code highly inspired by https://github.com/kormax/apple-home-key-reader/blob/main/util/iso18013.py
 */
#pragma once
#ifndef ISO18013_SECURE_CONTEXT_H
#define ISO18013_SECURE_CONTEXT_H

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <mbedtls/sha256.h>
#include <mbedtls/hkdf.h>
#include <mbedtls/aes.h>
#include <mbedtls/gcm.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <iomanip>
#include <YACL.h>

#include <nlohmann/json.hpp>

using nlohmann::literals::operator ""_json;
using namespace nlohmann::literals;
using namespace nlohmann::json_literals;
using namespace nlohmann::literals::json_literals;
using namespace nlohmann;

// Assuming similar definitions for util::iso7816::ISO7816Tag

class ISO18013SecureContext {
public:
    ISO18013SecureContext(const std::vector<uint8_t>& sharedSecret, const std::vector<uint8_t>& salt, size_t keyLength);

    std::vector<uint8_t> getReaderIV() const;
    std::vector<uint8_t> getEndpointIV() const;

    std::vector<uint8_t> encryptMessageToEndpoint(const std::vector<uint8_t>& message);
    std::vector<uint8_t> decryptMessageFromEndpoint(const std::vector<uint8_t>& message);

private:
  size_t keyLength;
  size_t readerCounter;
  size_t endpointCounter;
  std::vector<uint8_t> readerKey;
  std::vector<uint8_t> endpointKey;
};

#endif // ISO18013_SECURE_CONTEXT_H
