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

/** @brief Structure representation of IAS ACE Zone Table */
typedef struct esp_zb_zcl_ias_ace_zone_table_s {
    uint8_t            zone_id;       /**< Zone ID */
    uint16_t           zone_type;     /**< Zone Type, see esp_zb_zcl_ias_zone_zonetype_t */
    esp_zb_ieee_addr_t zone_address;  /**< Zone Address */
    uint8_t            *zone_label;   /**< The pointer to a string with Zone Label. NULL pointer means the Zone Label is not programmed */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_zone_table_t;

/** @brief Default value for IAS ACE cluster revision global attribute */
#define ESP_ZB_ZCL_IAS_ACE_CLUSTER_REVISION_DEFAULT ((uint16_t)0x0001u)

/** @brief IAS ACE Zone Table maximum length */
#define ESP_ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH      255

/** @brief IAS ACE ZoneType attribute maximum value */
#define ESP_ZB_ZCL_IAS_ACE_ZONE_TYPE_MAX_VALUE    0xfffe

/** @brief IAS ACE ZoneID attribute maximum value */
#define ESP_ZB_ZCL_IAS_ACE_ZONE_ID_MAX_VALUE      0xfe

/** @brief IAS ACE ZoneID attribute default value */
#define ESP_ZB_ZCL_IAS_ACE_ZONE_ID_DEFAULT_VALUE  0

/** @brief IAS ACE Set Zone Address default value */
#define ESP_ZB_ZCL_IAS_ACE_SET_ZONE_ADDRESS_DEFAULT_VALUE(ptr)  (ZB_IEEE_ADDR_COPY(ptr, &g_unknown_ieee_addr))

/** @brief IAS Ace cluster command identifiers */
typedef enum {
    ESP_ZB_ZCL_CMD_IAS_ACE_ARM_ID                    = 0x00,   /**< Arm command */
    ESP_ZB_ZCL_CMD_IAS_ACE_BYPASS_ID                 = 0x01,   /**< Bypass command */
    ESP_ZB_ZCL_CMD_IAS_ACE_EMERGENCY_ID              = 0x02,   /**< Emergency command */
    ESP_ZB_ZCL_CMD_IAS_ACE_FIRE_ID                   = 0x03,   /**< Fire command */
    ESP_ZB_ZCL_CMD_IAS_ACE_PANIC_ID                  = 0x04,   /**< Panic command */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_ZONE_ID_MAP_ID        = 0x05,   /**< Get Zone ID Map command */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_ZONE_INFO_ID          = 0x06,   /**< Get Zone Information command */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_ID       = 0x07,   /**< Get Panel Status command */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_BYPASSED_ZONE_LIST_ID = 0x08,   /**< Get Bypassed Zone List command */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_ID        = 0x09,   /**< Get Zone Status command */
} esp_zb_zcl_ias_ace_cmd_t;

/** @brief IAS Ace cluster response command identifiers */
typedef enum {
    ESP_ZB_ZCL_CMD_IAS_ACE_ARM_RESP_ID                  = 0x00, /**< Arm Response command */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_ZONE_ID_MAP_RESP_ID      = 0x01, /**< Get Zone ID Map Response command */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_ZONE_INFO_RESP_ID        = 0x02, /**< Get Zone Information Response command */
    ESP_ZB_ZCL_CMD_IAS_ACE_ZONE_STATUS_CHANGED_ID       = 0x03, /**< Updates ACE clients of changes to zone status recorded by the server. */
    ESP_ZB_ZCL_CMD_IAS_ACE_PANEL_STATUS_CHANGED_ID      = 0x04, /**< Updates ACE clients of changes to zone status recorded by the server. */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_RESPONSE_ID = 0x05, /**< Updates ACE clients of changes to panel status recorded by the server. */
    ESP_ZB_ZCL_CMD_IAS_ACE_SET_BYPASSED_ZONE_LIST_ID    = 0x06, /**< Sets the list of bypassed zones on the IAS ACE client. */
    ESP_ZB_ZCL_CMD_IAS_ACE_BYPASS_RESPONSE_ID           = 0x07, /**< Bypass Response command. */
    ESP_ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_RESPONSE_ID  = 0x08, /**< Get Zone Status Response Command. */
} esp_zb_zcl_ias_ace_resp_cmd_t;

/******************************* Arm Command ******************************/

/** @brief Values of the Arm Mode */
typedef enum {
    ESP_ZB_ZCL_IAS_ACE_ARM_MODE_DISARM  = 0x00, /**< Disarm */
    ESP_ZB_ZCL_IAS_ACE_ARM_MODE_DAY     = 0x01, /**< Arm Day/Home Zones Only */
    ESP_ZB_ZCL_IAS_ACE_ARM_MODE_NIGHT   = 0x02, /**< Arm Night/Sleep Zones Only */
    ESP_ZB_ZCL_IAS_ACE_ARM_MODE_ALL     = 0x03, /**< Arm All Zones */
} esp_zb_zcl_ias_ace_arm_mode_t;

/** @brief Structure representation of Arm command */
typedef struct esp_zb_zcl_ias_ace_arm_s {
    uint8_t arm_mode;                                     /**< Arm Mode, see esp_zb_zcl_ias_ace_arm_mode_t */
    uint8_t arm_disarm_code[ESP_ZB_ZCL_MAX_STRING_SIZE];  /**< Arm/Disarm Code */
    uint8_t zone_id;                                      /**< Zone ID */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_arm_t;

/******************************* Bypass Command ******************************/

/** @brief Structure representation of Bypass command */
typedef struct esp_zb_zcl_ias_ace_bypass_s {
    uint8_t length;                                         /**< Number of Zones */
    uint8_t zone_id[ESP_ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH];  /**< Zone ID array, see esp_zb_zcl_ias_ace_zone_table_s */
    uint8_t arm_disarm_code[ESP_ZB_ZCL_MAX_STRING_SIZE];    /**< Arm/Disarm Code */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_bypass_t;

/******************************* Emergency Command ******************************/

/******************************* Fire Command ******************************/

/******************************* Panic Command ******************************/

/******************************* Get Zone ID Map command ******************************/

/******************************* Get Zone Information command ******************************/

/** @brief Structure representation of Get Zone Information command */
typedef struct esp_zb_zcl_ias_ace_get_zone_info_s {
    uint8_t zone_id;  /**< Zone ID, see @ref esp_zb_zcl_ias_ace_zone_table_s */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_get_zone_info_t;

/******************************* Get Panel Status command ******************************/

/******************************* Get Bypassed Zone List command ******************************/

/******************************* Get Zone Status command ******************************/

/** @brief Structure representation of Get Zone Status command */
typedef struct esp_zb_zcl_ias_ace_get_zone_status_s {
    uint8_t  starting_zone_id;       /**< Starting Zone ID, see esp_zb_zcl_ias_ace_zone_table_s */
    uint8_t  max_num_zone_ids;       /**< Max Number of Zone IDs Requested Field */
    uint8_t  zone_status_mask_flag;  /**< Zone Status Mask Flag Field */
    uint16_t zone_status_mask;       /**< Zone Status Mask Field, see esp_zb_zcl_ias_zone_zonestatus_t */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_get_zone_status_t;

/******************************* Arm Response command ******************************/

/** @brief Values of the Arm Notification */
typedef enum {
    ESP_ZB_ZCL_IAS_ACE_ARM_NOTIF_ALL_ZONES_DISARMED           = 0x00,   /**< All Zones Disarmed */
    ESP_ZB_ZCL_IAS_ACE_ARM_NOTIF_ONLY_DAY_HOME_ZONES_ARMED    = 0x01,   /**< Only Day/Home Zones Armed */
    ESP_ZB_ZCL_IAS_ACE_ARM_NOTIF_ONLY_NIGHT_SLEEP_ZONES_ARMED = 0x02,   /**< Only Night/Sleep Zones Armed */
    ESP_ZB_ZCL_IAS_ACE_ARM_NOTIF_ALL_ZONES_ARMED              = 0x03,   /**< All Zones Armed */
    ESP_ZB_ZCL_IAS_ACE_ARM_NOTIF_INVALID_ARM_DISARM_CODE      = 0x04,   /**< Invalid Arm/Disarm Code */
    ESP_ZB_ZCL_IAS_ACE_ARM_NOTIF_NOT_READY_TO_ARM             = 0x05,   /**< Not ready to arm */
    ESP_ZB_ZCL_IAS_ACE_ARM_NOTIF_ALREADY_DISARMED             = 0x06,   /**< Already disarmed */
} esp_zb_zcl_ias_ace_arm_notif_t;

/** @brief Structure representation of Arm Response command */
typedef struct esp_zb_zcl_ias_ace_arm_resp_s {
    uint8_t arm_notification; /**< Arm Notification, see esp_zb_zcl_ias_ace_arm_notif_t */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_arm_resp_t;

/******************************* Get Zone ID Map Response command ******************************/

/** @brief Length of array of Bitmap of Zone ID Map */
#define ESP_ZB_ZCL_IAS_ACE_GET_ZONE_ID_MAP_LENGTH   16

/** @brief Structure representation of Get Zone ID Map Response command */
typedef struct esp_zb_zcl_ias_ace_get_zone_id_map_resp_s {
    uint16_t zone_id_map[ESP_ZB_ZCL_IAS_ACE_GET_ZONE_ID_MAP_LENGTH]; /**< Zone ID Map */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_get_zone_id_map_resp_t;

/******************* Get Zone Information Response command ******************************/

/** @brief Zone Type is empty */
#define ESP_ZB_ZCL_IAS_ACE_GET_ZONE_INFO_TYPE_NONE   0xffff

/** @brief Structure representation of Get Zone Information Response command, see esp_zb_zcl_ias_ace_zone_table_s */
typedef struct esp_zb_zcl_ias_ace_get_zone_info_resp_s {
    uint8_t            zone_id;                                 /**< Zone ID */
    uint16_t           zone_type;                               /**< Zone Type, see esp_zb_zcl_ias_zone_zonetype_t */
    esp_zb_ieee_addr_t address;                                 /**< Zone Address */
    uint8_t            zone_label[ESP_ZB_ZCL_MAX_STRING_SIZE];  /**< Zone Label Field */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_get_zone_info_resp_t;

/******************* Zone Status Changed command ******************************/

/** @brief IAS ACE Audible Notification Field
    @see ZCL spec 8.3.2.4.4.4
*/
typedef enum {
    ESP_ZB_ZCL_IAS_ACE_AUD_NOTIFICATION_MUTE           = 0x00, /**< Mute (i.e., no audible notification) */
    ESP_ZB_ZCL_IAS_ACE_AUD_NOTIFICATION_DEF_SOUND      = 0x01, /**< Default sound */
    ESP_ZB_ZCL_IAS_ACE_AUD_NOTIFICATION_MANUF_SPECIFIC = 0x80, /**< Manufacturer specific */
} esp_zb_zcl_ias_ace_aud_notification_t;

/** @brief Structure representation of Zone Status Changed command */
typedef struct esp_zb_zcl_ias_ace_zone_status_changed_s {
    uint8_t  zone_id;                                 /**< Zone ID */
    uint16_t zone_status;                             /**< Zone Status, see esp_zb_zcl_ias_zone_zonestatus_t */
    uint8_t  aud_notification;                        /**< Audible Notification, see esp_zb_zcl_ias_ace_aud_notification_t */
    uint8_t  zone_label[ESP_ZB_ZCL_MAX_STRING_SIZE];  /**< Zone Label Field */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_zone_status_changed_t;

/******************* Panel Status Changed command ******************************/

/** @brief IAS ACE PanelStatus Parameter */
typedef enum {
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_DISARMED      = 0x00, /**< Panel disarmed (all zones disarmed) and ready to arm */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMED_STAY    = 0x01, /**< Armed stay */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMED_NIGHT   = 0x02, /**< Armed night */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMED_AWAY    = 0x03, /**< Armed away */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_EXIT_DELAY    = 0x04, /**< Exit delay */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_ENTRY_DELAY   = 0x05, /**< Entry delay */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_NOT_READY     = 0x06, /**< Not ready to arm */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_IN_ALARM      = 0x07, /**< In alarm */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMING_STAY   = 0x08, /**< Arming Stay */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMING_NIGHT  = 0x09, /**< Arming Night */
    ESP_ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMING_AWAY   = 0x0a, /**< Arming Away */
} esp_zb_zcl_ias_ace_panel_status_t;

/** @brief IAS ACE Alarm Status Field */
typedef enum {
    ESP_ZB_ZCL_IAS_ACE_ALARM_STATUS_NO_ALARM        = 0x00, /**< No alarm */
    ESP_ZB_ZCL_IAS_ACE_ALARM_STATUS_BURGLAR         = 0x01, /**< Burglar */
    ESP_ZB_ZCL_IAS_ACE_ALARM_STATUS_FIRE            = 0x02, /**< Fire */
    ESP_ZB_ZCL_IAS_ACE_ALARM_STATUS_EMERGENCY       = 0x03, /**< Emergency */
    ESP_ZB_ZCL_IAS_ACE_ALARM_STATUS_POLICE_PANIC    = 0x04, /**< Police Panic */
    ESP_ZB_ZCL_IAS_ACE_ALARM_STATUS_FIRE_PANIC      = 0x05, /**< Fire Panic */
    ESP_ZB_ZCL_IAS_ACE_ALARM_STATUS_EMERGENCY_PANIC = 0x06, /**< Emergency Panic */
} esp_zb_zcl_ias_ace_alarm_status_t;

/** @brief Structure representation of Panel Status Changed command */
typedef struct esp_zb_zcl_ias_ace_panel_status_changed_s {
    uint8_t panel_status;       /**< Panel Status, see esp_zb_zcl_ias_ace_panel_status_t */
    uint8_t seconds_remaining;  /**< Seconds Remaining Parameter */
    uint8_t aud_notification;   /**< Audible Notification, see esp_zb_zcl_ias_ace_aud_notification_t */
    uint8_t alarm_status;       /**< Alarm Status Field, see esp_zb_zcl_ias_ace_alarm_status_t */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_panel_status_changed_t;

/******************* Get Panel Status Response command ******************************/

/** @brief Structure representation of Get Panel Status Response command */
typedef struct esp_zb_zcl_ias_ace_get_panel_status_resp_s {
    uint8_t panel_status;       /**< Panel Status, see esp_zb_zcl_ias_ace_panel_status_t */
    uint8_t seconds_remaining;  /**< Seconds Remaining Parameter */
    uint8_t aud_notification;   /**< Audible Notification, see esp_zb_zcl_ias_ace_aud_notification_t */
    uint8_t alarm_status;       /**< Alarm Status Field, see esp_zb_zcl_ias_ace_alarm_status_t */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_get_panel_status_resp_t;

/******************* Set Bypassed Zone List command ******************************/

/** @brief Structure representation of Set Bypassed Zone List command */
typedef struct esp_zb_zcl_ias_ace_set_bypassed_zone_list_s {
    uint8_t length;                                         /**< Number of Zones */
    uint8_t zone_id[ESP_ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH];  /**< Zone ID array, see esp_zb_zcl_ias_ace_zone_table_s */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_set_bypassed_zone_list_t;

/******************* Bypass Response command ******************************/

/** @brief IAS ACE Bypass Result */
typedef enum {
    ESP_ZB_ZCL_IAS_ACE_BYPASS_RESULT_BYPASSED         = 0x00,   /**< Bypass request is successful. Zone is bypassed. */
    ESP_ZB_ZCL_IAS_ACE_BYPASS_RESULT_NOT_BYPASSED     = 0x01,   /**< Bypass request is unsuccessful. Zone is not bypassed. */
    ESP_ZB_ZCL_IAS_ACE_BYPASS_RESULT_NOT_ALLOWED      = 0x02,   /**< Bypass request is not allowed. Zone is not bypassed. */
    ESP_ZB_ZCL_IAS_ACE_BYPASS_RESULT_INVALID_ZONE_ID  = 0x03,   /**< Invalid Zone ID in the request. */
    ESP_ZB_ZCL_IAS_ACE_BYPASS_RESULT_UNKNOWN_ZONE_ID  = 0x04,   /**< Valid range of Zone ID, but unknown to server. */
    ESP_ZB_ZCL_IAS_ACE_BYPASS_RESULT_INVALID_ARM_CODE = 0x05,   /**< Arm/Disarm Code was entered incorrectly. */
} esp_zb_zcl_ias_ace_bypass_result_t;

/** @brief Structure representation of Bypass Response command */
typedef struct esp_zb_zcl_ias_ace_bypass_resp_s {
    uint8_t length;                                               /**< Number of Zones */
    uint8_t bypass_result[ESP_ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH];  /**< Bypass Result for Zone IDs, see esp_zb_zcl_ias_ace_bypass_result_t */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_bypass_resp_t;

/******************* Get Zone Status Response command ******************************/

/** @brief IAS ACE Zone Status structure */
typedef struct esp_zb_zcl_ias_ace_zone_status_s {
    uint8_t  zone_id;      /**< Zone ID, see esp_zb_zcl_ias_ace_zone_table_s */
    uint16_t zone_status;  /**< Zone Status, see esp_zb_zcl_ias_zone_zonestatus_t */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_zone_status_t;

/** @brief Structure representation of Get Zone Status Response command */
typedef struct esp_zb_zcl_ias_ace_get_zone_status_resp_s {
    uint8_t zone_status_complete;   /**< Zone Status Complete Field */
    uint8_t length;                 /**< Number of Zones */
    esp_zb_zcl_ias_ace_zone_status_t zone_id_status[ESP_ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH];  /**< Status of Zone IDs */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_ias_ace_get_zone_status_resp_t;

#ifdef __cplusplus
}
#endif
