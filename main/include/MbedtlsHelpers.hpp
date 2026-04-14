#pragma once

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>

namespace crypto {

struct ScopedEntropy {
    mbedtls_entropy_context ctx;
    ScopedEntropy() { mbedtls_entropy_init(&ctx); }
    ~ScopedEntropy() { mbedtls_entropy_free(&ctx); }
    mbedtls_entropy_context* get() { return &ctx; }
};

struct ScopedCtrDrbg {
    mbedtls_ctr_drbg_context ctx;
    ScopedCtrDrbg() { mbedtls_ctr_drbg_init(&ctx); }
    ~ScopedCtrDrbg() { mbedtls_ctr_drbg_free(&ctx); }
    mbedtls_ctr_drbg_context* get() { return &ctx; }
};

struct ScopedPk {
    mbedtls_pk_context ctx;
    ScopedPk() { mbedtls_pk_init(&ctx); }
    ~ScopedPk() { mbedtls_pk_free(&ctx); }
    mbedtls_pk_context* get() { return &ctx; }
};

struct ScopedX509Crt {
    mbedtls_x509_crt ctx;
    ScopedX509Crt() { mbedtls_x509_crt_init(&ctx); }
    ~ScopedX509Crt() { mbedtls_x509_crt_free(&ctx); }
    mbedtls_x509_crt* get() { return &ctx; }
};

} // namespace crypto
