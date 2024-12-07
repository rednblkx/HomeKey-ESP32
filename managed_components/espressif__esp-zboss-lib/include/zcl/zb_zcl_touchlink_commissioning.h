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
/* PURPOSE: Touchlink Commissioning cluster
*/

#if !defined (ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING_H)
#define ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING_H 1

#include <zboss_api_core.h> /* int types, zb_bufid_t */

/** @cond (DOXYGEN_ZCL_SECTION && DOXYGEN_TOUCHLINK_FEATURE) */

/** @addtogroup ZB_ZCL_TOUCHLINK_COMMISSIONING
    Touchlink Commissioning cluster
    @{
*/

/** @brief Default value for Touchlink Commissioning cluster revision global attribute */
#define ZB_ZCL_TOUCHLINK_COMMISSIONING_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/*!
  @brief Declare attribute list for Touchlink Commissioning cluster (only cluster revision attribute)
  @param attr_list - attribute list name
*/
#define ZB_ZCL_DECLARE_TOUCHLINK_COMMISSIONING_ATTR_LIST(attr_list)                            \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_TOUCHLINK_COMMISSIONING) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @brief First primary Touchlink channels mask. */
#define ZB_ZLL_PRIMARY_CHANNEL_MASK ((1l << 11) | (1l << 15) | (1l << 20) | (1l << 25))

/** @brief Configurable parameters: minimum address range that maybe assigned during commissioning */
#define ZB_ZLL_MIN_ADDR_RANGE 100
/** @brief Configurable parameters: default address range that is assigned during commissioning */
#define ZB_ZLL_DEFAULT_ADDR_RANGE 1000

/** @brief Configurable parameters: minimum group id range that maybe assigned during commissioning */
#define ZB_ZLL_MIN_GROUP_ID_RANGE 100
/** @brief Configurable parameters: default group id range that is assigned during commissioning */
#define ZB_ZLL_DEFAULT_GROUP_ID_RANGE 1000

/** @brief aplcInterPANTransIdLifetime constant value.
  * @see ZCL spec, subclause 13.3.4.2.
  */
#define ZB_ZLL_APLC_INTRP_TRANSID_LIFETIME ( ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(8000))

/** @brief aplcScanTimeBaseDuration constant value.
  * @see ZCL spec, subclause 13.3.4.2.
  */
#define ZB_ZLL_APLC_SCAN_TIME_BASE_DURATION ( ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(250))

/** @brief aplcRxWindowDuration constant value.
  * @see ZCL spec, subclause 13.3.4.2.
  */
#define ZB_ZLL_APLC_RX_WINDOW_DURATION (ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(5000))

/**
 *  @brief aplcMinStartupDelayTime constant value.
 *  @see ZCL spec, subclause 13.3.4.2.
 */
#define ZB_ZLL_APLC_MIN_STARTUP_DELAY_TIME  (ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(2000))

/** No attributes for reporting in Touchlink profile. */
#define ZB_ZLL_REPORT_ATTR_COUNT  0

/*! @name Touchlink Commissioning cluster internals
    Internal structures for Touchlink Commissioning cluster
    @cond internals_doc
    @internal
    @{
*/

/** @brief Touchlink commissioning general status is used in different commissioning
 * response command frames. */
typedef enum zb_zll_general_status_e
{
  ZB_ZLL_GENERAL_STATUS_SUCCESS,  /**< Status: OK. */
  ZB_ZLL_GENERAL_STATUS_FAILURE   /**< Status: Error. */
} zb_zll_general_status_t;

/**
 *  @brief Finish packet and send it to the designated IEEE address.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer containing packet.
 *  @param data_ptr [IN] - pointer to the packet tail.
 *  @param dst_addr_long [IN] - destination device's IEEE address.
 *  @param callback [IN] - callback to call upon send packet confirmation.
 *  @return packet send schedule status (see @ref zb_ret_t).
 */
zb_ret_t zb_zll_send_packet(zb_bufid_t buffer, zb_uint8_t* data_ptr, zb_ieee_addr_t dst_addr_long, zb_callback_t callback);
#define ZB_ZLL_SEND_PACKET(buffer, data_ptr, dst_addr_long, callback) \
  zb_zll_send_packet(buffer, data_ptr, dst_addr_long, callback)

/* TODO Provide normal battle-time implementation */
/** @brief Provides new response identifier. */
zb_uint32_t zb_zll_get_new_response_id(void);
#define ZB_ZLL_GET_NEW_RESPONSE_ID() zb_zll_get_new_response_id()

/* TODO Provide normal battle-time implementation */
/** @brief Provides new inter-PAN transaction identifier. */
zb_uint32_t zb_zll_get_new_trans_id(void);
#define ZB_ZLL_GET_NEW_TRANS_ID() zb_zll_get_new_trans_id()

/* TODO Remove internal API function calling */
/** @brief Get NIB Ext Pan ID */
zb_uint8_t* zb_zll_get_nib_ext_pan_id(void);

/* TODO Remove internal API function calling */
/** @brief Get NIB Update ID */
zb_uint8_t zb_zll_get_nib_update_id(void);

/*! @} 
    @endcond*/ /* Commissioning cluster internals */

/*! @name Touchlink Commissioning cluster commands
    @{
*/

/*! @brief Touchlink commissioning cluster command identifiers
    @see ZCL spec, subclauses 13.3.2.1
*/
enum zb_zll_commissioning_touchlink_cmd_e
{
  ZB_ZLL_CMD_COMMISSIONING_SCAN_REQ                    = 0x00,
  ZB_ZLL_CMD_COMMISSIONING_DEVICE_INFORMATION_REQ      = 0x02,
  ZB_ZLL_CMD_COMMISSIONING_IDENTIFY_REQ                = 0x06,
  ZB_ZLL_CMD_COMMISSIONING_RESET_TO_FACTORY_NEW_REQ    = 0x07,
  ZB_ZLL_CMD_COMMISSIONING_NETWORK_START_REQ           = 0x10,
  ZB_ZLL_CMD_COMMISSIONING_NETWORK_JOIN_ROUTER_REQ     = 0x12,
  ZB_ZLL_CMD_COMMISSIONING_NETWORK_JOIN_END_DEVICE_REQ = 0x14,
  ZB_ZLL_CMD_COMMISSIONING_NETWORK_UPDATE_REQ          = 0x16
};

/*! @brief  Touchlink commissioning cluster utility command identifiers
    @see ZCL spec, subclauses 13.3.2.1
*/
enum zb_zll_commissioning_utility_cmd_e
{
  ZB_ZLL_CMD_COMMISSIONING_GET_GROUP_IDENTIFIERS_REQUEST = 0x41,
  ZB_ZLL_CMD_COMMISSIONING_GET_ENDPOINT_LIST_REQUEST     = 0x42
};

/*! @brief Touchlink commissioning cluster response command identifiers
    @see ZCL spec, subclause 13.3.2.2
*/
enum zb_zll_commissioning_touchlink_cmd_resp_e
{
  ZB_ZLL_CMD_COMMISSIONING_SCAN_RES                    = 0x01,
  ZB_ZLL_CMD_COMMISSIONING_DEVICE_INFORMATION_RES      = 0x03,
  ZB_ZLL_CMD_COMMISSIONING_NETWORK_START_RES           = 0x11,
  ZB_ZLL_CMD_COMMISSIONING_NETWORK_JOIN_ROUTER_RES     = 0x13,
  ZB_ZLL_CMD_COMMISSIONING_NETWORK_JOIN_END_DEVICE_RES = 0x15
};

/*! @brief Touchlink commissioning cluster utility response command identifiers
    @see ZCL spec, subclause 13.3.2.2
*/
enum zb_zll_commissioning_utility_cmd_resp_e
{
  ZB_ZLL_CMD_COMMISSIONING_ENDPOINT_INFORMATION           = 0x40,
  ZB_ZLL_CMD_COMMISSIONING_GET_GROUP_IDENTIFIERS_RESPONSE = 0x41,
  ZB_ZLL_CMD_COMMISSIONING_GET_ENDPOINT_LIST_RESPONSE     = 0x42
};

/************************** Scan request command definitions ****************************/

/** @brief Parsed Scan request payload representation.
  * @see ZCL spec, subclause 13.3.2.1.1.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_scan_req_s
{
  /** @brief Inter-PAN transaction identifier
    * @see ZCL spec, subclause 13.3.2.1.1.1.
    */
  zb_uint32_t trans_id;
  /** @brief Zigbee information
    * @see ZCL spec, subclause 13.3.2.1.1.2.
    */
  zb_uint8_t zb_info;
  /** @brief Touchlink information
    * @see ZCL spec, subclause 13.3.2.1.1.3.
    */
  zb_uint8_t zll_info;
} ZB_PACKED_STRUCT zb_zll_commissioning_scan_req_t;

/** @brief Touchlink information bit values.
  * @see ZCL spec, subclause 13.3.2.1.1.3.
  */
enum zb_zll_info_e
{
  ZB_ZLL_INFO_FACTORY_NEW = 0x01,
  ZB_ZLL_INFO_ADDR_ASSIGNMENT = 0x02,
  ZB_ZLL_INFO_TOUCHLINK_INITIATOR = 0x10,
  ZB_ZLL_INFO_TOUCHLINK_PRIOR = 0x20
};

/** @brief Get Touchlink Info current value */
zb_uint8_t zb_zll_get_info_current_value(void);
#define ZB_ZLL_ZB_INFO_CURRENT_VALUE()  zb_zll_get_info_current_value()

/** @brief Check is Touchlink device factory new */
#define ZB_ZLL_DEVICE_INFO_GET_FACTORY_NEW(zll_info)  ((zll_info) & ZB_ZLL_INFO_FACTORY_NEW)

/** @brief Check is current Touchlink device factory new */
#define ZB_ZLL_IS_FACTORY_NEW() (ZB_ZLL_DEVICE_INFO_GET_FACTORY_NEW(ZLL_DEVICE_INFO().zll_info))

/** @brief Set current Touchlink device is factory new */
#define ZB_ZLL_SET_FACTORY_NEW() (ZLL_DEVICE_INFO().zll_info |= ZB_ZLL_INFO_FACTORY_NEW)

/** @brief Set current Touchlink device is not factory new */
#define ZB_ZLL_CLEAR_FACTORY_NEW() (ZLL_DEVICE_INFO().zll_info &= ~ZB_ZLL_INFO_FACTORY_NEW)

/** @brief Check is Touchlink device capable of address assignment */
#define ZB_ZLL_DEVICE_INFO_GET_ADDR_ASSIGNMENT(zll_info)  ((zll_info) & ZB_ZLL_INFO_ADDR_ASSIGNMENT)

/** @brief Check is current Touchlink device capable of address assignment */
#define ZB_ZLL_IS_ADDR_ASSIGNMENT() \
  (ZB_ZLL_DEVICE_INFO_GET_ADDR_ASSIGNMENT(ZLL_DEVICE_INFO().zll_info))

/** @brief Check is Touchlink device a link initiator */
#define ZB_ZLL_DEVICE_INFO_GET_LINK_INITIATOR(zll_info) \
  ((zll_info) & ZB_ZLL_INFO_TOUCHLINK_INITIATOR)

/** @brief Check is current Touchlink device a link initiator */
#define ZB_ZLL_IS_LINK_INITIATOR()  \
  (ZB_ZLL_DEVICE_INFO_GET_LINK_INITIATOR(ZLL_DEVICE_INFO().zll_info))

/** @brief Schedules Commissioning.ScanRequest sending.
  * @param buffer to use for packet creation.
  * @param callback to call upon packet confirmation.
  */
void zb_zll_commissioning_send_scan_req(zb_bufid_t buffer, zb_callback_t callback);
#define ZB_ZLL_COMMISSIONING_SEND_SCAN_REQ(buffer, callback)  \
  zb_zll_commissioning_send_scan_req(buffer, callback)

/** @brief Parses Commissioning.ScanRequest payload.
  * @attention Assumes that ZCL header was already cut.
  * @param data_ptr - pointer to a variable of type @ref zb_zll_commissioning_scan_req_s.
  * @param buffer with the Commissioning.ScanRequest payload.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_SCAN_REQ(data_ptr, buffer, status)    \
{                                                                      \
  zb_uint8_t* src_ptr = zb_buf_begin((buffer));                        \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_scan_req_t)) \
  {                                                                    \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                            \
  }                                                                    \
  else                                                                 \
  {                                                                    \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
    ZB_LETOH32(&((data_ptr)->trans_id), src_ptr);                      \
    src_ptr += sizeof(zb_uint32_t);                                    \
    (data_ptr)->zb_info = *(src_ptr++);                                \
    (data_ptr)->zll_info = *(src_ptr);                                 \
  }                                                                    \
}

/**
 *  @brief Endpoint information for parsed Commissioning.ScanResponse representation.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_ep_info_s
{
  /** @brief Endpoint identifier.
    *
    * This field shall only be present when the number of sub-devices field is equal to 1.
    */
  zb_uint8_t endpoint_id;
  /** @brief Profile identifier.
    *
    * This field shall only be present when the number of sub-devices field is equal to 1.
    */
  zb_uint16_t profile_id;
  /** @brief Device identifier.
    *
    * This field shall only be present when the number of sub-devices field is equal to 1.
    */
  zb_uint16_t device_id;
  /** @brief Version.
    *
    * This field shall only be present when the number of sub-devices field is equal to 1.
    */
  zb_uint8_t version;
  /** @brief Group identifiers count.
    *
    * This field shall only be present when the number of sub-devices field is equal to 1.
    */
  zb_uint8_t n_group_ids;
} ZB_PACKED_STRUCT zb_zll_commissioning_ep_info_t;

/** @brief Parsed Commissioning.ScanResponse structure.
  * @see ZCL spec, subclause 13.3.2.2.1.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_scan_res_s
{
  /** @brief Inter-PAN transaction identifier. */
  zb_uint32_t trans_id;
  /** @brief RSSI correction. */
  zb_uint8_t rssi_correction;
  /** @brief Zigbee device information. */
  zb_uint8_t zb_info;
  /** @brief Touchlink information. */
  zb_uint8_t zll_info;
  /** @brief Key bitmask. */
  zb_uint16_t key_bitmask;
  /** @brief Response identifier. */
  zb_uint32_t resp_id;
  /** @brief Extended Pan ID. */
  zb_ext_pan_id_t ext_pan_id;
  /** @brief Network update identifier. */
  zb_uint8_t nwk_upd_id;
  /** @brief Logical channel. */
  zb_uint8_t channel;
  /** @brief Pan ID. */
  zb_uint16_t pan_id;
  /** @brief Network address. */
  zb_uint16_t nwk_addr;
  /** @brief Number of sub-devices. */
  zb_uint8_t n_subdevs;
  /** @brief Total group identifiers. */
  zb_uint8_t group_ids_total;

  /**
   *  @brief Endpoint information.
   *  Shall be present if device has only single endpoint.
   */
  zb_zll_commissioning_ep_info_t endpoint_info;
} ZB_PACKED_STRUCT zb_zll_commissioning_scan_res_t;

/**
 *  @brief Schedules Commissioning.ScanResponse sending.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param seq_num [IN] - ZCL sequence number.
 *  @param callback [IN] - callback to call upon packet confirm.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
 */

/*
  AT: looks like that is a error, it completely breaks security subsystem for Touchlink commissioning,
  so i remove this code from ZB_ZLL_COMMISSIONING_SEND_SCAN_RES macro
  ZLL_DEVICE_INFO().response_id = ZLL_TRAN_CTX().transaction_id;
  TRACE_MSG(TRACE_ZLL1, "resp id %d", (FMT__P, ZLL_DEVICE_INFO().transaction_id));
  replaced by:
  ZLL_DEVICE_INFO().response_id = ZB_ZLL_GET_NEW_RESPONSE_ID();
  ZB_HTOLE32_VAL(&(scan_data->resp_id), ZLL_DEVICE_INFO().response_id);
*/

#define ZB_ZLL_COMMISSIONING_SEND_SCAN_RES(                                                           \
    buffer,                                                                                           \
    seq_num,                                                                                          \
    callback,                                                                                         \
    status)                                                                                           \
{                                                                                                     \
  zb_zll_commissioning_scan_res_t *scan_data;                                                         \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                                               \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(data_ptr);                                      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(data_ptr, (seq_num), ZB_ZLL_CMD_COMMISSIONING_SCAN_RES);            \
  scan_data = (zb_zll_commissioning_scan_res_t *)data_ptr;                                            \
  ZB_HTOLE32(&(scan_data->trans_id), &(ZLL_TRAN_CTX().transaction_id));                               \
  scan_data->rssi_correction = ZLL_DEVICE_INFO().rssi_correction;                                     \
  scan_data->zb_info = ZB_ZLL_ZB_INFO_CURRENT_VALUE();                                                \
  scan_data->zll_info = ZLL_DEVICE_INFO().zll_info;                                                   \
  ZB_HTOLE16(&(scan_data->key_bitmask), &(ZLL_DEVICE_INFO().key_info));                               \
  ZLL_DEVICE_INFO().response_id = ZB_ZLL_GET_NEW_RESPONSE_ID();                                       \
  ZB_HTOLE32_VAL(&(scan_data->resp_id), ZLL_DEVICE_INFO().response_id);                               \
  ZB_IEEE_ADDR_COPY(scan_data->ext_pan_id, zb_zll_get_nib_ext_pan_id());                              \
  scan_data->nwk_upd_id = (ZB_ZLL_IS_FACTORY_NEW() ? 0x00 : zb_zll_get_nib_update_id());              \
  scan_data->channel = ZB_PIBCACHE_CURRENT_CHANNEL();                                           \
  ZB_HTOLE16_VAL(&(scan_data->pan_id), ZB_PIBCACHE_PAN_ID());                                         \
  ZB_HTOLE16_VAL(&(scan_data->nwk_addr), ZB_PIBCACHE_NETWORK_ADDRESS());                              \
  scan_data->n_subdevs = ZCL_CTX().device_ctx->ep_count;                                              \
  scan_data->group_ids_total = ZLL_DEVICE_INFO().total_group_id_count;                                \
  if (ZCL_CTX().device_ctx->ep_count == 1)                                                            \
  {                                                                                                   \
    zb_af_endpoint_desc_t* descriptor = ZCL_CTX().device_ctx->ep_desc_list[0];                        \
    scan_data->endpoint_info.endpoint_id = descriptor->ep_id;                                         \
    ZB_HTOLE16(&(scan_data->endpoint_info.profile_id), &(descriptor->profile_id));                    \
    ZB_HTOLE16(&(scan_data->endpoint_info.device_id), &(descriptor->simple_desc->app_device_id));     \
    scan_data->endpoint_info.version = descriptor->simple_desc->app_device_version;                   \
    scan_data->endpoint_info.n_group_ids = descriptor->group_id_count;                                \
  }                                                                                                   \
  data_ptr += sizeof(zb_zll_commissioning_scan_res_t) -                                               \
    (ZCL_CTX().device_ctx->ep_count == 1 ? 0 : sizeof(zb_zll_commissioning_ep_info_t));               \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, ZLL_TRAN_CTX().src_addr, (callback)); \
}

/** @brief Parses Commissioning.ScanResponse.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref zb_zll_commissioning_scan_res_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_SCAN_RES(data_ptr, buffer, status)         \
{                                                                           \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_scan_res_t) &&    \
     zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_scan_res_t)-sizeof(zb_zll_commissioning_ep_info_t)) \
  {                                                                         \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                 \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    zb_zll_commissioning_scan_res_t *scan_data =                            \
      (zb_zll_commissioning_scan_res_t *)zb_buf_begin((buffer));            \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                 \
    ZB_LETOH32(&((data_ptr)->trans_id), &(scan_data->trans_id));            \
    (data_ptr)->rssi_correction = scan_data->rssi_correction;               \
    (data_ptr)->zb_info = scan_data->zb_info;                               \
    (data_ptr)->zll_info = scan_data->zll_info;                             \
    ZB_LETOH16(&((data_ptr)->key_bitmask), &(scan_data->key_bitmask));      \
    ZB_LETOH32(&((data_ptr)->resp_id), &(scan_data->resp_id));              \
    ZB_IEEE_ADDR_COPY((data_ptr)->ext_pan_id, scan_data->ext_pan_id);       \
    (data_ptr)->nwk_upd_id = scan_data->nwk_upd_id;                         \
    (data_ptr)->channel = scan_data->channel;                               \
    ZB_LETOH16(&((data_ptr)->pan_id), &(scan_data->pan_id));                \
    ZB_LETOH16(&((data_ptr)->nwk_addr), &(scan_data->nwk_addr));            \
    (data_ptr)->n_subdevs = scan_data->n_subdevs;                           \
    (data_ptr)->group_ids_total = scan_data->group_ids_total;               \
    if ((data_ptr)->n_subdevs == 1)                                         \
    {                                                                       \
      (data_ptr)->endpoint_info.endpoint_id = scan_data->endpoint_info.endpoint_id;                 \
      ZB_LETOH16(&((data_ptr)->endpoint_info.profile_id), &(scan_data->endpoint_info.profile_id));  \
      ZB_LETOH16(&((data_ptr)->endpoint_info.device_id), &(scan_data->endpoint_info.device_id));    \
      (data_ptr)->endpoint_info.version = scan_data->endpoint_info.version;                         \
      (data_ptr)->endpoint_info.n_group_ids = scan_data->endpoint_info.n_group_ids;                 \
                                                                                    \
      if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_scan_res_t))          \
      {                                                                             \
        (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
      }                                                                             \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      if (zb_buf_len((buffer)) !=                                                   \
          sizeof(zb_zll_commissioning_scan_res_t)-sizeof(zb_zll_commissioning_ep_info_t)) \
      {                                                                             \
        (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
      }                                                                \
    }                                                                  \
  }                                                                    \
}

/************************** Device information command definitions ****************************/

/** @brief Parsed Commissioning.DeviceInformationRequest payload.
  * @see ZCL spec, subclause 13.3.2.1.2.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_device_information_req_s
{
  /** @brief Inter-PAN transaction identifier. */
  zb_uint32_t trans_id;
  /** @brief Start index. */
  zb_uint8_t start_idx;
} ZB_PACKED_STRUCT zb_zll_commissioning_device_information_req_t;

/** @brief Schedules sending Commissioning.DeviceInformationRequest command.
  * @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
  * @param start_index [IN] - device index to start reporting with.
  * @param dst_addr [IN] - IEEE device address to send packet to.
  * @param callback [IN] - callback to schedule on packet send confirmation.
  * @param status [OUT] - packet schedule status (see @ref zb_ret_t).
  */
#define ZB_ZLL_COMMISSIONING_SEND_DEVICE_INFO_REQ(                          \
    buffer,                                                                 \
    start_index,                                                            \
    dst_addr,                                                               \
    callback,                                                               \
    status)                                                                 \
{                                                                           \
  zb_zll_commissioning_device_information_req_t *req;                       \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(data_ptr,           \
        ZB_ZCL_FRAME_DIRECTION_TO_SRV,                                      \
        ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                                   \
        ZB_ZCL_DISABLE_DEFAULT_RESPONSE);                                   \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                          \
      data_ptr,                                                             \
      ZB_ZCL_GET_SEQ_NUM(),                                                 \
      ZB_ZLL_CMD_COMMISSIONING_DEVICE_INFORMATION_REQ);                     \
  req = (zb_zll_commissioning_device_information_req_t*)data_ptr;           \
  ZB_HTOLE32(&(req->trans_id), &(ZLL_TRAN_CTX().transaction_id));           \
  req->start_idx= (start_index);                                            \
  data_ptr += sizeof(zb_zll_commissioning_device_information_req_t);        \
  status = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback)); \
}

/** @brief Parses Commissioning.DeviceInformationRequest.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_device_information_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_DEVICE_INFO_REQ(data_ptr, buffer, status)           \
{                                                                                    \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_device_information_req_t)) \
  {                                                                                  \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                          \
  }                                                                                  \
  else                                                                               \
  {                                                                                  \
    zb_zll_commissioning_device_information_req_t *req_data =                        \
      (zb_zll_commissioning_device_information_req_t*)zb_buf_begin((buffer));        \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                          \
    ZB_LETOH32(&((data_ptr)->trans_id), &(req_data->trans_id));                      \
    (data_ptr)->start_idx = req_data->start_idx;                                     \
  }                                                                                  \
}

/** @brief Parsed permanent part of the Commissioning.DeviceInformationResponse payload.
  * @see ZCL spec, subclause 13.3.2.2.2.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_device_information_common_res_s
{
  /** @brief Inter-PAN transaction identifier. */
  zb_uint32_t trans_id;
  /** @brief Number of sub-devices. */
  zb_uint8_t n_subdevs;
  /** @brief Start index. */
  zb_uint8_t start_idx;
  /** @brief Device information record count. */
  zb_uint8_t n_records;
} ZB_PACKED_STRUCT zb_zll_commissioning_device_information_common_res_t;

/** @brief Maximum number of device information records in the response. */
#define ZB_ZLL_COMMISSIONING_DEVICE_INFO_MAX_RECORDS  0x05

/** @brief Parsed device information record from the Commissioning.DeviceInformationResponse
  * payload.
  * @see ZCL spec, subclause 13.3.2.2.2.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_device_information_ep_info_res_s
{
  zb_ieee_addr_t ieee_addr;   /**< IEEE address of the sub-device. */
  zb_uint8_t ep_id;           /**< Device's endpoint identifier. */
  zb_uint16_t profile_id;     /**< Profile identifier. */
  zb_uint16_t device_id;      /**< Device identifier. */
  zb_uint8_t version;         /**< Device version. */
  zb_uint8_t group_id_count;  /**< Group identifier count. */
  zb_uint8_t sort;            /**< Sort. */
} ZB_PACKED_STRUCT zb_zll_commissioning_device_information_ep_info_res_t;

/** @brief Starts filling in Commissioning.DeviceInformationResponse command.
  * @param buffer to put packet to (by pointer).
  * @param data_ptr - pointer to contain address right after the response's permanent part's tail.
  * @param tsn - ZCL transaction sequence number.
  * @param start_index - index of the first reported device.
  * @param n_devs_reported - number of reported devices.
  */
#define ZB_ZLL_COMMISSIONING_INIT_DEVICE_INFO_RES(                     \
    buffer,                                                            \
    data_ptr,                                                          \
    tsn,                                                               \
    start_index,                                                       \
    n_devs_reported)                                                   \
{                                                                      \
  zb_zll_commissioning_device_information_common_res_t *req_data;      \
  data_ptr = ZB_ZCL_START_PACKET((buffer));                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL((data_ptr));     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                     \
      (data_ptr),                                                      \
      (tsn),                                                           \
      ZB_ZLL_CMD_COMMISSIONING_DEVICE_INFORMATION_RES);                \
  req_data = (zb_zll_commissioning_device_information_common_res_t*)data_ptr;   \
  ZB_HTOLE32(&(req_data->trans_id), &(ZLL_TRAN_CTX().transaction_id));          \
  req_data->n_subdevs = ZCL_CTX().device_ctx->ep_count;                         \
  req_data->start_idx = (start_index);                                          \
  req_data->n_records = (n_devs_reported);                                      \
  data_ptr += sizeof(zb_zll_commissioning_device_information_common_res_t);     \
}

/**
 *  @brief Add next device information record into Commissioning.DeviceInformationResponse.
 *  @param data_ptr [IN] - pointer to contain address right after the added record's tail.
 *  @param ep_id_ [IN] - endpoint identifier of the sub-device.
 *  @param profile [IN] - profile identifier of the sub-device.
 *  @param device [IN] - device identifier.
 *  @param version_ [IN] of the device.
 *  @param n_group_ids [IN] - group identifier count.
 *  @param sort_ [IN] - endpoint info sort flag.
 *  @attention According to ZCL spec, Device Information Response frame can contain not more than
 *  5 device information records. Caller should check this condition.
 */
#define ZB_ZLL_COMMISSIONING_ADD_EP_INFO_DEVICE_INFO_RES(                   \
    data_ptr,                                                               \
    ep_id_,                                                                 \
    profile,                                                                \
    device,                                                                 \
    version_,                                                               \
    n_group_ids,                                                            \
    sort_)                                                                  \
{                                                                           \
  zb_zll_commissioning_device_information_ep_info_res_t *req_data =         \
      (zb_zll_commissioning_device_information_ep_info_res_t*)data_ptr;     \
  ZB_IEEE_ADDR_COPY(req_data->ieee_addr, ZB_PIBCACHE_EXTENDED_ADDRESS());   \
  req_data->ep_id = (ep_id_);                                               \
  ZB_HTOLE16_VAL(&(req_data->profile_id), (profile));                       \
  ZB_HTOLE16_VAL(&(req_data->device_id), (device));                         \
  req_data->version = (version_);                                           \
  req_data->group_id_count = (n_group_ids);                                 \
  req_data->sort = (sort_);                                                 \
  data_ptr += sizeof(zb_zll_commissioning_device_information_ep_info_res_t);\
}

/**
 *  @brief Schedules Commissioning.DeviceInformationResponse for sending.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer containing payload.
 *  @param data_ptr [IN] - pointer to the payload's tail.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to call upon packet confirm.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
 */
#define ZB_ZLL_COMMISSIONING_SEND_DEVICE_INFO_RES(buffer, data_ptr, dst_addr, callback, status) \
  (status) = ZB_ZLL_SEND_PACKET(buffer, data_ptr, dst_addr, callback)

/** @brief Parses Commissioning.DeviceInformationResponse permanent part.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_device_information_common_res_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_DEVICE_INFO_RES(data_ptr, buffer, status)                    \
{                                                                                             \
  if (zb_buf_len((buffer)) < sizeof(zb_zll_commissioning_device_information_common_res_t))    \
  {                                                                                           \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                                   \
  }                                                                                           \
  else                                                                                        \
  {                                                                                           \
    zb_zll_commissioning_device_information_common_res_t* src_ptr =                           \
      (zb_zll_commissioning_device_information_common_res_t*)zb_buf_begin((buffer));          \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                   \
    ZB_LETOH32(&((data_ptr)->trans_id), &(src_ptr->trans_id));                                \
    (data_ptr)->n_subdevs = src_ptr->n_subdevs;                                               \
    (data_ptr)->start_idx = src_ptr->start_idx;                                               \
    (data_ptr)->n_records = src_ptr->n_records;                                               \
    (void)zb_buf_cut_left((buffer), sizeof(zb_zll_commissioning_device_information_common_res_t)); \
  }                                                                                           \
}

/** Parses next device description record in Commissioning.DeviceInformationResponse payload.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_device_information_ep_info_res_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_NEXT_EP_INFO_DEVICE_INFO_RES(data_ptr, buffer, status)        \
{                                                                                              \
  if (zb_buf_len((buffer)) < sizeof(zb_zll_commissioning_device_information_ep_info_res_t))    \
  {                                                                                            \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                                    \
  }                                                                                            \
  else                                                                                         \
  {                                                                                            \
    zb_zll_commissioning_device_information_ep_info_res_t* src_ptr =                           \
      (zb_zll_commissioning_device_information_ep_info_res_t*)zb_buf_begin((buffer));          \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                    \
    ZB_IEEE_ADDR_COPY((data_ptr)->ieee_addr, (src_ptr->ieee_addr));                            \
    (data_ptr)->ep_id = src_ptr->ep_id;                                                        \
    ZB_LETOH16(&((data_ptr)->profile_id), &(src_ptr->profile_id));                             \
    ZB_LETOH16(&((data_ptr)->device_id), &(src_ptr->device_id));                               \
    (data_ptr)->version = src_ptr->version;                                                    \
    (data_ptr)->group_id_count = src_ptr->group_id_count;                                      \
    (data_ptr)->sort = src_ptr->sort;                                                          \
    (void)zb_buf_cut_left((buffer), sizeof(zb_zll_commissioning_device_information_ep_info_res_t)); \
  }                                                                                            \
}

/************************** Identify request command definitions ****************************/

/**
 *  @brief Parsed Commissioning.IdentifyRequest payload.
 *  @see ZCL spec, subclause 13.3.2.1.3.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_identify_req_s
{
  /** @brief Inter-PAN transaction identifier. */
  zb_uint32_t trans_id;
  /** @brief Identify duration. */
  zb_uint16_t duration;
} ZB_PACKED_STRUCT zb_zll_commissioning_identify_req_t;

/**
 *  @brief Schedules sending Commissioning.IdentifyRequest command.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param p_identify_duration [IN] - period to identify.
 *  @param ieee_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
 */
#define ZB_ZLL_COMMISSIONING_SEND_IDENTIFY_REQ(                              \
    buffer,                                                                  \
    p_identify_duration,                                                     \
    ieee_addr,                                                               \
    callback,                                                                \
    status)                                                                  \
{                                                                            \
  zb_zll_commissioning_identify_req_t *req_data;                             \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_O(data_ptr, ZB_ZCL_DISABLE_DEFAULT_RESPONSE); \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                           \
      data_ptr,                                                              \
      ZB_ZCL_GET_SEQ_NUM(),                                                  \
      ZB_ZLL_CMD_COMMISSIONING_IDENTIFY_REQ);                                \
  req_data = (zb_zll_commissioning_identify_req_t*)data_ptr;                 \
  ZB_HTOLE32(&(req_data->trans_id), &(ZLL_TRAN_CTX().transaction_id));       \
  ZB_HTOLE16(&(req_data->duration), (p_identify_duration));                  \
  data_ptr += sizeof(zb_zll_commissioning_identify_req_t);                   \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (ieee_addr), (callback)); \
}

/** @brief Parses Commissioning.IdentifyRequest.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref zb_zll_commissioning_identify_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_IDENTIFY_REQ(data_ptr, buffer, status)    \
{                                                                          \
  /* FIXED */                                                              \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_identify_req_t)) \
  {                                                                        \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                \
  }                                                                        \
  else                                                                     \
  {                                                                        \
    zb_zll_commissioning_identify_req_t* src_ptr =                         \
      (zb_zll_commissioning_identify_req_t*)zb_buf_begin((buffer));        \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                \
    ZB_LETOH32(&((data_ptr)->trans_id), &(src_ptr->trans_id));             \
    ZB_LETOH16(&((data_ptr)->duration), &(src_ptr->duration));             \
  }                                                                        \
}

/**
 *  @brief Identify request parameters.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_identify_req_param_s
{
  zb_ieee_addr_t dst_addr;    /**< @brief Destination device address. */
  /**
   *  @brief Time to identify.
   *  Has special values:
   *  @li ZB_ZLL_IDENTIFY_STOP - instructs target to exit identify mode.
   *  @li ZB_ZLL_IDENTIFY_DEFAULT_TIME - instructs target to identify for a default period
   *  known by the receiver.
   */
  zb_uint16_t identify_time;
} ZB_PACKED_STRUCT zb_zll_commissioning_identify_req_param_t;

/**
 *  @brief "Exit identify mode" constant for @ref
 *  zb_zll_commissioning_identify_req_param_s "Identify request."
 */
#define ZB_ZLL_IDENTIFY_STOP  0

/**
 *  @brief "Identify for default period" constant for @ref
 *  zb_zll_commissioning_identify_req_param_s "Identify request."
 */
#define ZB_ZLL_IDENTIFY_DEFAULT_TIME  0xffff

/**
 *  @brief Fills in appropriate structures and starts identify request sending process.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param address [IN] - IEEE address of the target device.
 *  @param period [IN] - time to identify.
 *  @param status [OUT] - status of the operation.
 */
#define ZB_ZLL_COMMISSIONING_FILL_IDENTIFY_REQ(buffer, address, period, status) \
{                                                                               \
  zb_zll_commissioning_identify_req_param_t* request =                          \
      ZB_BUF_GET_PARAM((buffer), zb_zll_commissioning_identify_req_param_t);    \
  ZB_IEEE_ADDR_COPY(request->dst_addr, (address));                              \
  request->identify_time = (period);                                            \
  (status) = zb_zll_identify(ZB_REF_FROM_BUF((buffer)));                        \
}

/**
 *  @brief Extract Identify request parameters from the buffer.
 *  @param data_ptr [OUT] - pointer to a memory region to put request parameters to. Must be
 *  capable of storing value of type @ref zb_zll_commissioning_identify_req_param_s.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer containing request parameters (as
 *  a tail parameter).
 */
#define ZB_ZLL_COMMISSIONING_GET_IDENTIFY_REQ_PARAM(data_ptr, buffer)                \
{                                                                                    \
  zb_zll_commissioning_identify_req_param_t* tmp_ptr =                               \
      ZB_BUF_GET_PARAM((buffer), zb_zll_commissioning_identify_req_param_t);         \
  ZB_MEMCPY((data_ptr), tmp_ptr, sizeof(zb_zll_commissioning_identify_req_param_t)); \
}

/**
 *  @brief Sets up Identify request and schedules it for sending.
 *  Checks transaction state, identify period acceptability, fills in and schedules for sending
 *  command packet.
 *  @param param [IN] - reference to the packet buffer containing @ref
 *  zb_zll_commissioning_identify_req_param_s structure as parameter.
 *  @returns
 *  @li RET_OK in case of success. In this case application will be notified on further status
 *  (packet received, timed out, etc.) via zboss_signal_handler().
 *  @li RET_INVALID_STATE if identify time period doesn't correspond to the transaction state. I.e.
 *  either identify request was already sent, and identify period is non-zero, or request was not
 *  sent, and identify period is zero zus instructing the target to exit identify state.
 *  @li packet schedule error otherwise.
 */
zb_ret_t zb_zll_identify(zb_uint8_t param);

/************************** Reset to factory new command definitions ****************************/

/** @brief Parsed Commissioning.ResetToFactoryNewRequest payload.
  * @see ZCL spec, subclause 13.3.2.1.4.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_reset_to_fn_s
{
  /** @brief Inter-PAN transaction identifier. */
  zb_uint32_t trans_id;
} ZB_PACKED_STRUCT zb_zll_commissioning_reset_to_fn_t;

/**
 *  @brief Schedules sending Commissioning.ResetToFactoryNewRequest command.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
 */
#define ZB_ZLL_COMMISSIONING_SEND_RESET_TO_FN_REQ(                          \
    buffer,                                                                 \
    dst_addr,                                                               \
    callback,                                                               \
    status)                                                                 \
{                                                                           \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_O(data_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);       \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                          \
      data_ptr,                                                             \
      ZB_ZCL_GET_SEQ_NUM(),                                                 \
      ZB_ZLL_CMD_COMMISSIONING_RESET_TO_FACTORY_NEW_REQ);                   \
  ZB_ZCL_PACKET_PUT_DATA32_VAL(data_ptr, ZLL_TRAN_CTX().transaction_id);    \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback));       \
}

/** @brief Parses Commissioning.ResetToFactoryNewRequest.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref zb_zll_commissioning_reset_to_fn_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_RESET_TO_FN_REQ(data_ptr, buffer, status) \
{                                                                          \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_reset_to_fn_t))  \
  {                                                                        \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                \
  }                                                                        \
  else                                                                     \
  {                                                                        \
    zb_uint8_t* src_ptr = zb_buf_begin((buffer));                          \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                \
    ZB_ZCL_PACKET_GET_DATA32(&((data_ptr)->trans_id), (src_ptr));          \
  }                                                                        \
}

/************************** Network start command definitions ****************************/

/**
 *  @brief Parsed Commissioning.NetworkStartRequest payload.
 *  @see ZCL spec, subclause 13.3.2.1.5.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_network_start_req_s
{
  zb_uint32_t trans_id;             /**< Inter-PAN transaction identifier. */
  zb_ext_pan_id_t ext_pan_id;       /**< Extended Pan ID. */
  zb_uint8_t key_idx;               /**< Key index. */
  zb_uint8_t encr_nwk_key[16];      /**< Encrypted network key. */
  zb_uint8_t channel;               /**< Logical channel. */
  zb_uint16_t pan_id;               /**< Pan ID. */
  zb_uint16_t short_addr;           /**< Network address. */
  zb_uint16_t group_ids_begin;      /**< Group identifiers begin. */
  zb_uint16_t group_ids_end;        /**< Group identifiers end. */
  zb_uint16_t free_nwk_addrs_begin; /**< Free network address range begin. */
  zb_uint16_t free_nwk_addrs_end;   /**< Free network address range end. */
  zb_uint16_t free_group_ids_begin; /**< Free group identifiers range begin. */
  zb_uint16_t free_group_ids_end;   /**< Free group identifiers range end. */
  zb_ieee_addr_t initiator_ieee;    /**< Initiator IEEE address. */
  zb_uint16_t initiator_short;      /**< Initiator network address. */
} ZB_PACKED_STRUCT zb_zll_commissioning_network_start_req_t;

/**
 *  @brief Schedules sending Commissioning.NetworkStartRequest command.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param ext_pan_id_ [IN] - extended Pan ID of the new network.
 *  @param key_index [IN] - index of the field to be used in the encrypted network key field.
 *  @param encrypted_nwk_key [IN] - encrypted network key.
 *  @param logical_channel [IN] - Touchlink channel to be used for the new network.
 *  @param pan_id_ [IN] - new network Pan ID.
 *  @param nwk_addr [IN] - short network address for the recipient.
 *  @param grp_ids_begin [IN] - group identifiers begin field.
 *  @param grp_ids_end [IN] - group identifiers end field.
 *  @param free_nwk_addrs_begin_ [IN] - free network addresses begin field.
 *  @param free_nwk_addrs_end_ [IN] - free network addresses end field.
 *  @param free_group_ids_begin_ [IN] - free group identifiers begin field.
 *  @param free_group_ids_end_ [IN] - free group identifiers end field.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
 */
#define ZB_ZLL_COMMISSIONING_SEND_NETWORK_START_REQ(                        \
    buffer,                                                                 \
    ext_pan_id_,                                                            \
    key_index,                                                              \
    encrypted_nwk_key,                                                      \
    logical_channel,                                                        \
    pan_id_,                                                                \
    nwk_addr,                                                               \
    grp_ids_begin,                                                          \
    grp_ids_end,                                                            \
    free_nwk_addrs_begin_,                                                  \
    free_nwk_addrs_end_,                                                    \
    free_group_ids_begin_,                                                  \
    free_group_ids_end_,                                                    \
    dst_addr,                                                               \
    callback,                                                               \
    status)                                                                 \
{                                                                           \
  zb_zll_commissioning_network_start_req_t *req_data;                       \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(                    \
      data_ptr,                                                             \
      ZB_ZCL_FRAME_DIRECTION_TO_SRV,                                        \
      ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                                     \
      ZB_ZCL_DISABLE_DEFAULT_RESPONSE);                                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                          \
      data_ptr,                                                             \
      ZB_ZCL_GET_SEQ_NUM(),                                                 \
      ZB_ZLL_CMD_COMMISSIONING_NETWORK_START_REQ);                          \
  /* do not use PACKET_PUT... define, get memory from buffer \
   * once for type zb_zll_commissioning_network_start_req_t, use struct->field to fill in all the values \
   * use this approach for ALL the cases */                                     \
  req_data = (zb_zll_commissioning_network_start_req_t*)data_ptr;               \
  ZB_HTOLE32(&(req_data->trans_id), &(ZLL_TRAN_CTX().transaction_id));          \
  ZB_IEEE_ADDR_COPY(req_data->ext_pan_id, (ext_pan_id_));                       \
  req_data->key_idx = (key_index);                                              \
  ZB_MEMCPY(req_data->encr_nwk_key, (encrypted_nwk_key), 16);                   \
  req_data->channel = (logical_channel);                                        \
  ZB_HTOLE16_VAL(&(req_data->pan_id), (pan_id_));                               \
  ZB_HTOLE16_VAL(&(req_data->short_addr), (nwk_addr));                          \
  ZB_HTOLE16_VAL(&(req_data->group_ids_begin), (grp_ids_begin));                \
  ZB_HTOLE16_VAL(&(req_data->group_ids_end), (grp_ids_end));                    \
  ZB_HTOLE16_VAL(&(req_data->free_nwk_addrs_begin), (free_nwk_addrs_begin_));   \
  ZB_HTOLE16_VAL(&(req_data->free_nwk_addrs_end), (free_nwk_addrs_end_));       \
  ZB_HTOLE16_VAL(&(req_data->free_group_ids_begin), (free_group_ids_begin_));   \
  ZB_HTOLE16_VAL(&(req_data->free_group_ids_end), (free_group_ids_end_));       \
  ZB_IEEE_ADDR_COPY(req_data->initiator_ieee, ZB_PIBCACHE_EXTENDED_ADDRESS());  \
  ZB_HTOLE16_VAL(&(req_data->initiator_short), ZB_PIBCACHE_NETWORK_ADDRESS());  \
  data_ptr += sizeof(zb_zll_commissioning_network_start_req_t);                 \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback));    \
}

/**
 *  @brief Parses Commissioning.NetworkStartRequest.
 *  @attention Assumes that ZCL header already cut.
 *  @param data_ptr [OUT] - pointer to a variable of type @ref
 *  zb_zll_commissioning_network_start_req_s.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer containing the packet.
 *  @param status [OUT] - variable to put parse status to (see @ref zb_zcl_parse_status_t).
 */
#define ZB_ZLL_COMMISSIONING_GET_NETWORK_START_REQ(data_ptr, buffer, status)            \
{                                                                                       \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_network_start_req_t))         \
  {                                                                                     \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                             \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    zb_zll_commissioning_network_start_req_t *src_ptr =                                 \
        (zb_zll_commissioning_network_start_req_t*)zb_buf_begin((buffer));              \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                             \
    ZB_LETOH32(&((data_ptr)->trans_id), &(src_ptr->trans_id));                          \
    ZB_IEEE_ADDR_COPY((data_ptr)->ext_pan_id, src_ptr->ext_pan_id);                     \
    (data_ptr)->key_idx = src_ptr->key_idx;                                             \
    ZB_MEMCPY((data_ptr)->encr_nwk_key, src_ptr->encr_nwk_key, 16);                     \
    (data_ptr)->channel  = src_ptr->channel;                                            \
    ZB_LETOH16(&((data_ptr)->pan_id), &(src_ptr->pan_id));                              \
    ZB_LETOH16(&((data_ptr)->short_addr), &(src_ptr->short_addr));                      \
    ZB_LETOH16(&((data_ptr)->group_ids_begin), &(src_ptr->group_ids_begin));            \
    ZB_LETOH16(&((data_ptr)->group_ids_end), &(src_ptr->group_ids_end));                \
    ZB_LETOH16(&((data_ptr)->free_nwk_addrs_begin), &(src_ptr->free_nwk_addrs_begin));  \
    ZB_LETOH16(&((data_ptr)->free_nwk_addrs_end), &(src_ptr->free_nwk_addrs_end));      \
    ZB_LETOH16(&((data_ptr)->free_group_ids_begin), &(src_ptr->free_group_ids_begin));  \
    ZB_LETOH16(&((data_ptr)->free_group_ids_end), &(src_ptr->free_group_ids_end));      \
    ZB_IEEE_ADDR_COPY((data_ptr)->initiator_ieee, (src_ptr->initiator_ieee));           \
    ZB_LETOH16(&((data_ptr)->initiator_short), &(src_ptr->initiator_short));            \
  }                                                                                     \
}

/**
 *  @brief Parsed Commissioning.NetworkStartResponse payload.
 *  @see ZLL spec, subclause 13.3.2.2.3.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_network_start_res_s
{
  zb_uint32_t trans_id;       /**< Inter-PAN transaction identifier. */
  zb_uint8_t status;          /**< Status. */
  zb_ext_pan_id_t ext_pan_id; /**< Extended Pan ID. */
  zb_uint8_t nwk_upd_id;      /**< Network update identifier. */
  zb_uint8_t channel;         /**< Logical channel. */
  zb_uint16_t pan_id;         /**< Pqn ID. */
} ZB_PACKED_STRUCT zb_zll_commissioning_network_start_res_t;

/**
 *  @brief The only acceptable value for network update identifier field in the network start
 *  response.
 *  @see ZCL spec, subclause 13.3.2.2.3.4.
 */
#define ZB_ZLL_NETWORK_UPDATE_ID  ((zb_uint8_t)0)

/**
 *  @brief Schedules Commissioning.NetworkStartResponse sending.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer containing payload.
 *  @param seq_num [IN] - ZCL transaction sequence number.
 *  @param nwk_status [IN] - network start request result.
 *  @param ext_pan_id_ [IN] - extended Pan ID of the network to be started.
 *  @param pan_id_ [IN] - Pan ID of the network to be started.
 *  @param channel_ [IN] - channel number network will be started on.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status_ [OUT] - packet send schedule status.
 */
#define ZB_ZLL_COMMISSIONING_SEND_NETWORK_START_RES(                                       \
    buffer,                                                                                \
    seq_num,                                                                               \
    nwk_status,                                                                            \
    ext_pan_id_,                                                                           \
    pan_id_,                                                                               \
    channel_,                                                                              \
    dst_addr,                                                                              \
    callback,                                                                              \
    status_)                                                                               \
{                                                                                          \
  zb_zll_commissioning_network_start_res_t *req_data;                                      \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(data_ptr);                           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(data_ptr, (seq_num),                                     \
            ZB_ZLL_CMD_COMMISSIONING_NETWORK_START_RES);                                   \
  req_data = (zb_zll_commissioning_network_start_res_t*)data_ptr;                          \
  ZB_HTOLE32(&(req_data->trans_id), &(ZLL_TRAN_CTX().transaction_id));                     \
  req_data->status = (nwk_status);                                                         \
  ZB_IEEE_ADDR_COPY(req_data->ext_pan_id, (ext_pan_id_));                                  \
  req_data->nwk_upd_id = ZB_ZLL_NETWORK_UPDATE_ID;                                         \
  req_data->channel = (channel_);                                                          \
  ZB_HTOLE16_VAL(&(req_data->pan_id), (pan_id_));                                          \
  data_ptr += sizeof(zb_zll_commissioning_network_start_res_t);                            \
  (status_) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback));              \
}

/**
 * @brief Parses Commissioning.NetworkStartResponse.
 * @attention Assumes that ZCL header already cut.
 * @param data_ptr [OUT] - pointer to a variable of type @ref
 * zb_zll_commissioning_network_start_res_s that will contain parsed information.
 * @param buffer [IN] - index @ref zb_bufid_t of the buffer containing the packet.
 * @param op_status [OUT] - variable to put parse status to (see @ref zb_zcl_parse_status_t).
 */
#define ZB_ZLL_COMMISSIONING_GET_NETWORK_START_RES(data_ptr, buffer, op_status) \
{                                                                               \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_network_start_res_t)) \
  {                                                                             \
    (op_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                  \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    zb_zll_commissioning_network_start_res_t *src_ptr =                         \
        (zb_zll_commissioning_network_start_res_t*)zb_buf_begin((buffer));      \
    (op_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                  \
    ZB_LETOH32(&((data_ptr)->trans_id), &(src_ptr->trans_id));                  \
    (data_ptr)->status = src_ptr->status;                                       \
    ZB_IEEE_ADDR_COPY((data_ptr)->ext_pan_id, src_ptr->ext_pan_id);             \
    (data_ptr)->nwk_upd_id = src_ptr->nwk_upd_id;                               \
    (data_ptr)->channel = src_ptr->channel;                                     \
    ZB_LETOH16(&((data_ptr)->pan_id), &(src_ptr->pan_id));                      \
  }                                                                             \
}

/************************** Network join router command definitions ****************************/

/**
 * @brief Parsed Commissioning.NetworkJoinRouterRequest payload.
 * @see ZCL spec, subclause 13.3.2.1.6.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_network_join_router_req_s
{
  zb_uint32_t trans_id;             /**< Inter-PAN transaction identifier. */
  zb_ext_pan_id_t ext_pan_id;       /**< Extended Pan ID. */
  zb_uint8_t key_idx;               /**< Key index. */
  zb_uint8_t encr_nwk_key[16];      /**< Encrypted network key. */
  zb_uint8_t nwk_upd_id;            /**< Network update identifier. */
  zb_uint8_t channel;               /**< Logical channel. */
  zb_uint16_t pan_id;               /**< Pan ID. */
  zb_uint16_t short_addr;           /**< Network address. */
  zb_uint16_t group_ids_begin;      /**< Group identifiers begin. */
  zb_uint16_t group_ids_end;        /**< Group identifiers end. */
  zb_uint16_t free_nwk_addrs_begin; /**< Free network address range begin. */
  zb_uint16_t free_nwk_addrs_end;   /**< Free network address range end. */
  zb_uint16_t free_group_ids_begin; /**< Free group identifiers range begin. */
  zb_uint16_t free_group_ids_end;   /**< Free group identifiers range end. */
} ZB_PACKED_STRUCT zb_zll_commissioning_network_join_router_req_t;

/**
 *  @brief Schedules sending Commissioning.NetworkJoinRouterRequest command.
 *  @param buffer to put packet to (by pointer).
 *  @param key_index [IN] - index of the field to be used in the encrypted network key field.
 *  @param encrypted_nwk_key [IN] - encrypted network key.
 *  @param nwk_addr [IN] - short network address for the recipient.
 *  @param grp_ids_begin [IN] - group identifiers begin field.
 *  @param grp_ids_end [IN] - group identifiers end field.
 *  @param free_nwk_addrs_begin_ [IN] - free network addresses begin field.
 *  @param free_nwk_addrs_end_ [IN] - free network addresses end field.
 *  @param free_group_ids_begin_ [IN] - free group identifiers begin field.
 *  @param free_group_ids_end_ [IN] - free group identifiers end field.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
  */
#define ZB_ZLL_COMMISSIONING_SEND_NETWORK_JOIN_ROUTER_REQ(                    \
    buffer,                                                                   \
    key_index,                                                                \
    encrypted_nwk_key,                                                        \
    nwk_addr,                                                                 \
    grp_ids_begin,                                                            \
    grp_ids_end,                                                              \
    free_nwk_addrs_begin_,                                                    \
    free_nwk_addrs_end_,                                                      \
    free_group_ids_begin_,                                                    \
    free_group_ids_end_,                                                      \
    dst_addr,                                                                 \
    callback,                                                                 \
    status)                                                                   \
{                                                                             \
  zb_zll_commissioning_network_join_router_req_t* req_data;                   \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET(buffer);                         \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(data_ptr,            \
      ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC);       \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                            \
      data_ptr,                                                               \
      ZB_ZCL_GET_SEQ_NUM(),                                                   \
      ZB_ZLL_CMD_COMMISSIONING_NETWORK_JOIN_ROUTER_REQ);                      \
  req_data = (zb_zll_commissioning_network_join_router_req_t*)data_ptr;       \
  ZB_HTOLE32(&(req_data->trans_id), &(ZLL_TRAN_CTX().transaction_id));        \
  ZB_IEEE_ADDR_COPY(req_data->ext_pan_id, zb_zll_get_nib_ext_pan_id());       \
  req_data->key_idx = (key_index);                                            \
  ZB_MEMCPY(req_data->encr_nwk_key, (encrypted_nwk_key), 16);                 \
  req_data->nwk_upd_id = zb_zll_get_nib_update_id();                          \
  req_data->channel = (ZLL_DEVICE_INFO().nwk_channel) ?                       \
    ZLL_DEVICE_INFO().nwk_channel : ZB_PIBCACHE_CURRENT_CHANNEL();      \
  ZB_HTOLE16_VAL(&(req_data->pan_id), ZB_PIBCACHE_PAN_ID());                  \
  ZB_HTOLE16_VAL(&(req_data->short_addr), (nwk_addr));                        \
  ZB_HTOLE16_VAL(&(req_data->group_ids_begin), (grp_ids_begin));              \
  ZB_HTOLE16_VAL(&(req_data->group_ids_end), (grp_ids_end));                  \
  ZB_HTOLE16_VAL(&(req_data->free_nwk_addrs_begin), (free_nwk_addrs_begin_)); \
  ZB_HTOLE16_VAL(&(req_data->free_nwk_addrs_end), (free_nwk_addrs_end_));     \
  ZB_HTOLE16_VAL(&(req_data->free_group_ids_begin), (free_group_ids_begin_)); \
  ZB_HTOLE16_VAL(&(req_data->free_group_ids_end), (free_group_ids_end_));     \
  data_ptr += sizeof(zb_zll_commissioning_network_join_router_req_t);         \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback));  \
}

/** @brief Parses Commissioning.NetworkJoinRouterRequest.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_network_join_router_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_NETWORK_JOIN_ROUTER_REQ(data_ptr, buffer, status)    \
{                                                                                     \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_network_join_router_req_t)) \
  {                                                                                   \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                           \
  }                                                                                   \
  else                                                                                \
  {                                                                                   \
    zb_zll_commissioning_network_join_router_req_t *src_ptr =                           \
        (zb_zll_commissioning_network_join_router_req_t*)zb_buf_begin((buffer));        \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                             \
    ZB_LETOH32(&((data_ptr)->trans_id), &(src_ptr->trans_id));                          \
    ZB_IEEE_ADDR_COPY((data_ptr)->ext_pan_id, src_ptr->ext_pan_id);                     \
    (data_ptr)->key_idx = src_ptr->key_idx;                                             \
    ZB_MEMCPY((data_ptr)->encr_nwk_key, src_ptr->encr_nwk_key, 16);                     \
    (data_ptr)->nwk_upd_id = src_ptr->nwk_upd_id;                                       \
    (data_ptr)->channel  = src_ptr->channel;                                            \
    ZB_LETOH16(&((data_ptr)->pan_id), &(src_ptr->pan_id));                              \
    ZB_LETOH16(&((data_ptr)->short_addr), &(src_ptr->short_addr));                      \
    ZB_LETOH16(&((data_ptr)->group_ids_begin), &(src_ptr->group_ids_begin));            \
    ZB_LETOH16(&((data_ptr)->group_ids_end), &(src_ptr->group_ids_end));                \
    ZB_LETOH16(&((data_ptr)->free_nwk_addrs_begin), &(src_ptr->free_nwk_addrs_begin));  \
    ZB_LETOH16(&((data_ptr)->free_nwk_addrs_end), &(src_ptr->free_nwk_addrs_end));      \
    ZB_LETOH16(&((data_ptr)->free_group_ids_begin), &(src_ptr->free_group_ids_begin));  \
    ZB_LETOH16(&((data_ptr)->free_group_ids_end), &(src_ptr->free_group_ids_end));      \
  }                                                                                     \
}

/**
 *  @brief Parsed Commissioning.NetworkJoinRouterResponse payload.
 *  @see ZLL spec, subclause 13.3.2.2.4.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_network_join_router_res_s
{
  zb_uint32_t trans_id; /**< Inter-PAN transaction identifier. */
  zb_uint8_t status;    /**< Status. */
} ZB_PACKED_STRUCT zb_zll_commissioning_network_join_router_res_t;

/**
 *  @brief Schedules sending Commissioning.NetworkJoinRouterResponse command.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param nwk_status [IN] - network join router operation status.
 *  @param tsn [IN] - ZCL transaction sequence number.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
  */
#define ZB_ZLL_COMMISSIONING_SEND_NETWORK_JOIN_ROUTER_RES(                  \
    buffer,                                                                 \
    nwk_status,                                                             \
    tsn,                                                                    \
    dst_addr,                                                               \
    callback,                                                               \
    status)                                                                 \
{                                                                           \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET(buffer);                       \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(data_ptr);            \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                          \
      data_ptr,                                                             \
      (tsn),                                                                \
      ZB_ZLL_CMD_COMMISSIONING_NETWORK_JOIN_ROUTER_RES);                    \
  ZB_ZCL_PACKET_PUT_DATA32_VAL(data_ptr, ZLL_TRAN_CTX().transaction_id);    \
  ZB_ZCL_PACKET_PUT_DATA8(data_ptr, (nwk_status));                          \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback)); \
}

/** @brief Parses Commissioning.NetworkJoinRouterResponse.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_network_join_router_res_s.
  * @param buffer containing the packet (by pointer).
  * @param op_status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_NETWORK_JOIN_ROUTER_RES(data_ptr, buffer, op_status) \
{                                                                                     \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_network_join_router_res_t)) \
  {                                                                                   \
    (op_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                        \
  }                                                                                   \
  else                                                                                \
  {                                                                                   \
    zb_uint8_t* src_ptr = zb_buf_begin((buffer));                                     \
    (op_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                        \
    ZB_ZCL_PACKET_GET_DATA32(&((data_ptr)->trans_id), (src_ptr));                     \
    ZB_ZCL_PACKET_GET_DATA8(&((data_ptr)->status), (src_ptr));                        \
  }                                                                                   \
}

/************************* Network join end device command definitions ***************************/

/** @brief Parsed Commissioning.NetworkJoinEndDeviceRequest payload.
  * @see ZCL spec, subclause 13.3.2.1.7.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_network_join_end_device_req_s
{
  zb_uint32_t trans_id;             /**< Inter-PAN transaction identifier. */
  zb_ext_pan_id_t ext_pan_id;       /**< Extended Pan ID. */
  zb_uint8_t key_idx;               /**< Key index. */
  zb_uint8_t encr_nwk_key[16];      /**< Encrypted network key. */
  zb_uint8_t nwk_upd_id;            /**< Network update identifier. */
  zb_uint8_t channel;               /**< Logical channel. */
  zb_uint16_t pan_id;               /**< Pan ID. */
  zb_uint16_t short_addr;           /**< Network address. */
  zb_uint16_t group_ids_begin;      /**< Group identifiers begin. */
  zb_uint16_t group_ids_end;        /**< Group identifiers end. */
  zb_uint16_t free_nwk_addrs_begin; /**< Free network address range begin. */
  zb_uint16_t free_nwk_addrs_end;   /**< Free network address range end. */
  zb_uint16_t free_group_ids_begin; /**< Free group identifiers range begin. */
  zb_uint16_t free_group_ids_end;   /**< Free group identifiers range end. */
} ZB_PACKED_STRUCT zb_zll_commissioning_network_join_end_device_req_t;

/**
 *  @brief Schedules sending Commissioning.NetworkJoinEndDeviceRequest command.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param key_index [IN] - index of the field to be used in the encrypted network key field.
 *  @param encrypted_nwk_key [IN] - encrypted network key.
 *  @param logical_channel [IN] - Touchlink channel to be used for the new network.
 *  @param nwk_addr [IN] - short network address for the recipient.
 *  @param grp_ids_begin [IN] - group identifiers begin field.
 *  @param grp_ids_end [IN] - group identifiers end field.
 *  @param free_nwk_addrs_begin_ [IN] - free network addresses begin field.
 *  @param free_nwk_addrs_end_ [IN] - free network addresses end field.
 *  @param free_group_ids_begin_ [IN] - free group identifiers begin field.
 *  @param free_group_ids_end_ [IN] - free group identifiers end field.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
 */
#define ZB_ZLL_COMMISSIONING_SEND_NETWORK_JOIN_ED_REQ(                      \
    buffer,                                                                 \
    key_index,                                                              \
    encrypted_nwk_key,                                                      \
    logical_channel,                                                        \
    nwk_addr,                                                               \
    grp_ids_begin,                                                          \
    grp_ids_end,                                                            \
    free_nwk_addrs_begin_,                                                  \
    free_nwk_addrs_end_,                                                    \
    free_group_ids_begin_,                                                  \
    free_group_ids_end_,                                                    \
    dst_addr,                                                               \
    callback,                                                               \
    status)                                                                 \
{                                                                           \
  zb_zll_commissioning_network_join_end_device_req_t *req_data;             \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET(buffer);                       \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(data_ptr);            \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                          \
      data_ptr,                                                             \
      ZB_ZCL_GET_SEQ_NUM(),                                                 \
      ZB_ZLL_CMD_COMMISSIONING_NETWORK_JOIN_END_DEVICE_REQ);                \
  req_data = (zb_zll_commissioning_network_join_end_device_req_t*)data_ptr; \
  ZB_HTOLE32(&(req_data->trans_id), &(ZLL_TRAN_CTX().transaction_id));      \
  ZB_IEEE_ADDR_COPY(req_data->ext_pan_id, zb_zll_get_nib_ext_pan_id());     \
  req_data->key_idx = (key_index);                                            \
  ZB_MEMCPY(req_data->encr_nwk_key, (encrypted_nwk_key), 16);                 \
  req_data->nwk_upd_id = zb_zll_get_nib_update_id();                          \
  req_data->channel = (logical_channel);                                      \
  ZB_HTOLE16_VAL(&(req_data->pan_id), ZB_PIBCACHE_PAN_ID());                  \
  ZB_HTOLE16_VAL(&(req_data->short_addr), (nwk_addr));                        \
  ZB_HTOLE16_VAL(&(req_data->group_ids_begin), (grp_ids_begin));              \
  ZB_HTOLE16_VAL(&(req_data->group_ids_end), (grp_ids_end));                  \
  ZB_HTOLE16_VAL(&(req_data->free_nwk_addrs_begin), (free_nwk_addrs_begin_)); \
  ZB_HTOLE16_VAL(&(req_data->free_nwk_addrs_end), (free_nwk_addrs_end_));     \
  ZB_HTOLE16_VAL(&(req_data->free_group_ids_begin), (free_group_ids_begin_)); \
  ZB_HTOLE16_VAL(&(req_data->free_group_ids_end), (free_group_ids_end_));     \
  data_ptr += sizeof(zb_zll_commissioning_network_join_end_device_req_t);     \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback));   \
}

/** @brief Parses Commissioning.NetworkJoinEndDeviceRequest.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_network_join_end_device_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_NETWORK_JOIN_ED_REQ(data_ptr, buffer, status)            \
{                                                                                         \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_network_join_end_device_req_t)) \
  {                                                                                       \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                               \
  }                                                                                       \
  else                                                                                    \
  {                                                                                       \
    zb_zll_commissioning_network_join_end_device_req_t *src_ptr =                         \
        (zb_zll_commissioning_network_join_end_device_req_t*)zb_buf_begin((buffer));      \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                               \
    ZB_LETOH32(&((data_ptr)->trans_id), &(src_ptr->trans_id));                            \
    ZB_IEEE_ADDR_COPY((data_ptr)->ext_pan_id, src_ptr->ext_pan_id);                       \
    (data_ptr)->key_idx = src_ptr->key_idx;                                               \
    ZB_MEMCPY((data_ptr)->encr_nwk_key, src_ptr->encr_nwk_key, 16);                       \
    (data_ptr)->nwk_upd_id = src_ptr->nwk_upd_id;                                         \
    (data_ptr)->channel  = src_ptr->channel;                                              \
    ZB_LETOH16(&((data_ptr)->pan_id), &(src_ptr->pan_id));                                \
    ZB_LETOH16(&((data_ptr)->short_addr), &(src_ptr->short_addr));                        \
    ZB_LETOH16(&((data_ptr)->group_ids_begin), &(src_ptr->group_ids_begin));              \
    ZB_LETOH16(&((data_ptr)->group_ids_end), &(src_ptr->group_ids_end));                  \
    ZB_LETOH16(&((data_ptr)->free_nwk_addrs_begin), &(src_ptr->free_nwk_addrs_begin));    \
    ZB_LETOH16(&((data_ptr)->free_nwk_addrs_end), &(src_ptr->free_nwk_addrs_end));        \
    ZB_LETOH16(&((data_ptr)->free_group_ids_begin), &(src_ptr->free_group_ids_begin));    \
    ZB_LETOH16(&((data_ptr)->free_group_ids_end), &(src_ptr->free_group_ids_end));        \
  }                                                                                       \
}

/**
 *  @brief Parsed Commissioning.NetworkJoinEndDeviceResponse payload.
 *  @see ZLL spec, subclause 13.3.2.2.5.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_network_join_end_device_res_s
{
  zb_uint32_t trans_id; /**< Inter-PAN transaction identifier. */
  zb_uint8_t status;    /**< Status. */
} ZB_PACKED_STRUCT zb_zll_commissioning_network_join_end_device_res_t;

/**
 *  @brief Schedules sending Commissioning.NetworkJoinEndDeviceResponse command.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param nwk_status [IN] - network join end device result.
 *  @param tsn [IN] - ZCL transaction sequence number.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
 */
#define ZB_ZLL_COMMISSIONING_SEND_NETWORK_JOIN_ED_RES(                      \
    buffer,                                                                 \
    nwk_status,                                                             \
    tsn,                                                                    \
    dst_addr,                                                               \
    callback,                                                               \
    status)                                                                 \
{                                                                           \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(data_ptr);            \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                          \
      data_ptr,                                                             \
      (tsn),                                                                \
      ZB_ZLL_CMD_COMMISSIONING_NETWORK_JOIN_END_DEVICE_RES);                \
  ZB_ZCL_PACKET_PUT_DATA32_VAL(data_ptr, ZLL_TRAN_CTX().transaction_id);    \
  ZB_ZCL_PACKET_PUT_DATA8(data_ptr, (nwk_status));                          \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback)); \
}

/** @brief Parses Commissioning.NetworkJoinEndDeviceResponse.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_network_join_end_device_res_s.
  * @param buffer containing the packet (by pointer).
  * @param op_status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_NETWORK_JOIN_ED_RES(data_ptr, buffer, op_status)         \
{                                                                                         \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_network_join_end_device_res_t)) \
  {                                                                                       \
    (op_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                            \
  }                                                                                       \
  else                                                                                    \
  {                                                                                       \
    zb_uint8_t* src_ptr = zb_buf_begin((buffer));                                         \
    (op_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                            \
    ZB_ZCL_PACKET_GET_DATA32(&((data_ptr)->trans_id), (src_ptr));                         \
    ZB_ZCL_PACKET_GET_DATA8(&((data_ptr)->status), (src_ptr));                            \
  }                                                                                       \
}

/************************** Network update command definitions ****************************/

/**
 *  @brief Parsed Commissioning.NetworkUpdateRequest payload.
 *  @see ZCL spec, subclause 13.3.2.2.5.
 */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_network_update_req_s
{
  zb_uint32_t trans_id;             /**< Inter-PAN transaction identifier. */
  zb_ext_pan_id_t ext_pan_id;       /**< Extended Pan ID. */
  zb_uint8_t nwk_upd_id;            /**< Network update identifier. */
  zb_uint8_t channel;               /**< Logical channel. */
  zb_uint16_t pan_id;               /**< Pan ID. */
  zb_uint16_t short_addr;           /**< Network address. */
} ZB_PACKED_STRUCT zb_zll_commissioning_network_update_req_t;

/**
 *  @brief Schedules sending Commissioning.NetworkUpdateRequest command.
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param nwk_addr [IN] - network address assigned to the target.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param logical_channel [IN] - channel number.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
  */
#define ZB_ZLL_COMMISSIONING_SEND_NETWORK_UPDATE_REQ_WITH_CHANNEL(              \
    buffer,                                                                     \
    nwk_addr,                                                                   \
    dst_addr,                                                                   \
    logical_channel,                                                            \
    callback,                                                                   \
    status)                                                                     \
{                                                                               \
  zb_zll_commissioning_network_update_req_t *req_data;                          \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                         \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_O(data_ptr, ZB_ZCL_DISABLE_DEFAULT_RESPONSE); \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                              \
      data_ptr,                                                                 \
      ZB_ZCL_GET_SEQ_NUM(),                                                     \
      ZB_ZLL_CMD_COMMISSIONING_NETWORK_UPDATE_REQ);                             \
  req_data = (zb_zll_commissioning_network_update_req_t*)data_ptr;              \
  ZB_HTOLE32(&(req_data->trans_id), &(ZLL_TRAN_CTX().transaction_id));          \
  ZB_IEEE_ADDR_COPY(req_data->ext_pan_id, zb_zll_get_nib_ext_pan_id());         \
  req_data->nwk_upd_id = zb_zll_get_nib_update_id();                            \
  req_data->channel = (logical_channel);                                        \
  ZB_HTOLE16_VAL(&(req_data->pan_id), ZB_PIBCACHE_PAN_ID());                    \
  ZB_HTOLE16_VAL(&(req_data->short_addr), (nwk_addr));                          \
  data_ptr += sizeof(zb_zll_commissioning_network_update_req_t);                \
  (status) = ZB_ZLL_SEND_PACKET((buffer), data_ptr, (dst_addr), (callback));    \
}

/**
 *  @brief Schedules sending Commissioning.NetworkUpdateRequest command.
 *  (send command with current value of channel from pib cache)
 *  @param buffer [IN] - index @ref zb_bufid_t of the buffer to put packet to.
 *  @param nwk_addr [IN] - network address assigned to the target.
 *  @param dst_addr [IN] - IEEE device address to send packet to.
 *  @param callback [IN] - callback to schedule on packet send confirmation.
 *  @param status [OUT] - packet send schedule status (see @ref zb_ret_t).
  */
#define ZB_ZLL_COMMISSIONING_SEND_NETWORK_UPDATE_REQ(                       \
    buffer,                                                                 \
    nwk_addr,                                                               \
    dst_addr,                                                               \
    callback,                                                               \
    status)                                                                 \
{                                                                           \
  ZB_ZLL_COMMISSIONING_SEND_NETWORK_UPDATE_REQ_WITH_CHANNEL(                \
      buffer,                                                               \
      nwk_addr,                                                             \
      dst_addr,                                                             \
      ZB_PIBCACHE_CURRENT_CHANNEL(),                                        \
      callback,                                                             \
      status);                                                              \
}

/** @brief Parses Commissioning.NetworkUpdateRequest.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref zb_zll_commissioning_network_update_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_NETWORK_UPDATE_REQ(data_ptr, buffer, status)    \
{                                                                                \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_network_update_req_t)) \
  {                                                                              \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                      \
  }                                                                              \
  else                                                                           \
  {                                                                              \
    zb_zll_commissioning_network_update_req_t *src_ptr =                         \
        (zb_zll_commissioning_network_update_req_t*)zb_buf_begin((buffer));      \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                      \
    ZB_LETOH32(&((data_ptr)->trans_id), &(src_ptr->trans_id));                   \
    ZB_IEEE_ADDR_COPY((data_ptr)->ext_pan_id, src_ptr->ext_pan_id);              \
    (data_ptr)->nwk_upd_id = src_ptr->nwk_upd_id;                                \
    (data_ptr)->channel  = src_ptr->channel;                                     \
    ZB_LETOH16(&((data_ptr)->pan_id), &(src_ptr->pan_id));                       \
    ZB_LETOH16(&((data_ptr)->short_addr), &(src_ptr->short_addr));               \
  }                                                                              \
}

/************************** Get group identifiers command definitions ****************************/

/** @brief Parsed Commissioning.GetGroupIdentifiersRequest payload.
  * @see ZCL spec, subclause 13.3.2.1.9.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_get_group_ids_req_s
{
  zb_uint8_t start_idx; /**< Start index. */
} ZB_PACKED_STRUCT zb_zll_commissioning_get_group_ids_req_t;

/** @brief Schedules sending Commissioning.GetGroupIdentifiersRequest command.
  * @param buffer to put packet to (by pointer).
  * @param start_index - Rindex at which to start returning group identifiers.
  * @param dst_addr - IEEE device address to send packet to.
  * @param dst_ep - endpoint of device to send packet to.
  * @param ep - endpoint of target device.
  * @param callback to schedule on packet send confirmation.
  */
#define ZB_ZLL_COMMISSIONING_SEND_GET_GROUP_ID_REQ(              \
    buffer,                                                      \
    start_index,                                                 \
    dst_addr,                                                    \
    dst_ep,                                                      \
    ep,                                                          \
    callback)                                                    \
{                                                                \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));          \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(data_ptr,\
      ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC);\
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                               \
      data_ptr,                                                  \
      ZB_ZCL_GET_SEQ_NUM(),                                      \
      ZB_ZLL_CMD_COMMISSIONING_GET_GROUP_IDENTIFIERS_REQUEST);   \
  ZB_ZCL_PACKET_PUT_DATA8(data_ptr, (start_index));              \
  ZB_ZCL_FINISH_PACKET((buffer), data_ptr)                       \
  ZB_ZCL_SEND_COMMAND_SHORT(                                     \
      (buffer),                                                  \
      dst_addr,                                                  \
      ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                          \
      dst_ep,                                                    \
      ep,                                                        \
      ZB_AF_ZLL_PROFILE_ID,                                      \
      ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,                 \
      callback);                                                 \
}

/** @brief Parses Commissioning.GetGroupIdentifiersRequest.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref zb_zll_commissioning_get_group_ids_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_GET_GROUP_ID_REQ(data_ptr, buffer, status)     \
{                                                                               \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_get_group_ids_req_t)) \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    zb_uint8_t* src_ptr = zb_buf_begin((buffer));                               \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
    ZB_ZCL_PACKET_GET_DATA8(&((data_ptr)->start_idx), (src_ptr));               \
  }                                                                             \
}

/** @brief Parsed Commissioning.GetGroupIdentifiersResponse payload.
  * @see ZCL spec, subclause 13.3.2.2.7.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_get_group_ids_res_permanent_s
{
  zb_uint8_t total;     /**< Total. */
  zb_uint8_t start_idx; /**< Start index. */
  zb_uint8_t count;     /**< Count. */
} ZB_PACKED_STRUCT zb_zll_commissioning_get_group_ids_res_permanent_t;

/** @brief Group information record for Commissioning.GetGroupIdentifiersResponse command.
  * @see ZCL spec, subclause 13.3.2.2.7.4.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_group_info_record_s
{
  zb_uint16_t group_id;   /**< Group identifier. */
  zb_uint8_t group_type;  /**< Group type. */
} ZB_PACKED_STRUCT zb_zll_commissioning_group_info_record_t;

/** @brief Group type value for group_type field of @ref zb_zll_commissioning_group_info_record_s.
  * @see ZCL spec, subclause 13.3.2.2.7.4.
  */
#define ZB_ZLL_COMMISSIONING_GROUP_INFO_GROUP_TYPE_VALUE  0x00

/** @brief Fills in mandatory part of Commissioning.GetGroupIdentifiersResponse payload.
  * @param buffer to put packet to (by pointer).
  * @param data_ptr - pointer to contain address right after the response's permanent part's tail.
  * @param total_ - all count group identifiers
  * @param start_index - Index at which to start returning group identifiers.
  * @param count_ - count of identifiers in this payload.
 *  @param tsn [IN] - ZCL transaction sequence number.
  */
#define ZB_ZLL_COMMISSIONING_INIT_GET_GROUP_ID_RES(              \
    buffer,                                                      \
    data_ptr,                                                    \
    total_,                                                      \
    start_index,                                                 \
    count_,                                                      \
    tsn)                                                         \
{                                                                \
  (data_ptr) = ZB_ZCL_START_PACKET((buffer));                    \
  zb_zll_commissioning_get_group_ids_res_permanent_t *req_data;  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(data_ptr); \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                               \
      data_ptr,                                                  \
      (tsn),                                                     \
      ZB_ZLL_CMD_COMMISSIONING_GET_GROUP_IDENTIFIERS_RESPONSE);  \
  req_data = (zb_zll_commissioning_get_group_ids_res_permanent_t*)data_ptr; \
  req_data->total = (total_);                                    \
  req_data->start_idx = (start_index);                           \
  req_data->count = (count_);                                    \
  data_ptr += sizeof(zb_zll_commissioning_get_group_ids_res_permanent_t);   \
}

#define ZB_ZLL_COMMISSIONING_ADD_GROUP_INFO_GET_GROUP_ID_RES(data_ptr, grp_id, grp_type) \
{                                                                                        \
  ZB_ZCL_PACKET_PUT_DATA16((data_ptr), (grp_id));                                        \
  ZB_ZCL_PACKET_PUT_DATA8((data_ptr), (grp_type));                                       \
}

#define ZB_ZLL_COMMISSIONING_SEND_GET_GROUP_ID_RES( \
    buffer,                                         \
    data_ptr,                                       \
    addr,                                           \
    dst_ep,                                         \
    ep,                                             \
    callback)                                       \
{                                                   \
  ZB_ZCL_FINISH_PACKET((buffer), (data_ptr))        \
  ZB_ZCL_SEND_COMMAND_SHORT(                        \
      (buffer),                                     \
      addr,                                         \
      ZB_APS_ADDR_MODE_16_ENDP_PRESENT,             \
      dst_ep,                                       \
      ep,                                           \
      ZB_AF_ZLL_PROFILE_ID,                         \
      ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,    \
      callback);                                    \
}

/** @brief Parses permanent part of Commissioning.GetGroupIdentifiersResponse.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_get_group_ids_res_permanent_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_GET_GROUP_ID_RES(data_ptr, buffer, status)                 \
{                                                                                           \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_get_group_ids_res_permanent_t))   \
  {                                                                                         \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                                 \
  }                                                                                         \
  else                                                                                      \
  {                                                                                         \
    zb_zll_commissioning_get_group_ids_res_permanent_t* src_ptr =                           \
      (zb_zll_commissioning_get_group_ids_res_permanent_t*)zb_buf_begin((buffer));          \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                 \
    ZB_MEMCPY((data_ptr), src_ptr,sizeof(zb_zll_commissioning_get_group_ids_res_permanent_t)); \
    (void)zb_buf_cut_left((buffer), sizeof(zb_zll_commissioning_get_group_ids_res_permanent_t)); \
  }                                                                                         \
}

/** @brief Extracts next group information record from
  * Commissioning.GetGroupIdentifiersResponse.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref zb_zll_commissioning_group_info_record_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_NEXT_GROUP_INFO_GET_GROUP_ID_RES(data_ptr, buffer, status) \
{                                                                                           \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_group_info_record_t))             \
  {                                                                                         \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                                 \
  }                                                                                         \
  else                                                                                      \
  {                                                                                         \
    zb_uint8_t* src_ptr = zb_buf_begin((buffer));                                           \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                 \
    ZB_ZCL_PACKET_GET_DATA16(&((data_ptr)->group_id), (src_ptr));                           \
    ZB_ZCL_PACKET_GET_DATA8(&((data_ptr)->group_type), (src_ptr));                          \
    (void)zb_buf_cut_left((buffer), sizeof(zb_zll_commissioning_group_info_record_t));      \
  }                                                                                         \
}

/************************** Get endpoint list command definitions ****************************/

/** @brief Parsed Commissioning.GetEndpointListRequest payload.
  * @see ZCL spec, subclause 13.3.2.1.10.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_get_endpoint_list_req_s
{
  zb_uint8_t start_idx; /**< Start index. */
} ZB_PACKED_STRUCT zb_zll_commissioning_get_endpoint_list_req_t;

/** @brief Schedules sending Commissioning.GetEndpointListRequest command.
  * @param buffer to put packet to (by pointer).
  * @param start_index - Index at which to start returning group identifiers.
  * @param addr - IEEE device address to send packet to.
  * @param dst_ep - endpoint of device to send packet to.
  * @param ep - endpoint of target device.
  * @param callback to schedule on packet send confirmation.
  */
#define ZB_ZLL_COMMISSIONING_SEND_GET_EP_LIST_REQ(               \
    buffer,                                                      \
    start_index,                                                 \
    addr,                                                        \
    dst_ep,                                                      \
    ep,                                                          \
    callback)                                                    \
{                                                                \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));          \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(data_ptr,\
  ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC);\
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                               \
      data_ptr,                                                  \
      ZB_ZCL_GET_SEQ_NUM(),                                      \
      ZB_ZLL_CMD_COMMISSIONING_GET_ENDPOINT_LIST_REQUEST);       \
  ZB_ZCL_PACKET_PUT_DATA8(data_ptr, (start_index));              \
  ZB_ZCL_FINISH_PACKET((buffer), data_ptr)                       \
  ZB_ZCL_SEND_COMMAND_SHORT(                                     \
      (buffer),                                                  \
      addr,                                                      \
      ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                          \
      dst_ep,                                                    \
      ep,                                                        \
      ZB_AF_ZLL_PROFILE_ID,                                      \
      ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,                 \
      callback);                                                 \
}

/** @brief Parses Commissioning.GetEndpointListRequest.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_get_endpoint_list_req_s.
  * @param status - status read parameters - see @ref zb_zcl_parse_status_e
  * @param buffer containing the packet (by pointer).
  */
#define ZB_ZLL_COMMISSIONING_GET_GET_EP_LIST_REQ(data_ptr, buffer, status)          \
{                                                                                   \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_get_endpoint_list_req_t)) \
  {                                                                                 \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                         \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    zb_uint8_t* src_ptr = zb_buf_begin((buffer));                                   \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                         \
    ZB_ZCL_PACKET_GET_DATA8(&((data_ptr)->start_idx), (src_ptr));                   \
  }                                                                                 \
}

/** @brief Parsed Commissioning.GetEndpointListResponse payload.
  * @see ZCL spec, subclause 13.3.2.2.8.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_get_endpoint_list_res_s
{
  zb_uint8_t total;     /**< Total. */
  zb_uint8_t start_idx; /**< Start index. */
  zb_uint8_t count;     /**< Count. */
} ZB_PACKED_STRUCT zb_zll_commissioning_get_endpoint_list_res_t;

/** @brief Endpoint information record for Commissioning.GetEndpointListResponse command.
  * @see ZCL spec, subclause 13.3.2.2.8.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_endpoint_info_record_s
{
  zb_uint16_t addr_short; /**< Network address. */
  zb_uint8_t endpoint;    /**< Endpoint identifier. */
  zb_uint16_t profile_id; /**< Profile identifier. */
  zb_uint16_t device_id;  /**< Device identifier. */
  zb_uint8_t version;     /**< Version. */
} ZB_PACKED_STRUCT zb_zll_commissioning_endpoint_info_record_t;

/** @brief Fills in mandatory part of Commissioning.GetGroupIdentifiersResponse payload.
  * @param buffer to put packet to (by pointer).
  * @param total_ - all count endpoint list
  * @param start_index - Index at which to start returning endpoint list.
  * @param count_ - count of endpoints in this payload.
  * @param tsn [IN] - ZCL transaction sequence number.
  * @param data_ptr - pointer to contain address right after the response's permanent part's tail.
  */
#define ZB_ZLL_COMMISSIONING_INIT_GET_EP_LIST_RES(                 \
    buffer,                                                        \
    total_,                                                        \
    start_index,                                                   \
    count_,                                                        \
    tsn,                                                           \
    data_ptr)                                                      \
{                                                                  \
  zb_zll_commissioning_get_endpoint_list_res_t *res_data;          \
  (data_ptr) = ZB_ZCL_START_PACKET((buffer));                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL((data_ptr)); \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                 \
      (data_ptr),                                                  \
      (tsn),                                                       \
      ZB_ZLL_CMD_COMMISSIONING_GET_ENDPOINT_LIST_RESPONSE);        \
  res_data = (zb_zll_commissioning_get_endpoint_list_res_t*)(data_ptr); \
  res_data->total = (total_);                                           \
  res_data->start_idx = (start_index);                                  \
  res_data->count = (count_);                                           \
  (data_ptr) += sizeof(zb_zll_commissioning_get_endpoint_list_res_t);   \
}

#define ZB_ZLL_COMMISSIONING_ADD_EP_INFO_GET_EP_LIST_RES( \
    data_ptr,                                             \
    addr,                                                 \
    ep,                                                   \
    profile,                                              \
    device,                                               \
    version_id)                                           \
{                                                         \
  zb_zll_commissioning_endpoint_info_record_t *res_data =               \
    (zb_zll_commissioning_endpoint_info_record_t*)data_ptr;             \
  ZB_HTOLE16_VAL(&(res_data->addr_short), (addr));                      \
  res_data->endpoint = (ep);                                            \
  ZB_HTOLE16_VAL(&(res_data->profile_id), (profile));                   \
  ZB_HTOLE16_VAL(&(res_data->device_id), (device));                     \
  res_data->version = (version_id);                                     \
  (data_ptr) += sizeof(zb_zll_commissioning_endpoint_info_record_t);    \
}

#define ZB_ZLL_COMMISSIONING_SEND_GET_EP_LIST_RES( \
    buffer,                                        \
    data_ptr,                                      \
    dst_addr,                                      \
    dst_ep,                                        \
    ep,                                            \
    callback)                                      \
{                                                  \
  ZB_ZCL_FINISH_PACKET((buffer), (data_ptr))       \
  ZB_ZCL_SEND_COMMAND_SHORT(                       \
      (buffer),                                    \
      dst_addr,                                    \
      ZB_APS_ADDR_MODE_16_ENDP_PRESENT,            \
      dst_ep,                                      \
      ep,                                          \
    ZB_AF_ZLL_PROFILE_ID,                        \
    ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,   \
    callback);                                   \
}

/** @brief Parses permanent part of Commissioning.GetEndpointListResponse.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_get_endpoint_list_res_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_GET_EP_LIST_RES(data_ptr, buffer, status)            \
{                                                                                     \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_get_endpoint_list_res_t))   \
  {                                                                                   \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                           \
  }                                                                                   \
  else                                                                                \
  {                                                                                   \
    zb_zll_commissioning_get_endpoint_list_res_t* src_ptr =                           \
      (zb_zll_commissioning_get_endpoint_list_res_t*)zb_buf_begin((buffer));          \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                           \
    ZB_MEMCPY((data_ptr), src_ptr, sizeof(zb_zll_commissioning_get_endpoint_list_res_t)); \
    (void)zb_buf_cut_left((buffer), sizeof(zb_zll_commissioning_get_endpoint_list_res_t)); \
  }                                                                                   \
}

/** @brief Extracts next endpoint information record from
  * Commissioning.GetEndpointListResponse.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_endpoint_info_record_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZLL_COMMISSIONING_GET_NEXT_EP_INFO_GET_EP_LIST_RES(data_ptr, buffer, status) \
{                                                                                       \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_endpoint_info_record_t))      \
  {                                                                                     \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                             \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    zb_zll_commissioning_endpoint_info_record_t* src_ptr =                              \
      (zb_zll_commissioning_endpoint_info_record_t*)zb_buf_begin((buffer));             \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                             \
    ZB_LETOH16(&((data_ptr)->addr_short), &(src_ptr->addr_short));                      \
    (data_ptr)->endpoint = src_ptr->endpoint;                                           \
    ZB_LETOH16(&((data_ptr)->profile_id), &(src_ptr->profile_id));                      \
    ZB_LETOH16(&((data_ptr)->device_id), &(src_ptr->device_id));                         \
    (data_ptr)->version = src_ptr->version;                                             \
    (void)zb_buf_cut_left((buffer), sizeof(zb_zll_commissioning_endpoint_info_record_t)); \
  }                                                                                     \
}

/************************** Endpoint information command definitions ****************************/

/** @brief Parsed Commissioning.EndpointInformation payload.
  * @see ZCL spec, subclause 13.3.2.2.6.
  */
typedef ZB_PACKED_PRE struct zb_zll_commissioning_endpoint_information_s
{
  zb_ieee_addr_t addr_long; /**< IEEE address. */
  zb_uint16_t addr_short;   /**< Network address. */
  zb_uint8_t endpoint;      /**< Endpoint identifier. */
  zb_uint16_t profile_id;   /**< Profile identifier. */
  zb_uint16_t device_id;    /**< Device identifier. */
  zb_uint8_t version;       /**< Version. */
} ZB_PACKED_STRUCT zb_zll_commissioning_endpoint_information_t;

/** @brief Schedules sending Commissioning.EndpointInformation command.
  * @param buffer to put packet to (by pointer).
  * @param endpoint_id - endpoint identifier.
  * @param profile - profile identifier.
  * @param device - device identifier.
  * @param version_ - device version.
  * @param dst_addr - IEEE device address to send packet to.
  * @param dst_ep - endpoint of device to send packet to.
  * @param ep - endpoint of target device.
  * @param callback to schedule on packet send confirmation.
  */
#define ZB_ZLL_COMMISSIONING_SEND_EP_INFO(                               \
    buffer,                                                              \
    endpoint_id,                                                         \
    profile,                                                             \
    device,                                                              \
    version_,                                                            \
    dst_addr,                                                            \
    dst_ep,                                                              \
    ep,                                                                  \
    callback)                                                            \
{                                                                        \
  zb_uint8_t* data_ptr = ZB_ZCL_START_PACKET((buffer));                  \
  zb_zll_commissioning_endpoint_information_t *req_data;                 \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(data_ptr,            \
  ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);       \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                       \
      data_ptr,                                                          \
      ZB_ZCL_GET_SEQ_NUM(),                                              \
      ZB_ZLL_CMD_COMMISSIONING_ENDPOINT_INFORMATION);                      \
  req_data = (zb_zll_commissioning_endpoint_information_t*)(data_ptr);   \
  ZB_IEEE_ADDR_COPY(req_data->addr_long, ZB_PIBCACHE_EXTENDED_ADDRESS());\
  ZB_HTOLE16_VAL(&(req_data->addr_short), ZB_PIBCACHE_NETWORK_ADDRESS());\
  req_data->endpoint = (endpoint_id);                                    \
  ZB_HTOLE16_VAL(&(req_data->profile_id), (profile));                    \
  ZB_HTOLE16_VAL(&(req_data->device_id), (device));                      \
  req_data->version = (version_);                                        \
  (data_ptr) += sizeof(zb_zll_commissioning_endpoint_information_t);     \
  ZB_ZCL_FINISH_PACKET((buffer), (data_ptr))                             \
  ZB_ZCL_SEND_COMMAND_SHORT(                                             \
      (buffer),                                                          \
      dst_addr,                                                          \
      ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                                  \
      dst_ep,                                                            \
      ep,                                                                \
      ZB_AF_ZLL_PROFILE_ID,                                              \
      ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING,                         \
      callback);                                                         \
}

/** @brief Parses Commissioning.EndpointInformation.
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zll_commissioning_endpoint_information_s.
  * @param buffer containing the packet (by pointer).
  * @param status - status read parameters - see @ref zb_zcl_parse_status_e
  */
#define ZB_ZLL_COMMISSIONING_GET_EP_INFO(data_ptr, buffer, status)                 \
{                                                                                  \
  if (zb_buf_len((buffer)) != sizeof(zb_zll_commissioning_endpoint_information_t)) \
  {                                                                                \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                        \
  }                                                                                \
  else                                                                             \
  {                                                                                \
    zb_zll_commissioning_endpoint_information_t* src_ptr =                         \
      (zb_zll_commissioning_endpoint_information_t*)zb_buf_begin((buffer));        \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                        \
    ZB_IEEE_ADDR_COPY((data_ptr)->addr_long, src_ptr->addr_long);                  \
    ZB_LETOH16(&((data_ptr)->addr_short), &(src_ptr->addr_short));                 \
    (data_ptr)->endpoint = src_ptr->endpoint;                                      \
    ZB_LETOH16(&((data_ptr)->profile_id), &(src_ptr->profile_id));                 \
    ZB_LETOH16(&((data_ptr)->device_id), &(src_ptr->device_id));                   \
    (data_ptr)->version = src_ptr->version;                                        \
  }                                                                                \
}

/*! @} */ /* Touchlink Commissioning cluster commands */

/*! @} */ /* Touchlink Commissioning cluster */

/** @endcond */ /* DOXYGEN_ZCL_SECTION && DOXYGEN_TOUCHLINK_FEATURE */

#define ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING_SERVER_ROLE_INIT (zb_zcl_cluster_init_t)NULL
#define ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING_CLIENT_ROLE_INIT (zb_zcl_cluster_init_t)NULL

#endif /* ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING_H */
