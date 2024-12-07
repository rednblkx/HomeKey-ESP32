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
/* PURPOSE: ZCL MDU (Multiple dwelling units) Pairing cluster
         Allow devices joining the NAN to acquire a list of the devices
         forming the 'virtual HAN' for the respective household.
*/
#ifndef ZB_ZCL_MDU_PAIRING_H
#define ZB_ZCL_MDU_PAIRING_H 1

#include "zcl/zb_zcl_common.h"

/** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */


/** @addtogroup ZB_ZCL_MDU_PAIRING
 *  @{
 *    @details
 *    When operating within a multi-dwelling unit (MDU), the commands within
 *    this cluster allow devices joining the NAN to acquire a list of the
 *    devices forming the 'virtual HAN' for the respective household.
 */


/** @defgroup ZB_ZCL_MDU_PAIRING_ATTRS MDU Pairing cluster attributes
 *  @{
 *    @brief MDU Pairing cluster attribute identifiers
 *    @see SE spec, subclause 13.2.2 - No attributes
 *
 */

/** @} */ /* ZB_ZCL_MDU_PAIRING_ATTRS */


/** @defgroup ZB_ZCL_MDU_PAIRING_COMMANDS MDU Pairing cluster commands
 *  @{
 */

/** @brief MDU Pairing cluster client commands
 *  @see SE spec, subclause 13.3.3
 */
enum zb_zcl_mdu_pairing_cmd_client_e
{
  /** The @e PairingRequest command allows a device joining a MDU network to
   * determine the devices that will constitute the 'virtual HAN' for the
   * household in which it is to operate.
   */
  ZB_ZCL_MDU_PAIRING_CLI_CMD_PAIRING_REQUEST = 0x00,  /**< Pairing Request */
};

/** @brief MDU Pairing cluster server commands
 *  @see SE spec, subclause 13.2.3
 */
enum zb_zcl_mdu_pairing_cmd_server_e
{
  /** The @e PairingResponse command provides a device joining a MDU network with
   *  a list of the devices that will constitute the 'virtual HAN' for the
   *  household in which the joining device is to operate.
   */
  ZB_ZCL_MDU_PAIRING_SRV_CMD_PAIRING_RESPONSE = 0x00,  /**< Pairing Request response */
};


/* MDU PAIRING cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_MDU_PAIRING_SERVER_ROLE_GENERATED_CMD_LIST ZB_ZCL_MDU_PAIRING_SRV_CMD_PAIRING_RESPONSE

#define ZB_ZCL_CLUSTER_ID_MDU_PAIRING_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_MDU_PAIRING_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_MDU_PAIRING_CLIENT_ROLE_GENERATED_CMD_LIST ZB_ZCL_MDU_PAIRING_CLI_CMD_PAIRING_REQUEST

#define ZB_ZCL_CLUSTER_ID_MDU_PAIRING_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_MDU_PAIRING_CLIENT_ROLE_GENERATED_CMD_LIST

/************* MDU Pairing cluster command structures **************/

/** @brief Pairing Request command payload
 *  @see SE spec, subclause 13.3.3.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_mdu_pairing_request_s
{
  zb_uint32_t    lpi_version;   /**< Identifies the version of pairing information currently held on the requesting device. */
  zb_ieee_addr_t eui64;         /**< Identifies the MAC address of the requesting device. */
}
ZB_PACKED_STRUCT zb_zcl_mdu_pairing_request_t;

/** @brief Pairing Response command payload
 *  @see SE spec, subclause 13.2.3.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_mdu_pairing_response_s
{
  zb_uint32_t    lpi_version;                 /**< Identifies the version of pairing information included in this command. */
  zb_uint8_t     total_number_of_devices;     /**< The total number of devices expected to form the 'virtual HAN'
                                               *   (including the device to which this command is being sent) */
  zb_uint8_t     command_index;               /**< The CommandIndex is uses to count the payload fragments in the
                                               *   case where the entire payload does not fit into one message. */
  zb_uint8_t     total_number_of_commands;    /**< In the case where the entire payload does not fit into one
                                               *   message, the Total Number of Commands field indicates the
                                               *   total number of sub-commands in the message. */
  zb_ieee_addr_t *eui64;                      /**< EUI64 of Device 1 to EUI64 of Device N represent the MAC address
                                               *   of devices that belong to the 'virtual HAN' of the requesting
                                               *   device; these include the requesting device itself and all other
                                               *   devices the requesting device shall perform service discovery
                                               *   and binding with. */
  zb_uint8_t     num_dev_cmd;                 /**< Number of devices in current command */
}
ZB_PACKED_STRUCT zb_zcl_mdu_pairing_response_t;


/** @def ZB_ZCL_MDU_PAIRING_SEND_PAIRING_REQUEST
 *  @brief Send @e PairingRequest command
 *  @param _param - Reference to buffer.
 *  @param _addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _ep - Current endpoint.
 *  @param _prfl_id - profile identifier
 *  @param _def_resp - enable/disable default zcl response
 *  @param _cb - Callback which should be called when the ZCL stack receives
 *  APS ack.
 *  @param _lpi_version - lpi_version field from @ref zb_zcl_mdu_pairing_response_t payload
 *  @param _eui64 - eui64 field from @ref zb_zcl_mdu_pairing_response_t payload
 *  @par Example
 *  @code{C}
 *    // The command is used as is
 *    ZB_ZCL_MDU_PAIRING_SEND_PAIRING_REQUEST(param, remote.addr, ZB_APS_ADDR_MODE_16_ENDP_PRESENT, remote.ep, this.ep, ZB_AF_SE_PROFILE_ID, 0, NULL, lpi, this.addr);
 *  @endcode
 */
#define ZB_ZCL_MDU_PAIRING_SEND_PAIRING_REQUEST(                                \
    _param, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _cb,      \
    _lpi_version, _eui64)                                                       \
{                                                                               \
  zb_bufid_t buffer = ZB_BUF_FROM_REF(_param);                                   \
  zb_uint8_t* __ptr = zb_zcl_start_command_header(buffer,                       \
    ZB_ZCL_CONSTRUCT_FRAME_CONTROL(ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,          \
                                   ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,            \
                                   ZB_ZCL_FRAME_DIRECTION_TO_SRV, (_def_resp)), \
    0,                          /* No manuf_code */                             \
    ZB_ZCL_MDU_PAIRING_CLI_CMD_PAIRING_REQUEST, NULL);                                      \
  ZB_ZCL_PACKET_PUT_DATA32(__ptr, (_lpi_version));                              \
  ZB_ZCL_PACKET_PUT_DATA64(__ptr, (_eui64));                                    \
  ZB_ZCL_FINISH_PACKET((buffer), __ptr)                                         \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                    \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),       \
      ZB_ZCL_CLUSTER_ID_MDU_PAIRING, (_cb));                                    \
}


/** @def ZB_ZCL_MDU_PAIRING_GET_PAIRING_REQUEST
 *  @brief Macro for parsing payload of @e PairingRequest command
 *  @param _data_ptr - pointer to @ref zb_zcl_mdu_pairing_request_t structure
 *  @param _buffer - pointer to buffer where Pairing Request command's payload is stored.
 *  @param _status - status of payload parsing
 *  @par Example
 *  @code{C}
 *    zb_bufid_t buf;
 *    zb_zcl_mdu_pairing_request_t req;
 *    zb_uint8_t status;
 *
 *    ZB_ZCL_MDU_PAIRING_GET_PAIRING_REQUEST(req, buf, status);
 *
 *    if (status == ZB_ZCL_PARSE_STATUS_SUCCESS)
 *    {
 *      (void)req.eui64;           // use it
 *      (void)req.lpi_version;     // use it
 *    }
 *  @endcode
 */
#define ZB_ZCL_MDU_PAIRING_GET_PAIRING_REQUEST(_data_ptr, _buffer, _status) \
{                                                                           \
  zb_uint8_t *src_ptr = (zb_uint8_t*)zb_buf_begin((_buffer));               \
                                                                            \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_mdu_pairing_request_t))         \
  {                                                                         \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                \
    ZB_HTOLE32(&(_data_ptr)->lpi_version, src_ptr);                         \
    ZB_HTOLE64(&(_data_ptr)->eui64, src_ptr+4);                             \
  }                                                                         \
}

/** @} */ /* ZB_ZCL_MDU_PAIRING_COMMANDS */

/** @} */ /* addtogroup */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */


/* @brief Send Pairing Response command */
/*
#define ZB_ZCL_MDU_PAIRING_SEND_PAIRING_RESPONSE(                               \
    _param, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _cb,      \
    _lpi_version, _eui64)                                                       \
{                                                                               \
  zb_bufid_t buffer = ZB_BUF_FROM_REF(_param);                                   \
  zb_uint8_t* __ptr = zb_zcl_start_command_header(buffer,                       \
    ZB_ZCL_CONSTRUCT_FRAME_CONTROL(ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,          \
                                   ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,            \
                                   ZB_ZCL_FRAME_DIRECTION_TO_SRV, (_def_resp)), \
    0,                          /  No manuf_code /                              \
    ZB_ZCL_MDU_PAIRING_CLI_CMD_PAIRING_REQUEST, NULL);                                      \
  ZB_ZCL_PACKET_PUT_DATA32(__ptr, (_lpi_version));                              \
  ZB_ZCL_PACKET_PUT_DATA64(__ptr, (_eui64));                                    \
  ZB_ZCL_FINISH_PACKET((buffer), __ptr)                                         \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                    \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),       \
      ZB_ZCL_CLUSTER_ID_MDU_PAIRING, (_cb));                                    \
}
*/

/* @brief Macro for getting Pairing Response command data */
/*
#define ZB_ZCL_MDU_PAIRING_GET_PAIRING_RESPONSE(_data_ptr, _buffer, _status)\
{                                                                           \
  zb_zcl_mdu_pairing_response_t *src_ptr = (zb_zcl_mdu_pairing_response_t*)zb_buf_begin((_buffer));               \
                                                                            \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_mdu_pairing_response_t))        \
  {                                                                         \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                \
    ZB_HTOLE32(&(_data_ptr)->lpi_version, src_ptr);                          \
    (_data_ptr)->total_number_of_devices = src_ptr->total_number_of_devices;  \
    (_data_ptr)->command_index = src_ptr->command_index;                      \
    (_data_ptr)->total_number_of_commands = src_ptr->total_number_of_commands;\
    ZB_HTOLE64(&(_data_ptr)->eui64,&(src_ptr->eui64));                                  \
  }                                                                         \
}
*/
zb_ret_t zb_zcl_mdu_pairing_send_cmd_pairing_request(
  zb_uint8_t param,
  const zb_addr_u *dst_addr,
  zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep,
  zb_uint8_t src_ep,
  const zb_zcl_mdu_pairing_request_t *payload,
  zb_ieee_addr_t *buf,
  zb_uint8_t buf_len_in_elements,
  zb_callback_t cb
);
/************************* MDU Pairing internal *******************************/
zb_uint8_t zb_zcl_process_mdu_pairing_specific_commands(zb_uint8_t param);


void zb_zcl_mdu_pairing_init_server(void);
void zb_zcl_mdu_pairing_init_client(void);

#define ZB_ZCL_CLUSTER_ID_MDU_PAIRING_SERVER_ROLE_INIT zb_zcl_mdu_pairing_init_server
#define ZB_ZCL_CLUSTER_ID_MDU_PAIRING_CLIENT_ROLE_INIT zb_zcl_mdu_pairing_init_client

#endif /* ZB_ZCL_MDU_PAIRING_H */
