/*
    Code was higly inspired by https://github.com/pyca/cryptography/blob/main/src/cryptography/hazmat/primitives/kdf/x963kdf.py
 */
#ifndef X963KDF_H
#define X963KDF_H

#include <iostream>
#include <stdexcept>
#include <memory>
#include <mbedtls/md.h>
#include <mbedtls/cmac.h>

class X963KDF {
public:
    ~X963KDF();
    X963KDF(mbedtls_md_type_t algorithm, size_t length, const unsigned char *sharedinfo, size_t sharedinfo_len);

    void derive(const unsigned char* key_material, size_t key_material_len, unsigned char* output);
    void verify(const unsigned char* key_material, size_t key_material_len, const unsigned char* expected_key);

private:
    mbedtls_md_type_t algorithm;
    mbedtls_md_context_t md_ctx;
    size_t length;
    const unsigned char* sharedinfo;
    size_t sharedinfo_len;
    bool used;

    static void _int_to_u32be(unsigned int n, unsigned char *result);
    static bool constant_time_eq(const unsigned char* a, const unsigned char* b, size_t len);
};

#endif // X963KDF_H
