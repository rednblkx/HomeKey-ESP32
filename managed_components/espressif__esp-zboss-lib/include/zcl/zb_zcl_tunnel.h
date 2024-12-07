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
/* PURPOSE: ZBOSS specific Tunnel cluster, purpose: general data tunneling.
*/
#ifndef ZB_ZCL_TUNNEL_H
#define ZB_ZCL_TUNNEL_H 1

/** @cond (DOXYGEN_ZCL_SECTION && DOXYGEN_CUSTOM_TUNNEL_CLUSTER) */

/** @addtogroup ZB_ZCL_TUNNEL
 *  @{
 *    @details
 *    Manufacture specific cluster for transmit custom data between peers
 *    Model: One (client) to More (servers)
 *    Usually a device has MIXED role
 */

  /*! @name Tunnel cluster commands
    @{
*/

/**
 * @brief Tunnel cluster commands
 */
enum zb_zcl_tunnel_cmd_e
{
  ZB_ZCL_CMD_TUNNEL_TRANSFER_DATA_REQ        = 0x00,  /**< Request to transfer data. */
  ZB_ZCL_CMD_TUNNEL_TRANSFER_DATA_RESP       = 0x01,  /**< Response on received data */
};


/* Tunnel manufacturer specific cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_TUNNEL_FC00_SERVER_ROLE_GENERATED_CMD_LIST ZB_ZCL_CMD_TUNNEL_TRANSFER_DATA_RESP

#define ZB_ZCL_CLUSTER_ID_TUNNEL_FC00_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_TUNNEL_FC00_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_TUNNEL_FC00_CLIENT_ROLE_GENERATED_CMD_LIST ZB_ZCL_CMD_TUNNEL_TRANSFER_DATA_REQ

#define ZB_ZCL_CLUSTER_ID_TUNNEL_FC00_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_TUNNEL_FC00_CLIENT_ROLE_GENERATED_CMD_LIST


/** Tunnel status; range: 0x00 - 0x3F (6-bit value) */
typedef enum zb_zcl_tunnel_status_e
{
  ZB_ZCL_TUNNEL_STATUS_OK = 0,                  /**< Data is sent or received. Use for inform User App about success current command */
  ZB_ZCL_TUNNEL_STATUS_ERROR = 1,               /**< General Error. Current operation has error: unexpected message etc. */
  ZB_ZCL_TUNNEL_STATUS_ERROR_NO_MEMORY = 2,     /**< No memory */
  ZB_ZCL_TUNNEL_STATUS_ERROR_TIMEOUT = 3,       /**< Timeout. Use for inform User App about when wait answer */
}
zb_zcl_tunnel_status_t;

/** Tunnel i/o operation type, range: 0x00 - 0x03 (2-bit value) */
enum zb_zcl_tunnel_operation_code_e
{
  ZB_ZCL_TUNNEL_OPERATION_TX = 1 << 0,  /**< Current command - send data */
  ZB_ZCL_TUNNEL_OPERATION_RX = 1 << 1   /**< Current command - receive data */
};

/************* Tunnel cluster command structures **************/

/**
 * @brief Tunnel Transfer Data Request command payload
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunnel_transfer_data_req_s
{
  zb_uint8_t tx_flag;   /**< see @ref zb_zcl_tunnel_tx_flags_e */
  /**< byte_num value:
     - if ZB_ZCL_TUNNEL_TX_START is on => total data size to be sent is specified
     - if ZB_ZCL_TUNNEL_TX_CONTINUE is on => current data offset is specified
  */
  zb_uint16_t byte_num;
  /**< length data - size of data */
  zb_uint8_t data_size;
  /**< data - part of transfer data */
}
ZB_PACKED_STRUCT
zb_zcl_tunnel_transfer_data_req_t;

/**
 * @brief Tunnel Transfer Data Request command internal payload
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunnel_transfer_data_req_internal_s
{
  zb_zcl_tunnel_transfer_data_req_t req_header;
  zb_uint8_t *tun_data;
}
ZB_PACKED_STRUCT
zb_zcl_tunnel_transfer_data_req_data_internal_t;

/**
 * @brief Tunnel Transfer Data Request flags
 */
enum zb_zcl_tunnel_tx_flags_e
{
  ZB_ZCL_TUNNEL_TX_START = 1 << 0,      /**< first block - if set then contains total length of transferring data
                                                           else - contains current offset of transferring data */
};

/*! @brief Send Transfer Data command
    @param _buffer - to put packet to
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _prfl_id - profile identifier
    @param _def_resp - enable or disable default response
    @param _cb - callback for getting command send status
    @param _manufacturer_id - Manufacturer code
    @param _flag - command flag, see @ref zb_zcl_tunnel_tx_flags_e
    @param _byte_num - all transfer length or current offset, see @ref zb_zcl_tunnel_transfer_data_req_t
    @param _data_size - data size
    @param _image_data - image data
*/
#define ZB_ZCL_TUNNEL_SEND_TRANSFER_REQ(                                        \
    _buffer, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _cb,     \
    _manufacturer_id, _flag, _byte_num, _data_size, _image_data)                \
{                                                                               \
  zb_uint8_t* __ptr = ZB_ZCL_START_PACKET((_buffer));                           \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(__ptr,                  \
      ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, (_def_resp));\
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(__ptr, ZB_ZCL_GET_SEQ_NUM(),              \
      ZB_ZCL_MANUFACTURER_SPECIFIC, (_manufacturer_id),                         \
            ZB_ZCL_CMD_TUNNEL_TRANSFER_DATA_REQ);                               \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, (_flag));                                      \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(__ptr, (_byte_num));                             \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, (_data_size));                                 \
  if((_data_size)>0)                                                            \
  {                                                                             \
    ZB_ZCL_PACKET_PUT_DATA_N(__ptr, (_image_data), (_data_size));               \
  }                                                                             \
  ZB_ZCL_FINISH_PACKET((_buffer), __ptr)                                       \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                    \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),       \
      ZB_ZCL_CLUSTER_ID_TUNNEL, (_cb));                                         \
}

/** @brief Macro for getting Send Transfer Data command
  * @attention Assumes that ZCL header already cut.
  * @param _data_ptr - pointer to a variable of type @ref
  * zb_zcl_tunnel_transfer_data_req_data_internal_t.
  * @param _buffer containing the packet (by pointer).
  * @param _status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  *
  * @note File data place is placed in buffer, payload saves pointer to data only!
  */
#define ZB_ZCL_TUNNEL_GET_TRANSFER_REQ(_data_ptr, _buffer, _status)     \
{                                                                       \
  zb_zcl_tunnel_transfer_data_req_t *src_ptr =                          \
       (zb_zcl_tunnel_transfer_data_req_t*)zb_buf_begin((_buffer));     \
                                                                        \
  if (zb_buf_len((_buffer)) != sizeof(zb_zcl_tunnel_transfer_data_req_t) \
      + src_ptr->data_size * sizeof(zb_uint8_t) )                       \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
    (_data_ptr)->req_header.tx_flag = src_ptr->tx_flag;                 \
    ZB_HTOLE16(&((_data_ptr)->req_header.byte_num), &(src_ptr->byte_num));   \
    (_data_ptr)->req_header.data_size =  src_ptr->data_size;            \
    (_data_ptr)->tun_data = (zb_uint8_t*)(src_ptr+1);                   \
  }                                                                     \
}

/**
 * @brief Tunnel Transfer Data Response command payload
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunnel_transfer_data_resp_s
{
  zb_uint8_t tun_status;        /**< enum @ref zb_zcl_tunnel_status_e */
}
ZB_PACKED_STRUCT
zb_zcl_tunnel_transfer_data_resp_t;

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
    @param _tun_status - command status, see @ref zb_zcl_tunnel_status_e
*/
#define ZB_ZCL_TUNNEL_SEND_TRANSFER_RESP(                                       \
    _buffer, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _seq, _cb,          \
    _manufacturer_id, _tun_status)                                              \
{                                                                               \
  zb_uint8_t* __ptr = ZB_ZCL_START_PACKET((_buffer));                           \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(__ptr,                 \
      ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);             \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(__ptr, (_seq), ZB_ZCL_MANUFACTURER_SPECIFIC,\
      (_manufacturer_id), ZB_ZCL_CMD_TUNNEL_TRANSFER_DATA_RESP);                \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, (_tun_status));                                \
  ZB_ZCL_FINISH_PACKET((_buffer), __ptr)                                       \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                    \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),       \
      ZB_ZCL_CLUSTER_ID_TUNNEL, (_cb));                                         \
}

/** @brief Macro for getting Send Transfer Data response command
  * @attention Assumes that ZCL header already cut.
  * @param _data_ptr - pointer to a variable of type @ref
  * zb_zcl_tunnel_transfer_data_resp_t.
  * @param _buffer containing the packet (by pointer).
  * @param _status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_TUNNEL_GET_TRANSFER_RESP(_data_ptr, _buffer, _status)            \
{                                                                               \
  zb_zcl_tunnel_transfer_data_resp_t *src_ptr =                                 \
       (zb_zcl_tunnel_transfer_data_resp_t*)zb_buf_begin((_buffer));            \
                                                                                \
  if (zb_buf_len((_buffer)) != sizeof(zb_zcl_tunnel_transfer_data_resp_t)  )    \
  {                                                                             \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                    \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                    \
    (_data_ptr)->tun_status = src_ptr->tun_status;                              \
  }                                                                             \
}

/*! @} */ /* Tunnel cluster commands */


/*********************** Tunnel API **************************/

/*! @name Tunnel cluster commands
    @{
*/

/** Callback to inform user about tx operation status or rx data;
 * @param param - param is reference to a buffer;
 * zb_zcl_tunnel_io_param_t is stored as buffer parameter */
typedef void (ZB_CODE * zb_zcl_tunnel_cb_t)(zb_uint8_t param);

/** Tunnel operation status
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunnel_op_status_s
{
  zb_bitfield_t status  :6; /**< zb_zcl_tunnel_status_e value */
  zb_bitfield_t op_code :2; /**< zb_zcl_tunnel_operation_code_e value */
}
ZB_PACKED_STRUCT
zb_zcl_tunnel_op_status_t;

/** structure to pass parameters for data i/o
 * Save in first of each buffer
 * */
typedef ZB_PACKED_PRE struct zb_zcl_tunnel_io_param_s
{
  zb_uint16_t length; /* total data size*/
  zb_uint16_t peer_addr;
  zb_uint8_t  peer_ep;
  zb_uint8_t  src_ep;
  zb_uint8_t  next_buf; /* pointer to the next buf for fragmented data */
  zb_zcl_tunnel_op_status_t op_status;
  zb_uint8_t seq;
}
ZB_PACKED_STRUCT
zb_zcl_tunnel_io_param_t;

/** Init CTX data table
 * @param manuf_id - manufacturer ID */
void zb_zcl_tunnel_init(zb_uint16_t manuf_id);

/** Register user callback */
void zb_zcl_tunnel_register_cb(zb_zcl_tunnel_cb_t tunnel_cb);

/** Start user data transmit. buf_param is a reference to a buffer;
 * zb_zcl_tunnel_io_param_t is stored as a buffer parameter;
 * data to be transmitted is stored as buffer data */
zb_ret_t zb_zcl_tunnel_transmit_data(zb_uint8_t buf_param);

/** Default user application
 * Contain default handle (usually free buffer) for different Tunnel Status command  */
void zb_zcl_tunnel_default_user_app(zb_uint8_t param);

/*! @} */ /* Tunnel cluster commands */

/**************************** Tunnel internal **********************************/

#define ZB_ZCL_TUNNEL_TIMEOUT   (10*ZB_TIME_ONE_SECOND)

/** @brief Maximum slots for send/receive data */
#define ZB_ZCL_TUNNEL_MAX_IO_SLOT_NUMBER 16

/** @brief Not slots index*/
#define ZB_ZCL_TUNNEL_IO_SLOT_UNKNOWN    0xff

typedef ZB_PACKED_PRE struct zb_zcl_tunnel_io_slot_s
{
  zb_zcl_tunnel_io_param_t io_param;
  zb_uint16_t offset; /* length of current sent/received data */
  zb_uint8_t seq;
}
ZB_PACKED_STRUCT
zb_zcl_tunnel_io_slot_t;

/** struct for next buffer in multi-buffer chain*/
typedef ZB_PACKED_PRE struct zb_zcl_tunnel_io_slot_continue_s
{
  zb_uint8_t next_buf;
}
ZB_PACKED_STRUCT
zb_zcl_tunnel_io_slot_continue_t;


/* internal cluster data - context info */
typedef struct zb_zcl_tunnel_context_s
{
  zb_uint16_t manufacturer_id; /* manufacturer ID - should be initialized */
  zb_zcl_tunnel_cb_t tunnel_cb; /* user callback - should be registered */
  /* store reference to i/o buffer: tx or rx;
   * zb_zcl_tunnel_io_param_t is stored as buffer parameter to keep status info */
  zb_uint8_t tunnel_io_slot[ZB_ZCL_TUNNEL_MAX_IO_SLOT_NUMBER];
}
zb_zcl_tunnel_context_t;

zb_ret_t zb_zcl_tunnel_transfer_data_req_handler(zb_uint8_t buf_param);
zb_ret_t zb_zcl_tunnel_transfer_data_resp_handler(zb_uint8_t buf_param);
void zb_zcl_tunnel_transfer_data_resp_send(zb_uint8_t param, zb_uint16_t endpoint16);

void zb_zcl_tunnel_send_fist_block(zb_uint8_t param, zb_uint16_t index16);
void zb_zcl_tunnel_timeout(zb_uint8_t index);
void zb_zcl_tunnel_invoke_user_app(zb_uint8_t param);

zb_ret_t zb_zcl_process_tunnel_default_response_commands(zb_uint8_t param);

zb_bool_t zb_zcl_process_tunnel_specific_commands(zb_uint8_t param);

/*! @} */ /* addtogroup */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_tunnel_init_server(void);
void zb_zcl_tunnel_init_client(void);
#define ZB_ZCL_CLUSTER_ID_TUNNEL_SERVER_ROLE_INIT zb_zcl_tunnel_init_server
#define ZB_ZCL_CLUSTER_ID_TUNNEL_CLIENT_ROLE_INIT zb_zcl_tunnel_init_client

#endif /* ZB_ZCL_TUNNEL_H */
