/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <esp_idf_version.h>
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
#include <string.h>
#include <esp_log.h>
#include "mbedtls/platform.h"
#include "mbedtls/pk.h"
#include "mbedtls/sha256.h"
#include "mbedtls/ecdsa.h"
#include "sha/sha_parallel_engine.h"

#include "esp_secure_cert_read.h"
#include "esp_rmaker_utils.h"
#include "esp_rmaker_client_data.h"

static const char *TAG = "esp_rmaker_user_node_auth";

static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    esp_fill_random(output, len);
    return 0;
}

static inline uint8_t to_hex_digit(unsigned val)
{
    return (val < 10) ? ('0' + val) : ('a' + val - 10);
}

static void bytes_to_hex(uint8_t *src, uint8_t *dst, int in_len)
{
    for (int i = 0; i < in_len; i++) {
        dst[2 * i] = to_hex_digit(src[i] >> 4);
        dst[2 * i + 1] = to_hex_digit(src[i] & 0xf);
    }
    dst[2 * in_len] = 0;
}

esp_err_t esp_rmaker_node_auth_sign_msg(const void *challenge, size_t inlen, void **response, size_t *outlen)
{
    if (!challenge || (inlen == 0)) {
        ESP_LOGE(TAG, "function arguments challenge and inlen cannot be NULL.");
        return ESP_ERR_INVALID_ARG;
    }
    
    /* Get private key */
    char *priv_key = NULL;
    size_t priv_key_len = 0;
    esp_secure_cert_key_type_t key_type;
    esp_err_t err = esp_secure_cert_get_priv_key_type(&key_type);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get the type of private key from secure cert partition, err:%d", err);
        return err;
    }
    if (key_type == ESP_SECURE_CERT_INVALID_KEY) {
        ESP_LOGE(TAG, "Private key type in secure cert partition is invalid");
        return ESP_FAIL;
    }
    /* This flow is for devices supporting ECDSA peripheral */
    if (key_type == ESP_SECURE_CERT_ECDSA_PERIPHERAL_KEY) {
#if CONFIG_USE_ESP32_ECDSA_PERIPHERAL
        /* TODO: code for signing the challenge on devices that have a DS peripheral. */
        return ESP_FAIL;
#else  /* !CONFIG_USE_ESP32_ECDSA_PERIPHERAL */
        return ESP_ERR_INVALID_STATE;
#endif /* CONFIG_USE_ESP32_ECDSA_PERIPHERAL */
    } else {
    /* This flow is for devices which do not support ECDSA peripheral */
#if !CONFIG_USE_ESP32_ECDSA_PERIPHERAL
        priv_key = esp_rmaker_get_client_key();
        priv_key_len = esp_rmaker_get_client_key_len();
#else /* CONFIG_USE_ESP32_ECDSA_PERIPHERAL */
        return ESP_ERR_INVALID_STATE;
#endif /* !CONFIG_USE_ESP32_ECDSA_PERIPHERAL */
    }
    if (!priv_key) {
        ESP_LOGE(TAG, "Error getting private key");
        return ESP_FAIL;
    }
    /* Calculate SHA of challenge */
    uint8_t hash[32];
    esp_sha(SHA2_256,(const unsigned char *)challenge, strlen((char *)challenge), hash);

    /* Sign the hash using ECDSA */
    mbedtls_pk_context pk_ctx;
    mbedtls_pk_init(&pk_ctx);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    int ret = mbedtls_pk_parse_key(&pk_ctx, (uint8_t *)priv_key, priv_key_len, NULL, 0, NULL, 0);
#else /* !(ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) */
    int ret = mbedtls_pk_parse_key(&pk_ctx, (uint8_t *)priv_key, priv_key_len, NULL, 0);
#endif /* ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0) */
    uint8_t *signature = (uint8_t *)MEM_CALLOC_EXTRAM(1, MBEDTLS_ECDSA_MAX_LEN);
    if (!signature) {
        ESP_LOGE(TAG, "Failed to allocate memory to signature.");
        mbedtls_pk_free(&pk_ctx);
        return ESP_ERR_NO_MEM;
    }
    size_t slen = 0;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    ret = mbedtls_ecdsa_write_signature(mbedtls_pk_ec(pk_ctx), MBEDTLS_MD_SHA256, hash, sizeof(hash), signature, MBEDTLS_ECDSA_MAX_LEN, &slen, myrand, NULL);
#else /* !(ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) */
    ret = mbedtls_ecdsa_write_signature(mbedtls_pk_ec(pk_ctx), MBEDTLS_MD_SHA256, hash, sizeof(hash), signature, &slen, myrand, NULL);
#endif /* (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)) */
    if (ret != 0) {
       ESP_LOGE(TAG, "Error in writing signature. err = %d", ret);
       free(signature);
       mbedtls_pk_free(&pk_ctx);
       return ESP_FAIL;
    }

    /* Convert hex stream to bytes */
#define BYTE_ENCODED_SIGNATURE_LEN ((2 * slen) + 1) /* +1 for null character */
    char *char_signature = (char *)MEM_ALLOC_EXTRAM(BYTE_ENCODED_SIGNATURE_LEN);
    if (!char_signature) {
        ESP_LOGE(TAG, "Error in allocating memory for challenge response.");
        free(signature);
        mbedtls_pk_free(&pk_ctx);
        return ESP_ERR_NO_MEM;
    }
    bytes_to_hex(signature, (uint8_t *)char_signature, slen);
    mbedtls_pk_free(&pk_ctx);
    free(signature);
    /* Set output variables */
    *(char **)response = char_signature;
    *outlen = slen;
    return ESP_OK;
}
#endif /* ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0) */
