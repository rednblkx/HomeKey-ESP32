/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FILETAG_RCP_VERSION = 0,
    FILETAG_RCP_FLASH_ARGS = 1,
    FILETAG_RCP_BOOTLOADER = 2,
    FILETAG_RCP_PARTITION_TABLE = 3,
    FILETAG_RCP_FIRMWARE = 4,
    FILETAG_BR_FIRMWARE = 5,
    FILETAG_IMAGE_HEADER = 0xff,
} esp_br_filetag_t;

struct esp_br_subfile_info {
    uint32_t tag;
    uint32_t size;
    uint32_t offset;
} __attribute__((packed));

typedef struct esp_br_subfile_info esp_br_subfile_info_t;

#define ESP_BR_RCP_IMAGE_FILENAME "rcp_image"

#ifdef __cplusplus
}
#endif
