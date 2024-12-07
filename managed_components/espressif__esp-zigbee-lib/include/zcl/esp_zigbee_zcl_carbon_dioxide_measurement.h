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

/** @brief Carbon Dioxide Measurement cluster attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_CARBON_DIOXIDE_MEASUREMENT_MEASURED_VALUE_ID     = 0x0000, /*!< MeasuredValue attribute */
    ESP_ZB_ZCL_ATTR_CARBON_DIOXIDE_MEASUREMENT_MIN_MEASURED_VALUE_ID = 0x0001, /*!< MinMeasuredValue attribute */
    ESP_ZB_ZCL_ATTR_CARBON_DIOXIDE_MEASUREMENT_MAX_MEASURED_VALUE_ID = 0x0002, /*!< MaxMeasuredValue attribute */
    ESP_ZB_ZCL_ATTR_CARBON_DIOXIDE_MEASUREMENT_TOLERANCE_ID          = 0x0003, /*!< Tolerance attribute */
} esp_zb_zcl_carbon_dioxide_measurement_attr_t;

/** @brief Minimum value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MIN_MEASURED_VALUE_MINIMUM (0.0)

/** @brief Maximum value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MIN_MEASURED_VALUE_MAXIMUM (1.0)

/** @brief Minimum value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MAX_MEASURED_VALUE_MINIMUM (0.0)

/** @brief Maximum value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MAX_MEASURED_VALUE_MAXIMUM (1.0)

/** @brief Unknown value for MeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_NaN

/** @brief Unknown value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MIN_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_NaN

/** @brief Unknown value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MAX_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_NaN

/** @brief Default value for MeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_NaN

/** @brief Default value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MIN_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_NaN

/** @brief Default value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_CARBON_DIOXIDE_MEASUREMENT_MAX_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_NaN

void esp_zb_zcl_carbon_dioxide_measurement_init_server(void);
#define ESP_ZB_ZCL_CLUSTER_ID_CARBON_DIOXIDE_MEASUREMENT_SERVER_ROLE_INIT esp_zb_zcl_carbon_dioxide_measurement_init_server
#define ESP_ZB_ZCL_CLUSTER_ID_CARBON_DIOXIDE_MEASUREMENT_CLIENT_ROLE_INIT NULL

#ifdef __cplusplus
}
#endif
