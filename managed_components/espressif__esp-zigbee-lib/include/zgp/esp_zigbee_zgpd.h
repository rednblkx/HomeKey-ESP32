/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "stdio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ZGP device application signal
 */
typedef enum esp_zb_zgpd_signal_e {
    ESP_ZB_ZGPD_SIGNAL_DEVICE_STARTED_UP, /*!< During the startup process */
    ESP_ZB_ZGPD_SIGNAL_COMMISSIONING,     /*!< During the commissioning process */
} esp_zb_zgpd_signal_t;

/**
 * @brief Supported command identifier for ZGP device
 *
 * @note 1. The recall scene 8-11 are got from LCGW. But GPPB specification does not define recall scenes 8-11. Only 8 scenes are supported.
 *       2. 0x39 - 0x3F, 0x4C - 0x4F, 0x4C - 0x4F, 0x52 - 0x5F, 0x6b-0x6f, 0x70-0x9f, 0xA7 0xA9 - 0xAE, 0xC0 - 0xDF, 0xE5 - 0xEF Reserved
 *       3. Manufacturer-defined GPD commands (payload is manufacturer-specific)
 *       4. 0xB1 - 0xBE: Manufacturer-defined GPD commands (payload is manufacturer-specific)
 *       5. 0xF4 - 0xF5: Reserved for other commands sent to the GPD
 *       6. 0xF7 - 0xFF: Reserved for other commands sent to the GPD
 */
typedef enum esp_zb_zgpd_cmd_id_e {
    ESP_ZB_GPDF_CMD_IDENTIFY                             = 0x00, /*!< Identify */
    ESP_ZB_GPDF_CMD_RECALL_SCENE0                        = 0x10, /*!< Recall scene */
    ESP_ZB_GPDF_CMD_RECALL_SCENE1                        = 0x11,
    ESP_ZB_GPDF_CMD_RECALL_SCENE2                        = 0x12,
    ESP_ZB_GPDF_CMD_RECALL_SCENE3                        = 0x13,
    ESP_ZB_GPDF_CMD_RECALL_SCENE4                        = 0x14,
    ESP_ZB_GPDF_CMD_RECALL_SCENE5                        = 0x15,
    ESP_ZB_GPDF_CMD_RECALL_SCENE6                        = 0x16,
    ESP_ZB_GPDF_CMD_RECALL_SCENE7                        = 0x17,
    ESP_ZB_GPDF_CMD_RECALL_SCENE8                        = 0x18,
    ESP_ZB_GPDF_CMD_RECALL_SCENE9                        = 0x19,
    ESP_ZB_GPDF_CMD_RECALL_SCENE10                       = 0x1A,
    ESP_ZB_GPDF_CMD_RECALL_SCENE11                       = 0x1B,
    ESP_ZB_GPDF_CMD_STORE_SCENE4                         = 0x1C, /*!< Store Scene */
    ESP_ZB_GPDF_CMD_STORE_SCENE5                         = 0x1D,
    ESP_ZB_GPDF_CMD_STORE_SCENE6                         = 0x1E,
    ESP_ZB_GPDF_CMD_STORE_SCENE7                         = 0x1F,
    ESP_ZB_GPDF_CMD_OFF                                  = 0x20, /*!< On/Off */
    ESP_ZB_GPDF_CMD_ON                                   = 0x21,
    ESP_ZB_GPDF_CMD_TOGGLE                               = 0x22,
    ESP_ZB_GPDF_CMD_RELEASE                              = 0x23,
    ESP_ZB_GPDF_CMD_MOVE_UP                              = 0x30, /* Level control */
    ESP_ZB_GPDF_CMD_MOVE_DOWN                            = 0x31,
    ESP_ZB_GPDF_CMD_STEP_UP                              = 0x32,
    ESP_ZB_GPDF_CMD_STEP_DOWN                            = 0x33,
    ESP_ZB_GPDF_CMD_LC_STOP                              = 0x34,
    ESP_ZB_GPDF_CMD_MOVE_UP_W_ONOFF                      = 0x35,
    ESP_ZB_GPDF_CMD_MOVE_DOWN_W_ONOFF                    = 0x36,
    ESP_ZB_GPDF_CMD_STEP_UP_W_ONOFF                      = 0x37,
    ESP_ZB_GPDF_CMD_STEP_DOWN_W_ONOFF                    = 0x38,
    ESP_ZB_GPDF_CMD_MOVE_HUE_STOP                        = 0x40, /* Color Control */
    ESP_ZB_GPDF_CMD_MOVE_HUE_UP                          = 0x41,
    ESP_ZB_GPDF_CMD_MOVE_HUE_DOWN                        = 0x42,
    ESP_ZB_GPDF_CMD_STEP_HUE_UP                          = 0x43,
    ESP_ZB_GPDF_CMD_STEP_HUE_DOWN                        = 0x44,
    ESP_ZB_GPDF_CMD_MOVE_SATURATION_STOP                 = 0x45,
    ESP_ZB_GPDF_CMD_MOVE_SATURATION_UP                   = 0x46,
    ESP_ZB_GPDF_CMD_MOVE_SATURATION_DOWN                 = 0x47,
    ESP_ZB_GPDF_CMD_STEP_SATURATION_UP                   = 0x48,
    ESP_ZB_GPDF_CMD_STEP_SATURATION_DOWN                 = 0x49,
    ESP_ZB_GPDF_CMD_MOVE_COLOR                           = 0x4A,
    ESP_ZB_GPDF_CMD_STEP_COLOR                           = 0x4B,
    ESP_ZB_GPDF_CMD_LOCK_DOOR                            = 0x50, /* Door Lock */
    ESP_ZB_GPDF_CMD_UNLOCK_DOOR                          = 0x51,
    ESP_ZB_GPDF_CMD_PRESS_1_OF_1                         = 0x60, /* Simple Generic Switch */
    ESP_ZB_GPDF_CMD_RELEASE_1_OF_1                       = 0x61,
    ESP_ZB_GPDF_CMD_PRESS_1_OF_2                         = 0x62,
    ESP_ZB_GPDF_CMD_RELEASE_1_OF_2                       = 0x63,
    ESP_ZB_GPDF_CMD_PRESS_2_OF_2                         = 0x64,
    ESP_ZB_GPDF_CMD_RELEASE_2_OF_2                       = 0x65,
    ESP_ZB_GPDF_CMD_SHORT_PRESS_1_OF_1                   = 0x66,
    ESP_ZB_GPDF_CMD_SHORT_PRESS_1_OF_2                   = 0x67,
    ESP_ZB_GPDF_CMD_SHORT_PRESS_2_OF_2                   = 0x68,
    ESP_ZB_GPDF_CMD_8BIT_VECTOR_PRESS                    = 0x69,
    ESP_ZB_GPDF_CMD_8BIT_VECTOR_RELEASE                  = 0x6A,
    ESP_ZB_GPDF_CMD_ATTR_REPORT                          = 0xA0,
    ESP_ZB_GPDF_CMD_MANUF_SPEC_ATTR_REPORT               = 0xA1,
    ESP_ZB_GPDF_CMD_MULTI_CLUSTER_ATTR_REPORT            = 0xA2,
    ESP_ZB_GPDF_CMD_MANUF_SPEC_MULTI_CLUSTER_ATTR_REPORT = 0xA3,
    ESP_ZB_GPDF_CMD_REQUEST_ATTRIBUTES                   = 0xA4,
    ESP_ZB_GPDF_CMD_READ_ATTR_RESP                       = 0xA5,
    ESP_ZB_GPDF_CMD_ZCL_TUNNELING_FROM_ZGPD              = 0xA6,
    ESP_ZB_GPDF_CMD_COMPACT_ATTR_REPORTING               = 0xA8,
    ESP_ZB_GPDF_CMD_ATTR_REPORT_ANY                      = 0xAF,
    ESP_ZB_GPDF_CMD_MANUF_DEFINED_B0                     = 0xB0,
    ESP_ZB_GPDF_CMD_MANUF_DEFINED_BF                     = 0xBF,
    ESP_ZB_GPDF_CMD_COMMISSIONING                        = 0xE0, /* commissioning from ZGPD */
    ESP_ZB_GPDF_CMD_DECOMMISSIONING                      = 0xE1,
    ESP_ZB_GPDF_CMD_SUCCESS                              = 0xE2,
    ESP_ZB_GPDF_CMD_CHANNEL_REQUEST                      = 0xE3,
    ESP_ZB_GPDF_CMD_APPLICATION_DESCR                    = 0xE4,
    ESP_ZB_GPDF_CMD_COMMISSIONING_REPLY                  = 0xF0, /* GPDF commands sent to GPD */
    ESP_ZB_GPDF_CMD_WRITE_ATTRIBUTES                     = 0xF1,
    ESP_ZB_GPDF_CMD_READ_ATTRIBUTES                      = 0xF2,
    ESP_ZB_GPDF_CMD_CHANNEL_CONFIGURATION                = 0xF3,
    ESP_ZB_GPDF_CMD_ZCL_TUNNELING_TO_ZGPD                = 0xF6,
} esp_zb_zgpd_cmd_id_t;

/**
 * @brief ZGP device commissioning signal callback
 */
typedef void (* esp_zb_zgpd_commissioning_signal_callback_t)(esp_zb_zgpd_signal_t, esp_err_t);

/**
 * @brief ZGP device stack initialization
 *
 * @param cb A callback for indicating the commissioning process for ZGP device
 */
void esp_zb_zgpd_stack_init(esp_zb_zgpd_commissioning_signal_callback_t cb);

/**
 * @brief ZGP device commissioning information initialization
 *
 * @param[in] zgp_app_id                App identifier, refer to esp_zb_zgp_app_id_t
 * @param[in] zgpd_commissioning_method Commissioning method, refer to esp_zb_zgpd_commissioning_method_t
 * @param[in] zgpd_device_id            Device identifier, refer to esp_zb_zgpd_dev_id_t
 */
void esp_zb_zgpd_init(uint8_t zgp_app_id, uint8_t zgpd_commissioning_method, uint8_t zgpd_device_id);

/**
 * @brief ZGP device start
 *
 * @return
 *      - ESP_OK: on success
 *      - ESP_FAIL: on failed
 */
esp_err_t esp_zb_zgpd_dev_start(void);

/**
 * @brief ZGP device main loop
 *
 */
void esp_zb_zgpd_main_loop(void);

/**
 * @brief ZGP device start commissioning
 *
 */
void esp_zb_zgpd_start_commissioning(void);

/**
 * @brief Set the source identifier for ZGP device
 *
 * @param src_id A 32-bit source identifier
 */
void esp_zb_zgpd_set_source_id(uint32_t src_id);

/**
 * @brief Set the security information for ZGP device
 *
 * @param[in] level Security level
 * @param[in] type  Security type
 * @param[in] key   Security key
 */
void esp_zb_zgpd_set_security(uint8_t level, uint8_t type, uint8_t *key);

/**
 * @brief Set the security out-of-the-box key for ZGP device
 *
 * @param[in] key The OOB key
 */
void esp_zb_zgpd_set_OOB_key(uint8_t *key);

/**
 * @brief Set the channel for ZGP device
 *
 * @param[in] channel The channel will be set for device
 */
void esp_zb_zgpd_set_channel(uint8_t  channel);

/**
 * @brief Set the security level for ZGP device
 *
 * @param[in] level Security level
 */
void esp_zb_zgpd_set_security_level(uint8_t level);

/**
 * @brief Set the type key of security for ZGP device
 *
 * @param[in] type Security type
 */
void esp_zb_zgpd_set_security_key_type(uint8_t type);

/**
 * @brief Set the frame counter for ZGP device security
 *
 * @param[in] counter The value of frame counter
 */
void esp_zb_zgpd_set_security_frame_counter(uint16_t counter);

/**
 * @brief Present the application information setting
 *
 * @param[in] presented The flag indicates whether to present the application information.
 */
void esp_zb_zgpd_set_application_info_present(uint8_t presented);

/**
 * @brief ZGP device application information presented command setting
 *
 * @param[in] presented Whether present the application information of ZGP device
 * @param[in] cmd       The application information command, note that the first byte indicates the length of command.
 */
void esp_zb_zgpd_config_application_info_cmd(uint8_t presented, uint8_t* cmd);

/**
 * @brief ZGP device send command
 *
 * @param[in] cmd_id The command identifier will be sent by the ZGP device
 */
void esp_zb_zgpd_send_command(esp_zb_zgpd_cmd_id_t cmd_id);

#ifdef __cplusplus
}
#endif
