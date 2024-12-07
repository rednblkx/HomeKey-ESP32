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

/** @brief Shade Configuration cluster information attribute set identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_SHADE_CONFIG_PHYSICAL_CLOSED_LIMIT_ID = 0x0000,  /*!< It indicates the most closed (numerically lowest) position that the shade can physically move to. */
    ESP_ZB_ZCL_ATTR_SHADE_CONFIG_MOTOR_STEP_SIZE_ID       = 0x0001,  /*!< It indicates the angle the shade motor moves for one step, measured in 1/10ths of a degree. */
    ESP_ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_ID                = 0x0002   /*!< Status attribute */
} esp_zb_zcl_shade_config_info_attr_t;

/** @brief Shade Configuration Status attribute default value */
#define ESP_ZB_ZCL_SHADE_CONFIG_STATUS_DEFAULT_VALUE 0x00

/** @brief ClosedLimit attribute default value */
#define ESP_ZB_ZCL_SHADE_CONFIG_CLOSED_LIMIT_DEFAULT_VALUE 0x0001

/** @brief Mode attribute default value */
#define ESP_ZB_ZCL_SHADE_CONFIG_MODE_DEFAULT_VALUE 0x00

#ifdef __cplusplus
}
#endif
