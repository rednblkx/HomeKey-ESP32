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

#ifdef CONFIG_ZB_GP_ENABLED
#include "zgp/esp_zigbee_zgp.h"
#endif /* CONFIG_ZB_GP_ENABLED */

/**
 * @brief ZDP status values
 * @anchor esp_zb_zdp_status
 * @note the status feedback for the zdo command
 */
typedef enum {
    ESP_ZB_ZDP_STATUS_SUCCESS               = 0x00,   /*!< The requested operation or transmission was completed successfully. */
    ESP_ZB_ZDP_STATUS_INV_REQUESTTYPE       = 0x80,   /*!< The supplied request type was invalid. */
    ESP_ZB_ZDP_STATUS_DEVICE_NOT_FOUND      = 0x81,   /*!< The requested device did not exist on a device following a child descriptor request to a parent.*/
    ESP_ZB_ZDP_STATUS_INVALID_EP            = 0x82,   /*!< The supplied endpoint was equal to 0x00 or between 0xf1 and 0xff. */
    ESP_ZB_ZDP_STATUS_NOT_ACTIVE            = 0x83,   /*!< The requested endpoint is not described by simple descriptor. */
    ESP_ZB_ZDP_STATUS_NOT_SUPPORTED         = 0x84,   /*!< The requested optional feature is not supported on the target device. */
    ESP_ZB_ZDP_STATUS_TIMEOUT               = 0x85,   /*!< A timeout has occurred with the requested operation. */
    ESP_ZB_ZDP_STATUS_NO_MATCH              = 0x86,   /*!< The end device bind request was unsuccessful due to a failure to match any suitable clusters.*/
    ESP_ZB_ZDP_STATUS_NO_ENTRY              = 0x88,   /*!< The unbind request was unsuccessful due to the coordinator or source device not having an entry in its binding table to unbind.*/
    ESP_ZB_ZDP_STATUS_NO_DESCRIPTOR         = 0x89,   /*!< A child descriptor was not available following a discovery request to a parent. */
    ESP_ZB_ZDP_STATUS_INSUFFICIENT_SPACE    = 0x8a,   /*!< The device does not have storage space to support the requested operation. */
    ESP_ZB_ZDP_STATUS_NOT_PERMITTED         = 0x8b,   /*!< The device is not in the proper state to support the requested operation. */
    ESP_ZB_ZDP_STATUS_TABLE_FULL            = 0x8c,   /*!< The device does not have table space to support the operation. */
    ESP_ZB_ZDP_STATUS_NOT_AUTHORIZED        = 0x8d,   /*!< The permissions configuration table on the target indicates that the request is not authorized from this device.*/
    ESP_ZB_ZDP_STATUS_BINDING_TABLE_FULL    = 0x8e,   /*!< The device doesn't have binding table space to support the operation */
    ESP_ZB_ZDP_STATUS_INVALID_INDEX         = 0x8f,   /*!< The index in the received command is out of bounds. */
} esp_zb_zdp_status_t;

/**
 * @brief signal passed to the esp_zb_application_signal_handler
 * @anchor esp_zb_app_signal_type_t
 * @note It indicates current BDB, NWK or ZDO signal is finished
 */
typedef enum {
    ESP_ZB_ZDO_SIGNAL_DEFAULT_START                             = 0x00,
    ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP                              = 0x01,
    ESP_ZB_ZDO_SIGNAL_DEVICE_ANNCE                              = 0x02,
    ESP_ZB_ZDO_SIGNAL_LEAVE                                     = 0x03,
    ESP_ZB_ZDO_SIGNAL_ERROR                                     = 0x04,
    ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START                        = 0x05,
    ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT                             = 0x06,
    ESP_ZB_BDB_SIGNAL_TOUCHLINK_NWK_STARTED                     = 0x07,
    ESP_ZB_BDB_SIGNAL_TOUCHLINK_NWK_JOINED_ROUTER               = 0x08,
    ESP_ZB_BDB_SIGNAL_TOUCHLINK                                 = 0x09,
    ESP_ZB_BDB_SIGNAL_STEERING                                  = 0x0a,
    ESP_ZB_BDB_SIGNAL_FORMATION                                 = 0x0b,
    ESP_ZB_BDB_SIGNAL_FINDING_AND_BINDING_TARGET_FINISHED       = 0x0c,
    ESP_ZB_BDB_SIGNAL_FINDING_AND_BINDING_INITIATOR_FINISHED    = 0x0d,
    ESP_ZB_BDB_SIGNAL_TOUCHLINK_TARGET                          = 0x0e,
    ESP_ZB_BDB_SIGNAL_TOUCHLINK_NWK                             = 0x0f,
    ESP_ZB_BDB_SIGNAL_TOUCHLINK_TARGET_FINISHED                 = 0x10,
    ESP_ZB_BDB_SIGNAL_TOUCHLINK_ADD_DEVICE_TO_NWK               = 0x11,
    ESP_ZB_NWK_SIGNAL_DEVICE_ASSOCIATED                         = 0x12,
    ESP_ZB_ZDO_SIGNAL_LEAVE_INDICATION                          = 0x13,
    ESP_ZB_BDB_SIGNAL_WWAH_REJOIN_STARTED                       = 0x14,
    ESP_ZB_ZGP_SIGNAL_COMMISSIONING                             = 0x15,
    ESP_ZB_COMMON_SIGNAL_CAN_SLEEP                              = 0x16,
    ESP_ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY                   = 0x17,
    ESP_ZB_NWK_SIGNAL_NO_ACTIVE_LINKS_LEFT                      = 0x18,
    ESP_ZB_SE_SIGNAL_SKIP_JOIN                                  = 0x19,
    ESP_ZB_SE_SIGNAL_REJOIN                                     = 0x1a,
    ESP_ZB_SE_SIGNAL_CHILD_REJOIN                               = 0x1b,
    ESP_ZB_SE_TC_SIGNAL_CHILD_JOIN_CBKE                         = 0x1c,
    ESP_ZB_SE_TC_SIGNAL_CHILD_JOIN_NON_CBKE                     = 0x1d,
    ESP_ZB_SE_SIGNAL_CBKE_FAILED                                = 0x1e,
    ESP_ZB_SE_SIGNAL_CBKE_OK                                    = 0x1f,
    ESP_ZB_SE_SIGNAL_SERVICE_DISCOVERY_START                    = 0x20,
    ESP_ZB_SE_SIGNAL_SERVICE_DISCOVERY_DO_BIND                  = 0x21,
    ESP_ZB_SE_SIGNAL_SERVICE_DISCOVERY_BIND_OK                  = 0x22,
    ESP_ZB_SE_SIGNAL_SERVICE_DISCOVERY_BIND_FAILED              = 0x23,
    ESP_ZB_SE_SIGNAL_SERVICE_DISCOVERY_BIND_INDICATION          = 0x24,
    ESP_ZB_SE_SIGNAL_SERVICE_DISCOVERY_OK                       = 0x25,
    ESP_ZB_SE_SIGNAL_SERVICE_DISCOVERY_FAILED                   = 0x26,
    ESP_ZB_SE_SIGNAL_APS_KEY_READY                              = 0x27,
    ESP_ZB_SE_SIGNAL_APS_KEY_FAIL                               = 0x28,
    ESP_ZB_SIGNAL_SUBGHZ_SUSPEND                                = 0x29,
    ESP_ZB_SIGNAL_SUBGHZ_RESUME                                 = 0x2a,
    ESP_ZB_MACSPLIT_DEVICE_BOOT                                 = 0x2b,
    ESP_ZB_MACSPLIT_DEVICE_READY_FOR_UPGRADE                    = 0x2c,
    ESP_ZB_MACSPLIT_DEVICE_FW_UPGRADE_EVENT                     = 0x2d,
    ESP_ZB_SIGNAL_NWK_INIT_DONE                                 = 0x2e,
    ESP_ZB_ZDO_SIGNAL_DEVICE_AUTHORIZED                         = 0x2f,
    ESP_ZB_ZDO_SIGNAL_DEVICE_UPDATE                             = 0x30,
    ESP_ZB_NWK_SIGNAL_PANID_CONFLICT_DETECTED                   = 0x31,
    ESP_ZB_NLME_STATUS_INDICATION                               = 0x32,
    ESP_ZB_TCSWAP_DB_BACKUP_REQUIRED_SIGNAL                     = 0x33,
    ESP_ZB_TC_SWAPPED_SIGNAL                                    = 0x34,
    ESP_ZB_BDB_SIGNAL_TC_REJOIN_DONE                            = 0x35,
    ESP_ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS                        = 0x36,
    ESP_ZB_BDB_SIGNAL_STEERING_CANCELLED                        = 0x37,
    ESP_ZB_BDB_SIGNAL_FORMATION_CANCELLED                       = 0x38,
    ESP_ZB_SIGNAL_READY_TO_SHUT                                 = 0x39,
    ESP_ZB_SIGNAL_INTERPAN_PREINIT                              = 0x3a,
    ESP_ZB_ZGP_SIGNAL_MODE_CHANGE                               = 0x3b,
    ESP_ZB_ZDO_DEVICE_UNAVAILABLE                               = 0x3c,
    ESP_ZB_ZGP_SIGNAL_APPROVE_COMMISSIONING                     = 0x3d,
    ESP_ZB_SIGNAL_END                                           = 0x3e,
} esp_zb_app_signal_type_t;

/**
 * @brief The struct of esp zboss app signal message table
 * @anchor esp_zb_app_signal_msg_t
 */
typedef struct
{
    esp_zb_app_signal_type_t signal;        /*!< The signal type of Zigbee */
    const char *msg;                        /*!< The string of Zigbee signal */
} esp_zb_app_signal_msg_t;

/**
 * @brief The enum of mode of Base Device Behavior (BDB)
 * @anchor esp_zb_bdb_commissioning_mode
 */
typedef enum {
    ESP_ZB_BDB_MODE_INITIALIZATION                  = 0,
    ESP_ZB_BDB_MODE_NETWORK_STEERING                = 2,
    ESP_ZB_BDB_MODE_NETWORK_FORMATION               = 4,
} esp_zb_bdb_commissioning_mode_t;

/**
 * @brief The enum of bind request destination address mode
 * @anchor esp_zb_zdo_bind_dst_addr_mode_t
 */
typedef enum {
    ESP_ZB_ZDO_BIND_DST_ADDR_MODE_16_BIT_GROUP                  = 0x01U,
    ESP_ZB_ZDO_BIND_DST_ADDR_MODE_64_BIT_EXTENDED               = 0x03U,
} esp_zb_zdo_bind_dst_addr_mode_t;

/* zdo param define */
/**
 * @name Leave types
 * @anchor nwk_leave_type
 */
typedef enum {
    ESP_ZB_NWK_LEAVE_TYPE_RESET                 = 0x00, /*!< Leave without rejoin */
    ESP_ZB_NWK_LEAVE_TYPE_REJOIN                = 0x01  /*!< Leave with rejoin */
} esp_zb_nwk_leave_type_t;

/**
 * @brief Leave parameters
 *
 * @note Stack passes this parameter to application when device leave the network.
 */
typedef struct esp_zb_zdo_signal_leave_params_s {
    uint8_t leave_type;             /*!< Leave type. @ref nwk_leave_type */
} esp_zb_zdo_signal_leave_params_t;

/**
 * @brief Leave Indication parameters
 *
 * @note Stack passes this parameter to application upon receipt of leave indication primitive.
 */
typedef struct esp_zb_zdo_signal_leave_indication_params_s {
    uint16_t short_addr;                    /*!< Short address of device requested to leave or leaving device*/
    esp_zb_ieee_addr_t device_addr;         /*!< Long address of device requested to leave or leaving device*/
    uint8_t rejoin;                         /*!< 1 if this was leave with rejoin; 0 - otherwise */
} esp_zb_zdo_signal_leave_indication_params_t;

/**
 * @brief Association parameters
 *
 * @note Stack passes this parameter to application when some device joins/rejoins to network.
 */
typedef struct esp_zb_zdo_signal_device_annce_params_s {
    uint16_t device_short_addr;           /*!< address of device that recently joined to network */
    esp_zb_ieee_addr_t   ieee_addr;       /*!< The 64-bit (IEEE) address assigned to the device. */
    uint8_t       capability;             /*!< The capability of the device. */
} esp_zb_zdo_signal_device_annce_params_t;

/**
 * @brief Macsplit device boot parameters.
 *
 * @note Macsplit passes this parameter to application when device is booted.
 */
typedef struct esp_zb_zdo_signal_macsplit_dev_boot_params_s {
    uint32_t    dev_version;              /*!< macsplit device version */
    esp_zb_ieee_addr_t extended_address;  /*!< The 64-bit (IEEE) address assigned to the device. */
    char      version_str[80];            /*!< macsplit version string */
} esp_zb_zdo_signal_macsplit_dev_boot_params_t;

/**
 * @brief Device Updated signal parameters
 * @note  status of the device update
            0x00 = Standard device secured rejoin
            0x01 = Standard device unsecured join
            0x02 = Device left
            0x03 = Standard device trust center rejoin
            0x04 – 0x07 = Reserved
 */
typedef struct esp_zb_zdo_signal_device_update_params_s {
    esp_zb_ieee_addr_t long_addr;   /*!< Long Address of the updated device */
    uint16_t short_addr;            /*!< Short Address of the updated device */
    uint8_t status;                 /*!< Indicates the updated status of the device */
} esp_zb_zdo_signal_device_update_params_t;

/**
 * @brief Device authorized signal parameters
 * @note The authorization_type as following:
 *          0x00 = Authorization type for legacy devices ( < r21)
 *              Status:
 *                  0x00: Authorization success
 *                  0x01: Authorization failed
 *          0x01 = Authorization type for r21 device through TCLK
 *              Status:
 *                  0x00: Authorization success
 *                  0x01: Authorization timeout
 *                  0x02: Authorization failed
 *          0x02 = Authorization type for SE through CBKE
 *              Status:
 *                  0x00: Authorization success
 */
typedef struct esp_zb_zdo_signal_device_authorized_params_s {
    esp_zb_ieee_addr_t long_addr; /*!< Long Address of the updated device */
    uint16_t short_addr;          /*!< Short Address of the updated device */
    uint8_t authorization_type;   /*!< Type of the authorization procedure */
    uint8_t authorization_status; /*!< Status of the authorization procedure which depends on authorization_type */
} esp_zb_zdo_signal_device_authorized_params_t;

#ifdef CONFIG_ZB_GP_ENABLED
/**
 * @brief ZGP approve commissioning parameters
 *
 */
typedef struct esp_zgp_approve_comm_params_s {
  esp_zb_zgpd_id_t              zgpd_id;                                /*!< ZGPD ID */
  uint8_t                       device_id;                              /*!< ZGPD device ID */
  uint16_t                      manuf_id;                               /*!< Manufacturer ID (meaningful if device_id = 0xFE or 0xFF) */
  uint16_t                      manuf_model_id;                         /*!< Manufacturer model ID (meaningful if device_id = 0xFE or 0xFF) */
  esp_zb_ieee_addr_t            ieee_addr;                              /*!< ZGPD long IEEE address if available, otherwise filled with zeroes */
  uint8_t                       pairing_endpoint;                       /*!< Device endpoint, on which commissioning is currently active */
  esp_zb_zgp_gpd_cmds_list_t    gpd_cmds_list;                          /*!< ZGPD Command list */
  esp_zb_zgp_cluster_list_t     cluster_list;                           /*!< ZGPD Cluster list */
  uint8_t                       num_of_reports;                         /*!< total number of different report descriptors that GPD sent during the commissioning process */
  esp_zb_zgp_raw_report_desc_t  reports[ZB_ZGP_APP_DESCR_REPORTS_NUM];  /*!< array of reports*/
  bool                          pairing_configuration;                  /*!< It is ZB_TRUE in case this approve signal was triggered by GP Pairing Configuration command */
  uint8_t                       actions;                                /*!< Pairing configuration actions */
  uint8_t                       num_of_endpoints;                       /*!< Pairing configuration number of paired endpoints field
                                                                          - 0x00 and 0xfd: there are no paired endpoints
                                                                          - 0xff: all matching endpoints are to be paired
                                                                          - 0xfe: paired endpoints are to be derived by the sink itself
                                                                          - other values: paired_endpoints field is present and contains the list of local endpoints paired to this GPD
                                                                        **/
  uint8_t              paired_endpoints[ZB_ZGP_MAX_PAIRED_ENDPOINTS];   /*!< Paired endpoint numbers */
} esp_zgp_approve_comm_params_t;

/**
 * @brief ZGP approve commissioning parameter structure
 *
 */
typedef struct esp_zb_zgp_signal_approve_comm_params_s {
  esp_zgp_approve_comm_params_t *param;  /*!< Parameter for approving commissioning */
} esp_zb_zgp_signal_approve_comm_params_t;

/**
 *  @brief ZGP mode changed parameters
 *
 *  Stack passes this parameter to application to notify about GP mode change.
 */
typedef struct esp_zb_zgp_signal_mode_change_params_s {
  esp_zb_zgp_mode_change_reason_t reason; /*!< Reason for mode change, refer to esp_zb_zgp_mode_change_reason_t */
  esp_zb_zgp_mode_t new_mode;             /*!< New mode */
} esp_zb_zgp_signal_mode_change_params_t;

/**
 * @brief ZGP commissioning parameters
 *
 */
typedef struct esp_zb_zgp_signal_commissioning_params_s {
  esp_zb_zgpd_id_t zgpd_id;         /*!< Pointer to GPD ID */
  esp_zb_zgp_comm_status_t result;  /*!< commissioning result, refer to esp_zb_zgp_comm_status_t */
} esp_zb_zgp_signal_commissioning_params_t;
#endif /* CONFIG_ZB_GP_ENABLED */

#ifdef __cplusplus
}
#endif
