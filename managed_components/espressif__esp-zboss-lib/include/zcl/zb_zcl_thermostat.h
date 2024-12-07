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
/* PURPOSE: Thermostat cluster definitions
*/

#ifndef ZB_ZCL_THERMOSTAT_H
#define ZB_ZCL_THERMOSTAT_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/* Cluster ZB_ZCL_CLUSTER_ID_THERMOSTAT */

/** @addtogroup ZB_ZCL_THERMOSTAT
 *  @{
 */

/* <31.07.13>  Tokarev CR function prototype moved from C file into header for public access */
/**
 * Thermostat initialization function
 * initialize setpoint mode, schedule table
 */
void zb_zcl_thermostat_init(void);
/* <31.07.13>  Tokarev CR function prototype moved from C file into header for public access */

/*! @name Thermostat cluster attributes
    @{
*/

/*! @brief Thermostat cluster attribute identifiers
    @see ZCL spec, subclause 6.3.2.2
*/
enum zb_zcl_thermostat_attr_e
{
  /** @brief Local Temperature attribute */
  ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID                     = 0x0000,
  /** @brief OutdoorTemperature represents the outdoor temperature in degrees Celsius,
   *  as measured locally or remotely (over the network). */
  ZB_ZCL_ATTR_THERMOSTAT_OUTDOOR_TEMPERATURE_ID                   = 0x0001,
  /** @brief Occupancy specifies whether the heated/cooled space is occupied or not,
   *  as measured locally or remotely (over the network). */
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPANCY_ID                             = 0x0002,
  /** @brief The AbsMinHeatSetpointLimit attribute specifies the absolute minimum level
   *  that the heating setpoint MAY be set to. */
  ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_HEAT_SETPOINT_LIMIT_ID           = 0x0003,
  /** @brief The AbsMaxHeatSetpointLimit attribute specifies the absolute maximum level
   *  that the heating setpoint MAY be set to. */
  ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_HEAT_SETPOINT_LIMIT_ID           = 0x0004,
  /** @brief The AbsMinCoolSetpointLimit attribute specifies the absolute minimum level
   *  that the cooling setpoint MAY be set to. */
  ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_COOL_SETPOINT_LIMIT_ID           = 0x0005,
  /** @brief The AbsMaxCoolSetpointLimit attribute specifies the absolute maximum level
   *  that the cooling setpoint MAY be set to. */
  ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_COOL_SETPOINT_LIMIT_ID           = 0x0006,
  /** @brief The PICoolingDemand attribute is 8 bits in length and specifies the level
   *  of cooling demanded by the PI (proportional integral) control loop in use
   *  by the thermostat (if any), in percent. */
  ZB_ZCL_ATTR_THERMOSTAT_PI_COOLING_DEMAND_ID                     = 0x0007,
  /** @brief The PIHeatingDemand attribute is 8 bits in length and specifies the level
   *  of heating demanded by the PI loop in percent. */
  ZB_ZCL_ATTR_THERMOSTAT_PI_HEATING_DEMAND_ID                     = 0x0008,
  /** @brief The HVACSystemTypeConfiguration attribute specifies the HVAC system type
   *  controlled by the thermostat. */
  ZB_ZCL_ATTR_THERMOSTAT_HVAC_SYSTEM_TYPE_CONFIGURATION_ID        = 0x0009,
  /** @brief Local Temperature Calibration */
  ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID         = 0x0010,
  /** @brief Occupied Cooling Setpoint attribute */
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID             = 0x0011,
  /** @brief Occupied Heating Setpoint attribute */
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID             = 0x0012,
  /** @brief Unoccupied Cooling Setpoint attribute */
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_COOLING_SETPOINT_ID           = 0x0013,
  /** @brief Unoccupied Heating Setpoint attribute */
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_HEATING_SETPOINT_ID           = 0x0014,
  /** @brief The MinHeatSetpointLimit attribute specifies the minimum level that the heating
   *  setpoint MAY be set to. */
  ZB_ZCL_ATTR_THERMOSTAT_MIN_HEAT_SETPOINT_LIMIT_ID               = 0x0015,
  /** @brief The MaxHeatSetpointLimit attribute specifies the maximum level that the heating
   *  setpoint MAY be set to. */
  ZB_ZCL_ATTR_THERMOSTAT_MAX_HEAT_SETPOINT_LIMIT_ID               = 0x0016,
  /** @brief The MinCoolSetpointLimit attribute specifies the minimum level that the cooling
   *  setpoint MAY be set to. */
  ZB_ZCL_ATTR_THERMOSTAT_MIN_COOL_SETPOINT_LIMIT_ID               = 0x0017,
  /** @brief The MaxCoolSetpointLimit attribute specifies the maximum level that the cooling
   *  setpoint MAY be set to. */
  ZB_ZCL_ATTR_THERMOSTAT_MAX_COOL_SETPOINT_LIMIT_ID               = 0x0018,
  /** @brief The MinSetpointDeadBand attribute specifies the minimum difference between the
   *  Heat Setpoint and the Cool SetPoint, in steps of 0.1C. */
  ZB_ZCL_ATTR_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_ID                = 0x0019,
  /** @brief The RemoteSensing attribute is an 8-bit bitmap that specifies whether the local
   *  temperature, outdoor temperature and occupancy are being sensed by internal sensors
   *  or remote networked sensors. */
  ZB_ZCL_ATTR_THERMOSTAT_REMOTE_SENSING_ID                        = 0x001a,
  /** @brief Control Sequence Of Operation attribute */
  ZB_ZCL_ATTR_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_ID         = 0x001b,
  /** @brief System Mode attribute */
  ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID                           = 0x001c,
  /** The AlarmMask attribute specifies whether each of the alarms is enabled. */
  ZB_ZCL_ATTR_THERMOSTAT_ALARM_MASK_ID                            = 0x001d,
  /** @brief Thermostat Running Mode attribute */
  ZB_ZCL_ATTR_THERMOSTAT_RUNNING_MODE_ID                          = 0x001e,
  /*! @brief Start of Week attribute */
  ZB_ZCL_ATTR_THERMOSTAT_START_OF_WEEK_ID                         = 0x0020,
  /** @brief NumberOfWeeklyTransitions attribute determines how many weekly schedule
   *  transitions the thermostat is capable of handling. */
  ZB_ZCL_ATTR_THERMOSTAT_NUMBER_OF_WEEKLY_TRANSITIONS_ID          = 0x0021,
  /** @brief NumberOfDailyTransitions attribute determines how many daily schedule
   *  transitions the thermostat is capable of handling. */
  ZB_ZCL_ATTR_THERMOSTAT_NUMBER_OF_DAILY_TRANSITIONS_ID           = 0x0022,
  /** @brief TemperatureSetpointHold specifies the temperature hold status on the thermostat. */
  ZB_ZCL_ATTR_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_ID             = 0x0023,
  /** @brief TemperatureSetpointHoldDuration sets the period in minutes for which a setpoint
   *  hold is active. */
  ZB_ZCL_ATTR_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_ID    = 0x0024,
  /** @brief The ThermostatProgrammingOperationMode attribute determines the operational state
   *  of the thermostats programming. */
  ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_PROGRAMMING_OPERATION_MODE_ID = 0x0025,
  /** @brief ThermostatRunningState represents the current relay state of the heat, cool,
   *  and fan relays. */
  ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID              = 0x0029,
  /** @brief The SetpointChangeSource attribute specifies the source of the current active
   *  OccupiedCoolingSetpoint or OccupiedHeatingSetpoint (i.e., who or what determined
   *  the current setpoint). */
  ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_SOURCE_ID                = 0x0030,
  /** @brief The SetpointChangeAmount attribute specifies the delta between the current active
   *  OccupiedCoolingSetpoint or OccupiedHeatingSetpoint and the previous active
   *  setpoint. */
  ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_AMOUNT_ID                = 0x0031,
  /** @brief The SetpointChangeSourceTimestamp attribute specifies the time in UTC at which
   *  the SetpointChangeSourceAmount attribute change was recorded. */
  ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_SOURCE_TIMESTAMP_ID      = 0x0032,
  /** @brief Specifies the degrees Celsius, in 0.1 degree increments, the Thermostat server
   *  will allow the LocalTemperature attribute to float above the OccupiedCooling
   *  setpoint (i.e., OccupiedCooling + OccupiedSetback) or below the OccupiedHeating
   *  setpoint (i.e., occupiedHeating, OccupiedSetback) before initiating a state change
   *  to bring the temperature back to the users desired setpoint. */
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_ID                      = 0x0034,
  /** @brief Specifies the minimum degrees Celsius, in 0.1 degree increments,
   *  the Thermostat server will allow the OccupiedSetback attribute
   *  to be configured by a user. */
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_MIN_ID                  = 0x0035,
  /** @brief Specifies the maximum degrees Celsius, in 0.1 degree increments,
   *  the Thermostat server will allow the OccupiedSetback attribute
   *  to be configured by a user. */
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_MAX_ID                  = 0x0036,
  /** @brief Specifies the degrees Celsius, in 0.1 degree increments, the Thermostat server
   *  will allow the LocalTemperature attribute to float above the UnoccupiedCooling
   *  setpoint (i.e., UnoccupiedCooling + UnoccupiedSetback) or below the
   *  UnoccupiedHeating setpoint (i.e., UnoccupiedHeating - UnoccupiedSetback)
   *  before initiating a state change to bring the temperature back
   *  to the users desired setpoint. */
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_ID                    = 0x0037,
  /** @brief Specifies the minimum degrees Celsius, in 0.1 degree increments, the Thermostat
   *  server will allow the UnoccupiedSetback attribute to be configured by a user. */
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_MIN_ID                = 0x0038,
  /** @brief Specifies the maximum degrees Celsius, in 0.1 degree increments, the Thermostat
   *  server will allow the UnoccupiedSetback attribute to be configured by a user. */
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_MAX_ID                = 0x0039,
  /** @brief Specifies the delta, in 0.1 degrees Celsius, between LocalTemperature and
   *  the OccupiedHeatingSetpoint or UnoccupiedHeatingSetpoint attributes at which
   *  the Thermostat server will operate in emergency heat mode. */
  ZB_ZCL_ATTR_THERMOSTAT_EMERGENCY_HEAT_DELTA_ID                  = 0x003a,
  /** @brief Indicates the type of Mini Split ACType of Mini Split AC is defined depending
   *  on how Cooling and Heating condition is achieved by Mini Split AC. */
  ZB_ZCL_ATTR_THERMOSTAT_AC_TYPE_ID                               = 0x0040,
  /** @brief Indicates capacity of Mini Split AC in terms of the format defined by
   *  the ACCapacityFormat attribute. */
  ZB_ZCL_ATTR_THERMOSTAT_AC_CAPACITY_ID                           = 0x0041,
  /** @brief Indicates type of refrigerant used within the Mini Split AC. */
  ZB_ZCL_ATTR_THERMOSTAT_AC_REFRIGERANT_TYPE_ID                   = 0x0042,
  /** @brief This indicates type of Compressor used within the Mini Split AC. */
  ZB_ZCL_ATTR_THERMOSTAT_AC_COMPRESSOR_TYPE_ID                    = 0x0043,
  /** @brief This indicates the type of errors encountered within the Mini Split AC. */
  ZB_ZCL_ATTR_THERMOSTAT_AC_ERROR_CODE_ID                         = 0x0044,
  /** @brief AC Louver position attribute */
  ZB_ZCL_ATTR_THERMOSTAT_AC_LOUVER_POSITION_ID                    = 0x0045,
  /** @brief ACCoilTemperature represents the temperature in degrees Celsius, as measured
   *  locally or remotely (over the network). */
  ZB_ZCL_ATTR_THERMOSTAT_AC_COIL_TEMPERATURE_ID                   = 0x0046,
  /** @brief This is the format for the ACCapacity attribute. */
  ZB_ZCL_ATTR_THERMOSTAT_AC_CAPACITY_FORMAT_ID                    = 0x0047,
};

/*! @brief Values for Control Sequence Of Operation attribute,
    @see ZCL spec, subclause 6.3.2.2.2.12 */
enum zb_zcl_thermostat_control_sequence_of_operation_e
{
  /*! Cooling Only value */
  ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_COOLING_ONLY                            = 0x00,
  /*! Cooling With Reheat value */
  ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_COOLING_WITH_REHEAT                     = 0x01,
  /*! Heating Only value */
  ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_HEATING_ONLY                            = 0x02,
  /*! Heating With Reheat value */
  ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_HEATING_WITH_REHEAT                     = 0x03,
  /*! Cooling and Heating 4-pipes value */
  ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_COOLING_AND_HEATING_4_PIPES             = 0x04,
  /*! Cooling and Heating 4-pipes with Reheat value */
  ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_COOLING_AND_HEATING_4_PIPES_WITH_REHEAT = 0x05,

  ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_RESERVED                                = 0x06
};

/*! @brief Values for System Mode attribute,
    @see ZCL spec, subclause 6.3.2.2.2.13 */
enum zb_zcl_thermostat_system_mode_e
{
  /*! Off value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_OFF               = 0x00,
  /*! Auto value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_AUTO              = 0x01,
  /*! Cool value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_COOL              = 0x03,
  /*! Heat value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_HEAT              = 0x04,
  /*! Emergency Heating value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_EMERGENCY_HEATING = 0x05,
  /*! Precooling value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_PRECOOLING        = 0x06,
  /*! Fan Only value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_FAN_ONLY          = 0x07,
  /*! Dry value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_DRY               = 0x08,
  /*! Sleep value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_SLEEP             = 0x09,
  /* 07/02/2015 CR [AV] */
  /*! Unknown system mode value */
  ZB_ZCL_THERMOSTAT_SYSTEM_MODE_RESERVED = 0x0a
};


/*! @brief Values for AC Louver position attribute */
enum zb_zcl_thermostat_ac_louver_position_e
{
  ZB_ZCL_THERMOSTAT_LOUVER_FULLY_CLOSED        = 0x01,
  ZB_ZCL_THERMOSTAT_LOUVER_FULLY_OPENED        = 0x02,
  ZB_ZCL_THERMOSTAT_LOUVER_QUARTER_OPEN        = 0x03,
  ZB_ZCL_THERMOSTAT_LOUVER_HALF_OPEN           = 0x04,
  ZB_ZCL_THERMOSTAT_LOUVER_THREE_QUARTERS_OPEN = 0x05,
};

/*! @brief Values for Start of Week attribute,
    @see ZCL spec, subclause 6.3.2.2.3.1 */
enum zb_zcl_thermostat_start_of_week_e
{
  /*! Sunday value */
  ZB_ZCL_THERMOSTAT_START_OF_WEEK_SUNDAY         = 0x00,
  /*! Monday value */
  ZB_ZCL_THERMOSTAT_START_OF_WEEK_MONDAY         = 0x01,
  /*! Tuesday value */
  ZB_ZCL_THERMOSTAT_START_OF_WEEK_TUESDAY        = 0x02,
  /*! Wednesday value */
  ZB_ZCL_THERMOSTAT_START_OF_WEEK_WEDNESDAY      = 0x03,
  /*! Thursday value */
  ZB_ZCL_THERMOSTAT_START_OF_WEEK_THURSDAY       = 0x04,
  /*! Friday value */
  ZB_ZCL_THERMOSTAT_START_OF_WEEK_FRIDAY         = 0x05,
  /*! Saturday value */
  ZB_ZCL_THERMOSTAT_START_OF_WEEK_SATURDAY       = 0x06,
};

/*! @brief Values for Temperature Setpoint Hold attribute,
    @see ZCL spec, subclause 6.3.2.2.3.4 */
enum zb_zcl_thermostat_temperature_setpoint_hold_e
{
  /*! Off value */
  ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_OFF = 0x00,
  /*! On value */
  ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_ON  = 0x01,
};

/*! @brief Thermostat Alarm Code for Alarming
    @see ZCL spec 6.3.2.2.2.14, Table 6-18
*/
enum zb_zcl_thermostat_alarm_code_e
{
  /** @brief MainsVoltageAlarmCode - MainsVoltageMinThreshold reached for Mains Voltage**/
    ZB_ZCL_THERMOSTAT_ALARM_CODE_INITIALIZATION_FAILURE = 0x00,
  /** @brief MainsVoltageAlarmCode - MainsVoltageMaxThreshold reached for Mains Voltage**/
    ZB_ZCL_THERMOSTAT_ALARM_CODE_HARDWARE_FAILURE = 0x01,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold or
   * BatteryPercentageMinThreshold reached for Battery Source 1 */
    ZB_ZCL_THERMOSTAT_ALARM_CODE_SELFCALIBRATION_FAILURE = 0x02,
};

/** @brief Default value for Thermostat cluster revision global attribute */
#define ZB_ZCL_THERMOSTAT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/* Default value for Start Of Week attribute */
#define ZB_ZCL_THERMOSTAT_START_OF_WEEK_DEFAULT_VALUE ZB_ZCL_THERMOSTAT_START_OF_WEEK_SUNDAY

/** @brief Min value for Local Temperature attribute */
#define ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_MIN_VALUE 0x954d

/** @brief Max value for Local Temperature attribute */
#define ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_MAX_VALUE 0x7fff

/** @brief Min value for Outdoor Temperature attribute */
#define ZB_ZCL_THERMOSTAT_OUTDOOR_TEMPERATURE_MIN_VALUE 0x954d

/** @brief Max value for Outdoor Temperature attribute */
#define ZB_ZCL_THERMOSTAT_OUTDOOR_TEMPERATURE_MAX_VALUE 0x7fff

/** @brief Invalid value for Local Temperature attribute */
#define ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_INVALID ZB_ZCL_NON_VALUE_INT16

/** @brief Default value for Local Temperature attribute */
#define ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_DEFAULT_VALUE 0xffff

/** @brief Min value for Local Temperature Calibration attribute */
#define ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_MIN_VALUE 0xe7

/** @brief Max value for Local Temperature Calibration attribute */
#define ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_MAX_VALUE 0x19

/** @brief Default value for Occupied Cooling Setpoint attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_DEFAULT_VALUE 0x0a28

/** @brief Min value for Occupied Cooling Setpoint attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_MIN_VALUE 0x954d

/** @brief Max value for Occupied Cooling Setpoint attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_MAX_VALUE 0x7fff

/** @brief Default value for Occupied Heating Setpoint attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_DEFAULT_VALUE 0x07d0

/** @brief Min value for Occupied Heating Setpoint attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_MIN_VALUE 0x954d

/** @brief Max value for Occupied Heating Setpoint attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_MAX_VALUE 0x7fff

/** @brief Min value for Min Setpoint Dead Band attribute */
#define ZB_ZCL_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_MIN_VALUE 0x0a

/** @brief Max value for Min Setpoint Dead Band attribute */
#define ZB_ZCL_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_MAX_VALUE 0x19

/** @brief Local Temperature Sensed Remotely bit of Remote Sensing attribute  */
#define ZB_ZCL_THERMOSTAT_REMOTE_SENSING_LOCAL_TEMPERATURE_SENSED_REMOTELY_BIT (1<<0)

/** @brief Outdoor Temperature Sensed Remotely bit of Remote Sensing attribute  */
#define ZB_ZCL_THERMOSTAT_REMOTE_SENSING_OUTDOOR_TEMPERATURE_SENSED_REMOTELY_BIT (1<<1)

/** @brief Occupancy Sensed Sensed Remotely bit of Remote Sensing attribute  */
#define ZB_ZCL_THERMOSTAT_REMOTE_SENSING_OCCUPANCY_SENSED_REMOTELY_BIT (1<<2)

/** @brief Default value for Control Sequence Of Operation attribute */
#define ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_DEFAULT_VALUE 0x04

/** @brief Default value for System Mode attribute */
#define ZB_ZCL_THERMOSTAT_CONTROL_SYSTEM_MODE_DEFAULT_VALUE 0x01

/** @brief Default value for Occupancy attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPANCY_DEFAULT_VALUE ((zb_uint8_t)1U)

/** @brief Bitmask for Occupancy attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPANCY_BITMASK ((zb_uint8_t)0x01)

/** @brief Default value for AbsMinHeatSetpointLimit attribute */
#define ZB_ZCL_THERMOSTAT_ABS_MIN_HEAT_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x02BC)

/** @brief Default value for AbsMaxHeatSetpointLimit attribute */
#define ZB_ZCL_THERMOSTAT_ABS_MAX_HEAT_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0BB8)

/** @brief Default value for AbsMinCoolSetpointLimit attribute */
#define ZB_ZCL_THERMOSTAT_ABS_MIN_COOL_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0640)

/** @brief Default value for AbsMaxCoolSetpointLimit attribute */
#define ZB_ZCL_THERMOSTAT_ABS_MAX_COOL_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0C80)

/** @brief Min value for PICoolingDemand attribute */
#define ZB_ZCL_THERMOSTAT_PI_COOLING_DEMAND_MIN_VALUE ((zb_uint8_t)0x00)

/** @brief Max value for PICoolingDemand attribute */
#define ZB_ZCL_THERMOSTAT_PI_COOLING_DEMAND_MAX_VALUE ((zb_uint8_t)0x64)

/** @brief Min value for PIHeatingDemand attribute */
#define ZB_ZCL_THERMOSTAT_PI_HEATING_DEMAND_MIN_VALUE ((zb_uint8_t)0x00)

/** @brief Max value for PIHeatingDemand attribute */
#define ZB_ZCL_THERMOSTAT_PI_HEATING_DEMAND_MAX_VALUE ((zb_uint8_t)0x64)

/** @brief Default value for HVACSystemTypeConfiguration attribute */
#define ZB_ZCL_THERMOSTAT_HVAC_SYSTEM_TYPE_CONFIGURATION_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for LocalTemperatureCalibration attribute */
#define ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_DEFAULT_VALUE ((zb_int8_t)0x00)

/** @brief Default value for UnoccupiedCoolingSetpoint attribute */
#define ZB_ZCL_THERMOSTAT_UNOCCUPIED_COOLING_SETPOINT_DEFAULT_VALUE ((zb_int16_t)0x0A28)

/** @brief Default value for UnoccupiedHeatingSetpoint attribute */
#define ZB_ZCL_THERMOSTAT_UNOCCUPIED_HEATING_SETPOINT_DEFAULT_VALUE ((zb_int16_t)0x07D0)

/** @brief Default value for MinHeatSetpointLimit attribute */
#define ZB_ZCL_THERMOSTAT_MIN_HEAT_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x02BC)

/** @brief Default value for MaxHeatSetpointLimit attribute */
#define ZB_ZCL_THERMOSTAT_MAX_HEAT_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0BB8)

/** @brief Default value for MinCoolSetpointLimit attribute */
#define ZB_ZCL_THERMOSTAT_MIN_COOL_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0640)

/** @brief Default value for MaxCoolSetpointLimit attribute */
#define ZB_ZCL_THERMOSTAT_MAX_COOL_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0C80)

/** @brief Default value for MinSetpointDeadBand attribute */
#define ZB_ZCL_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_DEFAULT_VALUE ((zb_int8_t)0x19)

/** @brief Default value for RemoteSensing attribute */
#define ZB_ZCL_THERMOSTAT_REMOTE_SENSING_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for ControlSequenceOfOperation attribute */
#define ZB_ZCL_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_DEFAULT_VALUE ((zb_uint8_t)0x04)

/** @brief Default value for SystemMode attribute */
#define ZB_ZCL_THERMOSTAT_SYSTEM_MODE_DEFAULT_VALUE ((zb_uint8_t)0x01)

/** @brief Default value for AlarmMask attribute */
#define ZB_ZCL_THERMOSTAT_ALARM_MASK_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for RunningMode attribute */
#define ZB_ZCL_THERMOSTAT_RUNNING_MODE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Off value for RunningMode attribute */
#define ZB_ZCL_THERMOSTAT_RUNNING_MODE_OFF_VALUE ((zb_uint8_t)0x00)

/** @brief Cool value for RunningMode attribute */
#define ZB_ZCL_THERMOSTAT_RUNNING_MODE_COOL_VALUE ((zb_uint8_t)0x03)

/** @brief Heat value for RunningMode attribute */
#define ZB_ZCL_THERMOSTAT_RUNNING_MODE_HEAT_VALUE ((zb_uint8_t)0x04)

/** @brief Default value for TemperatureSetpointHold attribute */
#define ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for TemperatureSetpointHoldDuration attribute */
#define ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Min value for TemperatureSetpointHoldDuration attribute */
#define ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_MIN_VALUE ((zb_uint16_t)0x0000)

/** @brief Max value for TemperatureSetpointHoldDuration attribute */
#define ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_MAX_VALUE ((zb_uint16_t)0x05a0)

/** @brief Default value for ThermostatProgrammingOperationMode attribute */
#define ZB_ZCL_THERMOSTAT_THERMOSTAT_PROGRAMMING_OPERATION_MODE_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for SetpointChangeSource attribute */
#define ZB_ZCL_THERMOSTAT_SETPOINT_CHANGE_SOURCE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for SetpointChangeAmount attribute */
#define ZB_ZCL_THERMOSTAT_SETPOINT_CHANGE_AMOUNT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for SetpointChangeSourceTimestamp attribute */
#define ZB_ZCL_THERMOSTAT_SETPOINT_CHANGE_SOURCE_TIMESTAMP_DEFAULT_VALUE ((zb_time_t)0x00000000)

/** @brief Default value for OccupiedSetback attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_SETBACK_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for OccupiedSetbackMin attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_SETBACK_MIN_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for OccupiedSetbackMax attribute */
#define ZB_ZCL_THERMOSTAT_OCCUPIED_SETBACK_MAX_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for UnoccupiedSetback attribute */
#define ZB_ZCL_THERMOSTAT_UNOCCUPIED_SETBACK_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for UnoccupiedSetbackMin attribute */
#define ZB_ZCL_THERMOSTAT_UNOCCUPIED_SETBACK_MIN_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for UnoccupiedSetbackMax attribute */
#define ZB_ZCL_THERMOSTAT_UNOCCUPIED_SETBACK_MAX_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for EmergencyHeatDelta attribute */
#define ZB_ZCL_THERMOSTAT_EMERGENCY_HEAT_DELTA_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for ACType attribute */
#define ZB_ZCL_THERMOSTAT_AC_TYPE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Unknown value for ACType attribute */
#define ZB_ZCL_THERMOSTAT_AC_TYPE_UNKNOWN_VALUE ZB_ZCL_THERMOSTAT_AC_TYPE_DEFAULT_VALUE

/** @brief Cooling and Fixed Speed value for ACType attribute */
#define ZB_ZCL_THERMOSTAT_AC_TYPE_COOLING_AND_FIXED_SPEED_VALUE ((zb_uint8_t)0x01)

/** @brief Heat Pump and Fixed Speed value for ACType attribute */
#define ZB_ZCL_THERMOSTAT_AC_TYPE_HEAT_PUMP_AND_FIXED_SPEED_VALUE ((zb_uint8_t)0x02)

/** @brief Cooling and Inverter value for ACType attribute */
#define ZB_ZCL_THERMOSTAT_AC_TYPE_COOLING_AND_INVERTER_VALUE ((zb_uint8_t)0x03)

/** @brief Heat Pump and Inverter value value for ACType attribute */
#define ZB_ZCL_THERMOSTAT_AC_TYPE_HEAT_PUMP_AND_INVERTER_VALUE ((zb_uint8_t)0x04)

/** @brief Default value for ACCapacity attribute */
#define ZB_ZCL_THERMOSTAT_AC_CAPACITY_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for ACRefrigerantType attribute */
#define ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Unknown value for ACRefrigerantType attribute */
#define ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_UNKNOWN_VALUE ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_DEFAULT_VALUE

/** @brief R22 value for ACRefrigerantType attribute */
#define ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_R22_VALUE ((zb_uint8_t)0x01)

/** @brief R410a value for ACRefrigerantType attribute */
#define ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_R410A_VALUE ((zb_uint8_t)0x02)

/** @brief R407c value for ACRefrigerantType attribute */
#define ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_R407C_VALUE ((zb_uint8_t)0x03)

/** @brief Default value for ACCompressorType attribute */
#define ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Unknown value for ACCompressorType attribute */
#define ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_UNKNOWN_VALUE ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_DEFAULT_VALUE

/** @brief T1(Max working ambient 43 ºC) value for ACCompressorType attribute */
#define ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_T1_VALUE ((zb_uint8_t)0x01)

/** @brief T2(Max working ambient 35 ºC) value for ACCompressorType attribute */
#define ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_T2_VALUE ((zb_uint8_t)0x02)

/** @brief T3(Max working ambient 52 ºC) value for ACCompressorType attribute */
#define ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_T3_VALUE ((zb_uint8_t)0x03)

/** @brief Default value for ACErrorCode attribute */
#define ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_DEFAULT_VALUE ((zb_uint32_t)0x00000000)

/** @brief Compressor Failure or Refrigerant Leakage bit of AC Error Code attribute  */
#define ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_COMPRESSOR_FAILURE_OR_REFRIGERANT_LEAKAGE_BIT (1<<0)

/** @brief Room Temperature Sensor Failure bit of AC Error Code attribute  */
#define ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_ROOM_TEMPERATURE_SENSOR_FAILURE_BIT (1<<1)

/** @brief Outdoor Temperature Sensor Failure bit of AC Error Code attribute  */
#define ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_OUTDOOR_TEMPERATURE_SENSOR_FAILURE_BIT (1<<2)

/** @brief Indoor Coil Temperature Sensor Failure bit of AC Error Code attribute  */
#define ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_INDOOR_COIL_TEMPERATURE_SENSOR_FAILURE_BIT (1<<3)

/** @brief Fan Failure bit of AC Error Code attribute  */
#define ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_FAN_FAILURE_BIT (1<<4)

/** @brief Default value for ACLouverPosition attribute */
#define ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Fully Closed value for ACLouverPosition attribute */
#define ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_FULLY_CLOSED_VALUE ((zb_uint8_t)0x01)

/** @brief Fully Open value for ACLouverPosition attribute */
#define ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_FULLY_OPEN_VALUE ((zb_uint8_t)0x02)

/** @brief Quarter Open value for ACLouverPosition attribute */
#define ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_QUARTER_OPEN_VALUE ((zb_uint8_t)0x03)

/** @brief Three Quarters Open value for ACLouverPosition attribute */
#define ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_THREE_QUARTERS_OPEN_VALUE ((zb_uint8_t)0x05)

/** @brief Half Open value for ACLouverPosition attribute */
#define ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_HALF_OPEN_VALUE ((zb_uint8_t)0x04)

/** @brief Min value for AC Coil Temperature attribute */
#define ZB_ZCL_THERMOSTAT_AC_COIL_TEMPERATURE_MIN_VALUE 0x954d

/** @brief Max value for AC Coil Temperature attribute */
#define ZB_ZCL_THERMOSTAT_AC_COIL_TEMPERATURE_MAX_VALUE 0x7fff

/** @brief Default value for AC Capacity Format attribute */
#define ZB_ZCL_THERMOSTAT_AC_CAPACITY_FORMAT_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief BTUh value for AC Capacity Format attribute */
#define ZB_ZCL_THERMOSTAT_AC_CAPACITY_FORMAT_BTUH_VALUE ZB_ZCL_THERMOSTAT_AC_CAPACITY_FORMAT_DEFAULT_VALUE

/** @brief Default value for ACCapacityFormat attribute */
#define ZB_ZCL_THERMOSTAT_AC_CAPACITY_FORMAT_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Initialization failure bit of Alarm Mask attribute  */
#define ZB_ZCL_THERMOSTAT_ALARM_MASK_INITIALIZATION_FAILURE_BIT (1<<0)

/** @brief Hardware Failure bit of Alarm Mask attribute  */
#define ZB_ZCL_THERMOSTAT_ALARM_MASK_HARDWARE_FAILURE_BIT (1<<1)

/** @brief Self-Calibration Failure bit of Alarm Mask attribute  */
#define ZB_ZCL_THERMOSTAT_ALARM_MASK_SELFCALIBRATION_FAILURE_BIT (1<<2)

/** @brief Simple/Setpoint(0) or Schedule_Programming(1) mode bit of Thermostat Programming Operation Mode attribute  */
#define ZB_ZCL_THERMOSTAT_PROGRAMMING_OPERATION_MODE_SIMPLE_SETPOINT_OR_PROGRAMMING_MODE_BIT  (1<<0)

/** @brief Auto/recovery mode bit of Thermostat Programming Operation Mode attribute  */
#define ZB_ZCL_THERMOSTAT_PROGRAMMING_OPERATION_MODE_AUTO_RECOVERY_MODE_BIT  (1<<1)

/** @brief Economy/EnergyStar mode bit of Thermostat Programming Operation Mode attribute  */
#define ZB_ZCL_THERMOSTAT_PROGRAMMING_OPERATION_MODE_ECONOMY_ENERGY_STAR_MODE_BIT  (1<<2)

/** @brief Heat State On bit of Thermostat Running State attribute  */
#define ZB_ZCL_THERMOSTAT_RUNNNING_STATE_HEAT_STATE_ON_BIT (1<<0)

/** @brief Cool State On bit of Thermostat Running State attribute  */
#define ZB_ZCL_THERMOSTAT_RUNNNING_STATE_COOL_STATE_ON_BIT (1<<1)

/** @brief Fan State On bit of Thermostat Running State attribute  */
#define ZB_ZCL_THERMOSTAT_RUNNNING_STATE_FAN_STATE_ON_BIT (1<<2)

/** @brief Heat 2nd Stage State On bit of Thermostat Running State attribute  */
#define ZB_ZCL_THERMOSTAT_RUNNNING_STATE_HEAT_2ND_STAGE_STATE_ON_BIT (1<<3)

/** @brief Cool 2nd Stage State On bit of Thermostat Running State attribute  */
#define ZB_ZCL_THERMOSTAT_RUNNNING_STATE_COOL_2ND_STAGE_STATE_ON_BIT (1<<4)

/** @brief Fan 2nd Stage State On bit of Thermostat Running State attribute  */
#define ZB_ZCL_THERMOSTAT_RUNNNING_STATE_FAN_2ND_STAGE_STATE_ON_BIT (1<<5)

/** @brief Fan 3rd Stage State On bit of Thermostat Running State attribute  */
#define ZB_ZCL_THERMOSTAT_RUNNNING_STATE_FAN_3RD_STAGE_STATE_ON_BIT (1<<6)

/** @brief Declare attribute list for Thermostat cluster
    @param attr_list - attribute list name
    @param local_temperature - pointer to variable to store Local Temperature attribute value
    @param local_temperature_calibration - pointer to variable to store Local Temperature Calibration attribute value
    @param occupied_cooling_setpoint - pointer to variable to store Occupied Cooling Setpoint attribute value
    @param occupied_heating_setpoint - pointer to variable to store Occupied Heating Setpoint attribute value
    @param control_seq_of_operation - pointer to variable to store Control Sequence Of Operation attribute value
    @param system_mode - pointer to variable to store System Mode attribute value
    @param start_of_week - pointer to variable to store Start of Week attribute value
*/
#define ZB_ZCL_DECLARE_THERMOSTAT_ATTRIB_LIST(attr_list,                                                \
      local_temperature, local_temperature_calibration, occupied_cooling_setpoint,                      \
      occupied_heating_setpoint, control_seq_of_operation, system_mode, start_of_week) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_THERMOSTAT)                       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID, (local_temperature))                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID, (local_temperature_calibration))                        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID, (occupied_cooling_setpoint))        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID, (occupied_heating_setpoint))        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_ID, (control_seq_of_operation))     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, (system_mode))                            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_START_OF_WEEK_ID, (start_of_week)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @brief Declare attribute list for Thermostat cluster
    @param attr_list - attribute list name
    @param local_temperature - pointer to variable to store Local Temperature attribute value
    @param abs_min_heat_setpoint_limit - pointer to variable to store Abs Min Heat Setpoint Limit attribute value
    @param abs_max_heat_setpoint_limit - pointer to variable to store Abs Max Heat Setpoint Limit attribute value
    @param abs_min_cool_setpoint_limit - pointer to variable to store Abs Min Cool Setpoint Limit attribute value
    @param abs_max_cool_setpoint_limit - pointer to variable to store Abs Max Cool Setpoint Limit attribute value
    @param PI_cooling_demand - pointer to variable to store PI Cooling Demand attribute value
    @param PI_heating_demand - pointer to variable to store PI Heating Demand attribute value
    @param HVAC_system_type_configuration - pointer to variable to store HVAC system type
    configuration attribute value; write-optional access according to ZCL8 specification, be careful
    when redefining its handling
    @param local_temperature_calibration - pointer to variable to store Local Temperature
    Calibration attribute value
    @param occupied_cooling_setpoint - pointer to variable to store Occupied Cooling Setpoint attribute value
    @param occupied_heating_setpoint - pointer to variable to store Occupied Heating Setpoint attribute value
    @param unoccupied_cooling_setpoint - pointer to variable to store Unoccupied Cooling Setpoint attribute value
    @param unoccupied_heating_setpoint - pointer to variable to store Unoccupied Heating Setpoint attribute value
    @param min_heat_setpoint_limit - pointer to variable to store Min Heat Setpoint Limit attribute value
    @param max_heat_setpoint_limit - pointer to variable to store Max Heat Setpoint Limit attribute value
    @param min_cool_setpoint_limit - pointer to variable to store Min Cool Setpoint Limit attribute value
    @param max_cool_setpoint_limit - pointer to variable to store Max Cool Setpoint Limit attribute value
    @param min_setpoint_dead_band - pointer to variable to store Min Setpoint Dead Band attribute
    value; write-optional access according to ZCL8 specification, be careful
    when redefining its handling
    @param remote_sensing - pointer to variable to store remote Sensing attribute value
    @param control_seq_of_operation - pointer to variable to store Control Sequence Of Operation attribute value
    @param system_mode - pointer to variable to store System Mode attribute value
    @param start_of_week - pointer to variable to store Start of Week attribute value
*/
#define ZB_ZCL_DECLARE_THERMOSTAT_ATTRIB_LIST_EXT(attr_list,                                                                  \
      local_temperature, abs_min_heat_setpoint_limit, abs_max_heat_setpoint_limit, abs_min_cool_setpoint_limit,               \
      abs_max_cool_setpoint_limit, PI_cooling_demand, PI_heating_demand, HVAC_system_type_configuration,                      \
      local_temperature_calibration, occupied_cooling_setpoint, occupied_heating_setpoint, unoccupied_cooling_setpoint,       \
      unoccupied_heating_setpoint, min_heat_setpoint_limit, max_heat_setpoint_limit, min_cool_setpoint_limit,                 \
      max_cool_setpoint_limit, min_setpoint_dead_band, remote_sensing, control_seq_of_operation, system_mode, start_of_week)  \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_THERMOSTAT)                                             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID, (local_temperature))                                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_HEAT_SETPOINT_LIMIT_ID, (abs_min_heat_setpoint_limit))                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_HEAT_SETPOINT_LIMIT_ID, (abs_max_heat_setpoint_limit))                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_COOL_SETPOINT_LIMIT_ID, (abs_min_cool_setpoint_limit))                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_COOL_SETPOINT_LIMIT_ID, (abs_max_cool_setpoint_limit))                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_PI_COOLING_DEMAND_ID, (PI_cooling_demand))                                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_PI_HEATING_DEMAND_ID, (PI_heating_demand))                                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_HVAC_SYSTEM_TYPE_CONFIGURATION_ID, (HVAC_system_type_configuration))            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID, (local_temperature_calibration))              \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID, (occupied_cooling_setpoint))                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID, (occupied_heating_setpoint))                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_COOLING_SETPOINT_ID, (unoccupied_cooling_setpoint))                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_HEATING_SETPOINT_ID, (unoccupied_heating_setpoint))                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_MIN_HEAT_SETPOINT_LIMIT_ID, (min_heat_setpoint_limit))                          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_MAX_HEAT_SETPOINT_LIMIT_ID, (max_heat_setpoint_limit))                          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_MIN_COOL_SETPOINT_LIMIT_ID, (min_cool_setpoint_limit))                          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_MAX_COOL_SETPOINT_LIMIT_ID, (max_cool_setpoint_limit))                          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_ID, (min_setpoint_dead_band))                            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_REMOTE_SENSING_ID, (remote_sensing))                                            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_ID, (control_seq_of_operation))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID, (system_mode))                                                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_START_OF_WEEK_ID, (start_of_week))                                              \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @brief Bits of ThermostatProgrammingOperationMode */
enum zb_zcl_thermostat_programming_operation_mode_bit_e
{
  /*! Schedule programming bit */
  ZB_ZCL_THERMOSTAT_SCHEDULE_PROGRAMMING_MODE_BIT             = 0x01,
  /*! Auto/recovery bit  */
  ZB_ZCL_THERMOSTAT_AUTO_RECOVERY_PROGRAMMING_MODE_BIT        = 0x02,
  /*! Economy/energy star bit */
  ZB_ZCL_THERMOSTAT_ECONOMY_ENERGY_START_PROGRAMMING_MODE_BIT = 0x04,
};

/*! @} */ /* Thermostat cluster attributes */

/*! @name Thermostat cluster commands
    @{
*/

/*! @brief Thermostat cluster command identifiers
    @see ZCL spec, subclause 6.3.2.3
*/
enum zb_zcl_thermostat_cmd_e
{
  /** Setpoint Raise/Lower command */
  ZB_ZCL_CMD_THERMOSTAT_SETPOINT_RAISE_LOWER  = 0x00,
  /** Set Weekly Schedule command */
  ZB_ZCL_CMD_THERMOSTAT_SET_WEEKLY_SCHEDULE   = 0x01,
  /** Get Weekly Schedule command */
  ZB_ZCL_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE   = 0x02,
  /** Clear Weekly Schedule command */
  ZB_ZCL_CMD_THERMOSTAT_CLEAR_WEEKLY_SCHEDULE = 0x03,
  /** Get Relay Status Log command */
  ZB_ZCL_CMD_THERMOSTAT_GET_RELAY_STATUS_LOG  = 0x04
};

/*! @brief Thermostat cluster command identifiers
    @see ZCL spec, subclause 6.3.2.4
*/
enum zb_zcl_thermostat_cmd_resp_ha_e
{
  /** Get Weekly Schedule response command */
  ZB_ZCL_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE_RESP  = 0x00,
  /** Get Relay Status Log response command */
  ZB_ZCL_CMD_THERMOSTAT_GET_RELAY_STATUS_LOG_RESP = 0x01
};

/** @cond internals_doc */
/* Thermostat cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_SERVER_ROLE_GENERATED_CMD_LIST                 \
                                      ZB_ZCL_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE_RESP,  \
                                      ZB_ZCL_CMD_THERMOSTAT_GET_RELAY_STATUS_LOG_RESP

#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_THERMOSTAT_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_CLIENT_ROLE_GENERATED_CMD_LIST                 \
                                      ZB_ZCL_CMD_THERMOSTAT_SETPOINT_RAISE_LOWER,      \
                                      ZB_ZCL_CMD_THERMOSTAT_SET_WEEKLY_SCHEDULE,       \
                                      ZB_ZCL_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE,       \
                                      ZB_ZCL_CMD_THERMOSTAT_CLEAR_WEEKLY_SCHEDULE,     \
                                      ZB_ZCL_CMD_THERMOSTAT_GET_RELAY_STATUS_LOG

#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_THERMOSTAT_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/*! @} */ /* Thermostat cluster commands */

/*!
    @cond internals_doc
    @internal @name Thermostat cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID(data_ptr)             \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID,                                                   \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_OUTDOOR_TEMPERATURE_ID(data_ptr)           \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_OUTDOOR_TEMPERATURE_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_OCCUPANCY_ID(data_ptr)                     \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPANCY_ID,                                                           \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_HEAT_SETPOINT_LIMIT_ID(data_ptr)   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_HEAT_SETPOINT_LIMIT_ID,                                         \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_HEAT_SETPOINT_LIMIT_ID(data_ptr)   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_HEAT_SETPOINT_LIMIT_ID,                                         \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_COOL_SETPOINT_LIMIT_ID(data_ptr)   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_COOL_SETPOINT_LIMIT_ID,                                         \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_COOL_SETPOINT_LIMIT_ID(data_ptr)   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_COOL_SETPOINT_LIMIT_ID,                                         \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_PI_COOLING_DEMAND_ID(data_ptr)             \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_PI_COOLING_DEMAND_ID,                                                   \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_PI_HEATING_DEMAND_ID(data_ptr)             \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_PI_HEATING_DEMAND_ID,                                                   \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_PROGRAMMING_OPERATION_MODE_ID(data_ptr)   \
{                                                                                                   \
  ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_PROGRAMMING_OPERATION_MODE_ID,                                  \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                               \
  (void*) data_ptr                                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID(data_ptr)         \
{                                                                                                   \
  ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID,                                               \
  ZB_ZCL_ATTR_TYPE_16BITMAP,                                                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                               \
  (void*) data_ptr                                                                                  \
}

/** According to ZCL8, Table 6-11. Attributes of the Thermostat Information Attribute Set,
 * ZB_ZCL_ATTR_THERMOSTAT_HVAC_SYSTEM_TYPE_CONFIGURATION_ID attribute access was changed to Read and Write
 * Optional. For compatibility reasons access rights in the following constructor were not changed.
 *
 * If it is needed to change access rights to this attribute, this macros should be modified. */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_HVAC_SYSTEM_TYPE_CONFIGURATION_ID(data_ptr)   \
{                                                                                                   \
  ZB_ZCL_ATTR_THERMOSTAT_HVAC_SYSTEM_TYPE_CONFIGURATION_ID,                                         \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                               \
  (void*) data_ptr                                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID(data_ptr) \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID,                                       \
  ZB_ZCL_ATTR_TYPE_S8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID(data_ptr)     \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID,                                           \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_SCENE,                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID(data_ptr)     \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID,                                           \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_SCENE,                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_COOLING_SETPOINT_ID(data_ptr)   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_COOLING_SETPOINT_ID,                                         \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_HEATING_SETPOINT_ID(data_ptr)   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_HEATING_SETPOINT_ID,                                         \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_MIN_HEAT_SETPOINT_LIMIT_ID(data_ptr)       \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_MIN_HEAT_SETPOINT_LIMIT_ID,                                             \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_MAX_HEAT_SETPOINT_LIMIT_ID(data_ptr)       \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_MAX_HEAT_SETPOINT_LIMIT_ID,                                             \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_MIN_COOL_SETPOINT_LIMIT_ID(data_ptr)       \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_MIN_COOL_SETPOINT_LIMIT_ID,                                             \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_MAX_COOL_SETPOINT_LIMIT_ID(data_ptr)       \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_MAX_COOL_SETPOINT_LIMIT_ID,                                             \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

/** According to ZCL8, Table 6-13. Attributes of the Thermostat Settings Attribute Set,
 * ZB_ZCL_ATTR_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_ID attribute access was changed to Read and Write
 * Optional. For compatibility reasons access rights in the following constructor were not changed.
 *
 * If it is needed to change access rights to this attribute, this macros should be modified. */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_ID(data_ptr)        \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_ID,                                              \
  ZB_ZCL_ATTR_TYPE_S8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_REMOTE_SENSING_ID(data_ptr)                \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_REMOTE_SENSING_ID,                                                      \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_SCENE,                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_ID(data_ptr) \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID(data_ptr)                   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID,                                                         \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_SCENE,                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_ALARM_MASK_ID(data_ptr)                    \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_ALARM_MASK_ID,                                                          \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_RUNNING_MODE_ID(data_ptr)                  \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_RUNNING_MODE_ID,                                                        \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_START_OF_WEEK_ID(data_ptr)                 \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_START_OF_WEEK_ID,                                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_NUMBER_OF_WEEKLY_TRANSITIONS_ID(data_ptr)  \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_NUMBER_OF_WEEKLY_TRANSITIONS_ID,                                        \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_NUMBER_OF_DAILY_TRANSITIONS_ID(data_ptr)   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_NUMBER_OF_DAILY_TRANSITIONS_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_ID(data_ptr)     \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_ID,                                           \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_ID(data_ptr)     \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_ID,                                  \
  ZB_ZCL_ATTR_TYPE_U16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_SOURCE_ID(data_ptr)        \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_SOURCE_ID,                                              \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_AMOUNT_ID(data_ptr)        \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_AMOUNT_TIMESTAMP_ID,                                    \
  ZB_ZCL_ATTR_TYPE_U16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_SOURCE_TIMESTAMP_ID(data_ptr) \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_SOURCE_TIMESTAMP_ID,                                    \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_ID(data_ptr)              \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_ID,                                                    \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_MIN_ID(data_ptr)          \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_MIN_ID,                                                \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_MAX_ID(data_ptr)          \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_MAX_ID,                                                \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_ID(data_ptr)            \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_ID,                                                  \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_MIN_ID(data_ptr)        \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_MIN_ID,                                              \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_MAX_ID(data_ptr)        \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_MAX_ID,                                              \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_EMERGENCY_HEAT_DELTA_ID(data_ptr)          \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_EMERGENCY_HEAT_DELTA_ID,                                                \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_AC_TYPE_ID(data_ptr)                       \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_AC_TYPE_ID,                                                             \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_AC_CAPACITY_ID(data_ptr)                   \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_AC_CAPACITY_ID,                                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_AC_REFRIGERANT_TYPE_ID(data_ptr)           \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_AC_REFRIGERANT_TYPE_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_AC_COMPRESSOR_TYPE_ID(data_ptr)            \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_AC_COMPRESSOR_TYPE_ID,                                                  \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_AC_ERROR_CODE_ID(data_ptr)                 \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_AC_ERROR_CODE_ID,                                                       \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_AC_LOUVER_POSITION_ID(data_ptr)            \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_AC_LOUVER_POSITION_ID,                                                  \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_AC_COIL_TEMPERATURE_ID(data_ptr)           \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_AC_COIL_TEMPERATURE_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_S16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_AC_CAPACITY_FORMAT_ID(data_ptr)            \
{                                                                                                \
  ZB_ZCL_ATTR_THERMOSTAT_AC_CAPACITY_FORMAT_ID,                                                  \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

/*! @internal Number of attributes mandatory for reporting in Thermostat cluster */
#define ZB_ZCL_THERMOSTAT_REPORT_ATTR_COUNT 1

/***************************** commands *****************************/

/* Setpoint is defined in 0.01C, while amount is defined in 0.1C */
#define ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_AMOUNT_MUL 10

/** @internal Structure for Setpoint Raise/Lower command
 */
typedef ZB_PACKED_PRE struct zb_zcl_thermostat_setpoint_raise_lower_req_s
{
  zb_uint8_t mode;
  zb_int8_t amount;
} ZB_PACKED_STRUCT zb_zcl_thermostat_setpoint_raise_lower_req_t;

/*! @brief Values for Mode field,
    @see ZCL spec, subclause 6.3.2.3.1.2 */
enum zb_zcl_thermostat_setpoint_raise_lower_mode_e
{
  /*! Heat value */
  ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_HEAT     = 0x00,
  /*! Cool value */
  ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_COOL     = 0x01,
  /*! Both (Heat and Cool) value */
  ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_BOTH     = 0x02,

  ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_RESERVED = 0x03
};

#define ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_thermostat_setpoint_raise_lower_req_t)

/*! @brief Send Setpoint Raise/Lower command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param mode - Mode value
    @param amount - Amount value
*/
#define ZB_ZCL_THERMOSTAT_SEND_SETPOINT_RAISE_LOWER_REQ(                                 \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, mode, amount)          \
{                                                                                        \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                         \
        ZB_ZCL_CMD_THERMOSTAT_SETPOINT_RAISE_LOWER);                                     \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (mode));                                                  \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (amount));                                                \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                             \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_THERMOSTAT, cb); \
}

/*!
  @brief Parses Get Setpoint Raise/Lower command and fills data request
  structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param setpoint_raise_lower_req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_THERMOSTAT_GET_SETPOINT_RAISE_LOWER_REQ(data_buf, req, status)           \
{                                                                                       \
  zb_zcl_thermostat_setpoint_raise_lower_req_t *req_ptr =                               \
     zb_buf_len(data_buf) >= ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_REQ_PAYLOAD_LEN ?   \
         (zb_zcl_thermostat_setpoint_raise_lower_req_t*)zb_buf_begin(data_buf) : NULL;  \
  if (req_ptr != NULL)                                                                  \
  {                                                                                     \
    ZB_MEMCPY(&(req), req_ptr, sizeof(zb_zcl_thermostat_setpoint_raise_lower_req_t));   \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                               \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                               \
  }                                                                                     \
}

/* Add/Get Weekly Schedule record for Set/Get Weekly Schedule commands */

/** @internal Structure for Weekly Schedule record
 */
typedef ZB_PACKED_PRE struct zb_zcl_thermostat_weekly_schedule_point_pair_s
{
  zb_uint16_t transition_time;
  zb_uint16_t heat_set_point;
  zb_uint16_t cool_set_point;
} ZB_PACKED_STRUCT zb_zcl_thermostat_weekly_schedule_point_pair_t;

#define ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_POINT_PAIR_FULL_SIZE \
  sizeof(zb_zcl_thermostat_weekly_schedule_point_pair_t)

#define ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_POINT_PAIR_ONE_SIZE \
  sizeof(zb_zcl_thermostat_weekly_schedule_point_pair_t) - sizeof(zb_uint16_t)

#define ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MAX_NUM_OF_TRANSITIONS 10

/*! @brief Values for Mode for Sequence field,
    @see ZCL spec, subclause 10.1.3.3.1.4 */
enum zb_zcl_thermostat_weekly_schedule_mode_for_seq_e
{
  /*! Heat value */
  ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_HEAT     = 0x01,
  /*! Cool value */
  ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_COOL     = 0x02,
  /*! Both (Heat and Cool) value */
  ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_BOTH     = 0x03,

  ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_RESERVED = 0x04
};

/** @brief Add attribute value to command payload
   @param cmd_ptr - pointer to a command data memory
   @param mode_for_seq - Mode For Sequence value (from header)
   @param transition_time - Transition Time value
   @param heat_set_point - Heat Set Point value
   @param cool_set_point - Cool Set Point value
*/
#define ZB_ZCL_THERMOSTAT_ADD_VALUE_WEEKLY_SCHEDULE(cmd_ptr,                    \
      mode_for_seq, transition_time, heat_set_point, cool_set_point)            \
{                                                                               \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (transition_time));                     \
  if (mode_for_seq & ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_HEAT)       \
  {                                                                             \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (heat_set_point));                    \
  }                                                                             \
  if (mode_for_seq & ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_COOL)       \
  {                                                                             \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (cool_set_point));                    \
  }                                                                             \
}

/** @brief Parses Set Weekly Schedule request and returns next Set point pair record or NULL if there
  * is no more data.

   If request contains invalid data, NULL is returned.
   @param data_buf - pointer to zb_buf_t buffer containing write attribute request data
   @param mode_for_seq - Mode For Sequence value (from Set Weekly Schedule header)
   @param pair_req - out pointer to Set Point Pair record
 */
#define ZB_ZCL_THERMOSTAT_GET_NEXT_WEEKLY_SCHEDULE_POINT_PAIR_REQ(data_buf, mode_for_seq, pair_req, status) \
{                                                                                       \
  zb_zcl_thermostat_weekly_schedule_point_pair_t *pair_req_ptr;                         \
  zb_uint8_t req_size;                                                                  \
                                                                                        \
  if (mode_for_seq == ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_BOTH)              \
  {                                                                                     \
    req_size = ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_POINT_PAIR_FULL_SIZE;                  \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    req_size = ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_POINT_PAIR_ONE_SIZE;                   \
  }                                                                                     \
                                                                                        \
  pair_req_ptr = (zb_buf_len(data_buf) >= req_size) ?                                   \
      (zb_zcl_thermostat_weekly_schedule_point_pair_t*)zb_buf_begin(data_buf) : NULL;   \
  if (pair_req_ptr)                                                                     \
  {                                                                                     \
    ZB_MEMCPY(&(pair_req), pair_req_ptr, req_size);                                     \
                                                                                        \
    ZB_ZCL_HTOLE16_INPLACE(&(pair_req.transition_time));                                \
    ZB_ZCL_HTOLE16_INPLACE(&(pair_req.heat_set_point));                                 \
    if (mode_for_seq == ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_COOL)            \
    {                                                                                   \
      (pair_req).cool_set_point = (pair_req).heat_set_point;                            \
    }                                                                                   \
                                                                                        \
    (void)zb_buf_cut_left((data_buf), req_size);                                        \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                             \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                             \
  }                                                                                     \
}

/* Set Weekly Schedule command */

typedef ZB_PACKED_PRE struct zb_zcl_thermostat_set_weekly_schedule_req_s
{
  zb_uint8_t num_of_transitions;
  zb_uint8_t day_of_week;
  zb_uint8_t mode_for_seq;
} ZB_PACKED_STRUCT zb_zcl_thermostat_set_weekly_schedule_req_t;

#define ZB_ZCL_THERMOSTAT_SET_WEEKLY_SCHEDULE_REQ_SIZE \
  sizeof(zb_zcl_thermostat_set_weekly_schedule_req_t)

/* 2018/08/10 CR:MAJOR (rev. 39327): According to the spec, one Set Weekly Schedule pkt may contain up to 10 df
 * transitions, but we support only one. */

/** @brief Initialize Set Weekly Schedule command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param def_resp - enable/disable default response
    @param num_of_transition - Number Of Transition value
    @param day_of_week - Day Of Week value
    @param mode_for_seq - Mode For Sequence value
    @param transition - Transition Time value
    @param heat_set - Heat Set value
    @param cool_set - Cool Set value
*/
#define ZB_ZCL_THERMOSTAT_INIT_SET_WEEKLY_SCHEDULE_REQ(buffer,                  \
    cmd_ptr, def_resp, num_of_transition, day_of_week, mode_for_seq,            \
    transition, heat_set, cool_set)                                             \
{                                                                               \
  cmd_ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(cmd_ptr, def_resp)        \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(),            \
      ZB_ZCL_CMD_THERMOSTAT_SET_WEEKLY_SCHEDULE);                               \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (num_of_transition));                        \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (day_of_week));                              \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (mode_for_seq));                             \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (transition));                          \
  if (ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_HEAT == (mode_for_seq))    \
  {                                                                             \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (heat_set));                          \
  }                                                                             \
  else                                                                          \
  if (ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_COOL == (mode_for_seq))    \
  {                                                                             \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (cool_set));                          \
  }                                                                             \
  else                                                                          \
  if (ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_BOTH == (mode_for_seq))    \
  {                                                                             \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (heat_set));                          \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (cool_set));                          \
  }                                                                             \
}


/** @brief Send Set Weekly Schedule command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param profile_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_THERMOSTAT_SEND_SET_WEEKLY_SCHEDULE_REQ(                                 \
    buffer, cmd_ptr, addr, dst_addr_mode, dst_ep, ep, profile_id, cb)                   \
{                                                                                       \
  ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr)                                                \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, profile_id,        \
      ZB_ZCL_CLUSTER_ID_THERMOSTAT, cb);                                                \
}

/** @brief Parses Set Weekly Schedule request header

   If request contains invalid data, status is ZB_FALSE.
   @param data_buf - pointer to zb_buf_t buffer containing write attribute request data
   @param req - record of type @ref zb_zcl_thermostat_set_weekly_schedule_req_t
   @param status - result of parsing
 */
#define ZB_ZCL_THERMOSTAT_GET_SET_WEEKLY_SCHEDULE_REQ(data_buf, req, status)                    \
{                                                                                               \
  zb_zcl_thermostat_set_weekly_schedule_req_t *req_ptr =                                        \
        zb_buf_len(data_buf) >= ZB_ZCL_THERMOSTAT_SET_WEEKLY_SCHEDULE_REQ_SIZE ?                \
              (zb_zcl_thermostat_set_weekly_schedule_req_t*)zb_buf_begin(data_buf) : NULL;      \
  if (req_ptr)                                                                                  \
  {                                                                                             \
    ZB_MEMCPY(&(req), req_ptr, sizeof(zb_zcl_thermostat_set_weekly_schedule_req_t));            \
    ZB_ASSERT(req.num_of_transitions <= ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MAX_NUM_OF_TRANSITIONS);\
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                       \
                                                                                                \
    (void)zb_buf_cut_left((data_buf), sizeof(zb_zcl_thermostat_set_weekly_schedule_req_t));     \
  }                                                                                             \
  else                                                                                          \
  {                                                                                             \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                       \
  }                                                                                             \
}

/* Get Weekly Schedule command */

typedef ZB_PACKED_PRE struct zb_zcl_thermostat_get_weekly_schedule_req_s
{
  zb_uint8_t days_to_return;
  zb_uint8_t mode_to_return;
} ZB_PACKED_STRUCT zb_zcl_thermostat_get_weekly_schedule_req_t;

#define ZB_ZCL_THERMOSTAT_GET_WEEKLY_SCHEDULE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_thermostat_get_weekly_schedule_req_t)

/*! @brief Send Get Weekly Schedule command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param days_to_return - Days To Return value
    @param mode_to_return - Mode To Return value
*/
#define ZB_ZCL_THERMOSTAT_SEND_GET_WEEKLY_SCHEDULE_REQ(                 \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,       \
  days_to_return, mode_to_return)                                       \
{                                                                       \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)    \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),        \
        ZB_ZCL_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE);                     \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (days_to_return));                       \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (mode_to_return));                       \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                    \
  ZB_ZCL_SEND_COMMAND_SHORT((buffer), (addr), (dst_addr_mode),          \
        (dst_ep), (ep), (prfl_id), ZB_ZCL_CLUSTER_ID_THERMOSTAT, (cb)); \
}

/*!
  @brief Parses Get Weekly Schedule command and fills data request
  structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - record of type @ref zb_zcl_thermostat_set_weekly_schedule_req_t
  @param status - success or not read parameters - ZB_ZCL_PARSE_STATUS_SUCCESS or
  ZB_ZCL_PARSE_STATUS_FAILURE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_THERMOSTAT_GET_GET_WEEKLY_SCHEDULE_REQ(data_buf, req, status)            \
{                                                                                       \
  zb_zcl_thermostat_get_weekly_schedule_req_t *req_ptr =                                \
        zb_buf_len(data_buf) >= ZB_ZCL_THERMOSTAT_GET_WEEKLY_SCHEDULE_REQ_PAYLOAD_LEN ? \
            (zb_zcl_thermostat_get_weekly_schedule_req_t*)zb_buf_begin(data_buf) : NULL;\
  if (req_ptr)                                                                          \
  {                                                                                     \
    ZB_MEMCPY(&(req), req_ptr, sizeof(zb_zcl_thermostat_get_weekly_schedule_req_t));    \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                               \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                               \
  }                                                                                     \
}

/* Get Weekly Schedule response command */

/** Structure for Get Weekly Schedule resp command
 */
typedef ZB_PACKED_PRE struct zb_zcl_thermostat_get_weekly_schedule_resp_s
{
  zb_uint8_t num_of_transitions;
  zb_uint8_t day_of_week;
  zb_uint8_t mode_for_seq;
} ZB_PACKED_STRUCT zb_zcl_thermostat_get_weekly_schedule_resp_t;

/** @brief Initialize Get Weekly Schedule resp command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param seq - sequency
    @param num_of_transitions - Number Of Transition value
    @param day_of_week - Day Of Week value
    @param mode_for_seq - Mode For Sequence value
*/
#define ZB_ZCL_THERMOSTAT_INIT_GET_WEEKLY_SCHEDULE_RESP(buffer, cmd_ptr, seq,   \
    num_of_transitions, day_of_week, mode_for_seq)                              \
{                                                                               \
  cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                        \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(cmd_ptr);                 \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(cmd_ptr, (seq),                               \
        ZB_ZCL_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE_RESP);                        \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (num_of_transitions));                       \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (day_of_week));                              \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (mode_for_seq));                             \
}

/** @brief Send Get Weekly Schedule resp command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param profile_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_THERMOSTAT_SEND_GET_WEEKLY_SCHEDULE_RESP(                        \
    buffer, cmd_ptr, addr, dst_addr_mode, dst_ep, ep, profile_id, cb)           \
{                                                                               \
  ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr)                                        \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, profile_id,\
      ZB_ZCL_CLUSTER_ID_THERMOSTAT, cb);                                        \
}

/** @brief Parses Get Weekly Schedule response header

   If request contains invalid data, status is ZB_FALSE.
   @param data_buf - pointer to zb_buf_t buffer containing write attribute request data
   @param resp - record of type @ref zb_zcl_thermostat_get_weekly_schedule_resp_t
   @param status - result of parsing
 */
#define ZB_ZCL_THERMOSTAT_GET_GET_WEEKLY_SCHEDULE_RESP(data_buf, resp, status)          \
{                                                                                       \
  zb_zcl_thermostat_get_weekly_schedule_resp_t *resp_ptr =                              \
        zb_buf_len(data_buf) >= sizeof(zb_zcl_thermostat_get_weekly_schedule_resp_t) ?  \
              (zb_zcl_thermostat_get_weekly_schedule_resp_t*)zb_buf_begin(data_buf) : NULL; \
  if (resp_ptr)                                                                         \
  {                                                                                     \
    ZB_MEMCPY(&(resp), resp_ptr, sizeof(zb_zcl_thermostat_get_weekly_schedule_resp_t)); \
    ZB_ASSERT(resp.num_of_transitions <=                                                \
              ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MAX_NUM_OF_TRANSITIONS);                \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                               \
                                                                                        \
    (void)zb_buf_cut_left((data_buf), sizeof(zb_zcl_thermostat_get_weekly_schedule_resp_t)); \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                               \
  }                                                                                     \
}

/* Clear Weekly Schedule command */
/*! @brief Clear Weekly Schedule command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_THERMOSTAT_SEND_CLEAR_WEEKLY_SCHEDULE_REQ(                       \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)             \
{                                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                             \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)            \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_THERMOSTAT_CLEAR_WEEKLY_SCHEDULE);                             \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                            \
  ZB_ZCL_SEND_COMMAND_SHORT((buffer), (addr), (dst_addr_mode), (dst_ep),        \
      (ep), (prfl_id), ZB_ZCL_CLUSTER_ID_THERMOSTAT, (cb));                     \
}

/* Get Relay Status Log command */
/*! @brief Send Get Relay Status Log req command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_THERMOSTAT_SEND_GET_RELAY_STATUS_LOG_REQ(                        \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)             \
{                                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                             \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)            \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_THERMOSTAT_GET_RELAY_STATUS_LOG);                              \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                            \
  ZB_ZCL_SEND_COMMAND_SHORT((buffer), (addr), (dst_addr_mode), (dst_ep),        \
      (ep), (prfl_id), ZB_ZCL_CLUSTER_ID_THERMOSTAT, (cb));                     \
}

/* Get Relay Status Log response */

typedef ZB_PACKED_PRE struct zb_zcl_thermostat_get_relay_status_log_resp_s
{
  zb_uint16_t time_of_day;
  zb_uint8_t relay_status;
  zb_int16_t local_temperature;
  zb_uint8_t humidity_in_percentage;
  zb_int16_t set_point;
  zb_uint16_t unread_entries;
} ZB_PACKED_STRUCT zb_zcl_thermostat_get_relay_status_log_resp_t;

#define ZB_ZCL_THERMOSTAT_GET_RELAY_STATUS_LOG_RESP_SIZE \
  sizeof(zb_zcl_thermostat_get_relay_status_log_resp_t)

/*! @brief Send Get Relay Status Log resp command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param seq - sequency
    @param cb - callback for getting command send status
    @param time_of_day - Time Of Day value
    @param relay_status - Relay Status value
    @param local_temperature - Local Temperature value
    @param humidity_in_percentage - Humidity In Percentage value
    @param set_point - Set Point value
    @param unread_entries - Unread Entries value
*/
#define ZB_ZCL_THERMOSTAT_SEND_GET_RELAY_STATUS_LOG_RESP(                                       \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, seq, cb, time_of_day, relay_status,         \
  local_temperature, humidity_in_percentage, set_point, unread_entries)                         \
{                                                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                                \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);                                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq, ZB_ZCL_CMD_THERMOSTAT_GET_RELAY_STATUS_LOG_RESP);   \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (time_of_day));                                             \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (relay_status));                                                 \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (local_temperature));                                       \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (humidity_in_percentage));                                       \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (set_point));                                               \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (unread_entries));                                          \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                            \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                    \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_THERMOSTAT, cb);        \
}

/*!
  @brief Parses Get Relay Status Log command and fills data request
  structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param resp - variable to save command request
  @param status - success or not read parameters - ZB_ZCL_PARSE_STATUS_SUCCESS or
  ZB_ZCL_PARSE_STATUS_FAILURE
    @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_THERMOSTAT_GET_GET_RELAY_STATUS_LOG_RESP(data_buf, resp, status)         \
{                                                                                       \
  zb_zcl_thermostat_get_relay_status_log_resp_t *resp_ptr =                             \
     zb_buf_len(data_buf) >= ZB_ZCL_THERMOSTAT_GET_RELAY_STATUS_LOG_RESP_SIZE ?         \
     (zb_zcl_thermostat_get_relay_status_log_resp_t*)zb_buf_begin(data_buf) : NULL;     \
  if (resp_ptr)                                                                         \
  {                                                                                     \
    ZB_MEMCPY(&(resp), resp_ptr, sizeof(zb_zcl_thermostat_get_relay_status_log_resp_t));\
    ZB_ZCL_HTOLE16_INPLACE(&(resp)->time_of_day);                                       \
    /* FIXME: Can we use it with signed 16-bit int? */                                  \
    ZB_ZCL_HTOLE16_INPLACE(&(resp)->local_temperature);                                 \
    ZB_ZCL_HTOLE16_INPLACE(&(resp)->set_point);                                         \
    ZB_ZCL_HTOLE16_INPLACE(&(resp)->unread_entries);                                    \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                               \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                               \
  }                                                                                     \
}

/*! @}
 *  @endcond */ /* Thermostat cluster internals */

/*! @cond internals_doc
    @name Thermostat cluster command internals
    Internal structures and defines for execute cluster command handlers.
    @{
*/

/** @brief Size of table with Weekly Schedule records */
#define ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_TABLE_SIZE    20

/** @brief Flag days field "record is free" */
#define ZB_ZCL_THERMOSTAT_WEEKLY_FREE_RECORD            0

/** @brief Bit set "All days" */
#define ZB_ZCL_THERMOSTAT_WEEKLY_ALL                    0x7f

/** @brief Bit "Weekly vacation" */
#define ZB_ZCL_THERMOSTAT_WEEKLY_VACATION               (1<<7)

/** @brief Parameter - Thermostat will be schedule
 * without any specific record*/
#define ZB_ZCL_THERMOSTAT_PROCESS_NONE                  0xff

/** @brief Second per week */
#define ZB_SECOND_PER_WEEK                              (60*60*24*7)

/** @brief Second per day */
#define ZB_SECOND_PER_DAY                               (60*60*24)

/** @brief Second per minutes */
#define ZB_SECOND_PER_MINUTE                            60

/** @brief Days per week */
#define ZB_DAYS_PER_WEEK 7

/** @brief Time delta for using setpoint: skip setpoint if it expired
 * more then this value */
#define ZB_ZCL_THERMOSTAT_SETPOINT_TIME_DELTA   (10 * 60) /* 10 minutes */

/** @brief Subtract time in seconds round week */
#define ZB_WEEK_TIME_SUBTRACT(a, b) ((a) > (b)) ? ((a) - (b)) : ((a) + ZB_SECOND_PER_WEEK - (b))

// 1 day (24h)
/** @brief Maximum period of Thermostat process loop */
#define ZB_ZCL_THERMOSTAT_TIMER_MAX          (24*60*60)

/** @brief Struct with one Weekly Schedule attribute */
typedef struct zb_zcl_thermostat_weekly_schedule_record_s
{
  zb_uint8_t  day_of_week;      /*< Day of week, ZB_ZCL_THERMOSTAT_WEEKLY_FREE_RECORD (0) - free record */
  zb_uint8_t  mode_for_seq;     /*< Mode for Sequence */
  zb_uint16_t transition_time;  /*< Transition time in minutes after midnight */
  zb_uint16_t heat_setpoint;    /*< Heat Set Point */
  zb_uint16_t cool_setpoint;    /*< Cool Set Point */
}
zb_zcl_thermostat_weekly_schedule_record_t;


/*! @}
    @endcond */ /* Thermostat cluster internals */

/*! @} */ /* ZCL HA Thermostat cluster definitions */

/** @cond internals_doc */
/** @brief Struct with Context Thermostat cluster */
typedef struct zb_zcl_thermostat_context_s
{
  zb_bool_t is_hold;
  zb_uint8_t weekly_limit;
  zb_uint8_t weekly_used;
  zb_uint8_t daily_limit;
  zb_uint8_t daily_used[ZB_DAYS_PER_WEEK];
  zb_zcl_thermostat_weekly_schedule_record_t *schedule_table;
}
zb_zcl_thermostat_context_t;


/** @brief Declare run Thermostat command for User Application
*/
typedef ZB_PACKED_PRE struct zb_zcl_thermostat_value_param_s
{
  zb_uint8_t mode;              /*!< Mode for Sequence */
  zb_uint16_t heat_setpoint;    /*!< Heat Set Point */
  zb_uint16_t cool_setpoint;    /*!< Cool Set Point */
} ZB_PACKED_STRUCT zb_zcl_thermostat_value_param_t;

typedef ZB_PACKED_PRE struct zb_zcl_thermostat_weekly_schedule_table_param_s
{
  zb_zcl_thermostat_set_weekly_schedule_req_t header;
  zb_zcl_thermostat_weekly_schedule_record_t records;
} ZB_PACKED_STRUCT zb_zcl_thermostat_weekly_schedule_table_param_t;

/*! @}
 *  @endcond */ /* internals_doc */
/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_thermostat_init(void);
zb_uint8_t zb_zcl_thermostat_get_current_day_of_week(zb_uint8_t start_of_week);
zb_ret_t zb_zcl_thermostat_weekly_schedule_start(void);
zb_ret_t zb_zcl_thermostat_weekly_schedule_stop(void);
zb_ret_t zb_zcl_thermostat_weekly_schedule_get_next_record(zb_uint16_t *iterator, zb_zcl_thermostat_weekly_schedule_record_t *record);
void zb_zcl_thermostat_init_server(void);
void zb_zcl_thermostat_init_client(void);
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_SERVER_ROLE_INIT zb_zcl_thermostat_init_server
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_CLIENT_ROLE_INIT zb_zcl_thermostat_init_client

#endif /* ZB_ZCL_THERMOSTAT_H */
