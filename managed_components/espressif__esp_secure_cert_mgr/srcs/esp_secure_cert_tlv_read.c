/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_crc.h"
#include "inttypes.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
#include "esp_random.h"
#else
#include "esp_system.h"
#endif

#include "esp_rom_sys.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "soc/soc_caps.h"
#include "esp_fault.h"
#include "esp_heap_caps.h"

#include "mbedtls/gcm.h"
#include "mbedtls/sha256.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/pk.h"
#include "mbedtls/version.h"

#include "esp_secure_cert_read.h"
#include "esp_secure_cert_tlv_config.h"
#include "esp_secure_cert_tlv_read.h"
#include "esp_secure_cert_tlv_private.h"
#include "esp_secure_cert_crypto.h"

#if SOC_HMAC_SUPPORTED
#include "esp_hmac.h"
#endif

#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
/* mbedtls 2.x backward compatibility */
#define MBEDTLS_2X_COMPAT 1
/**
 * Mbedtls-3.0 forward compatibility
 */
#ifndef MBEDTLS_PRIVATE
#define MBEDTLS_PRIVATE(member) member
#endif
#endif /* (MBEDTLS_VERSION_NUMBER < 0x03000000) */

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "spi_flash_mmap.h"
#include "esp_memory_utils.h"
#include "entropy_poll.h"
#else
#include "mbedtls/entropy_poll.h"
#include "soc/soc_memory_layout.h"
#endif

#endif

static const char *TAG = "esp_secure_cert_tlv";

#define MIN_ALIGNMENT_REQUIRED 16


#if SOC_HMAC_SUPPORTED
static esp_err_t esp_secure_cert_hmac_based_decryption(char *in_buf, uint32_t len, char *output_buf);
static esp_err_t esp_secure_cert_gen_ecdsa_key(esp_secure_cert_tlv_subtype_t subtype, char *output_buf, size_t buf_len);
#define ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE  121
#endif

/* This is the mininum required flash address alignment in bytes to write to an encrypted flash partition */

/*
 * Map the entire esp_secure_cert partition
 * and return the virtual address.
 *
 * @note
 * The mapping is done only once and function shall
 * simply return same address in case of successive calls.
 **/
const void *esp_secure_cert_get_mapped_addr(void)
{
    // Once initialized, these variable shall contain valid data till reboot.
    static const void *esp_secure_cert_mapped_addr;
    if (esp_secure_cert_mapped_addr) {
        return esp_secure_cert_mapped_addr;
    }

    esp_partition_iterator_t it = esp_partition_find(ESP_SECURE_CERT_TLV_PARTITION_TYPE,
                                  ESP_PARTITION_SUBTYPE_ANY, ESP_SECURE_CERT_TLV_PARTITION_NAME);
    if (it == NULL) {
        ESP_LOGE(TAG, "Partition not found.");
        return NULL;
    }

    const esp_partition_t *partition = esp_partition_get(it);
    if (partition == NULL) {
        ESP_LOGE(TAG, "Could not get partition.");
        return NULL;
    }

    /* Encrypted partitions need to be read via a cache mapping */
    spi_flash_mmap_handle_t handle;
    esp_err_t err;

    /* Map the entire partition */
    err = esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &esp_secure_cert_mapped_addr, &handle);
    if (err != ESP_OK) {
        return NULL;
    }
    return esp_secure_cert_mapped_addr;
}

/*
 * Get the padding length applicable for the given TLV
 *
 * @input   The pointer to the start of the TLV (TLV Header)
 * @note    The padding length is defined in the design of the TLV.
 *          It says that each TLV data entry should be a multiple of MIN_ALIGNMENT_REQUIRED
 *          The actual tlv data is automatically padded to the nearest multiple of MIN_ALIGNMENT_REQUIRED
 */
static uint8_t esp_secure_cert_get_padding_length(esp_secure_cert_tlv_header_t *tlv_header)
{
    return ((MIN_ALIGNMENT_REQUIRED - (tlv_header->length % MIN_ALIGNMENT_REQUIRED)) % MIN_ALIGNMENT_REQUIRED);
}

/*
 * Get the total length of the TLV pointed by tlv_header
 * @input
 * tlv_header   The pointer to the start of the TLV (TLV Header)
 *
 * @note
 *          The total length of the TLV consists of following parts
 *          1) TLV Header
 *          2) TLV data
 *          3) Padding
 *          4) TLV footer
 *
 *          It is ensured by design that this length shall always be a multiple of MIN_ALIGNMENT_REQUIRED
 *
 */
static uint16_t esp_secure_cert_get_tlv_total_length(esp_secure_cert_tlv_header_t *tlv_header)
{
    uint16_t padding_length = esp_secure_cert_get_padding_length(tlv_header);
    uint16_t total_length = sizeof(esp_secure_cert_tlv_header_t) + tlv_header->length + padding_length + sizeof(esp_secure_cert_tlv_footer_t);
    return total_length;
}

/*
 * Verify the TLV integrity
 *
 * @input
 * tlv_header   The pointer to the start of the TLV (TLV header)
 *
 * @note
 *      This API calculates the crc value of header + tlv_data + padding
 *      This value is compared with the value stored in the TLV footer.
 *
 * @return
 *      True    TLV integrity verified
 *      False   TLV intefrity could not be verified
 */
static bool esp_secure_cert_verify_tlv_integrity(esp_secure_cert_tlv_header_t *tlv_header)
{
    ESP_LOGD(TAG, "Verifying the TLV integrity");
    if (!(tlv_header->magic == ESP_SECURE_CERT_TLV_MAGIC)) {
        return false;
    }

    uint8_t padding_length = esp_secure_cert_get_padding_length(tlv_header);
    ESP_LOGD(TAG, "Padding length obtained = %u", padding_length);
    size_t crc_data_len = sizeof(esp_secure_cert_tlv_header_t) + tlv_header->length + padding_length;
    uint32_t data_crc = esp_crc32_le(UINT32_MAX, (const uint8_t * )tlv_header, crc_data_len);
    esp_secure_cert_tlv_footer_t *tlv_footer = (esp_secure_cert_tlv_footer_t *)((void*)tlv_header + crc_data_len);
    if (tlv_footer->crc != data_crc) {
        ESP_LOGD(TAG, "Calculated crc = %04X does not match with crc"
                 " read from esp_secure_cert partition = %04X", (unsigned int)data_crc, (unsigned int)tlv_footer->crc);
        return false;
    }
    return true;
}

/*
 * Find the offset of tlv structure of given type in the esp_secure_cert partition
 *
 * Note: This API also validates the crc of the respective tlv before returning the offset
 * @input
 * esp_secure_cert_addr     Memory mapped address of the esp_secure_cert partition
 * type                     Type of the tlv structure.
 *                          for calculating current crc for esp_secure_cert
 * subtype                  Subtype of the given tlv structure. If subtype is given as ESP_SECURE_CERT_SUBTYPE_MAX then the entry with highest value of subtype is given as output
 *
 * tlv_address              Void pointer to store tlv address
 *
 */
esp_err_t esp_secure_cert_find_tlv(const void *esp_secure_cert_addr, esp_secure_cert_tlv_type_t type, uint8_t subtype, void **tlv_address)
{
    /* start from the begining of the partition */
    uint16_t tlv_offset = 0;
    uint8_t latest_subtype = 0;
    esp_secure_cert_tlv_header_t *latest_tlv_header = NULL;
    bool read_latest_tlv = 0;

    if (subtype == ESP_SECURE_CERT_SUBTYPE_MAX) {
        read_latest_tlv = 1;
    }

    while (1) {
        esp_secure_cert_tlv_header_t *tlv_header = (esp_secure_cert_tlv_header_t *)(esp_secure_cert_addr + tlv_offset);
        ESP_LOGD(TAG, "Reading from offset of %d from base of esp_secure_cert", tlv_offset);
        if (tlv_header->magic != ESP_SECURE_CERT_TLV_MAGIC) {
            if (read_latest_tlv) {
                if (latest_tlv_header != NULL) {
                    // Verifying the latest TLV here because we only need to verify the last TLV entry of given subtype
                    // instead of all available entries of the given subtype
                    ESP_LOGD(TAG, "Verify integrity of latest tlv obtained of subtype = %d", latest_subtype);
                    if (esp_secure_cert_verify_tlv_integrity(latest_tlv_header)) {
                        ESP_LOGD(TAG, "tlv structure of type %d and subtype %d found and verified", type, latest_subtype);
                        *tlv_address = (void *)latest_tlv_header;
                        return ESP_OK;
                    } else {
                        return ESP_FAIL;
                    }
                }
            }
            if (type == ESP_SECURE_CERT_TLV_END) {
                /* The invalid magic means last tlv read successfully was the last valid tlv structure present,
                 * so send the end address of the tlv.
                 * This address can be used to add a new tlv structure. */
                *tlv_address = (void *)tlv_header;
                return ESP_OK;
            }
            ESP_LOGD(TAG, "Unable to find tlv of type: %d", type);
            ESP_LOGD(TAG, "Expected magic byte is %04X, obtained magic byte = %04X", ESP_SECURE_CERT_TLV_MAGIC, (unsigned int) tlv_header->magic);
            return ESP_FAIL;
        }

        if (((esp_secure_cert_tlv_type_t)tlv_header->type) == type) {
            if (read_latest_tlv) {
                ESP_LOGD(TAG, "TLV entry of type: %d and subtype:%d found", tlv_header->type, tlv_header->subtype);
                ESP_LOGD(TAG, "Continuing to find more recent entry of given subtype");
                latest_subtype = tlv_header->subtype;
                // Store the tlv address of the latest entry found with given type
                latest_tlv_header = (void *)tlv_header;
                // Dont stop here and keep traversing till last TLV entry
            } else {
                if (tlv_header->subtype == subtype) {
                    if (esp_secure_cert_verify_tlv_integrity(tlv_header)) {
                        ESP_LOGD(TAG, "tlv structure of type %d and subtype %d found and verified", type, subtype);
                        *tlv_address = (void *)tlv_header;
                        return ESP_OK;
                    } else {
                        return ESP_FAIL;
                    }
                }
            }
        }
        // Move the offset to the start of the next tlv entry
        tlv_offset = tlv_offset + esp_secure_cert_get_tlv_total_length(tlv_header);
    }
}

/**

@brief Retrieve the header of a specific ESP Secure Certificate TLV record.
This function retrieves the header of a specific ESP Secure Certificate TLV record, identified by the type parameter. The tlv_header parameter is a pointer to a buffer where the header will be stored. The function returns ESP_OK if the header is successfully retrieved, otherwise it returns an error code.
@param[in] type The type of the TLV record to retrieve the header for.
@param[out] tlv_header The pointer at this location shall be updated with tlv_header pointer value.
@return ESP_OK          Success
        ESP_FAIL/
        relevant error  Failure
*/
static esp_err_t esp_secure_cert_tlv_get_header(esp_secure_cert_tlv_type_t type, uint8_t subtype, esp_secure_cert_tlv_header_t **tlv_header)
{
    esp_err_t err = ESP_FAIL;
    char *esp_secure_cert_addr = (char *)esp_secure_cert_get_mapped_addr();
    if (esp_secure_cert_addr == NULL) {
        ESP_LOGE(TAG, "Error in obtaining esp_secure_cert memory mapped address");
        return ESP_FAIL;
    }
    err = esp_secure_cert_find_tlv(esp_secure_cert_addr, type, subtype, (void **)tlv_header);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Could not find the tlv of type %d and subtype %d", type, subtype);
        return err;
    }
    return err;
}

esp_err_t esp_secure_cert_tlv_get_addr(esp_secure_cert_tlv_type_t type, esp_secure_cert_tlv_subtype_t subtype, char **buffer, uint32_t *len)
{
    esp_err_t err;
    esp_secure_cert_tlv_header_t *tlv_header = NULL;
    err = esp_secure_cert_tlv_get_header(type, (uint8_t)subtype, &tlv_header);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Could not find header for TLV type %d and subtype %d", type, subtype);
        return err;
    }

    // Special case : We return the end address of the last
    // valid TLV in this case
    // and length is set to the total length of valid TLV entries
    if (type == ESP_SECURE_CERT_TLV_END) {
        *buffer = (char *)tlv_header;
        const void *esp_secure_cert_addr = esp_secure_cert_get_mapped_addr();
        *len = (void*)tlv_header - esp_secure_cert_addr;
        return ESP_OK;
    }

    *buffer = (char *)&tlv_header->value;
    *len = tlv_header->length;

    if (ESP_SECURE_CERT_IS_TLV_ENCRYPTED(tlv_header->flags)) {
#if SOC_HMAC_SUPPORTED
        ESP_LOGD(TAG, "TLV data is encrypted");
        char *output_buf = (char *)heap_caps_calloc(1, sizeof(char) * (*len - HMAC_ENCRYPTION_TAG_LEN), MALLOC_CAP_INTERNAL);
        if (output_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory");
            return ESP_ERR_NO_MEM;
        }

        err = esp_secure_cert_hmac_based_decryption(*buffer, *len, output_buf);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to decrypt the data");
            free(output_buf);
            return err;
        }
        ESP_FAULT_ASSERT(err == ESP_OK);
        *buffer = output_buf;
        *len =  *len - HMAC_ENCRYPTION_TAG_LEN;
#else
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else if (ESP_SECURE_CERT_HMAC_ECDSA_KEY_DERIVATION(tlv_header->flags)) {
#if SOC_HMAC_SUPPORTED
        ESP_LOGD(TAG, "ECDSA private key shall be generated with help of HMAC");
        char *output_buf = (char *)heap_caps_calloc(1, sizeof(char) * (ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE), MALLOC_CAP_INTERNAL);
        if (output_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory");
            return ESP_ERR_NO_MEM;
        }
        err = esp_secure_cert_gen_ecdsa_key(subtype, output_buf, ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE);
        if (err != ESP_OK) {
            free(output_buf);
            ESP_LOGE(TAG, "Failed to generate ECDSA key, returned %04X", err);
            return ESP_FAIL;
        }
        ESP_FAULT_ASSERT(err == ESP_OK);
        *buffer = output_buf;
        *len = ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE;
#else
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else {
        ESP_LOGD(TAG, "TLV data is not encrypted");
    }
    return ESP_OK;
}

esp_err_t esp_secure_cert_get_tlv_info(esp_secure_cert_tlv_config_t *tlv_config, esp_secure_cert_tlv_info_t *tlv_info)
{
    if (tlv_config == NULL || tlv_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(tlv_info, 0, sizeof(esp_secure_cert_tlv_info_t));
    esp_err_t esp_ret;

    esp_secure_cert_tlv_header_t *tlv_header = NULL;

    // Try to obtain the TLV header
    esp_ret = esp_secure_cert_tlv_get_header(tlv_config->type, tlv_config->subtype, &tlv_header);
    if (esp_ret != ESP_OK) {
        return esp_ret;
    }

    // Try to obtain the TLV data
    esp_ret = esp_secure_cert_tlv_get_addr(tlv_config->type, tlv_config->subtype, &tlv_info->data, &tlv_info->length);
    if (esp_ret != ESP_OK) {
        tlv_info->data = NULL;
        tlv_info->length = 0;
        return esp_ret;
    }

    // Everything successfully obtained, copy to output tlv_info
    tlv_info->type = tlv_header->type;
    tlv_info->subtype = tlv_header->subtype;
    tlv_info->flags = tlv_header->flags;

    // Special case
    if (tlv_info->type == ESP_SECURE_CERT_TLV_END) {
        // In this case the user is requesting the end address
        // of the TLV entries, invalidate the subtype field
        // in the tlv_info struct
        tlv_info->subtype = ESP_SECURE_CERT_SUBTYPE_INVALID;
    }
    return esp_ret;
}

esp_err_t esp_secure_cert_free_tlv_info(esp_secure_cert_tlv_info_t *tlv_info)
{
    if (tlv_info) {
        if (!esp_ptr_in_drom((const void *) tlv_info->data)) {
            /* Free the buffer only if it is not from the drom section */
            free(tlv_info->data);
        }
    }
    return ESP_OK;
}

esp_err_t esp_secure_cert_iterate_to_next_tlv(esp_secure_cert_tlv_iterator_t *tlv_iterator)
{
    if (tlv_iterator == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_secure_cert_tlv_header_t *tlv_header = (esp_secure_cert_tlv_header_t *)tlv_iterator->iterator;
    if (tlv_header == NULL) {
        ESP_LOGD(TAG, "tlv_header value is NULL, finding the first TLV entry");
        tlv_header = (esp_secure_cert_tlv_header_t *) esp_secure_cert_get_mapped_addr();
        // Verify the TLV entry integrity before returning
        if (esp_secure_cert_verify_tlv_integrity(tlv_header)) {
            tlv_iterator->iterator = (void*)tlv_header;
            return ESP_OK;
        } else {
            ESP_LOGI(TAG, "The esp_secure_cert partition does not appear to be valid");
            return ESP_FAIL;
        }
    }

    // Verify the current TLV entry integrity
    if (!esp_secure_cert_verify_tlv_integrity(tlv_header)) {
        ESP_LOGE(TAG, "Invalid iterator value provided");
        return ESP_FAIL;
    }

    // Move to the next TLV entry
    tlv_header = ((void*) tlv_header + esp_secure_cert_get_tlv_total_length(tlv_header));
    if (esp_secure_cert_verify_tlv_integrity(tlv_header)) {
        // Verify the TLV entry integrity before returning
        tlv_iterator->iterator = (void*)tlv_header;
        return ESP_OK;
    }

    // We shall come here if the integrity verification
    // fails above, this means no more valid TLV entries
    ESP_LOGD(TAG, "Iterator is at the last TLV entry");
    // Keeping the log level as debug as this
    // is acceptable failure in some cases
    return ESP_FAIL;
}

esp_err_t esp_secure_cert_get_tlv_info_from_iterator(esp_secure_cert_tlv_iterator_t *tlv_iterator, esp_secure_cert_tlv_info_t *tlv_info)
{
    if (tlv_iterator == NULL || tlv_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_secure_cert_tlv_header_t *tlv_header = (esp_secure_cert_tlv_header_t*)tlv_iterator->iterator;
    if (!esp_secure_cert_verify_tlv_integrity(tlv_header)) {
        ESP_LOGE(TAG, "Iterator does not point to a valid TLV entry");
        return ESP_ERR_INVALID_ARG;
    }

    esp_secure_cert_tlv_config_t tlv_config = {};
    tlv_config.type = tlv_header->type;
    tlv_config.subtype = tlv_header->subtype;
    return esp_secure_cert_get_tlv_info(&tlv_config, tlv_info);
}

static void esp_secure_cert_print_tlv_info(esp_secure_cert_tlv_info_t *tlv_info)
{
    if (tlv_info != NULL) {
        ESP_LOGI(TAG, "Type: %d, Subtype: %d, Data length: %"PRIu32"", tlv_info->type, tlv_info->subtype, tlv_info->length);
        ESP_LOGI(TAG, "Flags: %X", tlv_info->flags);
    }
}

void esp_secure_cert_list_tlv_entries(void)
{
    esp_secure_cert_tlv_iterator_t tlv_iterator = {};
    size_t count = 0;
    while (esp_secure_cert_iterate_to_next_tlv(&tlv_iterator) == ESP_OK) {
        esp_secure_cert_tlv_info_t tlv_info;
        if (esp_secure_cert_get_tlv_info_from_iterator(&tlv_iterator, &tlv_info) == ESP_OK) {
            count++;
            printf("\n");
            ESP_LOGI(TAG, "TLV Entry No: %d", count);
            esp_secure_cert_print_tlv_info(&tlv_info);
        }
    }
}

#if SOC_HMAC_SUPPORTED
esp_err_t esp_secure_cert_calculate_hmac_encryption_iv(uint8_t *iv)
{
    if (iv == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    const uint32_t iv_message[HMAC_ENCRYPTION_MESSAGE_LEN / 4] = {[0 ... 7] = 0xABCDABCD};
    esp_err_t esp_ret = ESP_FAIL;
    esp_efuse_block_t efuse_block = EFUSE_BLK_MAX;
    if (!esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_HMAC_UP, &efuse_block)) {
        ESP_LOGE(TAG, "Failed to get the block with purpose set to HMAC_UP");
        return ESP_FAIL;;
    }

    hmac_key_id_t hmac_key_id = efuse_block - (int)EFUSE_BLK_KEY0;
    uint8_t hmac[HMAC_ENCRYPTION_AES_GCM_KEY_LEN];
    esp_ret = esp_hmac_calculate(hmac_key_id, (uint8_t *)iv_message, HMAC_ENCRYPTION_MESSAGE_LEN, hmac);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Could not calculate the HMAC value, returned %04X", esp_ret);
        ESP_LOGE(TAG, "returned %04X", esp_ret);
        return esp_ret;
    }
    memcpy(iv, hmac, HMAC_ENCRYPTION_IV_LEN);
    return ESP_OK;
}

esp_err_t esp_secure_cert_calculate_hmac_encryption_key(uint8_t *aes_key)
{
    if (aes_key == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    const uint32_t key_message[HMAC_ENCRYPTION_MESSAGE_LEN / 4] = {[0 ... 7] = 0xFFFFFFFF};
    esp_err_t esp_ret = ESP_FAIL;
    esp_efuse_block_t efuse_block = EFUSE_BLK_MAX;
    if (!esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_HMAC_UP, &efuse_block)) {
        ESP_LOGE(TAG, "Failed to get the block with purpose set to HMAC_UP");
        return ESP_FAIL;;
    }

    hmac_key_id_t hmac_key_id = efuse_block - (int)EFUSE_BLK_KEY0;
    esp_ret = esp_hmac_calculate(hmac_key_id, (uint8_t *)key_message, HMAC_ENCRYPTION_MESSAGE_LEN, aes_key);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Could not calculate the HMAC value, returned %04X", esp_ret);
        return esp_ret;
    }
    return ESP_OK;
}

#define HMAC_ENCRYPTION_RANDOM_DELAY_LIMIT 100
/*
 * @info
 * Decrypt the data encrypted using hmac based encryption
 *
 * in_buf       This is a pointer in flash that points to the encrypted data
 * len          Length of the encrypted data
 * output_buf   Buffer to write the decrypted data
 *              This must be of size = len - HMAC_ENCRYPTION_TAG_LEN
 */
static esp_err_t esp_secure_cert_hmac_based_decryption(char *in_buf, uint32_t len, char *output_buf)
{
    esp_err_t esp_ret = ESP_FAIL;
    int ret = -1;
    uint8_t aes_gcm_key[HMAC_ENCRYPTION_AES_GCM_KEY_LEN];
    uint8_t iv[HMAC_ENCRYPTION_IV_LEN];

    esp_ret = esp_secure_cert_calculate_hmac_encryption_iv(iv);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to calculate hmac encryption iv");
        return esp_ret;
    }
    esp_ret = esp_secure_cert_calculate_hmac_encryption_key(aes_gcm_key);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to calculate hmac encryption key");
        return esp_ret;
    }

    mbedtls_gcm_context gcm_ctx;
    mbedtls_gcm_init(&gcm_ctx);
    ret = mbedtls_gcm_setkey(&gcm_ctx, MBEDTLS_CIPHER_ID_AES, (unsigned char *)aes_gcm_key, HMAC_ENCRYPTION_AES_GCM_KEY_LEN * 8);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failure at mbedtls_gcm_setkey with error code : -0x%04X", -ret);
        mbedtls_gcm_free(&gcm_ctx);
        return ESP_FAIL;
    }

    uint32_t rand_delay;
    rand_delay = esp_random() % HMAC_ENCRYPTION_RANDOM_DELAY_LIMIT;
    esp_rom_delay_us(rand_delay);

    len = len - HMAC_ENCRYPTION_TAG_LEN;
    ret = mbedtls_gcm_auth_decrypt(&gcm_ctx, len, iv,
                                   HMAC_ENCRYPTION_IV_LEN, NULL, 0,
                                   (unsigned char *) (in_buf + len),
                                   HMAC_ENCRYPTION_TAG_LEN,
                                   (const unsigned char *)(in_buf),
                                   (unsigned char *)output_buf);

    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to decrypt the data, mbedtls_gcm_crypt_and_tag returned %02X", ret);
        mbedtls_gcm_free(&gcm_ctx);
        return ESP_FAIL;
    }

    ESP_FAULT_ASSERT(ret == ESP_OK);

    return ESP_OK;
}

static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t olen;
    (void) olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

/*
 * The API converts the 256 bit ECDSA key to DER format.
 * @input
 * key_buf      The readable buffer containing the plaintext key
 * key_buf_len  The length of the key buf in bytes
 * output_buf   The writable buffer to write the DER key
 * output_buf_len Length of the output buffer
 *
 */
static esp_err_t esp_secure_cert_convert_key_to_der(char *key_buf, size_t key_buf_len, char* output_buf, size_t output_buf_len)
{
    esp_err_t ret = ESP_FAIL;
    // Convert the private key to der
    mbedtls_pk_context key;
    mbedtls_pk_init(&key);
    ret = mbedtls_pk_setup(&key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to setup pk key, returned %04X", ret);
        goto exit;
    }

    mbedtls_ecdsa_context *key_ctx = mbedtls_pk_ec(key);
    ret = mbedtls_ecp_group_load(&key_ctx->MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to load the ecp group, returned %04X", ret);
        goto exit;
    }

    ret = mbedtls_mpi_read_binary(&key_ctx->MBEDTLS_PRIVATE(d), (const unsigned char *) key_buf, key_buf_len);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to read binary, returned %04X", ret);
        goto exit;
    }

    // Calculate the public key
    ret = mbedtls_ecp_mul(&key_ctx->MBEDTLS_PRIVATE(grp), &key_ctx->MBEDTLS_PRIVATE(Q), &key_ctx->MBEDTLS_PRIVATE(d), &key_ctx->MBEDTLS_PRIVATE(grp).G, myrand, NULL);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to generate public key, returned %04X", ret);
        goto exit;
    }

    // Write the private key in DER format
    ret = mbedtls_pk_write_key_der(&key, (unsigned char *) output_buf, output_buf_len);
    if (ret != ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE) {
        ESP_LOGE(TAG, "Failed to write the pem key, returned %04X", ret);
        goto exit;
    }
    ret = ESP_OK;

exit:
    mbedtls_pk_free(&key);
    return ret;
}
/*
 * @info
 * Generate the ECDSA private key (DER format) with help of the PBKDF2 hmac implementation.
 * In this case the first eFuse key block with purpose set to HMAC_UP shall be used for generating the private key.
 * The key shall be generated for the SECP256R1 curve, the length of the key in DER format shall be ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE bytes.
 * The API assumes that a valid salt is stored in the esp_secure_cert partition, Otherwise the API would return with failure.
 *
 * @input
 * output_buf  A writable buffer to store the DER formatted ECDSA private key
 * buf_len     The length of the buffer in bytes. This must be exactly ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE bytes.
 *
 */
static esp_err_t esp_secure_cert_gen_ecdsa_key(esp_secure_cert_tlv_subtype_t subtype, char *output_buf, size_t buf_len)
{
    esp_err_t err = ESP_FAIL;
    int ret = 0;
    if (buf_len != ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }

    // Obtain the salt stored in the esp_secure_cert partition
    uint8_t *salt = NULL;
    uint32_t salt_len = 0;
    err = esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_HMAC_ECDSA_KEY_SALT, subtype, (void *)&salt, &salt_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error in reading salt, returned %04X", err);
    }
    ESP_FAULT_ASSERT(err == ESP_OK);

    ESP_LOG_BUFFER_HEX_LEVEL("SALT", salt, salt_len, ESP_LOG_DEBUG);

    esp_efuse_block_t efuse_block = EFUSE_BLK_KEY_MAX;
    bool res = esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_HMAC_UP, &efuse_block);
    if (!res) {
        ESP_LOGE(TAG, "Failed to get the block with purpose set to HMAC_UP");
        return ESP_FAIL;
    }
    ESP_FAULT_ASSERT(res);

    // Allocate memory for private key
    char *key_buf = (char *)heap_caps_calloc(1, sizeof(char) * (ESP_SECURE_CERT_DERIVED_ECDSA_KEY_SIZE), MALLOC_CAP_INTERNAL);
    if (key_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return ESP_ERR_NO_MEM;
    }

    // Generate the private key
    ret = esp_pbkdf2_hmac_sha256(efuse_block - (int)EFUSE_BLK_KEY0, salt, salt_len, ESP_SECURE_CERT_KEY_DERIVATION_ITERATION_COUNT, ESP_SECURE_CERT_DERIVED_ECDSA_KEY_SIZE, (unsigned char *)key_buf);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to derive the ECDSA key using HMAC, returned %04X", ret);
        free(key_buf);
        return ESP_FAIL;
    }
    ESP_FAULT_ASSERT(ret == 0);

    err = esp_secure_cert_convert_key_to_der(key_buf, ESP_SECURE_CERT_DERIVED_ECDSA_KEY_SIZE, output_buf, ESP_SECURE_CERT_ECDSA_DER_KEY_SIZE);
    if (err != ESP_OK) {
        free(key_buf);
        free(output_buf);
        ESP_LOGE(TAG, "Failed to convert the plaintext key to DER format");
        return ESP_FAIL;
    }
    // Free the plaintext private key as it is no longer needed
    free(key_buf);
    return ESP_OK;
}
#endif

#ifdef CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL
esp_ds_data_ctx_t *esp_secure_cert_tlv_get_ds_ctx(void)
{
    esp_err_t esp_ret;
    esp_ds_data_ctx_t *ds_data_ctx;

    ds_data_ctx = (esp_ds_data_ctx_t *)calloc(1, sizeof(esp_ds_data_ctx_t));
    if (ds_data_ctx == NULL) {
        ESP_LOGE(TAG, "Error in allocating memory for esp_ds_data_context");
        goto exit;
    }

    uint32_t len;
    esp_ds_data_t *esp_ds_data;
    esp_ret = esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_DS_DATA_TLV, ESP_SECURE_CERT_SUBTYPE_0, (void *) &esp_ds_data, &len);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error in reading ds_data, returned %04X", esp_ret);
        goto exit;
    }

    esp_ds_data_ctx_t *ds_data_ctx_flash;
    esp_ret = esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_DS_CONTEXT_TLV, 0, (void *) &ds_data_ctx_flash, &len);
    memcpy(ds_data_ctx, ds_data_ctx_flash, len);
    ds_data_ctx->esp_ds_data = esp_ds_data;
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error in reading ds_context, returned %04X", esp_ret);
        goto exit;
    }
    return ds_data_ctx;
exit:
    free(ds_data_ctx);
    return NULL;
}

void esp_secure_cert_tlv_free_ds_ctx(esp_ds_data_ctx_t *ds_ctx)
{
    free(ds_ctx);
}
#endif /* CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL */

bool esp_secure_cert_is_tlv_partition(void)
{
    char *esp_secure_cert_addr = (char *)esp_secure_cert_get_mapped_addr();
    if (esp_secure_cert_addr == NULL) {
        return 0;
    }
    esp_secure_cert_tlv_header_t *tlv_header = (esp_secure_cert_tlv_header_t *)(esp_secure_cert_addr );
    if (tlv_header->magic == ESP_SECURE_CERT_TLV_MAGIC) {
        ESP_LOGD(TAG, "TLV partition identified");
        return 1;
    }
    return 0;
}

#ifndef CONFIG_ESP_SECURE_CERT_SUPPORT_LEGACY_FORMATS
esp_err_t esp_secure_cert_get_device_cert(char **buffer, uint32_t *len)
{
    return esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_DEV_CERT_TLV, ESP_SECURE_CERT_SUBTYPE_0, buffer, len);
}

esp_err_t esp_secure_cert_free_device_cert(char *buffer)
{
    (void) buffer; /* nothing to do */
    return ESP_OK;
}

esp_err_t esp_secure_cert_get_ca_cert(char **buffer, uint32_t *len)
{
    return esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_CA_CERT_TLV, ESP_SECURE_CERT_SUBTYPE_0, buffer, len);
}

esp_err_t esp_secure_cert_free_ca_cert(char *buffer)
{
    (void) buffer; /* nothing to do */
    return ESP_OK;
}

#ifndef CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL
esp_err_t esp_secure_cert_get_priv_key(char **buffer, uint32_t *len)
{
    return esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_PRIV_KEY_TLV, ESP_SECURE_CERT_SUBTYPE_0, buffer, len);
}

esp_err_t esp_secure_cert_free_priv_key(char *buffer)
{
    if (!esp_ptr_in_drom((const void *) buffer)) {
        free(buffer);
        return ESP_OK;
    }
    (void) buffer; /* nothing to do */
    return ESP_OK;
}

#else /* !CONFIG_ESP_SECURE_CERT_DS_PEIPHERAL */

esp_ds_data_ctx_t *esp_secure_cert_get_ds_ctx(void)
{
    return esp_secure_cert_tlv_get_ds_ctx();
}

void esp_secure_cert_free_ds_ctx(esp_ds_data_ctx_t *ds_ctx)
{
    esp_secure_cert_tlv_free_ds_ctx(ds_ctx);
}
#endif /* CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL */

esp_err_t esp_secure_cert_get_priv_key_type(esp_secure_cert_key_type_t *priv_key_type) {
    esp_err_t err;
    if (priv_key_type == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_secure_cert_tlv_header_t *tlv_header = NULL;
    err = esp_secure_cert_tlv_get_header(ESP_SECURE_CERT_PRIV_KEY_TLV, 0, &tlv_header);
    if (err != ESP_OK) {
        *priv_key_type = ESP_SECURE_CERT_INVALID_KEY;
        ESP_LOGE(TAG, "Could not find header for priv key");
        return err;
    }

    if (ESP_SECURE_CERT_HMAC_ECDSA_KEY_DERIVATION(tlv_header->flags)) {
        *priv_key_type = ESP_SECURE_CERT_HMAC_DERIVED_ECDSA_KEY;
    } else if (ESP_SECURE_CERT_KEY_ECDSA_PERIPHERAL(tlv_header->flags)) {
        *priv_key_type = ESP_SECURE_CERT_ECDSA_PERIPHERAL_KEY;
    } else if (ESP_SECURE_CERT_IS_TLV_ENCRYPTED(tlv_header->flags)) {
        *priv_key_type = ESP_SECURE_CERT_HMAC_ENCRYPTED_KEY;
    } else {
        *priv_key_type = ESP_SECURE_CERT_DEFAULT_FORMAT_KEY;
    }
    return ESP_OK;
}

esp_err_t esp_secure_cert_get_priv_key_efuse_id(uint8_t *efuse_block_id) {
    esp_err_t err;
    esp_secure_cert_tlv_header_t *tlv_header = NULL;
    if (efuse_block_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = esp_secure_cert_tlv_get_header(ESP_SECURE_CERT_TLV_SEC_CFG, 0,  &tlv_header);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not find header for TLV security configurations");
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_secure_cert_tlv_sec_cfg_t *tlv_sec_cfg;
    tlv_sec_cfg = (esp_secure_cert_tlv_sec_cfg_t *) tlv_header->value;
    *efuse_block_id = tlv_sec_cfg->priv_key_efuse_id;

    return ESP_OK;
}
#endif /* CONFIG_ESP_SECURE_CERT_SUPPORT_LEGACY_FORMATS */
