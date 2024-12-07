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

/** @brief Binary Input cluster attribute identifiers.
*/
typedef enum {

    ESP_ZB_ZCL_ATTR_BINARY_INPUT_ACTIVE_TEXT_ID               = 0x0004,    /*!< This attribute holds a human readable description of the ACTIVE state of a binary PresentValue. */
    ESP_ZB_ZCL_ATTR_BINARY_INPUT_DESCRIPTION_ID               = 0x001C,    /*!< The description of the usage of the input, output or value, as appropriate to the cluster. */
    ESP_ZB_ZCL_ATTR_BINARY_INPUT_INACTIVE_TEXT_ID             = 0x002E,    /*!< This attribute holds a human readable description of the INACTIVE state of a binary PresentValue. */
    ESP_ZB_ZCL_ATTR_BINARY_INPUT_OUT_OF_SERVICE_ID            = 0x0051,    /*!< OutOfService attribute */
    ESP_ZB_ZCL_ATTR_BINARY_INPUT_POLARITY_ID                  = 0x0054,    /*!< This attribute indicates the relationship between the physical state of the input (or output as appropriate for the cluster) and the logical state represented by a binary PresentValue attribute, when OutOfService is FALSE. */
    ESP_ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID             = 0x0055,    /*!< PresentValue attribute */
    ESP_ZB_ZCL_ATTR_BINARY_INPUT_RELIABILITY_ID               = 0x0067,    /*!< The attribute indicates whether the PresentValue or the operation of the physical input, output or value in question (as appropriate for the cluster) is reliable. */
    ESP_ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID               = 0x006F,    /*!< StatusFlag attribute */
    ESP_ZB_ZCL_ATTR_BINARY_INPUT_APPLICATION_TYPE_ID          = 0x0100,    /*!< The attribute indicates the specific application usage for this cluster. */
} esp_zb_zcl_binary_input_attr_t;

/** @brief Default value for ActiveText attribute */
#define ESP_ZB_ZCL_BINARY_INPUT_ACTIVE_TEXT_DEFAULT_VALUE {0}

/** @brief Default value for Description attribute */
#define ESP_ZB_ZCL_BINARY_INPUT_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for InactiveText attribute */
#define ESP_ZB_ZCL_BINARY_INPUT_INACTIVE_TEXT_DEFAULT_VALUE {0}

/** @brief OutOfService attribute default value */
#define ESP_ZB_ZCL_BINARY_INPUT_OUT_OF_SERVICE_DEFAULT_VALUE false

/** @brief Default value for Polarity attribute */
#define ESP_ZB_ZCL_BINARY_INPUT_POLARITY_DEFAULT_VALUE ((uint8_t)0)

/** @brief Default value for Reliability attribute */
#define ESP_ZB_ZCL_BINARY_INPUT_RELIABILITY_DEFAULT_VALUE ((uint8_t)0)

/** @brief StatusFlag attribute default value */
#define ESP_ZB_ZCL_BINARY_INPUT_STATUS_FLAG_DEFAULT_VALUE ESP_ZB_ZCL_BINARY_INPUT_STATUS_FLAG_MIN_VALUE

/** @brief StatusFlag attribute minimum value */
#define ESP_ZB_ZCL_BINARY_INPUT_STATUS_FLAG_MIN_VALUE 0

/** @brief StatusFlag attribute maximum value */
#define ESP_ZB_ZCL_BINARY_INPUT_STATUS_FLAG_MAX_VALUE 0x0F

#ifdef __cplusplus
}
#endif
