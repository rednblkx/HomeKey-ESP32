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
/* PURPOSE: DOOR_LOCK cluster definitions
*/

#ifndef ZB_ZCL_DOOR_LOCK_H
#define ZB_ZCL_DOOR_LOCK_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_DOOR_LOCK
 *  @{
 *    @details
 *    Door Lock cluster defined in ZCL spec, clause 7.3.
 *
 *    The cluster contains 2 command sets:
 *    @li Lock Door request and response;
 *    @li Unlock Door request and response.
 *
 *    Both requests have no payload and could be scheduled for sending with corresponding macros.
 *    Both responses have simple payload, and their sending and parsing with corresponding macros
 *    is almost trivial.
 *
 *    @par Examples:
 *
 *    Send Door Lock command:
 *    @snippet HA_samples/door_lock/sample_zed.c send_door_lock_req
 *
 *    Send Door Unlock command:
 *    @snippet HA_samples/door_lock/sample_zed.c send_door_lock_unlock_req
 *
 */

/*! @name Door Lock cluster attributes
    @{
*/

/*! @brief Door Lock cluster attribute identifiers
    @see ZCL spec, subclause 7.3.2.2
*/
enum zb_zcl_door_lock_attr_e
{
  /** @brief LockState attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID                = 0x0000,
  /** @brief LockType attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID                 = 0x0001,
  /** @brief ActuatorEnabled attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID          = 0x0002,
  /** @brief DoorState attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_ID                = 0x0003,
  /** @brief DoorOpenEvents attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_NUM_OF_DOOR_OPEN_EVENTS_ID   = 0x0004,
  /** @brief DoorClosedEvents attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_NUM_OF_DOOR_CLOSED_EVENTS_ID = 0x0005,
  /** @brief OpenPeriod attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_OPEN_PERIOD_ID                    = 0x0006,
  /** The number of available log records. */
  ZB_ZCL_ATTR_DOOR_LOCK_NUMBER_OF_LOG_RECORDS_SUPPORTED_ID  = 0x0010,
  /** @brief NumberOfTotalUsersSupported attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_NUM_TOTAL_USERS_ID                = 0x0011,
  /** @brief NumberOfPINUsersSupported attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_NUM_PIN_USERS_ID                  = 0x0012,
  /** The number of RFID users supported. */
  ZB_ZCL_ATTR_DOOR_LOCK_NUMBER_OF_RFID_USERS_SUPPORTED_ID   = 0x0013,
  /** @brief NumberOfWeekDaySchedulesSupportedPerUser attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_NUM_WEEK_DAY_SCHEDULE_PER_USER_ID = 0x0014,
  /** @brief NumberOfYearDaySchedulesSupportedPerUser attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_NUM_YEAR_DAY_SCHEDULE_PER_USER_ID = 0x0015,
  /** @brief NumberOfHolidaySchedulesSupported attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_NUM_HOLIDAY_SCHEDULE_ID           = 0x0016,
  /** @brief Max PIN code length attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_MAX_PIN_LEN_ID                    = 0x0017,
  /** @brief Min PIN code length attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_MIN_PIN_LEN_ID                    = 0x0018,
  /** An 8-bit value indicates the maximum length in bytes of a RFID Code
   *  on this device. */
  ZB_ZCL_ATTR_DOOR_LOCK_MAX_RFID_CODE_LENGTH_ID             = 0x0019,
  /** An 8-bit value indicates the minimum length in bytes of a RFID Code
   *  on this device. */
  ZB_ZCL_ATTR_DOOR_LOCK_MIN_RFID_CODE_LENGTH_ID             = 0x001A,
  /** Enable/disable event logging. */
  ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_LOGGING_ID                   = 0x0020,
  /** Modifies the language for the on-screen or audible user interface using
   *  three bytes from ISO-639-1. */
  ZB_ZCL_ATTR_DOOR_LOCK_LANGUAGE_ID                         = 0x0021,
  /** The settings for the LED support three different modes. */
  ZB_ZCL_ATTR_DOOR_LOCK_LED_SETTINGS_ID                     = 0x0022,
  /** The number of seconds to wait after unlocking a lock before it automatically
   *  locks again. */
  ZB_ZCL_ATTR_DOOR_LOCK_AUTO_RELOCK_TIME_ID                 = 0x0023,
  /** The sound volume on a door lock has three possible settings: silent, low
   *  and high volumes. */
  ZB_ZCL_ATTR_DOOR_LOCK_SOUND_VOLUME_ID                     = 0x0024,
  /** @brief OperatingMode attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_OPERATING_MODE_ID                 = 0x0025,
  /** @brief SupportedOperatingModes attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_OPERATION_MODES_SUPPORTED_ID      = 0x0026,
  /** This attribute represents the default configurations as they are physically
   *  set on the device (example: hardware dip switch setting, etc) and represents
   *  the default setting for some of the attributes within this Operational Setting
   *  Attribute Set (for example: LED, Auto Lock, Sound Volume, and Operating Mode
   *  attributes). */
  ZB_ZCL_ATTR_DOOR_LOCK_DEFAULT_CONFIGURATION_REGISTER_ID   = 0x0027,
  /** @brief EnableLocalProgramming attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_LOCAL_PROGRAMMING_ID       = 0x0028,
  /** Enable/disable the ability to lock the door lock with a single touch on
   *  the door lock. */
  ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_ONE_TOUCH_LOCKING_ID         = 0x0029,
  /** Enable/disable an inside LED that allows the user to see at a glance
   *  if the door is locked. */
  ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_INSIDE_STATUS_LED_ID         = 0x002A,
  /** Enable/disable a button inside the door that is used to put the lock
   *  into privacy mode. */
  ZB_ZCL_ATTR_DOOR_LOCK_ENABLE_PRIVACY_MODE_BUTTON_ID       = 0x002B,
  /** The number of incorrect codes or RFID presentment attempts a user
   *  is allowed to enter before the door will enter a lockout state. */
  ZB_ZCL_ATTR_DOOR_LOCK_WRONG_CODE_ENTRY_LIMIT_ID           = 0x0030,
  /** The number of seconds that the lock shuts down following wrong code entry. */
  ZB_ZCL_ATTR_DOOR_LOCK_USER_CODE_TEMPORARY_DISABLE_TIME_ID = 0x0031,
  /** Boolean set to True if it is ok for the door lock server to send PINs
   *  over the air. */
  ZB_ZCL_ATTR_DOOR_LOCK_SEND_PIN_OVER_THE_AIR_ID            = 0x0032,
  /** @brief Require PIN for RF operation attribute */
  ZB_ZCL_ATTR_DOOR_LOCK_REQUIRE_PIN_RF_ID                 = 0x0033,
  /** The Security Level attribute allows the door lock manufacturer to indicate
   *  what level of security the door lock requires. */
  ZB_ZCL_ATTR_DOOR_LOCK_SECURITY_LEVEL_ID                   = 0x0034,
  /** The alarm mask is used to turn on/off alarms for particular functions */
  ZB_ZCL_ATTR_DOOR_LOCK_ALARM_MASK_ID                       = 0x0040,
  /** Event mask used to turn on and off the transmission of keypad operation events. */
  ZB_ZCL_ATTR_DOOR_LOCK_KEYPAD_OPERATION_EVENT_MASK_ID      = 0x0041,
  /** Event mask used to turn on and off the transmission of RF operation events. */
  ZB_ZCL_ATTR_DOOR_LOCK_RF_OPERATION_EVENT_MASK_ID          = 0x0042,
  /** Event mask used to turn on and off manual operation events. */
  ZB_ZCL_ATTR_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_ID      = 0x0043,
  /** Event mask used to turn on and off RFID operation events. */
  ZB_ZCL_ATTR_DOOR_LOCK_RFID_OPERATION_EVENT_MASK_ID        = 0x0044,
  /** Event mask used to turn on and off keypad programming events. */
  ZB_ZCL_ATTR_DOOR_LOCK_KEYPAD_PROGRAMMING_EVENT_MASK_ID    = 0x0045,
  /** Event mask used to turn on and off RF programming events. */
  ZB_ZCL_ATTR_DOOR_LOCK_RF_PROGRAMMING_EVENT_MASK_ID        = 0x0046,
  /** Event mask used to turn on and off RFID programming events. */
  ZB_ZCL_ATTR_DOOR_LOCK_RFID_PROGRAMMING_EVENT_MASK_ID      = 0x0047,
};


/** @brief Values for LockState attribute
  * @see ZCL spec, subclause 7.3.2.2.1
  */
enum zb_zcl_door_lock_lock_state_e
{
  /*! "Not fully locked" value */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_NOT_FULLY_LOCKED  = 0x00,
  /*! "Locked" value */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_LOCKED            = 0x01,
  /*! "Unlocked" value */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_UNLOCKED          = 0x02,
  /*! "Not Defined" value */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_UNDEFINED         = 0xff
};

/** @brief Default value for Door Lock cluster revision global attribute */
#define ZB_ZCL_DOOR_LOCK_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/** @brief Default value for Lock State attribute. */
#define ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_DEFAULT_VALUE ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_UNDEFINED

/** @brief Values for LockType attribute
  * @see ZCL spec, subclause 7.3.2.2.2
  */
enum zb_zcl_door_lock_lock_type_e
{
  /*! "Deadbolt" value */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_DEADBOLT     = 0x00,
  /*! "Magnetic" value */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_MAGNETIC     = 0x01,
  /*! "Other" value */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_OTHER        = 0x02,
  /*! "Not Defined" value */
  ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_UNDEFINED    = 0xff
};

/** @brief Default value for Lock Type attribute. */
#define ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_DEFAULT_VALUE   ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_UNDEFINED

/** @brief Values for ActuatorEnabled attribute
  * @see ZCL spec, subclause 7.3.2.2.3
  */
enum zb_zcl_door_lock_actuator_enabled_e
{
  /*! "Disabled" value */
  ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_DISABLED  = 0x0b0,
  /*! "Enabled" value */
  ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ENABLED   = 0x0b1
};

/** @brief Default value for Actuator Enabled attribute. */
#define ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_DEFAULT_VALUE \
  ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ENABLED

/** @brief Values for DoorState attribute
  * @see ZCL spec, subclause 7.3.2.2.4
  */
enum zb_zcl_door_lock_door_state_e
{
  /*! "Open" value */
  ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_OPEN              = 0x00,
  /*! "Closed" value */
  ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_CLOSED            = 0x01,
  /*! "Error (jammed)" value */
  ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_ERROR_JAMMED      = 0x02,
  /*! "Error (forced open)" value */
  ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_ERROR_FORCED_OPEN = 0x03,
  /*! "Error (unspecified)" value */
  ZB_ZCL_ATTR_DOOR_LOCK_DOOR_STATE_ERROR_UNSPECIFIED = 0x04
};

/** @brief Declare attribute list for Door Lock cluster
    @param attr_list - attribute list name
    @param lock_state
    @param lock_type
    @param actuator_enabled
*/
#define ZB_ZCL_DECLARE_DOOR_LOCK_ATTRIB_LIST(attr_list,                               \
                                             lock_state,                              \
                                             lock_type,                               \
                                             actuator_enabled)                        \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_DOOR_LOCK)      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID, (lock_state))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID, (lock_type))               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID, (actuator_enabled)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Door Lock cluster attributes */

/*! @name Door Lock cluster commands
    @{
*/

/*! @brief Door Lock cluster commands
    @see ZCL spec, subclause 7.3.2.3 Commands Received
*/
enum zb_zcl_door_lock_cmd_e
{
  /** This command causes the lock device to lock the door. */
  ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR      = 0x00,
  /** This command causes the lock device to unlock the door. */
  ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR    = 0x01,
  /** Request the status of the lock. */
  ZB_ZCL_CMD_DOOR_LOCK_TOGGLE                           = 0x02,
  /** This command causes the lock device to unlock the door with a timeout parameter. */
  ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_WITH_TIMEOUT              = 0x03,
  /** Request a log record. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_LOG_RECORD                   = 0x04,
  /** Set a PIN into the lock. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_PIN_CODE                     = 0x05,
  /** Retrieve a PIN Code. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_PIN_CODE                     = 0x06,
  /** Delete a PIN. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_PIN_CODE                   = 0x07,
  /** Clear out all PINs on the lock. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_PIN_CODES              = 0x08,
  /** Set the status of a user ID. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_USER_STATUS                  = 0x09,
  /** Get the status of a user. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_USER_STATUS                  = 0x0A,
  /** Set a weekly repeating schedule for a specified user. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_WEEKDAY_SCHEDULE             = 0x0B,
  /** Retrieve the specific weekly schedule for the specific user. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_WEEKDAY_SCHEDULE             = 0x0C,
  /** Clear the specific weekly schedule for the specific user. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_WEEKDAY_SCHEDULE           = 0x0D,
  /** Set a time-specific schedule ID for a specified user. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_YEAR_DAY_SCHEDULE            = 0x0E,
  /** Retrieve the specific year day schedule for the specific user. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_YEAR_DAY_SCHEDULE            = 0x0F,
  /** Clears the specific year day schedule for the specific user. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_YEAR_DAY_SCHEDULE          = 0x10,
  /** Set the holiday Schedule by specifying local start time and local end time
   *  with respect to any Lock Operating Mode. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_HOLIDAY_SCHEDULE             = 0x11,
  /** Get the holiday Schedule by specifying Holiday ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_HOLIDAY_SCHEDULE             = 0x12,
  /** Clear the holiday Schedule by specifying Holiday ID. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_HOLIDAY_SCHEDULE           = 0x13,
  /** Set the type byte for a specified user. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_USER_TYPE                    = 0x14,
  /** Retrieve the type byte for a specific user. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_USER_TYPE                    = 0x15,
  /** Set an ID for RFID access into the lock. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_RFID_CODE                    = 0x16,
  /** Retrieve an ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_RFID_CODE                    = 0x17,
  /** Delete an ID. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_RFID_CODE                  = 0x18,
  /** Clear out all RFIDs on the lock. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_RFID_CODES             = 0x19,
};

/*! @brief Door Lock cluster commands response
    @see ZCL spec, subclause 7.3.2.4 Commands Generated
*/
enum zb_zcl_door_lock_cmd_resp_e
{
  /** This command is sent in response to a Lock command with one status byte payload. */
  ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR_RES    = 0x00,
  /** This command is sent in response to a Unlock command with one status byte payload. */
  ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR_RES                 = 0x01,
  /** This command is sent in response to a Toggle command with one status byte payload. */
  ZB_ZCL_CMD_DOOR_LOCK_TOGGLE_RESPONSE                  = 0x02,
  /** This command is sent in response to an Unlock with Timeout command
   *  with one status byte payload. */
  ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_WITH_TIMEOUT_RESPONSE     = 0x03,
  /** Returns the specified log record. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_LOG_RECORD_RESPONSE          = 0x04,
  /** Returns status of the PIN set command. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_PIN_CODE_RESPONSE            = 0x05,
  /** Returns the PIN for the specified user ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_PIN_CODE_RESPONSE            = 0x06,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_PIN_CODE_RESPONSE          = 0x07,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_PIN_CODES_RESPONSE     = 0x08,
  /** Returns the pass or fail value for the setting of the user status. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_USER_STATUS_RESPONSE         = 0x09,
  /** Returns the user status for the specified user ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_USER_STATUS_RESPONSE         = 0x0A,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_WEEKDAY_SCHEDULE_RESPONSE    = 0x0B,
  /** Returns the weekly repeating schedule data for the specified schedule ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_WEEKDAY_SCHEDULE_RESPONSE    = 0x0C,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_WEEKDAY_SCHEDULE_RESPONSE  = 0x0D,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_YEAR_DAY_SCHEDULE_RESPONSE   = 0x0E,
  /** Returns the weekly repeating schedule data for the specified schedule ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_YEAR_DAY_SCHEDULE_RESPONSE   = 0x0F,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_YEAR_DAY_SCHEDULE_RESPONSE = 0x10,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_HOLIDAY_SCHEDULE_RESPONSE    = 0x11,
  /** Returns the Holiday Schedule Entry for the specified Holiday ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_HOLIDAY_SCHEDULE_RESPONSE    = 0x12,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_HOLIDAY_SCHEDULE_RESPONSE  = 0x13,
  /** Returns the pass or fail value for the setting of the user type. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_USER_TYPE_RESPONSE           = 0x14,
  /** Returns the user type for the specified user ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_USER_TYPE_RESPONSE           = 0x15,
  /** Returns status of the Set RFID Code command. */
  ZB_ZCL_CMD_DOOR_LOCK_SET_RFID_CODE_RESPONSE           = 0x16,
  /** Returns the RFID code for the specified user ID. */
  ZB_ZCL_CMD_DOOR_LOCK_GET_RFID_CODE_RESPONSE           = 0x17,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_RFID_CODE_RESPONSE         = 0x18,
  /** Returns pass/fail of the command. */
  ZB_ZCL_CMD_DOOR_LOCK_CLEAR_ALL_RFID_CODES_RESPONSE    = 0x19,
  /** The door lock server sends out operation event notification when the
   *  event is triggered by the various event sources. */
  ZB_ZCL_CMD_DOOR_LOCK_OPERATION_EVENT_NOTIFICATION_ID = 0x20,
  /** The door lock server sends out a programming event notification
   *   whenever a programming event takes place on the door lock. */
  ZB_ZCL_CMD_DOOR_LOCK_PROGRAMMING_EVENT_NOTIFICATION   = 0x21,
};

enum zb_zcl_door_lock_operation_event_source_e
{
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_SOURCE_KEYPAD        = 0x00,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_SOURCE_RF            = 0x01,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_SOURCE_MANUAL        = 0x02,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_SOURCE_RFID          = 0x03,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_SOURCE_INDETERMINATE = 0xFF,
};

/** @cond internals_doc */
/* Door lock cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_DOOR_LOCK_SERVER_ROLE_GENERATED_CMD_LIST                     \
                                                  ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR_RES,  \
                                                  ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR_RES

#define ZB_ZCL_CLUSTER_ID_DOOR_LOCK_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_DOOR_LOCK_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_DOOR_LOCK_CLIENT_ROLE_GENERATED_CMD_LIST                     \
                                                  ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR,      \
                                                  ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR

#define ZB_ZCL_CLUSTER_ID_DOOR_LOCK_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_DOOR_LOCK_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Default value for NumberOfLogRecordsSupported attribute */
#define ZB_ZCL_DOOR_LOCK_NUMBER_OF_LOG_RECORDS_SUPPORTED_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for NumTotalUsers attribute */
#define ZB_ZCL_DOOR_LOCK_NUM_TOTAL_USERS_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for NumPINUsers attribute */
#define ZB_ZCL_DOOR_LOCK_NUM_PIN_USERS_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for NumberOfRFIDUsersSupported attribute */
#define ZB_ZCL_DOOR_LOCK_NUMBER_OF_RFID_USERS_SUPPORTED_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for NumWeekDaySchedulePerUser attribute */
#define ZB_ZCL_DOOR_LOCK_NUM_WEEK_DAY_SCHEDULE_PER_USER_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for NumYearDaySchedulePerUser attribute */
#define ZB_ZCL_DOOR_LOCK_NUM_YEAR_DAY_SCHEDULE_PER_USER_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for NumHolidaySchedule attribute */
#define ZB_ZCL_DOOR_LOCK_NUM_HOLIDAY_SCHEDULE_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for MaxPINLen attribute */
#define ZB_ZCL_DOOR_LOCK_MAX_PIN_LEN_DEFAULT_VALUE ((zb_uint8_t)0x08)

/** @brief Default value for MinPINLen attribute */
#define ZB_ZCL_DOOR_LOCK_MIN_PIN_LEN_DEFAULT_VALUE ((zb_uint8_t)0x04)

/** @brief Default value for MaxRFIDCodeLength attribute */
#define ZB_ZCL_DOOR_LOCK_MAX_RFID_CODE_LENGTH_DEFAULT_VALUE ((zb_uint8_t)0x14)

/** @brief Default value for MinRFIDCodeLength attribute */
#define ZB_ZCL_DOOR_LOCK_MIN_RFID_CODE_LENGTH_DEFAULT_VALUE ((zb_uint8_t)0x08)

/** @brief Default value for EnableLogging attribute */
#define ZB_ZCL_DOOR_LOCK_ENABLE_LOGGING_DEFAULT_VALUE ((zb_bool_t)0)

/** @brief Default value for Language attribute */
#define ZB_ZCL_DOOR_LOCK_LANGUAGE_DEFAULT_VALUE {0}

/** @brief Default value for LEDSettings attribute */
#define ZB_ZCL_DOOR_LOCK_LED_SETTINGS_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for AutoRelockTime attribute */
#define ZB_ZCL_DOOR_LOCK_AUTO_RELOCK_TIME_DEFAULT_VALUE ((zb_uint32_t)0)

/** @brief Default value for SoundVolume attribute */
#define ZB_ZCL_DOOR_LOCK_SOUND_VOLUME_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for OperatingMode attribute */
#define ZB_ZCL_DOOR_LOCK_OPERATING_MODE_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for OperationModesSupported attribute */
#define ZB_ZCL_DOOR_LOCK_OPERATION_MODES_SUPPORTED_DEFAULT_VALUE ((zb_uint16_t)0x0001)

/** @brief Default value for DefaultConfigurationRegister attribute */
#define ZB_ZCL_DOOR_LOCK_DEFAULT_CONFIGURATION_REGISTER_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for EnableLocalProgramming attribute */
#define ZB_ZCL_DOOR_LOCK_ENABLE_LOCAL_PROGRAMMING_DEFAULT_VALUE ((zb_bool_t)0x01)

/** @brief Default value for EnableOneTouchLocking attribute */
#define ZB_ZCL_DOOR_LOCK_ENABLE_ONE_TOUCH_LOCKING_DEFAULT_VALUE ((zb_bool_t)0)

/** @brief Default value for EnableInsideStatusLED attribute */
#define ZB_ZCL_DOOR_LOCK_ENABLE_INSIDE_STATUS_LED_DEFAULT_VALUE ((zb_bool_t)0)

/** @brief Default value for EnablePrivacyModeButton attribute */
#define ZB_ZCL_DOOR_LOCK_ENABLE_PRIVACY_MODE_BUTTON_DEFAULT_VALUE ((zb_bool_t)0)

/** @brief Default value for WrongCodeEntryLimit attribute */
#define ZB_ZCL_DOOR_LOCK_WRONG_CODE_ENTRY_LIMIT_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for UserCodeTemporaryDisableTime attribute */
#define ZB_ZCL_DOOR_LOCK_USER_CODE_TEMPORARY_DISABLE_TIME_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for SendPINOverTheAir attribute */
#define ZB_ZCL_DOOR_LOCK_SEND_PIN_OVER_THE_AIR_DEFAULT_VALUE ((zb_bool_t)0)

/** @brief Default value for RequirePIN_RF_ID attribute */
#define ZB_ZCL_DOOR_LOCK_REQUIRE_PIN_RF_ID_DEFAULT_VALUE ((zb_bool_t)0)

/** @brief Default value for SecurityLevel attribute */
#define ZB_ZCL_DOOR_LOCK_SECURITY_LEVEL_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for AlarmMask attribute */
#define ZB_ZCL_DOOR_LOCK_ALARM_MASK_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for KeypadOperationEventMask attribute */
#define ZB_ZCL_DOOR_LOCK_KEYPAD_OPERATION_EVENT_MASK_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RFOperationEventMask attribute */
#define ZB_ZCL_DOOR_LOCK_RF_OPERATION_EVENT_MASK_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for ManualOperationEventMask attribute */
#define ZB_ZCL_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RFIDOperationEventMask attribute */
#define ZB_ZCL_DOOR_LOCK_RFID_OPERATION_EVENT_MASK_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for KeypadProgrammingEventMask attribute */
#define ZB_ZCL_DOOR_LOCK_KEYPAD_PROGRAMMING_EVENT_MASK_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RFProgrammingEventMask attribute */
#define ZB_ZCL_DOOR_LOCK_RF_PROGRAMMING_EVENT_MASK_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for RFIDProgrammingEventMask attribute */
#define ZB_ZCL_DOOR_LOCK_RFID_PROGRAMMING_EVENT_MASK_DEFAULT_VALUE ((zb_uint16_t)0x0000)

enum zb_zcl_door_lock_operation_event_code_e
{
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_CODE_UNKNOWN       = 0x00,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_CODE_LOCK          = 0x01,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_CODE_UNLOCK        = 0x02,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_CODE_KEY_LOCK      = 0x08,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_CODE_KEY_UNLOCK    = 0x09,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_CODE_MANUAL_LOCK   = 0x0D,
  ZB_ZCL_DOOR_LOCK_OPERATION_EVENT_CODE_MANUAL_UNLOCK = 0x0E,
};

enum zb_zcl_door_lock_keypad_operation_event_mask_e
{
  ZB_ZCL_DOOR_LOCK_KEYPAD_OPERATION_EVENT_MASK_UNKNOWN = (1l << 0),
  ZB_ZCL_DOOR_LOCK_KEYPAD_OPERATION_EVENT_MASK_LOCK    = (1l << 1),
  ZB_ZCL_DOOR_LOCK_KEYPAD_OPERATION_EVENT_MASK_UNLOCK  = (1l << 2),
};

enum zb_zcl_door_lock_rf_operation_event_mask_e
{
  ZB_ZCL_DOOR_LOCK_RF_OPERATION_EVENT_MASK_UNKNOWN = (1l << 0),
  ZB_ZCL_DOOR_LOCK_RF_OPERATION_EVENT_MASK_LOCK    = (1l << 1),
  ZB_ZCL_DOOR_LOCK_RF_OPERATION_EVENT_MASK_UNLOCK  = (1l << 2),
};

enum zb_zcl_door_lock_manual_operation_event_mask_e
{
  ZB_ZCL_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_UNKNOWN           = (1l << 0),
  ZB_ZCL_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_THUMBTURN_LOCK    = (1l << 1),
  ZB_ZCL_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_THUMBTURN_UNLOCK  = (1l << 2),
  ZB_ZCL_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_KEY_LOCK          = (1l << 4),
  ZB_ZCL_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_KEY_UNLOCK        = (1l << 5),
  ZB_ZCL_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_MANUAL_LOCK       = (1l << 9),
  ZB_ZCL_DOOR_LOCK_MANUAL_OPERATION_EVENT_MASK_MANUAL_UNLOCK     = (1l << 10),
};


/*! @brief General macro for sending a "Door Lock" cluster command
    @param buffer to put command payload and headers to
    @param dst_addr address to send to
    @param dst_addr_mode destination address mode
    @param dst_ep destination endpoint
    @param ep source endpoint
    @param dis_default_resp "Disable default response" flag
    @param cb command send status callback
*/
#define ZB_ZCL_DOOR_LOCK_SEND_LOCK_DOOR_REQ(buffer,           \
                                            dst_addr,         \
                                            dst_addr_mode,    \
                                            dst_ep,           \
                                            ep,               \
                                            dis_default_resp, \
                                            cb)               \
  ZB_ZCL_SEND_CMD(  buffer,                                   \
                    dst_addr,                                 \
                    dst_addr_mode,                            \
                    dst_ep,                                   \
                    ep,                                       \
                    ZB_AF_HA_PROFILE_ID,                      \
                    dis_default_resp,                         \
                    ZB_ZCL_CLUSTER_ID_DOOR_LOCK,              \
                    ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR,           \
                    cb)

/*! @brief General macro for sending a "Door Unlock" cluster command
    @param buffer to put command payload and headers to
    @param dst_addr address to send to
    @param dst_addr_mode destination address mode
    @param dst_ep destination endpoint
    @param ep source endpoint
    @param dis_default_resp "Disable default response" flag
    @param cb command send status callback
*/
#define ZB_ZCL_DOOR_LOCK_SEND_UNLOCK_DOOR_REQ(buffer,             \
                                              dst_addr,           \
                                              dst_addr_mode,      \
                                              dst_ep,             \
                                              ep,                 \
                                              dis_default_resp,   \
                                              cb)                 \
  ZB_ZCL_SEND_CMD(buffer,                                         \
                  dst_addr,                                       \
                  dst_addr_mode,                                  \
                  dst_ep,                                         \
                  ep,                                             \
                  ZB_AF_HA_PROFILE_ID,                            \
                  dis_default_resp,                               \
                  ZB_ZCL_CLUSTER_ID_DOOR_LOCK,                    \
                  ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR, cb)

/*! @brief Door Lock command send response macro
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param seq_num - sequence number
    @param status - result of command execute
    @param aps_secured - APS security mode
*/
#define ZB_ZCL_DOOR_LOCK_SEND_LOCK_DOOR_RES(buffer,                                  \
                                            addr,                                    \
                                            dst_addr_mode,                           \
                                            dst_ep,                                  \
                                            ep,                                      \
                                            prfl_id,                                 \
                                            seq_num,                                 \
                                            status,                                  \
                                            aps_secured)                             \
{                                                                                    \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);                          \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq_num, ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR_RES); \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, status);                                              \
  ZB_ZCL_FINISH_N_SEND_PACKET_NEW(buffer,                                            \
                                  ptr,                                               \
                                  addr,                                              \
                                  dst_addr_mode,                                     \
                                  dst_ep,                                            \
                                  ep,                                                \
                                  prfl_id,                                           \
                                  ZB_ZCL_CLUSTER_ID_DOOR_LOCK,                       \
                                  NULL,                                              \
                                  aps_secured,                                       \
                                  ZB_FALSE,                                          \
                                  0);                                                \
}

/*! @brief Door Unlock command send response macro
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param seq_num - sequence number
    @param status - result of command execute
    @param aps_secured - APS security mode
*/
#define ZB_ZCL_DOOR_LOCK_SEND_UNLOCK_DOOR_RES(buffer,                                  \
                                              addr,                                    \
                                              dst_addr_mode,                           \
                                              dst_ep,                                  \
                                              ep,                                      \
                                              prfl_id,                                 \
                                              seq_num,                                 \
                                              status,                                  \
                                              aps_secured)                             \
{                                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                       \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);                            \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq_num, ZB_ZCL_CMD_DOOR_LOCK_UNLOCK_DOOR_RES); \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, status);                                                \
  ZB_ZCL_FINISH_N_SEND_PACKET_NEW(buffer,                                              \
                                  ptr,                                                 \
                                  addr,                                                \
                                  dst_addr_mode,                                       \
                                  dst_ep,                                              \
                                  ep,                                                  \
                                  prfl_id,                                             \
                                  ZB_ZCL_CLUSTER_ID_DOOR_LOCK,                         \
                                  NULL,                                                \
                                  aps_secured,                                         \
                                  ZB_FALSE,                                            \
                                  0);                                                  \
}

/*! Door lock commands response payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_door_lock_read_lock_door_res_payload_s
{
  zb_uint8_t status;      /*!< Command execution status */
} ZB_PACKED_STRUCT zb_zcl_door_lock_read_lock_door_res_payload_t;

/*! @brief Door Lock response structured reading
    @param buffer - ID of the message buffer (of type zb_bufid_t) containing payload
    @return pointer to @ref zb_zcl_door_lock_read_lock_door_res_payload_s structure
    @attention returned pointer will point to the same data in the buffer thus being valid until
    buffer data will be overwritten.
*/
#define ZB_ZCL_DOOR_LOCK_READ_LOCK_DOOR_RES(buffer)                                   \
  (   (zb_buf_len((buffer)) < sizeof(zb_zcl_door_lock_read_lock_door_res_payload_t))  \
   ?  NULL                                                                            \
   : (zb_zcl_door_lock_read_lock_door_res_payload_t*)zb_buf_begin((buffer)))

/*! Door unlock commands response payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_door_lock_read_unlock_door_res_payload_s
{
  zb_uint8_t status;      /*!< Command execution status */
} ZB_PACKED_STRUCT zb_zcl_door_lock_read_unlock_door_res_payload_t;

/*! @brief Door unLock response structured reading
    @param buffer - ID of the message buffer (of type zb_buf_t)
    containing payload
    @return pointer to @ref zb_zcl_door_lock_read_unlock_door_res_payload_s structure
    @attention returned pointer will point to the same data in the buffer thus
    being valid until buffer data will be overwritten.
*/
#define ZB_ZCL_DOOR_LOCK_READ_UNLOCK_DOOR_RES(buffer)                                   \
  (   (zb_buf_len((buffer)) < sizeof(zb_zcl_door_lock_read_unlock_door_res_payload_t))  \
   ?  NULL                                                                              \
   : (zb_zcl_door_lock_read_unlock_door_res_payload_t*)zb_buf_begin((buffer)))

/*! @} */ /* Door Lock cluster commands */

/*! @cond internals_doc
    @internal @name Door Lock cluster internals
    Internal structures for Door Lock cluster
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID(data_ptr)                 \
  {                                                                                          \
    ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID,                                                     \
    ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                              \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING | ZB_ZCL_ATTR_ACCESS_SCENE,  \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                      \
    (void*) data_ptr                                                                         \
  }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID(data_ptr) \
  {                                                               \
    ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID,                           \
    ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                   \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
    (void*) data_ptr                                              \
  }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID(data_ptr) \
  {                                                               \
    ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID,                    \
    ZB_ZCL_ATTR_TYPE_BOOL,                                        \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
    (void*) data_ptr                                              \
  }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DOOR_LOCK_RF_OPERATION_EVENT_MASK_ID(data_ptr) \
  {                                                               \
    ZB_ZCL_ATTR_DOOR_LOCK_RF_OPERATION_EVENT_MASK_ID,             \
    ZB_ZCL_ATTR_TYPE_16BITMAP,                                    \
    ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING, \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
    (void*) data_ptr                                              \
  }

/*! @internal @brief Number of attributes mandatory for reporting in Door Lock cluster */
#define ZB_ZCL_DOOR_LOCK_REPORT_ATTR_COUNT  1

/*! @}
    @endcond */ /* Door Lock cluster internals */

/*! @} */ /* ZCL Door Lock cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_door_lock_init_server(void);
void zb_zcl_door_lock_init_client(void);
#define ZB_ZCL_CLUSTER_ID_DOOR_LOCK_SERVER_ROLE_INIT zb_zcl_door_lock_init_server
#define ZB_ZCL_CLUSTER_ID_DOOR_LOCK_CLIENT_ROLE_INIT zb_zcl_door_lock_init_client

#endif /* ZB_ZCL_DOOR_LOCK_H */
