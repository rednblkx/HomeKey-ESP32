/*
  Code highly inspired by https://github.com/kormax/apple-home-key-reader/blob/main/util/iso18013.py
 */

#include <util/ISO18013SecureContext.h>

const std::vector<uint8_t> READER_CONTEXT = {'S', 'K', 'R', 'e', 'a', 'd', 'e', 'r'};
const std::vector<uint8_t> ENDPOINT_CONTEXT = {'S', 'K', 'D', 'e', 'v', 'i', 'c', 'e'};

const std::vector<uint8_t> READER_MODE = {0x00, 0x00, 0x00, 0x00};
const std::vector<uint8_t> ENDPOINT_MODE = {0x00, 0x00, 0x00, 0x01};

ISO18013SecureContext::ISO18013SecureContext(const std::vector<uint8_t> &sharedSecret, const std::vector<uint8_t> &salt, size_t keyLength)
{
    std::cout << "shared_secret: " << std::endl;
    for (auto& byte : sharedSecret)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::endl;
    std::cout << "salt: " << std::endl;
    for (auto& byte : salt)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::endl;
    std::cout << "keylength: " << keyLength << std::endl;
    this->readerCounter = 1;
    this->endpointCounter = 1;
    this->keyLength = keyLength;
    uint8_t outReader[keyLength];
    uint8_t outEndpoint[keyLength];
    int ret1 = mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), salt.data(), salt.size(), sharedSecret.data(), sharedSecret.size(),
                            READER_CONTEXT.data(), READER_CONTEXT.size(),
                            outReader, keyLength);
    int ret2 = mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), salt.data(), salt.size(), sharedSecret.data(), sharedSecret.size(),
                            ENDPOINT_CONTEXT.data(), ENDPOINT_CONTEXT.size(),
                            outEndpoint, keyLength);

    std::cout << "keylength: " << keyLength << std::endl;
    std::cout << "ret reader: " << ret1 << std::endl;
    std::cout << "ret endpoint: " << ret2 << std::endl;
    std::cout << "outReader: " << std::endl;
    for (auto& byte : outReader)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::endl;
    std::cout << "outEndpoint: " << std::endl;
    for (auto& byte : outEndpoint)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::endl;
    if(ret1){
        throw "HKDF: Could not derive READER key";
    }
    if(ret2){
        throw "HKDF: Could not derive ENDPOINT key";
    }
    this->readerKey.clear();
    this->readerKey.insert(this->readerKey.begin(), outReader, outReader + keyLength);
    this->endpointKey.clear();
    this->endpointKey.insert(this->endpointKey.begin(), outEndpoint, outEndpoint + keyLength);
}

std::vector<uint8_t> ISO18013SecureContext::getReaderIV() const
{
    std::vector<uint8_t> iv(4, 0);
    iv.insert(iv.end(), READER_MODE.begin(), READER_MODE.end());
    uint8_t counter[4] = {0x0, 0x0, 0x0, (uint8_t)this->readerCounter};
    iv.insert(iv.end(), counter, counter + sizeof(counter));
    return iv;
}

std::vector<uint8_t> ISO18013SecureContext::getEndpointIV() const
{
    std::vector<uint8_t> iv(4, 0);
    iv.insert(iv.end(), ENDPOINT_MODE.begin(), ENDPOINT_MODE.end());
    uint8_t counter[4] = { 0x0, 0x0, 0x0, (uint8_t)this->endpointCounter};
    iv.insert(iv.end(), counter, counter + sizeof(counter));
    return iv;
}

std::vector<uint8_t> ISO18013SecureContext::encryptMessageToEndpoint(const std::vector<uint8_t> &message)
{
    std::cout << "message: " << std::endl;
    for (auto& byte : message)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::endl;
    uint8_t ciphertext[message.size() + 16];
    std::vector<uint8_t> iv = getReaderIV();
    std::vector<uint8_t> cbor;
    mbedtls_gcm_context ctx;
    mbedtls_gcm_init(&ctx);

    mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, this->readerKey.data(), this->keyLength * 8);

    int ret = mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT, message.size(),
                                        iv.data(), iv.size(), NULL, 0,
                                        message.data(), ciphertext,
                                        16, ciphertext + message.size());

    std::cout << "IV: " << std::endl;
    for (auto& byte : iv)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::endl;
    std::cout << "ret gcm_crypt: " << ret << std::endl;

    mbedtls_gcm_free(&ctx);


    if (ret != 0)
    {
        throw std::runtime_error("Encryption failed");
    }

    CBOR cipherCbor = CBOR();
    cipherCbor.encode(ciphertext, sizeof(ciphertext));
    CBORPair cipherPair = CBORPair();
    cipherPair.append("data", cipherCbor);

    readerCounter++;

    cbor.insert(cbor.begin(), cipherPair.get_buffer(), cipherPair.get_buffer() + cipherPair.length());

    return cbor;
}

std::vector<uint8_t> ISO18013SecureContext::decryptMessageFromEndpoint(const std::vector<uint8_t> &message)
{
    CBOR inputData = CBOR(message.data(), message.size());
    std::cout << inputData.is_pair() << std::endl;
    std::cout << inputData["data"].is_bytestring() << std::endl;
    std::vector<uint8_t> cborCiphertext;
    uint8_t bytestring[inputData["data"].get_bytestring_len()];
    inputData["data"].get_bytestring(bytestring);
    cborCiphertext.insert(cborCiphertext.begin(), bytestring, bytestring + sizeof(bytestring));
    uint8_t plaintext[cborCiphertext.size() - 16];

    std::vector<uint8_t> iv = getEndpointIV();

    mbedtls_gcm_context ctx;
    mbedtls_gcm_init(&ctx);

    mbedtls_gcm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, endpointKey.data(), keyLength * 8);

    int ret = mbedtls_gcm_auth_decrypt(&ctx, cborCiphertext.size() - 16,
                                       iv.data(), iv.size(), nullptr, 0,
                                       cborCiphertext.data() + cborCiphertext.size() - 16, 16,
                                       cborCiphertext.data(),
                                       plaintext);

    // std::cout << "\ndecrypt status: " << ret << std::endl;

    mbedtls_gcm_free(&ctx);

    if (ret != 0)
    {
        throw std::runtime_error("Decryption failed");
    }

    endpointCounter++;

    return std::vector<unsigned char> {plaintext, plaintext+sizeof(plaintext)};
}