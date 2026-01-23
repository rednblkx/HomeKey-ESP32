#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <mbedtls/sha256.h>

namespace Utils {

/**
 * @brief Compute the ISO/IEC 14443-A (CRC_A) checksum for a data buffer and write it in little-endian.
 *
 * @param data Pointer to the input byte buffer.
 * @param size Number of bytes in the buffer to process.
 * @param result Pointer to a 2-byte array that receives the checksum in little-endian order (low byte first).
 */
inline void crc16a(unsigned char* data, unsigned int size, unsigned char* result) {
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

/**
 * @brief Produce the HomeKit issuer identifier derived from a public key.
 *
 * Computes SHA-256 over the ASCII string "key-identifier" concatenated with the provided public key
 * and returns the first 8 bytes of the digest as the issuer identifier.
 *
 * @param key Pointer to the public key bytes (expected length: 32).
 * @param len Length of the public key in bytes (should be 32).
 * @return std::vector<uint8_t> An 8-byte vector containing the first 8 bytes of the SHA-256 digest.
 */
inline std::vector<uint8_t> getHashIdentifier(const uint8_t* key, size_t len) {
    std::vector<unsigned char> hashable;
    std::string prefix = "key-identifier";
    hashable.insert(hashable.begin(), prefix.begin(), prefix.end());
    hashable.insert(hashable.end(), key, key + len);

    uint8_t hash[32];
    mbedtls_sha256(&hashable.front(), hashable.size(), hash, 0);
    
    // The issuer ID is the first 8 bytes of the hash
    return std::vector<uint8_t>{hash, hash + 8};
}

} // namespace Utils

