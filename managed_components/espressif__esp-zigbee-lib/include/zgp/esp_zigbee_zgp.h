/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "zb_config.h"
#include "zb_config_common.h"
#include "esp_zigbee_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fill the security level for ZGP sink
 *
 */
#define ESP_ZB_ZGP_FILL_GPS_SECURITY_LEVEL(sec_level, with_link_key, involve_tc) \
    (((sec_level) & 3U) | ((!!(with_link_key)) << 2U) | ((!!(involve_tc)) << 3U))

/**
 * @brief Compare the ZGP devive identifier
 *
 */
#define ESP_ZB_ZGPD_IDS_COMPARE(id1, id2)              \
    (((id1)->app_id == (id2)->app_id) &&               \
     (((id1)->app_id == ESP_ZB_ZGP_APP_ID_0000)        \
          ? ((id1)->addr.src_id == (id2)->addr.src_id) \
          : (!memcmp(&(id1)->addr.ieee_addr, &(id2)->addr.ieee_addr, sizeof(esp_zb_ieee_addr_t)) && ((id1)->endpoint == (id2)->endpoint))))

#define ESP_ZB_GPDF_CMD_UNDEFINED (0xff)
#define ESP_ZB_ZGP_ZCL_CMD_ID_UNDEFINED (0xff)
#define ESP_ZB_ZGP_ZCL_CLUSTER_ID_UNDEFINED (0xffff)
#define ESP_ZB_ZGP_MAPPING_ENTRY_PARSED_PAYLOAD (0xfe)

/**
 * @brief Possible ZGP commissioning result
 */
typedef enum esp_zb_zgp_comm_status_e {
    ESP_ZB_ZGP_COMMISSIONING_COMPLETED,         /*!< Commissioning with some device completed successfully */
    ESP_ZB_ZGP_COMMISSIONING_FAILED,            /*!< Commissioning failed. */
    ESP_ZB_ZGP_COMMISSIONING_TIMED_OUT,         /*!< Commissioning failed, because of timeout */
    ESP_ZB_ZGP_COMMISSIONING_NO_MATCH_ERROR,    /*!< No functionality match with commissioning device is found. */
    ESP_ZB_ZGP_COMMISSIONING_INTERNAL_ERROR,    /*!< Commissioning failed, because some internal error occurred in stack. */
    ESP_ZB_ZGP_COMMISSIONING_EXTERNAL_ERROR,    /*!< Commissioning failed, because some external error has occurred. */
    ESP_ZB_ZGP_COMMISSIONING_CANCELLED_BY_USER, /*!< User cancelled commissioning by calling esp_zb_zgps_stop_commissioning */
    ESP_ZB_ZGP_ZGPD_DECOMMISSIONED,             /*!< ZGPD sent Decommissioning command */
} esp_zb_zgp_comm_status_t;

/** @brief possible types of GP device */
typedef enum esp_zgp_gp_device_e {
    ESP_ZGP_DEVICE_PROXY              = 0x0060,   /*!< Proxy device */
    ESP_ZGP_DEVICE_PROXY_BASIC        = 0x0061,   /*!< Proxy basic device */
    ESP_ZGP_DEVICE_TARGET_PLUS        = 0x0062,   /*!< Target plus device */
    ESP_ZGP_DEVICE_TARGET             = 0x0063,   /*!< Target device */
    ESP_ZGP_DEVICE_COMMISSIONING_TOOL = 0x0064,   /*!< Commissioning tool device */
    ESP_ZGP_DEVICE_COMBO              = 0x0065,   /*!< Combo device */
    ESP_ZGP_DEVICE_COMBO_BASIC        = 0x0066    /*!< Combo basic device */
} esp_zgp_gp_device_t;

/**
 * @brief ZGP Sink mode change reason
 */
typedef enum esp_zb_zgp_mode_change_reason_e {
    ESP_ZB_ZGP_MODE_CHANGE_TRIGGERED_BY_COMMAND = 1, /*!< GP Sink Commissioning Mode Command */
    ESP_ZB_ZGP_MODE_CHANGE_TRIGGERED_BY_USER,        /*!< Application decided to change the mode */
    ESP_ZB_ZGP_MODE_CHANGE_ON_FIRST_PARING_EXIT,     /*!< The device joined and the gpsCommissioningExitMode attribute has "On First Pairing success" bit seted */
    ESP_ZB_ZGP_MODE_CHANGE_TIMEOUT,                  /*!< It is possible if timeout is set and expired  "On CommissioningWindow expiration" bit is set Timeout parameter to
                                                          user API gps_commissioning_start().*/
} esp_zb_zgp_mode_change_reason_t;

/**
 * @brief ZGP communication mode
 */
typedef enum esp_zb_zgp_communication_mode_e {
    ESP_ZB_ZGP_COMMUNICATION_MODE_FULL_UNICAST = 0,          /*!< Full unicast */
    ESP_ZB_ZGP_COMMUNICATION_MODE_GROUPCAST_DERIVED,         /*!< Groupcast derived */
    ESP_ZB_ZGP_COMMUNICATION_MODE_GROUPCAST_PRECOMMISSIONED, /*!< Groupcast percommissioned */
    ESP_ZB_ZGP_COMMUNICATION_MODE_LIGHTWEIGHT_UNICAST,       /*!< Light weight unicate */
} esp_zb_zgp_communication_mode_t;


typedef enum esp_zgp_commissioning_exit_mode_e {
    ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION  = (1<<0),
    ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_PAIRING_SUCCESS                  = (1<<1),
    ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT = (1<<2),
    ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_CWE_OR_PS                        = (ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION |
                                                                          ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_PAIRING_SUCCESS),
    ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_CWE_OR_PCM                       = (ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION |
                                                                          ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT),
    ESP_ZGP_COMMISSIONING_EXIT_MODE_ALL                                 = (ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION |
                                                                          ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_PAIRING_SUCCESS |
                                                                          ESP_ZGP_COMMISSIONING_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT)
} esp_zgp_commissioning_exit_mode_t;

/**
 * @brief Current mode of ZGP endpoint
 */
typedef enum esp_zb_zgp_mode_e {
    ESP_ZB_ZGP_OPERATIONAL_MODE,  /*!< Operational mode */
    ESP_ZB_ZGP_COMMISSIONING_MODE /*!< Commissioning mode */
} esp_zb_zgp_mode_t;

/**
 * @brief ZGP device application ID
 */
typedef enum esp_zb_zgp_app_id_e {
    ESP_ZB_ZGP_APP_ID_0000    = 0x00, /*!< ApplicationID value 0b000 - usage of the SrcID */
    ESP_ZB_ZGP_APP_ID_0001    = 0x01, /*!< ApplicationID value 0b001 - LPED */
    ESP_ZB_ZGP_APP_ID_0010    = 0x02, /*!< ApplicationID value 0b010 - usage of the GPD IEEE address.*/
    ESP_ZB_ZGP_APP_ID_INVALID = 0x07, /*!< Invalid ApplicationID */
} esp_zb_zgp_app_id_t;

/**
 * @brief ZGP table request entries type
 *
 */
typedef enum esp_zgp_table_request_entries_type_e {
    ESP_ZGP_REQUEST_TABLE_ENTRIES_BY_GPD_ID,    /*!< GPD ID type */
    ESP_ZGP_REQUEST_TABLE_ENTRIES_BY_INDEX      /*!< Index type */
} esp_zgp_table_request_entries_type_t;

/**
 * @brief ZGP security level type
 */
typedef enum esp_zb_zgp_security_level_e {
    ESP_ZB_ZGP_SEC_LEVEL_NO_SECURITY = 0x00,   /*!< No security */
    ESP_ZB_ZGP_SEC_LEVEL_REDUCED = 0x01,       /*!< 1LSB of frame counter and short (2B) MIC */
    ESP_ZB_ZGP_SEC_LEVEL_FULL_NO_ENC = 0x02,   /*!< Full (4B) frame counter and full (4B) MIC */
    ESP_ZB_ZGP_SEC_LEVEL_FULL_WITH_ENC = 0x03, /*!< Encryption & full (4B) frame counter and full (4B) MIC */
} esp_zb_zgp_security_level_t;

/**
 * @brief ZGP security key type
 */
typedef enum esp_zb_zgp_security_key_type_e {
    ESP_ZB_ZGP_SEC_KEY_TYPE_NO_KEY = 0x00,             /*!< No key */
    ESP_ZB_ZGP_SEC_KEY_TYPE_NWK = 0x01,                /*!< Zigbee NWK key */
    ESP_ZB_ZGP_SEC_KEY_TYPE_GROUP = 0x02,              /*!< ZGPD group key */
    ESP_ZB_ZGP_SEC_KEY_TYPE_GROUP_NWK_DERIVED = 0x03,  /*!< NWK-key derived ZGPD group key */
    ESP_ZB_ZGP_SEC_KEY_TYPE_ZGPD_INDIVIDUAL = 0x04,    /*!< (Individual) out-of-the-box ZGPD key */
    ESP_ZB_ZGP_SEC_KEY_TYPE_DERIVED_INDIVIDUAL = 0x07, /*!< Derived individual ZGPD key */
} esp_zb_zgp_security_key_type_t;

/**
 * @brief ZGP gpsSecurityLevel attribute
 */
typedef enum esp_zb_zgp_security_level_protection_with_gp_link_key_e {
    ESP_ZB_ZGP_SEC_LEVEL_PROTECTION_WITHOUT_GP_LINK_KEY = 0x00, /*!< Do not used GP link key */
    ESP_ZB_ZGP_SEC_LEVEL_PROTECTION_WITH_GP_LINK_KEY = 0x01,    /*!< Use GP link key */
} esp_zb_zgp_security_level_protection_with_gp_link_key_t;

/**
 * @brief ZGP gpsSecurityLevel attribute
 */
typedef enum esp_zb_zgp_security_level_involve_tc_e {
    ESP_ZB_ZGP_SEC_LEVEL_PROTECTION_DO_NOT_INVOLVE_TC = 0x00, /*!< Do not involve TC */
    ESP_ZB_ZGP_SEC_LEVEL_PROTECTION_INVOLVE_TC = 0x01,        /*!< Involve TC */
} esp_zb_zgp_security_level_involve_tc_t;

/**
 * @brief ZGPD commissioning method
 */
typedef enum esp_zb_zgpd_commissioning_method_e {
    ESP_ZB_ZGPD_COMMISSIONING_BIDIR,         /*!< Bidirectional commissioning */
    ESP_ZB_ZGPD_COMMISSIONING_UNIDIR,        /*!< Unidirectional commissioning */
    ESP_ZB_ZGPD_COMMISSIONING_AUTO           /*!< Auto-commissioning */
} esp_zb_zgpd_commissioning_method_t;

/**
 * @brief Possible ZGPD device identifiers
 */
typedef enum esp_zb_zgpd_dev_id_e {
    ESP_ZB_ZGP_SIMPLE_GEN_1_STATE_SWITCH_DEV_ID   = 0x00, /*!< Simple Generic 1-state ZGP switch */
    ESP_ZB_ZGP_SIMPLE_GEN_2_STATE_SWITCH_DEV_ID   = 0x01, /*!< Simple Generic 2-state ZGP switch */
    ESP_ZB_ZGP_ON_OFF_SWITCH_DEV_ID               = 0x02, /*!< ZGP On/Off switch */
    ESP_ZB_ZGP_LEVEL_CONTROL_SWITCH_DEV_ID        = 0x03, /*!< ZGP Level Control Switch */
    ESP_ZB_ZGP_SIMPLE_SENSOR_DEV_ID               = 0x04, /*!< ZGP Simple Sensor */
    ESP_ZB_ZGP_ADVANCED_GEN_1_STATE_SWITCH_DEV_ID = 0x05, /*!< Advanced Generic 1-state ZGP switch */
    ESP_ZB_ZGP_ADVANCED_GEN_2_STATE_SWITCH_DEV_ID = 0x06, /*!< Advanced Generic 2-state ZGP switch */
    ESP_ZB_ZGP_GEN_8_CONT_SWITCH_DEV_ID           = 0x07, /*!< Generic 8-contact ZGP switch */
    ESP_ZB_ZGP_COLOR_DIMMER_SWITCH_DEV_ID         = 0x10, /*!< Color Dimmer ZGP Switch */
    ESP_ZB_ZGP_LIGHT_SENSOR_DEV_ID                = 0x11, /*!< ZGP Light Sensor */
    ESP_ZB_ZGP_OCCUPANCY_SENSOR_DEV_ID            = 0x12, /*!< ZGP Occupancy Sensor */
    ESP_ZB_ZGP_DOOR_LOCK_CONTROLLER_DEV_ID        = 0x20, /*!< ZGP Door Lock Controller */
    ESP_ZB_ZGP_TEMPERATURE_SENSOR_DEV_ID          = 0x30, /*!< ZGP temperature sensor */
    ESP_ZB_ZGP_PRESSURE_SENSOR_DEV_ID             = 0x31, /*!< ZGP Pressure Sensor */
    ESP_ZB_ZGP_FLOW_SENSOR_DEV_ID                 = 0x32, /*!< ZGP Flow sensor */
    ESP_ZB_ZGP_ENVIRONMENT_SENSOR_DEV_ID          = 0x33, /*!< ZGP Temperature + Humidity sensor */
    ESP_ZB_ZGP_MANUF_SPECIFIC_DEV_ID              = 0xfe, /*!< Manufactures-specific; more fields in the Commissioning frame. See 4.1 */
    ESP_ZB_ZGP_UNDEFINED_DEV_ID                   = 0xff, /*!< Undefined device type */
} esp_zb_zgpd_dev_id_t;

/**
 * @brief Green power proxy funtionality
 *
 */
typedef enum esp_zgp_gpp_functionality_e {
    ESP_ZGP_GPP_GP_FEATURE                               = (1 <<  0),
    ESP_ZGP_GPP_DIRECT_COMMUNICATION                     = (1 <<  1),
    ESP_ZGP_GPP_DERIVED_GROUPCAST_COMMUNICATION          = (1 <<  2),
    ESP_ZGP_GPP_PRECOMMISSIONED_GROUPCAST_COMMUNICATION  = (1 <<  3),
    ESP_ZGP_GPP_FULL_UNICAST_COMMUNICATION               = (1 <<  4),
    ESP_ZGP_GPP_LIGHTWEIGHT_UNICAST_COMMUNICATION        = (1 <<  5),
    ESP_ZGP_GPP_RESERVED_B6                              = (1 <<  6),
    ESP_ZGP_GPP_BIDIRECTIONAL_OPERATION                  = (1 <<  7),
    ESP_ZGP_GPP_PROXY_TABLE_MAINTENANCE                  = (1 <<  8),
    ESP_ZGP_GPP_RESERVED_B9                              = (1 <<  9),
    ESP_ZGP_GPP_GP_COMMISSIONING                         = (1 << 10),
    ESP_ZGP_GPP_CT_BASED_COMMISSIONING                   = (1 << 11),
    ESP_ZGP_GPP_MAINTENANCE_OF_GPD                       = (1 << 12),
    ESP_ZGP_GPP_SEC_LEVEL_NO_SECURITY                    = (1 << 13),
    ESP_ZGP_GPP_SEC_LEVEL_REDUCED                        = (1 << 14),
    ESP_ZGP_GPP_SEC_LEVEL_FULL_NO_ENC                    = (1 << 15),
    ESP_ZGP_GPP_SEC_LEVEL_FULL_WITH_ENC                  = (1 << 16),
    ESP_ZGP_GPP_RESERVED_B17                             = (1 << 17),
    ESP_ZGP_GPP_RESERVED_B18                             = (1 << 18),
    ESP_ZGP_GPP_GPD_IEEE_ADDRESS                         = (1 << 19)
} esp_zgp_gpp_functionality_t;

/**
 * @brief Green power sink funtionality
 *
 */
typedef enum esp_zgp_gps_functionality_e {
    ESP_ZGP_GPS_GP_FEATURE                               = (1 <<  0),
    ESP_ZGP_GPS_DIRECT_COMMUNICATION                     = (1 <<  1),
    ESP_ZGP_GPS_DERIVED_GROUPCAST_COMMUNICATION          = (1 <<  2),
    ESP_ZGP_GPS_PRECOMMISSIONED_GROUPCAST_COMMUNICATION  = (1 <<  3),
    ESP_ZGP_GPS_FULL_UNICAST_COMMUNICATION               = (1 <<  4),
    ESP_ZGP_GPS_LIGHTWEIGHT_UNICAST_COMMUNICATION        = (1 <<  5),
    ESP_ZGP_GPS_PROXIMITY_BIDIRECTIONAL_OPERATION        = (1 <<  6),
    ESP_ZGP_GPS_MULTIHOP_BIDIRECTIONAL_OPERATION         = (1 <<  7),
    ESP_ZGP_GPS_PROXY_TABLE_MAINTENANCE                  = (1 <<  8),
    ESP_ZGP_GPS_PROXIMITY_COMMISSIONING                  = (1 <<  9),
    ESP_ZGP_GPS_MULTIHOP_COMMISSIONING                   = (1 << 10),
    ESP_ZGP_GPS_CT_BASED_COMMISSIONING                   = (1 << 11),
    ESP_ZGP_GPS_MAINTENANCE_OF_GPD                       = (1 << 12),
    ESP_ZGP_GPS_SEC_LEVEL_NO_SECURITY                    = (1 << 13),
    ESP_ZGP_GPS_SEC_LEVEL_REDUCED                        = (1 << 14),
    ESP_ZGP_GPS_SEC_LEVEL_FULL_NO_ENC                    = (1 << 15),
    ESP_ZGP_GPS_SEC_LEVEL_FULL_WITH_ENC                  = (1 << 16),
    ESP_ZGP_GPS_SINK_TABLE_BASED_GROUPCAST_FORWARDING    = (1 << 17),
    ESP_ZGP_GPS_TRANSLATION_TABLE                        = (1 << 18),
    ESP_ZGP_GPS_GPD_IEEE_ADDRESS                         = (1 << 19)
} esp_zgp_gps_functionality_t;


#define ESP_ZB_GPDF_CMD_STORE_SCENE0 ESP_ZB_GPDF_CMD_RECALL_SCENE8
#define ESP_ZB_GPDF_CMD_STORE_SCENE1 ESP_ZB_GPDF_CMD_RECALL_SCENE9
#define ESP_ZB_GPDF_CMD_STORE_SCENE2 ESP_ZB_GPDF_CMD_RECALL_SCENE10
#define ESP_ZB_GPDF_CMD_STORE_SCENE3 ESP_ZB_GPDF_CMD_RECALL_SCENE11
#define ESP_ZB_GPDF_CMD_LVL_CTRL_STOP ESP_ZB_GPDF_CMD_LC_STOP
#define ESP_ZB_GPDF_CMD_MOVE_UP_WITH_ON_OFF ESP_ZB_GPDF_CMD_MOVE_UP_W_ONOFF
#define ESP_ZB_GPDF_CMD_MOVE_DOWN_WITH_ON_OFF ESP_ZB_GPDF_CMD_MOVE_DOWN_W_ONOFF
#define ESP_ZB_GPDF_CMD_MOVE_STEP_ON ESP_ZB_GPDF_CMD_STEP_UP_W_ONOFF
#define ESP_ZB_GPDF_CMD_MOVE_STEP_OFF ESP_ZB_GPDF_CMD_STEP_DOWN_W_ONOFF

/**
 * @brief ZGP device Cluster List
 */
typedef struct esp_zb_zgp_cluster_list_s {
    uint8_t server_cl_num;                                     /*!< Number of server ClusterIDs */
    uint8_t client_cl_num;                                     /*!< Number of client ClusterIDs */
    uint16_t server_clusters[ZB_ZGP_MAX_PAIRED_CONF_CLUSTERS]; /*!< ClusterID List Server */
    uint16_t client_clusters[ZB_ZGP_MAX_PAIRED_CONF_CLUSTERS]; /*!< ClusterID List Client */
} esp_zb_zgp_cluster_list_t;

/**
 * @brief ZGP device address
 *
 * @note ZGPD is identified in network whether by SrcId or its IEEE address.
 */
typedef union esp_zb_zgpd_addr_u {
    uint32_t src_id;              /*!< ZGPD source id */
    esp_zb_ieee_addr_t ieee_addr; /*!< ZGPD IEEE address */
} ESP_ZB_PACKED_STRUCT esp_zb_zgpd_addr_t;

/**
 * @brief ZGP device identifier
 */
typedef struct esp_zb_zgpd_id_s {
    uint8_t app_id;          /*!< One of the esp_zb_zgp_app_id_t values */
    uint8_t endpoint;        /*!< Identifier of the GPD endpoint, which jointly with the GPD IEEE address identifies a unique logical GPD device.*/
    esp_zb_zgpd_addr_t addr; /*!< ZGPD SrcId or IEEE address */
} ESP_ZB_PACKED_STRUCT esp_zb_zgpd_id_t;

/**
 * @brief ZGP device CommandID list
 */
typedef struct esp_zb_zgp_gpd_cmds_list_s {
    uint8_t num;                                       /*!< Number of GP commands */
    uint8_t cmds[ZB_ZGP_MAX_PAIRED_CONF_GPD_COMMANDS]; /*!< GPD CommandID list */
} esp_zb_zgp_gpd_cmds_list_t;

/**
 * @brief Mapping table entry structure for ZGP sink
 */
typedef struct esp_zb_zgps_mapping_entry_s {
    uint8_t options;            /*!< Options related to this table entry */
    esp_zb_zgpd_addr_t gpd_id;  /*!< Identifier of the GPD */
    uint8_t gpd_endpoint;       /*!< Endpoint */
    uint8_t gpd_command;        /*!< The GPD command to be mapped */
    uint8_t endpoint;           /*!< The EndPoint for which the mapping is valid */
    uint16_t profile;           /*!< The Profile of the command after mapping */
    uint16_t cluster;           /*!< The cluster of the Profile on the endpoint */
    uint8_t zcl_command;        /*!< The Command ID of the Cluster into which GP Command is mapped */
    uint8_t zcl_payload_length; /*!< The payload length for the Zigbee Command, payload should be located in memory after this structure */
} ESP_ZB_PACKED_STRUCT esp_zb_zgps_mapping_entry_t;

/**
 * @brief Raw report descriptor received from the GPD
 */
typedef struct esp_zb_zgp_raw_report_desc_s {
    uint8_t len;                                     /*!< total len, in octets, of the current report descriptor. */
    uint8_t data[ZB_ZGP_APP_DESCR_REPORT_DATA_SIZE]; /*!< array of not parsed zgp_data_point_desc_t */
} esp_zb_zgp_raw_report_desc_t;

/**
 * @brief  The structure for the paired group List
 *
 */
typedef struct esp_zgp_pair_group_list_s {
    uint16_t sink_group;        /*!< Sink group */
    uint16_t alias;             /*!< Alias for the group communication */
} ESP_ZB_PACKED_STRUCT esp_zgp_pair_group_list_t;

/**
 * @brief The structure for Green Power table entry
 *
 */
typedef struct esp_zgp_tbl_ent_s {
    esp_zb_zgpd_addr_t   zgpd_id;                 /*!< ID of the paired ZGPD */
    uint16_t      options;                        /*!< The options for the reception from ZGPD */
    uint16_t      zgpd_assigned_alias;            /*!< The commissioned 16-bit ID to be used as alias for ZGPD */
    uint32_t      security_counter;               /*!< The incoming security frame counter for ZGPD */
    uint8_t       zgpd_key[ESP_ZB_CCM_KEY_SIZE];  /*!< Security key for the GPD */
    uint8_t       endpoint;                       /*!< Endpoint pair of IEEE:EP if App ID is 010.  */
    uint8_t       sec_options;                    /*!< Security options */
    uint8_t       groupcast_radius;               /*!< To limit the range of the groupcast */
    uint8_t       is_sink;                        /*!< Sink or proxy */
    union {
      struct esp_zgp_proxy_tbl_ent_s {
        uint16_t ext_options;                                           /*!< The tunneling options */
        struct esp_zgp_lwsink_addr_list_s {
          uint8_t addr_ref;                                             /*!< Address reference */
        } lwsaddr[ZB_ZGP_MAX_LW_UNICAST_ADDR_PER_GPD];                  /*!< Sink address list */
        esp_zgp_pair_group_list_t sgrp[ZB_ZGP_MAX_SINK_GROUP_PER_GPD];  /*!< Sink group list */
      } proxy;                                                          /*!< Proxy table */
      struct esp_zgp_sink_tbl_ent_s {
        uint8_t       device_id;                                        /*!< ZGPD Device ID from Commissioning frame, refer to esp_zb_zgpd_dev_id_t */
        esp_zgp_pair_group_list_t sgrp[ZB_ZGP_MAX_SINK_GROUP_PER_GPD];  /*!< Sink group list */
        uint8_t match_dev_tbl_idx;                                      /*!< index in matching table matched by device_id or app_info.manuf_model_id  */
        esp_zb_ieee_addr_t   ieee_addr;                                 /*!< Extension to the table, Dest IEEE address to use in GPT to GPD packets even if GPD is identified by SrcID */
      } sink;                                                           /*!< Sink table */
    } u;                                                                /*!< ZGP table */
} esp_zgp_tbl_ent_t;

/**
 * @brief Sink table structure
 * 
 */
typedef esp_zgp_tbl_ent_t esp_zb_zgp_sink_tbl_ent_t;

/**
 * @brief Proxy table structure
 * 
 */
typedef esp_zgp_tbl_ent_t esp_zb_zgp_proxy_tbl_ent_t;

/**
 * @brief Set whether skip the Green power device frame
 *
 * @param[in] skip Skip or not
 */
void esp_zb_zgp_set_skip_gpdf(uint8_t skip);

/**
 * @brief Get the setting of skip the Green power device frame
 *
 * @return
 *    - true: Skip
 *    - false: Not skip
 */
uint8_t esp_zb_zgp_get_skip_gpdf(void);

/**
 * @brief Set shared security key type
 *
 * @param[in] type The type of security key
 */
void esp_zb_zgp_set_shared_security_key_type(esp_zb_zgp_security_key_type_t type);

/**
 * @brief Set shared security key
 *
 * @param[in] key The 16-byte pointer will be set as the shared security keyof green power
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 */
esp_err_t esp_zb_zgp_set_shared_security_key(uint8_t *key);

/**
 * @brief Set shared security key
 *
 * @param[in] key The 16-byte pointer will be set as the link key of green power
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 */
esp_err_t esp_zb_zgp_set_link_key(uint8_t *key);

/**
 * @brief Set the ZGP proxy functionality
 *
 * @param functionality         Functionality, refer to esp_zgp_gpp_functionality_t
 * @param active_functionality  Active functionality indicates which bits of functionality are active.
 */
void esp_zb_zgp_set_proxy_functionality(uint32_t functionality, uint32_t active_functionality);

#ifdef __cplusplus
}
#endif
