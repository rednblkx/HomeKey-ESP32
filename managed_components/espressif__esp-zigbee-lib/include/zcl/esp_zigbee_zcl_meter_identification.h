/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief Meter Identification server attributes identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_COMPANY_NAME_ID      = 0x0000, /**< Company Name */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_METER_TYPE_ID_ID     = 0x0001, /**< Meter Type Id */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID_ID   = 0x0004, /**< Data Quality Id */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_CUSTOMER_NAME_ID     = 0x0005, /**< Customer Name */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_MODEL_ID             = 0x0006, /**< Model */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_PART_NUMBER_ID       = 0x0007, /**< Part Number */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_PRODUCT_REVISION_ID  = 0x0008, /**< Product Revision */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_SOFTWARE_REVISION_ID = 0x000A, /**< Software Revision */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_UTILITY_NAME_ID      = 0x000B, /**< Utility Name */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_POD_ID               = 0x000C, /**< Pod */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER_ID   = 0x000D, /**< Available Power */
    ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD_ID   = 0x000E, /**< Power Threshold */
} esp_zb_zcl_meter_identification_attr_t;

/** @brief ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_METER_TYPE_ID "MeterTypeID" attribute values */
typedef enum {
    ESP_ZB_ZCL_METER_TYPE_UTILITY_PRIMARY    = 0x0000,  /**< Utility Primary Meter */
    ESP_ZB_ZCL_METER_TYPE_UTILITY_PRODUCTION = 0x0001,  /**< Utility Production Meter */
    ESP_ZB_ZCL_METER_TYPE_UTILITY_SECONDARY  = 0x0002,  /**< Utility Secondary Meter */
    ESP_ZB_ZCL_METER_TYPE_PRIVATE_PRIMARY    = 0x0100,  /**< Private Primary Meter */
    ESP_ZB_ZCL_METER_TYPE_PRIVATE_PRODUCTION = 0x0101,  /**< Private Production Meter */
    ESP_ZB_ZCL_METER_TYPE_PRIVATE_SECONDARY  = 0x0102,  /**< Private Secondary Meters */
    ESP_ZB_ZCL_METER_TYPE_GENERIC            = 0x0110   /**< Generic Meter */
} esp_zb_zcl_meter_identification_meter_type_t;

/** @brief ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID "DataQualityID" attribute values */
typedef enum {
    ESP_ZB_ZCL_DATA_QUALITY_ALL_DATA_CERTIFIED                       = 0x0000,  /**< All Data Certified */
    ESP_ZB_ZCL_DATA_QUALITY_ONLY_INSTANTANEOUS_POWER_NOT_CERTIFIED   = 0x0001,  /**< Only Instantaneous Power not Certified */
    ESP_ZB_ZCL_DATA_QUALITY_ONLY_CUMULATED_CONSUMPTION_NOT_CERTIFIED = 0x0002,  /**< Only Cumulated Consumption not Certified */
    ESP_ZB_ZCL_DATA_QUALITY_NOT_CERTIFIED_DATA                       = 0x0003   /**< Not Certified data */
} esp_zb_zcl_meter_identification_data_quality_t;

/** @brief Default value for Meter Identification cluster revision global attribute */
#define ESP_ZB_ZCL_METER_IDENTIFICATION_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Default value for Company Name attribute */
#define ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_COMPANY_NAME_DEFAULT_VALUE { 0 }

/** @brief Default value for Meter Type ID attribute */
#define ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_METER_TYPE_ID_DEFAULT_VALUE 0

/** @brief Default value for Data Quality ID attribute */
#define ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID_DEFAULT_VALUE 0

/** @brief Default value for POD (Point of Delivery) attribute */
#define ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_POD_DEFAULT_VALUE { 0 }

/** @brief Default value for Available Power attribute */
#define ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER_DEFAULT_VALUE ZB_INIT_UINT24(0,0)

/** @brief Default value for Power Threshold attribute */
#define ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD_DEFAULT_VALUE ZB_INIT_UINT24(0, 0)

/** @brief Max length of Company Name attribute */
#define ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_COMPANY_NAME_MAX_LENGTH 16

/** @brief Max length of POD (Point of Delivery) attribute */
#define ESP_ZB_ZCL_ATTR_METER_IDENTIFICATION_POD_MAX_LENGTH 16

#ifdef __cplusplus
}
#endif
