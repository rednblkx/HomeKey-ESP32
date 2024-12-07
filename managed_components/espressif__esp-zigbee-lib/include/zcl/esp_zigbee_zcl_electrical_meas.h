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

/** @brief Electrical Measurement cluster attribute identifiers
*/
typedef enum {
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID                          = 0x0000,     /*!< This attribute indicates a device s measurement capabilities. */
  /* DC Measurement */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_ID                                = 0x0100,     /*!< The DCVoltage attribute represents the most recent DC voltage reading in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MIN_ID                            = 0x0101,     /*!< The DCVoltageMin attribute represents the lowest DC voltage value measured in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MAX_ID                            = 0x0102,     /*!< The DCVoltageMax attribute represents the highest DC voltage value measured in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_ID                                = 0x0103,     /*!< The DCCurrent attribute represents the most recent DC current reading in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_MIN_ID                            = 0x0104,     /*!< The DCCurrentMin attribute represents the lowest DC current value measured in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_MAX_ID                            = 0x0105,     /*!< The DCCurrentMax attribute represents the highest DC current value measured in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DCPOWER_ID                                   = 0x0106,     /*!< The @e DCPower attribute represents the most recent DC power reading in @e Watts (W) */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_POWER_MIN_ID                              = 0x0107,     /*!< The DCPowerMin attribute represents the lowest DC power value measured in Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_POWER_MAX_ID                              = 0x0108,     /*!< The DCPowerMax attribute represents the highest DC power value measured in Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MULTIPLIER_ID                     = 0x0200,     /*!< The DCVoltageMultiplier provides a value to be multiplied against the DCVoltage, DCVoltageMin, and DCVoltageMax attributes. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_DIVISOR_ID                        = 0x0201,     /*!< The DCVoltageDivisor provides a value to be divided against the DCVoltage, DCVoltageMin, and DCVoltageMax attributes. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_MULTIPLIER_ID                     = 0x0202,     /*!< The DCCurrentMultiplier provides a value to be multiplied against the DCCurrent, DCCurrentMin, and DCCurrentMax attributes. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_DIVISOR_ID                        = 0x0203,     /*!< The DCCurrentDivisor provides a value to be divided against the DCCurrent, DCCurrentMin, and DCCurrentMax attributes. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_POWER_MULTIPLIER_ID                       = 0x0204,     /*!< The DCPowerMultiplier provides a value to be multiplied against the DCPower, DCPowerMin, and DCPowerMax attributes. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_POWER_DIVISOR_ID                          = 0x0205,     /*!< The DCPowerDivisor provides a value to be divided against the DCPower, DCPowerMin, and DCPowerMax attributes. */
  /* AC Measurement (Non Phase) */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_ID                              = 0x0300,     /*!< The ACFrequency attribute represents the most recent AC Frequency reading in Hertz (Hz). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MIN_ID                          = 0x0301,     /*!< The ACFrequencyMin attribute represents the lowest AC Frequency value measured in Hertz (Hz). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MAX_ID                          = 0x0302,     /*!< The ACFrequencyMax attribute represents the highest AC Frequency value measured in Hertz (Hz). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_NEUTRAL_CURRENT_ID                           = 0x0303,     /*!< The NeutralCurrent attribute represents the AC neutral (Line-Out) current value at the moment in time the attribute is read, in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_ACTIVE_POWER_ID                        = 0x0304,     /*!< Active power represents the current demand of active power delivered or received at the premises, in @e kW */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_REACTIVE_POWER_ID                      = 0x0305,     /*!< Reactive power represents the current demand of reactive power delivered or received at the premises, in kVAr. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_APPARENT_POWER_ID                      = 0x0306,     /*!< Represents the current demand of apparent power, in @e kVA */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED1ST_HARMONIC_CURRENT_ID              = 0x0307,     /*!< Attribute represent the most recent 1st harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED3RD_HARMONIC_CURRENT_ID              = 0x0308,     /*!< Attribute represent the most recent 3rd harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED5TH_HARMONIC_CURRENT_ID              = 0x0309,     /*!< Attribute represent the most recent 5th harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED7TH_HARMONIC_CURRENT_ID              = 0x030a,     /*!< Attribute represent the most recent 7th harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED9TH_HARMONIC_CURRENT_ID              = 0x030b,     /*!< Attribute represent the most recent 9th harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED11TH_HARMONIC_CURRENT_ID             = 0x030c,     /*!< Attribute represent the most recent 11th harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE1ST_HARMONIC_CURRENT_ID        = 0x030d,     /*!< Attribute represent the most recent phase of the 1st harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE3RD_HARMONIC_CURRENT_ID        = 0x030e,     /*!< Attribute represent the most recent phase of the 3rd harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE5TH_HARMONIC_CURRENT_ID        = 0x030f,     /*!< Attribute represent the most recent phase of the 5th harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE7TH_HARMONIC_CURRENT_ID        = 0x0310,     /*!< Attribute represent the most recent phase of the 7th harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE9TH_HARMONIC_CURRENT_ID        = 0x0311,     /*!< Attribute represent the most recent phase of the 9th harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE11TH_HARMONIC_CURRENT_ID       = 0x0312,     /*!< Attribute represent the most recent phase of the 11th harmonic current reading in an AC frequency. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MULTIPLIER_ID                   = 0x0400,     /*!< Provides a value to be multiplied against the ACFrequency attribute. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_DIVISOR_ID                      = 0x0401,     /*!< Provides a value to be divided against the ACFrequency attribute. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_MULTIPLIER_ID                          = 0x0402,     /*!< Provides a value to be multiplied against a raw or uncompensated sensor count of power being measured by the metering device. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_DIVISOR_ID                             = 0x0403,     /*!< Provides a value to divide against the results of applying the @e Multiplier attribute against a raw or uncompensated sensor count of power being measured by the metering device. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_HARMONIC_CURRENT_MULTIPLIER_ID               = 0x0404,     /*!< Represents the unit value for the MeasuredNthHarmonicCurrent attribute in the format MeasuredNthHarmonicCurrent * 10 ^ HarmonicCurrentMultiplier amperes. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_PHASE_HARMONIC_CURRENT_MULTIPLIER_ID         = 0x0405,     /*!< Represents the unit value for the MeasuredPhaseNthHarmonicCurrent attribute in the format MeasuredPhaseNthHarmonicCurrent * 10 ^ PhaseHarmonicCurrentMultiplier degrees. */
  /* AC Measurement (Phase A)*/
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_LINE_CURRENT_ID                              = 0x0501,     /*!< Represents the single phase or Phase A, AC line current (Square root of active and reactive current) value at the moment in time the attribute is read, in @e Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_ID                            = 0x0502,     /*!< Represents the single phase or Phase A, AC active/resistive current value at the moment in time the attribute is read, in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_ID                          = 0x0503,     /*!< Represents the single phase or Phase A, AC reactive current value at the moment in time the attribute is read, in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID                                = 0x0505,     /*!< Represents the most recent RMS voltage reading in @e Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_ID                           = 0x0506,     /*!< Represents the lowest RMS voltage value measured in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_ID                           = 0x0507,     /*!< Represents the highest RMS voltage value measured in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID                                = 0x0508,     /*!< Represents the most recent RMS current reading in @e Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_ID                           = 0x0509,     /*!< Represents the lowest RMS current value measured in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_ID                           = 0x050a,     /*!< Represents the highest RMS current value measured in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID                              = 0x050B,     /*!< Represents the single phase or Phase A, current demand of active power delivered or received at the premises, in @e Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_ID                          = 0x050c,     /*!< Represents the lowest AC power value measured in Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_ID                          = 0x050d,     /*!< Represents the highest AC power value measured in Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_ID                            = 0x050e,     /*!< Represents the single phase or Phase A, current demand of reactive power delivered or received at the premises, in VAr. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_ID                            = 0x050F,     /*!< Represents the single phase or Phase A, current demand of apparent (Square root of active and reactive power) power, in @e VA. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_ID                              = 0x0510,     /*!< Contains the single phase or PhaseA, Power Factor ratio in 1/100th. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_ID     = 0x0511,     /*!< The Period in seconds that the RMS voltage is averaged over. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_ID          = 0x0512,     /*!< The number of times the average RMS voltage, has been above the AverageRMS OverVoltage threshold since last reset. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_ID         = 0x0513,     /*!< The number of times the average RMS voltage, has been below the AverageRMS underVoltage threshold since last reset. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_ID           = 0x0514,     /*!< The duration in seconds used to measure an extreme over voltage condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_ID          = 0x0515,     /*!< The duration in seconds used to measure an extreme under voltage condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_ID                    = 0x0516,     /*!< The duration in seconds used to measure a voltage sag condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_ID                  = 0x0517,     /*!< The duration in seconds used to measure a voltage swell condition. */
    /* AC Formatting */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_ID                      = 0x0600,     /*!< Provides a value to be multiplied against the @e InstantaneousVoltage and RMSVoltage attributes. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_ID                         = 0x0601,     /*!< Provides a value to be divided against the @e InstantaneousVoltage */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_ID                      = 0x0602,     /*!< Provides a value to be multiplied against the @e InstantaneousCurrent and @e RMSCurrent attributes */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID                         = 0x0603,     /*!< Provides a value to be divided against the @e ACCurrent, @e InstantaneousCurrent and @e RMSCurrent attributes. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID                        = 0x0604,     /*!< Provides a value to be multiplied against the @e InstantaneousPower and @e ActivePower attributes */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID                           = 0x0605,     /*!< Provides a value to be divided against the @e InstantaneousPower and @e ActivePower attributes. */
  /* DC Manufacturer Threshold Alarms */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_OVERLOAD_ALARMS_MASK_ID                   = 0x0700,     /*!< Specifies which configurable alarms may be generated. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_OVERLOAD_ID                       = 0x0701,     /*!< Specifies the alarm threshold, set by the manufacturer, for the maximum output voltage supported by device. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_OVERLOAD_ID                       = 0x0702,     /*!< Specifies the alarm threshold, set by the manufacturer, for the maximum output current supported by device. */
  /* AC Manufacturer Threshold Alarms */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_ALARMS_MASK_ID                            = 0x0800,     /*!< Specifies which configurable alarms may be generated. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_VOLTAGE_OVERLOAD_ID                       = 0x0801,     /*!< Specifies the alarm threshold, set by the manufacturer, for the maximum output voltage supported by device. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_CURRENT_OVERLOAD_ID                       = 0x0802,     /*!< Specifies the alarm threshold, set by the manufacturer, for the maximum output current supported by device. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_ACTIVE_POWER_OVERLOAD_ID                  = 0x0803,     /*!< Specifies the alarm threshold, set by the manufacturer, for the maximum output active power supported by device. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_REACTIVE_POWER_OVERLOAD_ID                = 0x0804,     /*!< Specifies the alarm threshold, set by the manufacturer, for the maximum output reactive power supported by device. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_ID                  = 0x0805,     /*!< The average RMS voltage above which an over voltage condition is reported. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_ID                 = 0x0806,     /*!< The average RMS voltage below which an under voltage condition is reported. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_ID                  = 0x0807,     /*!< The RMS voltage above which an extreme under voltage condition is reported. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_ID                 = 0x0808,     /*!< The RMS voltage below which an extreme under voltage condition is reported. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_ID                           = 0x0809,     /*!< The RMS voltage below which a sag condition is reported. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_ID                         = 0x080a,     /*!< The RMS voltage above which a swell condition is reported. */
  /* AC Measurement (Phase B)*/
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_LINE_CURRENT_PH_B_ID                             = 0x0901,     /*!< Represents the Phase B, AC line current (Square root sum of active and reactive currents) value at the moment in time the attribute is read, in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_PH_B_ID                           = 0x0902,     /*!< Represents the Phase B, AC active/resistive current value at the moment in time */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_PH_B_ID                         = 0x0903,     /*!< Represents the Phase B, AC reactive current value at the moment in time the */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHB_ID                                = 0x0905,     /*!< Represents the most recent RMS voltage reading in @e Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_PH_B_ID                          = 0x0906,     /*!< Represents the lowest RMS voltage value measured in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_PH_B_ID                          = 0x0907,     /*!< Represents the highest RMS voltage value measured in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHB_ID                                = 0x0908,     /*!< Represents the most recent RMS current reading in @e Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_PH_B_ID                          = 0x0909,     /*!< Represents the lowest RMS current value measured in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_PH_B_ID                          = 0x090a,     /*!< Represents the highest RMS current value measured in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHB_ID                              = 0x090B,     /*!< Represents the Phase B, current demand of active power delivered or received at the premises, in @e Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_PH_B_ID                         = 0x090c,     /*!< Represents the lowest AC power value measured in Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_PH_B_ID                         = 0x090d,     /*!< Represents the highest AC power value measured in Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_PH_B_ID                           = 0x090e,     /*!< Represents the Phase B, current demand of reactive power delivered or received at the premises, in VAr. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHB_ID                            = 0x090F,     /*!< Represents the Phase B, current demand of apparent (Square root of active and reactive power) power, in @e VA. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_PH_B_ID                             = 0x0910,     /*!< Contains the PhaseB, Power Factor ratio in 1/100th. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_ID     = 0x0911,     /*!< The number of times the average RMS voltage, has been above the @e AverageRMS @e OverVoltage threshold since last reset. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_PH_B_ID         = 0x0912,     /*!< The number of times the average RMS voltage, has been above the AverageRMS OverVoltage threshold since last reset. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_PH_B_ID        = 0x0913,     /*!< The number of times the average RMS voltage, has been below the AverageRMS underVoltage threshold since last reset. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PH_B_ID          = 0x0914,     /*!< The duration in seconds used to measure an extreme over voltage condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PH_B_ID         = 0x0915,     /*!< The duration in seconds used to measure an extreme under voltage condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_PH_B_ID                   = 0x0916,     /*!< The duration in seconds used to measure a voltage sag condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_PH_B_ID                 = 0x0917,     /*!< The duration in seconds used to measure a voltage swell condition. */
  /* AC Measurement (Phase C)*/
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_LINE_CURRENT_PH_C_ID                             = 0x0a01,     /*!< Represents the Phase C, AC line current (Square root of active and reactive current) value at the moment in time the attribute is read, in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_PH_C_ID                           = 0x0a02,     /*!< Represents the Phase C, AC active/resistive current value at the moment in time the attribute is read, in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_PH_C_ID                         = 0x0a03,     /*!< Represents the Phase C, AC reactive current value at the moment in time the attribute is read, in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHC_ID                                = 0x0A05,     /*!< Represents the most recent RMS voltage reading in @e Volts (V).*/
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_PH_C_ID                          = 0x0a06,     /*!< Represents the lowest RMS voltage value measured in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_PH_C_ID                          = 0x0a07,     /*!< Represents the highest RMS voltage value measured in Volts (V). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHC_ID                                = 0x0A08,     /*!< Represents the most recent RMS current reading in @e Amps (A).*/
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_PH_C_ID                          = 0x0a09,     /*!< Represents the lowest RMS current value measured in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_PH_C_ID                          = 0x0a0a,     /*!< Represents the highest RMS current value measured in Amps (A). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHC_ID                              = 0x0A0B,     /*!< Represents the Phase C, current demand of active power delivered or received at the premises, in @e Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_PH_C_ID                         = 0x0a0c,     /*!< Represents the lowest AC power value measured in Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_PH_C_ID                         = 0x0a0d,     /*!< Represents the highest AC power value measured in Watts (W). */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_PH_C_ID                           = 0x0a0e,     /*!< Represents the Phase C, current demand of reactive power delivered or received at the premises, in VAr. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHC_ID                            = 0x0A0F,     /*!< Represents the Phase C, current demand of apparent (Square root of active and reactive power) power, in @e VA. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_PH_C_ID                             = 0x0a10,     /*!< Contains the Phase C, Power Factor ratio in 1/100th. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_ID     = 0x0A11,     /*!< The Period in seconds that the RMS voltage is averaged over*/
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_PH_C_ID         = 0x0a12,     /*!< The number of times the average RMS voltage, has been above the AverageRMS OverVoltage threshold since last reset. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_PH_C_ID        = 0x0a13,     /*!< The number of times the average RMS voltage, has been below the AverageRMS underVoltage threshold since last reset. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PH_C_ID          = 0x0a14,     /*!< The duration in seconds used to measure an extreme over voltage condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PH_C_ID         = 0x0a15,     /*!< The duration in seconds used to measure an extreme under voltage condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_PH_C_ID                   = 0x0a16,     /*!< The duration in seconds used to measure a voltage sag condition. */
  ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_PH_C_ID                 = 0x0a17,     /*!< The duration in seconds used to measure a voltage swell condition. */
}esp_zb_zcl_electrical_measurement_attr_t;

typedef enum {
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_MEASUREMENT          = 0x00000001,     /*!< Active Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_REACTIVE_MEASUREMENT        = 0x00000002,     /*!< Reactive Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_APPARENT_MEASUREMENT        = 0x00000004,     /*!< Apparent Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PHASE_A_MEASUREMENT         = 0x00000008,     /*!< Phase A Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PHASE_B_MEASUREMENT         = 0x00000010,     /*!< Phase B Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PHASE_C_MEASUREMENT         = 0x00000020,     /*!< Phase C Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_MEASUREMENT              = 0x00000040,     /*!< DC Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_HARMONIC_MEASUREMENT        = 0x00000080,     /*!< Harmonic Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_POWER_QUALITY_MEASUREMENT   = 0x00000100,     /*!< Power Quality Measurement bit */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_RESERVED                    = 0x00000200
}esp_zb_zcl_electrical_measurement_measurement_type_t;


typedef enum{
  ESP_ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_PROFILE_INFO_RESPONSE_COMMAND        = 0x00,    /*!< This command is generated when the Client command GetProfileInfo is received. */
  ESP_ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_MEASUREMENT_PROFILE_RESPONSE_COMMAND = 0x01,    /*!< This command is generated when the Client command GetMeasurementProfile is received. */
}esp_zb_zcl_electrical_measurement_srv_cmd_t;

typedef enum{
  ESP_ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_PROFILE_INFO_COMMAND                 = 0x00,    /*!< Get Profile Info Command */
  ESP_ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_MEASUREMENT_PROFILE_COMMAND          = 0x01,    /*!< Get Measurement Profile Command */
}esp_zb_zcl_electrical_measurement_cli_cmd_t;

typedef enum{
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PROFILE_INTERVAL_PERIOD_DAILY   = 0x00,   /*!< Represents the daily interval or time frame used to capture parameter for profiling purposes */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PROFILE_INTERVAL_PERIOD_60MINS  = 0x01,   /*!< Represents the 60 minutes interval or time frame used to capture parameter for profiling purposes */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PROFILE_INTERVAL_PERIOD_30MINS  = 0x02,   /*!< Represents the 30 minutes interval or time frame used to capture parameter for profiling purposes */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PROFILE_INTERVAL_PERIOD_15MINS  = 0x03,   /*!< Represents the 15 minutes interval or time frame used to capture parameter for profiling purposes */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PROFILE_INTERVAL_PERIOD_10MINS  = 0x04,   /*!< Represents the 10 minutes interval or time frame used to capture parameter for profiling purposes */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PROFILE_INTERVAL_PERIOD_7_5MINS = 0x05,   /*!< Represents the 7.5 minutes interval or time frame used to capture parameter for profiling purposes */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PROFILE_INTERVAL_PERIOD_5MINS   = 0x06,   /*!< Represents the 5 minutes interval or time frame used to capture parameter for profiling purposes */
  ESP_ZB_ZCL_ELECTRICAL_MEASUREMENT_PROFILE_INTERVAL_PERIOD_2_5MINS = 0x07,   /*!< Represents the 2.5 minutes interval or time frame used to capture parameter for profiling purposes */
}esp_zb_zcl_electrical_measurement_profile_interval_period_t;

/**
* @brief The Zigbee electrical measurement cluster profile information command struct
*
*/
typedef struct esp_zb_electrical_measurement_profile_info_s {
    uint8_t count;                                                                  /*!< The counter for response command */
    esp_zb_zcl_electrical_measurement_profile_interval_period_t interval_period;    /*!< The Profile interval period for profile information response command */
    uint8_t max_number_of_intervals;                                                /*!< The Profile max number of intervals for profile information response command */
    uint16_t attributes_size;                                                       /*!< The Profile attributes size for profile information response command */
    uint16_t *attributes_list;                                                      /*!< The Profile attributes list for profile information response command */
}esp_zb_electrical_measurement_profile_info_t;

/**
* @brief The Zigbee electrical measurement cluster profile command struct
*
*/
typedef struct esp_zb_electrical_measurement_profile_s {
    uint32_t start_time;                                        /*!< The start time for profile response command */
    uint32_t status;                                            /*!< The status for profile response command */
    uint32_t interval_period;                                   /*!< The interval period for profile response command */
    uint8_t interval_delivered_number;                          /*!< The interval delivered number for profile response command */
    uint8_t attributes_id;                                      /*!< The interval attributes id for profile response command */
    void *intervals;                                            /*!< The array of atttibute values intervals id for profile response command */
}esp_zb_electrical_measurement_profile_t;


#ifdef __cplusplus
}
#endif
