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
/*  PURPOSE: Public Network layer API
*/
#ifndef ZB_ZBOSS_API_NWK_H
#define ZB_ZBOSS_API_NWK_H 1

#include "zboss_api_mm.h"

/** \addtogroup nwk_api */
/** @{ */

/** @addtogroup nwk_common_constants NWK common constants
 * @{
 */

/**
 * @name Network broadcast addresses types
 * @anchor nwk_broadcast_addresses
*/
/** @{ */
#define ZB_NWK_BROADCAST_ALL_DEVICES        0xFFFFu /**< All devices in PAN */
#define ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE    0xFFFDu /**< macRxOnWhenIdle = TRUE */
#define ZB_NWK_BROADCAST_ROUTER_COORDINATOR 0xFFFCu /**< All routers and coordinator */
#define ZB_NWK_BROADCAST_LOW_POWER_ROUTER   0xFFFBu /**< Low power routers only */

#define ZB_NWK_BROADCAST_RESERVED           0xFFF8u /**< Reserved value */
/** @} */ /* nwk_broadcast_addresses */

/** @} */ /* nwk_common_constants */

/** @addtogroup nwk_management_service NWK management service
 *  @{
 */
/** @brief Check that address is broadcast.
  * @param addr - 16-bit address
  * @return TRUE if address is broadcast, FALSE otherwise
  */
/* #define ZB_NWK_IS_ADDRESS_BROADCAST(addr) ( ((addr) & 0xFFF0) == 0xFFF0 ) */
#define ZB_NWK_IS_ADDRESS_BROADCAST(addr) ( (addr) >= ZB_NWK_BROADCAST_RESERVED )
/** @} */ /* nwk_management_service */

/** @addtogroup nwk_common_constants NWK common constants
 * @{
 */

/**
 * @name Network device type
 * @anchor nwk_device_type
 */
/** @{ */
#define ZB_NWK_DEVICE_TYPE_COORDINATOR 0U /*!< Device - Coordinator */
#define ZB_NWK_DEVICE_TYPE_ROUTER      1U /*!< Device - Router */
#define ZB_NWK_DEVICE_TYPE_ED          2U /*!< Device - End device */
#define ZB_NWK_DEVICE_TYPE_NONE        3U /*!< Unknown Device */
/** @} */

/**
 * @brief Type for network device type
 *
 * Holds one of @ref nwk_device_type. Kept for backward compatibility as
 * @ref nwk_device_type were declared previously as enum.
 */
typedef zb_uint8_t zb_nwk_device_type_t;

/**
 * @name Leave types
 * @anchor nwk_leave_type
 */
/** @{ */
#define ZB_NWK_LEAVE_TYPE_RESET  0x00U /**< Leave without rejoin */
#define ZB_NWK_LEAVE_TYPE_REJOIN 0x01U /**< Leave with rejoin */
/** @} */

/**
 * @brief Type for leave types
 *
 * Holds one of @ref nwk_leave_type. Kept for backward compatibility as
 * @ref nwk_leave_type were declared previously as enum.
 */
typedef zb_uint8_t zb_nwk_leave_type_t;


/** @cond internals_doc */
/**
 * @name Network command status codes
 * @anchor nwk_command_status
 */
/** @{ */
#define ZB_NWK_COMMAND_STATUS_NO_ROUTE_AVAILABLE          0x00U /**< No route available */
#define ZB_NWK_COMMAND_STATUS_TREE_LINK_FAILURE           0x01U /**< Tree link failure */
#define ZB_NWK_COMMAND_STATUS_NONE_TREE_LINK_FAILURE      0x02U /**< None-tree link failure */
#define ZB_NWK_COMMAND_STATUS_LOW_BATTERY_LEVEL           0x03U /**< Low battery level */
#define ZB_NWK_COMMAND_STATUS_NO_ROUTING_CAPACITY         0x04U /**< No routing capacity */
#define ZB_NWK_COMMAND_STATUS_NO_INDIRECT_CAPACITY        0x05U /**< No indirect capacity */
#define ZB_NWK_COMMAND_STATUS_INDIRECT_TRANSACTION_EXPIRY 0x06U /**< Indirect transaction expiry */
#define ZB_NWK_COMMAND_STATUS_TARGET_DEVICE_UNAVAILABLE   0x07U /**< Target device unavailable */
#define ZB_NWK_COMMAND_STATUS_TARGET_ADDRESS_UNALLOCATED  0x08U /**< Target address unallocated */
#define ZB_NWK_COMMAND_STATUS_PARENT_LINK_FAILURE         0x09U /**< Parent link failure */
#define ZB_NWK_COMMAND_STATUS_VALIDATE_ROUTE              0x0aU /**< Validate route */
#define ZB_NWK_COMMAND_STATUS_SOURCE_ROUTE_FAILURE        0x0bU /**< Source route failure */
#define ZB_NWK_COMMAND_STATUS_MANY_TO_ONE_ROUTE_FAILURE   0x0cU /**< Many-to-one route failure */
#define ZB_NWK_COMMAND_STATUS_ADDRESS_CONFLICT            0x0dU /**< Address conflict */
#define ZB_NWK_COMMAND_STATUS_VERIFY_ADDRESS              0x0eU /**< Verify address */
#define ZB_NWK_COMMAND_STATUS_PAN_IDENTIFIER_UPDATE       0x0fU /**< Pan ID update */
#define ZB_NWK_COMMAND_STATUS_NETWORK_ADDRESS_UPDATE      0x10U /**< Network address update */
#define ZB_NWK_COMMAND_STATUS_BAD_FRAME_COUNTER           0x11U /**< Bad frame counter  */
#define ZB_NWK_COMMAND_STATUS_BAD_KEY_SEQUENCE_NUMBER     0x12U /**< Bad key sequence number */
#define ZB_NWK_COMMAND_STATUS_UNKNOWN_COMMAND             0x13U /**< Command received is not known *//** @} */
/** @} */

/**
 * @brief Type for network command status codes
 *
 * Holds one of @ref nwk_command_status. Kept for backward compatibility as
 * @ref nwk_command_status were declared previously as enum.
 */
typedef zb_uint8_t zb_nwk_command_status_t;

/** @endcond */ /* internals_doc */
/** @} */ /* nwk_common_constants */

/** @cond internals_doc */
/** @brief 3.4.3 Network Status Command: Arguments of the NLME-STATUS.indication routine. */
typedef ZB_PACKED_PRE struct zb_nlme_status_indication_s
{
  zb_uint8_t status; /**< Error code associated with the failure */
  zb_uint16_t network_addr;  /**< Network device address associated with the status information */
  zb_uint8_t unknown_command_id; /**< Unknown command ID
                                    (required for ZB_NWK_COMMAND_STATUS_UNKNOWN_COMMAND) */
} ZB_PACKED_STRUCT
zb_nlme_status_indication_t;
/** @endcond */ /* internals_doc */

/** @addtogroup nwk_common_constants NWK common constants
 * @{
 */
/**
   LQI undefined value
 */
#define ZB_MAC_LQI_UNDEFINED 0U
/**
   RSSI undefined value
 */
#define ZB_MAC_RSSI_UNDEFINED 0x7fU

/**
   Number of the first channel
*/
/* #define ZB_MAC_START_CHANNEL_NUMBER 11 */
/**
   Maximal number of the channels
 */
#define ZB_MAC_MAX_CHANNEL_NUMBER   26U
/**
   Total number of supported channels
 */
#define ZB_MAC_SUPPORTED_CHANNELS   (ZB_MAC_MAX_CHANNEL_NUMBER + 1U)

/** @cond DOXYGEN_SE_SECTION */
/* See D.10.2.1.3.2 Channel Pages */

/* North America and Mexico region Page 23 */

/**
   The first logical channel for Page 23 Sub GHz
*/
#define ZB_PAGE23_SUB_GHZ_START_LOGICAL_CHANNEL 0U

/**
   Number of the first channel in channel mask for Page 23 Sub GHz
*/
#define ZB_PAGE23_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 23 Sub GHz
*/
#define ZB_PAGE23_SUB_GHZ_MAX_LOGICAL_CHANNEL 24U
/**
   Number of the last channel in channel mask for Page 23 Sub GHz
*/
#define ZB_PAGE23_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE23_SUB_GHZ_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE23_SUB_GHZ_START_LOGICAL_CHANNEL)

/* Europe region Pages 24 - 27 */

/**
   The first logical channel for Page 24 Sub GHz
*/
#define ZB_PAGE24_SUB_GHZ_START_LOGICAL_CHANNEL 56U

/**
   Number of the first channel in channel mask for Page 24 Sub GHz
*/
#define ZB_PAGE24_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 24 Sub GHz
*/
#define ZB_PAGE24_SUB_GHZ_MAX_LOGICAL_CHANNEL 76U
/**
   Number of the last channel in channel mask for Page 24 Sub GHz
*/
#define ZB_PAGE24_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE24_SUB_GHZ_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE24_SUB_GHZ_START_LOGICAL_CHANNEL)

/**
   The first logical channel for Page 25 Sub GHz
*/
#define ZB_PAGE25_SUB_GHZ_START_LOGICAL_CHANNEL 0U

/**
   Number of the first channel in channel mask for Page 25 Sub GHz
*/
#define ZB_PAGE25_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 25 Sub GHz
*/
#define ZB_PAGE25_SUB_GHZ_MAX_LOGICAL_CHANNEL 26U
/**
   Number of the last channel in channel mask for Page 25 Sub GHz
*/
#define ZB_PAGE25_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE25_SUB_GHZ_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE25_SUB_GHZ_START_LOGICAL_CHANNEL)

/**
   The first logical channel for Page 26 Sub GHz
*/
#define ZB_PAGE26_SUB_GHZ_START_LOGICAL_CHANNEL 27U

/**
   Number of the first channel in channel mask for Page 26 Sub GHz
*/
#define ZB_PAGE26_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 26 Sub GHz
*/
#define ZB_PAGE26_SUB_GHZ_MAX_LOGICAL_CHANNEL 34U
/**
   Number of the last channel in channel mask for Page 26 Sub GHz
*/
#define ZB_PAGE26_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE26_SUB_GHZ_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE26_SUB_GHZ_START_LOGICAL_CHANNEL)

/**
   The first logical channel for Page 27 Sub GHz
*/
#define ZB_PAGE27_SUB_GHZ_START_LOGICAL_CHANNEL 35U

/**
   Number of the first channel in channel mask for Page 27 Sub GHz
*/
#define ZB_PAGE27_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 27 Sub GHz
*/
#define ZB_PAGE27_SUB_GHZ_MAX_LOGICAL_CHANNEL 55U
/**
   Number of the last channel in channel mask for Page 27 Sub GHz
*/
#define ZB_PAGE27_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE27_SUB_GHZ_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE27_SUB_GHZ_START_LOGICAL_CHANNEL)

/* Great Britain region Pages 28 - 31 */

/**
   The first logical channel for Page 28 Sub GHz
*/
#define ZB_PAGE28_SUB_GHZ_START_LOGICAL_CHANNEL 0U

/* 08/21/2017 EE CR:MINOR Am I right this value is always 0 for any page? Do we ever need separate constants? */
/**
   Number of the first channel in channel mask for Page 28 Sub GHz
*/
#define ZB_PAGE28_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 28 Sub GHz
*/
#define ZB_PAGE28_SUB_GHZ_MAX_LOGICAL_CHANNEL 26U
/**
   Number of the last channel in channel mask for Page 28 Sub GHz
*/
#define ZB_PAGE28_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE28_SUB_GHZ_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE28_SUB_GHZ_START_LOGICAL_CHANNEL)

/**
   The first logical channel for Page 29 Sub GHz
*/
#define ZB_PAGE29_SUB_GHZ_START_LOGICAL_CHANNEL 27U
/**
   Number of the first channel in channel mask for Page 29 Sub GHz
*/
#define ZB_PAGE29_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 29 Sub GHz (for solid range 27-34)
*/
#define ZB_PAGE29_SUB_GHZ_PRE_MAX_LOGICAL_CHANNEL 34U
/**
   The last logical channel for Page 29 Sub GHz
*/
#define ZB_PAGE29_SUB_GHZ_MAX_LOGICAL_CHANNEL 62U
/**
   Number of the last channel in channel mask for Page 29 Sub GHz
*/
#define ZB_PAGE29_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE29_SUB_GHZ_PRE_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE29_SUB_GHZ_START_LOGICAL_CHANNEL + 1U /* 62 channel */)

/**
   The first logical channel for Page 30 Sub GHz
*/
#define ZB_PAGE30_SUB_GHZ_START_LOGICAL_CHANNEL 35U
/**
   Number of the first channel in channel mask for Page 30 Sub GHz
*/
#define ZB_PAGE30_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 30 Sub GHz
*/
#define ZB_PAGE30_SUB_GHZ_MAX_LOGICAL_CHANNEL 61U
/**
   Number of the last channel in channel mask for Page 30 Sub GHz
*/
#define ZB_PAGE30_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE30_SUB_GHZ_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE30_SUB_GHZ_START_LOGICAL_CHANNEL)

/**
   The first logical channel for Page 31 Sub GHz
*/
#define ZB_PAGE31_SUB_GHZ_START_LOGICAL_CHANNEL 0U
/**
   Number of the first channel in channel mask for Page 31 Sub GHz
*/
#define ZB_PAGE31_SUB_GHZ_START_CHANNEL_NUMBER 0U
/**
   The last logical channel for Page 31 Sub GHz
*/
#define ZB_PAGE31_SUB_GHZ_MAX_LOGICAL_CHANNEL 26U
/**
   Number of the last channel in channel mask for Page 31 Sub GHz
*/
#define ZB_PAGE31_SUB_GHZ_MAX_CHANNEL_NUMBER (ZB_PAGE31_SUB_GHZ_MAX_LOGICAL_CHANNEL - \
                                              ZB_PAGE31_SUB_GHZ_START_LOGICAL_CHANNEL)
  /** @endcond */ /* DOXYGEN_SE_SECTION */
/* ZB_MAC_START_CHANNEL_NUMBER */
/**
   The first logical channel for Page 0 2.4GHz
*/
#define ZB_PAGE0_2_4_GHZ_START_LOGICAL_CHANNEL 11U
/* Let's keep compatibility with bits layout in ZBOSS without sub-gig: start from 11 in 2.4.
   Actually, only subgig pages 28 and 31 are ok too (logical == bit#). Pages 29 and 30 are special case.
 */
/**
   Number of the first channel in channel mask for Page 0 2.4GHz
*/
#define ZB_PAGE0_2_4_GHZ_START_CHANNEL_NUMBER ZB_PAGE0_2_4_GHZ_START_LOGICAL_CHANNEL
/**
   The last logical channel in channel mask for Page 0 2.4GHz
*/
#define ZB_PAGE0_2_4_GHZ_MAX_LOGICAL_CHANNEL ZB_MAC_MAX_CHANNEL_NUMBER
/**
   Number of the last channel in channel mask for Page 0 2.4GHz
*/
#define ZB_PAGE0_2_4_GHZ_MAX_CHANNEL_NUMBER  ZB_MAC_MAX_CHANNEL_NUMBER
/* (ZB_PAGE0_2_4_GHZ_MAX_LOGICAL_CHANNEL - \ */
/*  ZB_PAGE0_2_4_GHZ_START_LOGICAL_CHANNEL) */

/** @cond DOXYGEN_SE_SECTION */
/**
   Maximal number of channels for Page 23 Sub GHz
*/
#define ZB_PAGE23_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE23_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE23_SUB_GHZ_START_CHANNEL_NUMBER + 1U)
/**
   Maximal number of channels for Page 24 Sub GHz
*/
#define ZB_PAGE24_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE24_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE24_SUB_GHZ_START_CHANNEL_NUMBER + 1U)
/**
   Maximal number of channels for Page 25 Sub GHz
*/
#define ZB_PAGE25_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE25_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE25_SUB_GHZ_START_CHANNEL_NUMBER + 1U)
/**
   Maximal number of channels for Page 26 Sub GHz
*/
#define ZB_PAGE26_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE26_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE26_SUB_GHZ_START_CHANNEL_NUMBER + 1U)
/**
   Maximal number of channels for Page 27 Sub GHz
*/
#define ZB_PAGE27_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE27_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE27_SUB_GHZ_START_CHANNEL_NUMBER + 1U)

/**
   Maximal number of channels for Page 28 Sub GHz
*/
#define ZB_PAGE28_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE28_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE28_SUB_GHZ_START_CHANNEL_NUMBER + 1U)
/**
   Maximal number of channels for Page 29 Sub GHz
*/
#define ZB_PAGE29_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE29_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE29_SUB_GHZ_START_CHANNEL_NUMBER + 1U)
/**
   Maximal number of channels for Page 30 Sub GHz
*/
#define ZB_PAGE30_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE30_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE30_SUB_GHZ_START_CHANNEL_NUMBER + 1U)
/**
   Maximal number of channels for Page 31 Sub GHz
*/
#define ZB_PAGE31_SUB_GHZ_MAX_CHANNELS_COUNT (ZB_PAGE31_SUB_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE31_SUB_GHZ_START_CHANNEL_NUMBER + 1U)
/** @endcond */ /* DOXYGEN_SE_SECTION */
/**
   Maximal number of channels for Page 0 2.4GHz
*/
#define ZB_PAGE0_2_4_GHZ_MAX_CHANNELS_COUNT  (ZB_PAGE0_2_4_GHZ_MAX_CHANNEL_NUMBER - \
                                              ZB_PAGE0_2_4_GHZ_START_CHANNEL_NUMBER  + 1U)
/** @cond DOXYGEN_SE_SECTION */
/**
   Maximal number of ed scan channels for all pages
*/
#define ZB_ED_SCAN_MAX_CHANNELS_COUNT                                   \
  ((ZB_IO_BUF_SIZE - sizeof(zb_uint8_t)) / sizeof(zb_energy_detect_channel_info_t))
/**
   Max # of network descriptors which can fit into a single buffer.
*/
#define ZB_ACTIVE_SCAN_MAX_NETWORK_COUNT                                   \
  ((ZB_IO_BUF_SIZE - sizeof(zb_nlme_network_discovery_confirm_t)) / sizeof(zb_nlme_network_descriptor_t))

#define ZB_CHANNEL_PAGE_MAX_CHANNELS_COUNT ZB_PAGE28_SUB_GHZ_MAX_CHANNELS_COUNT
/** @endcond */ /* DOXYGEN_SE_SECTION */
/** @} */ /* nwk_common_constants */

/** @cond internals_doc */
/** @addtogroup nwk_pib_cache PIB cache
 *  @{
 */

/**
  Data structure used to cache frequently used PIB attributes.

  Useful when using alien MAC layer without direct access to PIB: blockable "get"
  interface to PIB is too complex and slow.
 */
typedef struct zb_nwk_pib_cache_s
{
  zb_uint16_t          mac_short_address;                              /*!< The 16-bit address that the device uses
                                                                              to communicate in the PAN. */
  zb_uint16_t          mac_pan_id;                                     /*!< The 16-bit identifier of the PAN on which
                                                                              the device is operating. If this value is 0xffff,
                                                                              the device is not associated. */
  zb_ieee_addr_t       mac_extended_address;                           /*!< The 64-bit (IEEE) address assigned to the device. */

  zb_uint8_t           mac_association_permit;                         /*!< Indication of whether a coordinator is currently
                                                                              allowing association. A value of TRUE indicates*/
  zb_uint8_t           mac_rx_on_when_idle;                            /*!< Indication of whether the MAC sublayer is to enable
                                                                              its receiver during idle periods. */
  zb_uint8_t           phy_current_page[ZB_NWK_MAC_IFACE_TBL_SIZE];    /*!< Index of current physical channel page  */
  zb_uint8_t           phy_current_channel[ZB_NWK_MAC_IFACE_TBL_SIZE]; /*!< Index of current physical channel */
  zb_uint8_t           phy_primary_iface;                              /*!< Index of MAC interface that is used for joining */
} zb_nwk_pib_cache_t;

/** @} */


/**
  * @name PIB cache mirror
  * \par Here are cache of PIB / NIB mirror of PIB values.
  *
  * After that values modified in NWK or upper, must sync it with MAC using MLME-SET.
  *
  * @{
  */

/**
  Function used to access PIB cache.
  @return pointer to PIB cache.
  */
zb_nwk_pib_cache_t *zb_nwk_get_pib_cache(void);
/**
  Macro used to access PIB cache.
  @return pointer to PIB cache.
  */


/* We get false-positive violations of MISRA rules 5.2 and 5.4 here
 * because an internal header redefine ZB_PIB_CACHE macro for
 * optimization purposes, and C-STAT can not properly handle
 * #undef usage. Only one macro definition is valid at a time,
 * but MISRA rules 5.2 and 5.4 prohibit two definitions of a
 * symbol simultaneously.
 * We ensure that only one instance of the macro is effectively
 * defined here. */
#ifdef ZB_PIB_CACHE
#error Please ensure that you are not including any internal headers which redefine ZB_PIB_CACHE macro before including this file
#else /* ZB_PIB_CACHE */
/*cstat !MISRAC2012-Rule-5.2_c99 !MISRAC2012-Rule-5.4_c99 */
#define ZB_PIB_CACHE() zb_nwk_get_pib_cache()
#endif /* ZB_PIB_CACHE */

#ifndef NCP_MODE_HOST
/** Cached value of device network address */
#define ZB_PIBCACHE_NETWORK_ADDRESS()  ZB_PIB_CACHE()->mac_short_address
/** Cached value of device Pan ID */
#define ZB_PIBCACHE_PAN_ID()           ZB_PIB_CACHE()->mac_pan_id
/** Cached value of device extended address */
#define ZB_PIBCACHE_EXTENDED_ADDRESS() ZB_PIB_CACHE()->mac_extended_address
/** Cached value of RxOnWhenIdle attribute */
#ifdef ZB_ED_RX_OFF_WHEN_IDLE
#define ZB_PIBCACHE_RX_ON_WHEN_IDLE() ZB_B2U(!ZB_IS_DEVICE_ZED())
#else
#define ZB_PIBCACHE_RX_ON_WHEN_IDLE()  ZB_PIB_CACHE()->mac_rx_on_when_idle
#endif
/** Cached value of AssociationPermit attribute */
#define ZB_PIBCACHE_ASSOCIATION_PERMIT() ZB_PIB_CACHE()->mac_association_permit

#define ZB_PIBCACHE_PRIMARY_IFACE()  ZB_PIB_CACHE()->phy_primary_iface
#define ZB_PIBCACHE_PRIMARY_IFACE_PAGE() ZB_PIB_CACHE()->phy_current_page[ZB_PIB_CACHE()->phy_primary_iface]
#define ZB_PIBCACHE_PRIMARY_IFACE_CHANNEL() ZB_PIB_CACHE()->phy_current_channel[ZB_PIB_CACHE()->phy_primary_iface]

/** Cached value of CurrentChannel attribute */
#define ZB_PIBCACHE_CURRENT_CHANNEL()  ZB_PIBCACHE_PRIMARY_IFACE_CHANNEL()
#define ZB_PIBCACHE_CURRENT_CHANNEL_BY_IFACE(iface_id)  ZB_PIB_CACHE()->phy_current_channel[iface_id]
#define ZB_PIBCACHE_CURRENT_CHANNELS_LIST() ZB_PIB_CACHE()->phy_current_channel
/** Cached value of CurrentPage attribute */
#define ZB_PIBCACHE_CURRENT_PAGE()  ZB_PIBCACHE_PRIMARY_IFACE_PAGE()
#define ZB_PIBCACHE_CURRENT_PAGE_BY_IFACE(iface_id)  ZB_PIB_CACHE()->phy_current_page[iface_id]
#define ZB_PIBCACHE_CURRENT_PAGES_LIST() ZB_PIB_CACHE()->phy_current_page

#else /* NCP_MODE_HOST */

/** Cached value of device network address */
#define ZB_PIBCACHE_NETWORK_ADDRESS()  zb_get_short_address()
/** Cached value of RxOnWhenIdle attribute */
#ifdef ZB_ED_RX_OFF_WHEN_IDLE
/* Make simpler checks if ZED always rx-off-when-idle */
#undef ZB_PIBCACHE_RX_ON_WHEN_IDLE
#define ZB_PIBCACHE_RX_ON_WHEN_IDLE() ZB_B2U(!ZB_IS_DEVICE_ZED())
#else
#define ZB_PIBCACHE_RX_ON_WHEN_IDLE()  ZB_B2U(zb_get_rx_on_when_idle())
#endif
#endif /* !NCP_MODE_HOST */

/**
   Arguments of the NLME-PERMIT_JOINING.request routine.
*/
typedef ZB_PACKED_PRE struct zb_nlme_permit_joining_request_s
{
  zb_uint8_t permit_duration; /**< Time in seconds during which the coordinator
                               * or router will allow associations */
} ZB_PACKED_STRUCT
zb_nlme_permit_joining_request_t;

/**
   NLME-PERMIT-JOINING.request primitive

   Allow/disallow network joining

   @param param - buffer containing parameters - @see
   zb_nlme_network_formation_request_t
   @return RET_OK on success, error code otherwise.

   @snippet doxygen_snippets.dox zb_nlme_permit_joining_request
 */
void zb_nlme_permit_joining_request(zb_uint8_t param);


/** @} */
/** @endcond */ /* internals_doc */

/** @addtogroup nwk_panid_conflicts NWK PAN ID conflicts
 *  @{
 */

#define ZB_PAN_ID_CONFLICT_INFO_MAX_PANIDS_COUNT  16U

/** Structure describing a detected PAN ID conflict */
typedef ZB_PACKED_PRE struct zb_pan_id_conflict_info_s
{
  zb_uint16_t panid_count;      /* <! Count of neighboring PAN IDs. */
  zb_uint16_t panids[ZB_PAN_ID_CONFLICT_INFO_MAX_PANIDS_COUNT];
                                /* <! Array of such PAN IDs.
                                 *  Only values at indices 0..(panid_count - 1) will be taken
                                 *  into consideration. */
} ZB_PACKED_STRUCT zb_pan_id_conflict_info_t;

/**
   Allow/disallow network joining

   @param param - pointer to a structure containing PAN ID conflict
   information - @see zb_pan_id_conflict_info_t

 * @b Example:
 ZB_ZDO_SIGNAL_CUT_HEADER macro may be used to remove from the buffer everything but the signal parameter (zb_pan_id_conflict_info_t structure in this case)
   @code

void zboss_signal_handler(zb_uint8_t param)
{
  zb_pan_id_conflict_info_t *info = NULL;
  zb_zdo_app_signal_hdr_t *sg_p = NULL;
  zb_zdo_app_signal_type_t sig = zb_get_app_signal(param, &sg_p);

  if (ZB_GET_APP_SIGNAL_STATUS(param) == 0)
  {
    switch (sig)
    {
      case ZB_NWK_SIGNAL_PANID_CONFLICT_DETECTED:
        info = ZB_ZDO_SIGNAL_GET_PARAMS(sg_p, zb_pan_id_conflict_info_t);
        ZB_ZDO_SIGNAL_CUT_HEADER(ZB_BUF_FROM_REF(param));
        zb_start_pan_id_conflict_resolution(param);
        break;
    }
  }
}
   @endcode
 */
void zb_start_pan_id_conflict_resolution(zb_uint8_t param);


/**
   This function must be used for enabling/disabling automatic PAN ID conflict resolution
   If the automatic resolution is disabled, ZBOSS will issue a ZB_NWK_SIGNAL_PANID_CONFLICT_DETECTED
   signal each time it receives Network Report about PAN ID conflict or (in case the device is
   the network manager) each time it detects such conflicts.

   By default the automatic PAN ID conflict resolution is disabled.

   That call also enabled panid conflict resolution - see zb_enable_panid_conflict_resolution();

   @param status - ZB_TRUE in order to enable automatic PAN ID conflict, ZB_FALSE otherwise
 */
void zb_enable_auto_pan_id_conflict_resolution(zb_bool_t status);


/** @cond internals_doc */
/**
   Toggles panid conflict resolution.

   Call of that function forces linking of panid conflict resolution
   code and allows switching on/off panid conflict resolution and detection
   logic.
 */
void zb_enable_panid_conflict_resolution(zb_bool_t status);
/** @endcond */ /* internals_doc */
/** @} */ /* nwk_panid_conflicts */

/** @addtogroup nwk_management_service NWK management service
 * @{
 */
#ifdef ZB_LOW_SECURITY_MODE
/**
    Public API to set device security level to 0
    @deprecated This function will be removed in the next Major release after june 2023.
                Now security level is always equal to 5.
*/
void zb_disable_nwk_security(void);

/**
    Public API to set device security level to 5
    @deprecated This function will be removed in the next Major release after june 2023.
                Now security level is always equal to 5.
*/
void zb_enable_nwk_security(void);
#endif /* ZB_LOW_SECURITY_MODE */

#ifdef ZB_NWK_CONFIGURABLE_DST_IEEE_IN_HDR
/**
   Configure Destination IEEE policy for out NWK frames

   @param put_always - if ZB_TRUE, put DST IEEE for all NWK frame types, if it is set to ZB_FALSE -
   put it for NWK Command frame type only
   Default value is ZB_TRUE.
*/
void zb_nwk_set_ieee_policy(zb_bool_t put_always);
#endif /* ZB_NWK_CONFIGURABLE_DST_IEEE_IN_HDR */
/** @} */ /* nwk_management_service */

/** @addtogroup nwk_mtorr NWK MTORR functionality
 * @{
 */

#ifdef ZB_COORDINATOR_ROLE
/**
   Enable Concentrator mode for the device (disabled by default).
   It's possible to call this function to send MTORR immediately, e.g. after a new device joined the network.
   It does affect only for Coordinator role.

   @param radius - the hop count radius for concentrator route discoveries.
   If the value is set zero then the default radius will be used.
   @param disc_time - the time in seconds between concentrator route discoveries.
   If the value is set to zero, the route discoveries are done by the application layer only.
*/
void zb_start_concentrator_mode(zb_uint8_t radius, zb_uint32_t disc_time);

/**
   Disable Concentrator mode for the device.
   It does affect only for Coordinator role.
*/
void zb_stop_concentrator_mode(void);
#endif /* ZB_COORDINATOR_ROLE */
/** @} */ /* nwk_mtorr */

/** @cond internals_doc */
/**
  Change Transmit Failure Threshold for Neighbor table.
  This threshold defines how many packets may fail the delivery before stack will start Address
  Discovery.
  Formula for discovery start: if device is a neighbor and packet can not be delivered, try to
  - do "Transmit Failure Threshold" attempts of packet delivery
  - if packet is not delivered, wait "Transmit Failure Timeout"
  - if packet is not delivered, remove device from neighbors and start address discovery procedure
  @param transmit_failure_cnt - new threshold (1 to 255)
  Default value - @ref ZB_ZDO_NEIGHBOR_FAILURE_PKT_THRESHOLD
*/
void zb_set_nbt_transmit_failure_threshold(zb_uint8_t transmit_failure_cnt);
/**
  Change Transmit Failure Timeout for Neighbor table.
  This timeout defines how much time stack will wait before starting Network Address
  discovery. Countdown starts after Transmit Failure Threshold condition is met.
  @param transmit_failure_timeout - new timeout in seconds (0 to 255)
  Default value - @ref ZB_ZDO_NEIGHBOR_UNREACHABLE_TIMEOUT
*/
void zb_set_nbt_transmit_failure_timeout(zb_uint8_t transmit_failure_timeout);
/** @endcond */ /* internals_doc */

/** @addtogroup nwk_management_service NWK management service
 * @{
 */

/**
   Get own device type.

   @return One of the values from @ref nwk_device_type.

   @snippet onoff_server/on_off_output_zc.c zb_get_device_type_example
 */
zb_nwk_device_type_t zb_get_device_type(void);

/**
   Get short address of the parent node.

   @return Short address of the parent node or ZB_UNKNOWN_SHORT_ADDR if the device isn't joined to a network.
 */
zb_uint16_t zb_nwk_get_parent(void);

#define ZB_NWK_NBR_ITERATOR_INDEX_EOT 0xFFFFU /*! Index, indicating that the iterator reached boundaries of the neighbour table. */

typedef ZB_PACKED_PRE struct zb_nwk_nbr_iterator_cb_params_s {
   zb_uint16_t index;        /*!< In the callback function:
                              *     Index of the returned neighbour table entry.
                              *     The value of ZB_NWK_NBR_ITERATOR_INDEX_EOT
                              *     indicates that the entry was not returned and
                              *     the buffer payload should be ignored.
                              *   If the structure is passed as the API call parameters:
                              *     Index, from which the next neighbour table
                              *     entry should be searched.
                              */
   zb_uint32_t update_count; /*!< In the callback function:
                              *     The current value of the table update counter.
                              *     This parameter is ignored in the API call parameters.
                              */
} ZB_PACKED_STRUCT
zb_nwk_nbr_iterator_params_t;

typedef ZB_PACKED_PRE struct zb_nwk_nbr_iterator_entry_s
{
  zb_ieee_addr_t  ieee_addr;            /*!< Long address (EUI64) of the device. */
  zb_uint16_t     short_addr;           /*!< Short address (network address) of the device. */

  zb_uint8_t      device_type;          /*!< Neighbor device type - @see @ref nwk_device_type */
  zb_uint8_t      depth;                /*!< The network depth of this device.
                                         *    A value of 0x00 indicates that this device is the
                                         *    Zigbee coordinator for the network.
                                         */
  zb_uint8_t      rx_on_when_idle;      /*!< Indicates if neighbour receiver enabled during idle periods:
                                         *     TRUE = Receiver is on
                                         *     FALSE = Receiver is off
                                         *   This field should be present for entries that record the parent or
                                         *   children of a Zigbee router or Zigbee coordinator.
                                         */
  zb_uint8_t      relationship;         /*!< The relationship between the neighbour and the current device.
                                         *   This field shall be present in every neighbour table entry.
                                         *   @if DOXYGEN_INTERNAL_DOC See @ref nwk_relationship @endif
                                         */
  zb_uint8_t      send_via_routing;     /*!< That field is deprecated. Removed
                                         * from zb_neighbor_tbl_ent_t, always 0
                                         * here. */

  zb_uint8_t      keepalive_received;   /*!< This value indicates at least one keepalive
                                         *   has been received from the end device since
                                         *   the router has rebooted.
                                         */
  zb_uint8_t      mac_iface_idx;        /*!< An index into the MAC Interface Table
                                         * indicating what interface the neighbour or
                                         * child is bound to.
                                         */

  zb_uint8_t      transmit_failure_cnt; /*!< Transmit failure counter (used to initiate
                                         * device address search).
                                         */
  zb_uint8_t      lqi;                  /*!< Link quality. Also used to calculate
                                         * incoming cost
                                         */
  zb_int8_t       rssi;                 /*!< Received signal strength indicator */
  zb_uint8_t      outgoing_cost;        /*!< The cost of an outgoing link. Got from link status. */
  zb_uint8_t      age;                  /*!< Counter value for router aging.
                                         *   The number of nwkLinkStatusPeriod intervals since a
                                         *   link status command was received.
                                         */
  zb_uint32_t     device_timeout;       /*!< Configured end device timeout, in seconds. */
  zb_uint32_t     timeout_counter;      /*!< Timeout value ED aging, in milliseconds. */
} ZB_PACKED_STRUCT
zb_nwk_nbr_iterator_entry_t;

/**
   Read the next active entry from the NWK neighbour table.
   The index indicates the point, from which the entry will be searched in the neighbour table.
   This API returns neighbour table entry inside the buffer payload, that are connected to the same PAN
   and their entries are not marked as stale or timed out.
   The index of the entry is passed as buffer parameters.

   @param  bufid  The ZBOSS buffer, containing arguments defined by zb_nwk_nbr_iterator_params_t structure, passed as buffer parameters.
   @param  cb     Callback function, that will get the next neighbour table entry.
 */
zb_ret_t zb_nwk_nbr_iterator_next(zb_uint8_t bufid, zb_callback_t cb);

/** @} */ /* nwk_management_service */

zb_ret_t zb_mac_enable_interface(zb_uint8_t iface_id);
zb_ret_t zb_mac_disable_interface(zb_uint8_t iface_id);

zb_bool_t zb_mac_is_interface_active(zb_uint8_t iface_id);

void zb_nwk_mm_set_channel_mask(zb_uint8_t iface_id, zb_uint8_t page_index, zb_uint32_t channel_mask);

/** @} */ /* nwk_api */

#endif /*#ifndef ZB_ZBOSS_API_NWK_H*/
