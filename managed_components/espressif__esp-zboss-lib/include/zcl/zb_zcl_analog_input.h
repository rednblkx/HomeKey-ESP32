/*
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA
 * Copyright (c) 2021-2022 Espressif Systems (Shanghai) PTE LTD
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Espressif Systems
 *    integrated circuit in a product or a software update for such product,
 *    must reproduce the above copyright notice, this list of conditions and
 *    the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/* PURPOSE: Analog Input cluster definitions
*/

#ifndef ZB_ZCL_ANALOG_INPUT_H
#define ZB_ZCL_ANALOG_INPUT_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_ANALOG_INPUT
 *  @{
 *    @details
 *    The Analog Input cluster provides an interface for reading the value of an
 *    analog measurement and accessing various characteristics of that measurement.
 *    No cluster specific commands are received or generated.
 */


/* Cluster ZB_ZCL_CLUSTER_ID_ANALOG_INPUT */

/** @defgroup ZB_ZCL_ANALOG_INPUT_ATTRS_GROUP Analog Input cluster attributes
 *  @{
 */

/** @brief Analog Input cluster attribute identifiers
 *  @see ZCL spec, Analog Input (Basic) Cluster 3.14.2.4.1
 */
enum zb_zcl_analog_input_attr_e
{
  /** @brief Description attribute, ZCL spec 3.14.11.4 */
  ZB_ZCL_ATTR_ANALOG_INPUT_DESCRIPTION_ID       = 0x001c,
  /** @brief MaxPresentValue attribute, ZCL spec 3.14.11.5 */
  ZB_ZCL_ATTR_ANALOG_INPUT_MAX_PRESENT_VALUE_ID = 0x0041,
  /** @brief MinPresentValue attribute, ZCL spec 3.14.11.8 */
  ZB_ZCL_ATTR_ANALOG_INPUT_MIN_PRESENT_VALUE_ID = 0x0045,
  /** @brief OutOfService attribute, ZCL spec 3.14.11.1 */
  ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID    = 0x0051,
  /** @brief PresentValue attribute, ZCL spec 3.14.11.2 */
  ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID     = 0x0055,
  /** @brief Reliability attribute, ZCL spec 3.14.11.9 */
  ZB_ZCL_ATTR_ANALOG_INPUT_RELIABILITY_ID       = 0x0067,
  /** @brief Resolution attribute, ZCL spec 3.14.11.11 */
  ZB_ZCL_ATTR_ANALOG_INPUT_RESOLUTION_ID        = 0x006a,
  /** @brief StatusFlags attribute, ZCL spec 3.14.11.3 */
  ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID      = 0x006f,
  /** @brief EngineeringUnits attribute, ZCL spec 3.14.11.10 */
  ZB_ZCL_ATTR_ANALOG_INPUT_ENGINEERING_UNITS_ID = 0x0075,
  /** @brief ApplicationType attribute, ZCL spec 3.14.11.19 */
  ZB_ZCL_ATTR_ANALOG_INPUT_APPLICATION_TYPE_ID  = 0x0100,
};

/**
 *  @brief StatusFlags attribute values.
 *  @see ZCL spec 3.14.11.3 for details.
 */
enum zb_zcl_analog_input_status_flags_value_e
{
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_NORMAL         = 0x00,  /**< Normal (default) state. */
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_IN_ALARM       = 0x01,  /**< In alarm bit. */
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_FAULT          = 0x02,  /**< Fault bit. */
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OVERRIDDEN     = 0x04,  /**< Overridden bit. */
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OUT_OF_SERVICE = 0x08,  /**< Out of service bit. */
};

/** @brief Reliability attribute values.
 *  @see ZCL spec 3.14.11.9 for details.
 */
enum zb_zcl_analog_input_reliability_value_e
{
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_NO_FAULT_DETECTED   = 0x00,
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_NO_SENSOR           = 0x01,
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_OVER_RANGE          = 0x02,
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_UNDER_RANGE         = 0x03,
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_OPEN_LOOP           = 0x04,
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_SHORTED_LOOP        = 0x05,
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_NO_OUTPUT           = 0x06,
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_UNRELIABLE_OTHER    = 0x07,
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_PROCESS_ERROR       = 0x08,
  /* 0x09 is for multistate clusters only */
  ZB_ZCL_ANALOG_INPUT_RELIABILITY_CONFIGURATION_ERROR = 0x0a,
};

/** @defgroup ZB_ZCL_ANALOG_INPUT_APPLICATION_TYPES Analog Input cluster application types
 *  @{
 *    @details
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

#define ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(_type, _id) (((_type & 0xff) << 16) | (_id & 0xffff))

/** @brief Type values for Analog Input cluster applications
 *  @see ZCL spec 3.14.11.19.1
 */
enum zb_zcl_ai_application_types_e
{
  ZB_ZCL_AI_APP_TYPE_TEMPERATURE,
  ZB_ZCL_AI_APP_TYPE_HUMIDITY,
  ZB_ZCL_AI_APP_TYPE_PRESSURE,
  ZB_ZCL_AI_APP_TYPE_FLOW,
  ZB_ZCL_AI_APP_TYPE_PERCENTAGE,
  ZB_ZCL_AI_APP_TYPE_PPM,
  ZB_ZCL_AI_APP_TYPE_RPM,
  ZB_ZCL_AI_APP_TYPE_CURRENT_IN_AMPS,
  ZB_ZCL_AI_APP_TYPE_FREQUENCY,
  ZB_ZCL_AI_APP_TYPE_POWER_IN_WATTS,
  ZB_ZCL_AI_APP_TYPE_POWER_IN_KILOWATTS,
  ZB_ZCL_AI_APP_TYPE_ENERGY,
  ZB_ZCL_AI_APP_TYPE_COUNT_UNITLESS,
  ZB_ZCL_AI_APP_TYPE_ENTHALPY,
  ZB_ZCL_AI_APP_TYPE_TIME,
  /* Types 0x0f to 0xfe are reserved */
  ZB_ZCL_AI_APP_TYPE_OTHER = 0xff
};

/** @defgroup ZB_ZCL_AI_TEMPERATURE_APP_TYPE Analog Input cluster representing temperature
 *  @{
 */

/** @brief Values for 'Temperature in degrees Celsius' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.1
 */
enum zb_zcl_ai_temperature_in_degrees_celsius_e
{
  ZB_ZCL_AI_TEMPERATURE_2_PIPE_ENTERING = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_TEMPERATURE, 0x0000),
  ZB_ZCL_AI_TEMPERATURE_2_PIPE_LEAVING,
  ZB_ZCL_AI_TEMPERATURE_BOILER_ENTERING,
  ZB_ZCL_AI_TEMPERATURE_BOILER_LEAVING,
  ZB_ZCL_AI_TEMPERATURE_CHILLER_CHILLED_WATER_ENTERING,
  ZB_ZCL_AI_TEMPERATURE_CHILLER_CHILLED_WATER_LEAVING,
  ZB_ZCL_AI_TEMPERATURE_CHILLER_CONDENSER_WATER_ENTERING,
  ZB_ZCL_AI_TEMPERATURE_CHILLER_CONDENSER_WATER_LEAVING,
  ZB_ZCL_AI_TEMPERATURE_COLD_DECK,
  ZB_ZCL_AI_TEMPERATURE_COOLING_COIL_DISCHARGE,

  ZB_ZCL_AI_TEMPERATURE_COOLING_ENTERING_WATER,
  ZB_ZCL_AI_TEMPERATURE_COOLING_LEAVING_WATER,
  ZB_ZCL_AI_TEMPERATURE_CONDENSER_WATER_RETURN,
  ZB_ZCL_AI_TEMPERATURE_CONDENSER_WATER_SUPPLY,
  ZB_ZCL_AI_TEMPERATURE_DECOUPLE_LOOP_0, /* Note: Decouple Loop is duplicated in spec */
  ZB_ZCL_AI_TEMPERATURE_BUILDING_LOAD,
  ZB_ZCL_AI_TEMPERATURE_DECOUPLE_LOOP_1,
  ZB_ZCL_AI_TEMPERATURE_DEW_POINT,
  ZB_ZCL_AI_TEMPERATURE_DISCHARGE_AIR,
  ZB_ZCL_AI_TEMPERATURE_DISCHARGE,

  ZB_ZCL_AI_TEMPERATURE_EXHAUST_AIR_AFTER_HEAT_RECOVERY,
  ZB_ZCL_AI_TEMPERATURE_EXHAUST_AIR,
  ZB_ZCL_AI_TEMPERATURE_GLYCOL,
  ZB_ZCL_AI_TEMPERATURE_HEAT_RECOVERY_AIR,
  ZB_ZCL_AI_TEMPERATURE_HOT_DECK,
  ZB_ZCL_AI_TEMPERATURE_HEAT_EXCHANGER_BYPASS,
  ZB_ZCL_AI_TEMPERATURE_HEAT_EXCHANGER_ENTERING,
  ZB_ZCL_AI_TEMPERATURE_HEAT_EXCHANGER_LEAVING,
  ZB_ZCL_AI_TEMPERATURE_MECHANICAL_ROOM,
  ZB_ZCL_AI_TEMPERATURE_MIXED_AIR_0, /* Note: Mixed Air is duplicated in spec */

  ZB_ZCL_AI_TEMPERATURE_MIXED_AIR_1,
  ZB_ZCL_AI_TEMPERATURE_OUTDOOR_AIR_DEWPOINT,
  ZB_ZCL_AI_TEMPERATURE_OUTDOOR_AIR,
  ZB_ZCL_AI_TEMPERATURE_PREHEAT_AIR,
  ZB_ZCL_AI_TEMPERATURE_PREHEAT_ENTERING_WATER,
  ZB_ZCL_AI_TEMPERATURE_PREHEAT_LEAVING_WATER,
  ZB_ZCL_AI_TEMPERATURE_PRIMARY_CHILLED_WATER_RETURN,
  ZB_ZCL_AI_TEMPERATURE_PRIMARY_CHILLED_WATER_SUPPLY,
  ZB_ZCL_AI_TEMPERATURE_PRIMARY_HOT_WATER_RETURN,
  ZB_ZCL_AI_TEMPERATURE_PRIMARY_HOT_WATER_SUPPLY,

  ZB_ZCL_AI_TEMPERATURE_REHEAT_COIL_DISCHARGE,
  ZB_ZCL_AI_TEMPERATURE_REHEAT_ENTERING_WATER,
  ZB_ZCL_AI_TEMPERATURE_REHEAT_LEAVING_WATER,
  ZB_ZCL_AI_TEMPERATURE_RETURN_AIR,
  ZB_ZCL_AI_TEMPERATURE_SECONDARY_CHILLED_WATER_RETURN,
  ZB_ZCL_AI_TEMPERATURE_SECONDARY_CHILLED_WATER_SUPPLY,
  ZB_ZCL_AI_TEMPERATURE_SECONDARY_HW_RETURN,
  ZB_ZCL_AI_TEMPERATURE_SECONDARY_HW_SUPPLY,
  ZB_ZCL_AI_TEMPERATURE_SIDELOOP_RESET,
  ZB_ZCL_AI_TEMPERATURE_SIDELOOP_SETPOINT,

  ZB_ZCL_AI_TEMPERATURE_SIDELOOP,
  ZB_ZCL_AI_TEMPERATURE_SOURCE,
  ZB_ZCL_AI_TEMPERATURE_SUPPLY_AIR,
  ZB_ZCL_AI_TEMPERATURE_SUPPLY_LOW_LIMIT,
  ZB_ZCL_AI_TEMPERATURE_TOWER_BASIN,
  ZB_ZCL_AI_TEMPERATURE_TWO_PIPE_LEAVING_WATER,
  ZB_ZCL_AI_TEMPERATURE_RESERVED,
  ZB_ZCL_AI_TEMPERATURE_ZONE_DEWPOINT,
  ZB_ZCL_AI_TEMPERATURE_ZONE_SENSOR_SETPOINT,
  ZB_ZCL_AI_TEMPERATURE_ZONE_SENSOR_SETPOINT_OFFSET,

  ZB_ZCL_AI_TEMPERATURE_ZONE,
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_TEMPERATURE_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_TEMPERATURE, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_TEMPERATURE_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_HUMIDITY_APP_TYPE Analog Input cluster representing humidity
 *  @{
 */

/** @brief Values for 'Relative Humidity in %' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.2
 */
enum zb_zcl_ai_relative_humidity_in_percent_e
{
  ZB_ZCL_AI_HUMIDITY_DISCHARGE = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_HUMIDITY, 0x0000),
  ZB_ZCL_AI_HUMIDITY_EXHAUST,
  ZB_ZCL_AI_HUMIDITY_HOT_DECK,
  ZB_ZCL_AI_HUMIDITY_MIXED_AIR,
  ZB_ZCL_AI_HUMIDITY_OUTDOOR_AIR,
  ZB_ZCL_AI_HUMIDITY_RETURN,
  ZB_ZCL_AI_HUMIDITY_SIDELOOP,
  ZB_ZCL_AI_HUMIDITY_SPACE,
  ZB_ZCL_AI_HUMIDITY_ZONE,
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_HUMIDITY_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_HUMIDITY, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_HUMIDITY_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_PRESSURE_APP_TYPE Analog Input cluster representing pressure
 *  @{
 */

/** @brief Values for 'Pressure in Pascal' type of Analog Input cluster
 *  @see 3.14.11.19.1.3
 */
enum zb_zcl_ai_pressure_in_pascal_e
{
  ZB_ZCL_AI_PRESSURE_BOILER_PUMP_DIFFERENTIAL = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_PRESSURE, 0x0000),
  ZB_ZCL_AI_PRESSURE_BUILDING_STATIC,
  ZB_ZCL_AI_PRESSURE_COLD_DECK_DIFFERENTIAL_SENSOR,
  ZB_ZCL_AI_PRESSURE_CHILLED_WATER_BUILDING_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_COLD_DECK_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_COLD_DECK_STATIC,
  ZB_ZCL_AI_PRESSURE_CONDENSER_WATER_PUMP_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_DISCHARGE_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_DISCHARGE_STATIC_1,
  ZB_ZCL_AI_PRESSURE_DISCHARGE_STATIC_2,

  ZB_ZCL_AI_PRESSURE_EXHAUST_AIR_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_EXHAUST_AIR_STATIC,
  ZB_ZCL_AI_PRESSURE_EXHAUST_DIFFERENTIAL_0, /* Note: Exhaust Differential is duplicated in spec */
  ZB_ZCL_AI_PRESSURE_EXHAUST_DIFFERENTIAL_1,
  ZB_ZCL_AI_PRESSURE_HOT_DECK_DIFFERENTIAL_0, /* Note: Hot Deck Differential is duplicated in spec */
  ZB_ZCL_AI_PRESSURE_HOT_DECK_DIFFERENTIAL_1,
  ZB_ZCL_AI_PRESSURE_HOT_DECK_STATIC,
  ZB_ZCL_AI_PRESSURE_HOT_WATER_BLDG_DIFF,
  ZB_ZCL_AI_PRESSURE_HEAT_EXCHANGER_STEAM,
  ZB_ZCL_AI_PRESSURE_MIN_OUTDOOR_AIR_DIFFERENTIAL,

  ZB_ZCL_AI_PRESSURE_OUTDOOR_AIR_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_PRIMARY_CHILLED_WATER_PUMP_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_PRIMATY_HOT_WATER_PUMP_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_RELIEF_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_RETURN_AIR_STATIC,
  ZB_ZCL_AI_PRESSURE_RETURN_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_SECONDARY_CHILLED_WATER_PUMP_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_SECONDARY_HOT_WATER_PUMP_DIFFERENTIAL,
  ZB_ZCL_AI_PRESSURE_SIDELOOP,
  ZB_ZCL_AI_PRESSURE_STEAM,

  ZB_ZCL_AI_PRESSURE_SUPPLY_DIFFERENTIAL_SENSOR,
  /* 0x0200 to 0xffffe are Vendor defined */
  ZB_ZCL_AI_PRESSURE_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_PRESSURE, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_PRESSURE_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_FLOW_APP_TYPE Analog Input cluster representing flow
 *  @{
 */

/** @brief Values for 'Flow in Liters per Second' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.4
 */
enum zb_zcl_ai_flow_in_liters_per_second_e
{
  ZB_ZCL_AI_FLOW_CHILLED_WATER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_FLOW, 0x0000),
  ZB_ZCL_AI_FLOW_CHILLER_CHILLED_WATER,
  ZB_ZCL_AI_FLOW_CHILLER_CONDENSER_WATER,
  ZB_ZCL_AI_FLOW_COLD_DECK,
  ZB_ZCL_AI_FLOW_DECOUPLE_LOOP,
  ZB_ZCL_AI_FLOW_DISCHARGE,
  ZB_ZCL_AI_FLOW_EXHAUST_FAN,
  ZB_ZCL_AI_FLOW_EXHAUST,
  ZB_ZCL_AI_FLOW_FAN,
  ZB_ZCL_AI_FLOW_HOT_DECK,

  ZB_ZCL_AI_FLOW_HOT_WATER,
  ZB_ZCL_AI_FLOW_MIN_OUTDOOR_AIR_FAN,
  ZB_ZCL_AI_FLOW_MIN_OUTDOOR_AIR,
  ZB_ZCL_AI_FLOW_OUTDOOR_AIR,
  ZB_ZCL_AI_FLOW_PRIMARY_CHILLED_WATER,
  ZB_ZCL_AI_FLOW_RELIEF_FAN,
  ZB_ZCL_AI_FLOW_RELIEF,
  ZB_ZCL_AI_FLOW_RETURN_FAN,
  ZB_ZCL_AI_FLOW_RETURN,
  ZB_ZCL_AI_FLOW_SECONDARY_CHILLED_WATER_FLOW,

  ZB_ZCL_AI_FLOW_SUPPLY_FAN,
  ZB_ZCL_AI_FLOW_TOWER_FAN,
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_FLOW_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_FLOW, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_FLOW_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_PERCENTAGE_APP_TYPE Analog Input cluster representing percentage
 *  @{
 */

/** @brief Values for 'Percentage %' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.5
 */
enum zb_zcl_ai_percentage_e
{
  ZB_ZCL_AI_PERCENTAGE_CHILLER_FULL_LOAD_AMPERAGE = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_PERCENTAGE, 0x0000),
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_PERCENTAGE_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_PERCENTAGE, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_PERCENTAGE_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_PPM_APP_TYPE Analog Input cluster representing parts per million
 *  @{
 */

/** @brief Values for 'Parts per Million PPM' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.6
 */
enum zb_zcl_ai_ppm_e
{
  ZB_ZCL_AI_PPM_RETURN_CARBON_DIOXIDE = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_PPM, 0x0000),
  ZB_ZCL_AI_PPM_ZONE_CARBON_DIOXIDE,
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_PPM_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_PPM, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_PPM_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_RPM_APP_TYPE Analog Input cluster representing rotational speed
 *  @{
 */

/** @brief Values for 'Rotational Speed in RPM' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.7
 */
enum zb_zcl_ai_rpm_e
{
  ZB_ZCL_AI_RPM_EXHAUS_FAN_REMOTE = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_RPM, 0x0000),
  ZB_ZCL_AI_RPM_HEAT_RECOVERY_WHEEL_REMOTE,
  ZB_ZCL_AI_RPM_MIN_OUTDOOR_AIR_FAN_REMOTE,
  ZB_ZCL_AI_RPM_RELIEF_FAN_REMOTE,
  ZB_ZCL_AI_RPM_RETURN_FAN_REMOTE,
  ZB_ZCL_AI_RPM_SUPPLY_FAN_REMOTE,
  ZB_ZCL_AI_RPM_VARIABLE_SPEED_DRIVE_MOTOR,
  ZB_ZCL_AI_RPM_VARIABLE_SPEED_DRIVE_SETPOINT,
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_RPM_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_RPM, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_RPM_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_CURRENT_APP_TYPE Analog Input cluster representing current
 *  @{
 */

/** @brief Values for 'Current in Amps' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.8
 */
enum zb_zcl_ai_current_e
{
  ZB_ZCL_AI_CURRENT_CHILLER_AMPS = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_CURRENT_IN_AMPS, 0x0000),
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_CURRENT_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_CURRENT_IN_AMPS, 0xffff),
};

/** @} */ /* end of ZB_ZCL_AI_CURRENT_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_FREQUENCY_APP_TYPE Analog Input cluster representing frequency
 *  @{
 */

/** @brief Values for 'Frequency in Hz' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.9
 */
enum zb_zcl_ai_frequency_e
{
  ZB_ZCL_AI_FREQUENCY_VARIABLE_SPEED_DRIVE_OUTPUT = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_FREQUENCY, 0x0000),
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_FREQUENCY_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_FREQUENCY, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_FREQUENCY_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_POWER_IN_WATTS_APP_TYPE Analog Input cluster representing power in Watts
 *  @{
 */

/** @brief Values for 'Power in Watts' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.10
 */
enum zb_zcl_ai_power_in_watts_e
{
  ZB_ZCL_AI_POWER_IN_WATTS_CONSUMPTION = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_POWER_IN_WATTS, 0x0000),
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_POWER_IN_WATTS_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_POWER_IN_WATTS, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_POWER_IN_WATTS_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_POWER_IN_KILOWATTS_APP_TYPE Analog Input cluster representing power in kiloWatts
 *  @{
 */

/** @brief Values for 'Power in kW' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.11
 */
enum zb_zcl_ai_power_in_kilowatts_e
{
  ZB_ZCL_AI_POWER_IN_KILOWATTS_ABSOLUTE = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_POWER_IN_KILOWATTS, 0x0000),
  ZB_ZCL_AI_POWER_IN_KILOWATTS_CONSUMPTION,
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_POWER_IN_KILOWATTS_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_POWER_IN_KILOWATTS, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_POWER_IN_KILOWATTS_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_ENERGY_APP_TYPE Analog Input cluster representing energy
 *  @{
 */

/** @brief Values for 'Energy in kWH' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.12
 */
enum zb_zcl_ai_energy_kwh_e
{
  ZB_ZCL_AI_ENERGY_KWH_VARIABLE_SPEED_DRIVE = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_ENERGY, 0x0000),
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_ENERGY_KWH_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_ENERGY, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_ENERGY_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_COUNT_UNITLESS_APP_TYPE Analog Input cluster representing unitless count
 *  @{
 */

/** @brief Values for 'Count - Unitless' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.13
 */
enum zb_zcl_ai_count_unitless_e
{
  ZB_ZCL_AI_COUNT_UNITLESS_COUNT = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_COUNT_UNITLESS, 0x0000),
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_COUNT_UNITLESS_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_COUNT_UNITLESS, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_COUNT_UNITLESS_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_ENTHALPY_APP_TYPE Analog Input cluster representing enthalpy
 *  @{
 */

/** @brief Values for 'Enthalpy in KJoules per Kg' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.14
 */
enum zb_zcl_ai_enthaply_e
{
  ZB_ZCL_AI_ENTHALPY_OUTDOOR_AIR = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_ENTHALPY, 0x0000),
  ZB_ZCL_AI_ENTHALPY_RETURN_AIR,
  ZB_ZCL_AI_ENTHALPY_SPACE,
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_ENTHALPY_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_ENTHALPY, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_ENTHALPY_APP_TYPE group */

/** @defgroup ZB_ZCL_AI_TIME_APP_TYPE Analog Input cluster representing time
 *  @{
 */

/** @brief Values for 'Time in Seconds' type of Analog Input cluster
 *  @see ZCL spec 3.14.11.19.1.15
 */
enum zb_zcl_ai_time_e
{
  ZB_ZCL_AI_TIME_RELATIVE = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_TIME, 0x0000),
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_AI_TIME_OTHER = ZB_ZCL_AI_SET_APP_TYPE_WITH_ID(ZB_ZCL_AI_APP_TYPE_TIME, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AI_TIME_APP_TYPE group */

/** @} */ /* end of ZB_ZCL_ANALOG_INPUT_APPLICATION_TYPES */

/** @brief Default value for Analog Input cluster revision global attribute */
#define ZB_ZCL_ANALOG_INPUT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Default value for Description attribute */
#define ZB_ZCL_ANALOG_INPUT_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for OutOfService attribute */
#define ZB_ZCL_ANALOG_INPUT_OUT_OF_SERVICE_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for Reliability attribute */
#define ZB_ZCL_ANALOG_INPUT_RELIABILITY_DEFAULT_VALUE ZB_ZCL_ANALOG_INPUT_RELIABILITY_NO_FAULT_DETECTED

/** @brief Default value for StatusFlags attribute */
#define ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_DEFAULT_VALUE ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_NORMAL

/** @brief StatusFlags attribute minimum value */
#define ZB_ZCL_ANALOG_INPUT_STATUS_FLAGS_MIN_VALUE 0

/** @brief StatusFlags attribute maximum value */
#define ZB_ZCL_ANALOG_INPUT_STATUS_FLAGS_MAX_VALUE 0x0f

/** @cond internals_doc */
/** @name Analog Input cluster internals
 *  Internal structures for Analog Input cluster
 *  @{
 */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_DESCRIPTION_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_DESCRIPTION_ID,                          \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_MAX_PRESENT_VALUE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_MAX_PRESENT_VALUE_ID,                    \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_MIN_PRESENT_VALUE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_MIN_PRESENT_VALUE_ID,                    \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID,                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID,                        \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING,     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_RELIABILITY_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_RELIABILITY_ID,                          \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_RESOLUTION_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_RESOLUTION_ID,                           \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,                         \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_ENGINEERING_UNITS_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_ENGINEERING_UNITS_ID,                    \
  ZB_ZCL_ATTR_TYPE_16BIT_ENUM,                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_APPLICATION_TYPE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_APPLICATION_TYPE_ID,                     \
  ZB_ZCL_ATTR_TYPE_U32,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

/** Number of attributes mandatory for reporting in Analog Input cluster */
#define ZB_ZCL_ANALOG_INPUT_REPORT_ATTR_COUNT 2

/** @} */
/** @endcond */ /* Analog Input cluster internals */

/** @brief Declare attribute list for Analog Input cluster
 *  @param attr_list - attribute list name
 *  @param description - pointer to variable storing Description attribute value
 *  @param max_present_value - pointer to variable storing MaxPresentValue attribute value
 *  @param min_present_value - pointer to variable storing MinPresentValue attribute value
 *  @param out_of_service - pointer to variable storing OutOfService attribute value
 *  @param present_value -  pointer to variable storing PresentValue attribute value
 *  @param reliability - pointer to variable storing Reliability attribute value
 *  @param resolution - pointer to variable storing Resolution attribute value
 *  @param status_flags - pointer to variable storing StatusFlags attribute value
 *  @param engineering_units - pointer to variable storing EngineeringUnits attribute value
 *  @param app_type - pointer to variable storing ApplicationType attribute value
 */
#define ZB_ZCL_DECLARE_ANALOG_INPUT_ATTRIB_LIST(                                           \
    attr_list,                                                                             \
    description,                                                                           \
    max_present_value,                                                                     \
    min_present_value,                                                                     \
    out_of_service,                                                                        \
    present_value,                                                                         \
    reliability,                                                                           \
    resolution,                                                                            \
    status_flags,                                                                          \
    engineering_units,                                                                     \
    app_type)                                                                              \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_ANALOG_INPUT)        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_DESCRIPTION_ID, (description))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_MAX_PRESENT_VALUE_ID, (max_present_value)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_MIN_PRESENT_VALUE_ID, (min_present_value)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID, (out_of_service))       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID, (present_value))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_RELIABILITY_ID, (reliability))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_RESOLUTION_ID, (resolution))               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID, (status_flags))           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_ENGINEERING_UNITS_ID, (engineering_units)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_APPLICATION_TYPE_ID, (app_type))           \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* end of ZB_ZCL_ANALOG_INPUT_ATTRS_GROUP group */

/** @defgroup ZB_ZCL_ANALOG_INPUT_ATTR_API Analog Input cluster attribute value manipulation API
 *  @{
 */

/** @brief Set normal operating mode
 *  @param ep - endpoint number
 */
#define ZB_ZCL_ANALOG_INPUT_SET_NORMAL_MODE(ep)                           \
{                                                                         \
  zb_uint8_t val;                                                         \
                                                                          \
  val = ZB_FALSE;                                                         \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                        \
                       ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID,        \
                       &val, ZB_FALSE);                                   \
                                                                          \
  val = ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_NORMAL;                           \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                        \
                       ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,          \
                       &val, ZB_FALSE);                                   \
}

/** @brief Set Out of service operating mode
 *  @param ep - endpoint number
 */
#define ZB_ZCL_ANALOG_INPUT_SET_OUT_OF_SERVICE(ep)                                          \
{                                                                                           \
  zb_zcl_attr_t *attr_desc;                                                                 \
  zb_uint8_t val;                                                                           \
                                                                                            \
  val = ZB_TRUE;                                                                            \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                                  \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
                       ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID,                          \
                       &val, ZB_FALSE);                                                     \
                                                                                            \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                    \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                            \
                                     ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID);             \
  if (attr_desc)                                                                            \
  {                                                                                         \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OUT_OF_SERVICE; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                                \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
                         ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,                          \
                         &val, ZB_FALSE);                                                   \
  }                                                                                         \
}

/** @brief Set overridden operating mode
 * @param ep - endpoint number
 */
#define ZB_ZCL_ANALOG_INPUT_SET_OVERRIDDEN_MODE(ep)                                    \
{                                                                                      \
  zb_zcl_attr_t *attr_desc;                                                            \
  zb_uint8_t val;                                                                      \
                                                                                       \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                                     ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID);        \
  if (attr_desc)                                                                       \
  {                                                                                    \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OVERRIDDEN;\
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                   \
                         ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,                     \
                         &val, ZB_FALSE);                                              \
  }                                                                                    \
}

/** @brief Set fault status flag
 * @param ep - endpoint number
 */
#define ZB_ZCL_ANALOG_INPUT_SET_FAULT_STATUS(ep)                                       \
{                                                                                      \
  zb_zcl_attr_t *attr_desc;                                                            \
  zb_uint8_t val;                                                                      \
                                                                                       \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                                     ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID);        \
  if (attr_desc)                                                                       \
  {                                                                                    \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_FAULT;     \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                   \
                         ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,                     \
                         &val, ZB_FALSE);                                              \
  }                                                                                    \
}

/** @} */ /* end of ZB_ZCL_ANALOG_INPUT_ATTR_API group */

/** @} */ /* ZCL Analog Input cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_analog_input_init_server(void);
void zb_zcl_analog_input_init_client(void);
#define ZB_ZCL_CLUSTER_ID_ANALOG_INPUT_SERVER_ROLE_INIT zb_zcl_analog_input_init_server
#define ZB_ZCL_CLUSTER_ID_ANALOG_INPUT_CLIENT_ROLE_INIT zb_zcl_analog_input_init_client

#endif /* ZB_ZCL_ANALOG_INPUT_H */
