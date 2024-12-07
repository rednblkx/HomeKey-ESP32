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

/*! @brief Thermostat UI Configuration cluster attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID        = 0x0000, /*!< Temperature Display Mode attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID                  = 0x0001, /*!< Keypad Lockout attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_SCHEDULE_PROGRAMMING_VISIBILITY_ID = 0x0002, /*!< The Schedule ProgrammingVisibility attribute is used to hide the weekly schedule programming functionality or menu on a thermostat from a user to prevent local user programming of the weekly schedule. */
} esp_zb_zcl_thermostat_ui_config_attr_t;

/** @brief Default value for Temperature Display Mode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_DEFAULT_VALUE 0x00

/** @brief Default value for Keypad Lockout attribute */
#define ESP_ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_DEFAULT_VALUE 0x00

/** @brief Default value for ScheduleProgrammingVisibility attribute */
#define ESP_ZB_ZCL_THERMOSTAT_UI_CONFIG_SCHEDULE_PROGRAMMING_VISIBILITY_DEFAULT_VALUE ((zb_uint8_t)0x00)

#ifdef __cplusplus
}
#endif
