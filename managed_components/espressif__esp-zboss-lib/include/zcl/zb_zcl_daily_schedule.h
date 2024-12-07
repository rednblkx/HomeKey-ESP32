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
/* PURPOSE: Daily Schedule cluster definitions
*/

#ifndef ZB_ZCL_DAILY_SCHEDULE_H_
#define ZB_ZCL_DAILY_SCHEDULE_H_
/******************************************************************************/
/******************************************************************************/
#include "zboss_api_core.h"
#include "zboss_api_aps.h"
#include "zb_zcl_common.h"

/** @cond (DOXYGEN_ZCL_SECTION && DOXYGEN_DAILY_SCHEDULE_CLUSTER) */

/** @addtogroup ZB_ZCL_DAILY_SCHEDULE
  *  @{
  *   @details
  *   The Daily Schedule cluster allows information that can be scheduled for an entire day
  *   to be transferred within the premises. The information is distributed by an ESI
  *   or similar server device.
  */

/* Cluster ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE */

/** @defgroup ZB_ZCL_DAILY_SCHEDULE_ATTRS Daily Schedule cluster attributes
 * @{
 */


/** The attributes defined in this cluster are arranged into sets of related attributes;
 * each set can contain up to 256 attributes. Attribute identifiers are encoded
 * such that the most significant Octet specifies the attribute set and the least
 * significant Octet specifies the attribute within the set.
 * @see Zigbee-17-05035-004 Table D-3
 */
typedef enum zb_zcl_daily_schedule_srv_attr_sets_e
{
  ZB_ZCL_DAILY_SCHEDULE_AUXILIARY_SWITCH_LABEL_SET = 0x00,   /**< Auxiliary Switch Label
                                                                Attribute Set */

  ZB_ZCL_DAILY_SCHEDULE_SCHEDULE_ATTRIBUTES_SET = 0x01,      /**< Schedule Attribute Set */

  /* 0x02-0xFF – reserved */
} zb_zcl_daily_schedule_srv_attr_sets_t;


/** @see Zigbee-17-05035-004 Table D-4 – Auxiliary Switch Label Attribute Set */
typedef enum zb_zcl_daily_schedule_srv_auxiliary_switch_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_AUX_SWITCH_1_LABEL = 0x0000,  /**< The @e AuxSwitchNLabel attributes provide
                                                        a method for assigning a label to
                                                        * an Auxiliary Switch.
                                                        */
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_AUX_SWITCH_2_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_AUX_SWITCH_3_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_AUX_SWITCH_4_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_AUX_SWITCH_5_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_AUX_SWITCH_6_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_AUX_SWITCH_7_LABEL,
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_AUX_SWITCH_8_LABEL
} zb_zcl_daily_schedule_srv_auxiliary_switch_attr_set_t;


/** @see Zigbee-17-05035-004 Table D-5 – Schedule Attribute Set */
typedef enum zb_zcl_daily_schedule_srv_schedule_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_CURR_AUX_LOAD_SWITCH_STATE = 0x0100,  /**< The @e
                                                                      CurrentAuxiliaryLoadSwitchState
                                                                      attribute */
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_CURR_DELIVERED_TIER = 0x0101,    /**< The @e CurrentDeliveredTier
                                                                 attribute */
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_CURR_TIER_LABEL = 0x0102,        /**< The @e CurrentTierLabel attribute*/
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_LINKY_PEAK_PERIOD_STATUS = 0x0103, /**< The @e LinkyPeakPeriodStatus
                                                                   attribute */
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_PEAK_START_TIME = 0x0104,     /**< The @e PeakStartTime attribute */
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_PEAK_END_TIME = 0x0105,       /**< The @e PeakEndTime attribute */
  /* (O) */
  ZB_ZCL_ATTR_DAILY_SCHEDULE_CURR_TARIFF_LABEL = 0x0106,   /**< The @e CurrentTariffLabel attribute */
} zb_zcl_daily_schedule_srv_schedule_attr_set_t;


/** The @e LinkyPeakPeriodStatus attribute subfields */
typedef ZB_PACKED_PRE struct zb_zcl_daily_schedule_attr_linky_peak_period_status_s
{
  zb_bitfield_t on_peak:2;
  zb_bitfield_t peak_period_prior_notice:2;
  zb_bitfield_t next_day_color:2;
  zb_bitfield_t current_day_color:2;
} zb_zcl_daily_schedule_attr_linky_peak_period_status_t;

/** @brief Default value for Daily Schedule cluster revision global attribute (not defined anywhere) */
#define ZB_ZCL_DAILY_SCHEDULE_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/*!
  @brief Declare attribute list for Daily Schedule cluster (only cluster revision attribute)
  @param attr_list - attribute list name
*/
#define ZB_ZCL_DECLARE_DAILY_SCHEDULE_ATTR_LIST(attr_list)                            \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_DAILY_SCHEDULE) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/* Constants for Current Day Color and Next Day Color subfields */
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_COLOR_UNUSED       0b00
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_COLOR_LOW_BLUE     0b01
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_COLOR_MEDIUM_WHITE 0b10
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_COLOR_HIGH_RED     0b11

#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_PRIOR_NOTICE_NONE  0b00
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_PRIOR_NOTICE_PP1   0b01
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_PRIOR_NOTICE_PP2   0b10
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_PRIOR_NOTICE_PP3   0b11

#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_ON_PEAK_NONE       0b00
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_ON_PEAK_PERIOD_1   0b01
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_ON_PEAK_PERIOD_2   0b10
#define ZB_ZCL_ATTR_LINKY_PEAK_PERIOD_STATUS_ON_PEAK_PERIOD_3   0b11

/** @} */ /* ZB_ZCL_DAILY_SCHEDULE_ATTRS */


/** @defgroup ZB_ZCL_DAILY_SCHEDULE_COMMANDS Daily Schedule cluster commands
 * @{
 */

/** Daily Schedule cluster client command identifiers.
 *  @see Zigbee-17-05035-004 Table D-14 Commands Received by the Daily Schedule Cluster Server
 */
typedef enum zb_zcl_daily_schedule_cli_cmd_e
{
  /* (O) */
  ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE       = 0x00,  /**< This command initiates
                                                               PublishSchedule command(s) for
                                                               specified Schedule updates. */
  /* (O) */
  ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_DAY_PROFILE    = 0x01,  /**< This command initiates one or more
                                                               PublishDayProfile commands for the
                                                               referenced Schedule. */
  /* (O) */
  ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE_CANCELLATION = 0x05, /**< This command initiates
                                                                    the return of the last
                                                                    CancelSchedule command held
                                                                    on the associated server. */
} zb_zcl_daily_schedule_cli_cmd_t;


/** @see Zigbee-17-05035-004 Table D-3 Daily Schedule Cluster Attribute Sets */
typedef enum zb_zcl_daily_schedule_srv_cmd_e
{
  /* (O) */
  ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_SCHEDULE    = 0x00,  /**< The PublishSchedule command is
                                                                published in response to a GetSchedule
                                                                command or if new schedule information
                                                                is available.  */
  /* (O) */
  ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_DAY_PROFILE = 0x01,  /**< The PublishDayProfile command is
                                                                published in response to
                                                                a GetDayProfile command.  */
  /* (O) */
  ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_SCHEDULE     = 0x05,  /**< The CancelSchedule command indicates
                                                                that all data associated with
                                                                a particular schedule instance
                                                                should be discarded. */
  /* (O) */
  ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_ALL_SCHEDULES = 0x06,  /**< The CancelAllSchedules command indicates
                                                                that all data associated with
                                                                all schedules should be discarded. */
} zb_zcl_daily_schedule_srv_cmd_t;

/* Daily schedule cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE_SERVER_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_SCHEDULE,      \
                                      ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_DAY_PROFILE,   \
                                      ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_SCHEDULE,       \
                                      ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_ALL_SCHEDULES

#define ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE_CLIENT_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE,          \
                                      ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_DAY_PROFILE,      \
                                      ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE_CANCELLATION

#define ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE_CLIENT_ROLE_GENERATED_CMD_LIST


/** Schedule type enumeration.
 *  @see Zigbee-17-05035-004 Table D-12 Schedule Type Enumeration
 */
typedef enum zb_zcl_daily_schedule_type_e
{
  ZB_ZCL_DAILY_SCHEDULE_TYPE_LINKY_SCHEDULE             = 0x00,  /**< Linky Schedule */
  /* reserved 0x01 - 0xFF */
} zb_zcl_daily_schedule_type_t;


/** Check if schedule type is valid
 * @param _t - @ref zb_zcl_daily_schedule_get_schedule_payload_t::schedule_type field
 * @see @ref zb_zcl_daily_schedule_get_schedule_payload_t
 */
#define ZB_ZCL_DAILY_SCHEDULE_TYPE_IS_VALID(_t) \
  (_t <= ZB_ZCL_DAILY_SCHEDULE_TYPE_LINKY_SCHEDULE)


/** @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE "GetSchedule" command payload
 *  @see Zigbee-17-05035-004, D.9.2.4.1.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_daily_schedule_get_schedule_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity provider.
   * This field allows differentiation in deregulated markets where multiple commodity
   * providers may be available.
   */
  zb_uint32_t provider_id;

  /** UTC Timestamp indicating the earliest start time of schedules to be
   *  returned by the corresponding PublishSchedule command.
   */
  zb_uint32_t earliest_start_time;

  /** A 32-bit integer representing the minimum Issuer Event ID of schedules
   * to be returned by the corresponding PublishSchedule command
   */
  zb_uint32_t min_issuer_event_id;

  /** An 8-bit integer which represents the maximum number of PublishSchedule commands
   *  that the client is willing to receive in response to this command.
   */
  zb_uint8_t  number_of_schedules;

  /** An 8-bit enumeration identifying the type of the requested schedule.
   * @see zb_zcl_daily_schedule_type_t
   */
  zb_uint8_t  schedule_type;
} ZB_PACKED_STRUCT zb_zcl_daily_schedule_get_schedule_payload_t;


/** Macro for initialization @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE "GetSchedule" command payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_GET_SCHEDULE_PL_INIT \
  (zb_zcl_daily_schedule_get_schedule_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_DAY_PROFILE "GetDayProfile" command payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_GET_DAY_PROFILE_PL_INIT \
  (zb_zcl_daily_schedule_get_day_profile_payload_t) {0}


/** Check if @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE "GetSchedule" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_GET_SCHEDULE_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_daily_schedule_get_schedule_payload_t) ? ZB_TRUE : ZB_FALSE))


/** Check if @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_DAY_PROFILE "GetDayProfile" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_GET_DAY_PROFILE_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_daily_schedule_get_day_profile_payload_t) ? ZB_TRUE : ZB_FALSE))


/** This enumeration presents possible values of Schedule Time Reference field
  * @see Zigbee-17-05035-004, Table D-13 – Schedule Time Reference Enumeration
  */
typedef enum zb_zcl_daily_schedule_time_reference_e
{
  ZB_ZCL_DAILY_SCHEDULE_TIME_REFERENCE_UTC_TIME = 0x00,  /**< UTC time  */
  ZB_ZCL_DAILY_SCHEDULE_TIME_REFERENCE_STANDARD_TIME,    /**< Standard time */
  ZB_ZCL_DAILY_SCHEDULE_TIME_REFERENCE_LOCAL_TIME        /**< Local time */
  /* 0x03 - 0xFF is reserved */
} zb_zcl_daily_schedule_time_reference_t;


/** @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_SCHEDULE "PublishSchedule" command
 *  @see Zigbee-17-05035-004, subclause D.9.2.3.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_daily_schedule_publish_schedule_payload_s
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

  /** Unique identifier generated by the commodity Supplier to identify a particular schedule.
   */
  zb_uint32_t schedule_id;       /* (M) */

  /** Unique identifier generated by the commodity supplier.
   */
  zb_uint16_t day_id;       /* (M) */

  /** A UTC Time field to denote the time at which the published schedule becomes
   * valid. A start date/time of 0x00000000 shall indicate that the command
   * should be executed immediately.
   */
  zb_uint32_t start_time;               /* (M) */

  /** An 8-bit enumeration identifying the type of schedule published in this
   * command.
   * @see zb_zcl_daily_schedule_type_t
   */
  zb_uint8_t  schedule_type;            /* (M) */

  /** This field indicates how the Start Times contained in the schedule are to be interpreted.
   * @see zb_zcl_daily_schedule_time_reference_t
   */
  zb_uint8_t  schedule_time_reference;  /* (M) */

  /** The ScheduleName provides a method for utilities to assign a name to a schedule.
   */
  zb_uint8_t  schedule_name[1 + 12];    /* (M) */
} ZB_PACKED_STRUCT zb_zcl_daily_schedule_publish_schedule_payload_t;


/** Macro for initialization @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_SCHEDULE "PublishSchedule" command payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_PUBLISH_SCHEDULE_PL_INIT \
  (zb_zcl_daily_schedule_publish_schedule_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_PUBLISH_DAY_PROFILE_PL_INIT \
  (zb_zcl_daily_schedule_publish_day_profile_payload_t) {0}


/** Macro for initialization @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_SCHEDULE "CancelSchedule" command payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_CANCEL_SCHEDULE_PL_INIT \
  (zb_zcl_daily_schedule_cancel_schedule_payload_t) {0}


/** Compute expected size of received @ref zb_zcl_daily_schedule_publish_schedule_payload_t
 * data payload
 * @param pl - pointer to @ref zb_zcl_daily_schedule_publish_schedule_payload_t payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_PUBLISH_SCHEDULE_PL_EXPECTED_SIZE(pl) \
  (sizeof(zb_zcl_daily_schedule_publish_schedule_payload_t)-sizeof((pl)->schedule_name)+1+ZB_ZCL_GET_STRING_LENGTH((pl)->schedule_name))


/** Check if @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_SCHEDULE "PublishSchedule" command payload size is valid
 * @param pl - pointer to @ref zb_zcl_daily_schedule_publish_schedule_payload_t payload (pointer
 * to buffer beginning that contains @ref zb_zcl_daily_schedule_publish_schedule_payload_t data payload)
 * @param size - size of received data payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_PUBLISH_SCHEDULE_PL_SIZE_IS_VALID(pl, size) \
  ((size >= ZB_ZCL_DAILY_SCHEDULE_PUBLISH_SCHEDULE_PL_EXPECTED_SIZE((zb_zcl_daily_schedule_publish_schedule_payload_t *)pl)) ? ZB_TRUE : ZB_FALSE)


/** Check if @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_PUBLISH_DAY_PROFILE_PL_SIZE_IS_VALID(size) \
  ((size >= (sizeof(zb_zcl_daily_schedule_publish_day_profile_payload_t)-sizeof(void *)-sizeof(zb_uint8_t)) ? ZB_TRUE : ZB_FALSE))


/** Check if @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_SCHEDULE "CancelSchedule" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_DAILY_SCHEDULE_CANCEL_SCHEDULE_PL_SIZE_IS_VALID(size) \
  ((size >= sizeof(zb_zcl_daily_schedule_cancel_schedule_payload_t) ? ZB_TRUE : ZB_FALSE))


/** Linky Schedules Command Sub-Payload structure
 * See Zigbee-17-05035-004, D.9.2.3.2.3.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_daily_schedule_linky_schedule_entry_s
{
  /** The Start Time is represented in minutes from midnight.
   */
  zb_uint16_t start_time;                /* (M) */

  /** This is the current price tier that is valid until
   * the start time of the next Schedule Entry.
   */
  zb_uint8_t price_tier;                 /* (M) */

  /** The required status of the auxiliary switches is indicated by the state of the bits.
   * Bit0 correspond to Auxiliary Switch 1 and bit7 corresponds to Auxiliary Switch 8.
   */
  zb_uint8_t auxiliary_load_switch_state; /* (M) */
} ZB_PACKED_STRUCT zb_zcl_daily_schedule_linky_schedule_entry_t;


/** The format of schedule entry is dependent on schedule type.
 */
typedef ZB_PACKED_PRE union zb_zcl_daily_schedule_entry_u
{
  zb_zcl_daily_schedule_linky_schedule_entry_t linky_schedule_entry;
} ZB_PACKED_STRUCT zb_zcl_daily_schedule_entry_t;


/** @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command payload.
 * @see Zigbee-17-05035-004, subclause D.9.2.3.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_daily_schedule_publish_day_profile_payload_s
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

  /** Unique identifier generated by the commodity supplier. The Day ID is used as a reference to assign a Day Profile to a Daily Schedule
   */
  zb_uint16_t day_id;                   /* (M) */

  /** Unique identifier generated by the commodity Supplier to identify a particular schedule.
   */
  zb_uint32_t schedule_id;                   /* (M) */

  /** An 8-bit integer representing the total number of ScheduleEntries in this Day Profile.
   */
  zb_uint8_t total_number_of_schedule_entries;   /* (M) */

  /** The CommandIndex is used to count the payload fragments in the case where the entire payload
   * does not fit into one message. The CommandIndex starts at 0 and is incremented for each fragment
   *  belonging to the same command
   */
  zb_uint8_t command_index;                      /* (M) */

  /** In the case where the entire payload does not fit into one message, the
   * Total Number of Commands field indicates the total number of sub-commands
   * in the message.
   */
  zb_uint8_t total_number_of_commands;           /* (M) */

  /** An 8-bit enumeration identifying the type of schedule published in this
   * command.
   * @see zb_zcl_daily_schedule_type_t
   */
  zb_uint8_t  schedule_type;                     /* (M) */

  /**
   * day_schedule_entries pointer is considered as a beginning of array (of day schedule entries),
   * number_of_entries_in_this_command - as a number of entries. Please also take into account
   * overall ZCL payload size.
   * @see zb_zcl_daily_schedule_entry_t
   */
  zb_zcl_daily_schedule_entry_t *day_schedule_entries;   /* (M) */

  /** An 8-bit integer representing the number of Day Schedule entries in
   * this command
   */
   zb_uint8_t number_of_entries_in_this_command;         /* (ZBOSS) */
} ZB_PACKED_STRUCT zb_zcl_daily_schedule_publish_day_profile_payload_t;


/** @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_SCHEDULE "CancelSchedule" command payload.
 * @see Zigbee-17-05035-004, D.9.2.3.3
 */
typedef ZB_PACKED_PRE struct zb_zcl_daily_schedule_cancel_schedule_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;              /* (M) */

  /** Unique identifier generated by the commodity Supplier to identify a particular schedule.
   */
  zb_uint32_t schedule_id;              /* (M) */

  /** An 8-bit enumeration identifying the type of schedule published in this
   * command.
   * @see zb_zcl_daily_schedule_type_t
   */
  zb_uint8_t schedule_type;            /* (M) */
} ZB_PACKED_STRUCT zb_zcl_daily_schedule_cancel_schedule_payload_t;


/** @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_DAY_PROFILE "GetDayProfile" command payload
  * @see Zigbee-17-05035-004, D.9.2.4.2
  */
typedef ZB_PACKED_PRE struct zb_zcl_daily_schedule_get_day_profile_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;       /* (M) */

  /**
   * ScheduleID of the schedule to which the requested Day Profile belongs.
   */
  zb_uint16_t day_id;
} ZB_PACKED_STRUCT zb_zcl_daily_schedule_get_day_profile_payload_t;


/** Function for sending @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_SCHEDULE "PublishSchedule" command.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_daily_schedule_publish_schedule_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_daily_schedule_send_cmd_publish_schedule(zb_uint8_t param, const zb_addr_u *dst_addr,
                                               zb_aps_addr_mode_t dst_addr_mode,
                                               zb_uint8_t dst_ep, zb_uint8_t src_ep,
                                               const zb_zcl_daily_schedule_publish_schedule_payload_t *pl,
                                               zb_callback_t cb);


/** Function for send @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_daily_schedule_publish_day_profile_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_daily_schedule_send_cmd_publish_day_profile(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_daily_schedule_publish_day_profile_payload_t *pl,
  zb_callback_t cb
);


/** Function for sending @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE "GetSchedule" command.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_daily_schedule_get_schedule_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_daily_schedule_send_cmd_get_schedule(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_daily_schedule_get_schedule_payload_t *pl,
  zb_callback_t cb
);


/** Function for sending @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_DAY_PROFILE "GetDayProfile" command.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_daily_schedule_get_day_profile_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_daily_schedule_send_cmd_get_day_profile(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_daily_schedule_get_day_profile_payload_t *pl,
  zb_callback_t cb
);

/** Function for send @ref ZB_ZCL_DAILY_SCHEDULE_CLI_CMD_GET_SCHEDULE_CANCELLATION
 * "GetScheduleCancellation" command.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_daily_schedule_send_cmd_get_schedule_cancellation(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  zb_callback_t cb
);


/** Function for send @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_SCHEDULE "CancelSchedule" command.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_daily_schedule_cancel_schedule_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_daily_schedule_send_cmd_cancel_schedule(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  const zb_zcl_daily_schedule_cancel_schedule_payload_t *pl,
  zb_callback_t cb
);

/** Function for send @ref ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_ALL_SCHEDULES "CancelAllSchedules" command.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 */
void zb_zcl_daily_schedule_send_cmd_cancel_all_schedules(zb_uint8_t param,
  const zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep, zb_uint8_t src_ep,
  zb_callback_t cb
);

/** Macro for calling @ref zb_zcl_daily_schedule_send_cmd_publish_schedule function
 */
#define ZB_ZCL_DAILY_SCHEDULE_SEND_CMD_PUBLISH_SCHEDULE(_param, _dst_addr, _addr_mode, \
                                             _dst_ep, _src_ep, _payload)          \
  zb_zcl_daily_schedule_send_cmd_publish_schedule(_param, _dst_addr, _addr_mode,        \
                                            _dst_ep, _src_ep, _payload, NULL)


/** Macro for calling @ref zb_zcl_daily_schedule_send_cmd_publish_day_profile function
 */
#define ZB_ZCL_DAILY_SCHEDULE_SEND_CMD_PUBLISH_DAY_PROFILE(_param, _dst_addr, _addr_mode,  \
                                                     _dst_ep, _src_ep, _payload)     \
  zb_zcl_daily_schedule_send_cmd_publish_day_profile(_param, _dst_addr, _addr_mode,        \
                                               _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_daily_schedule_send_cmd_cancel_schedule function
 */
#define ZB_ZCL_DAILY_SCHEDULE_SEND_CMD_CANCEL_SCHEDULE(_param, _dst_addr, _addr_mode,  \
                                                 _dst_ep, _src_ep, _payload)     \
  zb_zcl_daily_schedule_send_cmd_cancel_schedule(_param, _dst_addr, _addr_mode,        \
                                           _dst_ep, _src_ep, _payload, NULL)

/** Macro for call @ref zb_zcl_daily_schedule_send_cmd_cancel_all_schedules function
 */
#define ZB_ZCL_DAILY_SCHEDULE_SEND_CMD_CANCEL_ALL_SCHEDULES(_param, _dst_addr, _addr_mode,  \
                                                 _dst_ep, _src_ep)     \
  zb_zcl_daily_schedule_send_cmd_cancel_all_schedules(_param, _dst_addr, _addr_mode,        \
                                           _dst_ep, _src_ep, NULL)

/** Macro for call @ref zb_zcl_daily_schedule_send_cmd_get_schedule function
 */
#define ZB_ZCL_DAILY_SCHEDULE_SEND_CMD_GET_SCHEDULE(_param, _dst_addr, _addr_mode,  \
                                              _dst_ep, _src_ep, _payload)     \
  zb_zcl_daily_schedule_send_cmd_get_schedule(_param, _dst_addr, _addr_mode,        \
                                        _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_daily_schedule_send_cmd_get_day_profile function
 */
#define ZB_ZCL_DAILY_SCHEDULE_SEND_CMD_GET_DAY_PROFILE(_param, _dst_addr, _addr_mode,  \
                                                  _dst_ep, _src_ep, _payload)     \
  zb_zcl_daily_schedule_send_cmd_get_day_profile(_param, _dst_addr, _addr_mode,        \
                                            _dst_ep, _src_ep, _payload, NULL)


/** Macro for call @ref zb_zcl_daily_schedule_send_cmd_get_schedule_cancellation function
 */
#define ZB_ZCL_DAILY_SCHEDULE_SEND_CMD_GET_SCHEDULE_CANCELLATION(_param, _dst_addr, _addr_mode,  \
                                                           _dst_ep, _src_ep)     \
  zb_zcl_daily_schedule_send_cmd_get_schedule_cancellation(_param, _dst_addr, _addr_mode,        \
                                                     _dst_ep, _src_ep, NULL)

/** @} */ /* ZB_ZCL_DAILY_SCHEDULE_COMMANDS */

/** @brief Default value for CurrentAuxiliaryLoadSwitchState attribute */
#define ZB_ZCL_DAILY_SCHEDULE_CURRENT_AUXILLARY_LOAD_SWITCH_STATE_DEFAULT_VALUE 0x00

/** @brief Default value for LinkyPeakPeriodStatus attribute */
#define ZB_ZCL_DAILY_SCHEDULE_LINKY_PEAK_PERIOD_STATUS_DEFAULT_VALUE 0x00

/** @brief Default value for PeakStartTime attribute */
#define ZB_ZCL_DAILY_SCHEDULE_PEAK_START_TIME_DEFAULT_VALUE ((zb_uint32_t)0xFFFFFFFF)

/** @brief Default value for PeakEndTime attribute */
#define ZB_ZCL_DAILY_SCHEDULE_PEAK_END_TIME_DEFAULT_VALUE ((zb_uint32_t)0xFFFFFFFF)

/** @brief Default value for CurrentTariffLabel attribute */
#define ZB_ZCL_DAILY_SCHEDULE_CURRENT_TARIFF_LABEL_DEFAULT_VALUE {0}
/** @} */ /* ZCL Daily Schedule cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

/******************************************************************************/
/* Server */
/******************************************************************************/
/* Client */
/******************************************************************************/
/* Common */

/******************************************************************************/

void zb_zcl_daily_schedule_init_server();
void zb_zcl_daily_schedule_init_client();
#define ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE_SERVER_ROLE_INIT zb_zcl_daily_schedule_init_server
#define ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE_CLIENT_ROLE_INIT zb_zcl_daily_schedule_init_client

/******************************************************************************/
#endif /* ZB_ZCL_DAILY_SCHEDULE_H_ */
