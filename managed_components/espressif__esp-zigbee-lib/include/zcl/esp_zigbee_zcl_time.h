/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief Time cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_TIME_TIME_ID             = 0x0000,  /*!< Time attribute */
    ESP_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID      = 0x0001,  /*!< Time Status attribute */
    ESP_ZB_ZCL_ATTR_TIME_TIME_ZONE_ID        = 0x0002,  /*!< Time Zone attribute */
    ESP_ZB_ZCL_ATTR_TIME_DST_START_ID        = 0x0003,  /*!< Dst Start attribute */
    ESP_ZB_ZCL_ATTR_TIME_DST_END_ID          = 0x0004,  /*!< Dst End attribute */
    ESP_ZB_ZCL_ATTR_TIME_DST_SHIFT_ID        = 0x0005,  /*!< Dst Shift attribute */
    ESP_ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID    = 0x0006,  /*!< Standard Time attribute */
    ESP_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID       = 0x0007,  /*!< Local Time attribute */
    ESP_ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID    = 0x0008,  /*!< Last Set Time attribute */
    ESP_ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID = 0x0009   /*!< Valid Until Time attribute */
} esp_zb_zcl_time_attr_t;

/** @brief Default value for Time attribute */
#define ESP_ZB_ZCL_TIME_TIME_DEFAULT_VALUE 0xffffffff

/** @brief Default value for Time Status attribute */
#define ESP_ZB_ZCL_TIME_TIME_STATUS_DEFAULT_VALUE 0x00

/** @brief Default value for Time Zone attribute */
#define ESP_ZB_ZCL_TIME_TIME_ZONE_DEFAULT_VALUE 0x00000000

/** @brief Default value for DstStart attribute */
#define ESP_ZB_ZCL_TIME_DST_START_DEFAULT_VALUE ((uint32_t)0xFFFFFFFF)

/** @brief Default value for DstEnd attribute */
#define ESP_ZB_ZCL_TIME_DST_END_DEFAULT_VALUE ((uint32_t)0xFFFFFFFF)

/** @brief Default value for Dst Shift attribute */
#define ESP_ZB_ZCL_TIME_DST_SHIFT_DEFAULT_VALUE 0x00000000

/** @brief Default value for StandardTime attribute */
#define ESP_ZB_ZCL_TIME_STANDARD_TIME_DEFAULT_VALUE ((uint32_t)0xFFFFFFFF)

/** @brief Default value for LocalTime attribute */
#define ESP_ZB_ZCL_TIME_LOCAL_TIME_DEFAULT_VALUE ((uint32_t)0xFFFFFFFF)

/** @brief Default value for Last Set Time attribute */
#define ESP_ZB_ZCL_TIME_LAST_SET_TIME_DEFAULT_VALUE 0xffffffff

/** @brief Default value for Valid Until Time attribute */
#define ESP_ZB_ZCL_TIME_VALID_UNTIL_TIME_DEFAULT_VALUE 0xffffffff

#ifdef __cplusplus
}
#endif
