/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_zigbee_type.h"
#include "zgp/esp_zigbee_zgp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The enumeration for attributes of the Zigbee Green Power cluster
 *
 */
typedef enum esp_zgp_gp_attr_e {
    ESP_ZB_ZCL_ATTR_GPS_MAX_SINK_TABLE_ENTRIES_ID       = 0x0000, /*!< Maximum number of Sink Table entries supported by this device */
    ESP_ZB_ZCL_ATTR_GPS_SINK_TABLE_ID                   = 0x0001, /*!< Sink Table, holding information about local bindings between a particular GPD and target‘s local endpoints */
    ESP_ZB_ZCL_ATTR_GPS_COMMUNICATION_MODE_ID           = 0x0002, /*!< Default communication mode requested by this GPS */
    ESP_ZB_ZCL_ATTR_GPS_COMMISSIONING_EXIT_MODE_ID      = 0x0003, /*!< Conditions for the GPS to exit the commissioning mode */
    ESP_ZB_ZCL_ATTR_GPS_COMMISSIONING_WINDOW_ID         = 0x0004, /*!< Default duration of the Commissioning window duration, in seconds, as re- quested by this GPS */
    ESP_ZB_ZCL_ATTR_GPS_SECURITY_LEVEL_ID               = 0x0005, /*!< The minimum required security level to be supported by the paired GPDs*/
    ESP_ZB_ZCL_ATTR_GPS_FUNCTIONALITY_ID                = 0x0006, /*!< The optional GP functionality supported by this GPS */
    ESP_ZB_ZCL_ATTR_GPS_ACTIVE_FUNCTIONALITY_ID         = 0x0007, /*!< The optional GP functionality supported by this GPS that is active */
    ESP_ZB_ZCL_ATTR_GPP_MAX_PROXY_TABLE_ENTRIES_ID      = 0x0010, /*!< Maximum number of Proxy Table entries supported by this device */
    ESP_ZB_ZCL_ATTR_GPP_PROXY_TABLE_ID                  = 0x0011, /*!< Proxy Table, holding information about pairings between a particular GPD ID and GPSs in the network*/
    ESP_ZB_ZCL_ATTR_GPP_NOTIFICATION_RETRY_NUMBER_ID    = 0x0012, /*!< Number of unicast GP Notification retries on lack of GP Notification Response */
    ESP_ZB_ZCL_ATTR_GPP_NOTIFICATION_RETRY_TIMER_ID     = 0x0013, /*!< Time in ms between unicast GP Notification retries on lack of GP Notification Response */
    ESP_ZB_ZCL_ATTR_GPP_MAX_SEARCH_COUNTER_ID           = 0x0014, /*!< The frequency of sink re-discovery for inactive Proxy Table entries */
    ESP_ZB_ZCL_ATTR_GPP_BLOCKED_GPDID_ID                = 0x0015, /*!< A list holding information about blocked GPD IDs*/
    ESP_ZB_ZCL_ATTR_GPP_FUNCTIONALITY_ID                = 0x0016, /*!< The optional GP functionality supported by this GPP */
    ESP_ZB_ZCL_ATTR_GPP_ACTIVE_FUNCTIONALITY_ID         = 0x0017, /*!< The optional GP functionality supported by this GPP that is active */
    ESP_ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_TYPE_ID      = 0x0020, /*!< The security key type to be used for the communication with all paired 0b11 GPD in this network */
    ESP_ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_ID           = 0x0021, /*!< The security key to be used for the communication with all paired GPD in this network */
    ESP_ZB_ZCL_ATTR_GP_LINK_KEY_ID                      = 0x0022  /*!< The security key to be used to encrypt the key exchanged with the GPD */
} esp_zgp_gp_attr_t;

/**
 * @brief Green power server command
 *
 */
typedef enum esp_zgp_server_commands_e {
  ESP_ZGP_SERVER_CMD_GP_NOTIFICATION                        = 0x00,
  ESP_ZGP_SERVER_CMD_GP_PAIRING_SEARCH                      = 0x01,
  ESP_ZGP_SERVER_CMD_GP_TUNNELING_STOP                      = 0x03,
  ESP_ZGP_SERVER_CMD_GP_COMMISSIONING_NOTIFICATION          = 0x04,
  ESP_ZGP_SERVER_CMD_GP_SINK_COMMISSIONING_MODE             = 0x05,
  ESP_ZGP_SERVER_CMD_GP_TRANSLATION_TABLE_UPDATE_COMMAND    = 0x07,
  ESP_ZGP_SERVER_CMD_GP_TRANSLATION_TABLE_REQUEST           = 0x08,
  ESP_ZGP_SERVER_CMD_GP_PAIRING_CONFIGURATION               = 0x09,
  ESP_ZGP_SERVER_CMD_GP_SINK_TABLE_REQUEST                  = 0x0a,
  ESP_ZGP_SERVER_CMD_GP_PROXY_TABLE_RESPONSE                = 0x0b
} esp_zgp_server_commands_t;

/**
 * @brief Green power client command
 *
 */
typedef enum esp_zgp_client_commands_e {
  ESP_ZGP_CLIENT_CMD_GP_NOTIFICATION_RESPONSE     = 0x00,
  ESP_ZGP_CLIENT_CMD_GP_PAIRING                   = 0x01,
  ESP_ZGP_CLIENT_CMD_GP_PROXY_COMMISSIONING_MODE  = 0x02,
  ESP_ZGP_CLIENT_CMD_GP_RESPONSE                  = 0x06,
  ESP_ZGP_CLIENT_CMD_GP_SINK_TABLE_RESPONSE       = 0x0a,
  ESP_ZGP_CLIENT_CMD_GP_PROXY_TABLE_REQUEST       = 0x0b
} esp_zgp_client_commands_t;

/**
 * @brief The Action sub-field of the zgp pairing configuration
 *
 */
typedef enum esp_zgp_pairing_config_action_e {
    ESP_ZGP_PAIRING_CONFIG_NO_ACTION                = 0xb000,   /*!< No action */
    ESP_ZGP_PAIRING_CONFIG_EXTEND_SINK_TABLE_ENTRY  = 0xb001,   /*!< Extend Sink Table entry */
    ESP_ZGP_PAIRING_CONFIG_REPLACE_SINK_TABLE_ENTRY = 0xb010,   /*!< Replace Sink Table entry */
    ESP_ZGP_PAIRING_CONFIG_REMOVE_A_PAIRING         = 0xb011,   /*!< Remove a pairing */
    ESP_ZGP_PAIRING_CONFIG_REMOVE_GPD               = 0xb100,   /*!< Remove GPD */
} esp_zgp_pairing_config_action_t;

#ifdef __cplusplus
}
#endif
