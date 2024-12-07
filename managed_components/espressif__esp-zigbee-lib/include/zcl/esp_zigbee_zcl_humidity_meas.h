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

/** @brief Water Content Measurement cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID     = 0x0000,  /*!<  MeasuredValue */
    ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_MIN_VALUE_ID = 0x0001,  /*!<  MinMeasuredValue Attribute */
    ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_MAX_VALUE_ID = 0x0002,  /*!<  MaxMeasuredValue Attribute */
    ESP_ZB_ZCL_ATTR_REL_HUMIDITY_TOLERANCE_ID             = 0x0003,  /*!<  The Tolerance attribute SHALL indicate the magnitude of the possible error that is associated with MeasuredValue, using the same units and resolution.*/
} esp_zb_zcl_rel_humidity_measurement_attr_t;

/** @brief Minimum value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MIN_MEASURED_VALUE_MINIMUM ((uint16_t)0x0000)

/** @brief Maximum value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MIN_MEASURED_VALUE_MAXIMUM ((uint16_t)0x270f)

/** @brief Minimum value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MAX_MEASURED_VALUE_MINIMUM ((uint16_t)0x0001)

/** @brief Maximum value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MAX_MEASURED_VALUE_MAXIMUM ((uint16_t)0x2710)

/** @brief Minimum value for Tolerance attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_TOLERANCE_MINIMUM ((uint16_t)0x0000)

/** @brief Maximum value for Tolerance attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_TOLERANCE_MAXIMUM ((uint16_t)0x0800)

/** @brief Unknown value for MeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_U16_NONE

/** @brief Unknown value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MIN_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_U16_NONE

/** @brief Unknown value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MAX_MEASURED_VALUE_UNKNOWN ESP_ZB_ZCL_VALUE_U16_NONE

/** @brief Default value for MeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_U16_NONE

/** @brief Default value for MinMeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MIN_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_U16_NONE

/** @brief Default value for MaxMeasuredValue attribute */
#define ESP_ZB_ZCL_REL_HUMIDITY_MEASUREMENT_MAX_MEASURED_VALUE_DEFAULT ESP_ZB_ZCL_VALUE_U16_NONE

void esp_zb_zcl_rel_humidity_measurement_init_server(void);
#define ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT_SERVER_ROLE_INIT esp_zb_zcl_rel_humidity_measurement_init_server
#define ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT_CLIENT_ROLE_INIT NULL

#ifdef __cplusplus
}
#endif
