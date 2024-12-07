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

/** @brief Door Lock cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID                       = 0x0000,  /*!< brief LockState attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID                        = 0x0001,  /*!< brief LockType attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID                 = 0x0002,  /*!< brief ActuatorEnabled attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_ID                       = 0x0003,  /*!< brief DoorState attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUM_OF_DOOR_OPEN_EVENTS_ID          = 0x0004,  /*!< brief DoorOpenEvents attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUM_OF_DOOR_CLOSED_EVENTS_ID        = 0x0005,  /*!< brief DoorClosedEvents attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_OPEN_PERIOD_ID                      = 0x0006,  /*!< brief OpenPeriod attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUMBER_OF_LOG_RECORDS_SUPPORTED_ID  = 0x0010,  /*!< The number of available log records. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUM_TOTAL_USERS_ID                  = 0x0011,  /*!< brief NumberOfTotalUsersSupported attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUM_PIN_USERS_ID                    = 0x0012,  /*!< brief NumberOfPINUsersSupported attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUMBER_OF_RFID_USERS_SUPPORTED_ID   = 0x0013,  /*!< he number of RFID users supported. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUM_WEEK_DAY_SCHEDULE_PER_USER_ID   = 0x0014,  /*!< brief NumberOfWeekDaySchedulesSupportedPerUser attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUM_YEAR_DAY_SCHEDULE_PER_USER_ID   = 0x0015,  /*!< brief NumberOfYearDaySchedulesSupportedPerUser attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_NUM_HOLIDAY_SCHEDULE_ID             = 0x0016,  /*!< brief NumberOfHolidaySchedulesSupported attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_MAX_PIN_LEN_ID                      = 0x0017,  /*!< brief Max PIN code length attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_MIN_PIN_LEN_ID                      = 0x0018,  /*!< brief Min PIN code length attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_MAX_RFID_CODE_LENGTH_ID             = 0x0019,  /*!< An 8-bit value indicates the maximum length in bytes of a RFID Code on this device. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_MIN_RFID_CODE_LENGTH_ID             = 0x001A,  /*!< An 8-bit value indicates the minimum length in bytes of a RFID Code on this device. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_LOGGING_ID                   = 0x0020,  /*!< Enable/disable event logging. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_LANGUAGE_ID                         = 0x0021,  /*!< Modifies the language for the on-screen or audible user interface using three bytes from ISO-639-1. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_LED_SETTINGS_ID                     = 0x0022,  /*!< The settings for the LED support three different modes. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_AUTO_RELOCK_TIME_ID                 = 0x0023,  /*!< The number of seconds to wait after unlocking a lock before it automatically locks again. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_SOUND_VOLUME_ID                     = 0x0024,  /*!< The sound volume on a door lock has three possible settings: silent, low and high volumes. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_OPERATING_MODE_ID                   = 0x0025,  /*!<  OperatingMode attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_OPERATION_MODES_SUPPORTED_ID        = 0x0026,  /*!< SupportedOperatingModes attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_DEFAULT_CONFIGURATION_REGISTER_ID   = 0x0027,  /*!< This attribute represents the default configurations as they are physically set on the device */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_LOCAL_PROGRAMMING_ID         = 0x0028,  /*!< EnableLocalProgramming attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_ONE_TOUCH_LOCKING_ID         = 0x0029,  /*!< Enable/disable the ability to lock the door lock with a single touch on the door lock. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_INSIDE_STATUS_LED_ID         = 0x002A,  /*!< Enable/disable an inside LED that allows the user to see at a glance if the door is locked. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_PRIVACY_MODE_BUTTON_ID       = 0x002B,  /*!< Enable/disable a button inside the door that is used to put the lock into privacy mode. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_WRONG_CODE_ENTRY_LIMIT_ID           = 0x0030,  /*!< The number of incorrect codes or RFID presentment attempts a user is allowed to enter before the door will enter a lockout state. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_USER_CODE_TEMPORARY_DISABLE_TIME_ID = 0x0031,  /*!< The number of seconds that the lock shuts down following wrong code entry. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_SEND_PIN_OVER_THE_AIR_ID            = 0x0032,  /*!< Boolean set to True if it is ok for the door lock server to send PINs over the air. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_REQUIRE_PIN_RF_ID                   = 0x0033,  /*!< Require PIN for RF operation attribute */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_SECURITY_LEVEL_ID                   = 0x0034,  /*!< It allows the door lock manufacturer to indicate what level of security the door lock requires. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_ALARM_MASK_ID                       = 0x0040,  /*!< The alarm mask is used to turn on/off alarms for particular functions */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_KEYPAD_OPERATION_EVENT_MASK_ID      = 0x0041,  /*!< Event mask used to turn on and off the transmission of keypad operation events. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_RF_OPERATION_EVENT_MASK_ID          = 0x0042,  /*!< Event mask used to turn on and off the transmission of RF operation events. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_ID      = 0x0043,  /*!< Event mask used to turn on and off manual operation events. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_RFID_OPERATION_EVENT_MASK_ID        = 0x0044,  /*!< Event mask used to turn on and off RFID operation events. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_KEYPAD_PROGRAMMING_EVENT_MASK_ID    = 0x0045,  /*!< Event mask used to turn on and off keypad programming events. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_RF_PROGRAMMING_EVENT_MASK_ID        = 0x0046,  /*!< Event mask used to turn on and off RF programming events. */
    ESP_ZB_ZCL_ATTR_DOOR_LOCK_RFID_PROGRAMMING_EVENT_MASK_ID      = 0x0047,  /*!< Event mask used to turn on and off RFID programming events. */
} esp_zb_zcl_door_lock_attr_t;

/** @brief Default value for Lock State attribute. */
#define ESP_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_DEFAULT_VALUE 0xff

/** @brief Default value for Lock Type attribute. */
#define ESP_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_DEFAULT_VALUE   0xff

/** @brief Default value for Actuator Enabled attribute. */
#define ESP_ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_DEFAULT_VALUE 0x01

/** @brief Door Lock cluster commands
*/
typedef enum {
    ESP_ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR                        = 0x00,  /*!< This command causes the lock device to lock the door. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR                      = 0x01,  /*!< This command causes the lock device to unlock the door. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_TOGGLE                           = 0x02,  /*!< Request the status of the lock.  */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_WITH_TIMEOUT              = 0x03,  /*!< This command causes the lock device to unlock the door with a timeout parameter. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_LOG_RECORD                   = 0x04,  /*!< Request a log record. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_PIN_CODE                     = 0x05,  /*!< Set a PIN into the lock. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_PIN_CODE                     = 0x06,  /*!< Retrieve a PIN Code. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_PIN_CODE                   = 0x07,  /*!< Delete a PIN. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_PIN_CODES              = 0x08,  /*!< Clear out all PINs on the lock. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_USER_STATUS                  = 0x09,  /*!< Set the status of a user ID. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_USER_STATUS                  = 0x0A,  /*!< Get the status of a user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_WEEKDAY_SCHEDULE             = 0x0B,  /*!< Set a weekly repeating schedule for a specified user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_WEEKDAY_SCHEDULE             = 0x0C,  /*!< Retrieve the specific weekly schedule for the specific user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_WEEKDAY_SCHEDULE           = 0x0D,  /*!< Clear the specific weekly schedule for the specific user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_YEAR_DAY_SCHEDULE            = 0x0E,  /*!< Set a time-specific schedule ID for a specified user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_YEAR_DAY_SCHEDULE            = 0x0F,  /*!< Retrieve the specific year day schedule for the specific user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_YEAR_DAY_SCHEDULE          = 0x10,  /*!< Clears the specific year day schedule for the specific user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_HOLIDAY_SCHEDULE             = 0x11,  /*!< Set the holiday Schedule by specifying local start time and local end time with respect to any Lock Operating Mode. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_HOLIDAY_SCHEDULE             = 0x12,  /*!< Get the holiday Schedule by specifying Holiday ID. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_HOLIDAY_SCHEDULE           = 0x13,  /*!< Clear the holiday Schedule by specifying Holiday ID. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_USER_TYPE                    = 0x14,  /*!< Set the type byte for a specified user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_USER_TYPE                    = 0x15,  /*!< Retrieve the type byte for a specific user. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_RFID_CODE                    = 0x16,  /*!< Set an ID for RFID access into the lock. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_RFID_CODE                    = 0x17,  /*!< Retrieve an ID. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_RFID_CODE                  = 0x18,  /*!< Delete an ID. */
    ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_RFID_CODES             = 0x19,  /*!< Clear out all RFIDs on the lock. */
} esp_zb_zcl_door_lock_cmd_id_t;

/** @brief Door Lock cluster response commands
*/
typedef enum {
  ESP_ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR_RES                    = 0x00, /*!< This command is sent in response to a Lock command */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR_RES                  = 0x01, /*!< This command is sent in response to a Unlock command */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_TOGGLE_RESPONSE                  = 0x02, /*!< This command is sent in response to a Toggle command*/
  ESP_ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_WITH_TIMEOUT_RESPONSE     = 0x03, /*!< This command is sent in response to an Unlock  */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_LOG_RECORD_RESPONSE          = 0x04, /*!< Returns the specified log record. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_PIN_CODE_RESPONSE            = 0x05, /*!< Returns status of the PIN set command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_PIN_CODE_RESPONSE            = 0x06, /*!< Returns the PIN for the specified user ID. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_PIN_CODE_RESPONSE          = 0x07, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_PIN_CODES_RESPONSE     = 0x08, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_USER_STATUS_RESPONSE         = 0x09, /*!< Returns the pass or fail value for the setting of the user status. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_USER_STATUS_RESPONSE         = 0x0A, /*!< Returns the user status for the specified user ID. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_WEEKDAY_SCHEDULE_RESPONSE    = 0x0B, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_WEEKDAY_SCHEDULE_RESPONSE    = 0x0C, /*!< Returns the weekly repeating schedule data for the specified schedule ID. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_WEEKDAY_SCHEDULE_RESPONSE  = 0x0D, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_YEAR_DAY_SCHEDULE_RESPONSE   = 0x0E, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_YEAR_DAY_SCHEDULE_RESPONSE   = 0x0F, /*!< Returns the weekly repeating schedule data for the specified schedule ID. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_YEAR_DAY_SCHEDULE_RESPONSE = 0x10, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_HOLIDAY_SCHEDULE_RESPONSE    = 0x11, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_HOLIDAY_SCHEDULE_RESPONSE    = 0x12, /*!< Returns the Holiday Schedule Entry for the specified Holiday ID. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_HOLIDAY_SCHEDULE_RESPONSE  = 0x13, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_USER_TYPE_RESPONSE           = 0x14, /*!< Returns the pass or fail value for the setting of the user type. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_USER_TYPE_RESPONSE           = 0x15, /*!< Returns the user type for the specified user ID. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_SET_RFID_CODE_RESPONSE           = 0x16, /*!< Returns status of the Set RFID Code command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_GET_RFID_CODE_RESPONSE           = 0x17, /*!< Returns the RFID code for the specified user ID. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_RFID_CODE_RESPONSE         = 0x18, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_RFID_CODES_RESPONSE    = 0x19, /*!< Returns pass/fail of the command. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_OPERATION_EVENT_NOTIFICATION_ID  = 0x20, /*!< The door lock server sends out operation event notification when the event is triggered by the various event sources. */
  ESP_ZB_ZCL_CMD_DOOR_LOCK_PROGRAMMING_EVENT_NOTIFICATION   = 0x21, /*!< The door lock server sends out a programming event notification whenever a programming event takes place on the door lock. */
} esp_zb_zcl_door_lock_cmd_resp_t;

#ifdef __cplusplus
}
#endif
