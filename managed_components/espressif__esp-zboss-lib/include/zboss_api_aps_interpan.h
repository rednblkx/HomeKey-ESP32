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
/* PURPOSE: Public APS Inter-Pan API
*/

#ifndef ZBOSS_API_APS_INTERPAN_H
#define ZBOSS_API_APS_INTERPAN_H 1

/** @addtogroup aps_api
  * @{
  */


#if defined ZB_ENABLE_INTER_PAN_EXCHANGE || defined DOXYGEN
/** @brief INTRP-DATA.request structure.
  *
  * This structure passed to @ref zb_intrp_data_request() in the packet buffer parameter.
  * @see SE spec, clause B.3.
  */
typedef ZB_PACKED_PRE struct zb_intrp_data_req_s
{
  /** @brief The addressing mode for the destination address used in this primitive.
    *
    * This parameter can take one of the values from the following list:
    * @li 0x01 = 16-bit group address
    * @li 0x02 = 16-bit NWK address, normally the broadcast address 0xffff
    * @li 0x03 = 64-bit extended address
    */
  zb_uint8_t dst_addr_mode;
  /** The 16-bit Pan identifier of the entity or entities to which the ASDU is being transferred or
    * the broadcast Pan ID 0xffff.
    */
  zb_uint16_t dst_pan_id;
  /** The address of the entity or entities to which the ASDU is being transferred. */
  zb_addr_u dst_addr;
  /** The identifier of the application profile for which this frame is intended. */
  zb_uint16_t profile_id;
  /** The identifier of the cluster, within the profile specified by the ProfileId parameter, which
    * defines the application semantics of the ASDU.
    */
  zb_uint16_t cluster_id;
  /** An integer handle associated with the ASDU to be transmitted. */
  zb_uint8_t asdu_handle;
} ZB_PACKED_STRUCT zb_intrp_data_req_t;
#endif /* defined ZB_ENABLE_INTER_PAN_EXCHANGE || defined DOXYGEN */

/** @brief Valid values for inter-PAN destination address mode.
  * @see SE spec, subclause B.3.1.
  */
enum zb_intrp_addr_mode_e
{
  /** 16-bit group address */
  ZB_INTRP_ADDR_GROUP                   = 0x01,
  /** 16-bit NWK address */
  ZB_INTRP_ADDR_NETWORK                 = 0x02,
  /** 64-bit extended device address */
  ZB_INTRP_ADDR_IEEE                    = 0x03
};

/** @brief Inter-PAN broadcast short network address */
#define ZB_INTRP_BROADCAST_SHORT_ADDR 0xffffU

/** @brief INTRP-DATA.indication parameters.
  * @see SE spec, subclause B.3.3.
  */
typedef ZB_PACKED_PRE struct zb_intrp_data_ind_s
{
  /** @brief Destination address mode.
    * Valid values are defined by @ref zb_intrp_addr_mode_e enumeration.
    */
  zb_uint8_t dst_addr_mode;
  /** @brief Destination Pan identifier.
    * Should be a valid Pan identifier or a broadcast Pan ID 0xffff.
    */
  zb_uint16_t dst_pan_id;
  /** @brief Destination address. */
  zb_addr_u dst_addr;
  /** @brief Source Pan identifier.
    * Assumed to be a valid Pan identifier in all cases.
    */
  zb_uint16_t src_pan_id;
  /** @brief Source device address. */
  zb_ieee_addr_t src_addr;
  /** @brief Profile identifier. */
  zb_uint16_t profile_id;
  /** @brief Cluster identifier. */
  zb_uint16_t cluster_id;
  /** @brief The link quality observed during the reception of the ASDU. */
  zb_uint8_t link_quality;
  zb_int8_t rssi;
} ZB_PACKED_STRUCT zb_intrp_data_ind_t;

/** @cond DOXYGEN_APS_INTER_PAN_NON_DEFAULT_CHANNEL_FEATURE */
#if defined ZB_ENABLE_INTER_PAN_NON_DEFAULT_CHANNEL || defined DOXYGEN
/** @brief A multi-channel INTRP-DATA.confirm representation structure.
  */
typedef struct zb_mchan_intrp_data_confirm_s
{
  zb_channel_page_t channel_status_page_mask;  /*!< Channel Page structure - binary encoded channel page
                                                and channels mask as list of channels, indicating on which
                                                channels the packet was successfully sent. */
  zb_uint8_t  asdu_handle;  /*!< An integer handle associated with the transmitted frame. */
} zb_mchan_intrp_data_confirm_t;

/**
 * Inter-pan packet handler
 *
 * @param param - index of buffer with inter-pan packet command
 * @param current_page - channel page at which packet was received
 * @param current_channel - channel at which packet was received
 * @return status (see @ref zb_bool_t)
 */
typedef zb_uint8_t (*zb_af_inter_pan_handler_t)(zb_uint8_t param, zb_uint8_t current_page, zb_uint8_t current_channel);

/** @brief Enable interpan procedure. This allows to send interpan frames at multiple channels
  *        and register handler to receive interpan frames.
  */
void zboss_enable_interpan_with_chan_change(void);

/** @brief Make INTRP-DATA request at multiple channels with time given as chan_wait_ms
  *        to wait for response packets. Can be used after zboss_enable_interpan_with_chan_change()
  *        was called.
  *
  * Assumes buffer contains data in its main part, and INTRP-DATA.request parameters in buffer's
  * parameter (represented as @ref zb_intrp_data_req_s structure).
  * @param buffer - reference (index) of the packet buffer.
  * @param channel_page_mask - Channel Page structure - binary encoded channel page
  *                            and channels mask as list of channels to send packet at.
  * @param chan_wait_ms - time in milliseconds to wait at channel after the packet is sent.
  * @param cb - user callback function called after the procedure is finished.
  * @return Returns RET_OK if procedure has started successfully,
  *         RET_BUSY if procedure is on-going,
  *         RET_ERROR if failed to start or procedure was not enabled,
  *         RET_INVALID_PARAMETER if one of function's arguments is invalid.
  * @note  cb buffer's parameter contains status which can be obtained using @ref zb_buf_get_status
  * function, RET_OK if packet was successfully sent at at least one channel, RET_ERROR otherwise.
  * @ref zb_mchan_intrp_data_confirm_t is put as buffer's parameter.
  * User is to free the buffer in the callback, if no callback is given, buffer is freed internally.
  */
zb_ret_t zb_intrp_data_request_with_chan_change(zb_bufid_t buffer, zb_channel_page_t channel_page_mask, zb_uint32_t chan_wait_ms, zb_callback_t cb);

/** @brief Register inter-pan indication callback, called when inter-pan packet is received.
  *        Can be used after zboss_enable_interpan_with_chan_change() was called.
  *
  * @param cb - callback function to be called when inter-pan packet is received.
  * @note If packet is processed in the callback function and no longer shall be processed
  *       by the stack, callback function shall free buffer and return ZB_TRUE. Return ZB_FALSE otherwise.
  *       Buffer has data put in the beginning of the buffer with inter-pan header truncated
  *       and contains @ref zb_intrp_data_ind_t given as buffer's parameter.
  */
void zb_af_interpan_set_data_indication(zb_af_inter_pan_handler_t cb);
#endif /* defined ZB_ENABLE_INTER_PAN_NON_DEFAULT_CHANNEL || defined DOXYGEN */
/** @endcond */ /* DOXYGEN_APS_INTER_PAN_NON_DEFAULT_CHANNEL_FEATURE */

#if defined ZB_ENABLE_INTER_PAN_EXCHANGE || defined DOXYGEN
/** @brief Make INTRP-DATA request.
  *
  * Assumes buffer contains data in its main part, and INTRP-DATA.request parameters in buffer's
  * parameter (represented as @ref zb_intrp_data_req_s structure).
  * @param param - reference (index) of the packet buffer.
  * @see SE spec, subclause B.5.1.
  */
void zb_intrp_data_request(zb_uint8_t param);
#endif /* defined ZB_ENABLE_INTER_PAN_EXCHANGE || defined DOXYGEN */

/** @} */

#endif /* ZBOSS_API_APS_INTERPAN_H */
