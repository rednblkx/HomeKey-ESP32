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

/** @brief OTA upgrade cluster attribute identifiers
*/
typedef enum {                                                               
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ID                     = 0x0000,             /*!< Indicates the address of the upgrade server */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_FILE_OFFSET_ID                = 0x0001,             /*!< Indicates the the current location in the OTA upgrade image */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_FILE_VERSION_ID               = 0x0002,             /*!< Indicates the file version of the running firmware image on the device */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_STACK_VERSION_ID              = 0x0003,             /*!< Brief CurrentZigbeeStackVersion attribute */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_ID    = 0x0004,             /*!< Indicates the file version of the downloaded image on the device */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_DOWNLOADED_STACK_VERSION_ID   = 0x0005,             /*!< Brief DownloadedZigbeeStackVersion attribute */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_STATUS_ID               = 0x0006,             /*!< Indicates the image upgrade status of the client device */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_MANUFACTURE_ID                = 0x0007,             /*!< Indicates the value for the manufacturer of the device */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_TYPE_ID                 = 0x0008,             /*!< Indicates the the image type of the file that the client is currently downloading */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_MIN_BLOCK_REQUE_ID            = 0x0009,             /*!< Indicates the delay between Image Block Request commands in milliseconds */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_IMAGE_STAMP_ID                = 0x000a,             /*!< Brief Image Stamp attribute */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_UPGRADE_ACTIVATION_POLICY_ID  = 0x000b,             /*!< Indicates what behavior the client device supports for activating a fully downloaded but not installed upgrade image */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_UPGRADE_TIMEOUT_POLICY_ID     = 0x000c,             /*!< Indicates what behavior the client device supports for activating a fully downloaded image when the OTA server cannot be reached */
  /* Server variables and Custom data */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ENDPOINT_ID            = 0xfff3,             /*!< Brief OTA server endpoint custom attribute */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_ADDR_ID                = 0xfff2,             /*!< Brief OTA server addr custom attribute */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_CLIENT_DATA_ID                = 0xfff1,             /*!< Brief OTA client data attribute, its type can refer to esp_zb_zcl_ota_upgrade_client_variable_t */
  ESP_ZB_ZCL_ATTR_OTA_UPGRADE_SERVER_DATA_ID                = 0xfff0,             /*!< Brief OTA server data attribute, its type can refer to esp_zb_zcl_ota_upgrade_server_variable_t */
} esp_zb_zcl_ota_upgrade_attr_t;

/** @brief Default value for UpgradeServerID attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_SERVER_DEF_VALUE                                 { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }

/** @brief Default value for FileOffset attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_FILE_OFFSET_DEF_VALUE                            0xffffffff

/** @brief Default value for CurrentFileVersion attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_FILE_VERSION_DEF_VALUE                           0xffffffff

/** @brief Default value for CurrentZigbeeStackVersion attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_STACK_VERSION_DEF_VALUE                          0xffff

/** @brief Default value for DownloadedFileVersion attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_DEF_VALUE                0xffffffff

/** @brief Default value for DownloadedZigbeeStackVersion attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_DOWNLOADED_STACK_DEF_VALUE                       0xffff

/** @brief Default value for ManufacturerCode attribute */
#define ESP_ZB_OTA_UPGRADE_MANUFACTURER_CODE_DEF_VALUE                          0x131B

/** @brief Default value for ImageType attribute */
#define ESP_ZB_OTA_UPGRADE_IMAGE_TYPE_DEF_VALUE                                 0xffbf

/** @brief Manufacturer Specific minimun value for ImageType attribute */
#define ESP_ZB_OTA_UPGRADE_MANUFACTURER_SPECIFIC_IMAGE_TYPE_MIN_VALUE           0x0000

/** @brief Manufacturer Specific maximum value for ImageType attribute */
#define ESP_ZB_OTA_UPGRADE_MANUFACTURER_SPECIFIC_IMAGE_TYPE_MAX_VALUE           0xffbf

/** @brief Client Security credentials value for ImageType attribute */
#define ESP_ZB_OTA_UPGRADE_CLI_SECURITY_CREDENTIALS_IMAGE_TYPE_VALUE            0xffc0

/** @brief Client Configuration value for ImageType attribute */
#define ESP_ZB_OTA_UPGRADE_CLI_CONFIG_IMAGE_TYPE_VALUE                          0xffc1

/** @brief Server Log value for ImageType attribute */
#define ESP_ZB_OTA_UPGRADE_SERVER_LOG_IMAGE_TYPE_VALUE                          0xffc2

/** @brief Picture value for ImageType attribute */
#define ESP_ZB_OTA_UPGRADE_PICTURE_IMAGE_TYPE_VALUE                             0xffc3

/** @brief Wild card value for ImageType attribute */
#define ESP_ZB_OTA_UPGRADE_WILD_CARD_IMAGE_TYPE_VALUE                           0xffff

/** @brief Default value for ImageUpgradeStatus attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE                           0x00

/** @brief Default value for MinimumBlockPeriod attribute */
#define ESP_ZB_OTA_UPGRADE_MIN_BLOCK_PERIOD_DEF_VALUE                           0x0000

/** @brief Maximum value for ImageStamp attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_DEF_VALUE                            0xffffffff

/** @brief Default value for OTA server endpoint custom attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_SERVER_ENDPOINT_DEF_VALUE                        0xff

/** @brief Default value for OTA server addr custom attribute */
#define ESP_ZB_ZCL_OTA_UPGRADE_SERVER_ADDR_DEF_VALUE                            0xffff

/** @brief Default Frequency request server about new upgrade file (minutes) */
#define ESP_ZB_ZCL_OTA_UPGRADE_QUERY_TIMER_COUNT_DEF                            (24*60)

/** @brief Possible statuses of OTA upgrade
 */
typedef enum {
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_START              = 0x0000,                /*!< Starts OTA upgrade */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_APPLY              = 0x0001,                /*!< Checks for manufacturer, image type etc are ok last step before actual upgrade */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_RECEIVE            = 0x0002,                /*!< Process image block */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_FINISH             = 0x0003,                /*!< OTA upgrade completed */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_ABORT              = 0x0004,                /*!< OTA upgrade aborted */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_CHECK              = 0x0005,                /*!< Downloading is finished, do additional checks if needed etc before upgrade end request */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_OK                 = 0x0006,                /*!< OTA upgrade end response is ok */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_ERROR              = 0x0007,                /*!< OTA upgrade return error code */
  ESP_ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_NORMAL       = 0x0008,                /*!< Accepted new image */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_BUSY               = 0x0009,                /*!< Another download is in progress, deny new image */
  ESP_ZB_ZCL_OTA_UPGRADE_STATUS_SERVER_NOT_FOUND   = 0x000A                 /*!< Notify an application that OTA Upgrade server not found */
} esp_zb_zcl_ota_upgrade_status_t;

/** @brief The status of OTA upgrade server */
typedef enum {
  ESP_ZB_ZCL_OTA_UPGRADE_SERVER_STARTED   = 0x00,   /*!< Start OTA */
  ESP_ZB_ZCL_OTA_UPGRADE_SERVER_ABORTED   = 0x01,   /*!< Abort OTA */
  ESP_ZB_ZCL_OTA_UPGRADE_SERVER_END       = 0x02,   /*!< End OTA */
} esp_zb_ota_upgrade_server_status_t;

/** @brief OTA File header - fc fields bitmasks */
typedef enum esp_zb_zcl_ota_upgrade_file_header_fc_e {
  ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_CREDENTIAL_VER      = 1 << 0, /*!< Indicate that the Security Credential Version field will be included. */
  ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_DEVICE_SPECIFIC     = 1 << 1, /*!< Indicate that the Device Specific field will be included. */
  ESP_ZB_ZCL_OTA_UPGRADE_FILE_HEADER_FC_HW_VER              = 1 << 2, /*!< Indicate that the Hardware Versions Present field will be included. */
} esp_zb_zcl_ota_upgrade_file_header_fc_t;

#ifdef __cplusplus
}
#endif