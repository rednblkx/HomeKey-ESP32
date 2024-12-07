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
/* PURPOSE: Poll Control cluster definitions
*/

#ifndef ZB_ZCL_CONTROL4_NETWORKING_H
#define ZB_ZCL_CONTROL4_NETWORKING_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_CONTROL4
  * @{
  *   @details
  *   @warning Using this cluster makes an application not conforming to the Zigbee PRO specification
  *
  *   This cluster provides an application level interface between Zigbee devices and a Control4
  *   Controller.
  *
  *   This is implemented using Control4 specific profile ZB_AF_CONTROL4_PROFILE_ID.
  *
  *   Control4 Network cluster also uses Many-To-One routing requests.
  *
  */

/** @name Control4 Network cluster attributes
  * @{
  */

enum zb_zcl_control4_networking_attr_e
{
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_DEVICE_TYPE_ID = 0x0000,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_ANNOUNCE_WINDOW_ID = 0x0001,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_MTORR_PERIOD_ID = 0x0002,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_FIRMWARE_VERSION_ID = 0x0004,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_REFLASH_VERSION_ID = 0x0005,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_BOOT_COUNT_ID = 0x0006,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_PRODUCT_STRING_ID = 0x0007,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_NODE_ID_ID = 0x0008,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_LONG_ID_ID = 0x0009,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_COST_ID = 0x000a,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_MESH_CHANNEL_ID = 0x000c,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_AVG_RSSI_ID = 0x0013,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_AVG_LQI_ID = 0x0014,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_BATTERY_LEVEL_ID = 0x0015,

  ZB_ZCL_ATTR_CONTROL4_NETWORKING_RADIO_4_BARS_ID = 0x0016
};

enum zb_zcl_control4_networking_device_type_e
{
  /*! "Non-sleepy end device" value */
  ZB_ZCL_CONTROL4_NETWORKING_DEVICE_TYPE_END_DEVICE = 0x03,
  /*! "Sleepy end device" value */
  ZB_ZCL_CONTROL4_NETWORKING_DEVICE_TYPE_SLEEPY_END_DEVICE  = 0x04
};

/** @brief Default value for Control4 networking cluster revision global attribute */
#define ZB_ZCL_CONTROL4_NETWORKING_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

#define ZB_ZCL_CONTROL4_NETWORKING_REFLASH_VERSION_VENDOR_SPECIFIC 0xff

#define ZB_ZCL_CONTROL4_NETWORKING_BOOT_COUNT_DEF_VALUE 0

#define ZB_ZCL_CONTROL4_NETWORKING_ANNOUNCE_WINDOW_MIN_VALUE ((zb_uint16_t)0x000f)

#define ZB_ZCL_CONTROL4_NETWORKING_ANNOUNCE_WINDOW_DEF_VALUE ((zb_uint16_t)0x012c)

#define ZB_ZCL_CONTROL4_NETWORKING_MTORR_PERIOD_MIN_VALUE ((zb_uint16_t)0x000f)

#define ZB_ZCL_CONTROL4_NETWORKING_MTORR_PERIOD_DEF_VALUE ((zb_uint16_t)0x012c)

#define ZB_ZCL_CONTROL4_NETWORKING_ACCESS_POINT_NODE_ID_DEF_VALUE ((zb_uint16_t)0xffff)

#define ZB_ZCL_CONTROL4_NETWORKING_ACCESS_POINT_LONG_ID_DEF_VALUE {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

#define ZB_ZCL_CONTROL4_NETWORKING_ACCESS_POINT_COST_DEF_VALUE 0xff

#define ZB_ZCL_CONTROL4_NETWORKING_MESH_CHANNEL_MIN_VALUE 0x0b

#define ZB_ZCL_CONTROL4_NETWORKING_MESH_CHANNEL_MAX_VALUE 0x19

#define ZB_ZCL_CONTROL4_NETWORKING_AVG_RSSI_MIN_VALUE -128

#define ZB_ZCL_CONTROL4_NETWORKING_AVG_RSSI_MAX_VALUE 0

#define ZB_ZCL_CONTROL4_NETWORKING_BATTERY_LEVEL_MIN_VALUE 0

#define ZB_ZCL_CONTROL4_NETWORKING_BATTERY_LEVEL_MAX_VALUE 100

#define ZB_ZCL_CONTROL4_NETWORKING_RADIO_4_BARS_MAX_VALUE 4

/* Number of searching ZAP attempts */
#define ZB_ZCL_CONTROL4_NETWORK_SEARCHING_ZAP_ATTEMPTS 2

/** @brief Declare attribute list for C4 Network cluster
    @param attr_list - attribute list name
    @param device_type - pointer to variable to store Device Type attribute value
    @param firmware_version - pointer to variable to store Firmware Version attribute value
    @param reflash_version - pointer to variable to store Reflash Version attribute value
    @param boot_count - pointer to variable to store Boot Count attribute value
    @param product_string - pointer to variable to store Product String attribute value
    @param access_point_node_ID - pointer to variable to store Access Point Node ID attribute value
    @param access_point_long_ID - pointer to variable to store Access Point Long ID attribute value
    @param access_point_cost - pointer to variable to store Access Point Cost ID attribute value
    @param mesh_channel - pointer to variable to store Mesh Channel attribute value
*/
#define ZB_ZCL_DECLARE_CONTROL4_NETWORKING_ATTRIB_LIST_SRV(attr_list, device_type, firmware_version,        \
  reflash_version, boot_count, product_string, access_point_node_ID, access_point_long_ID,                  \
  access_point_cost, mesh_channel)                                                                          \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_CONTROL4_NETWORKING)                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_DEVICE_TYPE_ID, (device_type))                       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_FIRMWARE_VERSION_ID, (firmware_version))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_REFLASH_VERSION_ID, (reflash_version))               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_BOOT_COUNT_ID, (boot_count))                         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_PRODUCT_STRING_ID, (product_string))                 \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_NODE_ID_ID, (access_point_node_ID))     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_LONG_ID_ID, (access_point_long_ID))     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_COST_ID, (access_point_cost))           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_MESH_CHANNEL_ID, (mesh_channel))                     \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @brief Declare extended attribute list for C4 Network cluster
    @param attr_list - attribute list name
    @param device_type - pointer to variable to store Device Type attribute value
    @param firmware_version - pointer to variable to store Firmware Version attribute value
    @param announce_window - pointer to variable to store Announce Window attribute value
    @param MTORR_period - pointer to variable to store MTORR Period attribute value
    @param reflash_version - pointer to variable to store Reflash Version attribute value
    @param boot_count - pointer to variable to store Boot Count attribute value
    @param product_string - pointer to variable to store Product String attribute value
    @param access_point_node_ID - pointer to variable to store Access Point Node ID attribute value
    @param access_point_long_ID - pointer to variable to store Access Point Long ID attribute value
    @param access_point_cost - pointer to variable to store Access Point Cost ID attribute value
    @param mesh_channel - pointer to variable to store Mesh Channel attribute value
    @param avg_RSSI - pointer to variable to store Avg RSSI attribute value
    @param avg_LQI - pointer to variable to store Avg LQI attribute value
    @param battery_level - pointer to variable to store Battery Level attribute value
    @param radio_4_bars - pointer to variable to store Radio 4-Bar attribute value
*/
#define ZB_ZCL_DECLARE_CONTROL4_NETWORKING_ATTRIB_LIST_SRV_EXT(attr_list, device_type, firmware_version,    \
  announce_window, MTORR_period,                                                                            \
  reflash_version, boot_count, product_string, access_point_node_ID, access_point_long_ID,                  \
  access_point_cost, mesh_channel,                                                                          \
  avg_RSSI, avg_LQI, battery_level, radio_4_bars)                                                           \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_CONTROL4_NETWORKING)                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_DEVICE_TYPE_ID, (device_type))                       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ANNOUNCE_WINDOW_ID, (announce_window))               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_MTORR_PERIOD_ID, (MTORR_period))                     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_FIRMWARE_VERSION_ID, (firmware_version))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_REFLASH_VERSION_ID, (reflash_version))               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_BOOT_COUNT_ID, (boot_count))                         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_PRODUCT_STRING_ID, (product_string))                 \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_NODE_ID_ID, (access_point_node_ID))     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_LONG_ID_ID, (access_point_long_ID))     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_COST_ID, (access_point_cost))           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_MESH_CHANNEL_ID, (mesh_channel))                     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_AVG_RSSI_ID, (avg_RSSI))                             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_AVG_LQI_ID, (avg_LQI))                               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_BATTERY_LEVEL_ID, (battery_level))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CONTROL4_NETWORKING_RADIO_4_BARS_ID, (radio_4_bars))                     \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /*Control4 Network cluster attributes */

/** @name Control4 Network cluster commands
 *  @{
 */
enum zb_zcl_control4_networking_cmd_e
{
  ZB_ZCL_CMD_CONTROL4_NETWORKING_IMMEDIATE_ANNOUNCE_ID     = 0x00 /**< "Immediate announce command" command. */
};

#define ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING_CLIENT_ROLE_GENERATED_CMD_LIST                     \
                               ZB_ZCL_CMD_CONTROL4_NETWORKING_IMMEDIATE_ANNOUNCE_ID              \

#define ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING_CLIENT_ROLE_GENERATED_CMD_LIST

/** @cond internals_doc */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_DEVICE_TYPE_ID(data_ptr)          \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_DEVICE_TYPE_ID,                                                \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_ANNOUNCE_WINDOW_ID(data_ptr)      \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_ANNOUNCE_WINDOW_ID,                                            \
  ZB_ZCL_ATTR_TYPE_U16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_MTORR_PERIOD_ID(data_ptr)         \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_MTORR_PERIOD_ID,                                               \
  ZB_ZCL_ATTR_TYPE_U16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_FIRMWARE_VERSION_ID(data_ptr)     \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_FIRMWARE_VERSION_ID,                                           \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_REFLASH_VERSION_ID(data_ptr)      \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_REFLASH_VERSION_ID,                                            \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_BOOT_COUNT_ID(data_ptr)           \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_BOOT_COUNT_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_U16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_PRODUCT_STRING_ID(data_ptr)       \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_PRODUCT_STRING_ID,                                             \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_NODE_ID_ID(data_ptr) \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_NODE_ID_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_LONG_ID_ID(data_ptr) \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_LONG_ID_ID,                                       \
  ZB_ZCL_ATTR_TYPE_IEEE_ADDR,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_COST_ID(data_ptr)    \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_ACCESS_POINT_COST_ID,                                          \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_MESH_CHANNEL_ID(data_ptr)         \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_MESH_CHANNEL_ID,                                               \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING,                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_AVG_RSSI_ID(data_ptr)             \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_AVG_RSSI_ID,                                                   \
  ZB_ZCL_ATTR_TYPE_S8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_AVG_LQI_ID(data_ptr)              \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_AVG_LQI_ID,                                                    \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_BATTERY_LEVEL_ID(data_ptr)        \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_BATTERY_LEVEL_ID,                                              \
  ZB_ZCL_ATTR_TYPE_S8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CONTROL4_NETWORKING_RADIO_4_BARS_ID(data_ptr)         \
{                                                                                                \
  ZB_ZCL_ATTR_CONTROL4_NETWORKING_RADIO_4_BARS_ID,                                               \
  ZB_ZCL_ATTR_TYPE_U8,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) data_ptr                                                                               \
}
/** @endcond */ /*internals_doc*/


/**
 * Control4 Network Cluster Endpoint.
 *
 * @attention This endpoint number is reserved for internal usage in the stack (for Control4 feature).
 */
#define ZB_CONTROL4_NETWORK_ENDPOINT 0xc4

#define ZB_ZCL_CONTROL4_NETWORK_DECLARE_CLUSTER_LIST(          \
  cluster_list_name,                                           \
  c4_network_attr_list)                                        \
  zb_zcl_cluster_desc_t cluster_list_name[] =                  \
  {                                                            \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING,                   \
      ZB_ZCL_ARRAY_SIZE(c4_network_attr_list, zb_zcl_attr_t),  \
      (c4_network_attr_list),                                  \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    ),                                                         \
    ZB_ZCL_CLUSTER_DESC(                                       \
      ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING,                   \
      0,                                                       \
      NULL,                                                    \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                              \
      ZB_ZCL_MANUF_CODE_INVALID                                \
    )                                                          \
  }

/*!
  @brief Declare simple descriptor for C4 Network Cluster endpoint
  @param ep_name - endpoint variable name
*/
#define ZB_ZCL_CONTROL4_NETWORK_DECLARE_SIMPLE_DESC(ep_name)   \
  /* ZB_DECLARE_SIMPLE_DESC(1, 1); it is already defined */    \
  ZB_AF_SIMPLE_DESC_TYPE(1, 1) simple_desc_##ep_name =         \
  {                                                            \
    ZB_CONTROL4_NETWORK_ENDPOINT,                              \
    ZB_AF_CONTROL4_PROFILE_ID,                                 \
    0, 1, 0, 1, 1,                                             \
    {                                                          \
      ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING,                   \
      ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING                    \
    }                                                          \
  }

/*!
  @brief Declare endpoint for C4 Network Cluster
  @param ep_name - endpoint variable name
  @param cluster_list - endpoint cluster list
 */
#define ZB_ZCL_CONTROL4_NETWORK_DECLARE_EP(ep_name, cluster_list)    \
  ZB_ZCL_CONTROL4_NETWORK_DECLARE_SIMPLE_DESC(ep_name);              \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                               \
    ZB_CONTROL4_NETWORK_ENDPOINT, ZB_AF_CONTROL4_PROFILE_ID,         \
    0,                                                               \
    NULL,                                                            \
    ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),          \
    cluster_list,                                                    \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                \
    0, NULL, 0, NULL)

/**
  Control4 Network Cluster states
*/
typedef enum zb_zcl_control4_network_state_e
{
  ZB_ZCL_CONTROL4_NETWORK_STATE_IDLE,
  ZB_ZCL_CONTROL4_NETWORK_STATE_SEARCHING_ZAP,
  ZB_ZCL_CONTROL4_NETWORK_STATE_RUNNING,
  ZB_ZCL_CONTROL4_NETWORK_STATE_STOPPED,
} zb_zcl_control4_network_state_t;

/**
  Control4 Zigbee Access Point (ZAP) discovery events
*/
typedef enum zb_zcl_control4_zap_discover_event_e
{
  /** Information from ZAP (NODE ID and/or LONG_NODE_ID) received and is different from the
   * current and default value. Bindings and/or reporting in the user application may require to be updated
   */
  ZB_ZCL_CONTROL4_NETWORK_ZAP_UPDATED,
  /** ZAP discovered failed after retrying ZB_ZCL_CONTROL4_NETWORK_SEARCHING_ZAP_ATTEMPTS times.
   *  User application may retry restart the Control4 network cluster or leave the network.
   */
  ZB_ZCL_CONTROL4_NETWORK_ZAP_DISCOVER_FAILED,
} zb_zcl_control4_zap_discover_event_t;

/** Control4 ZAP discover user app notification payload */
typedef struct zb_zcl_control4_zap_info_notify_s
{
  /** Event for user application to handle*/
  zb_zcl_control4_zap_discover_event_t event;
} zb_zcl_control4_zap_info_notify_t;

/**
  Start Control4 Network Cluster
*/
zb_ret_t zb_zcl_control4_network_cluster_start(void);

/**
  Stop Control4 Network Cluster
*/
void zb_zcl_control4_network_cluster_stop(void);

/**
  Get Control4 Network Cluster state
*/
zb_zcl_control4_network_state_t zb_zcl_control4_network_cluster_get_state(void);

/**
  Handle Read Attribute Response from Control4 Network Cluster
*/
void zb_zcl_control4_network_cluster_read_attr_resp_handler(zb_bufid_t param);
/** @} */ /*Control4 Network cluster commands */

/*! @} */ /* ZCL Control4 Network cluster */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

/**
  Control4 Network Cluster Initialization
*/
void zb_zcl_control4_networking_init_server(void);
void zb_zcl_control4_networking_init_client(void);

#define ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING_SERVER_ROLE_INIT zb_zcl_control4_networking_init_server
#define ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING_CLIENT_ROLE_INIT zb_zcl_control4_networking_init_client

#endif /* ZB_ZCL_CONTROL4_NETWORKING_H */
