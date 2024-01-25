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

class DigitalKeySecureContext {
public:
    DigitalKeySecureContext();
    DigitalKeySecureContext(const unsigned char *volatileKey);

    std::tuple<std::vector<uint8_t>, size_t, std::vector<uint8_t>> encrypt_command(unsigned char* data, size_t dataSize);
    std::tuple<std::vector<uint8_t>, size_t> decrypt_response(const unsigned char* data, size_t dataSize);

private:
    int counter;
    unsigned char mac_chaining_value[16];
    unsigned char command_pcb[15];
    unsigned char response_pcb[15];
    unsigned char kenc[16];
    unsigned char kmac[16];
    unsigned char krmac[16];

    void encrypt(unsigned char* plaintext, size_t data_size, const unsigned char* pcb, const unsigned char* key, int counter, unsigned char *ciphertext, size_t *olen);
    void decrypt(const unsigned char* ciphertext, size_t data_size, const unsigned char* pcb, const unsigned char* key, int counter, unsigned char *plaintext, size_t *olen);
    std::tuple<std::vector<uint8_t>, size_t> pad_mode_3(unsigned char* message, size_t message_size, unsigned char pad_byte, size_t block_size);
    void unpad_mode_3(unsigned char* message, size_t message_size, unsigned char *unpadded, size_t *outLen, unsigned char pad_flag_byte, size_t block_size);
    void encrypt_aes_cbc(const unsigned char* key, unsigned char* iv, const unsigned char* plaintext, size_t length, unsigned char* ciphertext);
    void decrypt_aes_cbc(const unsigned char* key, unsigned char* iv, const unsigned char* ciphertext, size_t length, unsigned char* plaintext);
    void aes_cmac(const unsigned char* key, const unsigned char* data, size_t data_size, unsigned char* mac);
    std::vector<uint8_t> concatenate_arrays(const unsigned char* a, const unsigned char* b, size_t size_a, size_t size_b);
};

#endif
