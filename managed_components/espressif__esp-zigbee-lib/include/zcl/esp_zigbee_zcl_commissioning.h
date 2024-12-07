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

/** @brief Commissioning Cluster server attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_COMMISSIONING_SHORT_ADDRESS_ID                        = 0x0000, /**< ShortAddress Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_EXTENDED_PANID_ID                       = 0x0001, /**< ExtendedPANId Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_PANID_ID                                = 0x0002, /**< PANId Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_CHANNEL_MASK_ID                         = 0x0003, /**< ChannelMask Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_PROTOCOL_VERSION_ID                     = 0x0004, /**< ProtocolVersion Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_STACK_PROFILE_ID                        = 0x0005, /**< StackProfile Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_STARTUP_CONTROL_ID                      = 0x0006, /**< StartupControl Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_TRUST_CENTER_ADDRESS_ID                 = 0x0010, /**< TrustCenterAddress Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_TRUST_CENTER_MASTER_KEY_ID              = 0x0011, /**< TrustCenterMasterKey Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_NETWORK_KEY_ID                          = 0x0012, /**< NetworkKey Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_USE_INSECURE_JOIN_ID                    = 0x0013, /**< UseInsecureJoin Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_PRECONFIGURED_LINK_KEY_ID               = 0x0014, /**< PreconfiguredLinkKey Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_NETWORK_KEY_SEQ_NUM_ID                  = 0x0015, /**< NetworkKeySeqNum Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_NETWORK_KEY_TYPE_ID                     = 0x0016, /**< NetworkKeyType Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_NETWORK_MANAGER_ADDRESS_ID              = 0x0017, /**< NetworkManagerAddress Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_SCAN_ATTEMPTS_ID                        = 0x0020, /**< ScanAttempts Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_TIME_BETWEEN_SCANS_ID                   = 0x0021, /**< TimeBetweenScans Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_REJOIN_INTERVAL_ID                      = 0x0022, /**< RejoinInterval Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_MAX_REJOIN_INTERVAL_ID                  = 0x0023, /**< MaxRejoinInterval Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_INDIRECT_POLL_RATE_ID                   = 0x0030, /**< IndirectPollRate Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_PARENT_RETRY_THRESHOLD_ID               = 0x0031, /**< ParentRetryThreshold Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_CONCENTRATOR_FLAG_ID                    = 0x0040, /**< ConcentratorFlag Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_CONCENTRATOR_RADIUS_ID                  = 0x0041, /**< ConcentratorRadius Attribute */
    ESP_ZB_ZCL_ATTR_COMMISSIONING_CONCENTRATOR_DISCOVERY_TIME_ID          = 0x0042, /**< ConcentratorDiscoveryTime Attribute */
} esp_zb_zcl_commissioning_srv_attr_t;

/** @brief Values for valid Startup Control attribute */
typedef enum {
    ESP_ZB_ZCL_ATTR_COMMISSIONING_STARTUP_TYPE_JOINED  = 0x00,
    ESP_ZB_ZCL_ATTR_COMMISSIONING_STARTUP_TYPE_FORM    = 0x01,
    ESP_ZB_ZCL_ATTR_COMMISSIONING_STARTUP_TYPE_REJOIN  = 0x02,
    ESP_ZB_ZCL_ATTR_COMMISSIONING_STARTUP_TYPE_SCRATCH = 0x03,
} esp_zb_zcl_commissioning_startup_type_t;

/** @brief Minimum value for ShortAddress attribute */
#define ESP_ZB_ZCL_COMMISSIONING_SHORT_ADDRESS_MIN_VALUE ((uint16_t)0x0)

/** @brief Maximum value for ShortAddress attribute */
#define ESP_ZB_ZCL_COMMISSIONING_SHORT_ADDRESS_MAX_VALUE ((uint16_t)0xfff7)

/** @brief Minimum value for ProtocolVersion attribute */
#define ESP_ZB_ZCL_COMMISSIONING_PROTOCOL_VERSION_MIN_VALUE ((uint8_t)0x2)

/** @brief Maximum value for ProtocolVersion attribute */
#define ESP_ZB_ZCL_COMMISSIONING_PROTOCOL_VERSION_MAX_VALUE ((uint8_t)0x2)

/** @brief Minimum value for StackProfile attribute */
#define ESP_ZB_ZCL_COMMISSIONING_STACK_PROFILE_MIN_VALUE ((uint8_t)0x1)

/** @brief Maximum value for StackProfile attribute */
#define ESP_ZB_ZCL_COMMISSIONING_STACK_PROFILE_MAX_VALUE ((uint8_t)0x2)

/** @brief Minimum value for StartupControl attribute */
#define ESP_ZB_ZCL_COMMISSIONING_STARTUP_CONTROL_MIN_VALUE ((uint8_t)0x0)

/** @brief Maximum value for StartupControl attribute */
#define ESP_ZB_ZCL_COMMISSIONING_STARTUP_CONTROL_MAX_VALUE ((uint8_t)0x3)

/** @brief Default value for ExtendedPANId attribute */
#define ESP_ZB_ZCL_COMMISSIONING_EXTENDED_PANID_DEFAULT_VALUE {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

/** @brief Default value for ProtocolVersion attribute */
#define ESP_ZB_ZCL_COMMISSIONING_PROTOCOL_VERSION_DEFAULT_VALUE ((uint8_t)ZB_PROTOCOL_VERSION)

/** @brief Default value for StackProfile attribute */
#define ESP_ZB_ZCL_COMMISSIONING_STACK_PROFILE_DEFAULT_VALUE ((uint8_t)ZB_STACK_PROFILE)

/** @brief Default value for ProtocolVersion attribute */
#define ESP_ZB_ZCL_COMMISSIONING_STARTUP_CONTROL_DEFAULT_VALUE ((uint8_t)ESP_ZB_ZCL_ATTR_COMMISSIONING_STARTUP_TYPE_SCRATCH)

/** @brief Default value for UseInsecureJoin attribute */
#define ESP_ZB_ZCL_COMMISSIONING_USE_INSECURE_JOIN_DEFAULT_VALUE ((bool)0x1)

/** @brief Commissioning Cluster server command identifiers */
typedef enum {
    ESP_ZB_ZCL_CMD_COMMISSIONING_RESTART_DEVICE_RESPONSE_ID               = 0x0000, /**< "Restart Device Response" command. */
    ESP_ZB_ZCL_CMD_COMMISSIONING_SAVE_STARTUP_PARAMETERS_RESPONSE_ID      = 0x0001, /**< "Save Startup Parameters Response" command. */
    ESP_ZB_ZCL_CMD_COMMISSIONING_RESTORE_STARTUP_PARAMETERS_RESPONSE_ID   = 0x0002, /**< "Restore Startup Parameters Response" command. */
    ESP_ZB_ZCL_CMD_COMMISSIONING_RESET_STARTUP_PARAMETERS_RESPONSE_ID     = 0x0003, /**< "Reset Startup Parameters Response" command. */
} esp_zb_zcl_commissioning_srv_cmd_id_t;

/** @brief Commissioning Cluster client command identifiers */
typedef enum {
    ESP_ZB_ZCL_CMD_COMMISSIONING_RESTART_DEVICE_ID                        = 0x0000, /**< "Restart Device" command. */
    ESP_ZB_ZCL_CMD_COMMISSIONING_SAVE_STARTUP_PARAMETERS_ID               = 0x0001, /**< "Save Startup Parameters" command. */
    ESP_ZB_ZCL_CMD_COMMISSIONING_RESTORE_STARTUP_PARAMETERS_ID            = 0x0002, /**< "Restore Startup Parameters" command. */
    ESP_ZB_ZCL_CMD_COMMISSIONING_RESET_STARTUP_PARAMETERS_ID              = 0x0003, /**< "Reset Startup Parameters" command. */
} esp_zb_zcl_commissioning_cli_cmd_id_t;

/**
 * @brief The values of "startup_mode" bitfield in "options" field of "esp_zb_zcl_commissioning_restart_device_payload_t"
 */
typedef enum {
    ESP_ZB_ZCL_COMMISSIONING_STARTUP_MODE_USE_PARAMETER_SET = 0,
    ESP_ZB_ZCL_COMMISSIONING_STARTUP_MODE_USE_CURRENT_STACK = 1,
} esp_zb_zcl_commissioning_startup_mode_t;

/**
 * @brief "options" field of "esp_zb_zcl_commissioning_restart_device_payload_t"
 */
typedef union {
    uint8_t u8;                      /**< Restart device options */
    struct {
        uint8_t startup_mode   : 3;  /**< startup mode: bit 0 - 2 */
        uint8_t immediate      : 1;  /**< immediate: bit 3 */
        uint8_t /* reserved */ : 4;  /**< reserved: bit 4 - 7 */
    };
} esp_zb_zcl_commissioning_restart_device_options_t;

/**
 * @brief "Restart Device Response" Command Payload Format
 */
typedef struct esp_zb_zcl_commissioning_restart_device_payload_s {
    esp_zb_zcl_commissioning_restart_device_options_t options;  /*!< Options for restart device */
    uint8_t delay;                                              /*!< Startup procedure is to be invoked, in seconds */
    uint8_t jitter;                                             /*!< Specifies a random jitter range, in millisecond */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_commissioning_restart_device_payload_t;

/**
 * @brief ESP_ZB_ZCL_CMD_COMMISSIONING_SAVE_STARTUP_PARAMETERS_ID "Save Startup Parameters" Command Payload Format
 */
typedef struct esp_zb_zcl_commissioning_save_startup_parameters_payload_s {
    uint8_t options; /*!< Reserved field */
    uint8_t index;   /*!< The current startup parameter attribute set is to be saved */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_commissioning_save_startup_parameters_payload_t;

/**
 * @brief ESP_ZB_ZCL_CMD_COMMISSIONING_RESTORE_STARTUP_PARAMETERS_ID "Restore Startup Parameters" Command Payload Format
 */
typedef struct esp_zb_zcl_commissioning_restore_startup_parameters_payload_s {
    uint8_t options; /*!< Reserved field */
    uint8_t index;   /*!< Index of the saved startup parameter attribute set to be restored to current status */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_commissioning_restore_startup_parameters_payload_t;

/**
 * @brief "options" field of "esp_zb_zcl_commissioning_reset_startup_parameters_payload_t"
 */
typedef union {
    uint8_t u8;                     /**< Reset startup options */
    struct {
        uint8_t reset_current   : 1; /**< reset current: bit 0 */
        uint8_t reset_all       : 1; /**< reset all: bit 1 */
        uint8_t erase_index     : 1; /**< erase index: bit 2 */
        uint8_t /* reserved */  : 5; /**< reserved: bit 3 - 7 */
    };
} esp_zb_zcl_commissioning_reset_startup_param_options_t;

/**
 * @brief "Reset Startup Parameters" Command Payload Format
 */
typedef struct esp_zb_zcl_commissioning_reset_startup_parameters_payload_s {
    esp_zb_zcl_commissioning_reset_startup_param_options_t options; /*!< The options of reset startup parameter */
    uint8_t index;                                                  /*!< Index of a saved startup parameter attribute set */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_commissioning_reset_startup_parameters_payload_t;

void esp_zb_zcl_commissioning_init_server(void);
void esp_zb_zcl_commissioning_init_client(void);
#define ESP_ZB_ZCL_CLUSTER_ID_COMMISSIONING_SERVER_ROLE_INIT esp_zb_zcl_commissioning_init_server
#define ESP_ZB_ZCL_CLUSTER_ID_COMMISSIONING_CLIENT_ROLE_INIT esp_zb_zcl_commissioning_init_client

#ifdef __cplusplus
}
#endif
