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

/** @brief Pressure Measurement cluster attribute identifiers
*/
typedef enum
{
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID             = 0x0000,       /*!< MeasuredValue */
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_ID         = 0x0001,       /*!< MinMeasuredValue */
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_ID         = 0x0002,       /*!< MaxMeasuredValue */
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_ID         = 0x0003,       /*!< MeasuredTolerance */
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_VALUE_ID      = 0x0010,       /*!< ScaledValue */
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_ID  = 0x0011,       /*!< MinScaledValue */
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_ID  = 0x0012,       /*!< MaxScaledValue */
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_ID  = 0x0013,       /*!< ScaledTolerance */
  ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_ID             = 0x0014,       /*!< Scale */
}esp_zb_zcl_pressure_measurement_attr_t;

/** @brief Default value for Pressure Measurement cluster revision global attribute */
#define ESP_ZB_ZCL_PRESSURE_MEASUREMENT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief MeasuredValue attribute unknown value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_UNKNOWN                  ((int16_t)0x8000)

/** @brief MinMeasuredValue attribute minimum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_MIN_VALUE            ((int16_t)0x954d)

/** @brief MinMeasuredValue attribute maximum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_MAX_VALUE            ((int16_t)0x7ffe)

/** @brief MinMeasuredValue attribute invalid value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_INVALID              ((int16_t)0x8000)

/** @brief MaxMeasuredValue attribute minimum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_MIN_VALUE            ((int16_t)0x954e)

/** @brief MaxMeasuredValue attribute maximum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_MAX_VALUE            ((int16_t)0x7fff)

/** @brief MaxMeasuredValue attribute invalid value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_INVALID              ((int16_t)0x8000)

/** @brief Tolerance attribute minimum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_MIN_VALUE            ((uint16_t)0x0000)

/** @brief Tolerance attribute maximum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_MAX_VALUE            ((uint16_t)0x0800)

/** @brief Default value for Value attribute */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_DEFAULT_VALUE            ((int16_t)0xFFFF)

/** @brief Default value for MinValue attribute */
#define ESP_ZB_ZCL_PATTR_RESSURE_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE        ((int16_t)0x8000)

/** @brief Default value for MaxValue attribute */
#define ESP_ZB_ZCL_PATTR_RESSURE_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE        ((int16_t)0x8000)

/** @brief MinScaledValue attribute unknown value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_UNKNOWN       ((int16_t)0x8000)

/** @brief MinScaledValue attribute minimum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_MIN_VALUE     ((int16_t)0x8001)

/** @brief MinScaledValue attribute maximum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_MAX_VALUE     ((int16_t)0x7ffe)

/** @brief MaxScaledValue attribute unknown value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_UNKNOWN       ((int16_t)0x8000)

/** @brief MaxScaledValue attribute minimum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_MIN_VALUE     ((int16_t)0x8002)

/** @brief MaxScaledValue attribute maximum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_MAX_VALUE     ((int16_t)0x7fff)

/** @brief MaxScaledValue attribute minimum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_MIN_VALUE     ((uint16_t)0x0000)

/** @brief MaxScaledValue attribute maximum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_MAX_VALUE     ((uint16_t)0x0800)

/** @brief MaxScaledValue attribute minimum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_MIN_VALUE                ((int8_t)0x81)

/** @brief MaxScaledValue attribute maximum value */
#define ESP_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_MAX_VALUE                ((int8_t)0x7f)

#ifdef __cplusplus
}
#endif
