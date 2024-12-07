/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <sys/time.h>
#include <esp_log.h>
#include <esp_idf_version.h>
#include <esp_rmaker_utils.h>
#include "esp_rmaker_secure_boot_digest.h"

static const char *TAG = "rmaker_secure_boot";

#ifdef CONFIG_SECURE_BOOT_V2_ENABLED

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

// Hex representation of secure boot digest. +1 for NULL termination
#define SECURE_BOOT_DIGEST_LEN (ESP_SECURE_BOOT_DIGEST_LEN * 2 + 1)

esp_err_t esp_rmaker_secure_boot_digest_free(char **digest)
{
    // cleanup
    for(int i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        if (digest[i]) {
            free(digest[i]);
        }
    }
    free(digest);
    return ESP_OK;
}

char** esp_rmaker_get_secure_boot_digest()
{
    char **secure_boot_digest = NULL;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_secure_boot_key_digests_t trusted_keys;
#else
    ets_secure_boot_key_digests_t trusted_keys;
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
    esp_err_t ret = esp_secure_boot_read_key_digests(&trusted_keys);
#else
    esp_err_t ret = ets_secure_boot_read_key_digests(&trusted_keys);
#endif
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Could not read the secure boot key digests from efuse.");
        return NULL;
    }
    secure_boot_digest = MEM_ALLOC_EXTRAM(SECURE_BOOT_NUM_BLOCKS * sizeof(char *));
    if (!secure_boot_digest) {
        ESP_LOGE(TAG, "Failed to allocate memory for secure boot digest");
        return NULL;
    }
    for(int i = 0; i < SECURE_BOOT_NUM_BLOCKS; i++) {
        secure_boot_digest[i] = NULL;
        if (trusted_keys.key_digests[i] != NULL) {
            secure_boot_digest[i] = MEM_ALLOC_EXTRAM(SECURE_BOOT_DIGEST_LEN);
            if (!secure_boot_digest[i]) {
                ESP_LOGE(TAG, "Failed to allocate memory for secure boot digest");
                for (int k = 0; k < i; k++) {
                    free(secure_boot_digest[k]);
                    secure_boot_digest[k] = NULL;
                }
                free(secure_boot_digest);
                secure_boot_digest = NULL;
                break;
            }
            bytes_to_hex((uint8_t *)trusted_keys.key_digests[i], (uint8_t *)secure_boot_digest[i], ESP_SECURE_BOOT_DIGEST_LEN);
        }
    }
    return secure_boot_digest;
}

#else /* CONFIG_SECURE_BOOT_V2_ENABLED */

esp_err_t esp_rmaker_secure_boot_digest_free(char **digest)
{
    (void) digest;
    return ESP_OK;
}

char** esp_rmaker_get_secure_boot_digest()
{
    ESP_LOGI(TAG, "Secure boot is not enabled. Could not get digest.");
    return NULL;
}

#endif
