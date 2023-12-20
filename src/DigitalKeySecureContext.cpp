// DigitalKeySecureContext.cpp
#include "DigitalKeySecureContext.h"
#include <iostream>
#include <Arduino.h>

const unsigned char RESPONSE_PCB[] = {0x80};

std::tuple<unsigned char*, size_t> DigitalKeySecureContext::pad_mode_3(unsigned char* message, size_t message_size, unsigned char pad_byte = 0x80, size_t block_size = 8) {
    size_t totalLen = message_size;
    size_t padding_length = block_size - (totalLen + 1) % block_size;
    uint8_t *buf = new unsigned char[totalLen + padding_length];
    memcpy(buf, message, message_size);
    memcpy(buf + message_size, &pad_byte, 1);
    totalLen += padding_length + 1;
    std::fill(buf + message_size + 1, buf + totalLen, 0);
    return std::make_tuple(buf, totalLen);
}

void DigitalKeySecureContext::unpad_mode_3(unsigned char* message, size_t message_size, unsigned char *unpadded, size_t *outLen, unsigned char pad_flag_byte = 0x80, size_t block_size = 16) {
    size_t result_size = 0;
    bool padding = true;
    printf("\n");
    for (int i = message_size - 1; i >= 0; --i)
    {
        if (!padding)
        {
            printf("%x ", message[i]);
            unpadded[result_size++] = message[i];
        }
        else if (message[i] == 0x00)
        {
            continue;
        }
        else if (message[i] == pad_flag_byte)
        {
            printf("found the padding!");
            padding = false;
        }
    }
    printf("\n");

    if (result_size == 0) {
        return;
    }

    std::reverse(unpadded, unpadded + result_size);
    memcpy(outLen, &result_size, sizeof(result_size));
}

void DigitalKeySecureContext::encrypt_aes_cbc(const unsigned char* key, unsigned char* iv, const unsigned char* plaintext, size_t length, unsigned char* ciphertext) {
    size_t olen;
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    int ret = mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, length, iv, plaintext, ciphertext);
    // printf("%d\n", strlen((const char *)ciphertext));
    mbedtls_aes_free(&aes_ctx);
}

void DigitalKeySecureContext::decrypt_aes_cbc(const unsigned char* key, unsigned char* iv, const unsigned char* ciphertext, size_t length, unsigned char* plaintext) {
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, length, iv, ciphertext, plaintext);
    mbedtls_aes_free(&aes_ctx);
    for (uint8_t i = 0; i < length; i++) {
        Serial.printf(" %02X", plaintext[i]);
    }
    Serial.printf("\n");
}

void DigitalKeySecureContext::aes_cmac(const unsigned char* key, const unsigned char* data, size_t data_size, unsigned char* mac) {
    const mbedtls_cipher_info_t *cipher_info = mbedtls_cipher_info_from_values(MBEDTLS_CIPHER_ID_AES, 128, MBEDTLS_MODE_ECB);
    int ret = mbedtls_cipher_cmac(cipher_info, key, 128, data, data_size, mac);
    // std::cout << "result of cmac: " << ret << std::endl;
}

DigitalKeySecureContext::DigitalKeySecureContext(const unsigned char* kenc, const unsigned char* kmac, const unsigned char* krmac){
    counter = 0;
    memcpy(this->kenc, kenc, 16);   // Assuming a 128-bit key size
    memcpy(this->kmac, kmac, 16);   // Assuming a 128-bit key size
    memcpy(this->krmac, krmac, 16); // Assuming a 128-bit key size
    memcpy(response_pcb, RESPONSE_PCB, 1);
    std::fill(mac_chaining_value, mac_chaining_value + 16, 0);
    std::fill(response_pcb + 1, response_pcb + 15, 0);
    std::fill(command_pcb, command_pcb + 15, 0);
}

std::tuple<unsigned char*, size_t, unsigned char*> DigitalKeySecureContext::encrypt_command(unsigned char* data, size_t dataSize) {
    unsigned char* ciphertext = new unsigned char[dataSize + 16];
    printf("\n%d\n", dataSize);
    size_t olen = 0;
    encrypt(data, dataSize, command_pcb, kenc, counter, ciphertext, &olen);

    printf("\n");
    for (uint8_t i = 0; i < olen; i++) {
        printf(" %02X", ciphertext[i]);
    }
    printf("\n");

    unsigned char* calculated_rmac = new unsigned char[16];
    size_t input_dataSize = 16 + olen;
    unsigned char* input_data = concatenate_arrays(mac_chaining_value, ciphertext, 16, olen);
    aes_cmac(kmac, input_data, input_dataSize, calculated_rmac);
    for (uint8_t i = 0; i < 16; i++) {
        printf(" %02X", calculated_rmac[i]);
    }
    printf("\n");

    unsigned char* result = concatenate_arrays(ciphertext, calculated_rmac, olen, 8);

    return std::make_tuple(result, olen + 8, calculated_rmac);
}

std::tuple<unsigned char*, size_t> DigitalKeySecureContext::decrypt_response(const unsigned char* data, size_t dataSize) {
    unsigned char* ciphertext = new unsigned char[dataSize - 8];
    memcpy(ciphertext, data, dataSize - 8);

    unsigned char* rmac = new unsigned char[8];
    memcpy(rmac, data + dataSize - 8, 8);

    unsigned char* calculated_rmac = new unsigned char[16];
    size_t input_dataSize = 16 + (dataSize - 8);
    unsigned char* input_data = concatenate_arrays(mac_chaining_value, ciphertext, 16, dataSize - 8);
    aes_cmac(krmac, input_data, input_dataSize, calculated_rmac);
    for (uint8_t i = 0; i < 8; i++) {
        Serial.printf(" %02X", calculated_rmac[i]);
    }
    Serial.printf("\n");
    for (uint8_t i = 0; i < 8; i++) {
        Serial.printf(" %02X", rmac[i]);
    }
    Serial.printf("\n");

    // assert(memcmp(rmac, calculated_rmac, 8) == 0);

    // if(memcmp(rmac, calculated_rmac, 8) != 0){
    //     return std::make_tuple(new unsigned char[2], 0);
    // }

    unsigned char *plaintext = new unsigned char[dataSize - 8];
    size_t olen = 0;
    decrypt(ciphertext, dataSize - 8, response_pcb, kenc, counter, plaintext, &olen);

    counter++;

    return std::make_tuple(plaintext, olen);

}

void DigitalKeySecureContext::encrypt(unsigned char* plaintext, size_t data_size, const unsigned char* pcb, const unsigned char* key, int counter, unsigned char* ciphertext, size_t *olen) {
    if (data_size == 0) {
        return;
    }

    // Pad plaintext
    auto padded = pad_mode_3(plaintext, data_size, 0x80, 16);

    printf("\n");
    for (uint8_t i = 0; i < std::get<1>(padded); i++) {
        printf(" %02X", std::get<0>(padded)[i]);
    }
    printf("\n");

    // Generate ICV
    unsigned char* icv = new unsigned char[16];
    unsigned char *iv = new unsigned char[16];
    std::fill(iv, iv + 16, 0);
    unsigned char counter_byte[] = {static_cast<unsigned char>(counter % 256)};
    size_t input_data_size = 15 + 1;
    unsigned char* input_data = concatenate_arrays(pcb, counter_byte, 15, 1);
    encrypt_aes_cbc(key, iv, input_data, input_data_size, icv);

    // Encrypt using AES-CBC
    encrypt_aes_cbc(key, icv, std::get<0>(padded), std::get<1>(padded), ciphertext);

    memcpy(olen, &std::get<1>(padded), 1);

    delete[] icv;
}

void DigitalKeySecureContext::decrypt(const unsigned char* ciphertext, size_t cipherTextLen, const unsigned char* pcb, const unsigned char* key, int counter, unsigned char* plaintext, size_t* olen) {
    if (cipherTextLen == 0) {
        return;
    }

    // Generate ICV
    unsigned char* icv = new unsigned char[16];
    unsigned char *iv = new unsigned char[16];
    std::fill(iv, iv + 16, 0);
    unsigned char counter_byte[] = {static_cast<unsigned char>(counter % 256)};
    size_t input_data_size = 15 + 1;
    unsigned char* input_data = concatenate_arrays(pcb, counter_byte, 15, 1);
    encrypt_aes_cbc(key, iv, input_data, input_data_size, icv);

    uint8_t dec[cipherTextLen];

    // Decrypt using AES-CBC
    decrypt_aes_cbc(key, icv, ciphertext, cipherTextLen, dec);

    // Unpad plaintext
    unpad_mode_3(dec, cipherTextLen, plaintext, olen);

    delete[] icv;
}

unsigned char* DigitalKeySecureContext::concatenate_arrays(const unsigned char* a, const unsigned char* b, size_t size_a, size_t size_b) {
    unsigned char* result = new unsigned char[size_a + size_b];
    memcpy(result, a, size_a);
    memcpy(result + size_a, b, size_b);

    return result;
}
