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

/** @brief Illuminance Measurement cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ID       = 0x0000,  /*!< MeasuredValue */
    ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_ID   = 0x0001,  /*!< MinMeasuredValue Attribute */
    ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_ID   = 0x0002,  /*!< MaxMeasuredValue Attribute */
    ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_TOLERANCE_ID            = 0x0003,  /*!< The Tolerance attribute SHALL indicate the magnitude of the possible error that is associated with MeasuredValue, using the same units and resolution.*/
    ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_LIGHT_SENSOR_TYPE_ID    = 0x0004   /*!< The LightSensorType attribute specifies the electronic type ofthe light sensor. */
} esp_zb_zcl_illuminance_measurement_attr_t;

/** @brief Default value for Illuminance Measurement cluster revision global attribute */
#define ESP_ZB_ZCL_ILLUMINANCE_MEASUREMENT_CLUSTER_REVISION_DEFAULT ((uint16_t)0x0002u)

/** @brief MeasuredValue attribute too-low value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_TOO_LOW       0

/** @brief MeasuredValue attribute invalid value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_INVALID       0xFFFF

/** @brief MeasuredValue attribute default value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_DEFAULT \
  ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_TOO_LOW

/** @brief Default value for LightSensorType attribute */
#define ESP_ZB_ZCL_ILLUMINANCE_MEASUREMENT_LIGHT_SENSOR_TYPE_DEFAULT_VALUE ((uint8_t)0xFF)

/** @brief MinMeasuredValue attribute minimum value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_MIN_VALUE 0x0001

/** @brief MinMeasuredValue attribute maximum value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_MAX_VALUE 0xFFFD

/** @brief MinMeasuredValue attribute not-defined value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_UNDEFINED 0xFFFF

/** @brief MaxMeasuredValue attribute minimum value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_MIN_VALUE 0x0002

/** @brief MaxMeasuredValue attribute maximum value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_MAX_VALUE 0xFFFE

/** @brief MaxMeasuredValue attribute not-defined value */
#define ESP_ZB_ZCL_ATTR_ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_UNDEFINED 0xFFFF

#ifdef __cplusplus
}
#endif
