#pragma once

// mbedtls v4 (IDF 6.x) compatibility helpers.
//
// The PK module in TF-PSA-Crypto 1.x no longer exposes the underlying
// RSA/ECC contexts (mbedtls_pk_ec/mbedtls_pk_rsa are gone) and key parsing
// lost its rng parameters. The PK type metadata moved to the private header,
// which we include with MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS (same approach
// as DDK's mbedtls_compat.h). Struct-field access uses MBEDTLS_PRIVATE().

#include <mbedtls/build_info.h>

#if MBEDTLS_VERSION_MAJOR >= 4
#ifndef MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#endif
#include "mbedtls/pk.h"
// The private PK header predates C++ linkage guards; wrap it ourselves.
extern "C" {
#include "mbedtls/private/pk_private.h"
#include "mbedtls/private/ctr_drbg.h"
#include "mbedtls/private/entropy.h"
}
#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"
#else
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#endif

#include <psa/crypto.h>
#include <psa/crypto_values.h>

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

#if MBEDTLS_VERSION_MAJOR >= 4

inline mbedtls_pk_type_t pk_get_type(const mbedtls_pk_context* ctx) {
    return mbedtls_pk_get_type(ctx);
}

/// Return whether an initialized context describes an ECC key pair.
inline bool pk_is_ec(const mbedtls_pk_context* ctx) {
    psa_key_type_t t = ctx->MBEDTLS_PRIVATE(psa_type);
    return PSA_KEY_TYPE_IS_ECC(t) && PSA_KEY_TYPE_IS_KEY_PAIR(t);
}

inline bool pk_is_rsa(const mbedtls_pk_context* ctx) {
    return PSA_KEY_TYPE_IS_RSA(ctx->MBEDTLS_PRIVATE(psa_type));
}

/// Return the stored PSA curve family; ctx must describe an EC key.
inline psa_ecc_family_t pk_ec_family(const mbedtls_pk_context* ctx) {
    return ctx->MBEDTLS_PRIVATE(ec_family);
}

inline size_t pk_key_bits(const mbedtls_pk_context* ctx) {
    return mbedtls_pk_get_bitlen(ctx);
}

/// Return "RSA", "EC" for an ECC key pair, or "None" for other key types.
inline const char* pk_type_name(const mbedtls_pk_context* ctx) {
    if (pk_is_rsa(ctx)) return "RSA";
    if (pk_is_ec(ctx)) return "EC";
    return "None";
}

#else

inline mbedtls_pk_type_t pk_get_type(const mbedtls_pk_context* ctx) {
    return mbedtls_pk_get_type(ctx);
}
/// Return whether an initialized context has an EC, EC-DH, or ECDSA key type.
inline bool pk_is_ec(const mbedtls_pk_context* ctx) {
    mbedtls_pk_type_t t = mbedtls_pk_get_type(ctx);
    return t == MBEDTLS_PK_ECKEY || t == MBEDTLS_PK_ECKEY_DH || t == MBEDTLS_PK_ECDSA;
}
inline bool pk_is_rsa(const mbedtls_pk_context* ctx) {
    return mbedtls_pk_get_type(ctx) == MBEDTLS_PK_RSA;
}
inline size_t pk_key_bits(const mbedtls_pk_context* ctx) {
    return mbedtls_pk_get_bitlen(ctx);
}
/// Return "RSA", "None" for an empty key, or "EC" for every other key type.
inline const char* pk_type_name(const mbedtls_pk_context* ctx) {
    mbedtls_pk_type_t t = mbedtls_pk_get_type(ctx);
    return t == MBEDTLS_PK_RSA ? "RSA" : (t == MBEDTLS_PK_NONE ? "None" : "EC");
}

#endif

} // namespace crypto
