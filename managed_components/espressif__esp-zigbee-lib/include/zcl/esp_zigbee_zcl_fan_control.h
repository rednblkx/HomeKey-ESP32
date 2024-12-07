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

/** @brief Fan Control cluster attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID          = 0x0000, /*!< Fan mode attribute */
    ESP_ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID = 0x0001  /*!< Fan mode sequence attribute */
} esp_zb_zcl_fan_control_attr_t;

/** @brief Values for Fan Mode attribute */
typedef enum {
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_OFF    = 0x00, /*! Off value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_LOW    = 0x01, /*! Low value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_MEDIUM = 0x02, /*! Medium value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_HIGH   = 0x03, /*! High value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_ON     = 0x04, /*! On value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_AUTO   = 0x05, /*! Auto value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_SMART  = 0x06, /*! Smart value */
} esp_zb_zcl_fan_control_fan_mode_t;

/** @brief Values for Fan Mode Sequence attribute */
typedef enum {
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_MED_HIGH      = 0x00, /*! Low/Med/High value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_HIGH          = 0x01, /*! Low/High value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_MED_HIGH_AUTO = 0x02, /*! Low/Med/High/Auto value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_HIGH_AUTO     = 0x03, /*! Low/High/Auto value */
    ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_ON_AUTO           = 0x04, /*! On/Auto value */
} esp_zb_zcl_fan_control_fan_mode_sequence_t;

/** @brief Default value for Fan Mode attribute */
#define ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_DEFAULT_VALUE 0x05

/** @brief Default value for Fan Mode attribute */
#define ESP_ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_DEFAULT_VALUE 0x02

#ifdef __cplusplus
}
#endif
