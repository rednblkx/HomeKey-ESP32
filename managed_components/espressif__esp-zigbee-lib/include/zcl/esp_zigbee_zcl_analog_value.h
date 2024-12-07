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

/** @brief Analog Value cluster attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID        = 0x001c, /*!< Description attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID     = 0x0051, /*!< OutOfService attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID      = 0x0055, /*!< PresentValue attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_PRIORITY_ARRAY_ID     = 0x0057, /*!< PriorityArray attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_RELIABILITY_ID        = 0x0067, /*!< Reliability attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_RELINQUISH_DEFAULT_ID = 0x0068, /*!< RelinquishDefault attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID       = 0x006f, /*!< StatusFlags attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_ENGINEERING_UNITS_ID  = 0x0075, /*!< EngineeringUnits attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_VALUE_APPLICATION_TYPE_ID   = 0x0100, /*!< ApplicationType attribute */
} esp_zb_zcl_analog_value_attr_t;

/** @brief Values for StatusFlags attribute */
typedef enum {
    ESP_ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_NORMAL         = 0x00, /*!< Normal (default) state. */
    ESP_ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_IN_ALARM       = 0x01, /*!< In alarm bit. */
    ESP_ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_FAULT          = 0x02, /*!< Fault bit. */
    ESP_ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_OVERRIDDEN     = 0x04, /*!< Overridden bit. */
    ESP_ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_OUT_OF_SERVICE = 0x08, /*!< Out of service bit. */
} esp_zb_zcl_analog_value_status_flags_value_t;

/** @brief Values for Reliability attribute */
typedef enum {
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_NO_FAULT_DETECTED = 0x00, /*!< No fault detected */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_NO_SENSOR         = 0x01, /*!< No sensor */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_OVER_RANGE        = 0x02, /*!< Over range */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_UNDER_RANGE       = 0x03, /*!< Under range */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_OPEN_LOOP         = 0x04, /*!< Open loop */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_SHORTED_LOOP      = 0x05, /*!< Shorted loop */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_NO_OUTPUT         = 0x06, /*!< No output */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_UNRELIABLE_OTHER  = 0x07, /*!< Unreliable other */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_PROCESS_ERROR     = 0x08, /*!< Process error */
    /* 0x09 is for multistate clusters only */
    ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_CONFIGURATION_ERROR = 0x0a, /*!< Configuration error */
} esp_zb_zcl_analog_value_reliability_value_t;

/** Analog Value cluster application types
 *      The ApplicationType attribute indicates the specific application usage
 *      for the cluster. ApplicationType is subdivided into Group, Type and
 *      an Index number, as follows:
 *
 *      - Group = Bits 24 to 31
 *        An indication of the cluster this attribute is part of. Analog Value
 *        clusters have Group = 0x02.
 *
 *      - Type = Bits 16 to 23
 *        The physical quantity that the PresentValue attribute of the cluster
 *        represents.
 *
 *      - Index = Bits 0 to 15
 *        The specific application usage of the cluster.
 */

#define ESP_ZB_ZCL_AV_GROUP_ID 0x02
#define ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(_type, _id) ((ESP_ZB_ZCL_AV_GROUP_ID << 24) | ((_type & 0xff) << 16) | (_id & 0xffff))

/** @brief Values for Analog Value cluster applications type */
typedef enum {
    ESP_ZB_ZCL_AV_APP_TYPE_TEMPERATURE = 0x00, /*!< Temperature */
    ESP_ZB_ZCL_AV_APP_TYPE_AREA        = 0x01, /*!< Area */
    ESP_ZB_ZCL_AV_APP_TYPE_MULTIPLIER  = 0x02, /*!< Multiplier */
    ESP_ZB_ZCL_AV_APP_TYPE_FLOW        = 0x03, /*!< Flow */
    ESP_ZB_ZCL_AV_APP_TYPE_OTHER       = 0xff, /*!< Other */
} esp_zb_zcl_av_application_types_t;

/** @brief Values for 'Temperature in degrees Celsius' type of Analog Value cluster */
typedef enum {
    ESP_ZB_ZCL_AV_TEMPERATURE_SETPOINT_OFFSET_0 = ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AV_APP_TYPE_TEMPERATURE, 0x0000), /*!< Setpoint Offset */
    ESP_ZB_ZCL_AV_TEMPERATURE_TEMP_DEADBAND,                                                                                      /*!< Temp deadband */
    ESP_ZB_ZCL_AV_TEMPERATURE_OCCUPIED_HEATING_SETPOINT,                                                                          /*!< Occupied heating setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_UNOCCUPIED_HEATING_SETPOINT,                                                                        /*!< Unoccupied heating setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_OCCUPIED_COOLING_SETPOINT,                                                                          /*!< Occupied cooling setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_UNOCCUPIED_COOLING_SETPOINT,                                                                        /*!< Unoccupied cooling setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_STANDBY_HEAT_SETPOINT,                                                                              /*!< Standby heat setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_STANDBY_COOLING_SETPOINT,                                                                           /*!< Standby cooling setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_OCCUPIED_HEATING_SETPOINT,                                                                /*!< Effective occupied heating setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_UNOCCUPIED_HEATING_SETPOINT,                                                              /*!< Effective unoccupied heating setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_OCCUPIED_COOLING_SETPOINT,                                                                /*!< Effective occupied cooling setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_UNOCCUPIED_COOLING_SETPOINT,                                                              /*!< Effective unoccupied cooling setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_STANDBY_HEAT_SETPOINT,                                                                    /*!< Effective standby heat setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_STANDBY_COOLING_SETPOINT,                                                                 /*!< Effective standby cooling setpoint */
    ESP_ZB_ZCL_AV_TEMPERATURE_SETPOINT_OFFSET_2,                                                                                  /*!< Setpoint offset 2 */
    ESP_ZB_ZCL_AV_TEMPERATURE_SETPOINT_SHIFT,                                                                                     /*!< Setpoint shift */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AV_TEMPERATURE_OTHER = ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AV_APP_TYPE_TEMPERATURE, 0xffff), /*!< Other*/
} esp_zb_zcl_av_temperature_in_degrees_celsius_t;

/** @brief Values for 'Area in Square Metres' type of Analog Value cluster */
typedef enum {
    ESP_ZB_ZCL_AV_AREA_DUCT_AREA = ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AV_APP_TYPE_AREA, 0x0000), /*!< Duct Area */
    /* 0x0200 to 0xffffe are Vendor defined */
    ESP_ZB_ZCL_AV_AREA_OTHER = ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AV_APP_TYPE_AREA, 0xffff), /*!< Other */
} esp_zb_zcl_av_area_in_squares_meters_t;

/** @brief Values for 'Multiplier - Number' type of Analog Value cluster */
typedef enum {
    ESP_ZB_ZCL_AV_MULTIPLIER_DUCT_MULTIPLIER = ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AV_APP_TYPE_MULTIPLIER, 0x0000), /*!< Duct Area */
    /* 0x0200 to 0xffffe are Vendor defined */
    ESP_ZB_ZCL_AV_MULTIPLIER_OTHER = ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AV_APP_TYPE_MULTIPLIER, 0xffff), /*!< Other */
} esp_zb_zcl_av_multiplier_number_t;

/** @brief Values for 'Flow in Litres per Second' type of Analog Value cluster */
typedef enum {
    ESP_ZB_ZCL_AV_FLOW_MINIMUM_AIR_FLOW = ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AV_APP_TYPE_FLOW, 0x0000), /*!< Minimum Air Flow */
    ESP_ZB_ZCL_AV_FLOW_MAXIMUM_AIR_FLOW,                                                                           /*!< Maximum Air Flow */
    ESP_ZB_ZCL_AV_FLOW_HEATING_MINIMUM_AIR_FLOW,                                                                   /*!< Heating Minimum Air Flow */
    ESP_ZB_ZCL_AV_FLOW_HEATING_MAXIMUM_AIR_FLOW,                                                                   /*!< Heating Maximum Air Flow */
    ESP_ZB_ZCL_AV_FLOW_STANDBY_MINIMUM_AIR_FLOW,                                                                   /*!< Standby Minimum Air Flow */
    ESP_ZB_ZCL_AV_FLOW_STANDBY_MAXIMUM_AIR_FLOW,                                                                   /*!< Standby Maximum Air Flow */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AV_FLOW_OTHER = ESP_ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AV_APP_TYPE_FLOW, 0xffff), /*!< Other */
} esp_zb_zcl_av_flow_in_liters_per_second_t;

/** @brief Default value for Description attribute */
#define ESP_ZB_ZCL_ANALOG_VALUE_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for OutOfService attribute */
#define ESP_ZB_ZCL_ANALOG_VALUE_OUT_OF_SERVICE_DEFAULT_VALUE false

/** @brief Default value for Reliability attribute */
#define ESP_ZB_ZCL_ANALOG_VALUE_RELIABILITY_DEFAULT_VALUE 0x00

/** @brief Default value for StatusFlags attribute */
#define ESP_ZB_ZCL_ANALOG_VALUE_STATUS_FLAGS_DEFAULT_VALUE 0

/** @brief StatusFlags attribute minimum value */
#define ESP_ZB_ZCL_ANALOG_VALUE_STATUS_FLAGS_MIN_VALUE 0

/** @brief StatusFlags attribute maximum value */
#define ESP_ZB_ZCL_ANALOG_VALUE_STATUS_FLAGS_MAX_VALUE 0x0f

#ifdef __cplusplus
}
#endif
