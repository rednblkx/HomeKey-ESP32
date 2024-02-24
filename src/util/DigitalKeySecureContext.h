/*
  Code highly inspired by https://github.com/kormax/apple-home-key-reader/blob/main/util/digital_key.py
 */

#ifndef DIGITAL_KEY_SECURE_CONTEXT_H
#define DIGITAL_KEY_SECURE_CONTEXT_H

#include <tuple>
#include <mbedtls/aes.h>
#include <mbedtls/cmac.h>
#include <mbedtls/platform_util.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <util/utils.h>

class DigitalKeySecureContext {
public:
    DigitalKeySecureContext();
    DigitalKeySecureContext(const unsigned char *volatileKey);

    std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> encrypt_command(unsigned char* data, size_t dataSize);
    std::vector<uint8_t> decrypt_response(const unsigned char* data, size_t dataSize);

private:
    const char *TAG = "DigitalKeySC";
    int counter;
    unsigned char mac_chaining_value[16] = {0x0, 0x0, 0x0, 0x0, 0x0 ,0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    unsigned char command_pcb[15] = {0x0, 0x0, 0x0, 0x0, 0x0 ,0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    unsigned char response_pcb[15] = {0x80, 0x0, 0x0, 0x0, 0x0 ,0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    unsigned char kenc[16];
    unsigned char kmac[16];
    unsigned char krmac[16];

    std::vector<uint8_t> encrypt(unsigned char* plaintext, size_t data_size, const unsigned char* pcb, const unsigned char* key);
    std::vector<uint8_t> decrypt(const unsigned char* ciphertext, size_t data_size, const unsigned char* pcb, const unsigned char* key);
    std::tuple<std::vector<uint8_t>, size_t> pad_mode_3(unsigned char* message, size_t message_size, unsigned char pad_byte, size_t block_size);
    int unpad_mode_3(unsigned char* message, size_t message_size, unsigned char pad_flag_byte, size_t block_size);
    int encrypt_aes_cbc(const unsigned char* key, unsigned char* iv, const unsigned char* plaintext, size_t length, unsigned char* ciphertext);
    int decrypt_aes_cbc(const unsigned char* key, unsigned char* iv, const unsigned char* ciphertext, size_t length, unsigned char* plaintext);
    int aes_cmac(const unsigned char* key, const unsigned char* data, size_t data_size, unsigned char* mac);
    std::vector<uint8_t> concatenate_arrays(const unsigned char* a, const unsigned char* b, size_t size_a, size_t size_b);
};

#endif
