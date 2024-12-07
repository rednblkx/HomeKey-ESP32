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

/** @brief Diagnostics cluster attribute identifiers */
typedef enum {
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID                = 0x0000, /*!< Number Of Resets */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_PERSISTENT_MEMORY_WRITES_ID        = 0x0001, /*!< Persistent Memory Writes */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_BCAST_ID                    = 0x0100, /*!< MAC Rx Broadcast */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_BCAST_ID                    = 0x0101, /*!< MAC Tx Broadcast */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_UCAST_ID                    = 0x0102, /*!< MAC Rx Unicast */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_ID                    = 0x0103, /*!< MAC Tx Unicast */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_RETRY_ID              = 0x0104, /*!< MAC Tx Unicast Retry */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_FAIL_ID               = 0x0105, /*!< MAC Tx Unicast Fail */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APS_RX_BCAST_ID                    = 0x0106, /*!< APS Rx Broadcast */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_BCAST_ID                    = 0x0107, /*!< APS Tx Broadcast */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APS_RX_UCAST_ID                    = 0x0108, /*!< APS Rx Unicast */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_SUCCESS_ID            = 0x0109, /*!< APS Tx Unicast Success */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_RETRY_ID              = 0x010A, /*!< APS Tx Unicast Retry */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_FAIL_ID               = 0x010b, /*!< APS Tx Unicast Fail */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_ROUTE_DISC_INITIATED_ID            = 0x010C, /*!< Route Disc Initiated */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_ADDED_ID                  = 0x010D, /*!< Neighbor Added */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_REMOVED_ID                = 0x010E, /*!< Neighbor Removed */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_STALE_ID                  = 0x010F, /*!< Neighbor Stale */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_JOIN_INDICATION_ID                 = 0x0110, /*!< Join Indication */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_CHILD_MOVED_ID                     = 0x0111, /*!< Child Moved */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_NWKFC_FAILURE_ID                   = 0x0112, /*!< NWKFC Failure */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APSFC_FAILURE_ID                   = 0x0113, /*!< APSFC Failure */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APS_UNAUTHORIZED_KEY_ID            = 0x0114, /*!< APS Unauthorized Key */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_NWK_DECRYPT_FAILURES_ID            = 0x0115, /*!< NWK Decrypt Failures */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_APS_DECRYPT_FAILURES_ID            = 0x0116, /*!< APS Decrypt Failures */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_BUFFER_ALLOCATE_FAILURES_ID = 0x0117, /*!< Packet Buffer Allocate Failures */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_RELAYED_UCAST_ID                   = 0x0118, /*!< Relayed Unicast */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_PHYTOMACQUEUELIMITREACHED_ID       = 0x0119, /*!< Phytomacqueuelimitreached */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_VALIDATEDROPCOUNT_ID        = 0x011A, /*!< Packet Validatedropcount */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_AVERAGE_MAC_RETRY_PER_APS_ID       = 0x011b, /*!< Average MAC Retry Per APS */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID                        = 0x011c, /*!< Last LQI */
    ESP_ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID                       = 0x011d, /*!< Last RSSI */
} esp_zb_zcl_diagnostics_attr_t;

/** @brief Default value for Diagnostics cluster revision global attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_CLUSTER_REVISION_DEFAULT ((uint16_t)0x0003u)

/** @brief Default value for number_of_resets attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_NUMBER_OF_RESETS_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for PersistentMemoryWrites attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_PERSISTENT_MEMORY_WRITES_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for MacRxBcast attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_MAC_RX_BCAST_DEFAULT_VALUE ((uint32_t)0)

/** @brief Default value for MacTxBcast attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_MAC_TX_BCAST_DEFAULT_VALUE ((uint32_t)0)

/** @brief Default value for MacRxUcast attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_MAC_RX_UCAST_DEFAULT_VALUE ((uint32_t)0)

/** @brief Default value for MacTxUcast attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_MAC_TX_UCAST_DEFAULT_VALUE ((uint32_t)0)

/** @brief Default value for MacTxUcastRetry attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_MAC_TX_UCAST_RETRY_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for MacTxUcastFail attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_MAC_TX_UCAST_FAIL_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for APSRxBcast attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APS_RX_BCAST_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for aps_tx_bcast attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APS_TX_BCAST_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for APSRxUcast attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APS_RX_UCAST_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for aps_tx_ucast_success attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APS_TX_UCAST_SUCCESS_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for APSTxUcastRetry attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APS_TX_UCAST_RETRY_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for aps_tx_ucast_fail attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APS_TX_UCAST_FAIL_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for RouteDiscInitiated attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_ROUTE_DISC_INITIATED_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for NeighborAdded attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_NEIGHBOR_ADDED_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for NeighborRemoved attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_NEIGHBOR_REMOVED_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for NeighborStale attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_NEIGHBOR_STALE_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for join_indication attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_JOIN_INDICATION_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for ChildMoved attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_CHILD_MOVED_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for nwk_fc_failure attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_NWKFC_FAILURE_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for aps_fc_failure attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APSFC_FAILURE_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for APSUnauthorizedKey attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APS_UNAUTHORIZED_KEY_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for NWKDecryptFailures attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_NWK_DECRYPT_FAILURES_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for APSDecryptFailures attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_APS_DECRYPT_FAILURES_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for packet_buffer_allocate_failures attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_PACKET_BUFFER_ALLOCATE_FAILURES_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for RelayedUcast attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_RELAYED_UCAST_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for PhytoMACqueuelimitreached attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_PHYTO_MA_CQUEUELIMITREACHED_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for PacketValidatedropcount attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_PACKET_VALIDATEDROPCOUNT_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for AverageMACRetryPerAPS attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_AVERAGE_MAC_RETRY_PER_APS_DEFAULT_VALUE ((uint16_t)0)

/** @brief Default value for LastLQI attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_LAST_LQI_DEFAULT_VALUE ((uint8_t)0)

/** @brief Default value for LastRSSI attribute */
#define ESP_ZB_ZCL_DIAGNOSTICS_LAST_RSSI_DEFAULT_VALUE ((int8_t)0)

#ifdef __cplusplus
}
#endif
