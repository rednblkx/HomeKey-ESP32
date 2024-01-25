/*
  Code highly inspired by https://github.com/kormax/apple-home-key-reader/blob/main/util/digital_key.py
 */

#include "util/DigitalKeySecureContext.h"

const unsigned char RESPONSE_PCB[] = {0x80};

std::tuple<std::vector<uint8_t>, size_t> DigitalKeySecureContext::pad_mode_3(unsigned char* message, size_t message_size, unsigned char pad_byte = 0x80, size_t block_size = 8) {
    size_t totalLen = message_size;
    size_t padding_length = block_size - (totalLen + 1) % block_size;
    std::vector<uint8_t> buf(totalLen + padding_length + 1);
    memcpy(buf.data(), message, message_size);
    memcpy(buf.data() + message_size, &pad_byte, 1);
    totalLen += padding_length + 1;
    std::fill(&buf.front() + message_size + 1, &buf.back(), 0);
    return std::make_tuple(buf, totalLen);
}

void DigitalKeySecureContext::unpad_mode_3(unsigned char* message, size_t message_size, unsigned char *unpadded, size_t *outLen, unsigned char pad_flag_byte = 0x80, size_t block_size = 16) {
    size_t result_size = 0;
    bool padding = true;
    // printf("\n");
    for (int i = message_size - 1; i >= 0; --i)
    {
        if (!padding)
        {
            // printf("%x ", message[i]);
            unpadded[result_size++] = message[i];
        }
        else if (message[i] == 0x00)
        {
            continue;
        }
        else if (message[i] == pad_flag_byte)
        {
            // printf("found the padding!");
            padding = false;
        }
    }
    // printf("\n");

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
    // for (uint8_t i = 0; i < length; i++) {
    //     Serial.printf(" %02X", plaintext[i]);
    // }
    // Serial.printf("\n");
}

void DigitalKeySecureContext::aes_cmac(const unsigned char* key, const unsigned char* data, size_t data_size, unsigned char* mac) {
    int ret = mbedtls_cipher_cmac(mbedtls_cipher_info_from_values(MBEDTLS_CIPHER_ID_AES, 128, MBEDTLS_MODE_ECB), key, 128, data, data_size, mac);
    // std::cout << "result of cmac: " << ret << std::endl;
}

DigitalKeySecureContext::DigitalKeySecureContext(){}

DigitalKeySecureContext::DigitalKeySecureContext(const unsigned char* volatileKey){
    counter = 0;
    memcpy(this->kenc, volatileKey, 16);   // Assuming a 128-bit key size
    memcpy(this->kmac, volatileKey + 16, 16);   // Assuming a 128-bit key size
    memcpy(this->krmac, volatileKey + 32, 16); // Assuming a 128-bit key size
    memcpy(response_pcb, RESPONSE_PCB, 1);
    std::fill(mac_chaining_value, mac_chaining_value + 16, 0);
    std::fill(response_pcb + 1, response_pcb + 15, 0);
    std::fill(command_pcb, command_pcb + 15, 0);
}

std::tuple<std::vector<uint8_t>, size_t, std::vector<uint8_t>> DigitalKeySecureContext::encrypt_command(unsigned char* data, size_t dataSize) {
    std::vector<uint8_t> ciphertext(dataSize + 16);
    // printf("\n%d\n", dataSize);
    size_t olen = 0;
    encrypt(data, dataSize, command_pcb, kenc, counter, ciphertext.data(), &olen);

    // printf("\n");
    // for (uint8_t i = 0; i < olen; i++) {
    //     printf(" %02X", ciphertext[i]);
    // }
    // printf("\n");

    std::vector<uint8_t> calculated_rmac(16);
    size_t input_dataSize = 16 + olen;
    std::vector<uint8_t> input_data = concatenate_arrays(mac_chaining_value, ciphertext.data(), 16, olen);
    aes_cmac(kmac, input_data.data(), input_dataSize, calculated_rmac.data());
    // for (uint8_t i = 0; i < 16; i++) {
    //     printf(" %02X", calculated_rmac[i]);
    // }
    // printf("\n");

    std::vector<uint8_t> result = concatenate_arrays(ciphertext.data(), calculated_rmac.data(), olen, 8);

    return std::make_tuple(result, olen + 8, calculated_rmac);
}

std::tuple<std::vector<uint8_t>, size_t> DigitalKeySecureContext::decrypt_response(const unsigned char* data, size_t dataSize) {
    std::vector<uint8_t> ciphertext(dataSize - 8);
    memcpy(ciphertext.data(), data, dataSize - 8);

    std::vector<uint8_t> rmac(8);
    memcpy(rmac.data(), data + dataSize - 8, 8);

    std::vector<uint8_t> calculated_rmac(16);
    size_t input_dataSize = 16 + (dataSize - 8);
    std::vector<uint8_t> input_data = concatenate_arrays(mac_chaining_value, ciphertext.data(), 16, dataSize - 8);
    aes_cmac(krmac, input_data.data(), input_dataSize, calculated_rmac.data());
    // for (uint8_t i = 0; i < 8; i++) {
    //     Serial.printf(" %02X", calculated_rmac[i]);
    // }
    // Serial.printf("\n");
    // for (uint8_t i = 0; i < 8; i++) {
    //     Serial.printf(" %02X", rmac[i]);
    // }
    // Serial.printf("\n");

    // assert(memcmp(rmac, calculated_rmac, 8) == 0);

    if(memcmp(rmac.data(), calculated_rmac.data(), 8)){
        return std::make_tuple(std::vector<uint8_t>{}, 0);
    }

    std::vector<uint8_t> plaintext(dataSize - 8);
    size_t olen = 0;
    decrypt(ciphertext.data(), dataSize - 8, response_pcb, kenc, counter, plaintext.data(), &olen);

    counter++;
    return std::make_tuple(plaintext, olen);
}

void DigitalKeySecureContext::encrypt(unsigned char* plaintext, size_t data_size, const unsigned char* pcb, const unsigned char* key, int counter, unsigned char* ciphertext, size_t *olen) {
    if (data_size == 0) {
        return;
    }

    // Pad plaintext
    auto padded = pad_mode_3(plaintext, data_size, 0x80, 16);

    // printf("\n");
    // for (uint8_t i = 0; i < std::get<1>(padded); i++) {
    //     printf(" %02X", std::get<0>(padded)[i]);
    // }
    // printf("\n");

    std::vector<uint8_t> icv(16);
    std::vector<uint8_t> iv(16);
    std::fill(iv.data(), iv.data() + 16, 0);
    unsigned char counter_byte[] = {static_cast<unsigned char>(counter % 256)};
    size_t input_data_size = 15 + 1;
    std::vector<uint8_t> input_data = concatenate_arrays(pcb, counter_byte, 15, 1);
    encrypt_aes_cbc(key, iv.data(), input_data.data(), input_data_size, icv.data());

    // Encrypt using AES-CBC
    encrypt_aes_cbc(key, icv.data(), std::get<0>(padded).data(), std::get<1>(padded), ciphertext);

    memcpy(olen, &std::get<1>(padded), 1);
}

void DigitalKeySecureContext::decrypt(const unsigned char* ciphertext, size_t cipherTextLen, const unsigned char* pcb, const unsigned char* key, int counter, unsigned char* plaintext, size_t* olen) {
    if (cipherTextLen == 0) {
        return;
    }

    std::vector<uint8_t> icv(16);
    std::vector<uint8_t> iv(16);
    std::fill(iv.data(), iv.data() + 16, 0);
    unsigned char counter_byte[] = {static_cast<unsigned char>(counter % 256)};
    size_t input_data_size = 15 + 1;
    std::vector<uint8_t> input_data = concatenate_arrays(pcb, counter_byte, 15, 1);
    encrypt_aes_cbc(key, iv.data(), input_data.data(), input_data_size, icv.data());

    uint8_t dec[cipherTextLen];

    // Decrypt using AES-CBC
    decrypt_aes_cbc(key, icv.data(), ciphertext, cipherTextLen, dec);

    // Unpad plaintext
    unpad_mode_3(dec, cipherTextLen, plaintext, olen);
}

std::vector<uint8_t> DigitalKeySecureContext::concatenate_arrays(const unsigned char* a, const unsigned char* b, size_t size_a, size_t size_b) {
    std::vector<uint8_t> result(size_a + size_b);
    memcpy(result.data(), a, size_a);
    memcpy(result.data() + size_a, b, size_b);

    return result;
}
