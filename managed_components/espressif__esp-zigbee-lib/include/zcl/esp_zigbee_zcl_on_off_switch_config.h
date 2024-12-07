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

/** @brief on-off switch type attribute
*/
typedef enum {
    ESP_ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_TOGGLE        = 0,   /*!< Toggle switch */
    ESP_ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_MOMENTARY     = 1,   /*!< Momentary switch */
    ESP_ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_TYPE_MULTIFUNCTION = 2    /*!< Multifunction switch */
} esp_zb_zcl_on_off_switch_configuration_switch_type_t;


/** @brief on-off switch actions attribute
*/
typedef enum {
    ESP_ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_ACTIONS_TYPE1  = 0,       /*!< First type command pattern */
    ESP_ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_ACTIONS_TYPE2  = 1,       /*!< Second type command pattern */
    ESP_ZB_ZCL_ON_OFF_SWITCH_CONFIGURATION_SWITCH_ACTIONS_TOGGLE = 2        /*!< Toggle command pattern */
} esp_zb_zcl_on_off_switch_configuration_switch_actions_t;

#ifdef __cplusplus
}
#endif
