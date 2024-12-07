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

/** @brief On/Off cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID            = 0,         /*!< OnOff attribute */
    ESP_ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL = 0x4000,    /*!< Global Scene Control attribute identifier. */
    ESP_ZB_ZCL_ATTR_ON_OFF_ON_TIME              = 0x4001,    /*!< On Time attribute identifier. */
    ESP_ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME        = 0x4002,    /*!< Off Wait Time attribute identifier. */
    ESP_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF      = 0x4003,    /*!< Define the desired startup behavior */
} esp_zb_zcl_on_off_attr_t;

/** @brief Default value for OnOff attribute */
#define ESP_ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE ((bool)0x00)

/** @brief Default value for GlobalSceneControl attribute */
#define ESP_ZB_ZCL_ON_OFF_GLOBAL_SCENE_CONTROL_DEFAULT_VALUE ((bool)0x01)

/** @brief Default value for OnTime attribute */
#define ESP_ZB_ZCL_ON_OFF_ON_TIME_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for OffWaitTime attribute */
#define ESP_ZB_ZCL_ON_OFF_OFF_WAIT_TIME_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief On/Off cluster command identifiers
*/
typedef enum {
    ESP_ZB_ZCL_CMD_ON_OFF_OFF_ID                         = 0x00, /*!< "Turn off" command. */
    ESP_ZB_ZCL_CMD_ON_OFF_ON_ID                          = 0x01, /*!< "Turn on" command. */
    ESP_ZB_ZCL_CMD_ON_OFF_TOGGLE_ID                      = 0x02, /*!< "Toggle state" command. */
    ESP_ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID             = 0x40, /*!< "Off with effect" command. */
    ESP_ZB_ZCL_CMD_ON_OFF_ON_WITH_RECALL_GLOBAL_SCENE_ID = 0x41, /*!< "On with recall global scene" command. */
    ESP_ZB_ZCL_CMD_ON_OFF_ON_WITH_TIMED_OFF_ID           = 0x42, /*!< "On with timed off" command. */
} esp_zb_zcl_on_off_cmd_id_t;

#ifdef __cplusplus
}
#endif
