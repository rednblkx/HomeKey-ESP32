/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/soc_caps.h"

#if SOC_HMAC_SUPPORTED
static const char *TAG = "esp_secure_cert_crypto";

#include "esp_hmac.h"
#define SHA256_MD_SIZE 32
int esp_pbkdf2_hmac_sha256(hmac_key_id_t hmac_key_id, const unsigned char *salt, size_t salt_len,
                           size_t iteration_count, size_t key_length, unsigned char *output)
{
    int ret = -1;
    int j;
    unsigned int i;
    unsigned char md1[SHA256_MD_SIZE] = {};
    unsigned char work[SHA256_MD_SIZE] = {};
    // Considering that we only have SHA256, fixing the MD_SIZE to 32 bytes
    const size_t MD_SIZE = SHA256_MD_SIZE;
    size_t use_len;
    unsigned char *out_p = output;
    unsigned char counter[4] = {};

    counter[3] = 1;
    uint8_t *hmac_input;
    esp_err_t esp_ret = ESP_FAIL;
    hmac_input = (uint8_t *) heap_caps_calloc(1, salt_len + sizeof(counter) + 1, MALLOC_CAP_INTERNAL);
    if (hmac_input == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for hmac input");
        return -1;
    }

    while (key_length) {
        // U1 ends up in work
        size_t hmac_input_len = 0;
        memcpy(hmac_input, salt, salt_len);
        hmac_input_len = hmac_input_len + salt_len;
        memcpy(hmac_input + salt_len, counter, sizeof(counter));
        hmac_input_len = hmac_input_len + sizeof(counter);
        esp_ret = esp_hmac_calculate(hmac_key_id, hmac_input, hmac_input_len, work);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Could not calculate the HMAC value, returned %04X", esp_ret);
            ret = -1;
            goto cleanup;
        }

        memcpy(md1, work, MD_SIZE);

        for (i = 1; i < iteration_count; i++) {
            // U2 ends up in md1
            esp_ret = esp_hmac_calculate(hmac_key_id, md1, MD_SIZE, md1);
            if (esp_ret != ESP_OK) {
                ESP_LOGE(TAG, "Could not calculate the HMAC value, returned %04X", esp_ret);
                ret = -1;
                goto cleanup;
            }
            // U1 xor U2
            //
            for (j = 0; j < MD_SIZE; j++) {
                work[j] ^= md1[j];
            }
        }

        use_len = (key_length < MD_SIZE) ? key_length : MD_SIZE;
        memcpy(out_p, work, use_len);

        key_length -= (uint32_t) use_len;
        out_p += use_len;

        for (i = 4; i > 0; i--) {
            if (++counter[i - 1] != 0) {
                break;
            }
        }
    }
    //Success
    ret = 0;
cleanup:

    /* Zeroise buffers to clear sensitive data from memory. */
    free(hmac_input);
    memset(work, 0, SHA256_MD_SIZE);
    memset(md1, 0, SHA256_MD_SIZE);
    return ret;
}
#endif
