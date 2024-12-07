/*
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA
 * Copyright (c) 2021-2022 Espressif Systems (Shanghai) PTE LTD
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Espressif Systems
 *    integrated circuit in a product or a software update for such product,
 *    must reproduce the above copyright notice, this list of conditions and
 *    the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/* PURPOSE: Diagnostics cluster definitions
*/

#ifndef ZB_ZCL_DIAGNOSTICS_H
#define ZB_ZCL_DIAGNOSTICS_H 1


#if defined(ZB_ZCL_SUPPORT_CLUSTER_DIAGNOSTICS) || defined(DOXYGEN)

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_DIAGNOSTICS
 *  @{
 *    @details
 *    ZCL Diagnostics cluster definitions
 */

/* Cluster ZB_ZCL_CLUSTER_ID_DIAGNOSTICS */

/*! @name Diagnostics cluster attributes
    @{
*/

typedef ZB_PACKED_PRE struct zb_zcl_diagnostics_ctx_s
{
  zb_mac_diagnostic_info_t mac_data;
  zdo_diagnostics_info_t zdo_data;
  zb_callback_t sync_data_cb;
  zb_uint8_t cb_param;
}
ZB_PACKED_STRUCT zb_zcl_diagnostics_ctx_t;

extern zb_zcl_diagnostics_ctx_t diagnostics_ctx_zcl;

/*! @brief Diagnostics cluster attribute identifiers
    @see HA spec, Diagnostics Cluster 9.3.2.2.2
*/
enum zb_zcl_diagnostics_attr_e
{
  /** @brief number_of_resets, Zigbee Diagnostic Cluster spec 1.2.2.1.1 */
  ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID                = 0x0000,
  /** This attribute keeps track of the number of writes to persistent memory.
   *  HA spec 9.2.2.2.1.2 */
  ZB_ZCL_ATTR_DIAGNOSTICS_PERSISTENT_MEMORY_WRITES_ID        = 0x0001,
  /** @brief MacRxBcast, HA spec 9.2.2.2.2.1 */
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_BCAST_ID                    = 0x0100,
  /** @brief MacTxBcast, HA spec 9.2.2.2.2.2 */
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_BCAST_ID                    = 0x0101,
  /** MacRxUcast Attribute A counter that is incremented each time the MAC
   *  layer receives a unicast. */
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_UCAST_ID                    = 0x0102,
  /** @brief MacTxUcast, HA spec 9.2.2.2.2.4 */
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_ID                    = 0x0103,
  /** MacTxUcastRetry Attribute A counter that is incremented each time
   *  the MAC layer retries a unicast. */
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_RETRY_ID              = 0x0104,
  /** MacTxUcastFail Attribute A counter that is incremented each time
   *  the MAC layer fails to send a unicast. */
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_FAIL_ID               = 0x0105,
  /** APSRxBcast Attribute A counter that is incremented each time
   *  the APS layer receives a broadcast. */
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_RX_BCAST_ID                    = 0x0106,
  /** @brief aps_tx_bcast, HA spec 9.3.2.2.2 (??) */
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_BCAST_ID                    = 0x0107,
  /** APSRxUcast Attribute A counter that is incremented each time
   *  the APS layer receives a unicast. */
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_RX_UCAST_ID                    = 0x0108,
  /** @brief aps_tx_ucast_success, HA spec 9.3.2.2.2 (??) */
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_SUCCESS_ID            = 0x0109,
  /** APSTxUcastRetry Attribute A counter that is incremented each time
   *  the APS layer retries the sending of a unicast. */
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_RETRY_ID              = 0x010A,
  /** @brief aps_tx_ucast_fail, HA spec 9.3.2.2.2 (??) */
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_FAIL_ID               = 0x010b,
  /** RouteDiscInitiated Attribute A counter that is incremented each time
   *  a route request is initiated . */
  ZB_ZCL_ATTR_DIAGNOSTICS_ROUTE_DISC_INITIATED_ID            = 0x010C,
  /** NeighborAdded Attribute A counter that is incremented each time
   *  an entry is added to the neighbor table. */
  ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_ADDED_ID                  = 0x010D,
  /** NeighborRemoved Attribute A counter that is incremented each time
   *  an entry is removed from the neighbor table. */
  ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_REMOVED_ID                = 0x010E,
  /** A counter that is incremented each time a neighbor table entry becomes stale
   *  because the neighbor has not been heard from. */
  ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_STALE_ID                  = 0x010F,
  /** @brief join_indication, HA spec 1.2.2.2.17 */
  ZB_ZCL_ATTR_DIAGNOSTICS_JOIN_INDICATION_ID                 = 0x0110,
  /** A counter that is incremented each time an entry is removed from the child table. */
  ZB_ZCL_ATTR_DIAGNOSTICS_CHILD_MOVED_ID                     = 0x0111,
  /** A counter that is incremented each time a message is dropped at the network
   *  layer because the APS frame counter was not higher than the last message seen
   *  from that source. */
  ZB_ZCL_ATTR_DIAGNOSTICS_NWKFC_FAILURE_ID                   = 0x0112,
  /** A counter that is incremented each time a message is dropped at the APS layer
   *  because the APS frame counter was not higher than the last message seen from
   *  that source. */
  ZB_ZCL_ATTR_DIAGNOSTICS_APSFC_FAILURE_ID                   = 0x0113,
  /** A counter that is incremented each time a message is dropped at the APS layer
   *  because it had APS encryption but the key associated with the sender has
   *  not been authenticated, and thus the key is not authorized for use
   *  in APS data messages. */
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_UNAUTHORIZED_KEY_ID            = 0x0114,
  /** A counter that is incremented each time a NWK encrypted message was received
   *  but dropped because decryption failed. */
  ZB_ZCL_ATTR_DIAGNOSTICS_NWK_DECRYPT_FAILURES_ID            = 0x0115,
  /** A counter that is incremented each time an APS encrypted message was received
   *  but dropped because decryption failed. */
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_DECRYPT_FAILURES_ID            = 0x0116,
  /** A counter that is incremented each time the stack failed to allocate a packet
   *  buffers. This doesn't necessarily mean that the packet buffer count was 0 at
   *  the time, but that the number requested was greater than the number free. */
  ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_BUFFER_ALLOCATE_FAILURES_ID = 0x0117,
  /** A counter that is incremented each time a unicast packet is relayed. */
  ZB_ZCL_ATTR_DIAGNOSTICS_RELAYED_UCAST_ID                   = 0x0118,
  /** A counter that is incremented each time a packet is dropped because the PHY to
   *  MAC queue was exhausted */
  ZB_ZCL_ATTR_DIAGNOSTICS_PHYTOMACQUEUELIMITREACHED_ID       = 0x0119,
  /** A counter that is incremented each time a packet was dropped due to a packet
   *  validation error. This could be due to length or other formatting problems
   *  in the packet. */
  ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_VALIDATEDROPCOUNT_ID        = 0x011A,
  /** A counter that is equal to the average number of MAC retries needed to send
   *  an APS message, HA spec 9.2.2.2.2.27 */
  ZB_ZCL_ATTR_DIAGNOSTICS_AVERAGE_MAC_RETRY_PER_APS_ID       = 0x011b,
  /** This is the Link Quality Indicator for the last message received. There is no
   *  current agreed upon standard for calculating the LQI. For some implementations
   *  LQI is related directly to RSSI for others it is a function of the number of
   *  errors received over a fixed number of bytes in a given message. The one thing
   *  that has been agreed is that the Link Quality Indicator is a value between 0
   *  and 255 where 0 indicates the worst possible link and 255 indicates the best
   *  possible link. Note that for a device reading the Last Message LQI the returned
   *  value SHALL be the LQI for the read attribute message used to read the attribute
   *  itself. */
  ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID                        = 0x011c,
  /** This is the receive signal strength indication for the last message received.
   *  As with Last Message LQI, a device reading the Last Message RSSI, the returned
   *  value SHALL be the RSSI of the read attribute message used to read the attribute
   *  itself. */
  ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID                       = 0x011d,
  /*! @brief A counter that is incremented on the NWK layer
   *         each time tries number of a packet resending are gone.
   *
   * @note It's a non-standard counter that depends on ZB_ENABLE_NWK_RETRANSMIT and
   *       will be zero always when the macro isn't set. */
  ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_NWK_RETRY_OVERFLOW_ID  = 0xff00,
  /** A non-standard counter that is incremented each time an the PHY layer was unable
   *  to transmit due to a failed CCA */
  ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_PHY_CCA_FAILURES_ID    = 0xff01,
  /** A non-standard counter of the number of times the NWK broadcast was dropped
   *  because the broadcast table was full. */
  ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_BCAST_TABLE_FULL_ID    = 0xff02
};

/** @brief Default value for Diagnostics cluster revision global attribute */
#define ZB_ZCL_DIAGNOSTICS_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/** @brief Default value for number_of_resets attribute */
#define ZB_ZCL_DIAGNOSTICS_NUMBER_OF_RESETS_DEFAULT_VALUE ((zb_uint16_t)0x00000000)

/** @brief Default value for PersistentMemoryWrites attribute */
#define ZB_ZCL_DIAGNOSTICS_PERSISTENT_MEMORY_WRITES_DEFAULT_VALUE ((zb_uint16_t)0x00000000)

/** @brief Default value for MacRxBcast attribute */
#define ZB_ZCL_DIAGNOSTICS_MAC_RX_BCAST_DEFAULT_VALUE ((zb_uint32_t)0)

/** @brief Default value for MacTxBcast attribute */
#define ZB_ZCL_DIAGNOSTICS_MAC_TX_BCAST_DEFAULT_VALUE ((zb_uint32_t)0)

/** @brief Default value for MacRxUcast attribute */
#define ZB_ZCL_DIAGNOSTICS_MAC_RX_UCAST_DEFAULT_VALUE ((zb_uint32_t)0)

/** @brief Default value for MacTxUcast attribute */
#define ZB_ZCL_DIAGNOSTICS_MAC_TX_UCAST_DEFAULT_VALUE ((zb_uint32_t)0)

/** @brief Default value for MacTxUcastRetry attribute */
#define ZB_ZCL_DIAGNOSTICS_MAC_TX_UCAST_RETRY_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for MacTxUcastFail attribute */
#define ZB_ZCL_DIAGNOSTICS_MAC_TX_UCAST_FAIL_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for APSRxBcast attribute */
#define ZB_ZCL_DIAGNOSTICS_APS_RX_BCAST_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for aps_tx_bcast attribute */
#define ZB_ZCL_DIAGNOSTICS_APS_TX_BCAST_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for APSRxUcast attribute */
#define ZB_ZCL_DIAGNOSTICS_APS_RX_UCAST_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for aps_tx_ucast_success attribute */
#define ZB_ZCL_DIAGNOSTICS_APS_TX_UCAST_SUCCESS_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for APSTxUcastRetry attribute */
#define ZB_ZCL_DIAGNOSTICS_APS_TX_UCAST_RETRY_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for aps_tx_ucast_fail attribute */
#define ZB_ZCL_DIAGNOSTICS_APS_TX_UCAST_FAIL_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for RouteDiscInitiated attribute */
#define ZB_ZCL_DIAGNOSTICS_ROUTE_DISC_INITIATED_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for NeighborAdded attribute */
#define ZB_ZCL_DIAGNOSTICS_NEIGHBOR_ADDED_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for NeighborRemoved attribute */
#define ZB_ZCL_DIAGNOSTICS_NEIGHBOR_REMOVED_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for NeighborStale attribute */
#define ZB_ZCL_DIAGNOSTICS_NEIGHBOR_STALE_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for join_indication attribute */
#define ZB_ZCL_DIAGNOSTICS_JOIN_INDICATION_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for ChildMoved attribute */
#define ZB_ZCL_DIAGNOSTICS_CHILD_MOVED_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for nwk_fc_failure attribute */
#define ZB_ZCL_DIAGNOSTICS_NWKFC_FAILURE_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for aps_fc_failure attribute */
#define ZB_ZCL_DIAGNOSTICS_APSFC_FAILURE_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for APSUnauthorizedKey attribute */
#define ZB_ZCL_DIAGNOSTICS_APS_UNAUTHORIZED_KEY_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for NWKDecryptFailures attribute */
#define ZB_ZCL_DIAGNOSTICS_NWK_DECRYPT_FAILURES_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for APSDecryptFailures attribute */
#define ZB_ZCL_DIAGNOSTICS_APS_DECRYPT_FAILURES_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for packet_buffer_allocate_failures attribute */
#define ZB_ZCL_DIAGNOSTICS_PACKET_BUFFER_ALLOCATE_FAILURES_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for RelayedUcast attribute */
#define ZB_ZCL_DIAGNOSTICS_RELAYED_UCAST_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for PhytoMACqueuelimitreached attribute */
#define ZB_ZCL_DIAGNOSTICS_PHYTO_MA_CQUEUELIMITREACHED_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for PacketValidatedropcount attribute */
#define ZB_ZCL_DIAGNOSTICS_PACKET_VALIDATEDROPCOUNT_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for AverageMACRetryPerAPS attribute */
#define ZB_ZCL_DIAGNOSTICS_AVERAGE_MAC_RETRY_PER_APS_DEFAULT_VALUE ((zb_uint16_t)0)

/** @brief Default value for LastLQI attribute */
#define ZB_ZCL_DIAGNOSTICS_LAST_LQI_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for LastRSSI attribute */
#define ZB_ZCL_DIAGNOSTICS_LAST_RSSI_DEFAULT_VALUE ((zb_int8_t)0)

#define ZB_ZCL_DIAGNOSTICS_CUSTOM_ATTR_NWK_RETRY_OVERFLOW_DEFAULT_VALUE ((zb_uint16_t)0)

#define ZB_ZCL_DIAGNOSTICS_CUSTOM_ATTR_PHY_CCA_FAILURES_DEFAULT_VALUE ((zb_uint16_t)0)

#define ZB_ZCL_DIAGNOSTICS_CUSTOM_ATTR_BCAST_TABLE_FULL_DEFAULT_VALUE ((zb_uint16_t)0)

/** @cond internals_doc */
/*! @internal @name Diagnostics cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID,                  \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_BUFFER_ALLOCATE_FAILURES_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_BUFFER_ALLOCATE_FAILURES_ID,   \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_JOIN_INDICATION_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_JOIN_INDICATION_ID,                   \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}


#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_BCAST_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_BCAST_ID,                      \
  ZB_ZCL_ATTR_TYPE_U32,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_BCAST_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_BCAST_ID,                      \
  ZB_ZCL_ATTR_TYPE_U32,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_UCAST_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_BCAST_ID,                      \
  ZB_ZCL_ATTR_TYPE_U32,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_ID,                      \
  ZB_ZCL_ATTR_TYPE_U32,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_RETRY_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_ID,                      \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_FAIL_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_ID,                      \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_BCAST_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_BCAST_ID,                      \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_SUCCESS_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_SUCCESS_ID,              \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_RETRY_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_RETRY_ID,                \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_FAIL_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_FAIL_ID,                 \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_ROUTE_DISC_INITIATED_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_ROUTE_DISC_INITIATED_ID,              \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_AVERAGE_MAC_RETRY_PER_APS_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_AVERAGE_MAC_RETRY_PER_APS_ID,         \
  ZB_ZCL_ATTR_TYPE_U16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID,                          \
  ZB_ZCL_ATTR_TYPE_U8,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID,                         \
  ZB_ZCL_ATTR_TYPE_S8,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_CHILD_MOVED_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_CHILD_MOVED_ID,                             \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_NWKFC_FAILURE_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_NWKFC_FAILURE_ID,                           \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_APSFC_FAILURE_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_APSFC_FAILURE_ID,                           \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_APS_UNAUTHORIZED_KEY_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_UNAUTHORIZED_KEY_ID,                    \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_NWK_DECRYPT_FAILURES_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_NWK_DECRYPT_FAILURES_ID,                    \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_APS_DECRYPT_FAILURES_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_APS_DECRYPT_FAILURES_ID,                    \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_PHYTOMACQUEUELIMITREACHED_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_PHYTOMACQUEUELIMITREACHED_ID,               \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_VALIDATEDROPCOUNT_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_VALIDATEDROPCOUNT_ID,                \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_ADDED_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_ADDED_ID,                          \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_REMOVED_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_REMOVED_ID,                        \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_STALE_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_STALE_ID,                          \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_NWK_RETRY_OVERFLOW_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_NWK_RETRY_OVERFLOW_ID,          \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_PHY_CCA_FAILURES_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_PHY_CCA_FAILURES_ID,            \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_BCAST_TABLE_FULL_ID(data_ptr) \
{                                                                     \
  ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_BCAST_TABLE_FULL_ID,            \
  ZB_ZCL_ATTR_TYPE_U16,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                 \
  (void*) data_ptr                                                    \
}

/** @brief Declare attribute list for Diagnostics cluster - server side
    @param attr_list - attribute list name
*/
#define ZB_ZCL_DECLARE_DIAGNOSTICS_ATTRIB_LIST(attr_list)               \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_DIAGNOSTICS)                           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_BCAST_ID,                    &diagnostics_ctx_zcl.mac_data.mac_rx_bcast) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_BCAST_ID,                    &diagnostics_ctx_zcl.mac_data.mac_tx_bcast) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_MAC_RX_UCAST_ID,                    &diagnostics_ctx_zcl.mac_data.mac_rx_ucast) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_ID,                    &diagnostics_ctx_zcl.mac_data.mac_tx_ucast_total_zcl) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_RETRY_ID,              &diagnostics_ctx_zcl.mac_data.mac_tx_ucast_retries_zcl) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_MAC_TX_UCAST_FAIL_ID,               &diagnostics_ctx_zcl.mac_data.mac_tx_ucast_failures_zcl) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID,                        &diagnostics_ctx_zcl.mac_data.last_msg_lqi) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID,                       &diagnostics_ctx_zcl.mac_data.last_msg_rssi) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_PHYTOMACQUEUELIMITREACHED_ID,       &diagnostics_ctx_zcl.mac_data.phy_to_mac_que_lim_reached) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_VALIDATEDROPCOUNT_ID,        &diagnostics_ctx_zcl.mac_data.mac_validate_drop_cnt) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_PHY_CCA_FAILURES_ID,    &diagnostics_ctx_zcl.mac_data.phy_cca_fail_count) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID,                &diagnostics_ctx_zcl.zdo_data.number_of_resets) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_BCAST_ID,                    &diagnostics_ctx_zcl.zdo_data.aps_tx_bcast) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_SUCCESS_ID,            &diagnostics_ctx_zcl.zdo_data.aps_tx_ucast_success) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_RETRY_ID,              &diagnostics_ctx_zcl.zdo_data.aps_tx_ucast_retry) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_APS_TX_UCAST_FAIL_ID,               &diagnostics_ctx_zcl.zdo_data.aps_tx_ucast_fail) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_ROUTE_DISC_INITIATED_ID,            &diagnostics_ctx_zcl.zdo_data.route_disc_initiated) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_ADDED_ID,                  &diagnostics_ctx_zcl.zdo_data.nwk_neighbor_added) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_REMOVED_ID,                &diagnostics_ctx_zcl.zdo_data.nwk_neighbor_removed) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_NEIGHBOR_STALE_ID,                  &diagnostics_ctx_zcl.zdo_data.nwk_neighbor_stale) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_JOIN_INDICATION_ID,                 &diagnostics_ctx_zcl.zdo_data.join_indication) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_AVERAGE_MAC_RETRY_PER_APS_ID,       &diagnostics_ctx_zcl.zdo_data.average_mac_retry_per_aps_message_sent) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_PACKET_BUFFER_ALLOCATE_FAILURES_ID, &diagnostics_ctx_zcl.zdo_data.packet_buffer_allocate_failures) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_CHILD_MOVED_ID,                     &diagnostics_ctx_zcl.zdo_data.childs_removed) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_NWKFC_FAILURE_ID,                   &diagnostics_ctx_zcl.zdo_data.nwk_fc_failure) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_APSFC_FAILURE_ID,                   &diagnostics_ctx_zcl.zdo_data.aps_fc_failure) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_APS_UNAUTHORIZED_KEY_ID,            &diagnostics_ctx_zcl.zdo_data.aps_unauthorized_key) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_NWK_DECRYPT_FAILURES_ID,            &diagnostics_ctx_zcl.zdo_data.nwk_decrypt_failure) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_APS_DECRYPT_FAILURES_ID,            &diagnostics_ctx_zcl.zdo_data.aps_decrypt_failure) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_NWK_RETRY_OVERFLOW_ID,  &diagnostics_ctx_zcl.zdo_data.nwk_retry_overflow) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DIAGNOSTICS_CUSTOM_ATTR_BCAST_TABLE_FULL_ID,    &diagnostics_ctx_zcl.zdo_data.nwk_bcast_table_full) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Diagnostics cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/* ACHTUNG! ATTENTION!
 *    If you need to add a counter, do the following steps:
 * 1) add a macro to declare an attribute like it's done above:
 *      ZB_SET_ATTR_DESCR_WITH_*attr_id*(data_ptr)
 * 2) declare the attribute in the attributes list:
 *      ZB_ZCL_DECLARE_DIAGNOSTICS_ATTRIB_LIST
 * 3) implement your counter at:
 *    3.1) ZDO
 *      - zb_zdo_globals.h  -> zdo_diagnostics_info_t
 *      - zdo_diagnostics.h -> zdo_diagnostics_counter_id_t
 *      - zdo_diagnostics.c -> zdo_diagnostics_inc()
 *      - increment your counter in code using the ZDO_DIAGNOSTICS_INC() macro;
 *    OR
 *    3.2) MAC
 *      - zb_mac.h -> zb_mac_diagnostic_info_t
 *      - mac_zcl_diagnostic.c -> implement a function to increment the counter;
 *      - mac_internal.h -> add macros to use the incrementing function;
 *      - increment your counter in code using the macro;
 * 4) add copying the counter value to
 *      zcl_diagnostics_commands.c -> zb_zcl_diagnostics_get_attr_value_by_id()
 *
 *   Important note:
 * We have asynchronous reading the Diagnostics attributes:
 * in the zb_zcl_read_attr_handler() we call zdo_diagnostics_get_stats()
 * that will copy all counters to the same buffer from the first byte.
 * What the problem? We have the following buffer structure:
 *   a) buffer begin (from zero byte! not from zb_buf_begin()) - will contain all counters;
 *   b) buffer middle (from zb_buf_begin()) - contains one or more zb_zcl_read_attr_req_t;
 *   c) buffer end (can be get with ZB_BUF_GET_PARAM()) - contains zb_zcl_parsed_hdr_t.
 * As you see, we can place all counters only from buffer begin (a).
 * It's possible to get a situation when we don't have enough space,
 * and you will catch assert in the zdo_diagnostics_get_mac_stats_cb() function.
 * In this case it's necessary to move data from buffer middle to the right a bit.
 */

/*! @} */ /* Diagnostics cluster attributes */

/*! @} */ /* ZCL Diagnostics cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_diagnostics_init_server(void);
void zb_zcl_diagnostics_init_client(void);

/**
 * @brief Synchronize MAC and ZDO counters with ZCL attributes.
 *
 * Call this function each time when you want to get an attribute value
 * and process statistic using @zb_zcl_get_attr_desc_a
 *
 * When synchronization will be finished, users callback
 * with a specified parameter will be called.
 *
 * @param cb_param - a parameter of users callback
 * @param cb - users callback
 */
zb_ret_t zb_zcl_diagnostics_sync_counters(zb_uint8_t cb_param, zb_callback_t cb);

#define ZB_ZCL_CLUSTER_ID_DIAGNOSTICS_SERVER_ROLE_INIT zb_zcl_diagnostics_init_server
#define ZB_ZCL_CLUSTER_ID_DIAGNOSTICS_CLIENT_ROLE_INIT zb_zcl_diagnostics_init_client

#else /* defined(ZB_ZCL_SUPPORT_CLUSTER_DIAGNOSTICS) */

#define ZB_ZCL_CLUSTER_ID_DIAGNOSTICS_SERVER_ROLE_INIT
#define ZB_ZCL_CLUSTER_ID_DIAGNOSTICS_CLIENT_ROLE_INIT

#endif /* defined(ZB_ZCL_SUPPORT_CLUSTER_DIAGNOSTICS) */

#endif /* ZB_ZCL_DIAGNOSTICS_H */
