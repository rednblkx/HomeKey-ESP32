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

/**
 * @brief BDB commissioning status
 *
 */
typedef enum esp_zb_bdb_commissioning_status_s
{
    ESP_ZB_BDB_STATUS_SUCCESS = 0,                 /*!< The commissioning sub-procedure was successful. */
    ESP_ZB_BDB_STATUS_IN_PROGRESS,                 /*!< One of the commissioning sub-procedures has started but is not yet complete. */
    ESP_ZB_BDB_STATUS_NOT_AA_CAPABLE,              /*!< The initiator is not address assignment capable during touchlink. */
    ESP_ZB_BDB_STATUS_NO_NETWORK,                  /*!< A network has not been found during network steering or touchlink. */
    ESP_ZB_BDB_STATUS_TARGET_FAILURE,              /*!< A node has not joined a network when requested during touchlink. */
    ESP_ZB_BDB_STATUS_FORMATION_FAILURE,           /*!< A network could not be formed during network formation. */
    ESP_ZB_BDB_STATUS_NO_IDENTIFY_QUERY_RESPONSE,  /*!< No response to an identify query command has been received during finding and binding. */
    ESP_ZB_BDB_STATUS_BINDING_TABLE_FULL,          /*!< A binding table entry could not be created due to insufficient space in the binding table during finding and binding. */
    ESP_ZB_BDB_STATUS_NO_SCAN_RESPONSE,            /*!< No response to a scan request inter-PAN command has been received during touchlink. */
    ESP_ZB_BDB_STATUS_NOT_PERMITTED,               /*!< A touchlink (steal) attempt was made when a node is already connected to a centralized security network.
                                                     A node was instructed to form a network when it did not have a logical type of either Zigbee coordinator or Zigbee router. */
    ESP_ZB_BDB_STATUS_TCLK_EX_FAILURE,             /*!< The Trust Center link key exchange procedure has failed attempting to join a centralized security network. */
    ESP_ZB_BDB_STATUS_NOT_ON_A_NETWORK,            /*!< A commissioning procedure was forbidden since the node was not currently on a network. */
    ESP_ZB_BDB_STATUS_ON_A_NETWORK,                /*!< A commissioning procedure was forbidden since the node was currently on a network. */
    ESP_ZB_BDB_STATUS_CANCELLED,                   /*!< The current operation (steering or formation) was cancelled by an app */
    ESP_ZB_BDB_STATUS_DEV_ANNCE_SEND_FAILURE       /*!< A device announce sending has been failed (e.g. device announce haven't acked by parent router). */
} esp_zb_bdb_commissioning_status_t;

#ifdef __cplusplus
}
#endif
