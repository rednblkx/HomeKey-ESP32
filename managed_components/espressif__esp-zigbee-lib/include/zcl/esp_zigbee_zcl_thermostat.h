/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_zigbee_type.h"

/** @brief Thermostat cluster attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_ID                     = 0x0000, /*!< Local Temperature attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_OUTDOOR_TEMPERATURE_ID                   = 0x0001, /*!< Outdoor Temperature attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPANCY_ID                             = 0x0002, /*!< Occupancy attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_HEAT_SETPOINT_LIMIT_ID           = 0x0003, /*!< The AbsMinHeatSetpointLimit attribute specifies the absolute minimum level that the heating setpoint MAY be set to */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_HEAT_SETPOINT_LIMIT_ID           = 0x0004, /*!< The AbsMaxHeatSetpointLimit attribute specifies the absolute maximum level that the heating setpoint MAY be set to */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_ABS_MIN_COOL_SETPOINT_LIMIT_ID           = 0x0005, /*!< The AbsMinCoolSetpointLimit attribute specifies the absolute minimum level that the cooling setpoint MAY be set to */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_ABS_MAX_COOL_SETPOINT_LIMIT_ID           = 0x0006, /*!< The AbsMaxCoolSetpointLimit attribute specifies the absolute maximum level that the cooling setpoint MAY be set to */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_PI_COOLING_DEMAND_ID                     = 0x0007, /*!< The PICoolingDemand attribute is 8 bits in length and specifies the level of cooling demanded by the PI (proportional integral) control loop in use by the thermostat (if any), in percent */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_PI_HEATING_DEMAND_ID                     = 0x0008, /*!< The PIHeatingDemand attribute is 8 bits in length and specifies the level of heating demanded by the PI loop in percent */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_HVAC_SYSTEM_TYPE_CONFIGURATION_ID        = 0x0009, /*!< The HVACSystemTypeConfiguration attribute specifies the HVAC system type controlled by the thermostat */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_ID         = 0x0010, /*!< Local Temperature Calibration */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_ID             = 0x0011, /*!< Occupied Cooling Setpoint attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_ID             = 0x0012, /*!< Occupied Heating Setpoint attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_COOLING_SETPOINT_ID           = 0x0013, /*!< Unoccupied Cooling Setpoint attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_HEATING_SETPOINT_ID           = 0x0014, /*!< Unoccupied Heating Setpoint attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_MIN_HEAT_SETPOINT_LIMIT_ID               = 0x0015, /*!< The MinHeatSetpointLimit attribute specifies the minimum level that the heating setpoint MAY be set to */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_MAX_HEAT_SETPOINT_LIMIT_ID               = 0x0016, /*!< The MaxHeatSetpointLimit attribute specifies the maximum level that the heating setpoint MAY be set to */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_MIN_COOL_SETPOINT_LIMIT_ID               = 0x0017, /*!< The MinCoolSetpointLimit attribute specifies the minimum level that the cooling setpoint MAY be set to */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_MAX_COOL_SETPOINT_LIMIT_ID               = 0x0018, /*!< The MaxCoolSetpointLimit attribute specifies the maximum level that the cooling setpoint MAY be set to */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_ID                = 0x0019, /*!< The MinSetpointDeadBand attribute specifies the minimum difference between the Heat Setpoint and the Cool SetPoint, in steps of 0.1C */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_REMOTE_SENSING_ID                        = 0x001a, /*!< The RemoteSensing attribute is an 8-bit bitmap that specifies whether the local temperature, outdoor temperature and occupancy are being sensed by internal sensors or remote networked sensors */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_ID         = 0x001b, /*!< Control Sequence Of Operation attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_SYSTEM_MODE_ID                           = 0x001c, /*!< System Mode attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_ALARM_MASK_ID                            = 0x001d, /*!< The AlarmMask attribute specifies whether each of the alarms is enabled */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_RUNNING_MODE_ID                          = 0x001e, /*!< Thermostat Running Mode attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_START_OF_WEEK_ID                         = 0x0020, /*!< Start of Week attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_NUMBER_OF_WEEKLY_TRANSITIONS_ID          = 0x0021, /*!< NumberOfWeeklyTransitions attribute determines how many weekly schedule transitions the thermostat is capable of handling */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_NUMBER_OF_DAILY_TRANSITIONS_ID           = 0x0022, /*!< NumberOfDailyTransitions attribute determines how many daily schedule transitions the thermostat is capable of handling */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_ID             = 0x0023, /*!< TemperatureSetpointHold specifies the temperature hold status on the thermostat */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_ID    = 0x0024, /*!< TemperatureSetpointHoldDuration sets the period in minutes for which a setpoint hold is active */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_PROGRAMMING_OPERATION_MODE_ID = 0x0025, /*!< The ThermostatProgrammingOperationMode attribute determines the operational state of the thermostats programming */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_THERMOSTAT_RUNNING_STATE_ID              = 0x0029, /*!< ThermostatRunningState represents the current relay state of the heat, cool, and fan relays */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_SOURCE_ID                = 0x0030, /*!< The SetpointChangeSource attribute specifies the source of the current active OccupiedCoolingSetpoint or OccupiedHeatingSetpoint (i.e., who or what determined the current setpoint) */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_AMOUNT_ID                = 0x0031, /*!< The SetpointChangeAmount attribute specifies the delta between the current active OccupiedCoolingSetpoint or OccupiedHeatingSetpoint and the previous active setpoint */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_SETPOINT_CHANGE_SOURCE_TIMESTAMP_ID      = 0x0032, /*!< The SetpointChangeSourceTimestamp attribute specifies the time in UTC at which the SetpointChangeSourceAmount attribute change was recorded */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_ID                      = 0x0034, /*!< Specifies the degrees Celsius, in 0.1 degree increments, the Thermostat server will allow the LocalTemperature attribute to float above the OccupiedCooling setpoint or below the OccupiedHeating setpoint before initiating a state change to bring the temperature back to the users desired setpoint */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_MIN_ID                  = 0x0035, /*!< Specifies the minimum degrees Celsius, in 0.1 degree increments, the Thermostat server will allow the OccupiedSetback attribute  to be configured by a user */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_OCCUPIED_SETBACK_MAX_ID                  = 0x0036, /*!< Specifies the maximum degrees Celsius, in 0.1 degree increments, the Thermostat server will allow the OccupiedSetback attribute to be configured by a user */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_ID                    = 0x0037, /*!< Specifies the degrees Celsius, in 0.1 degree increments, the Thermostat server will allow the LocalTemperature attribute to float above the UnoccupiedCooling setpoint or below the UnoccupiedHeating setpoint before initiating a state change to bring the temperature back to the users desired setpoint */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_MIN_ID                = 0x0038, /*!< Specifies the minimum degrees Celsius, in 0.1 degree increments, the Thermostat server will allow the UnoccupiedSetback attribute to be configured by a user */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_UNOCCUPIED_SETBACK_MAX_ID                = 0x0039, /*!< Specifies the maximum degrees Celsius, in 0.1 degree increments, the Thermostat server will allow the UnoccupiedSetback attribute to be configured by a user. */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_EMERGENCY_HEAT_DELTA_ID                  = 0x003a, /*!< Specifies the delta, in 0.1 degrees Celsius, between LocalTemperature and the OccupiedHeatingSetpoint or UnoccupiedHeatingSetpoint attributes at which the Thermostat server will operate in emergency heat mode */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_AC_TYPE_ID                               = 0x0040, /*!< Indicates the type of Mini Split ACType of Mini Split AC is defined depending on how Cooling and Heating condition is achieved by Mini Split AC */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_AC_CAPACITY_ID                           = 0x0041, /*!< Indicates capacity of Mini Split AC in terms of the format defined by the ACCapacityFormat attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_AC_REFRIGERANT_TYPE_ID                   = 0x0042, /*!< Indicates type of refrigerant used within the Mini Split AC */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_AC_COMPRESSOR_TYPE_ID                    = 0x0043, /*!< This indicates type of Compressor used within the Mini Split AC */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_AC_ERROR_CODE_ID                         = 0x0044, /*!< This indicates the type of errors encountered within the Mini Split AC */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_AC_LOUVER_POSITION_ID                    = 0x0045, /*!< AC Louver position attribute */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_AC_COIL_TEMPERATURE_ID                   = 0x0046, /*!< ACCoilTemperature represents the temperature in degrees Celsius, as measured locally or remotely (over the network) */
    ESP_ZB_ZCL_ATTR_THERMOSTAT_AC_CAPACITY_FORMAT_ID                    = 0x0047, /*!< This is the format for the ACCapacity attribute */
} esp_zb_zcl_thermostat_attr_t;

/** @brief Thermostat cluster command identifiers */
typedef enum {
    ESP_ZB_ZCL_CMD_THERMOSTAT_SETPOINT_RAISE_LOWER  = 0x00, /*!< Setpoint Raise/Lower command */
    ESP_ZB_ZCL_CMD_THERMOSTAT_SET_WEEKLY_SCHEDULE   = 0x01, /*!< Set Weekly Schedule command */
    ESP_ZB_ZCL_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE   = 0x02, /*!< Get Weekly Schedule command */
    ESP_ZB_ZCL_CMD_THERMOSTAT_CLEAR_WEEKLY_SCHEDULE = 0x03, /*!< Clear Weekly Schedule command */
    ESP_ZB_ZCL_CMD_THERMOSTAT_GET_RELAY_STATUS_LOG  = 0x04  /*!< Get Relay Status Log command */
} esp_zb_zcl_thermostat_cmd_id_t;

/** @brief Thermostat cluster command identifiers */
typedef enum {
    ESP_ZB_ZCL_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE_RESP  = 0x00, /*!< Get Weekly Schedule response command */
    ESP_ZB_ZCL_CMD_THERMOSTAT_GET_RELAY_STATUS_LOG_RESP = 0x01  /*!< Get Relay Status Log response command */
} esp_zb_zcl_thermostat_cmd_resp_id_t;

/** @brief Values for Control Sequence Of Operation attribute */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_COOLING_ONLY                            = 0x00, /*!< Cooling Only value */
    ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_COOLING_WITH_REHEAT                     = 0x01, /*!< Cooling With Reheat value */
    ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_HEATING_ONLY                            = 0x02, /*!< Heating Only value */
    ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_HEATING_WITH_REHEAT                     = 0x03, /*!< Heating With Reheat value */
    ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_COOLING_AND_HEATING_4_PIPES             = 0x04, /*!< Cooling and Heating 4-pipes value */
    ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_COOLING_AND_HEATING_4_PIPES_WITH_REHEAT = 0x05, /*!< Cooling and Heating 4-pipes with Reheat value */
} esp_zb_zcl_thermostat_control_sequence_of_operation_t;

/** @brief Values for System Mode attribute */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_OFF               = 0x00, /*!< Off value */
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_AUTO              = 0x01, /*!< Auto value */
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_COOL              = 0x03, /*!< Cool value */
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_HEAT              = 0x04, /*!< Heat value */
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_EMERGENCY_HEATING = 0x05, /*!< Emergency Heating value */
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_PRECOOLING        = 0x06, /*!< Precooling value */
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_FAN_ONLY          = 0x07, /*!< Fan Only value */
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_DRY               = 0x08, /*!< Dry value */
    ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_SLEEP             = 0x09, /*!< Sleep value */
} esp_zb_zcl_thermostat_system_mode_t;


/** @brief Values for AC Louver position attribute */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_LOUVER_FULLY_CLOSED        = 0x01, /*!< Fully close */
    ESP_ZB_ZCL_THERMOSTAT_LOUVER_FULLY_OPEN          = 0x02, /*!< Fully open */
    ESP_ZB_ZCL_THERMOSTAT_LOUVER_QUARTER_OPEN        = 0x03, /*!< Qyarter open */
    ESP_ZB_ZCL_THERMOSTAT_LOUVER_HALF_OPEN           = 0x04, /*!< Half open */
    ESP_ZB_ZCL_THERMOSTAT_LOUVER_THREE_QUARTERS_OPEN = 0x05, /*!< Three quaters open */
} esp_zb_zcl_thermostat_ac_louver_position_t;

/** @brief Values for Start of Week attribute */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_START_OF_WEEK_SUNDAY    = 0x00, /*!< Sunday value */
    ESP_ZB_ZCL_THERMOSTAT_START_OF_WEEK_MONDAY    = 0x01, /*!< Monday value */
    ESP_ZB_ZCL_THERMOSTAT_START_OF_WEEK_TUESDAY   = 0x02, /*!< Tuesday value */
    ESP_ZB_ZCL_THERMOSTAT_START_OF_WEEK_WEDNESDAY = 0x03, /*!< Wednesday value */
    ESP_ZB_ZCL_THERMOSTAT_START_OF_WEEK_THURSDAY  = 0x04, /*!< Thursday value */
    ESP_ZB_ZCL_THERMOSTAT_START_OF_WEEK_FRIDAY    = 0x05, /*!< Friday value */
    ESP_ZB_ZCL_THERMOSTAT_START_OF_WEEK_SATURDAY  = 0x06, /*!< Saturday value */
} esp_zb_zcl_thermostat_start_of_week_t;

/** @brief Values for Temperature Setpoint Hold attribute */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_OFF = 0x00, /*!< Off value */
    ESP_ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_ON  = 0x01, /*!< On value */
} esp_zb_zcl_thermostat_temperature_setpoint_hold_t;

/** @brief Values for AlarmMask attribute */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_ALARM_CODE_INITIALIZATION_FAILURE  = 0x00, /*!< MainsVoltageAlarmCode - MainsVoltageMinThreshold reached for Mains Voltage**/
    ESP_ZB_ZCL_THERMOSTAT_ALARM_CODE_HARDWARE_FAILURE        = 0x01, /*!< MainsVoltageAlarmCode - MainsVoltageMaxThreshold reached for Mains Voltage**/
    ESP_ZB_ZCL_THERMOSTAT_ALARM_CODE_SELFCALIBRATION_FAILURE = 0x02, /*!< BatteryAlarmCode - BatteryVoltageMinThreshold or BatteryPercentageMinThreshold reached for Battery Source 1 */
} esp_zb_zcl_thermostat_alarm_code_t;

/** @brief Bits of ThermostatProgrammingOperationMode */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_SCHEDULE_PROGRAMMING_MODE_BIT             = 0x01, /*!< Schedule programming bit */
    ESP_ZB_ZCL_THERMOSTAT_AUTO_RECOVERY_PROGRAMMING_MODE_BIT        = 0x02, /*!< Auto/recovery bit  */
    ESP_ZB_ZCL_THERMOSTAT_ECONOMY_ENERGY_START_PROGRAMMING_MODE_BIT = 0x04, /*!< Economy/energy star bit */
} esp_zb_zcl_thermostat_programming_operation_mode_bit_t;

/** @brief Values for Mode field for SetpointRaiseLower*/
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_HEAT = 0x00, /*!< Heat value */
    ESP_ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_COOL = 0x01, /*!< Cool value */
    ESP_ZB_ZCL_THERMOSTAT_SETPOINT_RAISE_LOWER_MODE_BOTH = 0x02, /*!< Both (Heat and Cool) value */
} esp_zb_zcl_thermostat_setpoint_raise_lower_mode_t;

/** @brief Values for Mode for Sequence field */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_HEAT = 0x01, /*!< Heat value */
    ESP_ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_COOL = 0x02, /*!< Cool value */
    ESP_ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_MODE_FOR_SEQ_BOTH = 0x03, /*!< Both (Heat and Cool) value */
} esp_zb_zcl_thermostat_weekly_schedule_mode_for_seq_t;

/** @brief Value for Day of Week */
typedef enum {
    ESP_ZB_ZCL_THERMOSTAT_DAY_OF_WEEK_SUNDAY    = (1 << 0), /*!< SUNDAY */
    ESP_ZB_ZCL_THERMOSTAT_DAY_OF_WEEK_MONDAY    = (1 << 1), /*!< MONDAY */
    ESP_ZB_ZCL_THERMOSTAT_DAY_OF_WEEK_TUESDAY   = (1 << 2), /*!< TUESDAY */
    ESP_ZB_ZCL_THERMOSTAT_DAY_OF_WEEK_WEDNESDAY = (1 << 3), /*!< WEDNESDAY */
    ESP_ZB_ZCL_THERMOSTAT_DAY_OF_WEEK_THURSDAY  = (1 << 4), /*!< THURSDAY */
    ESP_ZB_ZCL_THERMOSTAT_DAY_OF_WEEK_FRIDAY    = (1 << 5), /*!< FRIDAY */
    ESP_ZB_ZCL_THERMOSTAT_DAY_OF_WEEK_SATURDAY  = (1 << 6), /*!< SATURDAY */
    ESP_ZB_ZCL_THERMOSTAT_DAY_OF_WEEK_VACATION  = (1 << 7), /*!< VACATION */
} esp_zb_zcl_thermostat_day_of_week_t;

/* Default value for Start Of Week attribute */
#define ESP_ZB_ZCL_THERMOSTAT_START_OF_WEEK_DEFAULT_VALUE 0x00

/** @brief Min value for Local Temperature attribute */
#define ESP_ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_MIN_VALUE 0x954d

/** @brief Max value for Local Temperature attribute */
#define ESP_ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_MAX_VALUE 0x7fff

/** @brief Min value for Outdoor Temperature attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OUTDOOR_TEMPERATURE_MIN_VALUE 0x954d

/** @brief Max value for Outdoor Temperature attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OUTDOOR_TEMPERATURE_MAX_VALUE 0x7fff

/** @brief Invalid value for Local Temperature attribute */
#define ESP_ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_INVALID ((zb_int16_t)0x8000)

/** @brief Default value for Local Temperature attribute */
#define ESP_ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_DEFAULT_VALUE 0xffff

/** @brief Min value for Local Temperature Calibration attribute */
#define ESP_ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_MIN_VALUE 0xe7

/** @brief Max value for Local Temperature Calibration attribute */
#define ESP_ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_MAX_VALUE 0x19

/** @brief Default value for Occupied Cooling Setpoint attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_DEFAULT_VALUE 0x0a28

/** @brief Min value for Occupied Cooling Setpoint attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_MIN_VALUE 0x954d

/** @brief Max value for Occupied Cooling Setpoint attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_COOLING_SETPOINT_MAX_VALUE 0x7fff

/** @brief Default value for Occupied Heating Setpoint attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_DEFAULT_VALUE 0x07d0

/** @brief Min value for Occupied Heating Setpoint attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_MIN_VALUE 0x954d

/** @brief Max value for Occupied Heating Setpoint attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_HEATING_SETPOINT_MAX_VALUE 0x7fff

/** @brief Min value for Min Setpoint Dead Band attribute */
#define ESP_ZB_ZCL_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_MIN_VALUE 0x0a

/** @brief Max value for Min Setpoint Dead Band attribute */
#define ESP_ZB_ZCL_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_MAX_VALUE 0x19

/** @brief Local Temperature Sensed Remotely bit of Remote Sensing attribute */
#define ESP_ZB_ZCL_THERMOSTAT_REMOTE_SENSING_LOCAL_TEMPERATURE_SENSED_REMOTELY_BIT (1 << 0)

/** @brief Outdoor Temperature Sensed Remotely bit of Remote Sensing attribute */
#define ESP_ZB_ZCL_THERMOSTAT_REMOTE_SENSING_OUTDOOR_TEMPERATURE_SENSED_REMOTELY_BIT (1 << 1)

/** @brief Occupancy Sensed Sensed Remotely bit of Remote Sensing attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_REMOTE_SENSING_OCCUPANCY_SENSED_REMOTELY_BIT (1 << 2)

/** @brief Default value for Control Sequence Of Operation attribute */
#define ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQ_OF_OPERATION_DEFAULT_VALUE 0x04

/** @brief Default value for System Mode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_CONTROL_SYSTEM_MODE_DEFAULT_VALUE 0x01

/** @brief Default value for Occupancy attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPANCY_DEFAULT_VALUE ((zb_uint8_t)1U)

/** @brief Bitmask for Occupancy attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPANCY_BITMASK ((zb_uint8_t)0x01)

/** @brief Default value for AbsMinHeatSetpointLimit attribute */
#define ESP_ZB_ZCL_THERMOSTAT_ABS_MIN_HEAT_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x02BC)

/** @brief Default value for AbsMaxHeatSetpointLimit attribute */
#define ESP_ZB_ZCL_THERMOSTAT_ABS_MAX_HEAT_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0BB8)

/** @brief Default value for AbsMinCoolSetpointLimit attribute */
#define ESP_ZB_ZCL_THERMOSTAT_ABS_MIN_COOL_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0640)

/** @brief Default value for AbsMaxCoolSetpointLimit attribute */
#define ESP_ZB_ZCL_THERMOSTAT_ABS_MAX_COOL_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0C80)

/** @brief Min value for PICoolingDemand attribute */
#define ESP_ZB_ZCL_THERMOSTAT_PI_COOLING_DEMAND_MIN_VALUE ((zb_uint8_t)0x00)

/** @brief Max value for PICoolingDemand attribute */
#define ESP_ZB_ZCL_THERMOSTAT_PI_COOLING_DEMAND_MAX_VALUE ((zb_uint8_t)0x64)

/** @brief Min value for PIHeatingDemand attribute */
#define ESP_ZB_ZCL_THERMOSTAT_PI_HEATING_DEMAND_MIN_VALUE ((zb_uint8_t)0x00)

/** @brief Max value for PIHeatingDemand attribute */
#define ESP_ZB_ZCL_THERMOSTAT_PI_HEATING_DEMAND_MAX_VALUE ((zb_uint8_t)0x64)

/** @brief Default value for HVACSystemTypeConfiguration attribute */
#define ESP_ZB_ZCL_THERMOSTAT_HVAC_SYSTEM_TYPE_CONFIGURATION_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for LocalTemperatureCalibration attribute */
#define ESP_ZB_ZCL_THERMOSTAT_LOCAL_TEMPERATURE_CALIBRATION_DEFAULT_VALUE ((zb_int8_t)0x00)

/** @brief Default value for UnoccupiedCoolingSetpoint attribute */
#define ESP_ZB_ZCL_THERMOSTAT_UNOCCUPIED_COOLING_SETPOINT_DEFAULT_VALUE ((zb_int16_t)0x0A28)

/** @brief Default value for UnoccupiedHeatingSetpoint attribute */
#define ESP_ZB_ZCL_THERMOSTAT_UNOCCUPIED_HEATING_SETPOINT_DEFAULT_VALUE ((zb_int16_t)0x07D0)

/** @brief Default value for MinHeatSetpointLimit attribute */
#define ESP_ZB_ZCL_THERMOSTAT_MIN_HEAT_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x02BC)

/** @brief Default value for MaxHeatSetpointLimit attribute */
#define ESP_ZB_ZCL_THERMOSTAT_MAX_HEAT_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0BB8)

/** @brief Default value for MinCoolSetpointLimit attribute */
#define ESP_ZB_ZCL_THERMOSTAT_MIN_COOL_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0640)

/** @brief Default value for MaxCoolSetpointLimit attribute */
#define ESP_ZB_ZCL_THERMOSTAT_MAX_COOL_SETPOINT_LIMIT_DEFAULT_VALUE ((zb_int16_t)0x0C80)

/** @brief Default value for MinSetpointDeadBand attribute */
#define ESP_ZB_ZCL_THERMOSTAT_MIN_SETPOINT_DEAD_BAND_DEFAULT_VALUE ((zb_int8_t)0x19)

/** @brief Default value for RemoteSensing attribute */
#define ESP_ZB_ZCL_THERMOSTAT_REMOTE_SENSING_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for ControlSequenceOfOperation attribute */
#define ESP_ZB_ZCL_THERMOSTAT_CONTROL_SEQUENCE_OF_OPERATION_DEFAULT_VALUE ((zb_uint8_t)0x04)

/** @brief Default value for SystemMode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_SYSTEM_MODE_DEFAULT_VALUE ((zb_uint8_t)0x01)

/** @brief Default value for AlarmMask attribute */
#define ESP_ZB_ZCL_THERMOSTAT_ALARM_MASK_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for RunningMode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNING_MODE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Off value for RunningMode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNING_MODE_OFF_VALUE ((zb_uint8_t)0x00)

/** @brief Cool value for RunningMode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNING_MODE_COOL_VALUE ((zb_uint8_t)0x03)

/** @brief Heat value for RunningMode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNING_MODE_HEAT_VALUE ((zb_uint8_t)0x04)

/** @brief Default value for TemperatureSetpointHold attribute */
#define ESP_ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for TemperatureSetpointHoldDuration attribute */
#define ESP_ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Min value for TemperatureSetpointHoldDuration attribute */
#define ESP_ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_MIN_VALUE ((zb_uint16_t)0x0000)

/** @brief Max value for TemperatureSetpointHoldDuration attribute */
#define ESP_ZB_ZCL_THERMOSTAT_TEMPERATURE_SETPOINT_HOLD_DURATION_MAX_VALUE ((zb_uint16_t)0x05a0)

/** @brief Default value for ThermostatProgrammingOperationMode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_THERMOSTAT_PROGRAMMING_OPERATION_MODE_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for SetpointChangeSource attribute */
#define ESP_ZB_ZCL_THERMOSTAT_SETPOINT_CHANGE_SOURCE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for SetpointChangeAmount attribute */
#define ESP_ZB_ZCL_THERMOSTAT_SETPOINT_CHANGE_AMOUNT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for SetpointChangeSourceTimestamp attribute */
#define ESP_ZB_ZCL_THERMOSTAT_SETPOINT_CHANGE_SOURCE_TIMESTAMP_DEFAULT_VALUE ((zb_time_t)0x00000000)

/** @brief Default value for OccupiedSetback attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_SETBACK_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for OccupiedSetbackMin attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_SETBACK_MIN_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for OccupiedSetbackMax attribute */
#define ESP_ZB_ZCL_THERMOSTAT_OCCUPIED_SETBACK_MAX_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for UnoccupiedSetback attribute */
#define ESP_ZB_ZCL_THERMOSTAT_UNOCCUPIED_SETBACK_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for UnoccupiedSetbackMin attribute */
#define ESP_ZB_ZCL_THERMOSTAT_UNOCCUPIED_SETBACK_MIN_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for UnoccupiedSetbackMax attribute */
#define ESP_ZB_ZCL_THERMOSTAT_UNOCCUPIED_SETBACK_MAX_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for EmergencyHeatDelta attribute */
#define ESP_ZB_ZCL_THERMOSTAT_EMERGENCY_HEAT_DELTA_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for ACType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_TYPE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Unknown value for ACType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_TYPE_UNKNOWN_VALUE ESP_ZB_ZCL_THERMOSTAT_AC_TYPE_DEFAULT_VALUE

/** @brief Cooling and Fixed Speed value for ACType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_TYPE_COOLING_AND_FIXED_SPEED_VALUE ((zb_uint8_t)0x01)

/** @brief Heat Pump and Fixed Speed value for ACType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_TYPE_HEAT_PUMP_AND_FIXED_SPEED_VALUE ((zb_uint8_t)0x02)

/** @brief Cooling and Inverter value for ACType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_TYPE_COOLING_AND_INVERTER_VALUE ((zb_uint8_t)0x03)

/** @brief Heat Pump and Inverter value value for ACType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_TYPE_HEAT_PUMP_AND_INVERTER_VALUE ((zb_uint8_t)0x04)

/** @brief Default value for ACCapacity attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_CAPACITY_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for ACRefrigerantType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Unknown value for ACRefrigerantType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_UNKNOWN_VALUE ESP_ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_DEFAULT_VALUE

/** @brief R22 value for ACRefrigerantType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_R22_VALUE ((zb_uint8_t)0x01)

/** @brief R410a value for ACRefrigerantType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_R410A_VALUE ((zb_uint8_t)0x02)

/** @brief R407c value for ACRefrigerantType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_REFRIGERANT_TYPE_R407C_VALUE ((zb_uint8_t)0x03)

/** @brief Default value for ACCompressorType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Unknown value for ACCompressorType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_UNKNOWN_VALUE ESP_ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_DEFAULT_VALUE

/** @brief T1(Max working ambient 43 ºC) value for ACCompressorType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_T1_VALUE ((zb_uint8_t)0x01)

/** @brief T2(Max working ambient 35 ºC) value for ACCompressorType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_T2_VALUE ((zb_uint8_t)0x02)

/** @brief T3(Max working ambient 52 ºC) value for ACCompressorType attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_COMPRESSOR_TYPE_T3_VALUE ((zb_uint8_t)0x03)

/** @brief Default value for ACErrorCode attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_DEFAULT_VALUE ((zb_uint32_t)0x00000000)

/** @brief Compressor Failure or Refrigerant Leakage bit of AC Error Code attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_COMPRESSOR_FAILURE_OR_REFRIGERANT_LEAKAGE_BIT (1 << 0)

/** @brief Room Temperature Sensor Failure bit of AC Error Code attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_ROOM_TEMPERATURE_SENSOR_FAILURE_BIT (1 << 1)

/** @brief Outdoor Temperature Sensor Failure bit of AC Error Code attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_OUTDOOR_TEMPERATURE_SENSOR_FAILURE_BIT (1 << 2)

/** @brief Indoor Coil Temperature Sensor Failure bit of AC Error Code attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_INDOOR_COIL_TEMPERATURE_SENSOR_FAILURE_BIT (1 << 3)

/** @brief Fan Failure bit of AC Error Code attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_AC_ERROR_CODE_FAN_FAILURE_BIT (1 << 4)

/** @brief Default value for ACLouverPosition attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Fully Closed value for ACLouverPosition attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_FULLY_CLOSED_VALUE ((zb_uint8_t)0x01)

/** @brief Fully Open value for ACLouverPosition attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_FULLY_OPEN_VALUE ((zb_uint8_t)0x02)

/** @brief Quarter Open value for ACLouverPosition attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_QUARTER_OPEN_VALUE ((zb_uint8_t)0x03)

/** @brief Three Quarters Open value for ACLouverPosition attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_THREE_QUARTERS_OPEN_VALUE ((zb_uint8_t)0x05)

/** @brief Half Open value for ACLouverPosition attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_LOUVER_POSITION_HALF_OPEN_VALUE ((zb_uint8_t)0x04)

/** @brief Min value for AC Coil Temperature attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_COIL_TEMPERATURE_MIN_VALUE 0x954d

/** @brief Max value for AC Coil Temperature attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_COIL_TEMPERATURE_MAX_VALUE 0x7fff

/** @brief Default value for AC Capacity Format attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_CAPACITY_FORMAT_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief BTUh value for AC Capacity Format attribute */
#define ESP_ZB_ZCL_THERMOSTAT_AC_CAPACITY_FORMAT_BTUH_VALUE ESP_ZB_ZCL_THERMOSTAT_AC_CAPACITY_FORMAT_DEFAULT_VALUE

/** @brief Initialization failure bit of Alarm Mask attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_ALARM_MASK_INITIALIZATION_FAILURE_BIT (1 << 0)

/** @brief Hardware Failure bit of Alarm Mask attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_ALARM_MASK_HARDWARE_FAILURE_BIT (1 << 1)

/** @brief Self-Calibration Failure bit of Alarm Mask attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_ALARM_MASK_SELFCALIBRATION_FAILURE_BIT (1 << 2)

/** @brief Simple/Setpoint(0) or Schedule_Programming(1) mode bit of Thermostat Programming Operation Mode attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_PROGRAMMING_OPERATION_MODE_SIMPLE_SETPOINT_OR_PROGRAMMING_MODE_BIT (1 << 0)

/** @brief Auto/recovery mode bit of Thermostat Programming Operation Mode attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_PROGRAMMING_OPERATION_MODE_AUTO_RECOVERY_MODE_BIT (1 << 1)

/** @brief Economy/EnergyStar mode bit of Thermostat Programming Operation Mode attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_PROGRAMMING_OPERATION_MODE_ECONOMY_ENERGY_STAR_MODE_BIT (1 << 2)

/** @brief Heat State On bit of Thermostat Running State attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNNING_STATE_HEAT_STATE_ON_BIT (1 << 0)

/** @brief Cool State On bit of Thermostat Running State attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNNING_STATE_COOL_STATE_ON_BIT (1 << 1)

/** @brief Fan State On bit of Thermostat Running State attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNNING_STATE_FAN_STATE_ON_BIT (1 << 2)

/** @brief Heat 2nd Stage State On bit of Thermostat Running State attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNNING_STATE_HEAT_2ND_STAGE_STATE_ON_BIT (1 << 3)

/** @brief Cool 2nd Stage State On bit of Thermostat Running State attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNNING_STATE_COOL_2ND_STAGE_STATE_ON_BIT (1 << 4)

/** @brief Fan 2nd Stage State On bit of Thermostat Running State attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNNING_STATE_FAN_2ND_STAGE_STATE_ON_BIT (1 << 5)

/** @brief Fan 3rd Stage State On bit of Thermostat Running State attribute  */
#define ESP_ZB_ZCL_THERMOSTAT_RUNNNING_STATE_FAN_3RD_STAGE_STATE_ON_BIT (1 << 6)

/**
 * @brief Structure for Thermostat Weekly Schedule Transition field
 *
 */
typedef struct esp_zb_zcl_thermostat_weekly_schedule_transition_s {
    uint16_t transition_time;           /*!< This field represents the start time of the schedule transition during the associated day. */
    uint16_t heat_setpoint;             /*!< This field represents the heat setpoint to be applied at this associated transition start time. */
    uint16_t cool_setpoint;             /*!< this field represents the cool setpoint to be applied at this associated transition start time.*/
} esp_zb_zcl_thermostat_weekly_schedule_transition_t;

/**
 * @brief Structure for the record in Thermostat Weekly Schedule Table
 */
typedef struct esp_zb_zcl_thermostat_weekly_schedule_record_s {
    uint8_t  day_of_week;      /*!< Day of week, refer to esp_zb_zcl_thermostat_day_of_week_t */
    uint8_t  mode_for_seq;     /*!< Mode for Sequence, refer to esp_zb_zcl_thermostat_weekly_schedule_mode_for_seq_t */
    uint16_t transition_time;  /*!< Transition time in minutes after midnight */
    uint16_t heat_setpoint;    /*!< Heat Set Point */
    uint16_t cool_setpoint;    /*!< Cool Set Point */
} esp_zb_zcl_thermostat_weekly_schedule_record_t;

/**
 * @brief Start thermostat weekly schedule
 *
 * @return
 *      - ESP_OK: On success, otherwise, failure.
 */
esp_err_t esp_zb_zcl_thermostat_weekly_schedule_start(void);

/**
 * @brief Stop thermostat weekly schedule
 *
 * @return
 *      - ESP_OK: On success, otherwise, failure.
 */
esp_err_t esp_zb_zcl_thermostat_weekly_schedule_stop(void);

/**
 * @brief Get the next scheduled record from the thermostat weekly schedule table
 *
 * @param[in] iterator An iterator used to iterate through the thermostat weekly schedule table
 * @param[out] record The next record in the thermostat weekly schedule table, refer to esp_zb_zcl_thermostat_weekly_schedule_record_t
 * @return
 *      - ESP_OK: On success
 *      - ESP_ERR_INVALID_ARG: Invalid arguments
 *      - ESP_ERR_NOT_FOUND: End of the table reached
 *      - Otherwise: Failure
 */
esp_err_t esp_zb_zcl_thermostat_weekly_schedule_get_next_record(uint16_t *iterator, esp_zb_zcl_thermostat_weekly_schedule_record_t *record);

#ifdef __cplusplus
}
#endif
