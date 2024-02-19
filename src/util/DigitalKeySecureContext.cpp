/*
  Code highly inspired by https://github.com/kormax/apple-home-key-reader/blob/main/util/digital_key.py
 */

#include "util/DigitalKeySecureContext.h"

/**
 * The function `pad_mode_3` pads a given message with a specified pad byte and block size, and returns
 * the padded message along with its total length.
 * 
 * @param message A pointer to the start of the message data.
 * @param message_size The parameter `message_size` represents the size of the `message` array, which
 * is the length of the input message in bytes.
 * @param pad_byte The pad_byte parameter is the byte value used for padding the message. In this case,
 * it is set to 0x80, which is the hexadecimal representation of the byte value 128.
 * @param block_size The block_size parameter in the pad_mode_3 function is the size of each block in
 * bytes. In this case, it is set to 8 bytes.
 * 
 * @return a std::tuple containing a std::vector<uint8_t> and a size_t.
 */
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
/**
 * The function `unpad_mode_3` searches for a padding flag byte in a given message and returns the
 * index of the last occurrence of the flag byte.
 * 
 * @param message The `message` parameter is a pointer to an array of unsigned characters, which
 * represents the message that needs to be unpadded.
 * @param message_size The parameter `message_size` represents the size of the `message` array, which
 * is the length of the message in bytes.
 * @param pad_flag_byte The pad_flag_byte parameter is a byte value that is used to indicate the
 * padding in the message. In this case, the value is set to 0x80.
 * @param block_size The `block_size` parameter in the `unpad_mode_3` function represents the size of
 * each block in the message. In this case, the block size is set to 16 bytes.
 * 
 * @return the index of the padding flag byte in the message array.
 */
int DigitalKeySecureContext::unpad_mode_3(unsigned char* message, size_t message_size, unsigned char pad_flag_byte = 0x80, size_t block_size = 16) {
    for (int i = message_size - 1; i >= 0; --i)
    {
        if (message[i] == pad_flag_byte)
        {
            LOG(V, "Padding found at index: %d", i);
            return i;
        }
        else if (message[i] == 0x00)
        {
            continue;
        }
    }
    LOG(W, "Padding %x could not be found", pad_flag_byte);
    return message_size - 1;
}

/**
 * The function `encrypt_aes_cbc` encrypts a plaintext using AES-CBC encryption with a given key and
 * initialization vector (IV).
 * 
 * @param key The "key" parameter is a pointer to an array of unsigned characters that represents the
 * encryption key. It is used to set the encryption key for the AES algorithm.
 * @param iv The "iv" parameter stands for "initialization vector". It is a fixed-size random or
 * pseudorandom value that is used along with the encryption key to initialize the encryption
 * algorithm. The IV is typically used to ensure that the same plaintext does not encrypt to the same
 * ciphertext when encrypted multiple times with
 * @param plaintext The plaintext parameter is a pointer to the input data that you want to encrypt
 * using AES-CBC mode. It is of type unsigned char* and points to a buffer containing the plaintext
 * data.
 * @param length The "length" parameter represents the length of the plaintext in bytes.
 * @param ciphertext The `ciphertext` parameter is a pointer to an unsigned char array where the
 * encrypted data will be stored.
 * 
 * @return an integer value. If the value is 0, it means the encryption was successful. If the value is
 * non-zero, it indicates an error occurred during the encryption process.
 */
int DigitalKeySecureContext::encrypt_aes_cbc(const unsigned char* key, unsigned char* iv, const unsigned char* plaintext, size_t length, unsigned char* ciphertext) {
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    int crypt_cbc = mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, length, iv, plaintext, ciphertext);
    mbedtls_aes_free(&aes_ctx);
    if(crypt_cbc != 0){
        LOG(E, "crypt_cbc - %s", mbedtls_high_level_strerr(crypt_cbc));
        return crypt_cbc;
    }
    return 0;
}

/**
 * The function `decrypt_aes_cbc` decrypts a given ciphertext using AES-CBC mode with a specified key
 * and initialization vector (IV), and stores the resulting plaintext in a buffer.
 * 
 * @param key The "key" parameter is a pointer to the unsigned char array that contains the AES
 * encryption key. It is used to set the decryption key for the AES context.
 * @param iv The "iv" parameter stands for "initialization vector". It is a fixed-size random or
 * pseudorandom value that is used along with the encryption key to initialize the encryption
 * algorithm. The IV is used to ensure that even if the same plaintext is encrypted multiple times, the
 * resulting ciphertext will be different
 * @param ciphertext The `ciphertext` parameter is a pointer to the encrypted data that needs to be
 * decrypted. It is of type `const unsigned char*`, which means it is a pointer to an array of unsigned
 * characters (bytes) that represent the encrypted data.
 * @param length The "length" parameter represents the length of the ciphertext in bytes.
 * @param plaintext The `plaintext` parameter is a pointer to a buffer where the decrypted data will be
 * stored.
 * 
 * @return an integer value. If the value is 0, it means the decryption was successful. If the value is
 * non-zero, it indicates an error occurred during the decryption process.
 */
int DigitalKeySecureContext::decrypt_aes_cbc(const unsigned char* key, unsigned char* iv, const unsigned char* ciphertext, size_t length, unsigned char* plaintext) {
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);
    int crypt_cbc = mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, length, iv, ciphertext, plaintext);
    mbedtls_aes_free(&aes_ctx);
    if(crypt_cbc != 0){
        LOG(E, "crypt_cbc - %s", mbedtls_high_level_strerr(crypt_cbc));
        return crypt_cbc;
    }
    return 0;
}

/**
 * The function `aes_cmac` calculates the CMAC (Cipher-based Message Authentication Code) using AES
 * encryption algorithm.
 * 
 * @param key The "key" parameter is a pointer to an array of unsigned characters, which represents the
 * encryption key used for AES-CMAC algorithm. It is assumed to be 128 bits (16 bytes) in length.
 * @param data The "data" parameter is a pointer to the input data that you want to generate a CMAC
 * (Cipher-based Message Authentication Code) for. It is of type "unsigned char*" which means it is a
 * pointer to an array of unsigned characters (bytes).
 * @param data_size The parameter `data_size` represents the size (in bytes) of the `data` array. It
 * indicates the length of the data that needs to be processed for generating the CMAC (Cipher-based
 * Message Authentication Code).
 * @param mac The "mac" parameter is a pointer to an unsigned char array where the resulting CMAC
 * (Cipher-based Message Authentication Code) will be stored. The CMAC is a fixed-size authentication
 * tag that is generated using the AES-CMAC algorithm.
 * 
 * @return an integer value. If the value is 0, it means the CMAC calculation was successful. If the
 * value is non-zero, it indicates an error occurred during the CMAC calculation.
 */
int DigitalKeySecureContext::aes_cmac(const unsigned char* key, const unsigned char* data, size_t data_size, unsigned char* mac) {
    int cmac = mbedtls_cipher_cmac(mbedtls_cipher_info_from_values(MBEDTLS_CIPHER_ID_AES, 128, MBEDTLS_MODE_ECB), key, 128, data, data_size, mac);
    if(cmac != 0){
        LOG(E, "cmac - %s", mbedtls_high_level_strerr(cmac));
        return cmac;
    }
    return 0;
}

DigitalKeySecureContext::DigitalKeySecureContext(){}

DigitalKeySecureContext::DigitalKeySecureContext(const unsigned char* volatileKey){
    counter = 0;
    memcpy(this->kenc, volatileKey, 16);   // Assuming a 128-bit key size
    memcpy(this->kmac, volatileKey + 16, 16);   // Assuming a 128-bit key size
    memcpy(this->krmac, volatileKey + 32, 16); // Assuming a 128-bit key size
}

/**
 * The function encrypts a command using a digital key secure context and returns the encrypted data
 * along with the calculated message authentication code (MAC).
 * 
 * @param data The `data` parameter is a pointer to an array of unsigned characters (bytes) that
 * represents the data to be encrypted.
 * @param dataSize The parameter `dataSize` represents the size of the `data` array, which is of type
 * `unsigned char*`. It indicates the number of elements in the `data` array.
 * 
 * @return The function `encrypt_command` returns a tuple containing two vectors of type `uint8_t`. The
 * first element of the tuple is `result`, which is a vector containing the encrypted data concatenated
 * with the calculated rmac. The second element of the tuple is `calculated_rmac`, which is a vector
 * containing the calculated rmac.
 */
std::tuple<std::vector<uint8_t>, std::vector<uint8_t>> DigitalKeySecureContext::encrypt_command(unsigned char* data, size_t dataSize) {
    std::vector<uint8_t> ciphertext = encrypt(data, dataSize, command_pcb, kenc, counter);;
    std::vector<uint8_t> calculated_rmac(16);
    size_t input_dataSize = 16 + ciphertext.size();
    std::vector<uint8_t> input_data = concatenate_arrays(mac_chaining_value, ciphertext.data(), 16, ciphertext.size());
    int cmac_status = aes_cmac(kmac, input_data.data(), input_dataSize, calculated_rmac.data());
    if(cmac_status) return std::make_tuple(std::vector<uint8_t>(), std::vector<uint8_t>());

    std::vector<uint8_t> result = concatenate_arrays(ciphertext.data(), calculated_rmac.data(), ciphertext.size(), calculated_rmac.size());

    return std::make_tuple(result, calculated_rmac);
}

/**
 * The function decrypts a response by verifying the integrity of the data and then decrypting it using
 * a specified key.
 * 
 * @param data The `data` parameter is a pointer to the encrypted data that needs to be decrypted. It
 * is of type `const unsigned char*`.
 * @param dataSize The parameter `dataSize` represents the size of the `data` array, which is of type
 * `unsigned char*`. It is used to determine the length of the data that needs to be decrypted.
 * 
 * @return a vector of uint8_t, which represents the decrypted plaintext.
 */
std::vector<uint8_t> DigitalKeySecureContext::decrypt_response(const unsigned char* data, size_t dataSize) {
    LOG(V, "encrypted_data: %s", utils::bufToHexString(data, dataSize).c_str());
    std::vector<uint8_t> calculated_rmac(16);
    size_t input_dataSize = 16 + (dataSize - 8);
    std::vector<uint8_t> input_data = concatenate_arrays(mac_chaining_value, data, 16, dataSize - 8);
    int cmac_status = aes_cmac(krmac, input_data.data(), input_dataSize, calculated_rmac.data());
    LOG(V, "recv_rmac: %s", utils::bufToHexString(data + (dataSize - 8), 8).c_str());
    LOG(V, "calculated_rmac: %s", utils::bufToHexString(calculated_rmac.data(), 8).c_str());
    if(cmac_status) return std::vector<uint8_t>();

    if(memcmp(data + (dataSize - 8), calculated_rmac.data(), 8)){
        LOG(E, "calculated_rmac != recv_rmac");
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> plaintext = decrypt(data, dataSize - 8, response_pcb, kenc, counter);

    counter++;
    return plaintext;
}

/**
 * The function encrypts a given plaintext using AES-CBC encryption with a specified key and
 * initialization vector (IV), and returns the encrypted data.
 * 
 * @param plaintext A pointer to the plaintext data that needs to be encrypted.
 * @param data_size The `data_size` parameter represents the size of the plaintext data in bytes.
 * @param pcb The parameter `pcb` is a pointer to an array of unsigned characters. It is used as input
 * data for encryption.
 * @param key The "key" parameter is a pointer to an array of unsigned characters, which represents the
 * encryption key used in the AES-CBC encryption algorithm.
 * @param counter The "counter" parameter is an integer value that is used as part of the encryption
 * process. It is used to generate a counter byte, which is then concatenated with other data to form
 * the input data for the encryption algorithm. The counter value is used to ensure that each
 * encryption operation is unique and prevents
 * 
 * @return a std::vector<uint8_t> object, which contains the encrypted data.
 */
std::vector<uint8_t> DigitalKeySecureContext::encrypt(unsigned char* plaintext, size_t data_size, const unsigned char* pcb, const unsigned char* key, int counter) {
    if (data_size == 0) {
        return std::vector<uint8_t>();
    }

    // Pad plaintext
    auto padded = pad_mode_3(plaintext, data_size, 0x80, 16);

    std::vector<uint8_t> icv(16);
    std::vector<uint8_t> iv(16);
    std::fill(iv.data(), iv.data() + 16, 0);
    unsigned char counter_byte[] = {static_cast<unsigned char>(counter % 256)};
    size_t input_data_size = 15 + 1;
    std::vector<uint8_t> input_data = concatenate_arrays(pcb, counter_byte, 15, 1);
    int encrypt_status1 = encrypt_aes_cbc(key, iv.data(), input_data.data(), input_data_size, icv.data());
    if(encrypt_status1) return std::vector<uint8_t>();

    LOG(V, "ICV: %s", utils::bufToHexString(icv.data(), icv.size()).c_str());
    
    std::vector<uint8_t> enc(std::get<1>(padded));

    // Encrypt using AES-CBC
    int encrypt_status = encrypt_aes_cbc(key, icv.data(), std::get<0>(padded).data(), std::get<1>(padded), enc.data());

    if(encrypt_status) return std::vector<uint8_t>();

    LOG(V, "ENCRYPTED: %s", utils::bufToHexString(enc.data(), enc.size()).c_str());

    return enc;
}

/**
 * The function decrypts a given ciphertext using AES-CBC encryption with a specified key and
 * initialization vector (IV), and returns the decrypted plaintext after unpadding it.
 * 
 * @param ciphertext A pointer to the ciphertext data, which is an array of unsigned characters.
 * @param cipherTextLen The parameter `cipherTextLen` represents the length of the ciphertext, which is
 * the length of the `ciphertext` array.
 * @param pcb The parameter "pcb" stands for "Protocol Control Block". It is a data structure used in
 * network protocols to store information about the current state of a communication session. In this
 * context, it is used as input data for the encryption process.
 * @param key The "key" parameter is a pointer to an array of unsigned characters, which represents the
 * encryption key used for AES-CBC encryption and decryption.
 * @param counter The "counter" parameter is an integer value used for encryption. It is used to
 * generate a counter byte, which is then concatenated with other data to create the input for the
 * encryption algorithm.
 * 
 * @return a std::vector<uint8_t> object.
 */
std::vector<uint8_t> DigitalKeySecureContext::decrypt(const unsigned char* ciphertext, size_t cipherTextLen, const unsigned char* pcb, const unsigned char* key, int counter) {
    if (cipherTextLen == 0) {
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> icv(16, 0);
    std::vector<uint8_t> iv(16, 0);
    unsigned char counter_byte[] = {static_cast<unsigned char>(counter % 256)};
    size_t input_data_size = 15 + 1;
    std::vector<uint8_t> input_data = concatenate_arrays(pcb, counter_byte, 15, 1);
    int encrypt_status = encrypt_aes_cbc(key, iv.data(), input_data.data(), input_data_size, icv.data());
    if(encrypt_status) return std::vector<uint8_t>();

    LOG(V, "ICV: %s", utils::bufToHexString(icv.data(), icv.size()).c_str());

    std::vector<uint8_t> dec(cipherTextLen);

    // Decrypt using AES-CBC
    int decrypt_status = decrypt_aes_cbc(key, icv.data(), ciphertext, cipherTextLen, dec.data());

    if(decrypt_status) return std::vector<uint8_t>();

    LOG(V, "decryted: %s", utils::bufToHexString(dec.data(), dec.size()).c_str());

    // Unpad plaintext
    int padding_index = unpad_mode_3(dec.data(), cipherTextLen);

    dec.resize(padding_index);

    return dec;
}

/**
 * The function `concatenate_arrays` takes two arrays of unsigned characters, along with their
 * respective sizes, and concatenates them into a single vector of uint8_t.
 * 
 * @param a The parameter "a" is a pointer to an array of unsigned characters (bytes).
 * @param b The parameter "b" in the given code is a pointer to an array of unsigned characters
 * (unsigned char).
 * @param size_a The parameter `size_a` represents the size (in bytes) of the array `a`.
 * @param size_b The parameter "size_b" represents the size (number of elements) of the array "b".
 * 
 * @return a std::vector<uint8_t> object.
 */
std::vector<uint8_t> DigitalKeySecureContext::concatenate_arrays(const unsigned char* a, const unsigned char* b, size_t size_a, size_t size_b) {
    std::vector<uint8_t> result(size_a + size_b);
    memcpy(result.data(), a, size_a);
    memcpy(result.data() + size_a, b, size_b);

    return result;
}
