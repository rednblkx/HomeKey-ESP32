/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTDESP_ZB_ZCL_
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief Multistate Value cluster attribute identifiers */
typedef enum {
    // TODO: Support Text attribute
    // ESP_ZB_ZCL_ATTR_MULTI_VALUE_STATE_TEXT_ID = 0x000e, /*!< Text attribute */
    ESP_ZB_ZCL_ATTR_MULTI_VALUE_DESCRIPTION_ID = 0x001c, /*!< Description attribute */
    // TODO: Support NumberOfStates attribute
    // ESP_ZB_ZCL_ATTR_MULTI_VALUE_NUMBER_OF_STATES_ID = 0x004a, /*!< NumberOfStates attribute */
    ESP_ZB_ZCL_ATTR_MULTI_VALUE_OUT_OF_SERVICE_ID = 0x0051, /*!< OutOfService attribute */
    ESP_ZB_ZCL_ATTR_MULTI_VALUE_PRESENT_VALUE_ID  = 0x0055, /*!< PresentValue attribute */
    // TODO: Support PriorityArray attribute
    // ESP_ZB_ZCL_ATTR_MULTI_VALUE_PRIORITY_ARRAY_ID  = 0x0057, /*!< PriorityArray attribute */
    ESP_ZB_ZCL_ATTR_MULTI_VALUE_RELIABILITY_ID        = 0x0067, /*!< Reliability attribute */
    ESP_ZB_ZCL_ATTR_MULTI_VALUE_RELINQUISH_DEFAULT_ID = 0x0068, /*!< Reliability attribute */
    ESP_ZB_ZCL_ATTR_MULTI_VALUE_STATUS_FLAGS_ID       = 0x006f, /*!< StatusFlags attribute */
    ESP_ZB_ZCL_ATTR_MULTI_VALUE_APPLICATION_TYPE_ID   = 0x0100, /*!< ApplicationType attribute */
} esp_zb_zcl_multi_value_attr_t;

/** @brief Values for StatusFlags attribute */
typedef enum {
    ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_NORMAL         = 0x00, /*!< Normal (default) . */
    ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_IN_ALARM       = 0x01, /*!< In alarm bit. */
    ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_FAULT          = 0x02, /*!< Fault bit. */
    ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_OVERRIDDEN     = 0x04, /*!< Overridden bit. */
    ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_OUT_OF_SERVICE = 0x08, /*!< Out of service bit. */
} esp_zb_zcl_multi_value_status_flags_value_t;

/** @brief Values for Reliability attribute */
typedef enum {
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_NO_FAULT_DETECTED   = 0x00, /*!< No fault detected */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_NO_SENSOR           = 0x01, /*!< No sensor */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_OVER_RANGE          = 0x02, /*!< Over range */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_UNDER_RANGE         = 0x03, /*!< Under range */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_OPEN_LOOP           = 0x04, /*!< Open loop */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_SHORTED_LOOP        = 0x05, /*!< Shorted loop */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_NO_OUTPUT           = 0x06, /*!< No output */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_UNRELIABLE_OTHER    = 0x07, /*!< Unreliable other */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_PROCESS_ERROR       = 0x08, /*!< Process error */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_MULTI_STATE_FAULT   = 0x09, /*!< Mutlistate fault */
    ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_CONFIGURATION_ERROR = 0x0a, /*!< Configuration error */
} esp_zb_zcl_multi_value_reliability_value_t;

/** Multistate Value cluster application types
 *      The ApplicationType attribute indicates the specific application usage
 *      for the cluster. ApplicationType is subdivided into Group, Type and
 *      an Index number, as follows:
 *
 *      Group = Bits 24 to 31
 *        An indication of the cluster this attribute is part of. Multistate Value
 *        clusters have Group = 0x0d.
 *
 *      Type = Bits 16 to 23
 *        The application usage domain.
 *
 *      Index = Bits 0 to 15
 *        The specific application usage of the cluster.
 */

#define ESP_ZB_ZCL_MV_GROUP_ID                         0x0d
#define ESP_ZB_ZCL_MV_SET_APP_TYPE_WITH_ID(_type, _id) ((ESP_ZB_ZCL_MV_GROUP_ID << 24) | ((_type & 0xff) << 16) | (_id & 0xffff))

/** @brief Values for Multistate Value cluster applications type */
typedef enum {
    ESP_ZB_ZCL_MV_APP_TYPE_APP_DOMAIN_HVAC = 0x0000, /*!< Application Domain HVAC */
    /* All other group values are currently reserved. */
} esp_zb_zcl_mv_application_types_t;

typedef enum {
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_0 /*!< On, Off, Auto state */
    = ESP_ZB_ZCL_MV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_MV_APP_TYPE_APP_DOMAIN_HVAC, 0x0000),
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_1,  /*!< Off, Low, Medium, High state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_2,  /*!< Auto, Heat, Cool, Off, Emergency Heat, Fan Only, Max Heat state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_3,  /*!< Occupied, Unoccupied, Standby, Bypass state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_4,  /*!< Inactive, Active, Hold state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_5,  /*!< Auto, Warm-up, Water Flush, Autocalibration, Shutdown Open, Shutdown Closed, Low Limit, Test and Balance state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_6,  /*!< Off, Auto, Heat Control, Heat Only, Cool Only, Fan Only state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_7,  /*!< High, Normal, Low state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_8,  /*!< Occupied, Unoccupied, Startup, Shutdown state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_9,  /*!< Night, Day, Hold state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_10, /*!< Off, Cool, Heat, Auto, Emergency Heat state */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_STATES_GROUP_11, /*!< Shutdown Closed, Shutdown Open, Satisfied, Mixing, Cooling, Heating, Supplemental Heat state */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_MV_APP_DOMAIN_HVAC_OTHER /*!< Other */
    = ESP_ZB_ZCL_MV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_MV_APP_TYPE_APP_DOMAIN_HVAC, 0xffff),
} esp_zb_zcl_mv_app_domain_hvac_t;

/** @brief Default value for Description attribute */
#define ESP_ZB_ZCL_MULTI_VALUE_DESCRIPTION_DEFAULT_VALUE 0

/** @brief Default value for OutOfService attribute */
#define ESP_ZB_ZCL_MULTI_VALUE_OUT_OF_SERVICE_DEFAULT_VALUE false

/** @brief Default value for Reliability attribute */
#define ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_DEFAULT_VALUE ESP_ZB_ZCL_MULTI_VALUE_RELIABILITY_NO_FAULT_DETECTED

/** @brief Default value for StatusFlags attribute */
#define ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_DEFAULT_VALUE ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_NORMAL

/** @brief StatusFlags attribute minimum value */
#define ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_MIN_VALUE 0

/** @brief StatusFlags attribute maximum value */
#define ESP_ZB_ZCL_MULTI_VALUE_STATUS_FLAGS_MAX_VALUE 0x0f

/*! Maximum length of Description string field */
#define ESP_ZB_ZCL_MULTI_VALUE_DESCRIPTION_MAX_LEN 16

#ifdef __cplusplus
}
#endif
