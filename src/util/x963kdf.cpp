#include "util/x963kdf.h"

X963KDF::X963KDF(mbedtls_md_type_t algorithm, size_t length, const unsigned char* sharedinfo, size_t sharedinfo_len)
    : algorithm(algorithm), length(length), sharedinfo(sharedinfo), sharedinfo_len(sharedinfo_len), used(false) {
    mbedtls_md_init(&md_ctx);
    mbedtls_md_setup(&md_ctx, mbedtls_md_info_from_type(algorithm), 0);
}

void X963KDF::derive(const unsigned char* key_material, size_t key_material_len, unsigned char* output) {
    if (used) {
        throw std::logic_error("Already finalized");
    }
    used = true;

    size_t outlen = 0;
    size_t counter = 1;

    while (length > outlen) {
        mbedtls_md_starts(&md_ctx);
        mbedtls_md_update(&md_ctx, key_material, key_material_len);
        unsigned char counter_bytes[4];
        _int_to_u32be(counter, counter_bytes);
        mbedtls_md_update(&md_ctx, counter_bytes, sizeof(counter_bytes));
        if (sharedinfo != nullptr && sharedinfo_len > 0) {
            mbedtls_md_update(&md_ctx, sharedinfo, sharedinfo_len);
        }
        mbedtls_md_finish(&md_ctx, output + outlen);
        outlen += mbedtls_md_get_size(mbedtls_md_info_from_type(algorithm));
        counter += 1;
    }
}

void X963KDF::verify(const unsigned char* key_material, size_t key_material_len, const unsigned char* expected_key) {
    uint8_t derivedKey[32];
    derive(key_material, key_material_len, derivedKey);
    if (!constant_time_eq(derivedKey, expected_key, length))
    {
        throw std::invalid_argument("Invalid key");
    }
}

void X963KDF::_int_to_u32be(unsigned int n, unsigned char *result) {
    result[0] = static_cast<unsigned char>((n >> 24) & 0xFF);
    result[1] = static_cast<unsigned char>((n >> 16) & 0xFF);
    result[2] = static_cast<unsigned char>((n >> 8) & 0xFF);
    result[3] = static_cast<unsigned char>(n & 0xFF);
}

bool X963KDF::constant_time_eq(const unsigned char* a, const unsigned char* b, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}
