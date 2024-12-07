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

/** @brief Analog Input cluster attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_DESCRIPTION_ID       = 0x001c, /*!< Description attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_MAX_PRESENT_VALUE_ID = 0x0041, /*!< MaxPresentValue attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_MIN_PRESENT_VALUE_ID = 0x0045, /*!< MinPresentValue attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID    = 0x0051, /*!< OutOfService attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID     = 0x0055, /*!< PresentValue attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_RELIABILITY_ID       = 0x0067, /*!< Reliability attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_RESOLUTION_ID        = 0x006a, /*!< Resolution attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID      = 0x006f, /*!< StatusFlags attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_ENGINEERING_UNITS_ID = 0x0075, /*!< EngineeringUnits attribute */
    ESP_ZB_ZCL_ATTR_ANALOG_INPUT_APPLICATION_TYPE_ID  = 0x0100, /*!< ApplicationType attribute */
} esp_zb_zcl_analog_input_attr_t;

/** @brief Values for StatusFlags attribute */
typedef enum {
    ESP_ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_NORMAL         = 0x00, /*!< Normal (default) state. */
    ESP_ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_IN_ALARM       = 0x01, /*!< In alarm bit. */
    ESP_ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_FAULT          = 0x02, /*!< Fault bit. */
    ESP_ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OVERRIDDEN     = 0x04, /*!< Overridden bit. */
    ESP_ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OUT_OF_SERVICE = 0x08, /*!< Out of service bit. */
} esp_zb_zcl_analog_input_status_flags_value_t;

/** @brief Values for Reliability attribute */
typedef enum {
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_NO_FAULT_DETECTED = 0x00, /*!< No fault detected */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_NO_SENSOR         = 0x01, /*!< No sensor */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_OVER_RANGE        = 0x02, /*!< Over range */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_UNDER_RANGE       = 0x03, /*!< Under range */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_OPEN_LOOP         = 0x04, /*!< Open loop */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_SHORTED_LOOP      = 0x05, /*!< Shorted loop */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_NO_OUTPUT         = 0x06, /*!< No output */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_UNRELIABLE_OTHER  = 0x07, /*!< Unreliable other */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_PROCESS_ERROR     = 0x08, /*!< Process error */
    /* 0x09 is for multistate clusters only */
    ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_CONFIGURATION_ERROR = 0x0a, /*!< Configuration error */
} esp_zb_zcl_analog_input_reliability_value_t;

/** Analog Input cluster application types
 *      The ApplicationType attribute indicates the specific application usage
 *      for the cluster. ApplicationType is subdivided into Group, Type and
 *      an Index number, as follows:
 *
 *      Group = Bits 24 to 31
 *        An indication of the cluster this attribute is part of. Analog Input
 *        clusters have Group = 0x00.
 *
 *      Type = Bits 16 to 23
 *        The physical quantity that the PresentValue attribute of the cluster
 *        represents.
 *
 *      Index = Bits 0 to 15
 *        The specific application usage of the cluster.
 */

#define ESP_ZB_ZCL_AI_GROUP_ID 0x00
#define ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(_type, _id) (((_type & 0xff) << 16) | (_id & 0xffff))

/** @brief Values for Analog Input cluster applications type*/
typedef enum {
    ESP_ZB_ZCL_AI_APP_TYPE_TEMPERATURE,        /*!< Temperature */
    ESP_ZB_ZCL_AI_APP_TYPE_HUMIDITY,           /*!< Humidity */
    ESP_ZB_ZCL_AI_APP_TYPE_PRESSURE,           /*!< Pressure */
    ESP_ZB_ZCL_AI_APP_TYPE_FLOW,               /*!< Flow */
    ESP_ZB_ZCL_AI_APP_TYPE_PERCENTAGE,         /*!< Percentage */
    ESP_ZB_ZCL_AI_APP_TYPE_PPM,                /*!< Ppm */
    ESP_ZB_ZCL_AI_APP_TYPE_RPM,                /*!< Rpm */
    ESP_ZB_ZCL_AI_APP_TYPE_CURRENT_IN_AMPS,    /*!< Current In AMPS */
    ESP_ZB_ZCL_AI_APP_TYPE_FREQUENCY,          /*!< Frequence */
    ESP_ZB_ZCL_AI_APP_TYPE_POWER_IN_WATTS,     /*!< Power In Watts */
    ESP_ZB_ZCL_AI_APP_TYPE_POWER_IN_KILOWATTS, /*!< Power In Kilowatts */
    ESP_ZB_ZCL_AI_APP_TYPE_ENERGY,             /*!< Energy */
    ESP_ZB_ZCL_AI_APP_TYPE_COUNT_UNITLESS,     /*!< Count Unitless */
    ESP_ZB_ZCL_AI_APP_TYPE_ENTHALPY,           /*!< Enthalpy */
    ESP_ZB_ZCL_AI_APP_TYPE_TIME,               /*!< Time */
    /* Types 0x0f to 0xfe are reserved */
    ESP_ZB_ZCL_AI_APP_TYPE_OTHER = 0xff, /*!< Other */
} esp_zb_zcl_ai_application_types_t;

/** @brief Values for 'Temperature in degrees Celsius' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_TEMPERATURE_2_PIPE_ENTERING = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_TEMPERATURE, 0x0000), /*!< 2 Pipe Entering */
    ESP_ZB_ZCL_AI_TEMPERATURE_2_PIPE_LEAVING,                                                                                   /*!< 2 Pipe Leaving */
    ESP_ZB_ZCL_AI_TEMPERATURE_BOILER_ENTERING,                                                                                  /*!< Boiler Entering */
    ESP_ZB_ZCL_AI_TEMPERATURE_BOILER_LEAVING,                                                                                   /*!< Boiler Leaving */
    ESP_ZB_ZCL_AI_TEMPERATURE_CHILLER_CHILLED_WATER_ENTERING,                                                                   /*!< Chiller Chilled Water Entering */
    ESP_ZB_ZCL_AI_TEMPERATURE_CHILLER_CHILLED_WATER_LEAVING,                                                                    /*!< Chiller Chilled Water Leaving */
    ESP_ZB_ZCL_AI_TEMPERATURE_CHILLER_CONDENSER_WATER_ENTERING,                                                                 /*!< Chiller Condenser Water Entering */
    ESP_ZB_ZCL_AI_TEMPERATURE_CHILLER_CONDENSER_WATER_LEAVING,                                                                  /*!< Chiller Condenser Water Leaving */
    ESP_ZB_ZCL_AI_TEMPERATURE_COLD_DECK,                                                                                        /*!< Cold Deck */
    ESP_ZB_ZCL_AI_TEMPERATURE_COOLING_COIL_DISCHARGE,                                                                           /*!< Cooling Coil Discharge */

    ESP_ZB_ZCL_AI_TEMPERATURE_COOLING_ENTERING_WATER, /*!< Cooling entering water */
    ESP_ZB_ZCL_AI_TEMPERATURE_COOLING_LEAVING_WATER,  /*!< Cooling leaving Water */
    ESP_ZB_ZCL_AI_TEMPERATURE_CONDENSER_WATER_RETURN, /*!< Condenser water return */
    ESP_ZB_ZCL_AI_TEMPERATURE_CONDENSER_WATER_SUPPLY, /*!< Condenser water supply */
    ESP_ZB_ZCL_AI_TEMPERATURE_DECOUPLE_LOOP_0,        /*!< Decouple loop 0. Note: Decouple loop is duplicated in spec */
    ESP_ZB_ZCL_AI_TEMPERATURE_BUILDING_LOAD,          /*!< Building load */
    ESP_ZB_ZCL_AI_TEMPERATURE_DECOUPLE_LOOP_1,        /*!< Decouple loop 1 */
    ESP_ZB_ZCL_AI_TEMPERATURE_DEW_POINT,              /*!< Dew point */
    ESP_ZB_ZCL_AI_TEMPERATURE_DISCHARGE_AIR,          /*!< Discharge air */
    ESP_ZB_ZCL_AI_TEMPERATURE_DISCHARGE,              /*!< Discharge */

    ESP_ZB_ZCL_AI_TEMPERATURE_EXHAUST_AIR_AFTER_HEAT_RECOVERY, /*!< Exhaust air afer heat recovery */
    ESP_ZB_ZCL_AI_TEMPERATURE_EXHAUST_AIR,                     /*!< Exhaust air */
    ESP_ZB_ZCL_AI_TEMPERATURE_GLYCOL,                          /*!< Glycol */
    ESP_ZB_ZCL_AI_TEMPERATURE_HEAT_RECOVERY_AIR,               /*!< Heat recovery air */
    ESP_ZB_ZCL_AI_TEMPERATURE_HOT_DECK,                        /*!< Hot deck */
    ESP_ZB_ZCL_AI_TEMPERATURE_HEAT_EXCHANGER_BYPASS,           /*!< Heat exchanger bypass */
    ESP_ZB_ZCL_AI_TEMPERATURE_HEAT_EXCHANGER_ENTERING,         /*!< Heat exchanger entering */
    ESP_ZB_ZCL_AI_TEMPERATURE_HEAT_EXCHANGER_LEAVING,          /*!< Heat eXchanger leaving */
    ESP_ZB_ZCL_AI_TEMPERATURE_MECHANICAL_ROOM,                 /*!< Mechanical room */
    ESP_ZB_ZCL_AI_TEMPERATURE_MIXED_AIR_0,                     /*!< Mixed air 0.  Mixed air is duplicated in spec */

    ESP_ZB_ZCL_AI_TEMPERATURE_MIXED_AIR_1,                  /*!< Mixed air 1 */
    ESP_ZB_ZCL_AI_TEMPERATURE_OUTDOOR_AIR_DEWPOINT,         /*!< Outdoor air dewpoint */
    ESP_ZB_ZCL_AI_TEMPERATURE_OUTDOOR_AIR,                  /*!< Outdoor air */
    ESP_ZB_ZCL_AI_TEMPERATURE_PREHEAT_AIR,                  /*!< Preheat air */
    ESP_ZB_ZCL_AI_TEMPERATURE_PREHEAT_ENTERING_WATER,       /*!< Preheat entering water */
    ESP_ZB_ZCL_AI_TEMPERATURE_PREHEAT_LEAVING_WATER,        /*!< Preheat leaving water */
    ESP_ZB_ZCL_AI_TEMPERATURE_PRIMARY_CHILLED_WATER_RETURN, /*!< Primary chilled water return */
    ESP_ZB_ZCL_AI_TEMPERATURE_PRIMARY_CHILLED_WATER_SUPPLY, /*!< Primary chilled water supply */
    ESP_ZB_ZCL_AI_TEMPERATURE_PRIMARY_HOT_WATER_RETURN,     /*!< Primary hot watter return */
    ESP_ZB_ZCL_AI_TEMPERATURE_PRIMARY_HOT_WATER_SUPPLY,     /*!< Primary hot watter supply */

    ESP_ZB_ZCL_AI_TEMPERATURE_REHEAT_COIL_DISCHARGE,          /*!< Reheat coil discharge */
    ESP_ZB_ZCL_AI_TEMPERATURE_REHEAT_ENTERING_WATER,          /*!< Reheat entering water */
    ESP_ZB_ZCL_AI_TEMPERATURE_REHEAT_LEAVING_WATER,           /*!< Reheat leaving water */
    ESP_ZB_ZCL_AI_TEMPERATURE_RETURN_AIR,                     /*!< Return air */
    ESP_ZB_ZCL_AI_TEMPERATURE_SECONDARY_CHILLED_WATER_RETURN, /*!< Secondary chilled water return */
    ESP_ZB_ZCL_AI_TEMPERATURE_SECONDARY_CHILLED_WATER_SUPPLY, /*!< Secondary chilled water supply */
    ESP_ZB_ZCL_AI_TEMPERATURE_SECONDARY_HW_RETURN,            /*!< Secondary hw return */
    ESP_ZB_ZCL_AI_TEMPERATURE_SECONDARY_HW_SUPPLY,            /*!< Secondary hw supply */
    ESP_ZB_ZCL_AI_TEMPERATURE_SIDELOOP_RESET,                 /*!< Sideloop reset */
    ESP_ZB_ZCL_AI_TEMPERATURE_SIDELOOP_SETPOINT,              /*!< Sideloop setpoint */

    ESP_ZB_ZCL_AI_TEMPERATURE_SIDELOOP,                    /*!< Sideloop */
    ESP_ZB_ZCL_AI_TEMPERATURE_SOURCE,                      /*!< Source */
    ESP_ZB_ZCL_AI_TEMPERATURE_SUPPLY_AIR,                  /*!< Supply air */
    ESP_ZB_ZCL_AI_TEMPERATURE_SUPPLY_LOW_LIMIT,            /*!< Supply low limit */
    ESP_ZB_ZCL_AI_TEMPERATURE_TOWER_BASIN,                 /*!< Tower basin */
    ESP_ZB_ZCL_AI_TEMPERATURE_TWO_PIPE_LEAVING_WATER,      /*!< Two pipe leaving water */
    ESP_ZB_ZCL_AI_TEMPERATURE_RESERVED,                    /*!< Reserved */
    ESP_ZB_ZCL_AI_TEMPERATURE_ZONE_DEWPOINT,               /*!< Zone dewpoint */
    ESP_ZB_ZCL_AI_TEMPERATURE_ZONE_SENSOR_SETPOINT,        /*!< Zone sensor setpoint */
    ESP_ZB_ZCL_AI_TEMPERATURE_ZONE_SENSOR_SETPOINT_OFFSET, /*!< Zone sensor setpoint offset */

    ESP_ZB_ZCL_AI_TEMPERATURE_ZONE, /*!< Zone */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_TEMPERATURE_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_TEMPERATURE, 0xffff), /*!< Other */
} esp_zb_zcl_ai_temperature_in_degrees_celsius_t;

/** @brief Values for 'Relative Humidity in %' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_HUMIDITY_DISCHARGE = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_HUMIDITY, 0x0000), /*!< Discharge */
    ESP_ZB_ZCL_AI_HUMIDITY_EXHAUST,                                                                                 /*!< Exhaust */
    ESP_ZB_ZCL_AI_HUMIDITY_HOT_DECK,                                                                                /*!< Hot deck */
    ESP_ZB_ZCL_AI_HUMIDITY_MIXED_AIR,                                                                               /*!< Mixed air */
    ESP_ZB_ZCL_AI_HUMIDITY_OUTDOOR_AIR,                                                                             /*!< Outdoor air */
    ESP_ZB_ZCL_AI_HUMIDITY_RETURN,                                                                                  /*!< Return */
    ESP_ZB_ZCL_AI_HUMIDITY_SIDELOOP,                                                                                /*!< Sideloop */
    ESP_ZB_ZCL_AI_HUMIDITY_SPACE,                                                                                   /*!< Space */
    ESP_ZB_ZCL_AI_HUMIDITY_ZONE,                                                                                    /*!< Zone */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_HUMIDITY_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_HUMIDITY, 0xffff), /*!< Other */
} esp_zb_zcl_ai_relative_humidity_in_percent_t;

/** @brief Values for 'Pressure in Pascal' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_PRESSURE_BOILER_PUMP_DIFFERENTIAL = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_PRESSURE, 0x0000), /*!< Boier pump differential */
    ESP_ZB_ZCL_AI_PRESSURE_BUILDING_STATIC,                                                                                        /*!< Building static */
    ESP_ZB_ZCL_AI_PRESSURE_COLD_DECK_DIFFERENTIAL_SENSOR,                                                                          /*!< Cold deck differential sensor */
    ESP_ZB_ZCL_AI_PRESSURE_CHILLED_WATER_BUILDING_DIFFERENTIAL,                                                                    /*!< Chilled water building differential */
    ESP_ZB_ZCL_AI_PRESSURE_COLD_DECK_DIFFERENTIAL,                                                                                 /*!< Cold deck differential */
    ESP_ZB_ZCL_AI_PRESSURE_COLD_DECK_STATIC,                                                                                       /*!< Cold deck static */
    ESP_ZB_ZCL_AI_PRESSURE_CONDENSER_WATER_PUMP_DIFFERENTIAL,                                                                      /*!< Condenser water pump differential */
    ESP_ZB_ZCL_AI_PRESSURE_DISCHARGE_DIFFERENTIAL,                                                                                 /*!< Discharge differential */
    ESP_ZB_ZCL_AI_PRESSURE_DISCHARGE_STATIC_1,                                                                                     /*!< Discharge static 1 */
    ESP_ZB_ZCL_AI_PRESSURE_DISCHARGE_STATIC_2,                                                                                     /*!< Discharge static 2 */

    ESP_ZB_ZCL_AI_PRESSURE_EXHAUST_AIR_DIFFERENTIAL,     /*!< Exhaust air differential */
    ESP_ZB_ZCL_AI_PRESSURE_EXHAUST_AIR_STATIC,           /*!< Exhaust air static */
    ESP_ZB_ZCL_AI_PRESSURE_EXHAUST_DIFFERENTIAL_0,       /*!< Exhaust differential 0. Note: Exhaust differential is duplicated in spec */
    ESP_ZB_ZCL_AI_PRESSURE_EXHAUST_DIFFERENTIAL_1,       /*!< Exhaust differential 1 */
    ESP_ZB_ZCL_AI_PRESSURE_HOT_DECK_DIFFERENTIAL_0,      /*!< Hot deck differential 0. Note: Hot Deck differential is duplicated in spec */
    ESP_ZB_ZCL_AI_PRESSURE_HOT_DECK_DIFFERENTIAL_1,      /*!< Hot deck differential 1 */
    ESP_ZB_ZCL_AI_PRESSURE_HOT_DECK_STATIC,              /*!< Hot deck static */
    ESP_ZB_ZCL_AI_PRESSURE_HOT_WATER_BLDG_DIFF,          /*!< Hot water bldg Diff */
    ESP_ZB_ZCL_AI_PRESSURE_HEAT_EXCHANGER_STEAM,         /*!< Heat exchanger steam */
    ESP_ZB_ZCL_AI_PRESSURE_MIN_OUTDOOR_AIR_DIFFERENTIAL, /*!< Min outdoor air differential */

    ESP_ZB_ZCL_AI_PRESSURE_OUTDOOR_AIR_DIFFERENTIAL,                  /*!< Outdoor air differential */
    ESP_ZB_ZCL_AI_PRESSURE_PRIMARY_CHILLED_WATER_PUMP_DIFFERENTIAL,   /*!< Primary chilled water pump differential */
    ESP_ZB_ZCL_AI_PRESSURE_PRIMATY_HOT_WATER_PUMP_DIFFERENTIAL,       /*!< Primary hot water pump differential */
    ESP_ZB_ZCL_AI_PRESSURE_RELIEF_DIFFERENTIAL,                       /*!< Relief differential */
    ESP_ZB_ZCL_AI_PRESSURE_RETURN_AIR_STATIC,                         /*!< Return air Static */
    ESP_ZB_ZCL_AI_PRESSURE_RETURN_DIFFERENTIAL,                       /*!< Return differential */
    ESP_ZB_ZCL_AI_PRESSURE_SECONDARY_CHILLED_WATER_PUMP_DIFFERENTIAL, /*!< Secondary chilled water pump differential */
    ESP_ZB_ZCL_AI_PRESSURE_SECONDARY_HOT_WATER_PUMP_DIFFERENTIAL,     /*!< Secondary hot water pump differential */
    ESP_ZB_ZCL_AI_PRESSURE_SIDELOOP,                                  /*!< Sideloop */
    ESP_ZB_ZCL_AI_PRESSURE_STEAM,                                     /*!< Steam */

    ESP_ZB_ZCL_AI_PRESSURE_SUPPLY_DIFFERENTIAL_SENSOR, /*!< Supply differential sensor */
    /* 0x0200 to 0xffffe are Vendor defined */
    ESP_ZB_ZCL_AI_PRESSURE_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_PRESSURE, 0xffff), /*!< Other */
} esp_zb_zcl_ai_pressure_in_pascal_t;

/** @brief Values for 'Flow in Liters per Second' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_FLOW_CHILLED_WATER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_FLOW, 0x0000), /*!< Chilled water */
    ESP_ZB_ZCL_AI_FLOW_CHILLER_CHILLED_WATER,                                                                   /*!< Chiller chilled water */
    ESP_ZB_ZCL_AI_FLOW_CHILLER_CONDENSER_WATER,                                                                 /*!< Chiller condenser water */
    ESP_ZB_ZCL_AI_FLOW_COLD_DECK,                                                                               /*!< Cold deck */
    ESP_ZB_ZCL_AI_FLOW_DECOUPLE_LOOP,                                                                           /*!< Decouple loop */
    ESP_ZB_ZCL_AI_FLOW_DISCHARGE,                                                                               /*!< Discharge */
    ESP_ZB_ZCL_AI_FLOW_EXHAUST_FAN,                                                                             /*!< Exhaust fan */
    ESP_ZB_ZCL_AI_FLOW_EXHAUST,                                                                                 /*!< Exhaust */
    ESP_ZB_ZCL_AI_FLOW_FAN,                                                                                     /*!< Fan */
    ESP_ZB_ZCL_AI_FLOW_HOT_DECK,                                                                                /*!< Flow hot deck */

    ESP_ZB_ZCL_AI_FLOW_HOT_WATER,                    /*!< Hot water */
    ESP_ZB_ZCL_AI_FLOW_MIN_OUTDOOR_AIR_FAN,          /*!< Min outdoor air fan */
    ESP_ZB_ZCL_AI_FLOW_MIN_OUTDOOR_AIR,              /*!< Min outdoor Air */
    ESP_ZB_ZCL_AI_FLOW_OUTDOOR_AIR,                  /*!< Outdoor air */
    ESP_ZB_ZCL_AI_FLOW_PRIMARY_CHILLED_WATER,        /*!< Primary chilled water */
    ESP_ZB_ZCL_AI_FLOW_RELIEF_FAN,                   /*!< Relief fan */
    ESP_ZB_ZCL_AI_FLOW_RELIEF,                       /*!< Relief */
    ESP_ZB_ZCL_AI_FLOW_RETURN_FAN,                   /*!< Return fan */
    ESP_ZB_ZCL_AI_FLOW_RETURN,                       /*!< Return */
    ESP_ZB_ZCL_AI_FLOW_SECONDARY_CHILLED_WATER_FLOW, /*!< Secondary chilled water flow */

    ESP_ZB_ZCL_AI_FLOW_SUPPLY_FAN, /*!< Supply fan */
    ESP_ZB_ZCL_AI_FLOW_TOWER_FAN,  /*!< Tower fan */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_FLOW_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_FLOW, 0xffff), /*!< Other */
} esp_zb_zcl_ai_flow_in_liters_per_second_t;

/** @brief Values for 'Percentage %' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_PERCENTAGE_CHILLER_FULL_LOAD_AMPERAGE = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_PERCENTAGE, 0x0000), /*!< Chiller full load amperage */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_PERCENTAGE_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_PERCENTAGE, 0xffff), /*!< Other */
} esp_zb_zcl_ai_percentage_t;

/** @brief Values for 'Parts per Million PPM' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_PPM_RETURN_CARBON_DIOXIDE = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_PPM, 0x0000), /*!< Return carbon dioxide */
    ESP_ZB_ZCL_AI_PPM_ZONE_CARBON_DIOXIDE,                                                                            /*!< Zone carbon dioxide */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_PPM_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_PPM, 0xffff), /*!< Other */
} esp_zb_zcl_ai_ppm_t;

/** @brief Values for 'Rotational Speed in RPM' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_RPM_EXHAUS_FAN_REMOTE = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_RPM, 0x0000), /*!< Exhaust fan remote */
    ESP_ZB_ZCL_AI_RPM_HEAT_RECOVERY_WHEEL_REMOTE,                                                                 /*!< Heat recovery wheel remote */
    ESP_ZB_ZCL_AI_RPM_MIN_OUTDOOR_AIR_FAN_REMOTE,                                                                 /*!< Min outdoor air fan remote */
    ESP_ZB_ZCL_AI_RPM_RELIEF_FAN_REMOTE,                                                                          /*!< Relief fan remote */
    ESP_ZB_ZCL_AI_RPM_RETURN_FAN_REMOTE,                                                                          /*!< Return fan remote */
    ESP_ZB_ZCL_AI_RPM_SUPPLY_FAN_REMOTE,                                                                          /*!< Supply fan remote */
    ESP_ZB_ZCL_AI_RPM_VARIABLE_SPEED_DRIVE_MOTOR,                                                                 /*!< Variable speed drive motor */
    ESP_ZB_ZCL_AI_RPM_VARIABLE_SPEED_DRIVE_SETPOINT,                                                              /*!< Variable speed drive setpoint */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_RPM_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_RPM, 0xffff), /*!< Other */
} esp_zb_zcl_ai_rpm_t;

/** @brief Values for 'Current in Amps' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_CURRENT_CHILLER_AMPS = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_CURRENT_IN_AMPS, 0x0000), /*!< Chiller amps */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_CURRENT_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_CURRENT_IN_AMPS, 0xffff), /*!< Other */
} esp_zb_zcl_ai_current_t;

/** @brief Values for 'Frequency in Hz' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_FREQUENCY_VARIABLE_SPEED_DRIVE_OUTPUT = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_FREQUENCY, 0x0000), /*!< Variable speed drive output */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_FREQUENCY_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_FREQUENCY, 0xffff), /*!< Other */
} esp_zb_zcl_ai_frequency_t;

/** @brief Values for 'Power in Watts' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_POWER_IN_WATTS_CONSUMPTION = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_POWER_IN_WATTS, 0x0000), /*!< Consumption */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_POWER_IN_WATTS_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_POWER_IN_WATTS, 0xffff), /*!< Other */
} esp_zb_zcl_ai_power_in_watts_t;

/** @brief Values for 'Power in kW' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_POWER_IN_KILOWATTS_ABSOLUTE = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_POWER_IN_KILOWATTS, 0x0000), /*!< Absolute */
    ESP_ZB_ZCL_AI_POWER_IN_KILOWATTS_CONSUMPTION,                                                                                      /*!< Consumption */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_POWER_IN_KILOWATTS_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_POWER_IN_KILOWATTS, 0xffff), /*!< Other */
} esp_zb_zcl_ai_power_in_kilowatts_t;

/** @brief Values for 'Energy in kWH' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_ENERGY_KWH_VARIABLE_SPEED_DRIVE = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_ENERGY, 0x0000), /*!< Variable speed drive */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_ENERGY_KWH_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_ENERGY, 0xffff), /*!< Other */
} esp_zb_zcl_ai_energy_kwh_t;

/** @brief Values for 'Count - Unitless' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_COUNT_UNITLESS_COUNT = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_COUNT_UNITLESS, 0x0000), /*!< Count */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_COUNT_UNITLESS_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_COUNT_UNITLESS, 0xffff), /*!< Other */
} esp_zb_zcl_ai_count_unitless_t;

/** @brief Values for 'Enthalpy in KJoules per Kg' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_ENTHALPY_OUTDOOR_AIR = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_ENTHALPY, 0x0000), /*!< Out door air */
    ESP_ZB_ZCL_AI_ENTHALPY_RETURN_AIR,                                                                                /*!< Return air */
    ESP_ZB_ZCL_AI_ENTHALPY_SPACE,                                                                                     /*!< Space */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_ENTHALPY_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_ENTHALPY, 0xffff), /*!< Other */
} esp_zb_zcl_ai_enthaply_t;

/** @brief Values for 'Time in Seconds' type of Analog Input cluster */
typedef enum {
    ESP_ZB_ZCL_AI_TIME_RELATIVE = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_TIME, 0x0000), /*!< Relative */
    /* 0x0200 to 0xfffe are Vendor defined */
    ESP_ZB_ZCL_AI_TIME_OTHER = ESP_ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ESP_ZB_ZCL_AI_APP_TYPE_TIME, 0xffff), /*!< Other */
} esp_zb_zcl_ai_time_t;

/** @brief Default value for Description attribute */
#define ESP_ZB_ZCL_ANALOG_INPUT_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for OutOfService attribute */
#define ESP_ZB_ZCL_ANALOG_INPUT_OUT_OF_SERVICE_DEFAULT_VALUE false

/** @brief Default value for Reliability attribute */
#define ESP_ZB_ZCL_ANALOG_INPUT_RELIABILITY_DEFAULT_VALUE 0x00

/** @brief Default value for StatusFlags attribute */
#define ESP_ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_DEFAULT_VALUE 0

/** @brief StatusFlags attribute minimum value */
#define ESP_ZB_ZCL_ANALOG_INPUT_STATUS_FLAGS_MIN_VALUE 0

/** @brief StatusFlags attribute maximum value */
#define ESP_ZB_ZCL_ANALOG_INPUT_STATUS_FLAGS_MAX_VALUE 0x0f

/** Number of attributes mandatory for reporting in Analog Input cluster */
#define ESP_ZB_ZCL_ANALOG_INPUT_REPORT_ATTR_COUNT 2

#ifdef __cplusplus
}
#endif
