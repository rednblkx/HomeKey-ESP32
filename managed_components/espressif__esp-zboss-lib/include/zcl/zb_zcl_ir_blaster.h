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
/* PURPOSE: ZBOSS specific IR Blaster cluster, purpose: send IR Control commands.
*/
#ifndef ZB_ZCL_IR_BLASTER_H
#define ZB_ZCL_IR_BLASTER_H 1

#if defined ZB_ZCL_SUPPORT_CLUSTER_IR_BLASTER || defined DOXYGEN

/** @cond DOXYGEN_INTERNAL_DOC */

/** @addtogroup ZB_ZCL_IR_BLASTER
 *  @{
 *    @details
 *    Manufacture specific cluster for transmit IR codes
 *    Model: One (client) to More (servers)
 */

  /*! @name Ir_Blaster cluster commands
    @{
*/

#define ZB_ZCL_IR_BLASTER_IR_SIGNATURE_SIZE 22

/**
 * @brief Ir_Blaster cluster commands
 */
enum zb_zcl_ir_blaster_cmd_e
{
  ZB_ZCL_CMD_IR_BLASTER_TRANSMIT_IR_DATA        = 0x00,
  ZB_ZCL_CMD_IR_BLASTER_TRANSMISSION_STATUS     = 0x01,
  ZB_ZCL_CMD_IR_BLASTER_GET_IR_SIGNATURE        = 0x02,
  ZB_ZCL_CMD_IR_BLASTER_GET_IR_SIGNATURE_RESP   = 0x03,
};

/** Ir_Blaster status */
typedef enum zb_zcl_ir_blaster_transmission_status_e
{
  ZB_ZCL_IR_BLASTER_TRANSMISSION_STATUS_COMPLETED    = 0x30,
  ZB_ZCL_IR_BLASTER_TRANSMISSION_STATUS_ERROR        = 0x40,
}
zb_zcl_ir_blaster_transmit_status_t;

/************* Ir_Blaster cluster command structures **************/

typedef ZB_PACKED_PRE struct zb_zcl_ir_blaster_transmit_ir_data_s
{
  zb_uint8_t ir_code_id[16];
}
ZB_PACKED_STRUCT
zb_zcl_ir_blaster_transmit_ir_data_t;

typedef struct zb_zcl_ir_blaster_transmit_ir_data_value_param_s
{
  zb_zcl_parsed_hdr_t cmd_info;
  zb_zcl_ir_blaster_transmit_ir_data_t payload;
} zb_zcl_ir_blaster_transmit_ir_data_value_param_t;

/*! @brief Send Transfer Data command
    @param _buffer - to put packet to
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _prfl_id - profile identifier
    @param _cb - callback for getting command send status
    @param _manufacturer_id - Manufacturer code
    @param _flag - command flag, see @ref zb_zcl_ir_blaster_tx_flags_e
    @param _byte_num - all transfer length or current offset, see @ref zb_zcl_ir_blaster_transfer_data_header_t
    @param _data_size - data size
    @param _image_data - image data
*/
#define ZB_ZCL_IR_BLASTER_SEND_TRANSMIT_IR_DATA(                                 \
    _buffer, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _cb,      \
    _manufacturer_id, _ir_code_id)                                               \
{                                                                                \
  zb_uint8_t* __ptr = ZB_ZCL_START_PACKET((_buffer));                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(__ptr,                   \
      ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, (_def_resp)); \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(__ptr, ZB_ZCL_GET_SEQ_NUM(),               \
      ZB_ZCL_MANUFACTURER_SPECIFIC, (_manufacturer_id),                          \
            ZB_ZCL_CMD_IR_BLASTER_TRANSMIT_IR_DATA);                             \
  ZB_ZCL_PACKET_PUT_DATA_N(__ptr, (_ir_code_id),                                 \
                           (sizeof(zb_zcl_ir_blaster_transmit_ir_data_t)));      \
  ZB_ZCL_FINISH_PACKET((_buffer), __ptr)                                         \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                     \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),        \
      ZB_ZCL_CLUSTER_ID_IR_BLASTER, (_cb));                                      \
}

/** @brief Macro for getting Send Transfer Data command
  * @attention Assumes that ZCL header already cut.
  * @param _data_ptr - pointer to a variable of type @ref
  * zb_zcl_ir_blaster_transfer_data_internal_t.
  * @param _buffer containing the packet (by pointer).
  * @param _status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  *
  * NOTE file data place`s in buffer, payload saves pointer to data only!
  */
#define ZB_ZCL_IR_BLASTER_GET_TRANSMIT_IR_DATA(_data_ptr, _buffer, _status)     \
{                                                                       \
  zb_zcl_ir_blaster_transmit_ir_data_t *src_ptr =                          \
       (zb_zcl_ir_blaster_transmit_ir_data_t*)zb_buf_begin((_buffer));     \
                                                                        \
  if (zb_buf_len((_buffer)) != sizeof(zb_zcl_ir_blaster_transmit_ir_data_t)) \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
    ZB_MEMCPY((_data_ptr)->ir_code_id, (src_ptr),                       \
              sizeof(zb_zcl_ir_blaster_transmit_ir_data_t));            \
  }                                                                     \
}

typedef ZB_PACKED_PRE struct zb_zcl_ir_blaster_transmission_status_s
{
  zb_uint8_t status;        /**< enum @ref zb_zcl_ir_blaster_status_e */
}
ZB_PACKED_STRUCT
zb_zcl_ir_blaster_transmission_status_t;

typedef struct zb_zcl_ir_blaster_transmission_status_value_param_s
{
  /* zb_zcl_parsed_hdr_t cmd_info; */
  zb_zcl_ir_blaster_transmission_status_t payload;
} zb_zcl_ir_blaster_transmission_status_value_param_t;


/*! @brief Send Transfer Data response command
    @param _buffer - to put packet to
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _prfl_id - profile identifier
    @param _seq - request sequence
    @param _cb - callback for getting command send status
    @param _manufacturer_id - Manufacturer code
    @param _tun_status - command status, see @ref zb_zcl_ir_blaster_status_e
*/
#define ZB_ZCL_IR_BLASTER_SEND_TRANSMISSION_STATUS(                                \
    _buffer, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _seq, _cb,             \
    _manufacturer_id, _status)                                                     \
{                                                                                  \
  zb_uint8_t* __ptr = ZB_ZCL_START_PACKET((_buffer));                              \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(__ptr,                    \
      ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(__ptr, (_seq), ZB_ZCL_MANUFACTURER_SPECIFIC, \
      (_manufacturer_id), ZB_ZCL_CMD_IR_BLASTER_TRANSMISSION_STATUS);              \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, (_status));                                       \
  ZB_ZCL_FINISH_PACKET((_buffer), __ptr)                                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                       \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),          \
      ZB_ZCL_CLUSTER_ID_IR_BLASTER, (_cb));                                        \
}

/** @brief Macro for getting Send Transfer Data response command
  * @attention Assumes that ZCL header already cut.
  * @param _data_ptr - pointer to a variable of type @ref
  * zb_zcl_ir_blaster_transfer_data_resp_t.
  * @param _buffer containing the packet (by pointer).
  * @param _status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IR_BLASTER_GET_TRANSMISSION_STATUS(_data_ptr, _buffer, _status)    \
{                                                                                 \
  zb_zcl_ir_blaster_transmission_status_t *src_ptr =                              \
       (zb_zcl_ir_blaster_transmission_status_t*)zb_buf_begin((_buffer));         \
                                                                                  \
  if (zb_buf_len((_buffer)) != sizeof(zb_zcl_ir_blaster_transmission_status_t)  ) \
  {                                                                               \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                      \
  }                                                                               \
  else                                                                            \
  {                                                                               \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                      \
    (_data_ptr)->status = src_ptr->status;                                        \
  }                                                                               \
}

typedef struct zb_zcl_ir_blaster_get_ir_signature_value_param_s
{
  zb_zcl_parsed_hdr_t cmd_info;
} zb_zcl_ir_blaster_get_ir_signature_value_param_t;

/*! @brief Send Get IR Signature command
    @param _buffer - to put packet to
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _prfl_id - profile identifier
    @param _cb - callback for getting command send status
    @param _manufacturer_id - Manufacturer code
*/
#define ZB_ZCL_IR_BLASTER_SEND_GET_IR_SIGNATURE(                                 \
    _buffer, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _cb,      \
    _manufacturer_id)                                                            \
{                                                                                \
  zb_uint8_t* __ptr = ZB_ZCL_START_PACKET((_buffer));                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(__ptr,                   \
      ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, (_def_resp)); \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(__ptr, ZB_ZCL_GET_SEQ_NUM(),               \
      ZB_ZCL_MANUFACTURER_SPECIFIC, (_manufacturer_id),                          \
            ZB_ZCL_CMD_IR_BLASTER_GET_IR_SIGNATURE);                             \
  ZB_ZCL_FINISH_PACKET((_buffer), __ptr)                                         \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                     \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),        \
      ZB_ZCL_CLUSTER_ID_IR_BLASTER, (_cb));                                      \
}

typedef ZB_PACKED_PRE struct zb_zcl_ir_blaster_get_ir_signature_resp_s
{
  zb_uint8_t status;
  zb_uint8_t ir_signature[ZB_ZCL_IR_BLASTER_IR_SIGNATURE_SIZE];
}
ZB_PACKED_STRUCT
zb_zcl_ir_blaster_get_ir_signature_resp_t;

typedef struct zb_zcl_ir_blaster_get_ir_signature_resp_value_param_s
{
  zb_zcl_parsed_hdr_t cmd_info;
  zb_zcl_ir_blaster_get_ir_signature_resp_t payload;
} zb_zcl_ir_blaster_get_ir_signature_resp_value_param_t;


/*! @brief Send Transfer Data response command
    @param _buffer - to put packet to
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _prfl_id - profile identifier
    @param _seq - request sequence
    @param _cb - callback for getting command send status
    @param _manufacturer_id - Manufacturer code
    @param _tun_status - command status, see @ref zb_zcl_ir_blaster_status_e
*/
#define ZB_ZCL_IR_BLASTER_SEND_GET_IR_SIGNATURE_RESP(                              \
    _buffer, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _seq, _cb,             \
    _manufacturer_id, _status, _ir_signature)                                      \
{                                                                                  \
  zb_uint8_t* __ptr = ZB_ZCL_START_PACKET((_buffer));                              \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(__ptr,                    \
      ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(__ptr, (_seq), ZB_ZCL_MANUFACTURER_SPECIFIC, \
      (_manufacturer_id), ZB_ZCL_CMD_IR_BLASTER_GET_IR_SIGNATURE_RESP);            \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, _status);                                         \
  if (_status == ZB_ZCL_IR_BLASTER_TRANSMISSION_STATUS_COMPLETED)       \
  {                                                                     \
    ZB_ZCL_PACKET_PUT_DATA_N(__ptr, (_ir_signature), ZB_ZCL_IR_BLASTER_IR_SIGNATURE_SIZE); \
  }                                                                     \
  ZB_ZCL_FINISH_PACKET((_buffer), __ptr)                                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                       \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),          \
      ZB_ZCL_CLUSTER_ID_IR_BLASTER, (_cb));                                        \
}

/** @brief Macro for getting Send Transfer Data response command
  * @attention Assumes that ZCL header already cut.
  * @param _data_ptr - pointer to a variable of type @ref
  * zb_zcl_ir_blaster_transfer_data_resp_t.
  * @param _buffer containing the packet (by pointer).
  * @param _status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IR_BLASTER_GET_GET_IR_SIGNATURE_RESP(_data_ptr, _buffer, _status)  \
{                                                                                 \
  zb_zcl_ir_blaster_get_ir_signature_resp_t *src_ptr =                            \
       (zb_zcl_ir_blaster_get_ir_signature_resp_t*)zb_buf_begin((_buffer));       \
                                                                                  \
  if (zb_buf_len((_buffer)) == sizeof(zb_zcl_ir_blaster_get_ir_signature_resp_t) || \
      zb_buf_len((_buffer)) == sizeof(zb_uint8_t))                                \
  {                                                                               \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                      \
    (_data_ptr)->status = src_ptr->status;                                        \
    if (src_ptr->status == ZB_ZCL_IR_BLASTER_TRANSMISSION_STATUS_COMPLETED)       \
    {                                                                             \
      ZB_MEMCPY((_data_ptr)->ir_signature, src_ptr->ir_signature, ZB_ZCL_IR_BLASTER_IR_SIGNATURE_SIZE); \
    }                                                                             \
  }                                                                               \
  else                                                                            \
  {                                                                               \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                      \
  }                                                                               \
}

void zb_zcl_ir_blaster_init_server();
void zb_zcl_ir_blaster_init_client();
#define ZB_ZCL_CLUSTER_ID_IR_BLASTER_SERVER_ROLE_INIT zb_zcl_ir_blaster_init_server
#define ZB_ZCL_CLUSTER_ID_IR_BLASTER_CLIENT_ROLE_INIT zb_zcl_ir_blaster_init_client

/*! @} */ /* Ir_Blaster cluster commands */
/*! @} */ /* addtogroup */

/* @endcond */
#endif /*defined ZB_ZCL_SUPPORT_CLUSTER_IR_BLASTER */

#endif /* ZB_ZCL_IR_BLASTER_H */
