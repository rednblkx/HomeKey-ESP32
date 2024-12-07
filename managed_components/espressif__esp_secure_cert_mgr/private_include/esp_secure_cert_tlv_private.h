/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "esp_secure_cert_config.h"
#include "esp_secure_cert_tlv_config.h"

#ifdef CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL
#include "rsa_sign_alt.h"
#endif
#include "soc/soc_caps.h"

#define ESP_SECURE_CERT_TLV_PARTITION_TYPE      0x3F                        /* Custom partition type */
#define ESP_SECURE_CERT_TLV_PARTITION_NAME      "esp_secure_cert"           /* Name of the custom esp_secure_cert partition */
#define ESP_SECURE_CERT_TLV_MAGIC                0xBA5EBA11

#define ESP_SECURE_CERT_HMAC_KEY_ID              (0)                         /* The hmac_key_id value that shall be used for HMAC based ecdsa key generation */
#define ESP_SECURE_CERT_DERIVED_ECDSA_KEY_SIZE   (32)                       /* The key size in bytes of the derived ecdsa key */
#define ESP_SECURE_CERT_KEY_DERIVATION_ITERATION_COUNT  (2048)              /* The iteration count for ecdsa key derivation */



/**
 * Flags    8 bits
 * Used bits:
 *      bit7(MSB) & bit6 - hmac_based_encryption
 *          0b10 - (i.e. 2 in decimal) the data in the block needs to be
 *          decrypted first using the HMAC based encryption scheme
 *          before sending out
 *          0b01 - (i.e. 1 in decimal) the hmac based ecdsa
 *          private key generation is enabled. Generate the private key internally using the hardware HMAC peripheral.
 *
 *      bit5 & bit4 & bit3 - TLV key flags
 *          0b001 - (i.e. 1 in decimal) The ecdsa key is stored in an eFuse key block
 *
 *      In this case all the flags are mutually exclusive.
 * Ununsed bits:
 *      .
 *      .
 *      bit0 (LSB)
 */

#define ESP_SECURE_CERT_TLV_FLAG_HMAC_ENCRYPTION            (2 << 6)
#define ESP_SECURE_CERT_TLV_FLAG_HMAC_ECDSA_KEY_DERIVATION  (1 << 6)
#define ESP_SECURE_CERT_TLV_FLAG_KEY_ECDSA_PERIPHERAL       (1 << 3)
#define ESP_SECURE_CERT_TLV_KEY_FLAGS_BIT_MASK              (BIT5 | BIT4 | BIT3)

#define ESP_SECURE_CERT_IS_TLV_ENCRYPTED(flags) \
    ((flags & (BIT7 | BIT6)) == ESP_SECURE_CERT_TLV_FLAG_HMAC_ENCRYPTION)

#define ESP_SECURE_CERT_HMAC_ECDSA_KEY_DERIVATION(flags) \
    ((flags & (BIT7 | BIT6)) == ESP_SECURE_CERT_TLV_FLAG_HMAC_ECDSA_KEY_DERIVATION)

#define ESP_SECURE_CERT_KEY_ECDSA_PERIPHERAL(flags) \
    ((flags & ESP_SECURE_CERT_TLV_KEY_FLAGS_BIT_MASK) == ESP_SECURE_CERT_TLV_FLAG_KEY_ECDSA_PERIPHERAL)

/* secure cert partition of cust_flash type in this case is of 8 KB size,
 * out of which 3-3.1 KB size is utilized.
 */

/*
 * Header for each tlv
 */
typedef struct esp_secure_cert_tlv_header {
    uint32_t magic;
    uint8_t flags;                      /* flags byte that identifies different characteristics for the TLV */
    uint8_t reserved[3];                /* Reserved bytes for future use, the value currently should be 0x0 */
    uint8_t type;                       /* Type of tlv structure, this shall be typecasted
                                           to esp_secure_cert_tlv_type_t for further use */
    uint8_t subtype;                    /* Subtype of TLV structure, this acts as an index for the type */
    uint16_t length;                    /* Length of the data */
    uint8_t value[0];                   /* Actual data in form of byte array */
} __attribute__((packed)) esp_secure_cert_tlv_header_t;

/*
 * Footer for each tlv
 */
typedef struct esp_secure_cert_tlv_footer {
    uint32_t crc;                       /* crc of the data */
} esp_secure_cert_tlv_footer_t;

_Static_assert(sizeof(esp_secure_cert_tlv_header_t) == 12, "TLV header size should be 12 bytes");

_Static_assert(sizeof(esp_secure_cert_tlv_footer_t) == 4, "TLV footer size should be 4 bytes");

/*
 * Note:
 *
 * The data stored in a cust flash partition should be as follows:
 *
 * tlv_header1 -> data_1 -> tlv_footer1 -> tlv_header2...
 *
 */

typedef struct esp_secure_cert_tlv_sec_cfg {
    uint8_t priv_key_efuse_id; /* eFuse key id in which the private key is stored */
    uint8_t reserved[39];       /* Reserving 39 bytes for future use */
} __attribute__((packed)) esp_secure_cert_tlv_sec_cfg_t;

_Static_assert(sizeof(esp_secure_cert_tlv_sec_cfg_t) == 40, "TLV sec cfg size should be 40 bytes");

/*
 * Map the entire esp_secure_cert partition
 * and return the virtual address.
 *
 * @note
 * The mapping is done only once and function shall
 * simply return same address in case of successive calls.
 **/
const void *esp_secure_cert_get_mapped_addr(void);

/*
 * Find the offset of tlv structure of given type in the esp_secure_cert partition
 *
 * Note: This API also validates the crc of the respective tlv before returning the offset
 * @input
 * esp_secure_cert_addr     Memory mapped address of the esp_secure_cert partition
 * type                     Type of the tlv structure.
 *                          for calculating current crc for esp_secure_cert
 *
 * tlv_address              Void pointer to store tlv address
 *
 */
esp_err_t esp_secure_cert_find_tlv(const void *esp_secure_cert_addr, esp_secure_cert_tlv_type_t type, uint8_t subtype, void **tlv_address);

/*
 *  Get the flash address of the data of a TLV entry
 *
 * Note: This API also validates the crc of the respective tlv before returning the offset. The offset is not the physical address but the address where it is mapped in the memory space.
 * @input
 *     type                 Type of the TLV entry
 *     subtype              Subtype of the TLV entry (index)
 *     buffer               Pointer to the buffer to store the data address
 *     len                  Pointer to store the length of the data
 *
 * Note: If tlv type = ESP_SECURE_CERT_TLV_END then the address returned shall be the end address of current tlv formatted data.
 * If tlv subtype = ESP_SECURE_CERT_SUBTYPE_MAX then the the address of tlv of given type and highest subtype found shall be returned.
 * @return
 *
 *      - ESP_OK    On success
 *      - ESP_FAIL/other relevant esp error code
 *                  On failure
 */
esp_err_t esp_secure_cert_tlv_get_addr(esp_secure_cert_tlv_type_t type, esp_secure_cert_tlv_subtype_t subtype, char **buffer, uint32_t *len);

/*
 * Identify if esp_secure_cert partition of type TLV is present.
 * @return
 *       - 1 on if the partition is identified as TLV
 *       - 0 on failure
 */
bool esp_secure_cert_is_tlv_partition(void);

#ifdef CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL
/* @info
 *       This function returns the flash esp_ds_context which can then be
 *       directly provided to an esp-tls connection through its config structure.
 *       The memory for the context is dynamically allocated.
 *       The internal structures are however directly accessed from flash.
 *       e.g. esp_ds_data
 *
 * @params
 *      - ds_ctx    The pointer to the DS context
 * @return
 *      - ESP_OK    On success
 *      - ESP_FAIL/other relevant esp error code
 *                  On failure
 */
esp_ds_data_ctx_t *esp_secure_cert_tlv_get_ds_ctx(void);

/*
 *@info
 *      Free the ds context
 */
void esp_secure_cert_tlv_free_ds_ctx(esp_ds_data_ctx_t *ds_ctx);
#endif

#if SOC_HMAC_SUPPORTED
#define HMAC_ENCRYPTION_MESSAGE_LEN                     (32)
#define HMAC_ENCRYPTION_IV_LEN                          (16)
#define HMAC_ENCRYPTION_TAG_LEN                         (16)
#define HMAC_ENCRYPTION_AES_GCM_KEY_LEN                 (32)

/*
 * @info
 * Calculate the IV for the hmac based encryption
 * iv       The pointer to the buffer to which IV should be written
 *          The buffer must be a writable buffer of size HMAC_ENCRYPTION_IV_LEN
 */
esp_err_t esp_secure_cert_calculate_hmac_encryption_iv(uint8_t *iv);

/*
 * @info
 * Calculate the IV for the hmac based encryption
 * aes_key  The pointer to the buffer to which IV should be written
 *          The buffer must be a writable
 *          buffer of size HMAC_ENCRYPTION_AES_GCM_KEY_LEN
 */
esp_err_t esp_secure_cert_calculate_hmac_encryption_key(uint8_t *aes_key);
#endif
