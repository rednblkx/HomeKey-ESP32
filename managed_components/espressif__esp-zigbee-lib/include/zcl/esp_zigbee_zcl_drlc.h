/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief Demand response and load control client attribute enumeration */
typedef enum esp_zb_zcl_drlc_cli_attr_e {
    ESP_ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP        = 0x0000,   /*!< UtilityEnrollmentGroup attribute. Provides for
                                                                          utilities to assign devices to groups. */
    ESP_ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES     = 0x0001,   /*!< StartRandomizationMinutes attribute. Represents
                                                                          the maximum number of minutes to be used when
                                                                          randomizing the start of an event. */
    ESP_ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES  = 0x0002,   /*!< DurationRandomizationMinutes attribute. Represents
                                                                          the maximum number of minutes to be used when randomizing
                                                                          the duration of an event. */
    ESP_ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE              = 0x0003,   /*!< DeviceClassValue attribute. Identifies which bits the
                                                                          device will match in the Device Class fields. */
} esp_zb_zcl_drlc_cli_attr_t;

/** @brief The default value of ESP_ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP_DEFAULT_VALUE 0x00
/** @brief The minimum value of ESP_ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP_MIN_VALUE 0x00
/** @brief The maximum value of ESP_ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_UTILITY_ENROLLMENT_GROUP_MAX_VALUE 0xFF

/** @brief The default value of ESP_ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES_DEFAULT_VALUE 0x1E
/** @brief The minimum value of ESP_ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES_MIN_VALUE 0x00
/** @brief The maximum value of ESP_ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_START_RANDOMIZATION_MINUTES_MAX_VALUE 0x3C

/** @brief The maximum value of ESP_ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES_DEFAULT_VALUE 0x00
/** @brief The maximum value of ESP_ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES_MIN_VALUE 0x00
/** @brief The maximum value of ESP_ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_DURATION_RANDOMIZATION_MINUTES_MAX_VALUE 0x3C

/** @brief The maximum value of ESP_ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE_DEFAULT_VALUE 0x0000
/** @brief The maximum value of ESP_ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE_MIN_VALUE 0x0000
/** @brief The maximum value of ESP_ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE attribute */
#define ESP_ZB_ZCL_ATTR_DRLC_DEVICE_CLASS_VALUE_MAX_VALUE 0xFFFF

/** @brief DRLC device classes enumeration */
typedef enum esp_zb_zcl_drlc_device_class_e {
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_HVAC               = 1 << 0,  /*!< HVAC Compressor or Furnace */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_STRIP_HEATER       = 1 << 1,  /*!< Strip Heaters/Baseboard Heaters */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_WATER_HEATER       = 1 << 2,  /*!< Water Heater */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_POOL_PUMP          = 1 << 3,  /*!< Pool Pump/Spa/Jacuzzi */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_SMART_APPLIANCE    = 1 << 4,  /*!< Smart Appliances */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_IRRIGATION_PUMP    = 1 << 5,  /*!< Irrigation Pump */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_MCI_LOADS          = 1 << 6,  /*!< Managed Commercial & Industrial (C&I) loads */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_SIMPLE_LOADS       = 1 << 7,  /*!< Simple misc. (Residential On/Off) loads */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_EXTERIOR_LIGHTNING = 1 << 8,  /*!< Exterior Lighting */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_INTERIOR_LIGHTNING = 1 << 9,  /*!< Interior Lighting */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_ELECTRIC_VEHICLE   = 1 << 10, /*!< Electric Vehicle */
    ESP_ZB_ZCL_DRLC_DEVICE_CLASS_GENERATION_SYSTEMS = 1 << 11  /*!< Generation Systems */
} esp_zb_zcl_drlc_device_class_t;

/** @brief DRLC Criticality Level value applied by the device */
typedef enum esp_zb_zcl_drlc_criticality_levels_e {
    ESP_ZB_ZCL_DRLC_CRITICALITY_LEVEL_0 = 0x00,     /*!< Reserved */
    ESP_ZB_ZCL_DRLC_CRITICALITY_GREEN,              /*!< Green */
    ESP_ZB_ZCL_DRLC_CRITICALITY_LEVEL_1,            /*!< Level 1 */
    ESP_ZB_ZCL_DRLC_CRITICALITY_LEVEL_2,            /*!< Level 2 */
    ESP_ZB_ZCL_DRLC_CRITICALITY_LEVEL_3,            /*!< Level 3 */
    ESP_ZB_ZCL_DRLC_CRITICALITY_LEVEL_4,            /*!< Level 4 */
    ESP_ZB_ZCL_DRLC_CRITICALITY_LEVEL_5,            /*!< Level 5 */
    ESP_ZB_ZCL_DRLC_CRITICALITY_EMERGENCY,          /*!< Emergency */
    ESP_ZB_ZCL_DRLC_CRITICALITY_PLANNED_OUTAGE,     /*!< Planned Outage */
    ESP_ZB_ZCL_DRLC_CRITICALITY_SERVICE_DISCONNECT  /*!< Service Disconnect */
} esp_zb_zcl_drlc_criticality_levels_t;

/** @brief Commands are generated by DRLC Server */
typedef enum esp_zb_zcl_drlc_srv_cmd_e {
    ESP_ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT              = 0x00, /*!< LoadControlEvent is generated when the ESI wants to
                                                                         control one or more load control devices usually as
                                                                         the result of an energy curtailment command from the
                                                                         Smart Energy Network. @see esp_zb_zcl_drlc_load_control_event_payload_t */
    ESP_ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT       = 0x01, /*!< CancelLoadControlEvent is generated when the ESI wants to
                                                                         cancel previously scheduled control of one or more load
                                                                         control devices, usually as the result of an energy curtailment
                                                                         command from the Smart Energy network @see esp_zb_zcl_drlc_cancel_load_control_event_payload_t */
    ESP_ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS  = 0x02, /*!< Cancel AllLoadControlEvents is generated when the ESI wants to cancel all
                                                                         events for control device(s) */
} esp_zb_zcl_drlc_srv_cmd_t;

/** @brief Commands are generated by DRLC Client */
typedef enum esp_zb_zcl_drlc_cli_cmd_e {
    ESP_ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS  = 0x00,    /*!< ReportEventStatus is generated when the client device detects a change of
                                                                 state for an active LoadControl event. @see esp_zb_zcl_drlc_report_event_status_payload_t */
    ESP_ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS = 0x01,    /*!< GetScheduledEvents is generated when the client device wishes to verify the
                                                                 available LoadControl events. @see esp_zb_zcl_drlc_get_scheduled_events_payload_t */
} esp_zb_zcl_drlc_cli_cmd_t;

/** @brief ESP_ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT LoadControlEvent command payload. */
typedef struct esp_zb_zcl_drlc_load_control_event_payload_s {
    uint32_t issuer_event_id;                 /*!< Unique identifier generated by the Energy provider.
                                                   The value of this field allows matching of Event
                                                   reports with a specific Demand Response and Load
                                                   Control event. */
    uint16_t device_class;                    /*!< Bit encoded field representing the Device Class to apply
                                                   the current Load Control Event.Each bit, if set individually
                                                   or in combination, indicates the class device(s) needing to
                                                   participate in the event. */
    uint8_t utility_enrollment_group;         /*!< This field can be used in conjunction with the Device
                                                   Class bits for providing a mechanism to direct Load
                                                   Control Events to groups of Devices. */
    uint32_t start_time;                      /*!< UTC Timestamp representing when the event is scheduled to start.
                                                   A start time of 0x00000000 is a special time denoting now. */
    uint16_t duration_in_minutes;             /*!< Duration of this event in number of minutes. Maximum value is
                                                   1440 (one day). */
    uint8_t criticality_level;                /*!< This field defines the level of criticality of event. The action
                                                   for an event can be solely based on this value, or combination with
                                                   other Load Control Event fields. */
    uint8_t cooling_temperature_offset;       /*!< Requested offset to apply to the normal cooling setpoint at the
                                                   time of the start of the event in + 0.1 Celsius. */
    uint8_t heating_temperature_offset;       /*!< Requested offset to apply to the normal heating setpoint at the
                                                   time of the start of the event in + 0.1 Celsius.*/
    int16_t cooling_temperature_set_point;    /*!< Requested cooling set point in 0.01 degrees Celsius. */
    int16_t heating_temperature_set_point;    /*!< Requested heating set point in 0.01 degrees Celsius. */
    int8_t average_load_adjustment_percentage;/*!< Defines a maximum energy usage limit as a percentage of
                                                   the client implementations specific average energy usage. */
    uint8_t duty_cycle;                       /*!< Defines the maximum on state duty cycle as a percentage of
                                                   time. */
    uint8_t event_control;                    /*!< Identifies additional control options for the event. */
} esp_zb_zcl_drlc_load_control_event_payload_t;

/** @brief ESP_ZB_ZCL_DRLC_SRV_CMD_CANCEL_LOAD_CONTROL_EVENT CancelLoadControlEvent command payload. */
typedef struct esp_zb_zcl_drlc_cancel_load_control_event_payload_s {
    uint32_t issuer_event_id;         /*!< Unique identifier generated by the Energy provider.
                                           The value of this field allows matching of Event reports
                                           with a specific Demand Response and Load Control event. */
    uint16_t device_class;            /*!< Bit encoded field representing the Device Class to apply
                                           the current Load Control Event. Each bit, if set individually
                                           or in combination, indicates the class device(s) needing to
                                           participate in the event. */
    uint8_t utility_enrollment_group; /*!< This field can be used in conjunction with the Device Class bits
                                           for providing a mechanism to direct Load Control Events to groups
                                           of Devices. */
    uint8_t cancel_control;           /*!< This field is used to indicate that the Event is currently in process
                                           and a cancel command is received */
    uint32_t effective_time;          /*!< UTC Timestamp representing when the canceling of the event is
                                           scheduled to start. An effective time of 0x00000000 is a special time
                                           denoting “now.” */
} esp_zb_zcl_drlc_cancel_load_control_event_payload_t;

/** @brief ESP_ZB_ZCL_DRLC_SRV_CMD_CANCEL_ALL_LOAD_CONTROL_EVENTS CancelAllLoadControlEvents command payload. */
typedef struct esp_zb_zcl_drlc_cancel_all_load_control_events_payload_s {
    uint8_t cancel_control;             /*!< To be used when the Event is currently in process and a cancel command is received.
                                             0x00: indicates that randomization is overridden and the event should be terminated immediately.
                                             0x01: indicates the event should end using randomization settings in the original event.
                                             Otherwise, reserved. */
} esp_zb_zcl_drlc_cancel_all_load_control_events_payload_t;

/** @brief ESP_ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS ReportEventStatus command payload */
typedef struct esp_zb_zcl_drlc_report_event_status_payload_s {
    uint32_t issuer_event_id;                          /*!< Unique identifier generated by the Energy provider.
                                                            The value of this field allows matching of Event reports
                                                            with a specific Demand Response and Load Control event. */
    uint8_t event_status;                              /*!< This field stores value from set of event statuses*/
    uint32_t event_status_time;                        /*!< UTC Timestamp representing when the event status occurred.
                                                            This field shall not use the value of 0x00000000 */
    uint8_t criticality_level_applied;                 /*!< Criticality Level value applied by the device */
    uint16_t cooling_temperature_set_point_applied;    /*!< Defines Cooling Temperature Set Point value applied by the
                                                            device. The value 0x8000 means that this field has not been
                                                            used by the end device. */
    uint16_t heating_temperature_set_point_applied;    /*!< Defines Heating Temperature Set Point value applied by the
                                                            device. The value 0x8000 means that this field has not been
                                                            used by the end device. */
    int8_t average_load_adjustment_percentage_applied; /*!< Defines Average Load Adjustment Percentage value applied by
                                                            the device. The value 0x80 means that this field has not been
                                                            used by the end device. */
    uint8_t duty_cycle_applied;                        /*!< Defines the maximum on state duty cycle applied by the device.
                                                            The value 0xFF means that this field has not been used by the
                                                            end device. */
    uint8_t event_control;                             /*!< Identifies additional control options for the event. */
    uint8_t signature_type;                            /*!< This field is used to enumerate the type of algorithm use to
                                                            create the signature.*/
    uint8_t signature[42];                             /*!< A non-repudiation signature created by using the Matyas-Meyer-Oseas
                                                            hash function used in conjunction with ECDSA. */
} esp_zb_zcl_drlc_report_event_status_payload_t;

/** @brief ESP_ZB_ZCL_DRLC_CLI_CMD_GET_SCHEDULED_EVENTS GetScheduledEvents command payload */
typedef struct esp_zb_zcl_drlc_get_scheduled_events_payload_s {
    uint32_t start_time;        /*!< UTC Timestamp representing the minimum Start Time of events that shall be matched and
                                     sent by the Server. A Start Time of 0x00000000 has no special meaning. */   
    uint8_t number_of_events;   /*!< Represents the maximum number of events to be sent. A value of 0 indicates no maximum
                                     limit. */    
    uint32_t issuer_event_id;   /*!< Represents the minimum Issuer Event ID of events to be matched and sent by the server
                                     with the same Start Time as the Get Scheduled Events command. A value of 0xFFFFFFFF
                                     indicates this field will not be used. */
} esp_zb_zcl_drlc_get_scheduled_events_payload_t;

#ifdef __cplusplus
}
#endif
