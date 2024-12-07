/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief Temperature Measurement cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID         = 0x0000,  /*!< MeasuredValue */
    ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_MIN_VALUE_ID     = 0x0001,  /*!< MinMeasuredValue*/
    ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_MAX_VALUE_ID     = 0x0002,  /*!< MaxMeasuredValue */
    ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_TOLERANCE_ID     = 0x0003,  /*!< Tolerance */
} esp_zb_zcl_temp_measurement_attr_t;

/** @brief Minimum value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MIN_MEASURED_VALUE_MINIMUM ((int16_t)0x954d)

/** @brief Maximum value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MIN_MEASURED_VALUE_MAXIMUM ((int16_t)0x7ffe)

/** @brief Minimum value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MAX_MEASURED_VALUE_MINIMUM ((int16_t)0x954e)

/** @brief Maximum value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MAX_MEASURED_VALUE_MAXIMUM ((int16_t)0x7fff)

/** @brief Minimum value for Tolerance attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_TOLERANCE_MINIMUM ((uint16_t)0x0000)

/** @brief Maximum value for Tolerance attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_TOLERANCE_MAXIMUM ((uint16_t)0x0800)

/** @brief Unknown value for MeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_S16_NONE

/** @brief Unknown value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MIN_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_S16_NONE

/** @brief Unknown value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MAX_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_S16_NONE

/** @brief Default value for MeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_S16_NONE

/** @brief Default value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MIN_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_S16_NONE

/** @brief Default value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_TEMP_MEASUREMENT_MAX_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_S16_NONE

void esp_zb_zcl_temp_measurement_init_server(void);
#define ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT_SERVER_ROLE_INIT esp_zb_zcl_temp_measurement_init_server
#define ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT_CLIENT_ROLE_INIT NULL

#ifdef __cplusplus
}
#endif
