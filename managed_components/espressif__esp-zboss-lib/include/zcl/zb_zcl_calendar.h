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
/* PURPOSE: Calendar cluster definitions
*/

#ifndef ZB_ZCL_CALENDAR_H_
#define ZB_ZCL_CALENDAR_H_
/******************************************************************************/
/******************************************************************************/
#include "zboss_api_core.h"
#include "zboss_api_aps.h"
#include "zb_zcl_common.h"


/** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */

/** @addtogroup ZB_ZCL_CALENDAR
  *  @{
  *   @details
  *   The Calendar cluster implements commands to transfer calendar information
  *   within the premises. The calendar information is distributed by an ESI.
  */

/* Cluster ZB_ZCL_CLUSTER_ID_CALENDAR */

/** @defgroup ZB_ZCL_CALENDAR_ATTRS Calendar cluster attributes
 * @{
 */


/** The attributes defined in this cluster are arranged into sets of related attributes;
 * each set can contain up to 256 attributes. Attribute identifiers are encoded
 * such that the most significant Octet specifies the attribute set and the least
 * significant Octet specifies the attribute within the set.
 * @see SE spec, Table D-156 - Calendar Cluster Attribute Sets
 */
typedef enum zb_zcl_calendar_srv_attr_sets_e
{
  ZB_ZCL_CALENDAR_SET_AUXILIARY_SWITCH_LABEL = 0x00,  /**< Auxiliary Switch Label Attribute Set */
} zb_zcl_calendar_srv_attr_sets_t;


/** @see SE spec, Table D-157– Auxiliary Switch Label Attribute Set */
typedef enum zb_zcl_calendar_srv_auxiliary_switch_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_1_LABEL = 0x0000,  /**< The @e AuxSwitchNLabel attributes provide a method for assigning a label to
                                                           * an Auxiliary Switch.
                                                           */
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_2_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_3_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_4_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_5_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_6_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_7_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_8_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_CALENDAR_AUX_LOAD_SWITCH_STATE = 0x0080 /**< The @e AuxiliaryLoadSwitchState attribute represents the current state
                                                           *  of the @e AuxiliaryLoadSwitches as defined by the
                                                           *  @e AuxiliaryLoadPublishDayProfile. Bit0 corresponds to @e AuxiliarySwitch1 and
                                                           *  bit7 corresponds to @e AuxiliarySwitch 8. A bit set to “1” indicates an ON state
                                                           *  and a bit set to “0” indicates an OFF state.
                                                           */
} zb_zcl_calendar_srv_auxiliary_switch_attr_set_t;


/** @} */ /* ZB_ZCL_CALENDAR_ATTRS */


/** @defgroup ZB_ZCL_CALENDAR_COMMANDS Calendar cluster commands
 * @{
 */

/** Calendar cluster client command identifiers.
 *  @see SE spec, subclause D.9.2.4
 */
typedef enum zb_zcl_calendar_cli_cmd_e
{
  /* (O) */
  ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR              = 0x00,  /**< This command initiates PublishCalendar command(s) for scheduled Calendar
                                                               * updates.
                                                               * @see zb_zcl_calendar_get_calendar_payload_t
                                                               */
  /* (O) */
  ZB_ZCL_CALENDAR_CLI_CMD_GET_DAY_PROFILES,                   /**< This command initiates one or more PublishDayProfile commands for the
                                                                * referenced Calendar.
                                                                * @see zb_zcl_calendar_get_day_profiles_payload_t
                                                                */
  /* (O) */
  ZB_ZCL_CALENDAR_CLI_CMD_GET_WEEK_PROFILES,                  /**< This command initiates one or more PublishWeekProfile commands for the
                                                                * referenced Calendar.
                                                                * @see zb_zcl_calendar_get_week_profiles_payload_t
                                                                */
  /* (O) */
  ZB_ZCL_CALENDAR_CLI_CMD_GET_SEASONS,                        /**< This command initiates one or more PublishSeasons commands for the
                                                                * referenced Calendar.
                                                                * @see zb_zcl_calendar_get_seasons_payload_t
                                                                */
  /* (O) */
  ZB_ZCL_CALENDAR_CLI_CMD_GET_SPECIAL_DAYS,                   /**< This command initiates one or more PublishSpecialDays commands for the
                                                                * scheduled Special Day Table updates.
                                                                * @see zb_zcl_calendar_get_special_days_payload_t
                                                                */
  /* (O) */
  ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR_CANCELLATION,          /**< This command initiates the return of the last CancelCalendar command
                                                                * held on the associated server.
                                                                * @note This command has no payload.
                                                                */
} zb_zcl_calendar_cli_cmd_t;


/** @see SE spec, Table D-158 - Commands Generated by the Calendar Cluster Server */
typedef enum zb_zcl_calendar_srv_cmd_e
{
  /* (M) */
  ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_CALENDAR      = 0x00,   /**< The PublishCalendar command is published in response to a GetCalendar
                                                            * command or if new calendar information is available. The Calendar must be
                                                            * replaced as a whole; only the Special Day Table can be changed independently.
                                                            * @see zb_zcl_calendar_publish_calendar_payload_t
                                                            */

  /* (M) */
  ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_DAY_PROFILE,            /**< The PublishDayProfile command is published in response to a GetDayProfile
                                                            * command. If the IssuerCalendarID does not match with one of the stored
                                                            * calendar instances, the client shall ignore the command and respond using
                                                            * ZCL Default Response with a status response of NOT_FOUND.
                                                            * @see zb_zcl_calendar_publish_day_profile_payload_t
                                                            */

  /* (M) */
  ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_WEEK_PROFILE,           /**< The PublishWeekProfile command is published in response to a GetWeekProfile
                                                            * command. If the IssuerCalendarID does not match with one of the stored
                                                            * calendar instances, the client shall ignore the command and respond using
                                                            * ZCL Default Response with a status response of NOT_FOUND.
                                                            * @see zb_zcl_calendar_publish_week_profile_payload_t
                                                            */

  /* (M) */
  ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SEASONS,                /**< The PublishSeasons command is published in response to a GetSeason command.
                                                            * If the IssuerCalendarID does not match with one of the stored calendar
                                                            * instances, the client shall ignore the command and respond using ZCL
                                                            * Default Response with a status response of NOT_FOUND.
                                                            * @see zb_zcl_calendar_publish_seasons_payload_t
                                                            */

  /* (M) */
  ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SPECIAL_DAYS,           /**< The PublishSpecialDays command is published in response to a GetSpecialDays
                                                            * command or if a calendar update is available. If the Calendar Type does not
                                                            * match with one of the stored calendar instances, the client shall ignore
                                                            * the command and respond using ZCL Default Response with a status response
                                                            * of NOT_FOUND.
                                                            * @see zb_zcl_calendar_publish_special_days_payload_t
                                                            */

  /* (O) */
  ZB_ZCL_CALENDAR_SRV_CMD_CANCEL_CALENDAR,                /**< The CancelCalendar command indicates that all data associated with a
                                                            * particular calendar instance should be discarded.
                                                            * @see zb_zcl_calendar_cancel_calendar_payload_t
                                                            */
} zb_zcl_calendar_srv_cmd_t;


/* Calendar cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_CALENDAR_SERVER_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_CALENDAR,      \
                                      ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_DAY_PROFILE,   \
                                      ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_WEEK_PROFILE,  \
                                      ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SEASONS,       \
                                      ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SPECIAL_DAYS,  \
                                      ZB_ZCL_CALENDAR_SRV_CMD_CANCEL_CALENDAR

#define ZB_ZCL_CLUSTER_ID_CALENDAR_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_CALENDAR_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_CALENDAR_CLIENT_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR,          \
                                      ZB_ZCL_CALENDAR_CLI_CMD_GET_DAY_PROFILES,      \
                                      ZB_ZCL_CALENDAR_CLI_CMD_GET_WEEK_PROFILES,     \
                                      ZB_ZCL_CALENDAR_CLI_CMD_GET_SEASONS,           \
                                      ZB_ZCL_CALENDAR_CLI_CMD_GET_SPECIAL_DAYS,      \
                                      ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR_CANCELLATION

#define ZB_ZCL_CLUSTER_ID_CALENDAR_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_CALENDAR_CLIENT_ROLE_GENERATED_CMD_LIST



/** Calendar type enumeration.
 *  @see SE spec, Table D-159
 */
typedef enum zb_zcl_calendar_calendar_type_e
{
  ZB_ZCL_CALENDAR_TYPE_DELIVERED_CALENDAR               = 0x00,  /**< Delivered Calendar */
  ZB_ZCL_CALENDAR_TYPE_RECEIVED_CALENDAR                = 0x01,  /**< Received Calendar */
  ZB_ZCL_CALENDAR_TYPE_DELIVERED_AND_RECEIVED_CALENDAR  = 0x02,  /**< Delivered and Received Calendar */
  ZB_ZCL_CALENDAR_TYPE_FRIENDLY_CREDIT_CALENDAR         = 0x03,  /**< Friendly Credit Calendar */
  ZB_ZCL_CALENDAR_TYPE_AUXILLARY_LOAD_SWITCH_CALENDAR   = 0x04,  /**< Auxiliary Load Switch Calendar */
  /* reserved 0x05 - 0xFF */
} zb_zcl_calendar_calendar_type_t;


/** Check if calendar type is valid
 * @param _t - @ref zb_zcl_calendar_get_calendar_payload_t::calendar_type field
 *
 * @see @ref zb_zcl_calendar_get_calendar_payload_t
 */
#define ZB_ZCL_CALENDAR_CALENDAR_TYPE_IS_VALID(_t) \
  (!((_t > ZB_ZCL_CALENDAR_TYPE_AUXILLARY_LOAD_SWITCH_CALENDAR)&&(_t < 0xff)))


/** @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR "GetCalendar" Command Payload
 *  @see SE spec, subclause 9.2.4.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_get_calendar_payload_s
{
  /** UTC Timestamp indicating the earliest start time of calendars to be
   *  returned by the corresponding PublishCalendar command.
   */
  zb_uint32_t earliest_start_time;

  /** A 32-bit integer representing the minimum Issuer Event ID of calendars to
   *  be returned by the corresponding PublishCalendar command.
   */
  zb_uint32_t min_issuer_event_id;

  /** An 8-bit integer which represents the maximum number of PublishCalendar
   *  commands that the client is willing to receive in response to this command.
   */
  zb_uint8_t  number_of_calendars;

  /** An 8-bit enumeration identifying the calendar type of the requested calendar.
   * @see zb_zcl_calendar_calendar_type_t
   */
  zb_uint8_t  calendar_type;

  /** An unsigned 32-bit field containing a unique identifier for the commodity provider.
   */
  zb_uint32_t provider_id;
} ZB_PACKED_STRUCT zb_zcl_calendar_get_calendar_payload_t;

/** @brief Default value for Calendar cluster revision global attribute */
#define ZB_ZCL_CALENDAR_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/*!
  @brief Declare attribute list for Calendar cluster (only cluster revision attribute)
  @param attr_list - attribute list name
*/
#define ZB_ZCL_DECLARE_CALENDAR_ATTR_LIST(attr_list)                            \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_CALENDAR) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** Macro for initialization @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR "GetCalendar" command payload
 */
#define ZB_ZCL_CALENDAR_GET_CALENDAR_PL_INIT \
  (zb_zcl_calendar_get_calendar_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_DAY_PROFILES "GetDayProfiles" command payload
 */
#define ZB_ZCL_CALENDAR_GET_DAY_PROFILES_PL_INIT \
  (zb_zcl_calendar_get_day_profiles_payload_t) {0}

/** Macro for initialization @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_WEEK_PROFILES "GetWeekProfiles" command payload
 */
#define ZB_ZCL_CALENDAR_GET_WEEK_PROFILES_PL_INIT \
  (zb_zcl_calendar_get_week_profiles_payload_t) {0}

/** Macro for initialization @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SEASONS "GetSeasons" command payload
 */
#define ZB_ZCL_CALENDAR_GET_SEASONS_PL_INIT \
  (zb_zcl_calendar_get_seasons_payload_t) {0}

/** Macro for initialization @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SPECIAL_DAYS "GetSpecialDays" command payload
 */
#define ZB_ZCL_CALENDAR_GET_SPECIAL_DAYS_PL_INIT \
  (zb_zcl_calendar_get_special_days_payload_t) {0}

/** Check if @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR "GetCalendar" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_GET_CALENDAR_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_calendar_get_calendar_payload_t) ? ZB_TRUE : ZB_FALSE))


/** Check if @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_DAY_PROFILES "GetDayProfiles" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_GET_DAY_PROFILES_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_calendar_get_day_profiles_payload_t) ? ZB_TRUE : ZB_FALSE))


/** Check if @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_WEEK_PROFILES "GetWeekProfiles" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_GET_WEEK_PROFILES_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_calendar_get_week_profiles_payload_t) ? ZB_TRUE : ZB_FALSE))

/** Check if @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SEASONS "GetSeasons" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_GET_SEASONS_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_calendar_get_seasons_payload_t) ? ZB_TRUE : ZB_FALSE))

/** Check if @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SPECIAL_DAYS "GetSpecialDays" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_GET_SPECIAL_DAYS_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_calendar_get_special_days_payload_t) ? ZB_TRUE : ZB_FALSE))


/** This enumeration presents possible values of Calendar Time Reference field
  * @see SE spec, Table D-160 and @ref zb_zcl_calendar_publish_calendar_payload_t::calendar_time_reference
  */
typedef enum zb_zcl_calendar_calendar_time_reference_e
{
  ZB_ZCL_CALENDAR_TIME_REFERENCE_UTC_TIME = 0x00,  /**< UTC time  */
  ZB_ZCL_CALENDAR_TIME_REFERENCE_STANDARD_TIME,    /**< Standard time */
  ZB_ZCL_CALENDAR_TIME_REFERENCE_LOCAL_TIME        /**< Local time */
  /* 0x03 - 0xFF is reserved */
} zb_zcl_calendar_calendar_time_reference_t;


/** @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_CALENDAR "PublishCalendar" command
 *  @see SE spec, subclause 9.2.3.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_publish_calendar_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;              /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;          /* (M) */

  /** Unique identifier generated by the commodity Supplier to identify a
   * particular calendar.
   */
  zb_uint32_t issuer_calendar_id;       /* (M) */

  /** A UTC Time field to denote the time at which the published calendar becomes
   * valid. A start date/time of 0x00000000 shall indicate that the command
   * should be executed immediately.
   */
  zb_uint32_t start_time;               /* (M) */

  /** An 8-bit enumeration identifying the type of calendar published in this
   * command.
   * @see zb_zcl_calendar_calendar_type_t
   */
  zb_uint8_t  calendar_type;            /* (M) */

  /** This field indicates how the Start Times contained in the calendar are to
   * be interpreted.
   * @see zb_zcl_calendar_calendar_time_reference_t
   */
  zb_uint8_t  calendar_time_reference;  /* (M) */

  /** The CalendarName provides a method for utilities to assign a name to the
   * entire calendar.
   */
  zb_uint8_t  calendar_name[1 + 12];    /* (M) */

  /** Number of entries in the Seasons Table. A value of 0x00 means no Season
   * defined.
   */
  zb_uint8_t  number_of_seasons;        /* (M) */

  /** Number of week profiles in the Week Profile Table. A value of 0x00 means no
   * Week Profile defined.
   */
  zb_uint8_t  number_of_week_profiles;  /* (M) */

  /** Number of day profiles in the Day Profile Table. */
  zb_uint8_t  number_of_day_profiles;   /* (M) */
} ZB_PACKED_STRUCT zb_zcl_calendar_publish_calendar_payload_t;


/** Macro for initialization @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_CALENDAR "PublishCalendar" command payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_CALENDAR_PL_INIT \
  (zb_zcl_calendar_publish_calendar_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_DAY_PROFILE_PL_INIT \
  (zb_zcl_calendar_publish_day_profile_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_WEEK_PROFILE "PublishWeekProfile" command payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_WEEK_PROFILE_PL_INIT \
  (zb_zcl_calendar_publish_week_profile_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SEASONS "PublishSeasons" command payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_SEASONS_PL_INIT \
  (zb_zcl_calendar_publish_seasons_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SPECIAL_DAYS "PublishSpecialDays" command payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_SPECIAL_DAYS_PL_INIT \
  (zb_zcl_calendar_publish_special_days_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_CALENDAR_SRV_CMD_CANCEL_CALENDAR "CancelCalendar" command payload
 */
#define ZB_ZCL_CALENDAR_CANCEL_CALENDAR_PL_INIT \
  (zb_zcl_calendar_cancel_calendar_payload_t) {0}


/** Compute expected size of received @ref zb_zcl_calendar_publish_calendar_payload_t
 * data payload
 * @param pl - pointer to @ref zb_zcl_calendar_publish_calendar_payload_t payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_CALENDAR_PL_EXPECTED_SIZE(pl) \
  (sizeof(zb_zcl_calendar_publish_calendar_payload_t)-sizeof((pl)->calendar_name)+1+ZB_ZCL_GET_STRING_LENGTH((pl)->calendar_name))


/** Check if @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_CALENDAR "PublishCalendar" command payload size is valid
 * @param pl - pointer to @ref zb_zcl_calendar_publish_calendar_payload_t payload (pointer
 * to buffer beginning that contains @ref zb_zcl_calendar_publish_calendar_payload_t data payload)
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_CALENDAR_PL_SIZE_IS_VALID(pl, size) \
  ((size >= ZB_ZCL_CALENDAR_PUBLISH_CALENDAR_PL_EXPECTED_SIZE(pl)) ? ZB_TRUE : ZB_FALSE)


/** Check if @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_DAY_PROFILE_PL_SIZE_IS_VALID(size) \
  ((size >= (sizeof(zb_zcl_calendar_publish_day_profile_payload_t)-sizeof(void *)-sizeof(zb_uint8_t)) ? ZB_TRUE : ZB_FALSE))


/** Check if @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_WEEK_PROFILE "PublishWeekProfile" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_WEEK_PROFILE_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_calendar_publish_week_profile_payload_t) ? ZB_TRUE : ZB_FALSE))


/** Check if @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SEASONS "PublishSeasons" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_SEASONS_PL_SIZE_IS_VALID(size) \
  ((size >= (sizeof(zb_zcl_calendar_publish_seasons_payload_t)-sizeof(void *)-sizeof(zb_uint8_t)) ? ZB_TRUE : ZB_FALSE))


/** Check if @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SPECIAL_DAYS "PublishSpecialDays" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_PUBLISH_SPECIAL_DAYS_PL_SIZE_IS_VALID(size) \
  ((size >= (sizeof(zb_zcl_calendar_publish_special_days_payload_t)-sizeof(void *)-sizeof(zb_uint8_t)) ? ZB_TRUE : ZB_FALSE))


/** Check if @ref ZB_ZCL_CALENDAR_SRV_CMD_CANCEL_CALENDAR "CancelCalendar" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_CALENDAR_CANCEL_CALENDAR_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_calendar_cancel_calendar_payload_t) ? ZB_TRUE : ZB_FALSE))


/** The format of Day Schedule entries is dependent on the Calendar Type (@ref
  * zb_zcl_calendar_calendar_type_t). If the Calendar Type is 0x00 – 0x02 then
  * Rate Start Times shall be used. If the value is 0x03 then the Friendly Credit
  * Start Times shall be used. If the value is 0x04 then the Auxiliary Load Start
  * Times shall be used. A value other than these would be invalid.
  * @see SE spec, subclause D.9.2.3.2.3
  * @see @ref zb_zcl_calendar_publish_day_profile_payload_t::day_schedule_entries
  */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_day_schedule_entries_s
{
  /** The Start Time is represented in minutes from midnight. ScheduleEntries
   * must be arranged in ascending order of Start Times. The first Schedule Entry
   * must have 0x0000 (midnight) as the StartTime
   */
  zb_uint16_t start_time; /* (M) */

  /** \b Price \b Tier is the current price tier that is valid until the start time of the next
    * Schedule Entry.
    * The \b Friendly \b Credit \b Enable field is a Boolean denoting if the Friendly Credit
    * period is available for the consumer to use. A value of 1 means it is enabled
    * and a 0 means that the Friendly Credit period is not available for the
    * consumer to use.\n
    *
    * The required status of the auxiliary switches is indicated by the state
    * of the \b Auxiliary \b Load \b Switch \b State bits. Bit0 corresponds to Auxiliary
    * Switch 1 and bit7 corresponds to Auxiliary Switch 8. A bit set to "1"
    * indicates an ON state and a bit set to "0" indicates an OFF state.
    *
    * @see SE spec, subclause D.9.2.3.2.3.1, D.9.2.3.2.3.2 and D.9.2.3.2.3.3
    */
  ZB_PACKED_PRE union
  {
    zb_uint8_t price_tier;
    zb_uint8_t friendly_credit_enable;
    zb_uint8_t auxiliary_load_switch_state;
  } u;

} ZB_PACKED_STRUCT zb_zcl_calendar_day_schedule_entries_t;


/** @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command payload.
 * @see SE spec, subclause D.9.2.3.2.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_publish_day_profile_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;                       /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;                   /* (M) */

  /** Unique identifier generated by the commodity supplier. All parts of a
   * calendar instance shall have the same Issuer Calendar ID.
   */
  zb_uint32_t issuer_calendar_id;                /* (M) */

  /** Unique identifier generated by the commodity supplier. The Day ID is used
   * as reference to assign a Day Profile to a Special Day or days in a Week
   * Profile.
   */
  zb_uint8_t day_id;                             /* (M) */

  /** An 8-bit integer representing the total number of ScheduleEntries in this
   * Day Profile.
   */
  zb_uint8_t total_number_of_schedule_entries;   /* (M) */

  /** The CommandIndex is used to count the payload fragments in the case where
   * the entire payload does not fit into one message. The CommandIndex starts
   * at 0 and is incremented for each fragment belonging to the same command.
   */
  zb_uint8_t command_index;                      /* (M) */

  /** In the case where the entire payload does not fit into one message, the
   * Total Number of Commands field indicates the total number of sub-commands
   * in the message.
   */
  zb_uint8_t total_number_of_commands;           /* (M) */

  /** An 8-bit enumeration identifying the type of calendar published in this
   * command.
   * @see zb_zcl_calendar_calendar_type_t
   */
  zb_uint8_t calendar_type;                      /* (M) */

  /** @see zb_zcl_calendar_day_schedule_entries_s
   */
  zb_zcl_calendar_day_schedule_entries_t *day_schedule_entries;   /* (M) */

  /** An 8-bit integer representing the number of Day Schedule entries in
   * this command
   */
   zb_uint8_t number_of_entries_in_this_command;  /* (ZBOSS) */
} ZB_PACKED_STRUCT zb_zcl_calendar_publish_day_profile_payload_t;


/** @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_WEEK_PROFILE "PublishWeekProfile" command payload
 * @see SE spec, subclause D.9.2.3.3.2 and Figure D-143 - PublishWeekProfile Command Payload
 */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_publish_week_profile_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;          /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;      /* (M) */

  /** Unique identifier generated by the commodity supplier. All parts of a
   * calendar instance shall have the same Issuer Calendar ID.
   */
  zb_uint32_t issuer_calendar_id;   /* (M) */

  /** Unique identifier generated by the commodity supplier. The Week ID is used
   * as reference to assign a Week Profile to a Season Entry. When generating
   * calendars, Week IDs shall be allocated sequentially, starting from 1.
   */
  zb_uint8_t week_id;               /* (M) */

  /** Day ID Ref Monday until Day ID Ref Sunday (mandatory): Reference to the
   * related Day Profile entry.
   */
  zb_uint8_t day_id_ref_monday;     /* (O) */
  zb_uint8_t day_id_ref_tuesday;    /* (O) */
  zb_uint8_t day_id_ref_wednesday;  /* (O) */
  zb_uint8_t day_id_ref_thursday;   /* (O) */
  zb_uint8_t day_id_ref_friday;     /* (O) */
  zb_uint8_t day_id_ref_saturday;   /* (O) */
  zb_uint8_t day_id_ref_sunday;     /* (O) */
} ZB_PACKED_STRUCT zb_zcl_calendar_publish_week_profile_payload_t;


/** Season Entry payload
 * A Season Entry consists of a Season Start Date and the reference (Week ID Ref)
 * to the related Week Profile entry. The Start Date of the Season Entries must
 * be arranged in ascending order. The active season is valid until the Season
 * Start Date of the next Season Entry.
 * @see SE spec, Figure D-145 - Season Entry Sub-Payload
 * @see zb_zcl_calendar_publish_seasons_payload_t::season_entry
 */
typedef struct zb_zcl_calendar_season_entry_payload_s
{
  zb_uint32_t season_start_date;         /* (M) */
  zb_uint8_t week_id_ref;                /* (M) */
} zb_zcl_calendar_season_entry_payload_t;


/** @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SEASONS "PublishSeasons" command payload.
  * @see SE spec, subclause D.9.2.3.4.1
  */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_publish_seasons_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;              /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period,
   * this field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;          /* (M) */

  /** Unique identifier generated by the commodity supplier. All parts of a
   * calendar instance shall have the same Issuer Calendar ID.
   */
  zb_uint32_t issuer_calendar_id;       /* (M) */

  /** The Command Index is used to count the payload fragments in the case where
   * the entire payload does not fit into one message. The Command Index starts
   * at 0 and is incremented for each fragment belonging to the same command.
   */
  zb_uint8_t command_index;             /* (M) */

  /** In the case where the entire payload does not fit into one message, the
   * Total Number of Commands field indicates the total number of sub-commands
   * in the message.
   */
  zb_uint8_t total_number_of_commands;  /* (M) */

  /** A Season Entry consists of a Season Start Date and the reference
   * (Week ID Ref) to the related Week Profile entry. The Start Date of the
   * Season Entries must be arranged in ascending order. The active season is
   * valid until the Season Start Date of the next Season Entry.
   * @see zb_zcl_calendar_season_entry_payload_s
   */
  zb_zcl_calendar_season_entry_payload_t *season_entry;  /* (M) */

  /** An 8-bit integer representing the number of Season entries in
   * this command
   */
   zb_uint8_t number_of_entries_in_this_command;  /* (ZBOSS) */
} ZB_PACKED_STRUCT zb_zcl_calendar_publish_seasons_payload_t;


/** @ref ZB_ZCL_CALENDAR_SRV_CMD_CANCEL_CALENDAR "CancelCalendar" command payload.
 * @see SE spec, subclause D.9.2.3.6
 */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_cancel_calendar_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;              /* (M) */

  /** Unique identifier generated by the commodity Supplier. All parts of a
   * calendar instance shall have the same Issuer Calendar ID.
   */
  zb_uint32_t issuer_calendar_id;       /* (M) */

  /** An 8-bit enumeration identifying the type of calendar to be cancelled by
   * this command. Table D-159 details the enumeration of this field.
   * @see zb_zcl_calendar_calendar_type_t
   */
  zb_uint8_t calendar_type;
} ZB_PACKED_STRUCT zb_zcl_calendar_cancel_calendar_payload_t;


/** Special Day Entry
  * A SpecialDayEntry consists of the Special Day Date and a
  * reference (Day ID Ref) to the related Day Profile entry. The dates of the
  * Special Day Table must be arranged in ascending order.
  * @see SE spec, Figure D-147 - SpecialDayEntry Sub-Payload
  * @see zb_zcl_calendar_publish_special_days_payload_t::special_day_entry
  */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_special_day_entry_s
{
  zb_uint32_t special_day_date;  /* (M) */
  zb_uint8_t day_id_ref;         /* (M) */
} ZB_PACKED_STRUCT zb_zcl_calendar_special_day_entry_t;


/** @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SPECIAL_DAYS "PublishSpecialDays" command payload.
  * @see SE spec, subclause D.9.2.3.5
  */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_publish_special_days_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;                  /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;              /* (M) */

  /** Unique identifier generated by the commodity Supplier. All parts of a
   * calendar instance shall have the same Issuer Calendar ID.
   */
  zb_uint32_t issuer_calendar_id;           /* (M) */

  /** A UTC Time field to denote the time at which the Special Day Table becomes
   * valid. A start date/time of 0x00000000 shall indicate that the command
   * should be executed immediately.
   */
  zb_uint32_t start_time;                   /* (M) */

  /** An 8-bit enumeration identifying the type of calendar this day profile
   * belongs to. Generation Meters shall use the ‘Received’ Calendar.
   * @see zb_zcl_calendar_calendar_type_t
   */
  zb_uint8_t calendar_type;                 /* (M) */

  /** An 8-bit integer representing the total number of Special Day entries in
   * this Special Day Table
   */
  zb_uint8_t total_number_of_special_days;  /* (M) */

  /** The Command Index is used to count the payload fragments in the case where
   * the entire payload does not fit into one message. The Command Index starts
   * at 0 and is incremented for each fragment belonging to the same command.
   */
  zb_uint8_t command_index;                 /* (M) */

  /** In the case where the entire payload does not fit into one message, the
   * Total Number of Commands field indicates the total number of sub-commands
   * in the message.
   */
  zb_uint8_t total_number_of_commands;      /* (M) */

  /** A SpecialDayEntry consists of the Special Day Date and a reference
   * (Day ID Ref) to the related Day Profile entry. The dates of the Special Day
   * Table must be arranged in ascending order.
   * @see zb_zcl_calendar_special_day_entry_t
   */
  zb_zcl_calendar_special_day_entry_t *special_day_entry; /* (M) */

  /** An 8-bit integer representing the number of Special Day entries in
   * this command
   */
  zb_uint8_t number_of_entries_in_this_command;  /* (ZBOSS) */

} ZB_PACKED_STRUCT zb_zcl_calendar_publish_special_days_payload_t;


/** @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_DAY_PROFILES "GetDayProfiles" command payload
  * @see SE spec, subclause D.9.2.4.2
  */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_get_day_profiles_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;         /* (M) */

  /** IssuerCalendarID of the calendar to which the requested Day Profiles
   * belong.
   */
  zb_uint32_t issuer_calendar_id;  /* (M) */

  /** Unique identifier for a Day Profile generated by the commodity supplier.
   * The Start Day ID indicates the minimum ID of Day Profiles to be returned
   * by the corresponding PublishDayProfile command. A value of 0x01 indicates
   * that the (first) PublishDayProfile command should contain the profile with
   * the lowest Day ID held by the server. A value of 0x00 is unused.
   */
  zb_uint8_t start_day_id;         /* (M) */

  /** An 8-bit integer which represents the maximum number of Day Profiles that
   * the client is willing to receive in response to this command. A value of
   * 0x00 will cause the return of all day profiles with an ID equal to or
   * greater than the Start Day ID.
   */
  zb_uint8_t number_of_days;       /* (M) */
} ZB_PACKED_STRUCT zb_zcl_calendar_get_day_profiles_payload_t;


/** @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_WEEK_PROFILES "GetWeekProfiles" command payload.
  * @see SE spec, Figure D-151 - @e GetWeekProfiles Command Payload
  */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_get_week_profiles_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;          /* (M) */

  /** IssuerCalendarID of the calendar to which the requested Week Profiles
   * belong.
   */
  zb_uint32_t issuer_calendar_id;   /* (M) */

  /** Unique identifier for a Week Profile generated by the commodity supplier.
   * The Start Week ID indicates the minimum ID of Week Profiles to be returned
   * by the corresponding PublishWeekProfile command.
   */
  zb_uint8_t start_week_id;         /* (M) */

  /** An 8-bit integer which represents the maximum number of Week Profiles that
   * the client is willing to receive in response to this command. A value of
   * 0x00 will cause the return of all week profiles with an ID equal to or
   * greater than the Start Week ID.
   */
  zb_uint8_t number_of_weeks;       /* (M) */
} ZB_PACKED_STRUCT zb_zcl_calendar_get_week_profiles_payload_t;


/** @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SEASONS "GetSeasons" command payload.
  * Figure D-152 - @e GetSeasons Command Payload
  */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_get_seasons_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the
commodity provider. */
  zb_uint32_t provider_id;

  /** IssuerCalendarID of the calendar to which the requested Seasons belong. */
  zb_uint32_t issuer_calendar_id;
} ZB_PACKED_STRUCT zb_zcl_calendar_get_seasons_payload_t;


/** @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SPECIAL_DAYS "GetSpecialDays" command payload
  * @see SE spec and Figure D-153 - @e GetSpecialDays Command Payload
  */
typedef ZB_PACKED_PRE struct zb_zcl_calendar_get_special_days_payload_s
{

  /** UTC Timestamp to select active and scheduled events to be returned by the
   * corresponding PublishSpecialDays command.
   */
  zb_uint32_t start_time;          /* (M) */

  /** An 8-bit integer which represents the maximum number of Special Day Table
   * instances to be sent. A value of 0 would indicate all available Special Day
   * tables shall be returned.
   */
  zb_uint8_t number_of_events;     /* (M) */

  /** An 8-bit enumeration identifying the calendar type of the requested Special
   * Days. Generation Meters shall use the ‘Received’ Calendar.
   * @see zb_zcl_calendar_calendar_type_t
   */
  zb_uint8_t calendar_type;        /* (M) */

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider. This field allows differentiation in deregulated markets where
   * multiple commodity providers may be available.
   */
  zb_uint32_t provider_id;         /* (M) */

  /** Unique identifier generated by the commodity supplier. A value of
   * 0x00000000 will cause the return of all Special Days profiles.
   */
  zb_uint32_t issuer_calendar_id;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_calendar_get_special_days_payload_t;


/** Function for send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_CALENDAR "PublishCalendar" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with @ref ZB_ZCL_CALENDAR_GET_CALENDAR_CB_ID
 * callback id on reception of @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR "GetCalendar" command.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_PUBLISH_CALENDAR_CB_ID.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_publish_calendar_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_publish_calendar(zb_uint8_t param, const zb_addr_u *dst_addr,
                                               zb_aps_addr_mode_t dst_addr_mode,
                                               zb_uint8_t dst_ep, zb_uint8_t src_ep,
                                               const zb_zcl_calendar_publish_calendar_payload_t *pl,
                                               zb_callback_t cb);


/** Function for send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with @ref ZB_ZCL_CALENDAR_GET_DAY_PROFILES_CB_ID
 * callback id on reception of @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_DAY_PROFILES "GetDayProfiles" command.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_PUBLISH_DAY_PROFILE_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_publish_day_profile_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_publish_day_profile(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_publish_day_profile_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_WEEK_PROFILE "PublishWeekProfile" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with @ref ZB_ZCL_CALENDAR_GET_WEEK_PROFILES_CB_ID
 * callback id on reception of @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_WEEK_PROFILES "GetWeekProfiles" command.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_PUBLISH_WEEK_PROFILE_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_publish_week_profile_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_publish_week_profile(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_publish_week_profile_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SEASONS "PublishSeasons" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with @ref ZB_ZCL_CALENDAR_GET_SEASONS_CB_ID
 * callback id on reception of @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SEASONS "GetSeasons" command.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_PUBLISH_SEASONS_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_publish_seasons_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_publish_seasons(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_publish_seasons_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SPECIAL_DAYS "PublishSpecialDays" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with @ref ZB_ZCL_CALENDAR_GET_SPECIAL_DAYS_CB_ID
 * callback id on reception of @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SPECIAL_DAYS "GetSpecialDays" command.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_PUBLISH_SPECIAL_DAYS_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_publish_special_days_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_publish_special_days(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_publish_special_days_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR "GetCalendar" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_GET_CALENDAR_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_get_calendar_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_get_calendar(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_get_calendar_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_DAY_PROFILES "GetDayProfiles" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_GET_DAY_PROFILES_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_get_day_profiles_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_get_day_profiles(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_get_day_profiles_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_WEEK_PROFILES "GetWeekProfiles" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_GET_WEEK_PROFILES_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_get_week_profiles_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_get_week_profiles(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_get_week_profiles_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SEASONS "GetSeasons" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_GET_SEASONS_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_get_seasons_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_get_seasons(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_get_seasons_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_SPECIAL_DAYS "GetSpecialDays" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_GET_SPECIAL_DAYS_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_get_special_days_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_get_special_days(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_get_special_days_payload_t *pl,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_CLI_CMD_GET_CALENDAR_CANCELLATION "GetCalendarCancellation" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_GET_CALENDAR_CANCELLATION_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_get_calendar_cancellation(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_CALENDAR_SRV_CMD_CANCEL_CALENDAR "CancelCalendar" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_CALENDAR_CANCEL_CALENDAR_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_calendar_cancel_calendar_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_calendar_send_cmd_cancel_calendar(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_calendar_cancel_calendar_payload_t *pl,
  zb_callback_t cb
);


/** Macro for call @ref zb_zcl_calendar_send_cmd_publish_calendar function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_PUBLISH_CALENDAR(_param, _dst_addr, _addr_mode,  \
                                             _dst_ep, _src_ep, _payload)          \
  zb_zcl_calendar_send_cmd_publish_calendar(_param, _dst_addr, _addr_mode,        \
                                            _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_publish_day_profile function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_PUBLISH_DAY_PROFILE(_param, _dst_addr, _addr_mode,  \
                                                     _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_publish_day_profile(_param, _dst_addr, _addr_mode,        \
                                               _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_publish_week_profile function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_PUBLISH_WEEK_PROFILE(_param, _dst_addr, _addr_mode,  \
                                                      _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_publish_week_profile(_param, _dst_addr, _addr_mode,        \
                                                _dst_ep, _src_ep, _payload, NULL)



/** Macro for call @ref zb_zcl_calendar_send_cmd_publish_seasons function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_PUBLISH_SEASONS(_param, _dst_addr, _addr_mode,  \
                                                 _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_publish_seasons(_param, _dst_addr, _addr_mode,        \
                                           _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_publish_special_days function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_PUBLISH_SPECIAL_DAYS(_param, _dst_addr, _addr_mode,  \
                                                      _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_publish_special_days(_param, _dst_addr, _addr_mode,        \
                                                _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_cancel_calendar function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_CANCEL_CALENDAR(_param, _dst_addr, _addr_mode,  \
                                                 _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_cancel_calendar(_param, _dst_addr, _addr_mode,        \
                                           _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_get_calendar function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_GET_CALENDAR(_param, _dst_addr, _addr_mode,  \
                                              _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_get_calendar(_param, _dst_addr, _addr_mode,        \
                                        _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_get_day_profiles function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_GET_DAY_PROFILES(_param, _dst_addr, _addr_mode,  \
                                                  _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_get_day_profiles(_param, _dst_addr, _addr_mode,        \
                                            _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_get_week_profiles function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_GET_WEEK_PROFILES(_param, _dst_addr, _addr_mode,  \
                                                   _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_get_week_profiles(_param, _dst_addr, _addr_mode,        \
                                             _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_get_seasons function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_GET_SEASONS(_param, _dst_addr, _addr_mode,  \
                                             _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_get_seasons(_param, _dst_addr, _addr_mode,        \
                                       _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_get_special_days function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_GET_SPECIAL_DAYS(_param, _dst_addr, _addr_mode,  \
                                                  _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_get_special_days(_param, _dst_addr, _addr_mode,        \
                                            _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_calendar_send_cmd_get_calendar_cancellation function
 */
#define ZB_ZCL_CALENDAR_SEND_CMD_GET_CALENDAR_CANCELLATION(_param, _dst_addr, _addr_mode,  \
                                                           _dst_ep, _src_ep, _payload)     \
  zb_zcl_calendar_send_cmd_get_calendar_cancellation(_param, _dst_addr, _addr_mode,        \
                                                     _dst_ep, _src_ep, _payload, NULL)


/** @} */ /* ZB_ZCL_CALENDAR_COMMANDS */

/** @} */ /* ZCL Calendar cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

/******************************************************************************/
/* Server */
/******************************************************************************/
/* Client */
/******************************************************************************/
/* Common */

/******************************************************************************/

void zb_zcl_calendar_init_server(void);
void zb_zcl_calendar_init_client(void);
#define ZB_ZCL_CLUSTER_ID_CALENDAR_SERVER_ROLE_INIT zb_zcl_calendar_init_server
#define ZB_ZCL_CLUSTER_ID_CALENDAR_CLIENT_ROLE_INIT zb_zcl_calendar_init_client

/******************************************************************************/
#endif /* ZB_ZCL_CALENDAR_H_ */
