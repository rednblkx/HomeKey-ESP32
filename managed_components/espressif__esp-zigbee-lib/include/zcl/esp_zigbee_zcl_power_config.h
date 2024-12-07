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

/** @brief Power configuration cluster information attribute set identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID                 = 0x0000,         /*!< MainsVoltage attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID               = 0x0001,         /*!< MainsFrequency attribute */

    ESP_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_ALARM_MASK_ID              = 0x0010,         /*！< MainsAlarmMask attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD      = 0x0011,         /*！< MainsVoltageMinThreshold attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD      = 0x0012,         /*！< MainsVoltageMaxThreshold attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_DWELL_TRIP_POINT           = 0x0013,         /*！< MainsVoltageDwellTripPoint attribute */

    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID               = 0x0020,         /*!< BatteryVoltage attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID  = 0x0021,         /*!< BatteryPercentageRemaining attribute */

    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_MANUFACTURER_ID          = 0x0030,         /*!< Name of the battery manufacturer as a character string. */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_SIZE_ID                  = 0x0031,         /*!< BatterySize attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_A_HR_RATING_ID           = 0x0032,         /*!< The Ampere-hour rating of the battery, measured in units of 10mAHr. */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_QUANTITY_ID              = 0x0033,         /*!< BatteryQuantity attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_RATED_VOLTAGE_ID         = 0x0034,         /*!< BatteryRatedVoltage attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_MASK_ID            = 0x0035,         /*!< BatteryAlarmMask attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_MIN_THRESHOLD_ID = 0x0036,         /*!< BatteryVoltageMinThreshold attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD1_ID        = 0x0037,     /*!< BatteryVoltageThreshold1 attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD2_ID        = 0x0038,     /*!< BatteryVoltageThreshold2 attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD3_ID        = 0x0039,     /*!< BatteryVoltageThreshold3 attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_MIN_THRESHOLD_ID  = 0x003a,     /*!< BatteryPercentageMinThreshold attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD1_ID     = 0x003b,     /*!< BatteryPercentageThreshold1 attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD2_ID     = 0x003c,     /*!< BatteryPercentageThreshold2 attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD3_ID     = 0x003d,     /*!< BatteryPercentageThreshold3 attribute */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_STATE_ID               = 0x003e,     /*!< BatteryAlarmState attribute */

    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_ID                    = 0x0040,   /*!< Battery Information 2 attribute set */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_REMAINING_ID       = 0x0041,

    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_MANUFACTURER_ID               = 0x0050,   /*!< Battery Settings 2 attribute set */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_SIZE_ID                       = 0x0051,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_A_HR_RATING_ID                = 0x0052,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_QUANTITY_ID                   = 0x0053,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_RATED_VOLTAGE_ID              = 0x0054,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_ALARM_MASK_ID                 = 0x0055,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_MIN_THRESHOLD_ID      = 0x0056,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD1_ID         = 0x0057,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD2_ID         = 0x0058,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD3_ID         = 0x0059,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_MIN_THRESHOLD_ID   = 0x005a,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD1_ID      = 0x005b,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD2_ID      = 0x005c,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD3_ID      = 0x005d,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_ALARM_STATE_ID                = 0x005e,

    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_ID                    = 0x0060,   /*!< Battery Information 3 attribute set */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_REMAINING_ID       = 0x0061,

    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_MANUFACTURER_ID               = 0x0070,   /*!< Battery Settings 3 attribute set */
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_SIZE_ID                       = 0x0071,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_A_HR_RATING_ID                = 0x0072,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_QUANTITY_ID                   = 0x0073,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_RATED_VOLTAGE_ID              = 0x0074,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_ALARM_MASK_ID                 = 0x0075,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_MIN_THRESHOLD_ID      = 0x0076,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD1_ID         = 0x0077,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD2_ID         = 0x0078,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD3_ID         = 0x0079,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_MIN_THRESHOLD_ID   = 0x007a,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD1_ID      = 0x007b,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD2_ID      = 0x007c,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD3_ID      = 0x007d,
    ESP_ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_ALARM_STATE_ID                = 0x007e,
} esp_zb_zcl_power_config_attr_t;

/**
 * @brief Power Configuration MainsAlarmMask value
 */
typedef enum {
    ESP_ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_VOLTAGE_LOW = 0x01,          /*!< MainsAlarmMask - Mains Voltage too low */
    ESP_ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_VOLTAGE_HIGH = 0x02,         /*!< MainsAlarmMask - Mains Voltage too high */
    ESP_ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_VOLTAGE_UNAVAIL = 0x04,      /*!< MainsAlarmMask - Mains power supply lost/unavailable */
} esp_zb_zcl_power_config_mains_alarm_mask_t;

/**
 * @brief Power Configuration BatterySize value
 */
typedef enum {
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_NO_BATTERY = 0,    /*!< BatterySize - no battery*/
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_BUILT_IN   = 1,    /*!< BatterySize - built in */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_OTHER      = 2,    /*!< BatterySize - other */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_AA         = 3,    /*!< BatterySize - AA */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_AAA        = 4,    /*!< BatterySize - AAA */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_C          = 5,    /*!< BatterySize - C */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_D          = 6,    /*!< BatterySize - D */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_CR2        = 7,    /*!< BatterySize - CR2 */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_CR123A     = 8,    /*!< BatterySize - CR123A */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_SIZE_UNKNOWN    = 0xff, /*!< BatterySize - unknown */
} esp_zb_zcl_power_config_battery_size_t;

/**
 * @brief Power Configuration BatteryAlarmMask value
 */
typedef enum {
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_VOLTAGE_LOW = 0,     /*!< BatteryAlarmMask - Battery voltage too low */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_ALARM1 = 1,          /*!< BatteryAlarmMask - Alarm1 */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_ALARM2 = 2,          /*!< BatteryAlarmMask - Alarm2 */
    ESP_ZB_ZCL_POWER_CONFIG_BATTERY_ALARM_MASK_ALARM3 = 3,          /*!< BatteryAlarmMask - Alarm3 */
} esp_zb_zcl_power_config_battery_alarm_mask_t;

/** @brief Default value for MainsAlarmMask attribute */
#define ESP_ZB_ZCL_POWER_CONFIG_MAINS_ALARM_MASK_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for MainsVoltageMinThreshold attribute */
#define ESP_ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for MainsVoltageMaxThreshold attribute */
#define ESP_ZB_ZCL_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD_DEFAULT_VALUE ((uint16_t)0xFFFF)

/** @brief Default value for MainsDwellTripPoint attribute */
#define ESP_ZB_ZCL_POWER_CONFIG_MAINS_DWELL_TRIP_POINT_DEFAULT_VALUE ((uint16_t)0x0000)

#ifdef __cplusplus
}
#endif
