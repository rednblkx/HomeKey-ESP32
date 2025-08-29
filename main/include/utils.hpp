#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <mbedtls/sha256.h>

#define LOG(x, format, ...) ESP_LOG##x(TAG, "%s:%d > " format, __FUNCTION__ , __LINE__ __VA_OPT__(, ) __VA_ARGS__)
namespace Utils {

/**
 * @brief Calculates the CRC-16/X-25 checksum.
 * @param data Pointer to the data buffer.
 * @param size Size of the data.
 * @param result Pointer to a 2-byte array to store the result (little-endian).
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
 * @brief Creates the HomeKit issuer identifier hash from a public key.
 * This is a SHA-256 hash of the string "key-identifier" concatenated with the public key.
 * @param key Pointer to the 32-byte public key.
 * @param len Length of the key (should be 32).
 * @return An 8-byte vector containing the truncated hash.
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

#endif // UTILS_HPP
