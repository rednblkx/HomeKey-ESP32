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
/* PURPOSE: Power Configuration cluster definitions
*/

#ifndef ZB_ZCL_POWER_CONFIG_H
#define ZB_ZCL_POWER_CONFIG_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_POWER_CONFIG
 *  @{
 */

/**
 * @brief Hook on Write attribute
 * @param endpoint - endpoint number
 * @param attr_id - ID of attribute being written
 * @param new_value - pointer to new value of attribute
 */
void zb_zcl_power_config_write_attr_hook(zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *new_value);

/* Cluster ZB_ZCL_CLUSTER_ID_POWER_CONFIG */

/*! @name Power Configuration cluster attributes
    @{
*/

/*! @brief Power Configuration cluster attribute identifiers
    @see ZCL spec, Power Configuration Cluster 3.3.2.2
*/
enum zb_zcl_power_config_attr_e
{
  /** @brief MainsVoltage attribute, ZCL spec 3.3.2.2.1.1 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID                 = 0x0000,
  /** @brief MainsFrequency attribute, ZCL spec 3.3.2.2.1.2 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID               = 0x0001,
  /** @brief MainsAlarmMask attribute, ZCL spec 3.3.2.2.2.1 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_ALARM_MASK_ID              = 0x0010,
  /** @brief MainsVoltageMinThreshold attribute, ZCL spec 3.3.2.2.2.2 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD      = 0x0011,
  /** @brief MainsVoltageMaxThreshold attribute, ZCL spec 3.3.2.2.2.3 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD      = 0x0012,
  /** @brief MainsVoltageDwellTripPoint attribute, ZCL spec 3.3.2.2.2.4 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_DWELL_TRIP_POINT           = 0x0013,

  /** @brief BatteryVoltage attribute, ZCL spec 3.3.2.2.3.1 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID               = 0x0020,
  /*! @brief BatteryPercentageRemaining attribute, ZCL spec 3.3.2.2.3.2 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID  = 0x0021,

  /** The BatteryManufacturer attribute is a maximum of 16 bytes in length
   *  and specifies the name of the battery manufacturer as a character string. */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_MANUFACTURER_ID          = 0x0030,
  /** @brief BatterySize attribute, ZCL spec 3.3.2.2.4.2 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_SIZE_ID                  = 0x0031,
  /** The BatteryAHrRating attribute is 16 bits in length and specifies
   *  the Ampere-hour rating of the battery, measured in units of 10mAHr. */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_A_HR_RATING_ID           = 0x0032,
  /** @brief BatteryQuantity attribute, ZCL spec 3.3.2.2.4.4 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_QUANTITY_ID              = 0x0033,
  /** @brief BatteryRatedVoltage attribute, ZCL spec 3.3.2.2.4.5 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_RATED_VOLTAGE_ID         = 0x0034,
  /** @brief BatteryAlarmMask attribute, ZCL spec 3.3.2.2.4.6 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_MASK_ID            = 0x0035,
  /** @brief BatteryVoltageMinThreshold attribute, ZCL spec 3.3.2.2.4.7 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_MIN_THRESHOLD_ID = 0x0036,
    /*! @brief BatteryVoltageThreshold1 attribute, ZCL spec 3.3.2.2.4.8 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD1_ID        = 0x0037,
  /*! @brief BatteryVoltageThreshold2 attribute, ZCL spec 3.3.2.2.4.8 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD2_ID        = 0x0038,
  /*! @brief BatteryVoltageThreshold3 attribute, ZCL spec 3.3.2.2.4.8 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD3_ID        = 0x0039,

  /*! @brief BatteryPercentageMinThreshold attribute, ZCL spec 3.3.2.2.4.9 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_MIN_THRESHOLD_ID  = 0x003a,

  /*! @brief BatteryPercentageThreshold1 attribute, ZCL spec 3.3.2.2.4.10 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD1_ID     = 0x003b,
  /*! @brief BatteryPercentageThreshold2 attribute, ZCL spec 3.3.2.2.4.10 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD2_ID     = 0x003c,
  /*! @brief BatteryPercentageThreshold3 attribute, ZCL spec 3.3.2.2.4.10 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD3_ID     = 0x003d,

  /*! @brief BatteryAlarmState attribute, ZCL spec 3.3.2.2.4.11 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_STATE_ID               = 0x003e,

  /*! @brief Battery Information 2 attribute set, ZCL spec 3.3.2.2.5 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_ID = 0x0040,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_REMAINING_ID     = 0x0041,

  /*! @brief Battery Settings 2 attribute set, ZCL spec 3.3.2.2.6 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_MANUFACTURER_ID          = 0x0050,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_SIZE_ID = 0x0051,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_A_HR_RATING_ID           = 0x0052,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_QUANTITY_ID = 0x0053,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_RATED_VOLTAGE_ID = 0x0054,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_ALARM_MASK_ID = 0x0055,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_MIN_THRESHOLD_ID = 0x0056,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD1_ID        = 0x0057,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD2_ID        = 0x0058,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD3_ID        = 0x0059,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_MIN_THRESHOLD_ID  = 0x005a,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD1_ID     = 0x005b,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD2_ID     = 0x005c,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD3_ID     = 0x005d,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_ALARM_STATE_ID               = 0x005e,

  /*! @brief Battery Information 3 attribute set, ZCL spec 3.3.2.2.7 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_ID = 0x0060,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_REMAINING_ID  = 0x0061,

  /*! @brief Battery Settings 3 attribute set, ZCL spec 3.3.2.2.8 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_MANUFACTURER_ID          = 0x0070,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_SIZE_ID = 0x0071,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_A_HR_RATING_ID           = 0x0072,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_QUANTITY_ID = 0x0073,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_RATED_VOLTAGE_ID = 0x0074,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_ALARM_MASK_ID = 0x0075,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_MIN_THRESHOLD_ID = 0x0076,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD1_ID        = 0x0077,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD2_ID        = 0x0078,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD3_ID        = 0x0079,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_MIN_THRESHOLD_ID  = 0x007a,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD1_ID     = 0x007b,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD2_ID     = 0x007c,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD3_ID     = 0x007d,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_ALARM_STATE_ID               = 0x007e,
  /** @cond internals_doc */
  /* Custom attributes */
  ZB_ZCL_ATTR_POWER_CONFIG_CUSTOM_BATTERY_VOLTAGE_SHARP_ID  = 0x8020
  /*! @}
   *  @endcond */ /* internals_doc */
};

/**
 * @brief Power Configuration MainsAlarmMask value
 * see ZCL spec 3.3.2.2.4.7
 */
enum zb_zcl_power_config_mains_alarm_mask_e
{
  /** @brief MainsAlarmMask - Mains Voltage too low */
  ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_VOLTAGE_LOW = 0x01,
  /** @brief MainsAlarmMask - Mains Voltage too high */
  ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_VOLTAGE_HIGH = 0x02,
  /** @brief MainsAlarmMask - Mains power supply lost/unavailable */
  ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_VOLTAGE_UNAVAIL = 0x04,

};

/** @brief Default value for Power Configuration cluster revision global attribute */
#define ZB_ZCL_POWER_CONFIG_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Default value for MainsAlarmMask attribute */
#define ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for MainsVoltageMinThreshold attribute */
#define ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for MainsVoltageMaxThreshold attribute */
#define ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for MainsDwellTripPoint attribute */
#define ZB_ZCL_POWER_CONFIG_MAINS_DWELL_TRIP_POINT_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief BatteryVoltage attribute invalid value */
#define ZB_ZCL_POWER_CONFIG_BATTERY_VOLTAGE_INVALID         0xff

/** @brief Power Configuration BatteryPercentageRemaining attribute unknown value */
#define ZB_ZCL_POWER_CONFIG_BATTERY_REMAINING_UNKNOWN            0xff

/** @brief Default value for BatteryRemainingHA attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_REMAINING_HA_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for BatteryManufacturer attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_MANUFACTURER_DEFAULT_VALUE {0}

/** @brief BatterySize attribute default value */
#define ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_DEFAULT_VALUE          0xff

/** @brief BatteryAlarmMask attribute default value */
#define ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_DEFAULT_VALUE    0x00

/** @brief Default value for BatteryVoltageMinThreshold attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_VOLTAGE_MIN_THRESHOLD_DEFAULT_VALUE ((zb_uint8_t)0x0000)

/** @brief Default value for BatteryVoltageThreshold1 attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD1_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for BatteryVoltageThreshold2 attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD2_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for BatteryVoltageThreshold3 attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD3_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for BatteryPercentageMinThreshold attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_PERCENTAGE_MIN_THRESHOLD_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for BatteryPercentageThreshold1 attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD1_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for BatteryPercentageThreshold2 attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD2_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for BatteryPercentageThreshold3 attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD3_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for BatteryAlarmState attribute */
#define ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_DEFAULT_VALUE ((zb_uint32_t)0x00000000)

/** @brief MainsVoltageMinThreshold and MainsVoltageMaxThreshold values when alarm should not be generated*/
#define ZB_ZCL_POWER_CONFIG_THRESHOLD_ALARM_OMISSION_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Mains attribute set the least significant nibble*/
#define ZB_ZCL_POWER_CONFIG_MAINS_ATTRIBUTE_SET 0

/** @brief Battery attribute set the least significant nibble*/
#define ZB_ZCL_POWER_CONFIG_BATTERY_ATTRIBUTE_SET 0

/** @brief Battery Source 2 attribute set the least significant nibble*/
#define ZB_ZCL_POWER_CONFIG_BATTERY_SOURCE_2_ATTRIBUTE_SET 0x20

/** @brief Battery Source 3 attribute set the least significant nibble*/
#define ZB_ZCL_POWER_CONFIG_BATTERY_SOURCE_3_ATTRIBUTE_SET 0x40

/**
 * @brief Power Configuration BatterySize value
 * see ZCL spec 3.3.2.2.4.2
 */
enum zb_zcl_power_config_battery_size_e
{
  /** @brief BatterySize - no battery*/
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_NO_BATTERY = 0,
  /** @brief BatterySize - built in */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_BUILT_IN   = 1,
  /** @brief BatterySize - other */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_OTHER      = 2,
  /** @brief BatterySize - AA */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_AA         = 3,
  /** @brief BatterySize - AAA */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_AAA        = 4,
  /** @brief BatterySize - C */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_C          = 5,
  /** @brief BatterySize - D */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_D          = 6,
  /** @brief BatterySize - CR2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_CR2        = 7,
  /** @brief BatterySize - CR123A */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_CR123A     = 8,
  /** @brief BatterySize - unknown */
    ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_UNKNOWN    = 0xff,
};

/**
 * @brief Power Configuration BatteryAlarmMask value
 * see ZCL spec 3.3.2.2.4.7
 */
enum zb_zcl_power_config_battery_alarm_mask_e
{
  /** @brief BatteryAlarmMask - Battery voltage too low */
  ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_VOLTAGE_LOW = 0,
  /** @brief BatteryAlarmMask - Alarm1 */
  ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_ALARM1 = 1,
  /** @brief BatteryAlarmMask - Alarm2 */
  ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_ALARM2 = 2,
  /** @brief BatteryAlarmMask - Alarm3 */
  ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_ALARM3 = 3,
};

/*! @brief Power Configuration Alarm Code for Alarm
    @see ZCL spec 3.3.2.2.4.7, Table 3-25
*/
enum zb_zcl_power_config_battery_alarm_code_e
{
  /** @brief MainsVoltageAlarmCode - MainsVoltageMinThreshold reached for Mains Voltage**/
    ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_ALARM_CODE_MIN_THRESHOLD = 0x00,
  /** @brief MainsVoltageAlarmCode - MainsVoltageMaxThreshold reached for Mains Voltage**/
    ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_ALARM_CODE_MAX_THRESHOLD = 0x01,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold or
   * BatteryPercentageMinThreshold reached for Battery Source 1 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE1_MIN_THRESHOLD = 0x10,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold1 or
   * BatteryPercentageMinThreshold1 reached for Battery Source 1 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE1_VOLTAGE1   = 0x11,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold2 or
   * BatteryPercentageMinThreshold2 reached for Battery Source 1 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE1_VOLTAGE2   = 0x12,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold3 or
   * BatteryPercentageMinThreshold3 reached for Battery Source 1 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE1_VOLTAGE3   = 0x13,

  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold or
   * BatteryPercentageMinThreshold reached for Battery Source 2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE2_MIN_THRESHOLD = 0x20,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold1 or
   * BatteryPercentageMinThreshold1 reached for Battery Source 2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE2_VOLTAGE1   = 0x21,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold2 or
   * BatteryPercentageMinThreshold2 reached for Battery Source 2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE2_VOLTAGE2   = 0x22,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold3 or
   * BatteryPercentageMinThreshold3 reached for Battery Source 2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE2_VOLTAGE3   = 0x23,

  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold or
   * BatteryPercentageMinThreshold reached for Battery Source 3 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE3_MIN_THRESHOLD = 0x30,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold1 or
   * BatteryPercentageMinThreshold1 reached for Battery Source 3 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE3_VOLTAGE1   = 0x31,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold2 or
   * BatteryPercentageMinThreshold2 reached for Battery Source 3 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE3_VOLTAGE2   = 0x32,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold3 or
   * BatteryPercentageMinThreshold3 reached for Battery Source 3 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_SOURCE3_VOLTAGE3   = 0x33,
  /** @brief Mains power supply lost/unavailable (i.e., device is running on battery) */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_CODE_MAINS_POWER_SUPPLY_LOST_UNAVAILABLE   = 0x3a,
};


/*! @brief Power Configuration BatteryAlarmState
    @see ZCL spec 3.3.2.2.4.11, Table 3-26
*/
enum zb_zcl_power_config_battery_alarm_state_e
{
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold or
   * BatteryPercentageMinThreshold reached for Battery Source 1 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE1_MIN_THRESHOLD = 1 << 0,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold1 or
   * BatteryPercentageMinThreshold1 reached for Battery Source 1 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE1_VOLTAGE1   = 1 << 1,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold2 or
   * BatteryPercentageMinThreshold2 reached for Battery Source 1 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE1_VOLTAGE2   = 1 << 2,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold3 or
   * BatteryPercentageMinThreshold3 reached for Battery Source 1 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE1_VOLTAGE3   = 1 << 3,

  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold or
   * BatteryPercentageMinThreshold reached for Battery Source 2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE2_MIN_THRESHOLD = 1 << 10,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold1 or
   * BatteryPercentageMinThreshold1 reached for Battery Source 2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE2_VOLTAGE1   = 1 << 11,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold2 or
   * BatteryPercentageMinThreshold2 reached for Battery Source 2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE2_VOLTAGE2   = 1 << 12,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold3 or
   * BatteryPercentageMinThreshold3 reached for Battery Source 2 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE2_VOLTAGE3   = 1 << 13,

    /* use 1l to exclude overflow at 16-bit CPU: force 32-bit enum */
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold or
   * BatteryPercentageMinThreshold reached for Battery Source 3 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE3_MIN_THRESHOLD = 1l << 20,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold1 or
   * BatteryPercentageMinThreshold1 reached for Battery Source 3 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE3_VOLTAGE1   = 1l << 21,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold2 or
   * BatteryPercentageMinThreshold2 reached for Battery Source 3 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE3_VOLTAGE2   = 1l << 22,
  /** @brief BatteryAlarmCode - BatteryVoltageMinThreshold3 or
   * BatteryPercentageMinThreshold3 reached for Battery Source 3 */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_SOURCE3_VOLTAGE3   = 1l << 23,
  /** @brief Mains power supply lost/unavailable (i.e., device is running on battery) */
    ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_STATE_MAINS_POWER_SUPPLY_LOST_UNAVAILABLE   = 1l << 30,
};

/** @cond internals_doc */
/*! @internal @name Power Configuration cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID,                    \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID,                  \
  ZB_ZCL_ATTR_TYPE_U8,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_ALARM_MASK_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_ALARM_MASK_ID,         \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                             \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD, \
  ZB_ZCL_ATTR_TYPE_U16,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD, \
  ZB_ZCL_ATTR_TYPE_U16,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_DWELL_TRIP_POINT(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_DWELL_TRIP_POINT,      \
  ZB_ZCL_ATTR_TYPE_U16,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID(data_ptr, bat_num) \
{                                                               \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_VOLTAGE_ID,       \
  ZB_ZCL_ATTR_TYPE_U8,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_SIZE_ID(data_ptr, bat_num) \
{                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_SIZE_ID,  \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_QUANTITY_ID(data_ptr, bat_num) \
{                                                           \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_QUANTITY_ID,  \
  ZB_ZCL_ATTR_TYPE_U8,                                      \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                            \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                       \
  (void*) data_ptr                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_RATED_VOLTAGE_ID(data_ptr, bat_num) \
{                                                               \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_RATED_VOLTAGE_ID, \
  ZB_ZCL_ATTR_TYPE_U8,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

/* DA: HA12 change */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_MASK_ID(data_ptr, bat_num) \
{                                                             \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_ALARM_MASK_ID,  \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                   \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                         \
  (void*) data_ptr                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_MIN_THRESHOLD_ID(data_ptr, bat_num) \
{                                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_VOLTAGE_MIN_THRESHOLD_ID, \
  ZB_ZCL_ATTR_TYPE_U8,                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                   \
  (void*) data_ptr                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID(data_ptr, bat_num) \
{                                                                          \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_PERCENTAGE_REMAINING_ID,     \
  ZB_ZCL_ATTR_TYPE_U8,                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                      \
  (void*) data_ptr                                                         \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD1_ID(data_ptr, bat_num) \
{                                                                     \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_VOLTAGE_THRESHOLD1_ID,  \
  ZB_ZCL_ATTR_TYPE_U8,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD2_ID(data_ptr, bat_num) \
{                                                                     \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_VOLTAGE_THRESHOLD2_ID,  \
  ZB_ZCL_ATTR_TYPE_U8,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD3_ID(data_ptr, bat_num) \
{                                                                     \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_VOLTAGE_THRESHOLD3_ID,  \
  ZB_ZCL_ATTR_TYPE_U8,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_MIN_THRESHOLD_ID(data_ptr, bat_num) \
{                                                                          \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_PERCENTAGE_MIN_THRESHOLD_ID, \
  ZB_ZCL_ATTR_TYPE_U8,                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                      \
  (void*) data_ptr                                                         \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD1_ID(data_ptr, bat_num) \
{                                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_PERCENTAGE_THRESHOLD1_ID, \
  ZB_ZCL_ATTR_TYPE_U8,                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                   \
  (void*) data_ptr                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD2_ID(data_ptr, bat_num) \
{                                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_PERCENTAGE_THRESHOLD2_ID, \
  ZB_ZCL_ATTR_TYPE_U8,                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                   \
  (void*) data_ptr                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD3_ID(data_ptr, bat_num) \
{                                                                       \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_PERCENTAGE_THRESHOLD3_ID, \
  ZB_ZCL_ATTR_TYPE_U8,                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                   \
  (void*) data_ptr                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_STATE_ID(data_ptr, bat_num) \
{                                                               \
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY##bat_num##_ALARM_STATE_ID,   \
  ZB_ZCL_ATTR_TYPE_32BITMAP,                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

/*! @internal Number of attributes mandatory for reporting in Power
 *  Configuration cluster for 1 battery pack */
#undef ZB_ZCL_POWER_CONFIG_REPORT_ATTR_COUNT
#define ZB_ZCL_POWER_CONFIG_REPORT_ATTR_COUNT 2 /* Voltage + Alarm State */
/*! @internal Number of attributes mandatory for reporting in Power
 *  Configuration cluster for 2 Battery packs included */
#define ZB_ZCL_POWER_CONFIG_BAT_PACK_2_REPORT_ATTR_COUNT 4 /* Voltage + Voltage 2 + Alarm State +
                                                            * Alarm State 2 */
/*! @internal Number of attributes mandatory for reporting in Power
 *  Configuration cluster for 3 Battery packs included */
#define ZB_ZCL_POWER_CONFIG_BAT_PACK_3_REPORT_ATTR_COUNT 6 /* Voltage + Voltage 2 + Voltage 3 +
                                                            * Alarm State + Alarm State 2 + Alarm State 3*/

/*! @internal Alarm Code for Power Configuration cluster */
#define ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD_ALARM_CODE 0x00
#define ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD_ALARM_CODE 0x01

/*! @} */ /* Power Configuration cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Declare attribute list for Power Configuration cluster - server side
    @param attr_list - attribute list name
    @param voltage - pointer to variable to store BatteryVoltage attribute
    @param size - pointer to variable to store BatterySize attribute
    @param quantity - pointer to variable to store BatteryQuantity attribute
    @param rated_voltage - pointer to variable to store BatteryRatedVoltage attribute
    @param alarm_mask - pointer to variable to store BatteryAlarmMask attribute
    @param voltage_min_threshold - pointer to variable to store BatteryVoltageMinThreshold attribute
*/
#define ZB_ZCL_DECLARE_POWER_CONFIG_ATTRIB_LIST(attr_list,                                   \
                                                voltage, size, quantity, rated_voltage,      \
                                                alarm_mask, voltage_min_threshold)           \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_POWER_CONFIG)          \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID(voltage, ),             \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_SIZE_ID(size, ),                   \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_QUANTITY_ID(quantity, ),           \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_RATED_VOLTAGE_ID(rated_voltage, ), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_MASK_ID(alarm_mask, ),       \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_MIN_THRESHOLD_ID(voltage_min_threshold, ), \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @cond internals_doc */
#define ZB_ZCL_POWER_CONFIG_MAINS_ATTRIB_LIST(voltage, frequency, alarm_mask, \
                                              voltage_min_threshold, voltage_max_threshold, \
                                              dwell_trip_point)                             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID, (voltage))                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID, (frequency))            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POWER_CONFIG_MAINS_ALARM_MASK_ID, (alarm_mask))          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD, (voltage_min_threshold)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD, (voltage_max_threshold)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POWER_CONFIG_MAINS_DWELL_TRIP_POINT, (dwell_trip_point))
/*! @}
 *  @endcond */ /* internals_doc */


/** @brief Declare attribute list for Power Configuration cluster - server side (Mains
    attribute set)
    @param attr_list - attribute list name
    @param voltage - pointer to variable to store MainsVoltage attribute
    @param frequency - pointer to variable to store MainsFrequency attribute
    @param alarm_mask - pointer to variable to store BatteryAlarmMask attribute
    @param voltage_min_threshold - pointer to variable to store MainsVoltageMinThreshold attribute
    @param voltage_max_threshold - pointer to variable to store MainsVoltageMaxThreshold attribute
    @param dwell_trip_point - pointer to variable to store MainsVoltageDwellTripPoint attribute
*/
#define ZB_ZCL_DECLARE_POWER_CONFIG_MAINS_ATTRIB_LIST(attr_list, voltage, frequency, alarm_mask,    \
                                                      voltage_min_threshold, voltage_max_threshold, \
                                                      dwell_trip_point)                             \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_POWER_CONFIG)         \
  ZB_ZCL_POWER_CONFIG_MAINS_ATTRIB_LIST(voltage, frequency, alarm_mask,                     \
                                              voltage_min_threshold, voltage_max_threshold, \
                                              dwell_trip_point)                             \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @cond internals_doc */
#define ZB_ZCL_POWER_CONFIG_BATTERY_ATTRIB_LIST_EXT(bat_num,                                \
    voltage, size, quantity, rated_voltage, alarm_mask, voltage_min_threshold,              \
    remaining, threshold1, threshold2, threshold3, min_threshold, percent_threshold1,       \
    percent_threshold2, percent_threshold3, alarm_state)                                    \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID(voltage, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_SIZE_ID(size, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_QUANTITY_ID(quantity, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_RATED_VOLTAGE_ID(rated_voltage, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_MASK_ID(alarm_mask, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_MIN_THRESHOLD_ID(voltage_min_threshold, bat_num), \
  /* ext attribute */  \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID(remaining, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD1_ID(threshold1, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD2_ID(threshold2, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD3_ID(threshold3, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_MIN_THRESHOLD_ID(min_threshold, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD1_ID(percent_threshold1, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD2_ID(percent_threshold2, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD3_ID(percent_threshold3, bat_num), \
  ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_STATE_ID(alarm_state, bat_num),
/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Declare attribute list for Power Configuration cluster - server side (Battery attribute
    list extended)
    @param attr_list - attribute list name
    @param voltage - pointer to variable to store BatteryVoltage attribute
    @param size - pointer to variable to store BatterySize attribute
    @param quantity - pointer to variable to store BatteryQuantity attribute
    @param rated_voltage - pointer to variable to store BatteryRatedVoltage attribute
    @param alarm_mask - pointer to variable to store BatteryAlarmMask attribute
    @param voltage_min_threshold - pointer to variable to store BatteryVoltageMinThreshold attribute

    @param remaining - pointer to variable to store BatteryPercentageRemaining attribute
    @param threshold1 - pointer to variable to store BatteryVoltageThreshold1 attribute
    @param threshold2 - pointer to variable to store BatteryVoltageThreshold2 attribute
    @param threshold3 - pointer to variable to store BatteryVoltageThreshold3 attribute
    @param min_threshold - pointer to variable to store BatteryPercentageMinThreshold attribute
    @param percent_threshold1 - pointer to variable to store BatteryPercentageThreshold1 attribute
    @param percent_threshold2 - pointer to variable to store BatteryPercentageThreshold2 attribute
    @param percent_threshold3 - pointer to variable to store BatteryPercentageThreshold3 attribute
    @param alarm_state - pointer to variable to store BatteryAlarmState attribute
*/
#define ZB_ZCL_DECLARE_POWER_CONFIG_BATTERY_ATTRIB_LIST_EXT(attr_list,                      \
    voltage, size, quantity, rated_voltage, alarm_mask, voltage_min_threshold,              \
    remaining, threshold1, threshold2, threshold3, min_threshold, percent_threshold1,       \
    percent_threshold2, percent_threshold3, alarm_state)                                    \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_POWER_CONFIG)         \
  ZB_ZCL_POWER_CONFIG_BATTERY_ATTRIB_LIST_EXT(bat_num,                                      \
    voltage, size, quantity, rated_voltage, alarm_mask, voltage_min_threshold,              \
    remaining, threshold1, threshold2, threshold3, min_threshold, percent_threshold1,       \
    percent_threshold2, percent_threshold3, alarm_state)                                    \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Power Configuration cluster attributes */

/*! @name Power Configuration cluster commands
    @{
*/

/*! @} */ /* Power Configuration cluster commands */

/*! @} */ /* ZCL Power Configuration cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_power_config_init_server(void);
void zb_zcl_power_config_init_client(void);
#define ZB_ZCL_CLUSTER_ID_POWER_CONFIG_SERVER_ROLE_INIT zb_zcl_power_config_init_server
#define ZB_ZCL_CLUSTER_ID_POWER_CONFIG_CLIENT_ROLE_INIT zb_zcl_power_config_init_client

#endif /* ZB_ZCL_POWER_CONFIG_H */
