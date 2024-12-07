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
/* PURPOSE: Electrical Measurement cluster definitions
*/

#ifndef ZB_ZCL_ELECTRICAL_MEASUREMENT_H
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/* Cluster ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT */

/*! @addtogroup ZB_ZCL_ELECTRICAL_MEASUREMENT
    @{
    @name Electrical Measurement cluster attributes
    @{
*/

/** @brief Default value for Electrical Measurement cluster revision global attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/*! @brief Electrical Measurement cluster attribute identifiers
    @see HA spec, subclause 9.2.2.2
*/
enum zb_zcl_electrical_measurement_attr_e
{
  /** This attribute indicates a device s measurement capabilities. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID                          = 0x0000,
  /** The DCVoltage attribute represents the most recent DC voltage reading in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_ID                                = 0x0100,
  /** The DCVoltageMin attribute represents the lowest DC voltage value measured in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MIN_ID                            = 0x0101,
  /** The DCVoltageMax attribute represents the highest DC voltage value measured in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MAX_ID                            = 0x0102,
  /** The DCCurrent attribute represents the most recent DC current reading in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_ID                                = 0x0103,
  /** The DCCurrentMin attribute represents the lowest DC current value measured in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_MIN_ID                            = 0x0104,
  /** The DCCurrentMax attribute represents the highest DC current value measured in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_MAX_ID                            = 0x0105,
  /** The @e DCPower attribute represents the most recent DC power reading in @e Watts (W) */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DCPOWER_ID                                   = 0x0106,
  /** The DCPowerMin attribute represents the lowest DC power value measured in Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_POWER_MIN_ID                              = 0x0107,
  /** The DCPowerMax attribute represents the highest DC power value measured in Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_POWER_MAX_ID                              = 0x0108,
  /** The DCVoltageMultiplier provides a value to be multiplied against the DCVoltage,
   *  DCVoltageMin, and DCVoltageMax attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MULTIPLIER_ID                     = 0x0200,
  /** The DCVoltageDivisor provides a value to be divided against the DCVoltage,
   *  DCVoltageMin, and DCVoltageMax attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_DIVISOR_ID                        = 0x0201,
  /** The DCCurrentMultiplier provides a value to be multiplied against the DCCurrent,
   *  DCCurrentMin, and DCCurrentMax attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_MULTIPLIER_ID                     = 0x0202,
  /** The DCCurrentDivisor provides a value to be divided against the DCCurrent,
   *  DCCurrentMin, and DCCurrentMax attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_DIVISOR_ID                        = 0x0203,
  /** The DCPowerMultiplier provides a value to be multiplied against the DCPower,
   *  DCPowerMin, and DCPowerMax attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_POWER_MULTIPLIER_ID                       = 0x0204,
  /** The DCPowerDivisor provides a value to be divided against the DCPower,
   *  DCPowerMin, and DCPowerMax attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_POWER_DIVISOR_ID                          = 0x0205,
  /** The ACFrequency attribute represents the most recent AC Frequency reading in Hertz (Hz). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_ID                              = 0x0300,
  /** The ACFrequencyMin attribute represents the lowest AC Frequency value measured in Hertz (Hz). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MIN_ID                          = 0x0301,
  /** The ACFrequencyMax attribute represents the highest AC Frequency value measured in Hertz (Hz). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MAX_ID                          = 0x0302,
  /** The NeutralCurrent attribute represents the AC neutral (Line-Out) current value
   *  at the moment in time the attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_NEUTRAL_CURRENT_ID                           = 0x0303,
  /** Active power represents the current demand of active power delivered or
   *  received at the premises, in @e kW */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_ACTIVE_POWER_ID                        = 0x0304,
  /** Reactive power represents the current demand of reactive power delivered or
   *  received at the premises, in kVAr. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_REACTIVE_POWER_ID                      = 0x0305,
  /** Represents the current demand of apparent power, in @e kVA */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_APPARENT_POWER_ID                      = 0x0306,
  /** Attribute represent the most recent 1st harmonic current reading in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED1ST_HARMONIC_CURRENT_ID              = 0x0307,
  /** Attribute represent the most recent 3rd harmonic current reading in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED3RD_HARMONIC_CURRENT_ID              = 0x0308,
  /** Attribute represent the most recent 5th harmonic current reading in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED5TH_HARMONIC_CURRENT_ID              = 0x0309,
  /** Attribute represent the most recent 7th harmonic current reading in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED7TH_HARMONIC_CURRENT_ID              = 0x030a,
  /** Attribute represent the most recent 9th harmonic current reading in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED9TH_HARMONIC_CURRENT_ID              = 0x030b,
  /** Attribute represent the most recent 11th harmonic current reading in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED11TH_HARMONIC_CURRENT_ID             = 0x030c,
  /** Attribute represent the most recent phase of the 1st harmonic current reading
   *  in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE1ST_HARMONIC_CURRENT_ID        = 0x030d,
  /** Attribute represent the most recent phase of the 3rd harmonic current reading
   *  in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE3RD_HARMONIC_CURRENT_ID        = 0x030e,
  /** Attribute represent the most recent phase of the 5th harmonic current reading
   *  in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE5TH_HARMONIC_CURRENT_ID        = 0x030f,
  /** Attribute represent the most recent phase of the 7th harmonic current reading
   *  in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE7TH_HARMONIC_CURRENT_ID        = 0x0310,
  /** Attribute represent the most recent phase of the 9th harmonic current reading
   *  in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE9TH_HARMONIC_CURRENT_ID        = 0x0311,
  /** Attribute represent the most recent phase of the 11th harmonic current reading
   *  in an AC frequency. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASURED_PHASE11TH_HARMONIC_CURRENT_ID       = 0x0312,
  /** Provides a value to be multiplied against the ACFrequency attribute. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MULTIPLIER_ID                   = 0x0400,
  /** Provides a value to be divided against the ACFrequency attribute. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_DIVISOR_ID                      = 0x0401,
  /** Provides a value to be multiplied against a raw or uncompensated
   *  sensor count of power being measured by the metering device. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_MULTIPLIER_ID                          = 0x0402,
  /** Provides a value to divide against the results of applying the
   *  @e Multiplier attribute against a raw or uncompensated sensor
   *  count of power being measured by the metering device. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_DIVISOR_ID                             = 0x0403,
  /** Represents the unit value for the MeasuredNthHarmonicCurrent attribute in the
   *  format MeasuredNthHarmonicCurrent * 10 ^ HarmonicCurrentMultiplier amperes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_HARMONIC_CURRENT_MULTIPLIER_ID               = 0x0404,
  /** Represents the unit value for the MeasuredPhaseNthHarmonicCurrent attribute in
   *  the format MeasuredPhaseNthHarmonicCurrent * 10 ^ PhaseHarmonicCurrentMultiplier
   *  degrees. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_PHASE_HARMONIC_CURRENT_MULTIPLIER_ID         = 0x0405,
  /** Represents the single phase or Phase A, AC line current (Square root of active and
   *  reactive current) value at the moment in time the attribute is read, in @e Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_LINE_CURRENT_ID                              = 0x0501,
  /** Represents the single phase or Phase A, AC active/resistive current value at the
   *  moment in time the attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_ID                            = 0x0502,
  /** Represents the single phase or Phase A, AC reactive current value at the moment
   *  in time the attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_ID                          = 0x0503,
  /** Represents the most recent RMS voltage reading in @e Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID                                = 0x0505,
  /** Represents the lowest RMS voltage value measured in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_ID                           = 0x0506,
  /** Represents the highest RMS voltage value measured in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_ID                           = 0x0507,
  /** Represents the most recent RMS current reading in @e Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID                                = 0x0508,
  /** Represents the lowest RMS current value measured in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_ID                           = 0x0509,
  /** Represents the highest RMS current value measured in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_ID                           = 0x050a,
  /** Represents the single phase or Phase A, current demand of active power delivered or
   *  received at the premises, in @e Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID                              = 0x050B,
  /** Represents the lowest AC power value measured in Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_ID                          = 0x050c,
  /** Represents the highest AC power value measured in Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_ID                          = 0x050d,
  /** Represents the single phase or Phase A, current demand of reactive power delivered
   *  or received at the premises, in VAr. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_ID                            = 0x050e,
  /** Represents the single phase or Phase A, current demand of apparent (Square root
   *  of active and reactive power) power, in @e VA. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_ID                            = 0x050F,
  /** Contains the single phase or PhaseA, Power Factor ratio in 1/100th. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_ID                              = 0x0510,
  /** The Period in seconds that the RMS voltage is averaged over. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_ID     = 0x0511,
  /** The number of times the average RMS voltage, has been above the AverageRMS
   *  OverVoltage threshold since last reset. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_ID          = 0x0512,
  /** The number of times the average RMS voltage, has been below the AverageRMS
   *  underVoltage threshold since last reset. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_ID         = 0x0513,
  /** The duration in seconds used to measure an extreme over voltage condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_ID           = 0x0514,
  /** The duration in seconds used to measure an extreme under voltage condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_ID          = 0x0515,
  /** The duration in seconds used to measure a voltage sag condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_ID                    = 0x0516,
  /** The duration in seconds used to measure a voltage swell condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_ID                  = 0x0517,
  /** Provides a value to be multiplied against the @e InstantaneousVoltage and @e
   *  RMSVoltage attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_ID                      = 0x0600,
  /** Provides a value to be divided against the @e InstantaneousVoltage and
   *  @e RMSVoltage attributes. This attribute must be used in conjunction with the @e
   *  ACVoltageMultiplier attribute. 0x0000 is an invalid value for this attribute. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_ID                         = 0x0601,
  /** Provides a value to be multiplied against the @e InstantaneousCurrent and @e
   *  RMSCurrent attributes */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_ID                      = 0x0602,
  /** Provides a value to be divided against the @e ACCurrent, @e InstantaneousCurrent
   *  and @e RMSCurrent attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_ID                         = 0x0603,
  /** Provides a value to be multiplied against the @e InstantaneousPower and @e
   *  ActivePower attributes */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID                        = 0x0604,
  /** Provides a value to be divided against the @e InstantaneousPower and @e
   *  ActivePower attributes. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID                           = 0x0605,
  /** Specifies which configurable alarms may be generated. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_OVERLOAD_ALARMS_MASK_ID                   = 0x0700,
  /** Specifies the alarm threshold, set by the manufacturer, for the maximum output
   *  voltage supported by device. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_OVERLOAD_ID                       = 0x0701,
  /** Specifies the alarm threshold, set by the manufacturer, for the maximum output
   *  current supported by device. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_CURRENT_OVERLOAD_ID                       = 0x0702,
  /** Specifies which configurable alarms may be generated. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_ALARMS_MASK_ID                            = 0x0800,
  /** Specifies the alarm threshold, set by the manufacturer, for the maximum output
   *  voltage supported by device. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_VOLTAGE_OVERLOAD_ID                       = 0x0801,
  /** Specifies the alarm threshold, set by the manufacturer, for the maximum output
   *  current supported by device. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_CURRENT_OVERLOAD_ID                       = 0x0802,
  /** Specifies the alarm threshold, set by the manufacturer, for the maximum output
   *  active power supported by device. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_ACTIVE_POWER_OVERLOAD_ID                  = 0x0803,
  /** Specifies the alarm threshold, set by the manufacturer, for the maximum output
   *  reactive power supported by device. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AC_REACTIVE_POWER_OVERLOAD_ID                = 0x0804,
  /** The average RMS voltage above which an over voltage condition is reported. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_ID                  = 0x0805,
  /** The average RMS voltage below which an under voltage condition is reported. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_ID                 = 0x0806,
  /** The RMS voltage above which an extreme under voltage condition is reported. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_ID                  = 0x0807,
  /** The RMS voltage below which an extreme under voltage condition is reported. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_ID                 = 0x0808,
  /** The RMS voltage below which a sag condition is reported. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_ID                           = 0x0809,
  /** The RMS voltage above which a swell condition is reported. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_ID                         = 0x080a,
  /** Represents the Phase B, AC line current (Square root sum of active and reactive
   *  currents) value at the moment in time the attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_LINE_CURRENT_PH_B_ID                         = 0x0901,
  /** Represents the Phase B, AC active/resistive current value at the moment in time
   *  the attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_PH_B_ID                       = 0x0902,
  /** Represents the Phase B, AC reactive current value at the moment in time the
   *  attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_PH_B_ID                     = 0x0903,
  /** Represents the most recent RMS voltage reading in @e Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHB_ID                            = 0x0905,
  /** Represents the lowest RMS voltage value measured in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_PH_B_ID                      = 0x0906,
  /** Represents the highest RMS voltage value measured in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_PH_B_ID                      = 0x0907,
  /** Represents the most recent RMS current reading in @e Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHB_ID                            = 0x0908,
  /** Represents the lowest RMS current value measured in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_PH_B_ID                      = 0x0909,
  /** Represents the highest RMS current value measured in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_PH_B_ID                      = 0x090a,
  /** Represents the Phase B, current demand of active power delivered or received at
   *  the premises, in @e Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHB_ID                          = 0x090B,
  /** Represents the lowest AC power value measured in Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_PH_B_ID                     = 0x090c,
  /** Represents the highest AC power value measured in Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_PH_B_ID                     = 0x090d,
  /** Represents the Phase B, current demand of reactive power delivered or received
   *  at the premises, in VAr. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_PH_B_ID                       = 0x090e,
  /** Represents the Phase B, current demand of apparent (Square root of active and
   *  reactive power) power, in @e VA. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHB_ID                        = 0x090F,
  /** Contains the PhaseB, Power Factor ratio in 1/100th. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_PH_B_ID                         = 0x0910,
  /** The number of times the average RMS voltage, has been above the
   *  @e AverageRMS @e OverVoltage threshold since last reset. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_ID = 0x0911,
  /** The number of times the average RMS voltage, has been above the AverageRMS
   *  OverVoltage threshold since last reset. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_PH_B_ID     = 0x0912,
  /** The number of times the average RMS voltage, has been below the AverageRMS
   *  underVoltage threshold since last reset. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_PH_B_ID    = 0x0913,
  /** The duration in seconds used to measure an extreme over voltage condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PH_B_ID      = 0x0914,
  /** The duration in seconds used to measure an extreme under voltage condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PH_B_ID     = 0x0915,
  /** The duration in seconds used to measure a voltage sag condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_PH_B_ID               = 0x0916,
  /** The duration in seconds used to measure a voltage swell condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_PH_B_ID             = 0x0917,
  /** Represents the Phase C, AC line current (Square root of active and reactive
   *  current) value at the moment in time the attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_LINE_CURRENT_PH_C_ID                         = 0x0a01,
  /** Represents the Phase C, AC active/resistive current value at the moment in time
   *  the attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_PH_C_ID                       = 0x0a02,
  /** Represents the Phase C, AC reactive current value at the moment in time the
   *  attribute is read, in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_PH_C_ID                     = 0x0a03,
  /** Represents the most recent RMS voltage reading in @e Volts (V).*/
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHC_ID                            = 0x0A05,
  /** Represents the lowest RMS voltage value measured in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_PH_C_ID                      = 0x0a06,
  /** Represents the highest RMS voltage value measured in Volts (V). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_PH_C_ID                      = 0x0a07,
  /** Represents the most recent RMS current reading in @e Amps (A).*/
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHC_ID                            = 0x0A08,
  /** Represents the lowest RMS current value measured in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_PH_C_ID                      = 0x0a09,
  /** Represents the highest RMS current value measured in Amps (A). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_PH_C_ID                      = 0x0a0a,
  /** Represents the Phase C, current demand of active power delivered
   *  or received at the premises, in @e Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHC_ID                          = 0x0A0B,
  /** Represents the lowest AC power value measured in Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_PH_C_ID                     = 0x0a0c,
  /** Represents the highest AC power value measured in Watts (W). */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_PH_C_ID                     = 0x0a0d,
  /** Represents the Phase C, current demand of reactive power delivered or received
   *  at the premises, in VAr. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_PH_C_ID                       = 0x0a0e,
  /** Represents the Phase C, current demand of apparent (Square root of active and
   *  reactive power) power, in @e VA. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHC_ID                        = 0x0A0F,
  /** Contains the Phase C, Power Factor ratio in 1/100th. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_PH_C_ID                         = 0x0a10,
  /** The Period in seconds that the RMS voltage is averaged over*/
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_ID = 0x0A11,
  /** The number of times the average RMS voltage, has been above the AverageRMS
   *  OverVoltage threshold since last reset. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_PH_C_ID     = 0x0a12,
  /** The number of times the average RMS voltage, has been below the AverageRMS
   *  underVoltage threshold since last reset. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_PH_C_ID    = 0x0a13,
  /** The duration in seconds used to measure an extreme over voltage condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PH_C_ID      = 0x0a14,
  /** The duration in seconds used to measure an extreme under voltage condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PH_C_ID     = 0x0a15,
  /** The duration in seconds used to measure a voltage sag condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_PH_C_ID               = 0x0a16,
  /** The duration in seconds used to measure a voltage swell condition. */
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_PH_C_ID             = 0x0a17,
};

/*! @brief Bit mapping for Measurement Type attribute,
    @see HA spec, subclause 9.2.2.2.1.1 */
enum zb_zcl_electrical_measurement_measurement_type_e
{
  /*! Active Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_MEASUREMENT          = 0x00000001,
  /*! Reactive Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_REACTIVE_MEASUREMENT        = 0x00000002,
  /*! Apparent Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_APPARENT_MEASUREMENT        = 0x00000004,
  /*! Phase A Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_PHASE_A_MEASUREMENT         = 0x00000008,
  /*! Phase B Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_PHASE_B_MEASUREMENT         = 0x00000010,
  /*! Phase C Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_PHASE_C_MEASUREMENT         = 0x00000020,
  /*! DC Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_MEASUREMENT              = 0x00000040,
  /*! Harmonic Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_HARMONIC_MEASUREMENT        = 0x00000080,
  /*! Power Quality Measurement bit */
  ZB_ZCL_ELECTRICAL_MEASUREMENT_POWER_QUALITY_MEASUREMENT   = 0x00000100,

  ZB_ZCL_ELECTRICAL_MEASUREMENT_RESERVED                    = 0x00000200
};

/** @brief Default value for Measurement Type attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_DEFAULT_VALUE 0x00000000

/** @brief Default value for DCVoltage attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for DCVoltageMin attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MIN_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for DCVoltageMax attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MAX_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for DCCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for DCCurrentMin attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_CURRENT_MIN_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for DCCurrentMax attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_CURRENT_MAX_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for DCPowerMin attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_POWER_MIN_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for DCPowerMax attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_POWER_MAX_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for DCVoltageMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MULTIPLIER_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for DCVoltageDivisor attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_DIVISOR_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for DCCurrentMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_CURRENT_MULTIPLIER_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for DCCurrentDivisor attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_CURRENT_DIVISOR_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for DCPowerMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_POWER_MULTIPLIER_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for DCPowerDivisor attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_POWER_DIVISOR_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for ACFrequency attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ACFrequencyMin attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MIN_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ACFrequencyMax attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MAX_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for NeutralCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_NEUTRAL_CURRENT_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for DCPower attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DCPOWER_DEFAULT_VALUE 0x8000

/** @brief Default value for Measured1stHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED1ST_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for Measured3rdHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED3RD_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for Measured5thHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED5TH_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for Measured7thHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED7TH_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for Measured9thHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED9TH_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for Measured11thHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED11TH_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for MeasuredPhase1stHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED_PHASE1ST_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for MeasuredPhase3rdHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED_PHASE3RD_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for MeasuredPhase5thHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED_PHASE5TH_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for MeasuredPhase7thHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED_PHASE7TH_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for MeasuredPhase9thHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED_PHASE9TH_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for MeasuredPhase11thHarmonicCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_MEASURED_PHASE11TH_HARMONIC_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ACFrequencyMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_MULTIPLIER_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for ACFrequencyDivisor attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_FREQUENCY_DIVISOR_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for PowerMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_POWER_MULTIPLIER_DEFAULT_VALUE ((zb_uint32_t)0x000001)

/** @brief Default value for PowerDivisor attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_POWER_DIVISOR_DEFAULT_VALUE ((zb_uint32_t)0x000001)

/** @brief Default value for HarmonicCurrentMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_HARMONIC_CURRENT_MULTIPLIER_DEFAULT_VALUE ((zb_int8_t)0x00)

/** @brief Default value for PhaseHarmonicCurrentMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_PHASE_HARMONIC_CURRENT_MULTIPLIER_DEFAULT_VALUE ((zb_int8_t)0x00)

/** @brief Default value for LineCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_LINE_CURRENT_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ActiveCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ReactiveCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for RMSVoltageMin attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSVoltageMax attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSCurrentMin attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSCurrentMax attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ActivePowerMin attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ActivePowerMax attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ReactivePower attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ApparentPower attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_APPARENT_POWER_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for AverageRmsvoltageMeasurementPeriod attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for AverageRMSOverVoltageCounter attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for AverageRMSUnderVoltageCounter attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSExtremeOverVoltagePeriod attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSExtremeUnderVoltagePeriod attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSVoltageSagPeriod attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSVoltageSwellPeriod attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for AcvoltageMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACVOLTAGE_MULTIPLIER_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for AcvoltageDivisor attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACVOLTAGE_DIVISOR_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for DCOverloadAlarmsMask attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_OVERLOAD_ALARMS_MASK_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for DCVoltageOverload attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_OVERLOAD_DEFAULT_VALUE ((zb_int16_t)0xFFFF)

/** @brief Default value for DCCurrentOverload attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_CURRENT_OVERLOAD_DEFAULT_VALUE ((zb_int16_t)0xFFFF)

/** @brief Default value for ACAlarmsMask attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_ALARMS_MASK_DEFAULT_VALUE ((zb_uint16_t)0x00)

/** @brief Default value for ACVoltageOverload attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_VOLTAGE_OVERLOAD_DEFAULT_VALUE ((zb_int16_t)0xFFFF)

/** @brief Default value for ACCurrentOverload attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_CURRENT_OVERLOAD_DEFAULT_VALUE ((zb_int16_t)0xFFFF)

/** @brief Default value for ACActivePowerOverload attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_ACTIVE_POWER_OVERLOAD_DEFAULT_VALUE ((zb_int16_t)0xFFFF)

/** @brief Default value for ACReactivePowerOverload attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AC_REACTIVE_POWER_OVERLOAD_DEFAULT_VALUE ((zb_int16_t)0xFFFF)

/** @brief Default value for RMSVoltage attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_DEFAULT_VALUE 0xffff

/** @brief Default value for RMSCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMSCURRENT_DEFAULT_VALUE 0xffff

/** @brief Default value for ActivePower attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_DEFAULT_VALUE 0xffff

/** @brief Default value for RMSCurrent attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_POWER_FACTOR_DEFAULT_VALUE 0x0

/** @brief Default value for PowerMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_DEFAULT_VALUE 0x0001

/** @brief Default value for PowerDivisor attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_DEFAULT_VALUE 0x0001

/** @brief Default value for ACCurrentMultiplier attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACCURRENT_MULTIPLIER_DEFAULT_VALUE 0x0001

/** @brief Default value for ACCurrentDivisor attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACCURRENT_DIVISOR_DEFAULT_VALUE 0x0001

/** @brief Default value for LineCurrentPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_LINE_CURRENT_PH_B_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ActiveCurrentPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_PH_B_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ReactiveCurrentPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_PH_B_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for RmsvoltagePHB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHB_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSVoltageMinPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_PH_B_DEFAULT_VALUE ((zb_uint16_t)0x8000)

/** @brief Default value for RMSVoltageMaxPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_PH_B_DEFAULT_VALUE ((zb_uint16_t)0x8000)

/** @brief Default value for RmscurrentPHB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHB_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSCurrentMinPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_PH_B_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSCurrentMaxPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_PH_B_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ActivePowerPHB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHB_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ActivePowerMinPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_PH_B_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ActivePowerMaxPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_PH_B_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ReactivePowerPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_PH_B_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ApparentPowerPHB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHB_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for PowerFactorPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_POWER_FACTOR_PH_B_DEFAULT_VALUE ((zb_int8_t)0x00)

/** @brief Default value for AverageRmsvoltageMeasurementPeriodPHB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for AverageRMSOverVoltageCounterPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_PH_B_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for AverageRMSUnderVoltageCounterPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_PH_B_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSExtremeOverVoltagePeriodPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PH_B_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSExtremeUnderVoltagePeriodPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PH_B_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSVoltageSagPeriodPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_PH_B_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSVoltageSwellPeriodPhB attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_PH_B_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for LineCurrentPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_LINE_CURRENT_PH_C_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ActiveCurrentPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_CURRENT_PH_C_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ReactiveCurrentPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_REACTIVE_CURRENT_PH_C_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for rmsvoltagePHC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHC_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSVoltageMinPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MIN_PH_C_DEFAULT_VALUE ((zb_uint16_t)0x8000)

/** @brief Default value for RMSVoltageMaxPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_MAX_PH_C_DEFAULT_VALUE ((zb_uint16_t)0x8000)

/** @brief Default value for rmscurrentPHC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHC_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSCurrentMinPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MIN_PH_C_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for RMSCurrentMaxPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_CURRENT_MAX_PH_C_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ActivePowerPHC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHC_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ActivePowerMinPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MIN_PH_C_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ActivePowerMaxPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_MAX_PH_C_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ReactivePowerPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_REACTIVE_POWER_PH_C_DEFAULT_VALUE ((zb_int16_t)0x8000)

/** @brief Default value for ApparentPowerPHC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHC_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for PowerFactorPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_POWER_FACTOR_PH_C_DEFAULT_VALUE ((zb_int8_t)0x00)

/** @brief Default value for AverageRmsvoltageMeasurementPeriodPHC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for AverageRMSOverVoltageCounterPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_OVER_VOLTAGE_COUNTER_PH_C_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for AverageRMSUnderVoltageCounterPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_AVERAGE_RMS_UNDER_VOLTAGE_COUNTER_PH_C_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSExtremeOverVoltagePeriodPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_EXTREME_OVER_VOLTAGE_PERIOD_PH_C_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSExtremeUnderVoltagePeriodPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_EXTREME_UNDER_VOLTAGE_PERIOD_PH_C_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSVoltageSagPeriodPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SAG_PERIOD_PH_C_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RMSVoltageSwellPeriodPhC attribute */
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE_SWELL_PERIOD_PH_C_DEFAULT_VALUE ((zb_uint16_t)0x0000)


/*! @} */ /* Electrical Measurement cluster attributes */

/*! @brief Electrical Measurement cluster server command identifiers
    @see ZCL spec, subclause 4.9.2.3.1
*/
enum zb_zcl_electrical_measurement_srv_cmd_e
{
  /** This command is generated when the Client command GetProfileInfo is received. */
  ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_PROFILE_INFO_RESPONSE_COMMAND        = 0x00,
  /** This command is generated when the Client command GetMeasurementProfile is received. */
  ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_MEASUREMENT_PROFILE_RESPONSE_COMMAND = 0x01,
};

/*! @brief Electrical Measurement cluster client command identifiers
    @see ZCL spec, subclause 4.9.2.4.1
*/
enum zb_zcl_electrical_measurement_cli_cmd_e
{
  /** Get Profile Info Command */
  ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_PROFILE_INFO_COMMAND                 = 0x00,
  /** Get Measurement Profile Command */
  ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_MEASUREMENT_PROFILE_COMMAND          = 0x01,
};

/*! @cond internals_doc
    @internal @name Electrical Measurement cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID(data_ptr) \
{                                                                                               \
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_32BITMAP,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                           \
  (void*) data_ptr                                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DCPOWER_ID(data_ptr) \
{                                                                                      \
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DCPOWER_ID,                                       \
  ZB_ZCL_ATTR_TYPE_S16,                                                                \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                  \
  (void*) data_ptr                                                                     \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID(data_ptr) \
{                                                                                         \
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                            \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID(data_ptr) \
{                                                                                         \
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                            \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID(data_ptr)     \
{                                                                                               \
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID,                                           \
  ZB_ZCL_ATTR_TYPE_S16,                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                           \
  (void*) data_ptr                                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_ID(data_ptr)     \
{                                                                                               \
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_POWER_FACTOR_ID,                                           \
  ZB_ZCL_ATTR_TYPE_S8,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                           \
  (void*) data_ptr                                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID(data_ptr) \
{                                                                                                 \
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_MULTIPLIER_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                             \
  (void*) data_ptr                                                                                \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID(data_ptr) \
{                                                                                              \
  ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACPOWER_DIVISOR_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                          \
  (void*) data_ptr                                                                             \
}

/*!
* @}
* @endcond
*/
/** @brief Declare attribute list for Electrical Measurement cluster
    @param attr_list - attribute list name
    @param measurement_type - pointer to variable to store Measurement Type attribute value
    @param dcpower - pointer to variable to store DCPower attribute value
*/
#define ZB_ZCL_DECLARE_ELECTRICAL_MEASUREMENT_ATTRIB_LIST(attr_list, measurement_type, dcpower)    \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_ELECTRICAL_MEASUREMENT)      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID, (measurement_type)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DCPOWER_ID, (dcpower))                   \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*!
    @cond internals_doc
  @{
  @internal Number of attributes mandatory for reporting in Electrical Measurement cluster
*/
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_REPORT_ATTR_COUNT 2

#define ZB_ZCL_ELECTRICAL_MEASUREMENT_SEND_GET_PROFILE_INFO_RESP(                                 \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,                                 \
  profile_count, profile_interval_period, max_number_of_intervals,                                \
  list_of_attributes, list_of_attributes_size)                                                    \
{                                                                                                 \
  zb_uint8_t ind;                                                                                 \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                                  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);                                       \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(),                                      \
    ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_PROFILE_INFO_RESPONSE_COMMAND);                         \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (profile_count));                                                  \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (profile_interval_period));                                        \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (max_number_of_intervals));                                        \
  for (ind=0; ind < list_of_attributes_size; ind++)                                               \
  {                                                                                               \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (list_of_attributes[ind]));                                 \
  }                                                                                               \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                                             \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep,                              \
    prfl_id, ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, cb);                                       \
}

/**
 *  @deprecated This function will be removed in the next Major release
 *              Use @ref ZB_ZCL_ELECTRICAL_MEASUREMENT_SEND_GET_MEASUREMENT_PROFILE_RESP() instead
*/
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_SEND_GET_MEASUREMENT_PROFILE_INFO_RESP(                     \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,                                 \
  start_time, status, profile_interval_period, number_of_intervals_delivered, attr_id,            \
  list_of_intervals)                                                                              \
{                                                                                                 \
  zb_uint8_t ind;                                                                                 \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer)                                                   \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr, def_resp)                              \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(),                                      \
    ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_MEASUREMENT_PROFILE_RESPONSE_COMMAND);                  \
  ZB_ZCL_PACKET_PUT_DATA32_VAL(ptr, (start_time));                                                \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (status));                                                         \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (profile_interval_period));                                        \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (number_of_intervals_delivered));                                  \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (attr_id));                                                   \
  for (ind=0; ind < number_of_intervals_delivered; ind++)                                         \
  {                                                                                               \
                                                                                                  \
  }                                                                                               \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                               \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep,                              \
    prfl_id, ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, cb);                                       \
}

#define ZB_ZCL_ELECTRICAL_MEASUREMENT_SEND_GET_MEASUREMENT_PROFILE_RESP(                          \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,                                 \
  start_time, status, profile_interval_period, number_of_intervals_delivered, attr_id,            \
  attr_type, list_of_intervals)                                                                   \
{                                                                                                 \
  zb_uint8_t ind;                                                                                 \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer)                                                   \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr, def_resp)                              \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(),                                      \
    ZB_ZCL_CMD_ELECTRICAL_MEASUREMENT_GET_MEASUREMENT_PROFILE_RESPONSE_COMMAND);                  \
  ZB_ZCL_PACKET_PUT_DATA32_VAL(ptr, (start_time));                                                \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (status));                                                         \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (profile_interval_period));                                        \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (number_of_intervals_delivered));                                  \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (attr_id));                                                   \
  for (ind=0; ind < number_of_intervals_delivered; ind++)                                         \
  {                                                                                               \
    (ptr) = zb_zcl_put_value_to_packet((ptr), (attr_type), list_of_intervals[ind]);               \
  }                                                                                               \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                               \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep,                              \
    prfl_id, ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT, cb);                                       \
}

/*! @}
    @endcond */ /* Electrical Measurement cluster internals */

/*! @} */ /* ZCL HA Electrical Measurement cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_el_measurement_init_server(void);
void zb_zcl_el_measurement_init_client(void);

#define ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT_SERVER_ROLE_INIT zb_zcl_el_measurement_init_server
#define ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT_CLIENT_ROLE_INIT zb_zcl_el_measurement_init_client

#endif /* ZB_ZCL_ELECTRICAL_MEASUREMENT_H */
