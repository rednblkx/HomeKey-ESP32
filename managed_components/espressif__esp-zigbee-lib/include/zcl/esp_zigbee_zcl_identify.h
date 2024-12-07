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

/** @brief Identify cluster attribute identifier
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID = 0x0000  /*!< Identify time attribute */
} esp_zb_zcl_identify_attr_t;

/** @brief Default value for Identify attribute */
#define ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE 0x0000

/** @brief Command identifiers for "Identify" cluster
*/
typedef enum {
    ESP_ZB_ZCL_CMD_IDENTIFY_IDENTIFY_ID           = 0x00,  /*!< Identify command */
    ESP_ZB_ZCL_CMD_IDENTIFY_IDENTIFY_QUERY_ID     = 0x01,  /*!< Identify query command */
    ESP_ZB_ZCL_CMD_IDENTIFY_TRIGGER_EFFECT_ID     = 0x40,  /*!< "Trigger effect" command identifier. */
    ESP_ZB_ZCL_CMD_IDENTIFY_IDENTIFY_QUERY_RSP_ID = 0x00   /*!< Identify query response */
} esp_zb_zcl_identify_cmd_id_t;

/** @brief the values of the effect identifier field of the trigger effect command
 */
typedef enum esp_zb_zcl_identify_trigger_effect_s {
    ESP_ZB_ZCL_IDENTIFY_EFFECT_ID_BLINK = 0x00,             /*!< Effect identifier field value: Light is turned on/off once */
    ESP_ZB_ZCL_IDENTIFY_EFFECT_ID_BREATHE = 0x01,           /*!< Effect identifier field value: Light turned on/off over 1 second and repeated 15 times */
    ESP_ZB_ZCL_IDENTIFY_EFFECT_ID_OKAY = 0x02,              /*!< Effect identifier field value: Colored light turns green for 1 second; non-colored light flashes twice */
    ESP_ZB_ZCL_IDENTIFY_EFFECT_ID_CHANNEL_CHANGE = 0x0b,    /*!< Effect identifier field value: Colored light turns orange for 8 seconds; non-colored light
                                                                switches to maximum brightness for 0.5s and then minimum brightness for 7.5s */
    ESP_ZB_ZCL_IDENTIFY_EFFECT_ID_FINISH_EFFECT = 0xfe,     /*!< Effect identifier field value: Complete the current effect sequence before terminating. */
    ESP_ZB_ZCL_IDENTIFY_EFFECT_ID_STOP = 0xff,              /*!< Effect identifier field value: Terminate the effect as soon as possible */
} esp_zb_zcl_identify_trigger_effect_t;

#ifdef __cplusplus
}
#endif
