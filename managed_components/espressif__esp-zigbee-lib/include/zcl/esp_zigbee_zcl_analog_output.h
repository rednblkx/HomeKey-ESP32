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

/** @brief Analog Output cluster attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_DESCRIPTION_ID        = 0x001c, /*!< Description attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_MAX_PRESENT_VALUE_ID  = 0x0041, /*!< MaxPresentValue attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_MIN_PRESENT_VALUE_ID  = 0x0045, /*!< MinPresentValue attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_OUT_OF_SERVICE_ID     = 0x0051, /*!< OutOfService attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_PRESENT_VALUE_ID      = 0x0055, /*!< PresentValue attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_PRIORITY_ARRAY_ID     = 0x0057, /*!< PriorityArray attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_RELIABILITY_ID        = 0x0067, /*!< Reliability attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_RELINQUISH_DEFAULT_ID = 0x0068, /*!< RelinquishDefault attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_RESOLUTION_ID         = 0x006a, /*!< Resolution attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_STATUS_FLAGS_ID       = 0x006f, /*!< StatusFlags attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_ENGINEERING_UNITS_ID  = 0x0075, /*!< EngineeringUnits attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_OUTPUT_APPLICATION_TYPE_ID   = 0x0100, /*!< ApplicationType attribute */
} esp_zb_zcl_analog_output_attr_t;

/** @brief Values for StatusFlags attribute */
typedef enum {
    ESP_ZB_ZCL_ANALOG_OUTPUT_STATUS_FLAG_NORMAL         = 0x00, /*!< Normal (default) state */
    ESP_ZB_ZCL_ANALOG_OUTPUT_STATUS_FLAG_IN_ALARM       = 0x01, /*!< In alarm bit */
    ESP_ZB_ZCL_ANALOG_OUTPUT_STATUS_FLAG_FAULT          = 0x02, /*!< Fault bit */
    ESP_ZB_ZCL_ANALOG_OUTPUT_STATUS_FLAG_OVERRIDDEN     = 0x04, /*!< Overridden bit */
    ESP_ZB_ZCL_ANALOG_OUTPUT_STATUS_FLAG_OUT_OF_SERVICE = 0x08, /*!< Out of service bit */
} esp_zb_zcl_analog_output_status_flags_value_t;

/** @brief Values for Reliability attribute  */
typedef enum {
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_NO_FAULT_DETECTED = 0x00, /*!< No fault detected */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_NO_SENSOR         = 0x01, /*!< No sensor */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_OVER_RANGE        = 0x02, /*!< Over range */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_UNDER_RANGE       = 0x03, /*!< Under range */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_OPEN_LOOP         = 0x04, /*!< Open loop */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_SHORTED_LOOP      = 0x05, /*!< Shorted loop */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_NO_OUTPUT         = 0x06, /*!< No output */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_UNRELIABLE_OTHER  = 0x07, /*!< Unreliable other */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_PROCESS_ERROR     = 0x08, /*!< Process error */
    /* 0x09 is for multistate clusters only */
    ESP_ZB_ZCL_ANALOG_OUTPUT_RELIABILITY_CONFIGURATION_ERROR = 0x0a, /*!< Configuration error */
} esp_zb_zcl_analog_output_reliability_value_t;

/** Analog Output cluster application types
 *      The ApplicationType attribute indicates the specific application usage
 *      for the cluster. ApplicationType is subdivided into Group, Type and
 *      an Index number, as follows:
 *
 *      Group = Bits 24 to 31
 *        An indication of the cluster this attribute is part of. Analog Output
 *        clusters have Group = 0x01.
 *
 *      Type = Bits 16 to 23
 *        The physical quantity that the PresentValue attribute of the cluster
 *        represents.
 *
 *      Index = Bits 0 to 15
 *        The specific application usage of the cluster.
 */

#define ESP_ZB_ZCL_AO_GROUP_ID 0x01
#define ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(_type, _id) (((_type & 0xff) << 16) | (_id & 0xffff))

/** @brief Values for Analog Input cluster applications type */
typedef enum {
    ESP_ZB_ZCL_AO_APP_TYPE_TEMPERATURE,        /*!< Temperature */
    ESP_ZB_ZCL_AO_APP_TYPE_HUMIDITY,           /*!< Humidity */
    ESP_ZB_ZCL_AO_APP_TYPE_PRESSURE,           /*!< Pressure */
    ESP_ZB_ZCL_AO_APP_TYPE_FLOW,               /*!< Flow */
    ESP_ZB_ZCL_AO_APP_TYPE_PERCENTAGE,         /*!< Percentage */
    ESP_ZB_ZCL_AO_APP_TYPE_PPM,                /*!< Ppm */
    ESP_ZB_ZCL_AO_APP_TYPE_RPM,                /*!< Rpm */
    ESP_ZB_ZCL_AO_APP_TYPE_CURRENT_IN_AMPS,    /*!< Current in amps */
    ESP_ZB_ZCL_AO_APP_TYPE_FREQUENCY,          /*!< Frequency */
    ESP_ZB_ZCL_AO_APP_TYPE_POWER_IN_WATTS,     /*!< Power in watts */
    ESP_ZB_ZCL_AO_APP_TYPE_POWER_IN_KILOWATTS, /*!< Power in kilowatts */
    ESP_ZB_ZCL_AO_APP_TYPE_ENERGY,             /*!< Energy */
    ESP_ZB_ZCL_AO_APP_TYPE_COUNT_UNITLESS,     /*!< Count unitless */
    ESP_ZB_ZCL_AO_APP_TYPE_ENTHALPY,           /*!< Enthalpy */
    ESP_ZB_ZCL_AO_APP_TYPE_TIME,               /*!< Time */
    /* Types 0x0f to 0xfe are reserved */
    ESP_ZB_ZCL_AO_APP_TYPE_OTHER = 0xff, /*!< Other */
} esp_zb_zcl_ao_application_types_t;

/** @brief Values for 'Temperature in degrees Celsius' type of Analog Output cluster */
typedef enum {
    ESP_ZB_ZCL_AO_TEMPERATURE_BOILER = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_TEMPERATURE, 0x0000), /*!< Boiler */
    ESP_ZB_ZCL_AO_TEMPERATURE_BOILER_SETPOINT,                                                                         /*!< Boiler setpoint */
    ESP_ZB_ZCL_AO_TEMPERATURE_COLD_DECK,                                                                               /*!< Cold deck */
    ESP_ZB_ZCL_AO_TEMPERATURE_CHILLER_SETPOINT_0,                                                                      /*!< Chiller setpoint 0. Note: Chiller Setpoint is duplicated in spec */
    ESP_ZB_ZCL_AO_TEMPERATURE_CHILLER_SETPOINT_1,                                                                      /*!< Chiller setpoint 1 */
    ESP_ZB_ZCL_AO_TEMPERATURE_HOT_DECK,                                                                                /*!< Hot deck */
    ESP_ZB_ZCL_AO_TEMPERATURE_COOLING_VALVE,                                                                           /*!< Cooling valve */
    ESP_ZB_ZCL_AO_TEMPERATURE_ZONE_TEMPERATURE_SETPOINT,                                                               /*!< Zone temperature setpoint */
    ESP_ZB_ZCL_AO_TEMPERATURE_SETPOINT_OFFSET,                                                                         /*!< Setpoint offset */
    ESP_ZB_ZCL_AO_TEMPERATURE_SETPOINT_SHIFT,                                                                          /*!< Setpoint shift */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AO_TEMPERATURE_OTHER = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_TEMPERATURE, 0xffff), /*!< Other */
} esp_zb_zcl_ao_temperature_in_degrees_celsius_t;

/** @brief Values for 'Relative Humidity in %' type of Analog Output cluster */
typedef enum {
    ESP_ZB_ZCL_AO_HUMIDITY_HUMIDIFICATION = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_HUMIDITY, 0x0000), /*!< Humidification */
    ESP_ZB_ZCL_AO_HUMIDITY_ZONE_RELATIVE_HUMIDITY,                                                                       /*!< Zone relative humidity */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AO_HUMIDITY_OTHER = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_HUMIDITY, 0xffff), /*!< Other */
} esp_zb_zcl_ao_relative_humidity_in_percent_t;

/** @brief Values for 'Percentage %' type of Analog Output cluster */
typedef enum {
    ESP_ZB_ZCL_AO_PERCENTAGE_FACE_BYPASS_DAMPER = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_PERCENTAGE, 0x0000), /*!< Face bypass damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_HEAT_RECOVERY_VALVE,                                                                                /*!< Heat recovery valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_HEAT_RECOVERY_WHELL,                                                                                /*!< Heat recovery whell */
    ESP_ZB_ZCL_AO_PERCENTAGE_HEATING_VALVE,                                                                                      /*!< Heating valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_HOT_DECK_DAMPER,                                                                                    /*!< Hot deck damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_2_PIPE_DAMPER,                                                                                      /*!< 2 pipe damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_2_PIPE_VALVE,                                                                                       /*!< 2 pipe valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_BOILER_MIXING_VALVE,                                                                                /*!< Boiler mixing valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_BOX_COOLING_VALVE,                                                                                  /*!< Box cooling valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_BOX_HEATING_VALVE,                                                                                  /*!< Box heating valve */

    ESP_ZB_ZCL_AO_PERCENTAGE_CHILLED_WATER_BYPASS_VALVE, /*!< Chilled water bypass valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_COLD_DECK_DAMPER,           /*!< Cold deck damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_COOLING_DAMPER,             /*!< Cooling damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_COOLING_VALVE,              /*!< Cooling valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_DAMPER,                     /*!< Damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_EXHAUST_AIR_DAMPER,         /*!< Exhaust air damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_EXHAUST_DAMPER,             /*!< Exhaust damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_HOT_WATER_BYPASS_VALVE,     /*!< Hot water bypass valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_HOT_WATER_MIXING_VALVE,     /*!< Hot water mixing valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_MINIMUM_OUTSIDE_AIR_DAMPER, /*!< Minimum outside air damper */

    ESP_ZB_ZCL_AO_PERCENTAGE_MINIMUM_OUTSIDE_AIR_FAN,    /*!< Minimum outside air fan */
    ESP_ZB_ZCL_AO_PERCENTAGE_MIXED_AIR_DAMPER,           /*!< Mixed air damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_MIXING_VALVE,               /*!< Mixing valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_OUTSIDE_AIR_DAMPER,         /*!< Ourside air damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_PRIMARY_CHILLED_WATER_PUMP, /*!< Primary chilled water pump */
    ESP_ZB_ZCL_AO_PERCENTAGE_PRIMARY_HOT_WATER_PUMP,     /*!< Primary hot water pump */
    ESP_ZB_ZCL_AO_PERCENTAGE_PRIMARY_HEAT_EXCHANGE_PUMP, /*!< Primary heat exchange pump */
    ESP_ZB_ZCL_AO_PERCENTAGE_PREHEAT_DAMPER,             /*!< Preheat damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_PREHEAT_VALVE,              /*!< Preheat valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_REHEAT_VALVE_0,             /*!< Reheat valve 0. Note: Reheat valve is duplicated in spec */

    ESP_ZB_ZCL_AO_PERCENTAGE_REHEAT_VALVE_1,               /*!< Reheat valve 1 */
    ESP_ZB_ZCL_AO_PERCENTAGE_RETURN_AIR_DAMPER,            /*!< Return air damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_SECONDARY_CHILLED_WATER_PUMP, /*!< Secondary chilled water pump */
    ESP_ZB_ZCL_AO_PERCENTAGE_SEQUENCED_VALVE,              /*!< Sequenced valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_SECONDARY_HOT_WATER_PUMP,     /*!< Secondary hot water pump */
    ESP_ZB_ZCL_AO_PERCENTAGE_SECONDARY_HEAT_EXCHANGE_PUMP, /*!< Secondary heat exchange pump */
    ESP_ZB_ZCL_AO_PERCENTAGE_SIDELOOP,                     /*!< Sideloop */
    ESP_ZB_ZCL_AO_PERCENTAGE_SUPPLY_HEATING_VALVE,         /*!< Supply heating valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_SUPPLY_DAMPER,                /*!< Supply damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_TOWER_BYPASS_VALVE,           /*!< Tower bypass valve */

    ESP_ZB_ZCL_AO_PERCENTAGE_TOWER_FAN,                               /*!< Tower fan */
    ESP_ZB_ZCL_AO_PERCENTAGE_VALVE,                                   /*!< Value */
    ESP_ZB_ZCL_AO_PERCENTAGE_ZONE_1_DAMPER,                           /*!< Zone 1 damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_ZONE_1_HEATING_VALVE,                    /*!< Zone 1 heating valve */
    ESP_ZB_ZCL_AO_PERCENTAGE_HEAT_RECOVERY_EXHAUST_BYPASS_DAMPER,     /*!< Heat recovery exhaust bypass damper */
    ESP_ZB_ZCL_AO_PERCENTAGE_HEAT_RECOVERY_OUTSIDE_AIR_BYPASS_DAMPER, /*!< Heat recovery outside air bypass damper */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AO_PERCENTAGE_OTHER = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_PERCENTAGE, 0xffff), /*!< Other */
} esp_zb_zcl_ao_percentage_t;

/** @brief Values for 'Parts per Million PPM' type of Analog Output cluster */
typedef enum {
    ESP_ZB_ZCL_AO_PPM_SPACE_CARBON_DIOXIDE_LIMIT = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_PPM, 0x0000), /*!< Space carbon dioxide limit */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AO_PPM_OTHER = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_PPM, 0xffff), /*!< Other */
} esp_zb_zcl_ao_ppm_t;

/** @brief Values for 'Rotational Speed in RPM' type of Analog Output cluster */
typedef enum {
    ESP_ZB_ZCL_AO_RPM_EXHAUST_FAN_SPEED = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_RPM, 0x0000), /*!< Exhaust fan speed */
    ESP_ZB_ZCL_AO_RPM_FAN_SPEED,                                                                                  /*!< Fan speed */
    ESP_ZB_ZCL_AO_RPM_RELIEF_FAN_SPEED,                                                                           /*!< Relief fan speed */
    ESP_ZB_ZCL_AO_RPM_RETURN_FAN_SPEED,                                                                           /*!< Return fan speed */
    ESP_ZB_ZCL_AO_RPM_SUPPLY_FAN_SPEED,                                                                           /*!< Supply fan speed */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AO_RPM_OTHER = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_RPM, 0xffff), /*!< Other */
} esp_zb_zcl_ao_rpm_t;

/** @brief Values for 'Time in Seconds' type of Analog Output cluster */
typedef enum {
    ESP_ZB_ZCL_AO_TIME_RELATIVE = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_TIME, 0x0000), /*!< Relative */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AO_TIME_OTHER = ESP_ZB_ZCL_AO_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AO_APP_TYPE_TIME, 0xffff), /*!< Other */
} esp_zb_zcl_ao_time_t;

/** @brief Default value for Description attribute */
#define ESP_ZB_ZCL_ANALOG_OUTPUT_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for OutOfService attribute */
#define ESP_ZB_ZCL_ANALOG_OUTPUT_OUT_OF_SERVICE_DEFAULT_VALUE false

/** @brief Default value for StatusFlags attribute */
#define ESP_ZB_ZCL_ANALOG_OUTPUT_STATUS_FLAG_DEFAULT_VALUE 0

/** @brief StatusFlags attribute minimum value */
#define ESP_ZB_ZCL_ANALOG_OUTPUT_STATUS_FLAGS_MIN_VALUE 0

/** @brief StatusFlags attribute maximum value */
#define ESP_ZB_ZCL_ANALOG_OUTPUT_STATUS_FLAGS_MAX_VALUE 0x0f

/** Number of attributes mandatory for reporting in Analog Output cluster */
#define ESP_ZB_ZCL_ANALOG_OUTPUT_REPORT_ATTR_COUNT 2

void esp_zb_zcl_analog_output_init_server(void);
void esp_zb_zcl_analog_output_init_client(void);
#define ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT_SERVER_ROLE_INIT esp_zb_zcl_analog_output_init_server
#define ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT_CLIENT_ROLE_INIT esp_zb_zcl_analog_output_init_client

#ifdef __cplusplus
}
#endif
