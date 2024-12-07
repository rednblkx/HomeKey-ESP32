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
/* PURPOSE: Energy Management cluster definitions
*/

#ifndef ZB_ZCL_ENERGY_MANAGEMENT_H_
#define ZB_ZCL_ENERGY_MANAGEMENT_H_

#include "zboss_api_core.h" /* ZBOSS core API types */
/* Include this header for zb_zcl_drlc_event_status_t import*/
#include "zcl/zb_zcl_drlc.h"

/** @cond (DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION) */

/** @addtogroup ZB_ZCL_ENERGY_MANAGEMENT
  * @{
  *   @details
  *   The Energy Management cluster provides a way of modifying DRLC events, energy consumption
  *   behavior and querying the status of DRLC events.
  *   @note The ESI is defined as the Client. The DRLC device is a Server
  *   in this case; it holds the attributes and receives commands.
  */

/** @defgroup ZB_ZCL_ENERGY_MANAGEMENT_ATTRS Energy Management cluster attributes
 *  @{
 */

/** Energy Management Server Cluster Attributes
 *  @see SE spec, subclause 12.2.2
 */
typedef enum zb_zcl_energy_management_srv_attr_e
{

  /* (M) */
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_LOAD_CONTROL_STATE = 0x0000,  /**< This attribute shall be a BitMap showing the current state of the device.
                                                              * The attribute shall be read-only.
                                                              */

  /* (M) */
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_ID,            /**< If an event is in progress (current time is between the Effective Start Time
                                                             * and Effective End Time of an event), this attribute SHALL indicate
                                                             * the Issuer Event ID of the active event.
                                                             */

  /* (M) */
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_STATUS,        /**< This attribute represents the value returned in the Event Control field of
                                                             * the latest Report Event Status command (see D.2.3.3.1). This attribute provides
                                                             * a mechanism to allow a remote device to query whether this client device is
                                                             * currently participating in a load control event.
                                                             */

  /* (M) */
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CONFORMANCE_LEVEL,           /**< This is the minimum criticality level of a DRLC event that the device will observe.
                                                             * Events with a criticality level lower than the Conformance Level
                                                             * will be opted out.
                                                             */

  /* (M) */
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_OFF_TIME,            /**< This attribute is measured in seconds and used to determine the duty
                                                             * cycling times. It is the shortest times the device should be allowed off.
                                                             */

  /* (M) */
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_ON_TIME,             /**< This attribute is measured in seconds and used to determine the duty
                                                              * cycling times. It is the shortest times the device should be allowed on.
                                                              */

  /* (M) */
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_CYCLE_PERIOD         /**< This attribute is measured in seconds and used to determine the duty
                                                             * cycling times. It is the shortest cycling period allowed for duty cycling
                                                             */
} zb_zcl_energy_management_srv_attr_t;

/** @brief Default value for Energy Management cluster revision global attribute (not defined anywhere) */
#define ZB_ZCL_ENERGY_MANAGEMENT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @endcond */ /* (DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION) */
/** @cond DOXYGEN_INTERNAL_DOC */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ENERGY_MANAGEMENT_LOAD_CONTROL_STATE(data_ptr) \
{                                                                                         \
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_LOAD_CONTROL_STATE,                                       \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_ID(data_ptr) \
{                                                                                       \
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U32,                                                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                   \
  (void*) data_ptr                                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_STATUS(data_ptr) \
{                                                                                           \
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_STATUS,                                       \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                       \
  (void*) data_ptr                                                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CONFORMANCE_LEVEL(data_ptr) \
{                                                                                        \
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CONFORMANCE_LEVEL,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                    \
  (void*) data_ptr                                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_OFF_TIME(data_ptr) \
{                                                                                       \
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_OFF_TIME,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                   \
  (void*) data_ptr                                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_ON_TIME(data_ptr) \
{                                                                                      \
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_ON_TIME,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                                \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                  \
  (void*) data_ptr                                                                     \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_CYCLE_PERIOD(data_ptr) \
{                                                                                    \
  ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_CYCLE_PERIOD,                                \
  ZB_ZCL_ATTR_TYPE_U16,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                \
  (void*) data_ptr                                                                   \
}


/** @brief Declare attribute list for Energy Management cluster
    @param attr_list - attribute list name
    @param on_off - pointer to variable to store On/Off attribute value
    @param load_control_state - pointer to variable to store load control state attribute
    @param current_event_id - pointer to variable to store current event id
    @param current_event_status - pointer to variable to store current event status attribute
    @param conformance_level - pointer to variable to store conformance level attribute
    @param minimum_off_time - pointer to variable to store minimum off time attribute
    @param minimum_on_time - pointer to variable to store minimum on time attribute
    @param minimum_cycle_period - pointer to variable to store minimum cycle period attribute
*/
#define ZB_ZCL_DECLARE_ENERGY_MANAGEMENT_ATTRIB_LIST(attr_list, load_control_state, current_event_id,           \
                                                     current_event_status, conformance_level, minimum_off_time, \
                                                     minimum_on_time, minimum_cycle_period)                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_ENERGY_MANAGEMENT)           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ENERGY_MANAGEMENT_LOAD_CONTROL_STATE, (load_control_state))     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_ID, (current_event_id))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_STATUS, (current_event_status)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CONFORMANCE_LEVEL, (conformance_level))       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_OFF_TIME, (minimum_off_time))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_ON_TIME, (minimum_on_time))           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_CYCLE_PERIOD, (minimum_cycle_period)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


#define ZB_ZCL_DECLARE_ENERGY_MANAGEMENT_ATTR_LIST_INIT  \
  (zb_zcl_energy_management_attr_t)                       \
  { .load_control_state = 0,       \
    .current_event_id = 0xFFFFFFFF, \
    .current_event_status = 0,       \
    .conformance_level = 0}

/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/** @cond (DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION) */
/**
 *  @brief Energy Management cluster attributes
 */
typedef struct zb_zcl_energy_management_attr_s
{

  /** @copydoc ZB_ZCL_ATTR_ENERGY_MANAGEMENT_LOAD_CONTROL_STATE
   * @see ZB_ZCL_ATTR_ENERGY_MANAGEMENT_LOAD_CONTROL_STATE
   */
  zb_uint8_t load_control_state;

  /** @copydoc ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_ID
   * @see ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_ID
   */
  zb_uint32_t current_event_id;

  /** @copydoc ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_STATUS
   * @see ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CURRENT_EVENT_STATUS
   */
  zb_uint8_t current_event_status;

  /** @copydoc ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CONFORMANCE_LEVEL
   * @see ZB_ZCL_ATTR_ENERGY_MANAGEMENT_CONFORMANCE_LEVEL
   */
  zb_uint8_t conformance_level;

  /** @copydoc ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_OFF_TIME
   *  @note Default values is TBD
   *  @see ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_OFF_TIME
   */
  zb_uint16_t minimum_off_time;

  /** @copydoc ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_ON_TIME
   *  @note Default values is TBD
   *  @see ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_ON_TIME
   */
  zb_uint16_t minimum_on_time;

  /** @copydoc ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_CYCLE_PERIOD
   *  @note Default values is TBD
   *  @see ZB_ZCL_ATTR_ENERGY_MANAGEMENT_MINIMUM_CYCLE_PERIOD
   */
  zb_uint16_t minimum_cycle_period;
} zb_zcl_energy_management_attr_t;


/** @brief Declare attribute list for Energy Management cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of @ref zb_zcl_energy_management_attr_t type (containing Energy Management cluster attributes)
 */
#define ZB_ZCL_DECLARE_ENERGY_MANAGEMENT_ATTR_LIST(attr_list, attrs)                          \
    ZB_ZCL_DECLARE_ENERGY_MANAGEMENT_ATTRIB_LIST(attr_list, &attrs.load_control_state,          \
      &attrs.current_event_id, &attrs.current_event_status, &attrs.conformance_level,           \
      &attrs.minimum_off_time, &attrs.minimum_on_time, &attrs.minimum_cycle_period)


/** Load Control State BitMap/Encoding
 *  @see SE spec, Table D-193
 */
typedef enum zb_zcl_energy_management_load_control_state_encoding_e
{
  ZB_ZCL_ENERGY_MANAGEMENT_STATE_RELAY_OPEN           = 1 << 0,  /**< The device being controlled has been turned off and
                                                                  * prevented from consuming electric power or another commodity,
                                                                  * either by opening a relay or some other means.
                                                                  */

  ZB_ZCL_ENERGY_MANAGEMENT_STATE_EVENT_IN_PROGRESS    = 1 << 1,  /**< There is an event in progress. The current time is between the Effective Start
                                                                  * Time and Effective End Time of the event. When this bit is set, the
                                                                  * CurrentEventStatus attribute is valid.
                                                                  */

  ZB_ZCL_ENERGY_MANAGEMENT_STATE_POWER_STABILIZING    = 1 << 2,  /**< The device has automatically reduced consumption of the commodity for an
                                                                  * automatic reason, to aid in the stability of the system.
                                                                  */

  ZB_ZCL_ENERGY_MANAGEMENT_STATE_OTHER_LOAD_REDUCTION = 1 << 3,  /**< This bit indicates the device has automatically reduced consumption of
                                                                  * the commodity for some non-consumer-initiated reason.
                                                                  */

  ZB_ZCL_ENERGY_MANAGEMENT_STATE_CURRENT_FLOW         = 1 << 4,  /**< This bit indicates that the device is currently consuming the commodity.
                                                                  * The bit not being set means either no commodity being consumption
                                                                  * or the device does not have the ability to detect consumption. Support is optional.
                                                                  */

  ZB_ZCL_ENERGY_MANAGEMENT_STATE_LOAD_CALL            = 1 << 5   /**< This bit is set if there is currently no consumption but the device under
                                                                  * control would consume power if able to. Support is optional.
                                                                  * The bit shall be cleared if not supported.
                                                                  */
} zb_zcl_energy_management_load_control_state_encoding_t;


/** Current Event Status BitMap/Encoding
 *  @see SE spec, Table D-194
 */
typedef enum zb_zcl_energy_management_current_event_status_encoding_e
{
  ZB_ZCL_ENERGY_MANAGEMENT_STATUS_RANDOMIZED_START_TIME         = 1 << 0,  /**< Set if the current event had a randomized start time. */

  ZB_ZCL_ENERGY_MANAGEMENT_STATUS_RANDOMIZED_DURATION           = 1 << 1,  /**< Set if the duration of the current event is randomized. */

  ZB_ZCL_ENERGY_MANAGEMENT_STATUS_EXTENDED_BITS_PRESENT         = 1 << 2,  /**< This bit will always return 1. This allows the field to be used in DRLC
                                                                             * Event Status messages.
                                                                             */

  ZB_ZCL_ENERGY_MANAGEMENT_STATUS_EVENT_ACTIVE                  = 1 << 3,  /**< Set if the current time lies between the Effective Start Time and
                                                                             * Effective End Time of the event identified by the CurrentEventID attribute
                                                                             */

  ZB_ZCL_ENERGY_MANAGEMENT_STATUS_DEVICE_PARTICIPATING_IN_EVENT = 1 << 4,  /**< Set if the device is (or will be when the event starts) participating in
                                                                             * the event.
                                                                             */

  ZB_ZCL_ENERGY_MANAGEMENT_STATUS_REDUCING_LOAD                 = 1 << 5,  /**< Set if the device is currently shedding load in response to a DR event.
                                                                             * Set if an active load control event is duty cycling and currently off, or if
                                                                             * the new set points or offsets are lowering demand.
                                                                             */

  ZB_ZCL_ENERGY_MANAGEMENT_STATUS_ON_AT_END_OF_EVENT            = 1 << 6   /**< Set if the device will return to using a normal load after the event has
                                                                             * completed. For example, this would be False if the device supported the On/Off
                                                                             * cluster and the On/Off attribute was set to OFF.
                                                                             */
} zb_zcl_energy_management_current_event_status_encoding_t;


/** @} */ /* ZB_ZCL_ENERGY_MANAGEMENT_ATTRS */


/** @defgroup ZB_ZCL_ENERGY_MANAGEMENT_COMMANDS Energy Management cluster commands
 * @{
 */

/** Commands are generated by Energy Management Client
 *  @see SE spec, subclause 12.2.4
 */
typedef enum zb_zcl_energy_management_cli_cmd_e
{
  ZB_ZCL_ENERGY_MANAGEMENT_CLI_CMD_MANAGE_EVENT = 0x00,   /**< The Manage Event command allows a remote device (such as an IHD or web portal)
                                                           * to change the behavior of a DRLC cluster client when responding to a
                                                           * DRLC Load Control Event.
                                                           * @see zb_zcl_energy_management_manage_event_payload_s
                                                           */
} zb_zcl_energy_management_cli_cmd_t;


/** @ref ZB_ZCL_ENERGY_MANAGEMENT_CLI_CMD_MANAGE_EVENT "ManageEvent" command Payload
 *  @see SE spec, subclause 12.2.4.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_energy_management_manage_event_payload_s
{
  /* Mandatory fields. */

  /** Unique identifier generated by the Energy provider. The value of this field
   * allows the command to be matched with a specific Load Control Event.
   */
  zb_uint32_t issuer_event_id;          /* (M) */

  /** Bit encoded field representing the DRLC client Device Class to apply the
   * current Load Control Event. Each bit, if set individually or in combination,
   * indicates the class of device(s) the Manage Event command is directed at.
   */
  zb_uint16_t device_class;             /* (M) */

  /** The DRLC client Utility Enrollment Group field can be used in conjunction
   * with the Device Class bits. It provides a mechanism to direct the Manage Event
   * command to groups of devices.
   */
  zb_uint8_t utility_enrollment_group;  /* (M) */

  /* Optional fields */

  /** Bit encoded field indicating the action(s) to be carried out in regard to
   * the associated event
   */
  zb_uint8_t actions_required;          /* (O) */
} ZB_PACKED_STRUCT zb_zcl_energy_management_manage_event_payload_t;

/** @def ZB_ZCL_ENERGY_MANAGEMENT_MANAGE_EVENT_PAYLOAD_SIZE_IS_VALID
 */
#define ZB_ZCL_ENERGY_MANAGEMENT_MANAGE_EVENT_PAYLOAD_SIZE_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_energy_management_manage_event_payload_t))

/** Action(s) Required BitMap/Encoding
 *  @see SE spec, Table D-196
 */
typedef enum zb_zcl_energy_management_actions_required_encoding_e
{
  ZB_ZCL_ENERGY_MANAGEMENT_ACTIONS_EVENT_OPT_OUT        = 0,  /**< Opt Out of Event */
  ZB_ZCL_ENERGY_MANAGEMENT_ACTIONS_OPT_INTO_EVENT       = 1,  /**< Opt Into Event */
  ZB_ZCL_ENERGY_MANAGEMENT_ACTIONS_DISABLE_DUTY_CYCLING = 2,  /**< Disable Duty Cycling */
  ZB_ZCL_ENERGY_MANAGEMENT_ACTIONS_ENABLE_DUTY_CYCLING  = 3   /**< Enable Duty Cycling */
} zb_zcl_energy_management_actions_required_encoding_t;


/** Commands are generated by Energy Management Server
 *  @see SE spec, subclause 12.2.5
 */
typedef enum zb_zcl_energy_management_srv_cmd_e
{
  ZB_ZCL_ENERGY_MANAGEMENT_SRV_CMD_REPORT_EVENT_STATUS = 0x00,    /**< This command is reused from the DRLC cluster. This command is generated
                                                                   * in response to the Manage Event command.
                                                                   * @see zb_zcl_energy_management_report_event_status_payload_s
                                                                   */
} zb_zcl_energy_management_srv_cmd_t;

/* Energy mgmt cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT_SERVER_ROLE_GENERATED_CMD_LIST \
                                      ZB_ZCL_ENERGY_MANAGEMENT_SRV_CMD_REPORT_EVENT_STATUS

#define ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT_CLIENT_ROLE_GENERATED_CMD_LIST \
                                      ZB_ZCL_ENERGY_MANAGEMENT_CLI_CMD_MANAGE_EVENT

#define ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT_CLIENT_ROLE_GENERATED_CMD_LIST



/** @ref ZB_ZCL_ENERGY_MANAGEMENT_SRV_CMD_REPORT_EVENT_STATUS "ReportEventStatusCommandPayload".
 *  @see SE spec, subclause 12.2.5.1 and @ref zb_zcl_drlc_report_event_status_payload_t
 */
typedef ZB_PACKED_PRE struct zb_zcl_energy_management_report_event_status_payload_s
{
  /* Mandatory fields. */

  /** Event ID specified in the Manage Event command or, if the command specified
   * the current event, then the current running DRLC Issuer Event ID.
   */
  zb_uint32_t issuer_event_id;                           /* (M) */

  /** If the Manage Event command did not change the event, then this field will
   * contain the last sent Event Status for the event.
   */
  zb_uint8_t event_status;                               /* (M) */

  /** UTC Timestamp representing when the event status was changed. If the device
   * does not know the current time but still remembers events, it may return the
   * value of 0x00000000.
   */
  zb_uint32_t event_status_time;                         /* (M) */

  /** Criticality Level value applied by the device */
  zb_uint8_t criticality_level_applied;                  /* (M) */

  /* Optional fields */

  /** Cooling Temperature Set Point value applied by the device, see the
   * corresponding field in the Load Control Event command for more information.
   * The value 0x8000 means that this field has not been used by the end device.
   */
  zb_uint16_t cooling_temperature_set_point_applied;     /* (O) */

  /** Heating Temperature Set Point value applied by the device, see the
   * corresponding field in the Load Control Event command for more information.
   * The value 0x8000 means that this field has not been used by the end device.
   */
  zb_uint16_t heating_temperature_set_point_applied;     /* (O) */

  /** Average Load Adjustment Percentage value applied by the device, see the
   *  corresponding field in the Load Control Event command for more information.
   *  The value 0x80 means that this field has not been used by the end device.
   */
  zb_int8_t average_load_adjustment_percentage_applied;  /* (O) */

  /** Defines the maximum On state duty cycle applied by the device. The value
   * 0xFF means that this field has not been used by the end device.
   */
  zb_uint8_t duty_cycle_applied;                         /* (O) */

  /* Mandatory field */
  /** Identifies additional control options for the event. */
  zb_uint8_t event_control;                              /* (M) */
} ZB_PACKED_STRUCT zb_zcl_energy_management_report_event_status_payload_t;

/** @def ZB_ZCL_ENERGY_MANAGEMENT_REPORT_EVENT_STATUS_PAYLOAD_SIZE_IS_VALID
 */
#define ZB_ZCL_ENERGY_MANAGEMENT_REPORT_EVENT_STATUS_PAYLOAD_SIZE_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_energy_management_report_event_status_payload_t))

/** Macro for initialization @ref ZB_ZCL_ENERGY_MANAGEMENT_SRV_CMD_REPORT_EVENT_STATUS "ReportEventStatus" command payload
 * @see @ref zb_zcl_energy_management_report_event_status_payload_t
 */
#define ZB_ZCL_ENERGY_MANAGEMENT_REPORT_EVENT_STATUS_PAYLOAD_INIT    \
(zb_zcl_energy_management_report_event_status_payload_t)              \
{                                                                \
  .cooling_temperature_set_point_applied = (zb_uint16_t) 0x8000,  \
  .heating_temperature_set_point_applied = (zb_uint16_t) 0x8000,  \
  .average_load_adjustment_percentage_applied = (zb_int8_t) 0x80, \
  .duty_cycle_applied = (zb_uint8_t) 0xFF,                        \
}


/** Macro for initialization @ref ZB_ZCL_ENERGY_MANAGEMENT_CLI_CMD_MANAGE_EVENT "ManageEvent" command payload
 * @see @ref zb_zcl_energy_management_manage_event_payload_t
 */
#define ZB_ZCL_ENERGY_MANAGEMENT_MANAGE_EVENT_PAYLOAD_INIT   \
  (zb_zcl_energy_management_manage_event_payload_t)       {0} \


/** Function for send @ref ZB_ZCL_ENERGY_MANAGEMENT_SRV_CMD_REPORT_EVENT_STATUS "ReportEventStatus" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with @ref ZB_ZCL_ENERGY_MANAGEMENT_MANAGE_EVENT_CB_ID
 * callback id on reception of @ref ZB_ZCL_ENERGY_MANAGEMENT_CLI_CMD_MANAGE_EVENT "ManageEvent" command.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_ENERGY_MANAGEMENT_REPORT_EVENT_STATUS_CB_ID callback id.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (ref to @ref
 * zb_zcl_drlc_report_event_status_payload_t and @ref zb_zcl_energy_management_report_event_status_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 * @par Usage
 * @n Handle @ref ZB_ZCL_ENERGY_MANAGEMENT_CLI_CMD_MANAGE_EVENT "ManageEvent" command
 */
void zb_zcl_energy_management_server_send_report_event_status(zb_uint8_t param,
                                                             zb_addr_u *dst_addr,
                                                             zb_aps_addr_mode_t dst_addr_mode,
                                                             zb_uint8_t dst_ep,
                                                             zb_uint8_t src_ep,
                                                             zb_zcl_energy_management_report_event_status_payload_t *payload,
                                                             zb_callback_t cb);


/** Function for send @ref ZB_ZCL_ENERGY_MANAGEMENT_CLI_CMD_MANAGE_EVENT "ManageEvent" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_ENERGY_MANAGEMENT_MANAGE_EVENT_CB_ID callback id.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (@ref zb_zcl_energy_management_manage_event_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 * @par Usage
 * @n Example of sending @ref ZB_ZCL_ENERGY_MANAGEMENT_CLI_CMD_MANAGE_EVENT "ManageEvent" command
 */
void zb_zcl_energy_management_client_send_manage_event(zb_uint8_t param, zb_addr_u *dst_addr,
                                                      zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                      zb_uint8_t src_ep,
                                                      zb_zcl_energy_management_manage_event_payload_t *payload,
                                                      zb_callback_t cb);

/** Macro for call @ref zb_zcl_energy_management_server_send_report_event_status command  */
#define ZB_ZCL_ENERGY_MANAGEMENT_SEND_CMD_REPORT_EVENT_STATUS(_param, _dst_addr,               \
                                                              _dst_addr_mode, _dst_ep,         \
                                                              _src_ep, _payload)               \
  zb_zcl_energy_management_server_send_report_event_status(_param, _dst_addr, _dst_addr_mode,  \
                                                           _dst_ep, _src_ep, _payload, NULL)

/** Macro for call @ref zb_zcl_energy_management_client_send_manage_event command  */
#define ZB_ZCL_ENERGY_MANAGEMENT_SEND_CMD_MANAGE_EVENT(_param, _dst_addr,              \
                                                       _dst_addr_mode, _dst_ep,        \
                                                       _src_ep, _payload)              \
  zb_zcl_energy_management_client_send_manage_event(_param, _dst_addr, _dst_addr_mode, \
                                                    _dst_ep, _src_ep, _payload, NULL)

/** @} */ /* ZB_ZCL_ENERGY_MANAGEMENT_COMMANDS */

/** @} */ /* ZCL Energy Management cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */

/** Internal handler for Energy Management Cluster commands */

void zb_zcl_energy_management_init_server(void);
void zb_zcl_energy_management_init_client(void);
#define ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT_SERVER_ROLE_INIT zb_zcl_energy_management_init_server
#define ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT_CLIENT_ROLE_INIT zb_zcl_energy_management_init_client

#endif /* ZB_ZCL_ENERGY_MANAGEMENT_H_ */
