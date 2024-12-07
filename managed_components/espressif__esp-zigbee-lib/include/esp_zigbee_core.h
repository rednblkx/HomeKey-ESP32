/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "esp_err.h"
#include "zb_vendor.h"
#include "zb_config_platform.h"
#include "esp_zigbee_version.h"
#include "esp_zigbee_trace.h"
#include "esp_zigbee_type.h"
#include "esp_zigbee_attribute.h"
#include "esp_zigbee_cluster.h"
#include "esp_zigbee_endpoint.h"
#include "nwk/esp_zigbee_nwk.h"
#include "zcl/esp_zigbee_zcl_command.h"
#include "zdo/esp_zigbee_zdo_command.h"
#include "bdb/esp_zigbee_bdb_touchlink.h"
#include "bdb/esp_zigbee_bdb_commissioning.h"
#include "esp_zigbee_secur.h"
#include "esp_zigbee_ota.h"

#define ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK 0x07FFF800U /*!< channel 11-26 for compatibility with 2.4GHZ*/

#ifdef CONFIG_ZB_ZED
#define ESP_ZB_SLEEP_MAXIMUM_THRESHOLD_MS 86400000U /*! Maximum sleep threshold*/
#endif                                              /** CONFIG_ZB_ZED */

#ifdef CONFIG_ZB_GP_ENABLED
#define ESP_ZGP_GPPB_DEFAULT_FUNCTIONALITY 0x9ac2f /*!< GPP functionality, refer to esp_zgp_gpp_functionality_t */
#define ESP_ZGP_GPS_DEFAULT_FUNCTIONALITY 0x9ac2f /*!< GPS functionality, refer to esp_zgp_gps_functionality_t */
#endif /* CONFIG_ZB_GP_ENABLED */

/**
 * @name End Device (ED) timeout request
 * @anchor nwk_requested_timeout
 */
typedef enum {
    ESP_ZB_ED_AGING_TIMEOUT_10SEC    = 0U, /*!< 10 second timeout */
    ESP_ZB_ED_AGING_TIMEOUT_2MIN     = 1U, /*!< 2 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_4MIN     = 2U, /*!< 4 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_8MIN     = 3U, /*!< 8 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_16MIN    = 4U, /*!< 16 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_32MIN    = 5U, /*!< 32 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_64MIN    = 6U, /*!< 64 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_128MIN   = 7U, /*!< 128 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_256MIN   = 8U, /*!< 256 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_512MIN   = 9U, /*!< 512 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_1024MIN  = 10U, /*!< 1024 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_2048MIN  = 11U, /*!< 2048 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_4096MIN  = 12U, /*!< 4096 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_8192MIN  = 13U, /*!< 8192 minutes */
    ESP_ZB_ED_AGING_TIMEOUT_16384MIN = 14U, /*!< 16384 minutes */
} esp_zb_aging_timeout_t;

/**
 * @brief Enum of the Zigbee core action callback id
 *
 * @note
 *    1. If one endpoint possesses the same custom cluster identifier in both client and server roles,
 *       any request or response command for the custom cluster will only trigger the
 *       ``ESP_ZB_CORE_CMD_CUSTOM_CLUSTER_REQ_CB_ID`` callback.
 *    2. The callback ids without ``CMD`` in their names would provide messages of the following structure:
 *       @code{c}
 *       typedef struct xxx_message_s {
 *           esp_zb_device_cb_common_info_t info;
 *           ...
 *       } xxx_message_t;
 *       @endcode
 *       While the callback ids with ``CMD`` in their names would provide messages of the following structure:
 *       @code{c}
 *       typedef struct xxx_message_s {
 *           esp_zb_zcl_cmd_info_t info;
 *           ...
 *       } xxx_message_t;
 *       @endcode
 *
 */
typedef enum esp_zb_core_action_callback_id_s {
    ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID                    = 0x0000,   /*!< Set attribute value, refer to esp_zb_zcl_set_attr_value_message_t */
    ESP_ZB_CORE_SCENES_STORE_SCENE_CB_ID                = 0x0001,   /*!< Store scene, refer to esp_zb_zcl_store_scene_message_t */
    ESP_ZB_CORE_SCENES_RECALL_SCENE_CB_ID               = 0x0002,   /*!< Recall scene, refer to esp_zb_zcl_recall_scene_message_t */
    ESP_ZB_CORE_IAS_ZONE_ENROLL_RESPONSE_VALUE_CB_ID    = 0x0003,   /*!< IAS Zone enroll response, refer to esp_zb_zcl_ias_zone_enroll_response_message_t */
    ESP_ZB_CORE_OTA_UPGRADE_VALUE_CB_ID                 = 0x0004,   /*!< Upgrade OTA, refer to esp_zb_zcl_ota_upgrade_value_message_t */
    ESP_ZB_CORE_OTA_UPGRADE_SRV_STATUS_CB_ID            = 0x0005,   /*!< OTA Server status, refer to esp_zb_zcl_ota_upgrade_server_status_message_t */
    ESP_ZB_CORE_OTA_UPGRADE_SRV_QUERY_IMAGE_CB_ID       = 0x0006,   /*!< OTA Server query image, refer to esp_zb_zcl_ota_upgrade_server_query_image_message_t */
    ESP_ZB_CORE_THERMOSTAT_VALUE_CB_ID                  = 0x0007,   /*!< Thermostat value, refer to esp_zb_zcl_thermostat_value_message_t */
    ESP_ZB_CORE_METERING_GET_PROFILE_CB_ID              = 0x0008,   /*!< Metering get profile, refer to esp_zb_zcl_metering_get_profile_message_t */
    ESP_ZB_CORE_METERING_GET_PROFILE_RESP_CB_ID         = 0x0009,   /*!< Metering get profile response, refer to esp_zb_zcl_metering_get_profile_resp_message_t */
    ESP_ZB_CORE_METERING_REQ_FAST_POLL_MODE_CB_ID       = 0x000a,   /*!< Metering request fast poll mode, refer to esp_zb_zcl_metering_request_fast_poll_mode_message_t */
    ESP_ZB_CORE_METERING_REQ_FAST_POLL_MODE_RESP_CB_ID  = 0x000b,   /*!< Metering request fast poll mode response, refer to esp_zb_zcl_metering_request_fast_poll_mode_resp_message_t */
    ESP_ZB_CORE_METERING_GET_SNAPSHOT_CB_ID             = 0x000c,   /*!< Metering get snapshot, refer to esp_zb_zcl_metering_get_snapshot_message_t */
    ESP_ZB_CORE_METERING_PUBLISH_SNAPSHOT_CB_ID         = 0x000d,   /*!< Metering publish snapshot, refer to esp_zb_zcl_metering_publish_snapshot_message_t */
    ESP_ZB_CORE_METERING_GET_SAMPLED_DATA_CB_ID         = 0x000e,   /*!< Metering get sampled data, refer to esp_zb_zcl_metering_get_sampled_data_message_t */
    ESP_ZB_CORE_METERING_GET_SAMPLED_DATA_RESP_CB_ID    = 0x000f,   /*!< Metering get sampled data response, refer to esp_zb_zcl_metering_get_sampled_data_resp_message_t */
    ESP_ZB_CORE_DOOR_LOCK_LOCK_DOOR_CB_ID               = 0x0010,   /*!< Lock/unlock door request, refer to esp_zb_zcl_door_lock_lock_door_message_t */
    ESP_ZB_CORE_DOOR_LOCK_LOCK_DOOR_RESP_CB_ID          = 0x0011,   /*!< Lock/unlock door response, refer to esp_zb_zcl_door_lock_lock_door_resp_message_t */
    ESP_ZB_CORE_IDENTIFY_EFFECT_CB_ID                   = 0x0012,   /*!< Identify triggers effect request, refer to esp_zb_zcl_identify_effect_message_t */
    ESP_ZB_CORE_BASIC_RESET_TO_FACTORY_RESET_CB_ID      = 0x0013,   /*!< Reset all clusters of endpoint to factory default, refer to esp_zb_zcl_basic_reset_factory_default_message_t  */
    ESP_ZB_CORE_PRICE_GET_CURRENT_PRICE_CB_ID           = 0x0014,   /*!< Price get current price, refer to esp_zb_zcl_price_get_current_price_message_t */
    ESP_ZB_CORE_PRICE_GET_SCHEDULED_PRICES_CB_ID        = 0x0015,   /*!< Price get scheduled prices, refer to esp_zb_zcl_price_get_scheduled_prices_message_t */
    ESP_ZB_CORE_PRICE_GET_TIER_LABELS_CB_ID             = 0x0016,   /*!< Price get tier labels, refer to esp_zb_zcl_price_get_tier_labels_message_t */
    ESP_ZB_CORE_PRICE_PUBLISH_PRICE_CB_ID               = 0x0017,   /*!< Price publish price, refer to esp_zb_zcl_price_publish_price_message_t */
    ESP_ZB_CORE_PRICE_PUBLISH_TIER_LABELS_CB_ID         = 0x0018,   /*!< Price publish tier labels, refer to esp_zb_zcl_price_publish_tier_labels_message_t */
    ESP_ZB_CORE_PRICE_PRICE_ACK_CB_ID                   = 0x0019,   /*!< Price price acknowledgement, refer to esp_zb_zcl_price_ack_message_t */
    ESP_ZB_CORE_COMM_RESTART_DEVICE_CB_ID               = 0x001a,   /*!< Commissioning restart device, refer to esp_zigbee_zcl_commissioning_restart_device_message_t */
    ESP_ZB_CORE_COMM_OPERATE_STARTUP_PARAMS_CB_ID       = 0x001b,   /*!< Commissioning operate startup parameters, refer to esp_zigbee_zcl_commissioning_operate_startup_parameters_message_t */
    ESP_ZB_CORE_COMM_COMMAND_RESP_CB_ID                 = 0x001c,   /*!< Commissioning command response, refer to esp_zigbee_zcl_commissioning_command_response_message_t */
    ESP_ZB_CORE_IAS_WD_START_WARNING_CB_ID              = 0x001d,   /*!< IAS WD cluster Start Warning command, refer to esp_zb_zcl_ias_wd_start_warning_message_t */
    ESP_ZB_CORE_IAS_WD_SQUAWK_CB_ID                     = 0x001e,   /*!< IAS WD cluster Squawk command, refer to esp_zb_zcl_ias_wd_squawk_message_t */
    ESP_ZB_CORE_IAS_ACE_ARM_CB_ID                       = 0x001f,   /*!< IAS ACE cluster Arm command, refer to esp_zb_zcl_ias_ace_arm_message_t */
    ESP_ZB_CORE_IAS_ACE_BYPASS_CB_ID                    = 0x0020,   /*!< IAS ACE cluster Bypass command, refer to esp_zb_zcl_ias_ace_bypass_message_t */
    ESP_ZB_CORE_IAS_ACE_EMERGENCY_CB_ID                 = 0x0021,   /*!< IAS ACE cluster Emergency command, refer to esp_zb_zcl_ias_ace_emergency_message_t */
    ESP_ZB_CORE_IAS_ACE_FIRE_CB_ID                      = 0x0022,   /*!< IAS ACE cluster Fire command, refer to esp_zb_zcl_ias_ace_fire_message_t */
    ESP_ZB_CORE_IAS_ACE_PANIC_CB_ID                     = 0x0023,   /*!< IAS ACE cluster Panic command, refer to esp_zb_zcl_ias_ace_panic_message_t */
    ESP_ZB_CORE_IAS_ACE_GET_PANEL_STATUS_CB_ID          = 0x0024,   /*!< IAS ACE cluster Get Panel Status command, refer to esp_zb_zcl_ias_ace_get_panel_status_message_t */
    ESP_ZB_CORE_IAS_ACE_GET_BYPASSED_ZONE_LIST_CB_ID    = 0x0025,   /*!< IAS ACE cluster Get Bypass Zone List command, refer to esp_zb_zcl_ias_ace_get_bypassed_zone_list_message_t */
    ESP_ZB_CORE_IAS_ACE_GET_ZONE_STATUS_CB_ID           = 0x0026,   /*!< IAS ACE cluster Get Zone Status command, refer to esp_zb_zcl_ias_ace_get_zone_status_message_t */
    ESP_ZB_CORE_IAS_ACE_ARM_RESP_CB_ID                  = 0x0027,   /*!< IAS ACE cluster Arm command response, refer to esp_zb_zcl_ias_ace_arm_response_message_t */
    ESP_ZB_CORE_IAS_ACE_GET_ZONE_ID_MAP_RESP_CB_ID      = 0x0028,   /*!< IAS ACE cluster Get Zone ID MAP command response, refer to esp_zb_zcl_ias_ace_get_zone_id_map_response_message_t */
    ESP_ZB_CORE_IAS_ACE_GET_ZONE_INFO_RESP_CB_ID        = 0x0029,   /*!< IAS ACE cluster Get Zone Information command response, refer to esp_zb_zcl_ias_ace_get_zone_info_response_message_t */
    ESP_ZB_CORE_IAS_ACE_ZONE_STATUS_CHANGED_CB_ID       = 0x002a,   /*!< IAS ACE cluster Zone Status Changed command, refer to esp_zb_zcl_ias_ace_zone_status_changed_message_t */
    ESP_ZB_CORE_IAS_ACE_PANEL_STATUS_CHANGED_CB_ID      = 0x002b,   /*!< IAS ACE cluster Panel Status Changed command, refer to esp_zb_zcl_ias_ace_panel_status_changed_message_t */
    ESP_ZB_CORE_IAS_ACE_GET_PANEL_STATUS_RESP_CB_ID     = 0x002c,   /*!< IAS ACE cluster Get Panel Status command response, refer to esp_zb_zcl_ias_ace_get_panel_status_response_message_t */
    ESP_ZB_CORE_IAS_ACE_SET_BYPASSED_ZONE_LIST_CB_ID    = 0x002d,   /*!< IAS ACE cluster Set Bypassed Zone List command, refer to esp_zb_zcl_ias_ace_set_bypassed_zone_list_message_t */
    ESP_ZB_CORE_IAS_ACE_BYPASS_RESP_CB_ID               = 0x002e,   /*!< IAS ACE cluster Bypass command response, refer to esp_zb_zcl_ias_ace_bypass_response_message_t */
    ESP_ZB_CORE_IAS_ACE_GET_ZONE_STATUS_RESP_CB_ID      = 0x002f,   /*!< IAS ACE cluster Get Zone Status command response, refer to esp_zb_zcl_ias_ace_get_zone_status_response_message_t */
    ESP_ZB_CORE_WINDOW_COVERING_MOVEMENT_CB_ID          = 0x0030,   /*!< Window covering movement command, refer to esp_zb_zcl_window_covering_movement_message_t */
    ESP_ZB_CORE_OTA_UPGRADE_QUERY_IMAGE_RESP_CB_ID      = 0x0031,   /*!< OTA upgrade query image response message, refer to esp_zb_zcl_ota_upgrade_query_image_resp_message_t */
    ESP_ZB_CORE_THERMOSTAT_WEEKLY_SCHEDULE_SET_CB_ID    = 0x0032,   /*!< Thermostat weekly schedule stable set, refer to esp_zb_zcl_thermostat_weekly_schedule_set_message_t */
    ESP_ZB_CORE_DRLC_LOAD_CONTORL_EVENT_CB_ID           = 0x0040,   /*!< Demand response and load control cluster LoadControlEvent message, refer to esp_zb_zcl_drlc_load_control_event_message_t */
    ESP_ZB_CORE_DRLC_CANCEL_LOAD_CONTROL_EVENT_CB_ID    = 0x0041,   /*!< Demand response and load control cluster CancelLoadControlEvent message, refer to esp_zb_zcl_drlc_cancel_load_control_event_message_t */
    ESP_ZB_CORE_DRLC_CANCEL_ALL_LOAD_CONTROL_EVENTS_CB_ID = 0x0042, /*!< Demand response and load control cluster CancelAllLoadControlEvents message, refer to esp_zb_zcl_drlc_cancel_all_load_control_events_message_t */
    ESP_ZB_CORE_DRLC_REPORT_EVENT_STATUS_CB_ID          = 0x0043,   /*!< Demand response and load control cluster ReportEventStatus message, refer to esp_zb_zcl_drlc_report_event_status_message_t */
    ESP_ZB_CORE_DRLC_GET_SCHEDULED_EVENTS_CB_ID         = 0x0044,   /*!< Demand response and load control cluster GetScheduledEvents message, refer to esp_zb_zcl_drlc_get_scheduled_events_message_t */
    ESP_ZB_CORE_CMD_READ_ATTR_RESP_CB_ID                = 0x1000,   /*!< Read attribute response, refer to esp_zb_zcl_cmd_read_attr_resp_message_t */
    ESP_ZB_CORE_CMD_WRITE_ATTR_RESP_CB_ID               = 0x1001,   /*!< Write attribute response, refer to esp_zb_zcl_cmd_write_attr_resp_message_t */
    ESP_ZB_CORE_CMD_REPORT_CONFIG_RESP_CB_ID            = 0x1002,   /*!< Configure report response, refer to esp_zb_zcl_cmd_config_report_resp_message_t */
    ESP_ZB_CORE_CMD_READ_REPORT_CFG_RESP_CB_ID          = 0x1003,   /*!< Read report configuration response, refer to esp_zb_zcl_cmd_read_report_config_resp_message_t */
    ESP_ZB_CORE_CMD_DISC_ATTR_RESP_CB_ID                = 0x1004,   /*!< Discover attributes response, refer to esp_zb_zcl_cmd_discover_attributes_resp_message_t */
    ESP_ZB_CORE_CMD_DEFAULT_RESP_CB_ID                  = 0x1005,   /*!< Default response, refer to esp_zb_zcl_cmd_default_resp_message_t */
    ESP_ZB_CORE_CMD_OPERATE_GROUP_RESP_CB_ID            = 0x1010,   /*!< Group add group response, refer to esp_zb_zcl_groups_operate_group_resp_message_t */
    ESP_ZB_CORE_CMD_VIEW_GROUP_RESP_CB_ID               = 0x1011,   /*!< Group view response, refer to esp_zb_zcl_groups_view_group_resp_message_t */
    ESP_ZB_CORE_CMD_GET_GROUP_MEMBERSHIP_RESP_CB_ID     = 0x1012,   /*!< Group get membership response, refer to esp_zb_zcl_groups_get_group_membership_resp_message_t */
    ESP_ZB_CORE_CMD_OPERATE_SCENE_RESP_CB_ID            = 0x1020,   /*!< Scenes operate response, refer to esp_zb_zcl_scenes_operate_scene_resp_message_t */
    ESP_ZB_CORE_CMD_VIEW_SCENE_RESP_CB_ID               = 0x1021,   /*!< Scenes view response, refer to esp_zb_zcl_scenes_view_scene_resp_message_t */
    ESP_ZB_CORE_CMD_GET_SCENE_MEMBERSHIP_RESP_CB_ID     = 0x1022,   /*!< Scenes get membership response, refer to esp_zb_zcl_scenes_get_scene_membership_resp_message_t */
    ESP_ZB_CORE_CMD_IAS_ZONE_ZONE_ENROLL_REQUEST_ID     = 0x1030,   /*!< IAS Zone enroll request, refer to esp_zb_zcl_ias_zone_enroll_request_message_t */
    ESP_ZB_CORE_CMD_IAS_ZONE_ZONE_STATUS_CHANGE_NOT_ID  = 0x1031,   /*!< IAS Zone status change notification, refer to esp_zb_zcl_ias_zone_status_change_notification_message_t */
    ESP_ZB_CORE_CMD_CUSTOM_CLUSTER_REQ_CB_ID            = 0x1040,   /*!< Custom Cluster request, refer to esp_zb_zcl_custom_cluster_command_message_t */
    ESP_ZB_CORE_CMD_CUSTOM_CLUSTER_RESP_CB_ID           = 0x1041,   /*!< Custom Cluster response, refer to esp_zb_zcl_custom_cluster_command_message_t */
    ESP_ZB_CORE_CMD_PRIVILEGE_COMMAND_REQ_CB_ID         = 0x1050,   /*!< Custom Cluster request, refer to esp_zb_zcl_privilege_command_message_t */
    ESP_ZB_CORE_CMD_PRIVILEGE_COMMAND_RESP_CB_ID        = 0x1051,   /*!< Custom Cluster response, refer to esp_zb_zcl_privilege_command_message_t */
    ESP_ZB_CORE_CMD_TOUCHLINK_GET_GROUP_ID_RESP_CB_ID   = 0x1060,   /*!< Touchlink commissioning cluster get group id response, refer to esp_zb_touchlink_get_group_identifiers_resp_message_t */
    ESP_ZB_CORE_CMD_TOUCHLINK_GET_ENDPOINT_LIST_RESP_CB_ID = 0x1061,   /*!< Touchlink commissioning cluster get endpoint list response, refer to esp_zb_zcl_touchlink_get_endpoint_list_resp_message_t */
    ESP_ZB_CORE_CMD_THERMOSTAT_GET_WEEKLY_SCHEDULE_RESP_CB_ID = 0x1070, /*!< Thermostat cluster get weekly schedule response, refer to esp_zb_zcl_thermostat_get_weekly_schedule_resp_message_t */
    ESP_ZB_CORE_CMD_GREEN_POWER_RECV_CB_ID                 = 0x1F00,   /*!< Green power cluster command receiving, refer to esp_zb_zcl_cmd_green_power_recv_message_t */
    ESP_ZB_CORE_REPORT_ATTR_CB_ID                          = 0x2000,   /*!< Attribute Report, refer to esp_zb_zcl_report_attr_message_t */
} esp_zb_core_action_callback_id_t;

/**
 * @brief The Zigbee Coordinator/ Router device configuration.
 *
 */
typedef struct {
    uint8_t max_children; /*!< Max number of the children */
} esp_zb_zczr_cfg_t;

/**
 * @brief The Zigbee End device configuration.
 *
 */
typedef struct {
    uint8_t ed_timeout; /*!< Set End Device Timeout, refer to esp_zb_aging_timeout_t */
    uint32_t keep_alive; /*!< Set Keep alive Timeout, in milliseconds, with a maximum value of 65,000,000,000.*/
} esp_zb_zed_cfg_t;

/**
 * @brief The Zigbee device configuration.
 * @note  For esp_zb_role please refer defined by esp_zb_nwk_device_type_t.
 */
typedef struct esp_zb_cfg_s {
    esp_zb_nwk_device_type_t esp_zb_role; /*!< The nwk device type */
    bool install_code_policy;             /*!< Allow install code security policy or not */
    union {
        esp_zb_zczr_cfg_t zczr_cfg; /*!< The Zigbee zc/zr device configuration */
        esp_zb_zed_cfg_t zed_cfg;   /*!< The Zigbee zed device configuration */
    } nwk_cfg;                      /*!< Union of the network configuration */
} esp_zb_cfg_t;

/**
 * @brief The application signal struct for esp_zb_app_signal_handler
 *
 */
typedef struct esp_zb_app_signal_s {
    uint32_t *p_app_signal;   /*!< Application pointer signal type, refer to esp_zb_app_signal_type_t */
    esp_err_t esp_err_status; /*!< The error status of the each signal event, refer to esp_err_t */
} esp_zb_app_signal_t;

/**
 * @brief A callback for user to obtain interesting Zigbee message
 *
 * @note The returned value will be utilized by the stack for post-processing
 * @param[in] callback_id The id of Zigbee core action, refer to esp_zb_core_action_callback_id_t
 * @param[in] message The information of Zigbee core action that bind with the @p callback_id
 *
 * @return ESP_OK The action is handled successfully, others on failure
 */
typedef esp_err_t (*esp_zb_core_action_callback_t)(esp_zb_core_action_callback_id_t callback_id, const void *message);

/**
 * @brief A callback for user to obtain device_cb_id buffer id of ZBoss stack
 *
 * @return
 *      -   True: processed
 *      -   False: unprocessed
 */
typedef bool (*esp_zb_zcl_device_cb_id_callback_t)(uint8_t bufid);

/**
 * @brief A callback for user to obtain raw command bufid of ZBoss stack
 *
 * @note If the @p bufid has been processed in the callback, it should be freed using the zb_zcl_send_default_handler().
 *
 * @return
 *      -   True: processed
 *      -   False: unprocessed
 */
typedef bool (*esp_zb_zcl_raw_command_callback_t)(uint8_t bufid);

/** CLI response callback
 *
 * @param[in] bufid index of buffer with ZCL command
 *
 */
typedef uint8_t (*esp_zb_cli_resp_callback_t)(uint8_t bufid);

/** Identify callback
 *
 * @param[in] identify_on A indication that need start identify or stop
 *
 */
typedef void (*esp_zb_identify_notify_callback_t)(uint8_t identify_on);

/**
 * @brief ZCL reset default attribute callback
 *
 * @param[in] endpoint   The Endpoint identifier
 * @param[in] cluster_id The cluster identifier
 * @param[in] curr_attr  The current attribute information
 *
 * @return The default attribute value will be set to
 */
typedef void *(*esp_zb_zcl_reset_default_attr_callback_t)(uint8_t endpoint, uint16_t cluster_id, esp_zb_zcl_attribute_t curr_attr);

/**
 * @brief Register the Zigbee core action handler
 *
 * @param[in] cb A callback that user can handle the Zigbee action, refer to esp_zb_core_callback_t
 *
 */
void esp_zb_core_action_handler_register(esp_zb_core_action_callback_t cb);

/**
 * @brief Register the Zigbee device_cb_id handler
 *
 * @param[in] cb A callback that user can handle the Zigbee raw device_cb_id buffer id, refer to esp_zb_core_callback_t
 *
 */
void esp_zb_device_cb_id_handler_register(esp_zb_zcl_device_cb_id_callback_t cb);

/**
 * @brief Register the raw Zigbee command handler
 *
 * @param[in] cb A callback that user can handle the Zigbee raw command buffer id, refer to esp_zb_zcl_raw_command_callback_t
 *
 */
void esp_zb_raw_command_handler_register(esp_zb_zcl_raw_command_callback_t cb);

/**
 * @brief   Set the Command line interface (CLI) handler callback.
 *
 * @note  Set a command handler callback for handle response from other device to CLI device.
 * @param[in] cb A CLI command handler callback that user used refer to esp_zb_cli_resp_callback_t
 *
 */
void esp_zb_cli_resp_handler_register(esp_zb_cli_resp_callback_t cb);

/**
 * @brief Register the callback for the ZCL command send status
 *
 * @param[in] cb The ZCL command send status callback, refer to esp_zb_zcl_command_send_status_callback_t
 */
void esp_zb_zcl_command_send_status_handler_register(esp_zb_zcl_command_send_status_callback_t cb);

/**
 * @brief   Set the ZCL identify notify callback for specific endpoint.
 *
 * @note  Set a callback for user to handle identify command.
 *
 * @param[in] endpoint A specific endpoint
 * @param[in] cb A identify notify callback that user used
 *
 */
void esp_zb_identify_notify_handler_register(uint8_t endpoint, esp_zb_identify_notify_callback_t cb);

/**
 * @brief Add a callback and the privilege command the Zigbee cluster in endpoint.
 *
 * @note The privilege command will skip the Zigbee stack and be exposed to users by the callback indirectly.
 *       Allowing different commands to use the same callback.
 *
 * @param[in] endpoint The specific endpoint for @p cluster
 * @param[in] cluster The specific cluster for @p command
 * @param[in] command The specific command ID is required to handle for users.
 * @return
 *      - ESP_OK: on success
 *      - ESP_FAIL: on failure
 */
esp_err_t esp_zb_zcl_add_privilege_command(uint8_t endpoint, uint16_t cluster, uint16_t command);

/**
 * @brief Delete the privilege command from the @p cluster of @p endpoint
 *
 * @param[in] endpoint The specific endpoint for @p cluster
 * @param[in] cluster The specific cluster for @p command
 * @param[in] command The specific command ID will be deleted so that the stack will handle the command rather than user.
 * @return
 *      -   True: On success
 *      -   False: Nothing to delete
 */
bool esp_zb_zcl_delete_privilege_command(uint8_t endpoint, uint16_t cluster, uint16_t command);

/**
 * @brief Set the ZCL scenes cluster scene table for users.
 *
 * @param[in] endpoint          The endpoint identifier
 * @param[in] group_id          The group id of scene, which will be used to find scenes table record
 * @param[in] scene_id          The scene id of scene, which will be used to find scenes table record
 * @param[in] transition_time   The transition time of scene, whose unit is 100 milliseconds
 * @param[in] field             The pointer to zcl sense extension field list
 * @return
 *      - ESP_OK: on success
 *      - ESP_FAIL: the group id or scene id is invalid
 */
esp_err_t esp_zb_zcl_scenes_table_store(uint8_t endpoint, uint16_t group_id, uint8_t scene_id, uint16_t transition_time, esp_zb_zcl_scenes_extension_field_t *field);

/**
 * @brief View the zcl scene table
 *
 * @param[in] endpoint The specific endpoint identifier
 */
void esp_zb_zcl_scenes_table_show(uint8_t endpoint);

/**
 * @brief Clear zcl scenes table by index
 *
 * @param[in] index The index of scenes table
 * @return
 *       - ESP_OK: on success
 *       - ESP_ERR_INVALID: id out of range
 *       - ESP_FAILED: failed to clear scene table
 */
esp_err_t esp_zb_zcl_scenes_table_clear_by_index(uint16_t index);

/**
 * @brief Set the maximum number of devices in a Zigbee network
 *
 * @note The function will only take effect when called before esp_zb_init(), it determins
 *       several table size such as the neighbor table and routing table, 64 by default
 * @param[in] size The overall network size is expected to be set
 * @return
 *       - ESP_OK: on success
 *       - ESP_FAIL: on failure
 */
esp_err_t esp_zb_overall_network_size_set(uint16_t size);

/**
 * @brief Set Zigbee stack I/O buffer size
 *
 * @note The function will only take effect when called before esp_zb_init(), 80 by default.
 * @param[in] size The I/O buffer size is expected to be set
 * @return
 *       - ESP_OK: on success
 *       - ESP_FAIL: on failure
 */
esp_err_t esp_zb_io_buffer_size_set(uint16_t size);

/**
 * @brief Set Zigbee stack scheduler queue size
 *
 * @note The function will only take effect when called before esp_zb_init(), 80 by default.
 * @param[in] size The scheduler queue size is expected to be set
 * @return
 *       - ESP_OK: on success
 *       - ESP_FAIL: on failure
 */
esp_err_t esp_zb_scheduler_queue_size_set(uint16_t size);

/**
 * @brief Set APS source binding table size
 *
 * @note The function will only take effect when called before esp_zb_init(), 16 by default
 * @param[in] size The source binding table size is expected to be set
 * @return
 *       - ESP_OK: on success
 *       - ESP_FAIL: on failure
 */
esp_err_t esp_zb_aps_src_binding_table_size_set(uint16_t size);

/**
 * @brief Set APS destination binding table size
 *
 * @note The function will only take effect when called before esp_zb_init(), 16 by default
 * @param[in] size The destination binding table size is expected to be set
 * @return
 *       - ESP_OK: on success
 *       - ESP_FAIL: on failure
 */
esp_err_t esp_zb_aps_dst_binding_table_size_set(uint16_t size);

/**
 * @brief  Zigbee stack initialization.
 *
 * @note To be called inside the application's main cycle at start.
 * @note Default is no NVRAM erase from start up, user could call factory reset for erase NVRAM and other action please refer esp_zb_factory_reset().
 * @note Make sure to use correct corresponding nwk_cfg with your device type @ref esp_zb_cfg_s.
 * @anchor esp_zb_init
 * @param[in] nwk_cfg   The pointer to the initialization Zigbee configuration
 *
 */
void esp_zb_init(esp_zb_cfg_t *nwk_cfg);

/**
 * @brief Set the BDB primary channel mask.
 *
 * Beacon request will be sent on these channels for the BDB energy scan.
 *
 * @note  This function should be run AFTER @ref esp_zb_init is called and before @ref esp_zb_start. These masks define the allowable channels on which the device may attempt to
 * form or join a network at startup time. If function is not called, by default it will scan all channels or read from `zb_fct` NVRAM zone if available. Please refer to tools/mfg_tool.
 * @param[in] channel_mask  Valid channel mask is from 0x00000800 (only channel 11) to 0x07FFF800 (all channels from 11 to 26)
 * @return  - ESP_OK on success
            - ESP_ERR_INVALID_ARG if the channel mask is out of range
 */
esp_err_t esp_zb_set_primary_network_channel_set(uint32_t channel_mask);

/**
 * @brief Get the BDB primary channel mask
 *
 * @return A 32-bit channel mask
 */
uint32_t esp_zb_get_primary_network_channel_set(void);

/**
 * @brief   Set the BDB secondary channel mask.
 *
 * Beacon request will be sent on these channels for the BDB energy scan, if no network found after energy scan on the primary channels.
 *
 * @param[in] channel_mask Valid channel mask is from 0x00000800 (only channel 11) to 0x07FFF800 (all channels from 11 to 26)
 * @return  - ESP_OK on success
            - ESP_ERR_INVALID_ARG if the channel mask is out of range
 */
esp_err_t esp_zb_set_secondary_network_channel_set(uint32_t channel_mask);

/**
 * @brief Get the BDB secondary channel mask
 *
 * @return A 32-bit channel mask
 */
uint32_t esp_zb_get_secondary_network_channel_set(void);

/**
 * @brief   Set the 2.4G channel mask.
 *
 * @param[in] channel_mask Valid channel mask is from 0x00000800 (only channel 11) to 0x07FFF800 (all channels from 11 to 26)
 * @return  - ESP_OK on success
            - ESP_ERR_INVALID_ARG if the channel mask is out of range
 */
esp_err_t esp_zb_set_channel_mask(uint32_t channel_mask);

/**
 * @brief Get the 2.4 channel mask
 *
 * @return A 32-bit channel mask
 */
uint32_t esp_zb_get_channel_mask(void);

/**
 * @brief   Check if device is factory new.
 *
 * @return - True factory new.
 *
 */
bool esp_zb_bdb_is_factory_new(void);

/**
 * @brief Get the scan duration time
 *
 * @return Scan duration is in beacon intervals (15.36 ms)
 */
uint8_t esp_zb_bdb_get_scan_duration(void);

/**
 * @brief Set the scan duration time
 *
 * @param[in] duration  The scan duration time is in beacon intervals, defined as ((1 << duration) + 1) * 15.36 ms
 */
void esp_zb_bdb_set_scan_duration(uint8_t duration);

/**
 * @brief Open Zigbee network
 *
 * @param[in] permit_duration Zigbee network open time
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_NO_MEM: not memory
 *      - ESP_FAILED: on failed
 */
esp_err_t esp_zb_bdb_open_network(uint8_t permit_duration);

/**
 * @brief Close Zigbee network
 *
 * @return
 *      - ESP_OK: on success
 *      - ESP_FAIL: on failure
 */
esp_err_t esp_zb_bdb_close_network(void);

/**
 * @brief Check if device has joined network or not
 *
 * @return
 *      - true: device is joined
 *      - false: device is not joined
 */
bool esp_zb_bdb_dev_joined(void);

/**
 * @brief Set Touchlink NWK channel
 *
 * @param[in] channel Touchlink NWK channel value
 *
 */
void esp_zb_zdo_touchlink_set_nwk_channel(uint8_t channel);

/**
 * @brief   Set the tx power.
 * @param[in]  power 8-bit of power value in dB, ranging from IEEE802154_TXPOWER_VALUE_MIN to IEEE802154_TXPOWER_VALUE_MAX
 */
void esp_zb_set_tx_power(int8_t power);

/**
 * @brief   Get the tx power.
 * @param[in]  power 8-bit of power pointer value in dB
 */
void esp_zb_get_tx_power(int8_t *power);

/**
 * @brief  Start top level commissioning procedure with specified mode mask.
 *
 * @param[in] mode_mask commissioning modes refer to esp_zb_bdb_commissioning_mode
 *
 * @return - ESP_OK on success
 *
 */
esp_err_t esp_zb_bdb_start_top_level_commissioning(uint8_t mode_mask);

/**
 * @brief Perform `local reset` procedure
 * @note This only takes effect when the device is on a network. The device will leave the current network and
 *       clear all Zigbee persistent data, except the outgoing NWK frame counter. It will be in nearly the same
 *       state as when it left the factory. A `ZB_ZDO_SIGNAL_LEAVE` signal with `ESP_ZB_NWK_LEAVE_TYPE_RESET`
 *       will be triggered to indicate the end of the procedure.
 */
void esp_zb_bdb_reset_via_local_action(void);

/**
 *  @brief Perform "factory reset" procedure
 *  @note The device will completely erase the `zb_storage` partition and then restart
 */
void esp_zb_factory_reset(void);

/**
 * @brief Reset all endpoints to factory default
 *
 * @note If @p cb is not set or @p cb return NULL, the default attribute value will be set to zero
 * @param[in] reset_report Whether reset report of clusters or not
 * @param[in] cb The user can utilize the callback to set default attribute value
 *
 * @return
 *      - ESP_OK: on success
 *      - ESP_FAIL: on failed
 */
esp_err_t esp_zb_zcl_reset_all_endpoints_to_factory_default(bool reset_report, esp_zb_zcl_reset_default_attr_callback_t cb);

/**
 * @brief Reset endpoint to factory default
 *
 * @note If @p cb is not set or @p cb return NULL, the default attribute value will be set to zero
 * @param[in] endpoint      The endpoint identifier which will be reset
 * @param[in] reset_report  Whether reset report of clusters or not
 * @param[in] cb The user can utilize the callback to set default attribute value
 *
 * @return
 *      - ESP_OK: on success
 *      - ESP_FAIL: on failed
 */
esp_err_t esp_zb_zcl_reset_endpoint_to_factory_default(uint8_t endpoint, bool reset_report, esp_zb_zcl_reset_default_attr_callback_t cb);

/**
 * @brief Reset the NVRAM and ZCL data to factory default
 *
 */
void esp_zb_zcl_reset_nvram_to_factory_default(void);

/**
 * @brief   Start Zigbee function.
 *
 * @param[in] autostart - true    autostart mode
 *                      - false   no-autostart mode
 *
 * @note Autostart mode: It initializes, load some parameters from NVRAM and proceed with startup.
 * Startup means either Formation (for ZC), rejoin or discovery/association join.  After startup
 * complete, @ref esp_zb_app_signal_handler is called, so application will know when to do
 * some useful things.
 *
 * @note No-autostart mode: It initializes scheduler and buffers pool, but not MAC and upper layers.
 * Notifies the application that Zigbee framework (scheduler, buffer pool, etc.) has started, but no
 * join/rejoin/formation/BDB initialization has been done yet.
 * Typically esp_zb_start with no_autostart mode is used when application wants to do something before
 * starting joining the network.
 *
 * For example, you can use this function if it is needed to enable leds, timers
 * or any other devices on periphery to work with them before starting working in a network. It's
 * also useful if you want to run something locally during joining.
 *
 * @note Precondition: stack must be initialized by @ref esp_zb_init call. @ref esp_zb_init sets default IB
 * parameters, so caller has a chance to change some of them. Note that NVRAM and product config will be
 * loaded after esp_zb_start() call.
 *
 * @note Zigbee stack is not looped in this routine. Instead, it schedules callback and returns.  Caller
 * must run  esp_zb_stack_main_loop() after this routine.
 *
 * @note Application should later call Zigbee commissioning initiation - for instance,
 * esp_zb_bdb_start_top_level_commissioning().
 * @anchor esp_zb_start
 * @return - ESP_OK on success
 *
 */
esp_err_t esp_zb_start(bool autostart);

/**
 * @brief Get the stack is started or not.
 *
 * @return true if the stack has been started, false otherwise.
 *
 */
bool esp_zb_is_started(void);

/**
 * @brief  Acquire Zigbee lock.
 *
 * @note It's mandatory to acquire the lock before calling any Zigbee SDK APIs, except that the call site is in Zigbee callbacks.
 * @return
 *      - true: on success
 *
 */
bool esp_zb_lock_acquire(TickType_t block_ticks);

/**
 * @brief  Release Zigbee lock.
 *
 */
void esp_zb_lock_release(void);

/**
 * @brief  Zigbee main loop iteration.
 *
 * @deprecated Please use esp_zb_stack_main_loop() instead
 * @note Must be called after esp_zb_init() and esp_zb_start()
   inside the application's main cycle.
 *
 */
ESP_ZB_DEPRECATED
void esp_zb_main_loop_iteration(void);

/**
 * @brief Zigbee stack main loop iteration once.
 *
 * @note Must be called after esp_zb_init() and esp_zb_start().
 */
void esp_zb_stack_main_loop_iteration(void);

/**
 * @brief  Zigbee stack main loop.
 *
 * @note Must be called after esp_zb_init() and esp_zb_start(), it’s an infinite main loop.
 */
void esp_zb_stack_main_loop(void);

/**
 * @brief  Zigbee CLI example main loop iteration.
 *
 * @note It is ONLY for cli specific example loop iteration.
 *
 */
void esp_zb_cli_main_loop_iteration(void);

/**
 * @brief  Enable/Disable erasing the zb_storage field before the stack runs
 *
 * @note   Enable or disable NVRAM erasing on every application startup. Erasing is disabled by default.
 * @param erase - 1 to enable erasing; 0 - disable.
 *
 */
void esp_zb_nvram_erase_at_start(bool erase);

/**
 * @brief Zigbee stack application signal handler.
 * @anchor esp_zb_app_signal_handler
 *
 * @param[in] signal_s   pointer of Zigbee zdo app signal struct @ref esp_zb_app_signal_s.
 * @note After esp_zb_start, user shall based on the corresponding signal type refer to esp_zdo_app_signal_type from struct pointer signal_s to do certain actions.
 * User could also use refer to esp_zb_bdb_start_top_level_commissioning to change BDB mode.
 * @warning This function has to be defined by user in each example.
 */
void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_s);

/**
 * @brief Obtains pointer to parameters passed with application signal.
 *
 * @param[in] signal_p pointer to the application signal type @ref esp_zb_app_signal_s of p_app_signal
 * @return void pointer to certain signal param content
 */
void *esp_zb_app_signal_get_params(uint32_t *signal_p);

/**
 * @brief Schedule alarm - callback to be executed after timeout.
 *
 * @note Function will be called via scheduler after timeout expired in millisecond. Timer resolution depends on implementation. Same callback can be scheduled for execution more then once.
 *
 * @param[in] cb - function to call via scheduler
 * @param[in] param - parameter to pass to the function
 * @param[in] time - timeout, in millisecond
 */
void esp_zb_scheduler_alarm(esp_zb_callback_t cb, uint8_t param, uint32_t time);

/**
 * @brief Cancel scheduled alarm.
 *
 * @note This function cancel previously scheduled alarm.
 *
 * @param[in] cb - function to cancel
 * @param[in] param - parameter to pass to the function to cancel
 */
void esp_zb_scheduler_alarm_cancel(esp_zb_callback_t cb, uint8_t param);

/**
 * @brief Schedule user alarm - callback to be executed after timeout.
 *
 * @note Function will be called via scheduler after timeout expired in millisecond. Timer resolution depends on implementation. Same callback can be scheduled for execution more then once.
 *
 * @param[in] cb - function to call via scheduler
 * @param[in] param - parameter to pass to the function
 * @param[in] time - timeout, in millisecond
 *
 * @return
 *      - the handle used to cancel the user alarm
 */
esp_zb_user_cb_handle_t esp_zb_scheduler_user_alarm(esp_zb_user_callback_t cb, void *param, uint32_t time);

/**
 * @brief Cancel scheduled user alarm.
 *
 * @note This function cancel previously scheduled user alarm.
 *
 * @param[in] handle - the handle returned by esp_zb_scheduler_user_alarm
 *
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_NOT_FOUND: not found the user alarm
 */
esp_err_t esp_zb_scheduler_user_alarm_cancel(esp_zb_user_cb_handle_t handle);

/**
 * @brief  Set BDB commissioning mode.
 *
 * @param[in] commissioning_mode commissioning mode that refer to esp_zb_bdb_commissioning_mode_mask_t.
 *
 */
void esp_zb_set_bdb_commissioning_mode(esp_zb_bdb_commissioning_mode_mask_t commissioning_mode);

/**
 * @brief Set BDB commissioning mode
 *
 * @return commissioning mode, refer to esp_zb_bdb_commissioning_mode_mask_t
 */
esp_zb_bdb_commissioning_mode_mask_t esp_zb_get_bdb_commissioning_mode(void);

/**
 * @brief Schedule to cancel Steering procedure for a node not on a network
 *
 * @note The signal ESP_ZB_BDB_SIGNAL_STEERING_CANCELLED with the status of this operation will be raised.
 * @return
 *      - ESP_OK: on success
 *      - ESP_FAIL: on failed
 */
esp_err_t esp_zb_bdb_cancel_steering(void);

/**
 * @brief Schedule to cancel Formation procedure
 *
 * @note The signal ESP_ZB_BDB_SIGNAL_FORMATION_CANCELLED with the status of the operation will be raised.
 * @return
 *      - ESP_OK: on success
 *      - ESP_FAIL: on failed
 */
esp_err_t esp_zb_bdb_cancel_formation(void);

/* ZCL attribute, cluster, endpoint, device related */

/**
 * @brief Register a Zigbee device.
 *
 * @note
 *      1. After successful registration, the SDK will retain a copy of the whole data model,
 *         the \p ep_list will be freed.
 *      2. For any lists (e.g. ``esp_zb_cluster_list_t`` and ``esp_zb_attribute_list_t``) added to the data
 *         modal should only be created and modified by the API.
 *      3. Please DO NOT reuse ANY of lists in the data mode. If you have two identical stuff (e.g. two endpoints
 *         with identical clusters), please create/add them twice.
 *
 * @param[in] ep_list  An endpoint list which wants to register, see @ref esp_zb_ep_list_s
 *
 * @return
 *      - ESP_OK on success
 *
 */
esp_err_t esp_zb_device_register(esp_zb_ep_list_t *ep_list);

#ifdef ZB_DISTRIBUTED_SECURITY_ON

/**
 * @brief Set trust center policy state to distributed or not
 *
 * @param[in] enabled The trust center policy state
 */
void esp_zb_tc_policy_set_distributed_security(bool enabled);

/**
 * @brief Check the trust center policy is distributed
 *
 * @return
 *      - true: trust center policy is set as distributed
 *      - false: trust center policy is not set as distributed
 */
bool esp_zb_tc_policy_get_distributed_security();

/**
 * @brief Enable or disable the Zigbee distributed network.
 *
 * @param[in] enabled The status of Zigbee distribute network
 */
void esp_zb_enable_distributed_network(bool enabled);

/**
 * @brief Allow to setup network as distributed when started
 *
 */
void esp_zb_zdo_setup_network_as_distributed(void);

/**
 * @brief Check if the current network is a distributed security network
 *
 * @return - True: The current network is distributed, otherwise it is not.
 */
bool esp_zb_network_is_distributed(void);

/**
 * @brief Enable or disable the Zigbee device to join a distributed TC network
 *
 * @note It is disabled by default
 *
 * @param[in] enabled Enable or disable
 */
void esp_zb_enable_joining_to_distributed(bool enabled);

/**
 * @brief Determine whether the Zigbee device can join the distributed TC network or not
 *
 * @return - True: The Zigbee device can join the distributed TC network; otherwise, it cannot
 */
bool esp_zb_joining_to_distributed_network_enabled(void);
#endif

/**
 * @brief Set the sleep threshold on the device. When the scheduler detects that the device can enter sleep mode,
 *        it will notify the application with the signal ESP_ZB_COMMON_SIGNAL_CAN_SLEEP.
 *        The default sleep threshold is 20 milliseconds.
 *
 * @param[in] threshold_ms Sleep threshold in milliseconds
 *
 * @return ESP_OK if new threshold is valid and applied.
 * @return ESP_FAIL if the user attempts to set a threshold greater than ESP_ZB_SLEEP_MAXIMUM_THRESHOLD_MS.
 *
 */
esp_err_t esp_zb_sleep_set_threshold(uint32_t threshold_ms);

/**
* @brief Blocking function responsible for putting device into sleep mode.
*/
void esp_zb_sleep_now(void);

/**
 * @brief Enable the Zigbee sleep.
 *
 * @param[in] enable Enable Zigbee Sleep
 *
 */
void esp_zb_sleep_enable(bool enable);

/**
 * @brief Get Zigbee sleep is enable or not.
 *
 * @return TRUE Zigbee sleep is enable.
 * @return FALSE Zigbee sleep is disable.
 *
 */
bool esp_zb_sleep_is_enable(void);

/**
 * @brief Get bdb_commissioning_status
 *
 * @return commissioning_status refer to esp_zb_bdb_commissioning_status_t
 *
 */
#ifndef ZB_MACSPLIT_DEVICE
esp_zb_bdb_commissioning_status_t esp_zb_get_bdb_commissioning_status(void);
#endif

/**
 * @brief Set the Zigbee node descriptor manufacturer code.
 *
 * @note The function should be called in ESP_ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY signal

 * @param[in] manufacturer_code The manufacturer code of Zigbee device
 */
void esp_zb_set_node_descriptor_manufacturer_code(uint16_t manufacturer_code);

/**
 * @brief Get the version string of the SDK.
 *
 * @return The version string of the SDK.
 */
const char *esp_zb_get_version_string(void);

#ifdef __cplusplus
}
#endif
