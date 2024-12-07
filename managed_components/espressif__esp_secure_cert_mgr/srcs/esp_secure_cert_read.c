/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_crc.h"
#include "nvs_flash.h"
#include "esp_secure_cert_read.h"
#include "esp_secure_cert_config.h"
#include "esp_secure_cert_tlv_config.h"
#include "esp_secure_cert_tlv_private.h"
#include "esp_heap_caps.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include "spi_flash_mmap.h"
#endif
#endif

static const char *TAG = "esp_secure_cert";

typedef enum partition_format {
    ESP_SECURE_CERT_PF_INVALID = -1,
    ESP_SECURE_CERT_PF_CUST_FLASH,
    ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY,
    ESP_SECURE_CERT_PF_NVS,
    ESP_SECURE_CERT_PF_TLV,
} esp_secure_cert_part_fmt_t;

static esp_secure_cert_part_fmt_t current_partition_format = ESP_SECURE_CERT_PF_INVALID;
static const char *nvs_partition_name;
static const char *nvs_namespace_name;

// In case of esp_secure_cert nvs namespace is same as nvs partition name

#define NVS_STR         1
#define NVS_BLOB        2
#define NVS_U8          3
#define NVS_U16         4

/* API to find the esp_secure_cert partition
 * @params
 *  partition_type      The type of partition to find
 *  partition_name      The name of the partition to find
 *  partition_format    The partition_format of the partition,
 *                      The global variable current_partition_format shall
 *                      be set to this value if find partition operation
 *                      was successful
 */
static const esp_partition_t *esp_secure_cert_find_partition(esp_partition_type_t partition_type, const char *partition_name, esp_secure_cert_part_fmt_t format)
{
    esp_partition_iterator_t it = esp_partition_find(partition_type, ESP_PARTITION_SUBTYPE_ANY, partition_name);
    if (it != NULL) {
        current_partition_format = format;
        const esp_partition_t *part = esp_partition_get(it);
        if (part == NULL) {
            ESP_LOGE(TAG, "Failed to get partition");
            return NULL;
        } else {
            return part;
        }
    }
    // if it reaches here that means it is NULL
    return NULL;
}

static const esp_partition_t *esp_secure_cert_get_partition(void)
{
    static const esp_partition_t *part;
    if (part != NULL) {
        return part;
    }

    // This switch case statement is added for increasing readability
    // The actual behaviour is a simple fall through
    current_partition_format = ESP_SECURE_CERT_PF_TLV;

    switch (current_partition_format) {
    case ESP_SECURE_CERT_PF_TLV:
        part = esp_secure_cert_find_partition(ESP_SECURE_CERT_CUST_FLASH_PARTITION_TYPE, ESP_SECURE_CERT_TLV_PARTITION_NAME, ESP_SECURE_CERT_PF_TLV);
        if (part != NULL) {
            if (esp_secure_cert_is_tlv_partition()) {
                ESP_LOGI(TAG, "Pre-provisioned partition information:");
                ESP_LOGI(TAG, "partition format: %s, partition name: %s", "CUST_FLASH_TLV", ESP_SECURE_CERT_PARTITION_NAME);
                return part;
            }
        }
    // fall through
    case ESP_SECURE_CERT_PF_CUST_FLASH:
        part = esp_secure_cert_find_partition(ESP_SECURE_CERT_CUST_FLASH_PARTITION_TYPE, ESP_SECURE_CERT_PARTITION_NAME, ESP_SECURE_CERT_PF_CUST_FLASH);
        if (part != NULL) {
            ESP_LOGI(TAG, "Pre-provisioned partition information:");
            ESP_LOGI(TAG, "partition format: %s, partition name: %s", "CUST_FLASH", ESP_SECURE_CERT_PARTITION_NAME);
            return part;
        }

    // fall through
    case ESP_SECURE_CERT_PF_NVS:
        nvs_partition_name = ESP_SECURE_CERT_PARTITION_NAME;
        nvs_namespace_name = ESP_SECURE_CERT_PARTITION_NAME;
        part = esp_secure_cert_find_partition(ESP_SECURE_CERT_NVS_PARTITION_TYPE, nvs_partition_name, ESP_SECURE_CERT_PF_NVS);

        if (part != NULL) {
            if (nvs_flash_init_partition(nvs_partition_name) != ESP_OK) {
                ESP_LOGE(TAG, "NVS partition %s was found but inititlization failed", nvs_partition_name);
                return NULL;
            }
            ESP_LOGD(TAG, "Partition found, current format is NVS.");
            ESP_LOGD(TAG, "NVS partition %s is inititalized", nvs_partition_name);
            esp_err_t err;
            nvs_handle_t handle;

            err = nvs_open_from_partition(nvs_partition_name, nvs_namespace_name, NVS_READONLY, &handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Pre-provisioned partition information:");
                ESP_LOGI(TAG, "partition format: %s, partition name: %s, partition namespace: %s", "NVS", nvs_partition_name, nvs_namespace_name);
                nvs_close(handle);
                return part;
            }

            nvs_namespace_name = ESP_SECURE_CERT_LEGACY_PARTITION_NAME;
            err = nvs_open_from_partition(nvs_partition_name, nvs_namespace_name, NVS_READONLY, &handle);
            nvs_close(handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Pre-provisioned partition information:");
                ESP_LOGI(TAG, "partition format: %s, partition name: %s, partition namespace: %s", "NVS", nvs_partition_name, nvs_namespace_name);
                return part;
            } else {
                ESP_LOGE(TAG, "Failed to open nvs partition %s with namespace: %s", nvs_partition_name, nvs_namespace_name);
                return NULL;
            }
        }

        esp_err_t err;
        nvs_handle_t handle;
        nvs_partition_name = ESP_SECURE_CERT_LEGACY_PARTITION_NAME;
        nvs_namespace_name = ESP_SECURE_CERT_LEGACY_PARTITION_NAME;
        part = esp_secure_cert_find_partition(ESP_SECURE_CERT_NVS_PARTITION_TYPE, nvs_partition_name, ESP_SECURE_CERT_PF_NVS);
        if (part != NULL) {
            if (nvs_flash_init_partition(nvs_partition_name) != ESP_OK) {
                ESP_LOGE(TAG, "NVS partition %s was found but inititlization failed", nvs_partition_name);
                return NULL;
            }
            err = nvs_open_from_partition(nvs_partition_name, nvs_namespace_name, NVS_READONLY, &handle);
            nvs_close(handle);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Pre-provisioned partition information:");
                ESP_LOGI(TAG, "partition format: %s, partition name: %s, partition namespace: %s", "NVS", nvs_partition_name, nvs_namespace_name);
                return part;
            } else {
                ESP_LOGE(TAG, "Failed to open nvs partition %s with namespace: %s", nvs_partition_name, nvs_namespace_name);
                return NULL;
            }
        }

    // fall through
    case ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY:
        part = esp_secure_cert_find_partition(ESP_SECURE_CERT_CUST_FLASH_PARTITION_TYPE, ESP_SECURE_CERT_LEGACY_PARTITION_NAME, ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY);
        if (part != NULL) {
            ESP_LOGI(TAG, "Partition found, current format is cust flash legacy.");
            ESP_LOGI(TAG, "Current partition format: %s, partition name: %s", "CUST_FLASH_LEGACY", ESP_SECURE_CERT_LEGACY_PARTITION_NAME);
            return part;
        }

    // fall through
    case ESP_SECURE_CERT_PF_INVALID:
    default:
        current_partition_format = ESP_SECURE_CERT_PF_INVALID;
        ESP_LOGE(TAG, "Failed to get esp_secure_cert partition");
        return NULL;
    }
    return NULL;
}

static void esp_secure_cert_get_partition_format(void)
{
    if (current_partition_format != ESP_SECURE_CERT_PF_INVALID) {
        return;
    }

    const esp_partition_t *part = esp_secure_cert_get_partition();
    if (part == NULL) {
        ESP_LOGE(TAG, "Failed to obtain the current partition and partition format");
        current_partition_format = ESP_SECURE_CERT_PF_INVALID;
    }
    return;
}

static int nvs_get(const char *name_space, const char *key, char *value, size_t *len, size_t type)
{
    esp_err_t err;
    nvs_handle_t handle;

    err = nvs_open_from_partition(nvs_partition_name, name_space, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not open NVS handle (0x%x)!", err);
        return err;
    }

    switch (type) {
    case NVS_STR:
        err = nvs_get_str(handle, key, value, len);
        break;
    case NVS_BLOB:
        err = nvs_get_blob(handle, key, value, len);
        break;
    case NVS_U8:
        err = nvs_get_u8(handle, key, (uint8_t *)value);
        break;
    case NVS_U16:
        err = nvs_get_u16(handle, key, (uint16_t *)value);
        break;
    default:
        ESP_LOGE(TAG, "Invalid type of NVS data provided");
        err = ESP_ERR_INVALID_ARG;
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (0x%02X) reading NVS data!", err);
        return err;
    }

    nvs_close(handle);
    return err;
}

esp_err_t esp_secure_cert_init_nvs_partition(void)
{
    const esp_partition_t *part = esp_secure_cert_get_partition();
    if (part == NULL) {
        ESP_LOGE(TAG, "Could not get partition.");
        return ESP_FAIL;
    }

    if (nvs_flash_init_partition(ESP_SECURE_CERT_PARTITION_NAME) == ESP_OK) {
        return ESP_OK;
    } else {
        return nvs_flash_init_partition(ESP_SECURE_CERT_LEGACY_PARTITION_NAME);
    }
}

static esp_err_t esp_secure_cert_read_raw_flash(const esp_partition_t *partition, uint32_t src_offset, void *dst, uint32_t size)
{
    /* Encrypted partitions need to be read via a cache mapping */
    const void *buf;
    spi_flash_mmap_handle_t handle;
    esp_err_t err;

    err = esp_partition_mmap(partition, src_offset, size, SPI_FLASH_MMAP_DATA, &buf, &handle);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(dst, buf, size);
    spi_flash_munmap(handle);
    return ESP_OK;
}

const void *esp_secure_cert_mmap(const esp_partition_t *partition, uint32_t src_offset, uint32_t size)
{
    /* Encrypted partitions need to be read via a cache mapping */
    const void *buf;
    spi_flash_mmap_handle_t handle;
    esp_err_t err;

    err = esp_partition_mmap(partition, src_offset, size, SPI_FLASH_MMAP_DATA, &buf, &handle);
    if (err != ESP_OK) {
        return NULL;
    }
    return buf;
}

static esp_err_t esp_secure_cert_read_metadata(esp_secure_cert_metadata *metadata, size_t offset, const esp_partition_t *part, uint32_t *data_len, uint32_t *data_crc)
{
    esp_err_t err;
    err = esp_secure_cert_read_raw_flash(part, ESP_SECURE_CERT_METADATA_OFFSET, metadata, sizeof(esp_secure_cert_metadata));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not read metadata.");
        return ESP_FAIL;
    }

    if (metadata->magic_word != ESP_SECURE_CERT_METADATA_MAGIC_WORD) {
        ESP_LOGE(TAG, "Metadata magic word does not match");
        return ESP_FAIL;
    }

    switch (offset) {
    case ESP_SECURE_CERT_METADATA_OFFSET:
        *data_len = sizeof(metadata);
        break;
    case ESP_SECURE_CERT_DEV_CERT_OFFSET:
        if (current_partition_format == ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY) {
            /* In the legacy format the order of dev_cert_len, dev_cert_crc have been interchanged with ca_cert_len and ca_cert_crc */
            /* Returning the appropriate values here */
            *data_len = metadata->ca_cert_len;
            *data_crc = metadata->ca_cert_crc;
        } else {
            *data_len = metadata->dev_cert_len;
            *data_crc = metadata->dev_cert_crc;
        }
        break;
    case ESP_SECURE_CERT_CA_CERT_OFFSET:
        if (current_partition_format == ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY) {
            /* In the legacy format the order of dev_cert_len, dev_cert_crc have been interchanged with ca_cert_len and ca_cert_crc */
            /* Returning the appropriate values here */
            *data_len = metadata->dev_cert_len;
            *data_crc = metadata->dev_cert_crc;
        } else {
            *data_len = metadata->ca_cert_len;
            *data_crc = metadata->ca_cert_crc;
        }
        break;
#ifndef CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL
    case ESP_SECURE_CERT_PRIV_KEY_OFFSET:
        *data_len = metadata->priv_key_len;
        *data_crc = metadata->priv_key_crc;
        break;
#else /* !CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL */
    case ESP_SECURE_CERT_CIPHERTEXT_OFFSET:
        *data_len = metadata->ciphertext_len;
        *data_crc = metadata->ciphertext_crc;
        break;
    case ESP_SECURE_CERT_IV_OFFSET:
        *data_len = metadata->iv_len;
        *data_crc = metadata->iv_crc;
        break;
#endif /* CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL */
    default:
        err = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG, "Invalid offset value given");
    }

    return err;
}

static esp_err_t esp_secure_cert_get_addr(size_t offset, char **buffer, uint32_t *len)
{
    esp_err_t err;
    static esp_secure_cert_metadata metadata;
    uint32_t data_len = 0;
    uint32_t data_crc = 0;

    const esp_partition_t *part = esp_secure_cert_get_partition();
    if (part == NULL) {
        ESP_LOGE(TAG, "Could not get partition.");
        return ESP_FAIL;
    }

    err = esp_secure_cert_read_metadata(&metadata, offset, part, &data_len, &data_crc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error in reading the metadata");
        return err;
    }

    *len = data_len;
    *buffer = (char *)esp_secure_cert_mmap(part, offset, *len);
    if (buffer == NULL) {
        return ESP_FAIL;
    }

    uint32_t read_crc = esp_crc32_le(UINT32_MAX, (const uint8_t * )*buffer, data_len);
    if (read_crc != data_crc) {
        ESP_LOGE(TAG, "Data has been tampered");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_secure_cert_get_device_cert(char **buffer, uint32_t *len)
{
    // This API sets the global variable current_partition_format
    esp_secure_cert_get_partition_format();
    esp_err_t ret;
    switch (current_partition_format) {
    case ESP_SECURE_CERT_PF_TLV:
        return esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_DEV_CERT_TLV, ESP_SECURE_CERT_SUBTYPE_MAX, buffer, len);

    case ESP_SECURE_CERT_PF_CUST_FLASH:
    case ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY:
        return esp_secure_cert_get_addr(ESP_SECURE_CERT_DEV_CERT_OFFSET, buffer, len);;

    case ESP_SECURE_CERT_PF_NVS:
        ret = nvs_get(nvs_namespace_name, ESP_SECURE_CERT_DEV_CERT, NULL, (size_t *)len, NVS_STR);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get device cert length from nvs");
            return ret;
        }

        *buffer = (char *)calloc(1, *len + 1);
        if (*buffer == NULL) {
            ESP_LOGE(TAG, "Not enough memory for device cert buffer");
            return ESP_ERR_NO_MEM;
        }

        return nvs_get(nvs_namespace_name, ESP_SECURE_CERT_DEV_CERT, *buffer, (size_t *)len, NVS_STR);

    case ESP_SECURE_CERT_PF_INVALID:
    default:
        ESP_LOGE(TAG, "Invalid flash format");
        return ESP_FAIL;
    }

}

esp_err_t esp_secure_cert_free_device_cert(char *buffer)
{
    switch(current_partition_format) {
    // fall through
    case ESP_SECURE_CERT_PF_TLV:
    case ESP_SECURE_CERT_PF_CUST_FLASH:
    case ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY:
        return ESP_OK;
        break;

    case ESP_SECURE_CERT_PF_NVS:
        free(buffer);
        return ESP_OK;
        break;
    case ESP_SECURE_CERT_PF_INVALID:
    default:
        return ESP_FAIL;
    }
}

esp_err_t esp_secure_cert_get_ca_cert(char **buffer, uint32_t *len)
{
    // This API sets the global variable current_partition_format
    esp_secure_cert_get_partition_format();
    esp_err_t ret;

    switch (current_partition_format) {
    case ESP_SECURE_CERT_PF_TLV:
        return esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_CA_CERT_TLV, ESP_SECURE_CERT_SUBTYPE_MAX, buffer, len);

    case ESP_SECURE_CERT_PF_CUST_FLASH:
    case ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY:
        return esp_secure_cert_get_addr(ESP_SECURE_CERT_CA_CERT_OFFSET, buffer, len);

    case ESP_SECURE_CERT_PF_NVS:
        ret = nvs_get(nvs_namespace_name, ESP_SECURE_CERT_CA_CERT, NULL, (size_t *)len, NVS_STR);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get ca cert length from nvs");
            return ret;
        }

        *buffer = (char *)calloc(1, *len + 1);
        if (*buffer == NULL) {
            ESP_LOGE(TAG, "Not enough memory for ca cert buffer");
            return ESP_ERR_NO_MEM;
        }
        return nvs_get(nvs_namespace_name, ESP_SECURE_CERT_CA_CERT, *buffer, (size_t *)len, NVS_STR);

    case ESP_SECURE_CERT_PF_INVALID:
    default:
        ESP_LOGE(TAG, "Invalid flash format");
        return ESP_FAIL;
    }

}

esp_err_t esp_secure_cert_free_ca_cert(char *buffer)
{
    switch(current_partition_format) {
    // fall through
    case ESP_SECURE_CERT_PF_TLV:
    case ESP_SECURE_CERT_PF_CUST_FLASH:
    case ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY:
        return ESP_OK;
        break;

    case ESP_SECURE_CERT_PF_NVS:
        free(buffer);
        return ESP_OK;
        break;
    case ESP_SECURE_CERT_PF_INVALID:
    default:
        return ESP_FAIL;
    }
}

#ifndef CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL
esp_err_t esp_secure_cert_get_priv_key(char **buffer, uint32_t *len)
{

    // This API sets the global variable current_partition_format
    esp_secure_cert_get_partition_format();
    esp_err_t ret;

    switch (current_partition_format) {
    case ESP_SECURE_CERT_PF_TLV:
        return esp_secure_cert_tlv_get_addr(ESP_SECURE_CERT_PRIV_KEY_TLV, ESP_SECURE_CERT_SUBTYPE_MAX, buffer, len);
    case ESP_SECURE_CERT_PF_CUST_FLASH:
    case ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY:
        return esp_secure_cert_get_addr(ESP_SECURE_CERT_PRIV_KEY_OFFSET, buffer, len);;

    case ESP_SECURE_CERT_PF_NVS:
        ret = nvs_get(nvs_namespace_name, ESP_SECURE_CERT_PRIV_KEY, NULL, (size_t *)len, NVS_STR);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get priv key length from nvs");
            return ret;
        }

        *buffer = (char *)calloc(1, *len + 1);
        if (*buffer == NULL) {
            ESP_LOGE(TAG, "Not enough memory for priv key buffer");
            return ESP_ERR_NO_MEM;
        }

        return nvs_get(nvs_namespace_name, ESP_SECURE_CERT_PRIV_KEY, *buffer, (size_t *)len, NVS_STR);

    case ESP_SECURE_CERT_PF_INVALID:
    default:
        ESP_LOGE(TAG, "Invalid flash format");
        return ESP_FAIL;
    }

}

esp_err_t esp_secure_cert_free_priv_key(char *buffer)
{
    switch(current_partition_format) {
    // fall through
    case ESP_SECURE_CERT_PF_TLV:
    case ESP_SECURE_CERT_PF_CUST_FLASH:
    case ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY:
        return ESP_OK;
        break;

    case ESP_SECURE_CERT_PF_NVS:
        free(buffer);
        return ESP_OK;
        break;
    case ESP_SECURE_CERT_PF_INVALID:
    default:
        return ESP_FAIL;
    }
}
#endif

#ifdef CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL
static esp_err_t esp_secure_cert_read(size_t offset, unsigned char *buffer, uint32_t *len)
{
    esp_err_t err;
    static esp_secure_cert_metadata metadata;
    uint32_t data_len = 0;
    uint32_t data_crc = 0;

    const esp_partition_t *part = esp_secure_cert_get_partition();
    if (part == NULL) {
        ESP_LOGE(TAG, "Could not get partition.");
        return ESP_FAIL;
    }

    err = esp_secure_cert_read_metadata(&metadata, offset, part, &data_len, &data_crc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error in reading the metadata");
        return err;
    }

    if (buffer == NULL) {
        *len = data_len;
        return ESP_OK;
    }

    if (*len < data_len) {
        ESP_LOGE(TAG, "Insufficient length of buffer. buffer size: %"PRIu32", required: %"PRIu32"", *len, data_len);
        return ESP_FAIL;
    }

    /* If the requested offset belongs to the medatada, return the already read metadata */
    if (offset == ESP_SECURE_CERT_METADATA_OFFSET) {
        memcpy(buffer, &metadata, sizeof(metadata));
        return ESP_OK;
    }

    err = esp_secure_cert_read_raw_flash(part, offset, buffer, data_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not read data.");
        return ESP_FAIL;
    }

    uint32_t read_crc = esp_crc32_le(UINT32_MAX, (const uint8_t * )buffer, data_len);
    if (read_crc != data_crc) {
        ESP_LOGE(TAG, "Data has been tampered");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_ds_data_ctx_t *esp_secure_cert_get_ds_ctx(void)
{
    esp_secure_cert_get_partition_format();
    if (current_partition_format == ESP_SECURE_CERT_PF_TLV) {
        return esp_secure_cert_tlv_get_ds_ctx();
    }
    esp_err_t esp_ret;
    esp_ds_data_ctx_t *ds_data_ctx;
    uint32_t len = 0;
    ds_data_ctx = (esp_ds_data_ctx_t *)heap_caps_malloc(sizeof(esp_ds_data_ctx_t), MALLOC_CAP_INTERNAL);
    if (ds_data_ctx == NULL) {
        ESP_LOGE(TAG, "Error in allocating memory for esp_ds_data_context");
        goto exit;
    }

    ds_data_ctx->esp_ds_data = (esp_ds_data_t *)heap_caps_calloc(1, sizeof(esp_ds_data_t), MALLOC_CAP_INTERNAL);
    if (ds_data_ctx->esp_ds_data == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for DS data handle ");
        goto exit;
    }

    // This API sets the global variable current_partition_format

    if (current_partition_format == ESP_SECURE_CERT_PF_CUST_FLASH || current_partition_format == ESP_SECURE_CERT_PF_CUST_FLASH_LEGACY) {
        char *buffer;
        len = 0;
        esp_ret = esp_secure_cert_get_addr(ESP_SECURE_CERT_CIPHERTEXT_OFFSET, &buffer, &len);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Error in reading ciphertext");
            goto exit;
        }

        memcpy((void *)ds_data_ctx->esp_ds_data->c, buffer, len);

        len = 0;
        esp_ret = esp_secure_cert_get_addr(ESP_SECURE_CERT_IV_OFFSET, &buffer, &len);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Error in reading initialization vector");
            goto exit;
        }

        memcpy((void *)ds_data_ctx->esp_ds_data->iv, buffer, len);
        unsigned char metadata[ESP_SECURE_CERT_METADATA_SIZE] = {};
        len = sizeof(metadata);
        esp_err_t err = esp_secure_cert_read(ESP_SECURE_CERT_METADATA_OFFSET, metadata, &len);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error in reading metadata");
            goto exit;
        }
        ds_data_ctx->rsa_length_bits = ((esp_secure_cert_metadata *)metadata)->rsa_length;
        ds_data_ctx->efuse_key_id = ((esp_secure_cert_metadata *)metadata)->efuse_key_id;
        return ds_data_ctx;

    } else if (current_partition_format == ESP_SECURE_CERT_PF_NVS) {
        len = ESP_DS_C_LEN;
        esp_ret = nvs_get(nvs_namespace_name, ESP_SECURE_CERT_CIPHERTEXT, (char *) ds_data_ctx->esp_ds_data->c, (size_t *) &len, NVS_BLOB);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Error in reading ciphertext");
            goto exit;
        }

        len = ESP_DS_IV_LEN;
        esp_ret = nvs_get(nvs_namespace_name, ESP_SECURE_CERT_IV, (char *)ds_data_ctx->esp_ds_data->iv, (size_t *)&len, NVS_BLOB);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Error in reading initialization vector");
            goto exit;
        }

        esp_ret = nvs_get(nvs_namespace_name, ESP_SECURE_CERT_EFUSE_KEY_ID, (void *)&ds_data_ctx->efuse_key_id, 0, NVS_U8);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Error in reading efuse key id");
            goto exit;
        }

        esp_ret = nvs_get(nvs_namespace_name, ESP_SECURE_CERT_RSA_LEN, (void *)&ds_data_ctx->rsa_length_bits, 0, NVS_U16);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Error in reading rsa key length");
            goto exit;
        }
        return ds_data_ctx;
    }

exit:
    if (ds_data_ctx != NULL) {
        free(ds_data_ctx->esp_ds_data);
    }
    free(ds_data_ctx);
    return NULL;
}

void esp_secure_cert_free_ds_ctx(esp_ds_data_ctx_t *ds_ctx)
{
    esp_secure_cert_get_partition_format();
    if (current_partition_format == ESP_SECURE_CERT_PF_TLV) {
        esp_secure_cert_tlv_free_ds_ctx(ds_ctx);
    }

    if (ds_ctx != NULL) {
        free(ds_ctx->esp_ds_data);
    }
    free(ds_ctx);
}
#endif
