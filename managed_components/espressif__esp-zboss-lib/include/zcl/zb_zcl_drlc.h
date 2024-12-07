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
/* PURPOSE: Demand Response and Load Control cluster definitions
*/

#ifndef ZB_ZCL_DRLC_H_
#define ZB_ZCL_DRLC_H_

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_DRLC
 *  @{
 *    @details This cluster provides an interface to the functionality of Smart
 *    Energy Demand Response and Load Control.
 */


/* Cluster ZB_ZCL_CLUSTER_ID_DRLC */

/** @defgroup ZB_ZCL_DRLC_ATTRS_AND_ENUMS DRLC attributes and enumerations
 *  @{
 *    @details
 *    This section describes DRLC client attributes and common cluster enumerations.
 */

/** @brief Default value for DRLC cluster revision global attribute */
#define ZB_ZCL_DRLC_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Maximal value for implemented DRLC cluster revision global attribute */
#define ZB_ZCL_DRLC_CLUSTER_REVISION_MAX ZB_ZCL_DRLC_CLUSTER_REVISION_DEFAULT

/** DRLC Client attributes. @see SE spec, subclause D.2.3.2 */
typedef enum zb_zcl_drlc_cli_attr_e
{
  ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP        = 0x0000,   /**< UtilityEnrollmentGroup attribute.
                                                               * Provides for utilities to assign devices to groups.
                                                               */

  ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES     = 0x0001,   /**< StartRandomizationMinutes attribute.
                                                               * Represents the maximum number of minutes to be used
                                                               * when randomizing the start of an event.
                                                               */

  ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES  = 0x0002,   /**< DurationRandomizationMinutes attribute.
                                                               * Represents the maximum number of minutes to be used
                                                               * when randomizing the duration of an event.
                                                               */

  ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE              = 0x0003,   /**< DeviceClassValue attribute.
                                                               * Identifies which bits the device will match in the
                                                               * Device Class fields.
                                                               */
} zb_zcl_drlc_cli_attr_t;


/** Criticality Level value applied by the device.
 * @see SE spec, Table D-3
 */
typedef enum zb_zcl_drlc_criticality_levels_e
{
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_0 = 0x00, /**< Reserved */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_1,        /**< Green */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_2,        /**< Level 1 */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_3,        /**< Level 2 */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_4,        /**< Level 3 */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_5,        /**< Level 4 */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_6,        /**< Level 5 */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_7,        /**< Emergency */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_8,        /**< Planned Outage */
  ZB_ZCL_DRLC_CRITICALITY_LEVEL_9         /**< Service Disconnect */
} zb_zcl_drlc_criticality_levels_t;

/** DRLC device classes enumeration.
 * @see SE spec, Table D-2.
 */
typedef enum zb_zcl_drlc_device_class_e
{
  ZB_ZCL_DRLC_DEVICE_CLASS_HVAC               = 1 << 0,  /**< HVAC Compressor or Furnace */
  ZB_ZCL_DRLC_DEVICE_CLASS_STRIP_HEATER       = 1 << 1,  /**< Strip Heaters/Baseboard Heaters */
  ZB_ZCL_DRLC_DEVICE_CLASS_WATER_HEATER       = 1 << 2,  /**< Water Heater */
  ZB_ZCL_DRLC_DEVICE_CLASS_POOL_PUMP          = 1 << 3,  /**< Pool Pump/Spa/Jacuzzi */
  ZB_ZCL_DRLC_DEVICE_CLASS_SMART_APPLIANCE    = 1 << 4,  /**< Smart Appliances */
  ZB_ZCL_DRLC_DEVICE_CLASS_IRRIGATION_PUMP    = 1 << 5,  /**< Irrigation Pump */
  ZB_ZCL_DRLC_DEVICE_CLASS_MCI_LOADS          = 1 << 6,  /**< Managed Commercial & Industrial (C&I) loads */
  ZB_ZCL_DRLC_DEVICE_CLASS_SIMPLE_LOADS       = 1 << 7,  /**< Simple misc. (Residential On/Off) loads */
  ZB_ZCL_DRLC_DEVICE_CLASS_EXTERIOR_LIGHTNING = 1 << 8,  /**< Exterior Lighting */
  ZB_ZCL_DRLC_DEVICE_CLASS_INTERIOR_LIGHTNING = 1 << 9,  /**< Interior Lighting */
  ZB_ZCL_DRLC_DEVICE_CLASS_ELECTRIC_VEHICLE   = 1 << 10, /**< Electric Vehicle */
  ZB_ZCL_DRLC_DEVICE_CLASS_GENERATION_SYSTEMS = 1 << 11  /**< Generation Systems */
} zb_zcl_drlc_device_class_t;

/** DRLC Event Status enum
 * @see SE spec, Table D-9
 */
typedef enum zb_zcl_drlc_event_status_e
{
  ZB_ZCL_DRLC_EVENT_RESERVED_00 = 0x00,               /**< Reserved for future use. */
  ZB_ZCL_DRLC_EVENT_LCE_RECEIVED,                     /**< Load Control Event command received */
  ZB_ZCL_DRLC_EVENT_EVENT_STARTED,                    /**< Event started */
  ZB_ZCL_DRLC_EVENT_EVENT_COMPLETED,                  /**< Event completed */
  ZB_ZCL_DRLC_EVENT_OPT_OUT,                          /**< User has chosen to “Opt-Out”, user will not participate in this event */
  ZB_ZCL_DRLC_EVENT_OPT_IN,                           /**< User has chosen to “Opt-In”, user will participate in this event */
  ZB_ZCL_DRLC_EVENT_EVENT_CANCELLED,                  /**< The event has been cancelled */
  ZB_ZCL_DRLC_EVENT_EVENT_SUPERSEDED,                 /**< The event has been superseded */
  ZB_ZCL_DRLC_EVENT_EVENT_PARTIALLY_OPT_OUT,          /**< Event partially completed with User “Opt-Out”. */
  ZB_ZCL_DRLC_EVENT_EVENT_PARTIALLY_OPT_IN,           /**< Event partially completed due to User “Opt-In”. */
  ZB_ZCL_DRLC_EVENT_EVENT_COMPLETED_NO_USER,          /**< Event completed, no User participation (Previous “Opt-Out”). */
  ZB_ZCL_DRLC_EVENT_RESERVED_0B,                      /**< Reserved for future use. */
  ZB_ZCL_DRLC_EVENT_RESERVED_F7 = 0xF7,               /**< Reserved for future use. */
  ZB_ZCL_DRLC_EVENT_REJECTED_INVALID_CANCEL_CMD,      /**< Rejected - Invalid Cancel Command (Default) */
  ZB_ZCL_DRLC_EVENT_REJECTED_INVALID_EFFECTIVE_TIME,  /**< Rejected - Invalid Cancel Command (Invalid Effective Time) */
  ZB_ZCL_DRLC_EVENT_RESERVED_FA,                      /**< Reserved */
  ZB_ZCL_DRLC_EVENT_REJECTED_EXPIRED,                 /**< Rejected - Event was received after it had expired@n (Current Time > Start Time + Duration) */
  ZB_ZCL_DRLC_EVENT_RESERVED_FC,                      /**< Reserved for future use. */
  ZB_ZCL_DRLC_EVENT_REJECTED_UNDEFINED_EVENT,         /**< Rejected - Invalid Cancel Command (Undefined Event) */
  ZB_ZCL_DRLC_EVENT_REJECTED,                         /**< Load Control Event command Rejected */
  ZB_ZCL_DRLC_EVENT_RESERVED_FF                       /**< Reserved for future use. */
} zb_zcl_drlc_event_status_t;

/** DRLC signature type enum */
typedef enum zb_zcl_drlc_signature_type_e
{
  ZB_ZCL_DRLC_LCE_NO_SIGNATURE = 0x00,  /**< No Signature */
  ZB_ZCL_DRLC_LCE_SIGNATURE_ECDSA,      /**< ECDSA */
  ZB_ZCL_DRLC_LCE_SIGNATURE_RESERVED    /**< Reserved */
} zb_zcl_drlc_signature_type_t;

/** @} */ /* ZB_ZCL_DRLC_ATTRS_AND_ENUMS */


/** @defgroup ZB_ZCL_DRLC_COMMANDS_STRUCTURES_AND_DEFINITIONS DRLC commands
 *  @{
 *    @details
 *    This section describes all commands structures and data payloads.
 */


/** Commands are generated by DRLC Server.
 * @see SE spec, Table D-1
 */
typedef enum zb_zcl_drlc_srv_cmd_e
{
  ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT              = 0x00,  /**< LoadControlEvent is generated when the ESI wants to control one or more
                                                                * load control devices usually as the result of an energy curtailment
                                                                * command from the Smart Energy Network.
                                                                * @see zb_zcl_drlc_lce_payload_s
                                                                */

  ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT       = 0x01,  /**< CancelLoadControlEvent is generated when the ESI wants to cancel
                                                                * previously scheduled control of one or more load control devices, usually
                                                                * as the result of an energy curtailment command from the Smart Energy
                                                                * Network
                                                                * @see zb_zcl_drlc_cancel_lce_payload_s
                                                                */

  ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS  = 0x02,   /**< CancellAllLoadControlEvents is generated when the ESI wants to cancel all
                                                                 * events for control device(s)
                                                                 */
} zb_zcl_drlc_srv_cmd_t;


/** Commands are generated by DRLC Client.
 * @see SE spec, Table D-8
 */
typedef enum zb_zcl_drlc_cli_cmd_e
{
  ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS             = 0x00,  /**< ReportEventStatus is generated when the client device detects a change of
                                                                * state for an active LoadControl event.
                                                                * @see zb_zcl_drlc_report_event_status_payload_s
                                                                */

  ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS            = 0x01,  /**< GetScheduledEvents is generated when the client device wishes to verify
                                                                * the available LoadControl events.
                                                                * @see zb_zcl_drlc_get_scheduled_events_payload_s
                                                                */
} zb_zcl_drlc_cli_cmd_t;

/** @cond internals_doc */
/* DRLC cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_DRLC_SERVER_ROLE_GENERATED_CMD_LIST                          \
                                      ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT,          \
                                      ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT,   \
                                      ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS

#define ZB_ZCL_CLUSTER_ID_DRLC_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_DRLC_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_DRLC_CLIENT_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS,   \
                                      ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS

#define ZB_ZCL_CLUSTER_ID_DRLC_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_DRLC_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/* Payload description */

/** @ref ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT "LoadControlEvent" command payload.
 * @see SE spec, Figure D-2
 */
typedef ZB_PACKED_PRE struct zb_zcl_drlc_lce_payload_s {

  /** Unique identifier generated by the Energy provider.
   * The value of this field allows matching of Event reports
   * with a specific Demand Response and Load Control event.
   */
  zb_uint32_t issuer_event_id;                       /* (M) */

  /** Bit encoded field representing the Device Class to apply the
   * current Load Control Event. Each bit, if set individually or
   * in combination, indicates the class device(s) needing to
   * participate in the event.
   */
  zb_uint16_t device_class;                          /* (M) */

  /** This field can be used in conjunction with the Device Class bits
    * for providing a mechanism to direct Load Control Events
    * to groups of Devices.
    */
  zb_uint8_t utility_enrollment_group;               /* (M) */

  /** UTC Timestamp representing when the event is scheduled to start.
    * A start time of 0x00000000 is a special time denoting “now.”
    */
  zb_uint32_t start_time;                            /* (M) */

  /** Duration of this event in number of minutes.
    * Maximum value is 1440 (one day).
    */
  zb_uint16_t duration_in_minutes;                   /* (M) */

  /** This field defines the level of criticality of event. The
    * action for an event can be solely based on this value,
    * or combination with other Load Control Event fields.
    */
  zb_uint8_t criticality_level;                      /* (M) */

  /** Requested offset to apply to the normal cooling setpoint
    * at the time of the start of the event in + 0.1 Celsius.
    */
  zb_uint8_t cooling_temperature_offset;             /* (O) - use 0xff if optional */

  /** Requested offset to apply to the normal heating setpoint
    * at the time of the start of the event in + 0.1 Celsius.
    */
  zb_uint8_t heating_temperature_offset;             /* (O) - use 0xff if optional */

  /** Requested cooling set point in 0.01 degrees Celsius. */
  zb_int16_t cooling_temperature_set_point;          /* (O) - use 0x8000 if optional */

  /** Requested heating set point in 0.01 degrees Celsius. */
  zb_int16_t heating_temperature_set_point;          /* (O) - use 0x8000 if optional */

  /** Defines a maximum energy usage limit as a percentage of
    * the client implementations specific average energy usage.
    */
  zb_int8_t average_load_adjustment_percentage;      /* (O) */

  /** Defines the maximum on state duty cycle as a percentage of time. */
  zb_uint8_t duty_cycle;                             /* (O) */

  /** Identifies additional control options for the event. */
  zb_uint8_t event_control;                          /* (M) */
} ZB_PACKED_STRUCT zb_zcl_drlc_lce_payload_t;

/** @def ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT_IS_VALID
 */
#define ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_drlc_lce_payload_t))

/** @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT "CancelLoadControlEvent" command payload
 * @see SE spec, Figure D-3
 */
typedef ZB_PACKED_PRE struct zb_zcl_drlc_cancel_lce_payload_s {
  /* Mandatory fields. */

  /** Unique identifier generated by the Energy provider. The value of
    * this field allows matching of Event reports with a specific Demand Response
    * and Load Control event.
    */
  zb_uint32_t issuer_event_id;                       /* (M) */

  /** Bit encoded field representing the Device Class to apply the
    * current Load Control Event. Each bit, if set individually or in combination,
    * indicates the class device(s) needing to participate in the event.
    */
  zb_uint16_t device_class;                          /* (M) */

  /** This field can be used in conjunction with the Device Class bits
    * for providing a mechanism to direct Load Control Events
    * to groups of Devices.
    */
  zb_uint8_t utility_enrollment_group;               /* (M) */

  /** This field is used to indicate that the Event is currently in process and
    * a cancel command is received.*/
  zb_uint8_t cancel_control;                         /* (M) */

  /** UTC Timestamp representing when the canceling of the event is
    * scheduled to start. An effective time of 0x00000000 is a special
    * time denoting “now.”
    */
  zb_uint32_t effective_time;                        /* (M) */
} ZB_PACKED_STRUCT zb_zcl_drlc_cancel_lce_payload_t;

/** @def ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT_IS_VALID
 */
#define ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_drlc_cancel_lce_payload_t))


/** @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS "CancelAllLoadControlEvents" command payload
 * @see SE spec, Figure D-3
 */
typedef ZB_PACKED_PRE struct zb_zcl_drlc_cancel_alce_payload_s {
  /* Mandatory fields. */

  /** Where the Cancel Control field indicates that randomization is to be used, the receiving device should first
    * check whether Duration Time was to be randomized and, if so, termination of the event should be adjusted
    * according to the value of the DurationRandomizationMinutes attribute. 
    */
  zb_uint8_t cancel_control;                       /* (M) */
} ZB_PACKED_STRUCT zb_zcl_drlc_cancel_alce_payload_t;

/** @def ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS_IS_VALID
 */
#define ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_drlc_cancel_alce_payload_t))

/** @ref ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS "ReportEventStatus" command payload
 * @see SE spec, Figure D-5
 */
typedef ZB_PACKED_PRE struct zb_zcl_drlc_report_event_status_payload_s {
  /* Mandatory fields. */

  /** Unique identifier generated by the Energy provider. The value of
    * this field allows matching of Event reports with a specific Demand Response
    * and Load Control event.
    */
  zb_uint32_t issuer_event_id;                          /* (M) */

  /** This field stores value from set of event statuses*/
  zb_uint8_t event_status;                              /* (M) */

  /** UTC Timestamp representing when the event status
    * occurred. This field shall not use the value of 0x00000000.
    */
  zb_uint32_t event_status_time;                        /* (M) */

  /** Criticality Level value applied by the device */
  zb_uint8_t criticality_level_applied;                 /* (M) */

  /** Defines Cooling Temperature Set Point value applied by the device.
    * The value 0x8000 means that this field has not been used by the end device.
    */
  zb_uint16_t cooling_temperature_set_point_applied;    /* (O) */

  /** Defines Heating Temperature Set Point value applied by the device.
    * The value 0x8000 means that this field has not been used by
    * the end device.
    */
  zb_uint16_t heating_temperature_set_point_applied;    /* (O) */

  /** Defines Average Load Adjustment Percentage value applied by the device.
    * The value 0x80 means that this field has not been used by the end device.
    */
  zb_int8_t average_load_adjustment_percentage_applied; /* (O) */

  /** Defines the maximum on state duty cycle applied by the device.
    * The value 0xFF means that this field has not been used by the end device.
    */
  zb_uint8_t duty_cycle_applied;                        /* (O) */

  /** Identifies additional control options for the event. */
  zb_uint8_t event_control;                             /* (M) */

  /** This field is used to enumerate the type of algorithm use to create the signature.*/
  zb_uint8_t signature_type;                            /* (M) */

  /** A non-repudiation signature created by using the Matyas-Meyer-Oseas
    * hash function used in conjunction with ECDSA.
    */
  zb_uint8_t signature[42];                             /* (O) */
} ZB_PACKED_STRUCT zb_zcl_drlc_report_event_status_payload_t;

/** @def ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS_IS_VALID
 */
#define ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_drlc_report_event_status_payload_t)) 

/** @ref ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS "GetScheduledEvents" command payload
 * @see SE spec, Figure D-6
 */
typedef ZB_PACKED_PRE struct zb_zcl_drlc_get_scheduled_events_payload_s {
  /* Mandatory fields. */

  /** UTC Timestamp representing the minimum Start Time of events that shall be
    * matched and sent by the Server. A Start Time of 0x00000000 has no special
    * meaning.
    */
  zb_uint32_t start_time;                        /* (M) */

  /** Represents the maximum number of events to be sent. A value of 0 indicates
    * no maximum limit.
    */
  zb_uint8_t number_of_events;                   /* (M) */

  /** Represents the minimum Issuer Event ID of events to be matched and sent
    * by the server with the same Start Time as the Get Scheduled Events command.
    * A value of 0xFFFFFFFF indicates this field will not be used.
    */
  zb_uint32_t issuer_event_id;                   /* (O) */
} ZB_PACKED_STRUCT zb_zcl_drlc_get_scheduled_events_payload_t;

/** Initialize @ref ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT "LoadControlEvent" command @ref zb_zcl_drlc_lce_payload_t payload*/
#define ZB_ZCL_DRLC_LCE_PAYLOAD_INIT                \
  (zb_zcl_drlc_lce_payload_t)                       \
{                                                   \
  .cooling_temperature_offset         = 0xFF,       \
  .heating_temperature_offset         = 0xFF,       \
  .cooling_temperature_set_point      = (zb_int16_t) 0x8000,     \
  .heating_temperature_set_point      = (zb_int16_t) 0x8000,     \
  .average_load_adjustment_percentage = (zb_int8_t) 0x80,       \
  .duty_cycle                         = 0xFF,       \
}

/** Initialize @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT "CancelLoadControlEvent" command @ref zb_zcl_drlc_cancel_lce_payload_t payload */
#define ZB_ZCL_DRLC_CANCEL_LCE_PAYLOAD_INIT \
  (zb_zcl_drlc_cancel_lce_payload_t) {0}

/** Initialize @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS "CancelAllLoadControlEvents" command @ref zb_zcl_drlc_cancel_alce_payload_t payload */
#define ZB_ZCL_DRLC_CANCEL_ALCE_PAYLOAD_INIT \
  (zb_zcl_drlc_cancel_alce_payload_t) {0}

/** Initialize @ref ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS "ReportEventStatus" command @ref zb_zcl_drlc_report_event_status_payload_t payload */
#define ZB_ZCL_DRLC_REPORT_EVENT_STATUS_PAYLOAD_INIT    \
  (zb_zcl_drlc_report_event_status_payload_t)           \
{                                                       \
  .cooling_temperature_set_point_applied = (zb_uint16_t) 0x8000,       \
  .heating_temperature_set_point_applied = (zb_uint16_t) 0x8000,       \
  .average_load_adjustment_percentage_applied = (zb_int8_t) 0x80,   \
  .duty_cycle_applied = (zb_uint8_t) 0xFF,                           \
  .signature =                                          \
  {                                                     \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,                 \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,                 \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,                 \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,                 \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,                 \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,                 \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,                 \
  },                                                    \
}

/** Check if some size in range of variable size of specified payload.
 */
#define ZB_ZCL_DRLC_GET_SCHEDULED_EVENTS_PAYLOAD_SIZE_IS_VALID(size) \
((size) >= ((zb_int16_t)sizeof(zb_zcl_drlc_get_scheduled_events_payload_t) - \
(zb_int16_t)ZB_SIZEOF_FIELD(zb_zcl_drlc_get_scheduled_events_payload_t, issuer_event_id)))

/** Initialize @ref ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS "GetScheduledEvents" command @ref zb_zcl_drlc_get_scheduled_events_payload_t payload */
#define ZB_ZCL_DRLC_CMD_GET_SCHEDULED_EVENTS_PAYLOAD_INIT \
  (zb_zcl_drlc_get_scheduled_events_payload_t)            \
{                                                         \
  .issuer_event_id = 0xFFFFFFFF,                          \
}


/** Function for send @ref ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT "LoadControlEvent" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with @ref ZB_ZCL_DRLC_GET_SCHEDULED_EVENTS_CB_ID
 * callback id on reception of @ref ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS "GetScheduledEvents" command.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_DRLC_LOAD_CONTROL_EVENT_CB_ID callback id.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (@ref zb_zcl_drlc_lce_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 * @par Usage
 * @n Handle @ref ZB_ZCL_DRLC_GET_SCHEDULED_EVENTS_CB_ID
 * @snippet se/energy_service_interface/se_esi_zc.c handle_get_scheduled_events
 * @n Example of sending @ref ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT "LoadControlEvent" command
 * @snippet se/energy_service_interface/se_esi_zc.c esi_dev_cmd_send_lce_event1
 */
void zb_drlc_server_send_load_control_event(zb_uint8_t param,
  zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
  zb_uint8_t src_ep, zb_zcl_drlc_lce_payload_t *payload, zb_callback_t cb);


/** Function for send @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT "CancelLoadControlEvent" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_DRLC_CANCEL_LOAD_CONTROL_EVENT_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (@ref zb_zcl_drlc_cancel_lce_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 * @par Usage
 * @n Example of sending @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT "CancelLoadControlEvent" command
 * @snippet se/energy_service_interface/se_esi_zc.c esi_dev_cmd_send_lce_event2
 */
void zb_drlc_server_send_cancel_load_control_event(zb_uint8_t param,
  zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
  zb_uint8_t src_ep, zb_zcl_drlc_cancel_lce_payload_t *payload, zb_callback_t cb);


/** Function for send @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS "CancellAllLoadControlEvents" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_DRLC_CANCEL_ALL_LOAD_CONTROL_EVENTS_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload: pointer to zb_uint8_t, where
 * "Cancel Control" field value is placed (see SE spec, D.2.2.3.3.1).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 * @par Usage
 * @n Example of sending @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS "CancellAllLoadControlEvents" command
 * @snippet se/energy_service_interface/se_esi_zc.c esi_dev_cmd_send_lce_event3
 */
void zb_drlc_server_send_cancel_all_load_control_events(zb_uint8_t param,
  zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
  zb_uint8_t src_ep, zb_uint8_t *payload, zb_callback_t cb);

/** Function for send @ref  ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS "ReportEventStatus" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with one of @ref ZB_ZCL_DRLC_LOAD_CONTROL_EVENT_CB_ID,
 * @ref ZB_ZCL_DRLC_CANCEL_LOAD_CONTROL_EVENT_CB_ID, @ref ZB_ZCL_DRLC_CANCEL_ALL_LOAD_CONTROL_EVENTS_CB_ID
 * callback ids on reception of @ref ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT "LoadControlEvent",
 * @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT "CancelLoadControlEvent",
 * @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS "CancellAllLoadControlEvents" commands respectively.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_DRLC_REPORT_EVENT_STATUS_CB_ID callback id.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (@ref zb_zcl_drlc_report_event_status_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 * @par Usage
 * @n Handle @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS "CancellAllLoadControlEvents" command
 * @snippet se/in_home_display/se_ihd_zr.c ihd_handle_cancel_all_load_control_events
 * @n Example of sending @ref ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS "ReportEventStatus" command
 * @snippet se/in_home_display/se_ihd_zr.c ihd_send_report_event_status
 */
void zb_drlc_client_send_report_event_status(zb_uint8_t param,
  zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
  zb_uint8_t src_ep, zb_zcl_drlc_report_event_status_payload_t *payload, zb_callback_t cb);

/** Function for send @ref  ZB_ZCL_DRLC_SEND_CMD_REPORT_EVENT_STATUS_TSN "ReportEventStatus" command.
 * On sender's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with one of @ref ZB_ZCL_DRLC_LOAD_CONTROL_EVENT_CB_ID,
 * @ref ZB_ZCL_DRLC_CANCEL_LOAD_CONTROL_EVENT_CB_ID, @ref ZB_ZCL_DRLC_CANCEL_ALL_LOAD_CONTROL_EVENTS_CB_ID
 * callback ids on reception of @ref ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT "LoadControlEvent",
 * @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT "CancelLoadControlEvent",
 * @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS "CancellAllLoadControlEvents" commands respectively.
 * @n On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_DRLC_REPORT_EVENT_STATUS_CB_ID callback id.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (@ref zb_zcl_drlc_report_event_status_payload_t).
 * @param tsn - transaction sequence number of response
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 * @par Usage
 * @n Handle @ref ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS "CancellAllLoadControlEvents" command
 * @snippet se/in_home_display/se_ihd_zr.c ihd_handle_cancel_all_load_control_events
 * @n Example of sending @ref ZB_ZCL_DRLC_SEND_CMD_REPORT_EVENT_STATUS_TSN "ReportEventStatus" command
 * @snippet se/in_home_display/se_ihd_zr.c ihd_send_report_event_status
 */
void zb_drlc_client_send_report_event_status_tsn(zb_uint8_t param,
  zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
  zb_uint8_t src_ep, zb_zcl_drlc_report_event_status_payload_t *payload, zb_uint8_t tsn, zb_callback_t cb);



/** Function for send @ref ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS "GetScheduledEvents" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_DRLC_GET_SCHEDULED_EVENTS_CB_ID callback id.
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (@ref zb_zcl_drlc_get_scheduled_events_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 * @par Usage
 * @n Example of sending @ref ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS "GetScheduledEvents"
 * @snippet se/in_home_display/se_ihd_zr.c ihd_dev_cmd_get_scheduled_events
 */
void zb_drlc_client_send_get_scheduled_events(zb_uint8_t param,
  zb_addr_u *dst_addr, zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
  zb_uint8_t src_ep, zb_zcl_drlc_get_scheduled_events_payload_t *payload, zb_callback_t cb);

/** Macro for call @ref zb_drlc_server_send_load_control_event function
 */
#define ZB_ZCL_DRLC_SEND_CMD_LOAD_CONTROL_EVENT(_param, \
  _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload ) \
  zb_drlc_server_send_load_control_event(_param, \
    _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, NULL)

/** Macro for call @ref zb_drlc_server_send_cancel_load_control_event function
 */
#define ZB_ZCL_DRLC_SEND_CMD_CANCEL_LCE(_param, \
  _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload ) \
  zb_drlc_server_send_cancel_load_control_event(_param, \
    _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, NULL)

/** Macro for call @ref zb_drlc_server_send_cancel_all_load_control_events function
 */
#define ZB_ZCL_DRLC_SEND_CMD_CANCEL_ALL_LCE(_param, \
  _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload ) \
  zb_drlc_server_send_cancel_all_load_control_events(_param, \
    _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, NULL)

/** Macro for call @ref zb_drlc_client_send_get_scheduled_events function
 */
#define ZB_ZCL_DRLC_SEND_CMD_GET_SCHEDULED_EVENTS(_param, \
  _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload ) \
  zb_drlc_client_send_get_scheduled_events(_param, \
    _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, NULL)

/** Macro for call @ref zb_drlc_client_send_report_event_status function
 */
#define ZB_ZCL_DRLC_SEND_CMD_REPORT_EVENT_STATUS( _param, \
  _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload ) \
  zb_drlc_client_send_report_event_status(_param, \
    _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, NULL)

/** Macro for call @ref zb_drlc_client_send_report_event_status_tsn function
 */
#define ZB_ZCL_DRLC_SEND_CMD_REPORT_EVENT_STATUS_TSN( _param, \
  _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, _tsn ) \
  zb_drlc_client_send_report_event_status_tsn(_param, \
    _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, _tsn, NULL)

/** @} */ /* ZB_ZCL_DRLC_COMMANDS_STRUCTURES_AND_DEFINITIONS */

/** @brief Declare attribute list for DRLC cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] utility_enrollment_group  - pointer to variable to store @ref ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP value
 *  @param[in] start_randomization_munutes - pointer to variable to store @ref ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES value
 *  @param[in] duration_randomization_minutes - pointer to variable to store @ref ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES value
 *  @param[in] device_class - pointer to variable to store @ref ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE value
 */
 #define ZB_ZCL_DECLARE_DRLC_ATTRIB_LIST(attr_list, utility_enrollment_group,   \
  start_randomization_munutes, duration_randomization_minutes, device_class)    \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_DRLC)     \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP, (utility_enrollment_group), ZB_ZCL_ATTR_TYPE_8BIT, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES, (start_randomization_munutes), ZB_ZCL_ATTR_TYPE_8BIT, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES, (duration_randomization_minutes), ZB_ZCL_ATTR_TYPE_8BIT, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE, (device_class), ZB_ZCL_ATTR_TYPE_16BIT, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/** Initialize @ref zb_zcl_drlc_client_attrs_s DRLC cluster's attributes */
#define ZB_ZCL_DECLARE_DRLC_ATTR_LIST_INIT       \
          (zb_zcl_drlc_client_attrs_t)           \
          { .utility_enrollment_group = 0,       \
            .start_randomization_munutes = 0x1e, \
            .duration_randomization_minutes = 0, }


/**
 *  @brief DRLC client cluster attributes
 */
typedef struct zb_zcl_drlc_client_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP
   * @see ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP
   */
  zb_uint8_t utility_enrollment_group;

  /** @copydoc ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES
   * @see ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES
   */
  zb_uint8_t start_randomization_munutes;

  /** @copydoc ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES
   * @see ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES
   */
  zb_uint8_t duration_randomization_minutes;

  /** @copydoc ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE
   * @see ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE
   */
  zb_uint16_t device_class_value;
} zb_zcl_drlc_client_attrs_t;


/** @brief Declare attribute list for DRLC cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of @ref zb_zcl_drlc_client_attrs_t type (containing DRLC cluster attributes)
 */
#define ZB_ZCL_DECLARE_DRLC_ATTR_LIST(attr_list, attrs)  \
 ZB_ZCL_DECLARE_DRLC_ATTRIB_LIST(attr_list, &attrs.utility_enrollment_group,  \
   &attrs.start_randomization_munutes, &attrs.duration_randomization_minutes, \
   &attrs.device_class_value)

/**  @} */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */
/** Internal handler for DRLC Cluster commands */

void zb_zcl_drlc_init_server(void);
void zb_zcl_drlc_init_client(void);
#define ZB_ZCL_CLUSTER_ID_DRLC_SERVER_ROLE_INIT zb_zcl_drlc_init_server
#define ZB_ZCL_CLUSTER_ID_DRLC_CLIENT_ROLE_INIT zb_zcl_drlc_init_client

#endif /* ZB_ZCL_DRLC_H_ */
