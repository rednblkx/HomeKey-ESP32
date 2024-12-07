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
/* PURPOSE: Tunneling cluster definitions
*/
#ifndef ZB_ZCL_TUNNELING_H
#define ZB_ZCL_TUNNELING_H 1

#define ZB_ZCL_TUNNELING_MAX_INCOMING_TRANSFER_SIZE (ZB_ASDU_MAX_FRAG_LEN - 3 /* ZCL cmd */ - 2 /* Tunnel_ID */)
#define ZB_ZCL_TUNNELING_SRV_TABLE_SIZE 10

#include "zcl/zb_zcl_common.h"

/** @cond DOXYGEN_ZCL_SECTION */


/** @addtogroup ZB_ZCL_TUNNELING
 *  @{
 *    @details
 *    The tunneling cluster provides an interface for tunneling protocols. It is
 *    comprised of commands and attributes required to transport any existing
 *    metering communication protocol within the payload of standard Zigbee
 *    frames (including the handling of issues such as addressing, fragmentation
 *    and flow control). Examples for such protocols are DLMS/COSEM, IEC61107,
 *    ANSI C12, M-Bus, ClimateTalk etc.\n
 *    This cluster transmits custom data between peers\n
 *    Model: One (client) to More (servers)\n
 */


/** @defgroup ZB_ZCL_TUNNELING_ATTRS Tunneling cluster attributes
 *  @{
 */

/** @brief Tunneling cluster attribute identifiers
 *  @see SE spec, subclause D.6.2.2
 */
enum zb_zcl_tunneling_attr_e
{
  /*! @brief CloseTunnelTimeout defines the minimum number of seconds that the
   * server waits on an inactive tunnel before closing it on its own and freeing
   * its resources (without waiting for the CloseTunnel command from the client).
   * Inactive means here that the timer is re-started with each new reception of
   * a command.0x0000 is an invalid value.
   * @see ZB_ZCL_TUNNELING_CLOSE_TUNNEL_TIMEOUT_MIN_VALUE
   * @see ZB_ZCL_TUNNELING_CLOSE_TUNNEL_TIMEOUT_DEFAULT_VALUE
   */
  ZB_ZCL_ATTR_TUNNELING_CLOSE_TUNNEL_TIMEOUT_ID = 0
};

/** @brief Default value for Tunneling cluster revision global attribute */
#define ZB_ZCL_TUNNELING_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Min value for the @e CloseTunnelTimeout attribute
 *  @see ZB_ZCL_ATTR_TUNNELING_CLOSE_TUNNEL_TIMEOUT_ID */
#define ZB_ZCL_TUNNELING_CLOSE_TUNNEL_TIMEOUT_MIN_VALUE 0x0001

/** @brief Default value for the @e CloseTunnelTimeout attribute
 *  @see ZB_ZCL_ATTR_TUNNELING_CLOSE_TUNNEL_TIMEOUT_ID */
#define ZB_ZCL_TUNNELING_CLOSE_TUNNEL_TIMEOUT_DEFAULT_VALUE 0xFFFF


/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TUNNELING_CLOSE_TUNNEL_TIMEOUT_ID(data_ptr) \
{                                                                                      \
  ZB_ZCL_ATTR_TUNNELING_CLOSE_TUNNEL_TIMEOUT_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                                \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                  \
  (void*) data_ptr                                                                     \
}

/** @endcond */ /* internals_doc */

/** @def ZB_ZCL_DECLARE_TUNNELING_ATTRIB_LIST(attr_list, close_tunnel_timeout)
  * @brief Declare attribute list for Tunneling cluster
  * @param[in] attr_list - attribute list name
  * @param[in] close_tunnel_timeout - pointer to variable to store On/Tunneling attribute value
  */
#define ZB_ZCL_DECLARE_TUNNELING_ATTRIB_LIST(attr_list, close_tunnel_timeout)                 \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_TUNNELING)              \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TUNNELING_CLOSE_TUNNEL_TIMEOUT_ID, (close_tunnel_timeout)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**
 *  @brief Tunneling cluster attributes
 */
typedef struct zb_zcl_tunneling_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_TUNNELING_CLOSE_TUNNEL_TIMEOUT_ID
   * @see ZB_ZCL_ATTR_TUNNELING_CLOSE_TUNNEL_TIMEOUT_ID
   */
  zb_uint16_t close_tunnel_timeout;
} zb_zcl_tunneling_attrs_t;


/** @brief Declare attribute list for Tunneling cluster cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - pointer to @ref zb_zcl_tunneling_attrs_s structure
 */
#define ZB_ZCL_DECLARE_TUNNELING_ATTR_LIST(attr_list, attrs) \
  ZB_ZCL_DECLARE_TUNNELING_ATTRIB_LIST(attr_list, &attrs.close_tunnel_timeout )

/** @cond internals_doc */

#define ZB_ZCL_TUNNELING_TUNNEL_ID_INVALID_VALUE 0xFFFF

#define ZB_ZCL_TUNNELING_MANUFACTURER_CODE_NOT_USED 0xFFFF

/** @endcond */ /* internals_doc */

/** @} */ /* ZB_ZCL_TUNNELING_ATTRS */


/** @defgroup ZB_ZCL_TUNNELING_COMMANDS Tunneling cluster commands
 *  @{
 */

/** @brief Tunnel cluster commands received
 *  @see SE spec, subclause D.6.2.4
 */
enum zb_zcl_tunneling_cli_cmd_e
{

  ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL                 = 0x00,  /**< @brief  @e RequestTunnel is the client command used to setup a tunnel
                                                                    * association with the server. The request payload specifies the protocol
                                                                    * identifier for the requested tunnel, a manufacturer code in case of proprietary
                                                                    * protocols and the use of flow control for streaming protocols.
                                                                    * @see zb_zcl_tunneling_request_tunnel_s
                                                                    */

  ZB_ZCL_TUNNELING_CLI_CMD_CLOSE_TUNNEL                   = 0x01,  /**< @brief  Client command used to close the tunnel with the server. The parameter
                                                                    * in the payload specifies the tunnel identifier of the tunnel that has to be
                                                                    * closed. The server leaves the tunnel open and the assigned resources allocated
                                                                    * until the client sends the @e CloseTunnel command or the @e CloseTunnelTimeout
                                                                    * fires.
                                                                    * @see zb_zcl_tunneling_close_tunnel_s
                                                                    */

  ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA                  = 0x02,  /**< @brief Command that indicates (if received) that the client has sent data
                                                                    * to the server. The data itself is contained within the payload.
                                                                    * @see zb_zcl_tunneling_transfer_data_payload_s
                                                                    */

  ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA_ERROR            = 0x03,  /**< @brief  This command is generated by the receiver of a @e TransferData command
                                                                    * if the tunnel status indicates that something is wrong.
                                                                    * @details There are three cases in which @e TransferDataError is sent:\n
                                                                    * <ul>
                                                                    *   <li>The @e TransferData received contains a TunnelID that does not match to any
                                                                    *     of the active tunnels of the receiving device. This could happen if a
                                                                    *     (sleeping) device sends a @e TransferData command to a tunnel that has been
                                                                    *     closed by the server after the @e CloseTunnelTimeout.
                                                                    *   </li>
                                                                    *   <li>The @e TransferData received contains a proper TunnelID of an active
                                                                    *     tunnel, but the device sending the data does not match to it.
                                                                    *   </li>
                                                                    *   <li> The @e TransferData received contains more data
                                                                    *      @e MaximumIncomingTransferSize of the receiving device.
                                                                    *   </li>
                                                                    * </ul>
                                                                    * @see zb_zcl_tunneling_transfer_data_error_s
                                                                    */

  ZB_ZCL_TUNNELING_CLI_CMD_ACK_TRANSFER_DATA              = 0x04,  /**< Command sent in response to each TransferData command
                                                                    * in case – and only in case – flow control has been
                                                                    * requested by the client in the TunnelRequest command and
                                                                    * is supported by both tunnel endpoints. The response
                                                                    * payload indicates the number of octets that may still be
                                                                    * received by the receiver.
                                                                    * @see zb_zcl_tunneling_ack_transfer_data_s
                                                                    */

  ZB_ZCL_TUNNELING_CLI_CMD_READY_DATA                     = 0x05,  /**< The ReadyData command is generated – after a receiver
                                                                    * had to stop the dataflow using the AckTransferData(0)
                                                                    * command – to indicate that the device is now ready to
                                                                    * continue receiving data. The parameter NumberOfOctetsLeft
                                                                    *  gives a hint on how much space is left for the next
                                                                    * data transfer. The ReadyData command is only issued if
                                                                    * flow control is enabled.
                                                                    * @see zb_zcl_tunneling_ready_data_s
                                                                    */

  ZB_ZCL_TUNNELING_CLI_CMD_GET_SUPPORTED_TUNNEL_PROTOCOLS = 0x06,  /**< Get Supported Tunnel Protocols is the client command
                                                                    * used to determine the tunnel protocols supported on another device.
                                                                    * @see zb_zcl_tunneling_get_supported_tunnel_protocols_s
                                                                    */
};


/** @brief Tunnel cluster commands generated
 *  @see SE spec, subclause D.6.2.5
 */
enum zb_zcl_tunneling_srv_cmd_e
{
  ZB_ZCL_TUNNELING_SRV_CMD_REQUEST_TUNNEL_RESPONSE              = 0x00,  /**< @brief @e RequestTunnelResponse is sent by the server in response to a @e RequestTunnel
                                                                          * command previously received from the client. The response contains the
                                                                          * status of the @e RequestTunnel command and a tunnel identifier corresponding
                                                                          * to the tunnel that has been set-up in the server in case of success.
                                                                          * @see zb_zcl_tunneling_request_tunnel_response_s
                                                                          */

  ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA                        = 0x01,  /**< @brief Command that transfers data from server to the client. The data
                                                                          * itself has to be placed within the payload.
                                                                          * @see zb_zcl_tunneling_transfer_data_payload_s
                                                                          */

  ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA_ERROR                  = 0x02,  /**< @copydoc ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA_ERROR */

  ZB_ZCL_TUNNELING_SRV_CMD_ACK_TRANSFER_DATA                    = 0x03,  /**< @copydoc ZB_ZCL_TUNNELING_CLI_CMD_ACK_TRANSFER_DATA */

  ZB_ZCL_TUNNELING_SRV_CMD_READY_DATA                           = 0x04,  /**< @copydoc ZB_ZCL_TUNNELING_CLI_CMD_READY_DATA */

  ZB_ZCL_TUNNELING_SRV_CMD_SUPPORTED_TUNNEL_PROTOCOLS_RESPONSE  = 0x05,  /**< Supported Tunnel Protocols Response is sent
                                                                          * in response to a Get Supported Tunnel Protocols
                                                                          * command previously received. The response contains
                                                                          * a list of tunnel protocols supported by the device;
                                                                          * the payload of the response should be capable of
                                                                          * holding up to 16 protocols.
                                                                          * @see zb_zcl_tunneling_supported_tunnel_protocols_response_s
                                                                          */

  ZB_ZCL_TUNNELING_SRV_CMD_TUNNEL_CLOSURE_NOTIFICATION          = 0x06,  /**< TunnelClosureNotification is sent by the server
                                                                          * to indicate that a tunnel has been closed due to
                                                                          * expiration of a CloseTunnelTimeout.
                                                                          * @see zb_zcl_tunneling_tunnel_closure_notification_s
                                                                          */
};

/** @cond internals_doc */
/* TUNNELING cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_TUNNELING_SERVER_ROLE_GENERATED_CMD_LIST                      \
                                      ZB_ZCL_TUNNELING_SRV_CMD_REQUEST_TUNNEL_RESPONSE, \
                                      ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA,           \
                                      ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA_ERROR

#define ZB_ZCL_CLUSTER_ID_TUNNELING_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_TUNNELING_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_TUNNELING_CLIENT_ROLE_GENERATED_CMD_LIST                  \
                                      ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL,      \
                                      ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA,       \
                                      ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA_ERROR, \
                                      ZB_ZCL_TUNNELING_CLI_CMD_CLOSE_TUNNEL

#define ZB_ZCL_CLUSTER_ID_TUNNELING_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_TUNNELING_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/** @brief @e TunnelStatus parameter values.
 *  @see SE spec, Table D-127
 *  @see @ref ZB_ZCL_TUNNELING_SRV_CMD_REQUEST_TUNNEL_RESPONSE "RequestTunnelResponse"
 *  @see zb_zcl_tunneling_request_tunnel_response_s::tunnel_status
 */
enum zb_zcl_tunneling_tunnel_status_e
{

  ZB_ZCL_TUNNELING_STATUS_SUCCESS                    = 0,  /**< The tunnel has been opened and may now be used to transfer data in
                                                            * both directions.
                                                            */

  ZB_ZCL_TUNNELING_STATUS_BUSY                       = 1,  /**< The server is busy and cannot create a new tunnel at the moment.
                                                            * The client may try again after a recommended timeout of 3 minutes.
                                                            */

  ZB_ZCL_TUNNELING_STATUS_NO_MORE_IDS                = 2,   /**< The server has no more resources to setup requested tunnel. Clients
                                                             * should close any open tunnels before retrying.
                                                             */

  ZB_ZCL_TUNNELING_STATUS_PROTOCOL_NOT_SUPPORTED     = 3,   /**< The server does not support the protocol that has been requested
                                                             * in the ProtocolID parameter of the RequestTunnel command.
                                                             */

  ZB_ZCL_TUNNELING_STATUS_FLOW_CONTROL_NOT_SUPPORTED = 4,   /**< Flow control has been requested by the client in the RequestTunnel
                                                             * command but cannot be provided by the server (missing resources or no
                                                             * support).
                                                             */
};


/** @brief @e TransferDataStatus parameter values.
 *  @see SE spec, Table D-125
 *  @see zb_zcl_tunneling_transfer_data_error_t::transfer_data_status
 */
enum zb_zcl_tunneling_transfer_data_status_e
{

  /** @brief The @e TransferData command contains a @e TunnelID of a non-existent
   *  tunnel.
   */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_STATUS_NO_SUCH_TUNNEL = 0x00,

  /** @brief The @e TransferData command contains a @e TunnelID that does not match the
   *  device sending the data.
   */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_STATUS_WRONG_DEVICE   = 0x01,

  /** @brief The @e TransferData command contains more data than indicated by the
   *  @e MaximumIncomingTransferSize of the receiving device
   */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_STATUS_DATA_OVERFLOW  = 0x02,

  /* Custom Internal Error status */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_INTERNAL_ERROR        = 0xFE,

  /* Custom OK status (does not sent in pkt) */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_STATUS_OK             = 0xFF,
};

/** @brief Tunneling @e ProtocolID Enumerations
 *  @see SE spec, subclause D.6.2.4.1.2, Table D-124
 *  @see @ref ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL "RequestTunnel"
 *  @see zb_zcl_tunneling_request_tunnel_s::protocol_id
 */
typedef enum zb_zcl_tunneling_protocol_id_e
{
  ZB_ZCL_TUNNELING_PROTOCOL_DLMS_COSEM           = 0,     /**< DLMS/COSEM (IEC 62056) */
  ZB_ZCL_TUNNELING_PROTOCOL_IEC_61107            = 1,     /**< IEC 61107 */
  ZB_ZCL_TUNNELING_PROTOCOL_ANSI_C12             = 2,     /**< ANSI C12 */
  ZB_ZCL_TUNNELING_PROTOCOL_M_BUS                = 3,     /**< M-BUS */
  ZB_ZCL_TUNNELING_PROTOCOL_SML                  = 4,     /**< SML */
  ZB_ZCL_TUNNELING_PROTOCOL_CLIMATE_TALK         = 5,     /**< ClimateTalk */
  ZB_ZCL_TUNNELING_PROTOCOL_GB_HRGP              = 6,     /**< GB-HRGP */
  ZB_ZCL_TUNNELING_PROTOCOL_IPv4                 = 7,     /**< IP v4 */
  ZB_ZCL_TUNNELING_PROTOCOL_IPv6                 = 8,     /**< IP v6 */
  /* 9 to 199 reserved for future growth */
  ZB_ZCL_TUNNELING_PROTOCOL_MANUFACTURER_DEFINED = 200,   /**< 200 to 254 manufacturer-defined protocols */
  ZB_ZCL_TUNNELING_PROTOCOL_RESERVED             = 255    /**< reserved value */
} zb_zcl_tunneling_protocol_id_t;

/************* Tunnel cluster command structures **************/

/** @brief  @ref ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL "RequestTunnel" command payload
 *  @see SE spec, subclause D.6.2.4.1.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_request_tunnel_s
{

  /** @brief An enumeration representing the identifier of the metering
   * communication protocol for which the tunnel is requested.
   * @see zb_zcl_tunneling_protocol_id_e
   */
  zb_uint8_t protocol_id;                  /* (M) */

  /** @brief  A code that is allocated by the Zigbee Alliance, relating the
   * manufacturer to a device and – for the tunneling - a manufacturer specific
   * protocol. The parameter is ignored when the @e ProtocolID value is less than 200.
   *  This allows for 55 manufacturer-defined protocols for each manufacturer to
   * be defined. A value of 0xFFFF indicates that the Manufacturer Code is not used.
   */
  zb_uint16_t manufacturer_code;           /* (M) */

  /** @brief A boolean type parameter that indicates whether flow control support
   * is requested from the tunnel (ZB_TRUE) or not (ZB_FALSE). The default value is
   * FALSE (no flow control).
   */
  zb_bool_t flow_control_support;          /* (M) */

  /** @brief A value that defines the size, in octets, of the maximum data packet
   * that can be transferred to the client in the payload of a single
   * @e TransferData command.
   */
  zb_uint16_t max_incoming_transfer_size;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_tunneling_request_tunnel_t;


void zb_zcl_tunneling_set_max_incoming_to_cli_transfer_size(zb_uint16_t transfer_size);


/** @def ZB_ZCL_TUNNELING_SEND_REQUEST_TUNNEL
 *  @brief Send @ref ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL "RequestTunnel" command
 *  @param _param - Reference to buffer.
 *  @param _addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _ep - Current endpoint.
 *  @param _prfl_id - profile identifier
 *  @param _def_resp - enable/disable default zcl response
 *  @param _cb - Callback which should be called when the ZCL stack receives
 *  APS ack.
 *  @param _protocol_id - @ref zb_zcl_tunneling_request_tunnel_t::protocol_id value
 *  @param _manufacturer_code - @ref zb_zcl_tunneling_request_tunnel_t::manufacturer_code value
 *  @param _flow_control_support - @ref zb_zcl_tunneling_request_tunnel_t::flow_control_support value
 *  @param _max_incoming_transfer_size - @ref zb_zcl_tunneling_request_tunnel_t::max_incoming_transfer_size value
 *  @par Usage
 *  @n Example of sending @ref ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL "RequestTunnel" command
 */
#define ZB_ZCL_TUNNELING_SEND_REQUEST_TUNNEL(                                   \
    _param, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _cb,      \
    _protocol_id, _manufacturer_code, _flow_control_support,                    \
    _max_incoming_transfer_size)                                                  \
{                                                                               \
  zb_zcl_tunneling_set_max_incoming_to_cli_transfer_size(                         \
    _max_incoming_transfer_size);                                                 \
  {                                                                               \
    zb_bufid_t _buffer = _param;                                  \
    zb_uint8_t* __ptr = zb_zcl_start_command_header(_buffer,                      \
    ZB_ZCL_CONSTRUCT_FRAME_CONTROL(ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,          \
                                   ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,            \
                                   ZB_ZCL_FRAME_DIRECTION_TO_SRV, (_def_resp)), \
    0,                          /* No manuf_code */                             \
    ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL, NULL);                                 \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, (_protocol_id));                               \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(__ptr, (_manufacturer_code));                    \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, (_flow_control_support));                      \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(__ptr, (_max_incoming_transfer_size));           \
    ZB_ZCL_FINISH_PACKET(_buffer, __ptr)                                          \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                    \
      _buffer, (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),           \
      ZB_ZCL_CLUSTER_ID_TUNNELING, (_cb));                                      \
  }                                                                               \
}


/** @brief Macro for parsing payload of @ref ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL "RequestTunnel" command
 * @param _data_ptr - pointer to @ref zb_zcl_tunneling_request_tunnel_t structure
 * @param _buffer - pointer to buffer where @e RequestTunnel command's payload is stored.
 * @param _status - status of payload parsing
 */
#define ZB_ZCL_TUNNELING_GET_REQUEST_TUNNEL(_data_ptr, _buffer, _status)        \
{                                                                               \
  zb_zcl_tunneling_request_tunnel_t *src_ptr =                                  \
    (zb_zcl_tunneling_request_tunnel_t*)zb_buf_begin((_buffer));                \
                                                                                \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_tunneling_request_tunnel_t))        \
  {                                                                             \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                    \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                    \
    (_data_ptr)->protocol_id = src_ptr->protocol_id;                            \
    ZB_HTOLE16(&(_data_ptr)->manufacturer_code, &(src_ptr->manufacturer_code)); \
    (_data_ptr)->flow_control_support = src_ptr->flow_control_support;          \
    ZB_HTOLE16(&(_data_ptr)->max_incoming_transfer_size,                        \
               &(src_ptr->max_incoming_transfer_size));                         \
  }                                                                             \
}


/**
 * @brief @ref ZB_ZCL_TUNNELING_CLI_CMD_CLOSE_TUNNEL "CloseTunnel" command payload
 * @see SE spec, subclause D.6.2.4.2.2.
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_close_tunnel_s
{
  /** @brief The identifier of the tunnel that shall be closed. It is the same number
   * that has been previously returned in the response to a @ref ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL "RequestTunnel" command.
   * Valid numbers range between 0..65535 and must correspond to a tunnel that is
   * still active and maintained by the server.
   */
  zb_uint16_t tunnel_id;
}
ZB_PACKED_STRUCT
zb_zcl_tunneling_close_tunnel_t;


/** @brief @ref ZB_ZCL_TUNNELING_CLI_CMD_CLOSE_TUNNEL "CloseTunnel" command
 * @param _param - Reference to buffer.
 * @param _addr - Address of the device to send command to.
 * @param _dst_addr_mode - Address mode for _dst_addr.
 * @param _dst_ep - Destination endpoint.
 * @param _ep - Current endpoint.
 * @param _prfl_id - profile identifier
 * @param _def_resp - enable/disable default zcl response
 * @param _cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 * @param _tunnel_id - @ref zb_zcl_tunneling_close_tunnel_t::tunnel_id value
 * @par Usage
 * @n Example of sending @ref ZB_ZCL_TUNNELING_CLI_CMD_CLOSE_TUNNEL "CloseTunnel" command
 */
#define ZB_ZCL_TUNNELING_SEND_CLOSE_TUNNEL(                                     \
    _param, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _cb,      \
    _tunnel_id)                                                                 \
{                                                                               \
  zb_bufid_t _buffer = _param;                                  \
  zb_uint8_t* __ptr = zb_zcl_start_command_header(_buffer,                      \
    ZB_ZCL_CONSTRUCT_FRAME_CONTROL(ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,          \
                                   ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,            \
                                   ZB_ZCL_FRAME_DIRECTION_TO_SRV, (_def_resp)), \
    0,                          /* No manuf_code */                             \
    ZB_ZCL_TUNNELING_CLI_CMD_CLOSE_TUNNEL, NULL);                                   \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(__ptr, (_tunnel_id));                            \
  ZB_ZCL_FINISH_PACKET(_buffer, __ptr)                                          \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                    \
      _buffer, (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),         \
      ZB_ZCL_CLUSTER_ID_TUNNELING, (_cb));                                      \
}


/** @brief Macro for parsing payload of @ref ZB_ZCL_TUNNELING_CLI_CMD_CLOSE_TUNNEL "CloseTunnel" command
 * @param _data_ptr - pointer to @ref zb_zcl_tunneling_close_tunnel_t structure
 * @param _buffer - pointer to buffer where @e CloseTunnel command's payload is stored.
 * @param _status - status of payload parsing
 */
#define ZB_ZCL_TUNNELING_GET_CLOSE_TUNNEL(_data_ptr, _buffer, _status) \
{                                                                      \
  zb_zcl_tunneling_close_tunnel_t *src_ptr =                           \
    (zb_zcl_tunneling_close_tunnel_t*)zb_buf_begin((_buffer));         \
                                                                       \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_tunneling_close_tunnel_t)) \
  {                                                                    \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                           \
  }                                                                    \
  else                                                                 \
  {                                                                    \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                           \
    ZB_HTOLE16(&(_data_ptr)->tunnel_id, &(src_ptr->tunnel_id));        \
  }                                                                    \
}


/**
 * @brief @e TransferData commands header data.
 * @see SE spec, subclause D.6.2.4.3.2
 * @see @ref zb_zcl_tunneling_transfer_data_payload_t::hdr
 * @see @ref ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA "TransferData" client command
 * @see @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA "TransferData" server command
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_transfer_data_hdr_s
{
  /** @brief number between 0..65535 that uniquely identifies the tunnel that has
   * been allocated in the server triggered through the RequestTunnel command.
   * This ID must be used to send data through the tunnel or passed with any
   * commands concerning that specific tunnel.
   */
  zb_uint16_t tunnel_id;
}
ZB_PACKED_STRUCT
zb_zcl_tunneling_transfer_data_hdr_t;


typedef ZB_PACKED_PRE struct zb_zcl_tunneling_srv_rec_s
{
  zb_uint16_t dst_addr;
  zb_uint8_t  dst_ep;
  zb_uint8_t  src_ep;
  /** */
  zb_zcl_tunneling_request_tunnel_t tunnel_options;
  zb_uint16_t close_tunnel_timeout;
  zb_uint16_t max_incoming_to_srv_transfer_size;
}
ZB_PACKED_STRUCT zb_zcl_tunneling_srv_rec_t;


typedef ZB_PACKED_PRE struct zb_zcl_tunnel_request_params_out_s
{
  zb_uint8_t tunnel_status;
  zb_uint16_t max_incoming_to_srv_transfer_size;
} ZB_PACKED_STRUCT zb_zcl_tunnel_request_params_out_t;


/** @brief @e TransferData payload.
 * This structure presents custom implementation of Transfer Data command payload defined in SE spec.
 * @see SE spec, subclause D.6.2.4.3.2 and D.6.2.5.2
 * @see @ref ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA "TransferData" client command
 * @see @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA "TransferData" server command
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_transfer_data_payload_s
{

  /** @ref zb_zcl_tunneling_transfer_data_hdr_t
   */
  zb_zcl_tunneling_transfer_data_hdr_t hdr;

  /** Size of @ref tun_data */
  zb_uint8_t data_size;

  /** @brief Octet containing the data to be transferred through the tunnel in
   * the format of the communication protocol for which the tunnel has been
   * requested and opened. The payload contains the assembled data exactly as it
   * was sent by the client. Theoretically, its length is solely limited through
   * the fragmentation algorithm and the RX/TX transfer buffer sizes within the
   * communication partners. The content of the payload is up to the application
   * sending the data. It is neither guaranteed, that it contains a complete PDU
   * nor is any other assumption on its internal format made. This is left up to
   * the implementer of the specific protocol tunnel behavior.
   */
  zb_uint8_t *tun_data;
}
ZB_PACKED_STRUCT
zb_zcl_tunneling_transfer_data_payload_t;

/** @cond internals_doc */
zb_ret_t zb_zcl_tunneling_client_send_transfer_data(zb_uint8_t param, zb_uint8_t ep, zb_uint16_t prfl_id,
                                             zb_uint8_t def_resp, zb_callback_t cb, zb_uint16_t tunnel_id,
                                             zb_uint16_t data_size, zb_uint8_t *image_data);
zb_ret_t zb_zcl_tunneling_server_send_transfer_data(zb_uint8_t param, zb_uint8_t ep, zb_uint16_t prfl_id,
                                              zb_uint8_t def_resp, zb_callback_t cb, zb_uint16_t tunnel_id,
                                              zb_uint16_t data_size, zb_uint8_t *image_data);

/** @endcond */ /* internal_doc */

/*! Send @e TransferData command from Tunneling server to client
 * @param _param - Reference to buffer.
 * @param _ep - Current endpoint.
 * @param _prfl_id - profile identifier
 * @param _def_resp - enable/disable default zcl response
 * @param _cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 * @param _tunnel_id - @ref zb_zcl_tunneling_transfer_data_hdr_t::tunnel_id value
 * @param _data_size - @ref zb_zcl_tunneling_transfer_data_payload_t::data_size value
 * @param _image_data - tun_data from @ref zb_zcl_tunneling_transfer_data_payload_t::tun_data value
 * @par Usage
 * @n Example of sending @e TransferData command
 * @see @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA "TransferData" server command
 */
#define ZB_ZCL_TUNNELING_SERVER_SEND_TRANSFER_DATA(                                 \
  _param, _ep, _prfl_id, _def_resp, _cb,                                            \
  _tunnel_id, _data_size, _image_data)                                              \
  zb_zcl_tunneling_server_send_transfer_data(_param, _ep, _prfl_id, _def_resp, _cb, \
  _tunnel_id, _data_size, _image_data)

/*! Send @e TransferData command from Tunneling client to server
 * @param _param - Reference to buffer.
 * @param _ep - Current endpoint.
 * @param _prfl_id - profile identifier
 * @param _def_resp - enable/disable default zcl response
 * @param _cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 * @param _tunnel_id - @ref zb_zcl_tunneling_transfer_data_hdr_t::tunnel_id value
 * @param _data_size - @ref zb_zcl_tunneling_transfer_data_payload_t::data_size value
 * @param _image_data - tun_data from @ref zb_zcl_tunneling_transfer_data_payload_t::tun_data value
 * @par Usage
 * @n Example of sending @e TransferData command
 * @see @ref ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA "TransferData" client command
 */
#define ZB_ZCL_TUNNELING_CLIENT_SEND_TRANSFER_DATA(                                 \
  _param, _ep, _prfl_id, _def_resp, _cb,                                            \
  _tunnel_id, _data_size, _image_data)                                              \
  zb_zcl_tunneling_client_send_transfer_data(_param, _ep, _prfl_id, _def_resp, _cb, \
  _tunnel_id, _data_size, _image_data)

/** @brief Macro for parsing payload of @e TransferData command
 * @param _data_ptr - pointer to @ref zb_zcl_tunneling_transfer_data_payload_t structure
 * @param _buffer - pointer to buffer where @e TransferData command's payload is stored.
 * @param _status - status of payload parsing
 * @see @ref ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA "TransferData" client command
 * @see @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA "TransferData" server command
 */
#define ZB_ZCL_TUNNELING_GET_TRANSFER_DATA(_data_ptr, _buffer, _status)     \
{                                                                           \
  zb_uint8_t *src_ptr = (zb_uint8_t*)zb_buf_begin((_buffer));               \
                                                                            \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_tunneling_transfer_data_hdr_t)) \
  {                                                                         \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                \
    ZB_HTOLE16(&(_data_ptr)->hdr.tunnel_id, src_ptr);                       \
    (_data_ptr)->data_size = zb_buf_len((_buffer)) -                        \
      sizeof(zb_zcl_tunneling_transfer_data_hdr_t);                         \
    (_data_ptr)->tun_data = (zb_uint8_t*)(src_ptr +                         \
      sizeof(zb_zcl_tunneling_transfer_data_hdr_t));                        \
  }                                                                         \
}


/** @brief @e TransferDataError command payload
 *  @see SE spec, subclause D.6.2.4.4.2
 *  @see Client @ref ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA_ERROR "TransferDataError" command
 *  @see Server @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA_ERROR "TransferDataError" command
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_transfer_data_error_s
{

  /** @brief A number between 0..65535 that uniquely identifies the tunnel that
   * has been allocated in the server triggered through the RequestTunnel command.
   * This ID must be used for the data transfer through the tunnel or passed with
   * any commands concerning that specific tunnel.
   */
  zb_uint16_t tunnel_id;

  /** @brief The @e TransferDataStatus parameter indicates the error that occurred
   *  within the receiver after the last @e TransferData command.
   *  @see zb_zcl_tunneling_transfer_data_status_e
   */
  zb_uint8_t transfer_data_status;
}
ZB_PACKED_STRUCT zb_zcl_tunneling_transfer_data_error_t;


/*! @brief Send client @ref ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA_ERROR "TransferDataError" command and
 * server @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA_ERROR "TransferDataError" command
 * @param _buffer - pointer to buffer.
 * @param _addr - Address of the device to send command to.
 * @param _dst_addr_mode - Address mode for _dst_addr.
 * @param _dst_ep - Destination endpoint.
 * @param _ep - Current endpoint.
 * @param _prfl_id - profile identifier
 * @param _def_resp - enable/disable default zcl response
 * @param _cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 * @param _seq_num - ZCL transaction sequence number
 * @param _tunnel_id - @ref zb_zcl_tunneling_transfer_data_error_t::tunnel_id value
 * @param _transfer_data_status - @ref zb_zcl_tunneling_transfer_data_error_t::transfer_data_status value
 * @param _command_id - Tunneling cluster's command identifier.
 * @param _direction - command direction.
 */
#define ZB_ZCL_TUNNELING_SEND_TRANSFER_DATA_ERROR(                                  \
  _buffer, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _cb, _seq_num, \
  _tunnel_id, _transfer_data_status, _command_id, _direction)                       \
{                                                                                   \
  zb_uint8_t* __ptr = ZB_ZCL_START_PACKET((_buffer));                               \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(__ptr,                     \
    _direction,                                                                     \
    ZB_ZCL_NOT_MANUFACTURER_SPECIFIC);                                              \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(__ptr, _seq_num, _command_id);                    \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(__ptr, (_tunnel_id));                                \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, (_transfer_data_status));                          \
  ZB_ZCL_FINISH_PACKET((_buffer), __ptr)                                            \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                        \
      (_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),           \
      ZB_ZCL_CLUSTER_ID_TUNNELING, (_cb));                                          \
}


/** @brief Macro for parsing payload of @e TransferDataError command
 * @param _data_ptr - pointer to @ref zb_zcl_tunneling_transfer_data_error_t structure
 * @param _buffer - pointer to buffer where @e TransferDataError command's payload is stored.
 * @param _status - status of payload parsing
 * @see @ref ZB_ZCL_TUNNELING_CLI_CMD_TRANSFER_DATA_ERROR "TransferDataError" client command
 * @see @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA_ERROR "TransferDataError" server command
 */
#define ZB_ZCL_TUNNELING_GET_TRANSFER_DATA_ERROR(_data_ptr, _buffer, _status) \
{                                                                             \
  zb_zcl_tunneling_transfer_data_error_t *src_ptr =                           \
    (zb_zcl_tunneling_transfer_data_error_t*)zb_buf_begin((_buffer));         \
                                                                              \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_tunneling_transfer_data_error_t)) \
  {                                                                           \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                  \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                  \
    ZB_HTOLE16(&(_data_ptr)->tunnel_id, &(src_ptr->tunnel_id));               \
    (_data_ptr)->transfer_data_status = src_ptr->transfer_data_status;        \
  }                                                                           \
}


/** @brief @ref ZB_ZCL_TUNNELING_SRV_CMD_REQUEST_TUNNEL_RESPONSE "RequestTunnelResponse" command payload
 *  @see SE spec, subclause D.6.2.5.1.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_request_tunnel_response_s
{

  /** @brief  A number between 0..65535 that uniquely identifies the tunnel that
   * has been allocated in the server triggered through the @ref ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL "RequestTunnel"
   * command.
   * This ID must now be used to send data through this tunnel (@e TunnelID,
   * @e TransferData) and is also required to close the tunnel again (@ref ZB_ZCL_TUNNELING_CLI_CMD_CLOSE_TUNNEL "CloseTunnel").
   * If the command has failed, the @e TunnelStatus contains the reason of the error
   * and the @e TunnelID is set to 0xFFFF.
   */
  zb_uint16_t tunnel_id;

  /** @brief The @e TunnelStatus parameter indicates the server’s internal status
   * after the execution of a @ref ZB_ZCL_TUNNELING_CLI_CMD_REQUEST_TUNNEL "RequestTunnel" command.
   * @see @ref zb_zcl_tunneling_tunnel_status_e
   */
  zb_uint8_t tunnel_status;

  /** @brief A value that defines the size, in octets, of the maximum data packet
   * that can be transferred to the server in the payload of a single @e TransferData command.
   */
  zb_uint16_t max_incoming_transfer_size;
}
ZB_PACKED_STRUCT zb_zcl_tunneling_request_tunnel_response_t;


/** @brief Macro for sending @ref ZB_ZCL_TUNNELING_SRV_CMD_REQUEST_TUNNEL_RESPONSE "RequestTunnelResponse" command
 * @param _param - Reference to buffer.
 * @param _addr - Address of the device to send command to.
 * @param _dst_addr_mode - Address mode for _dst_addr.
 * @param _dst_ep - Destination endpoint.
 * @param _ep - Current endpoint.
 * @param _prfl_id - profile identifier
 * @param _def_resp - enable/disable default zcl response
 * @param _seq_num - ZCL transaction sequence number
 * @param _cb - Callback which should be called when the ZCL stack receives
 * APS ack.
 * @param _tunnel_id - @ref zb_zcl_tunneling_request_tunnel_response_t::tunnel_id value
 * @param _tunnel_status - @ref zb_zcl_tunneling_request_tunnel_response_t::tunnel_status value
 * @param _max_incoming_transfer_size - @ref zb_zcl_tunneling_request_tunnel_response_t::max_incoming_transfer_size value
 *
 */
#define ZB_ZCL_TUNNELING_SEND_REQUEST_TUNNEL_RESPONSE(                                            \
  _param, _addr, _dst_addr_mode, _dst_ep, _ep, _prfl_id, _def_resp, _seq_num, _cb,                \
  _tunnel_id, _tunnel_status, _max_incoming_transfer_size)                                        \
{                                                                                                 \
  zb_bufid_t _buffer = _param;                                                    \
  zb_uint8_t* __ptr = ZB_ZCL_START_PACKET(_buffer);                                               \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(__ptr,                                   \
                                                         ZB_ZCL_FRAME_DIRECTION_TO_CLI,           \
                                                         ZB_ZCL_NOT_MANUFACTURER_SPECIFIC);       \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(__ptr, _seq_num, ZB_ZCL_TUNNELING_SRV_CMD_REQUEST_TUNNEL_RESPONSE); \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(__ptr, (_tunnel_id));                                              \
  ZB_ZCL_PACKET_PUT_DATA8(__ptr, (_tunnel_status));                                               \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(__ptr, (_max_incoming_transfer_size));                             \
  ZB_ZCL_FINISH_PACKET(_buffer, __ptr)                                                            \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                      \
      _buffer, (_addr), (_dst_addr_mode), (_dst_ep), (_ep), (_prfl_id),                           \
      ZB_ZCL_CLUSTER_ID_TUNNELING, (_cb));                                                        \
}


/** @brief Macro for parsing payload of @ref ZB_ZCL_TUNNELING_SRV_CMD_REQUEST_TUNNEL_RESPONSE "RequestTunnelResponse" command
 * @param _data_ptr - pointer to @ref zb_zcl_tunneling_request_tunnel_response_t structure
 * @param _buffer - pointer to buffer where @e RequestTunnelResponse command's payload is stored.
 * @param _status - status of payload parsing
 */
#define ZB_ZCL_TUNNELING_GET_REQUEST_TUNNEL_RESPONSE(_data_ptr, _buffer, _status) \
{                                                                                 \
  zb_zcl_tunneling_request_tunnel_response_t *src_ptr =                           \
    (zb_zcl_tunneling_request_tunnel_response_t*)zb_buf_begin((_buffer));         \
                                                                                  \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_tunneling_request_tunnel_response_t)) \
  {                                                                               \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                      \
  }                                                                               \
  else                                                                            \
  {                                                                               \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                      \
    ZB_HTOLE16(&(_data_ptr)->tunnel_id, &(src_ptr->tunnel_id));                   \
    (_data_ptr)->tunnel_status = src_ptr->tunnel_status;                          \
    ZB_HTOLE16(&(_data_ptr)->max_incoming_transfer_size,                          \
               &(src_ptr->max_incoming_transfer_size));                           \
  }                                                                               \
}


/** @e AckTransferData command payload.
 * @see SE spec, subclause D.6.2.4.5.2.
 * @see Client @ref ZB_ZCL_TUNNELING_CLI_CMD_ACK_TRANSFER_DATA "AckTransferData" command
 * @see Server @ref ZB_ZCL_TUNNELING_SRV_CMD_ACK_TRANSFER_DATA "AckTransferData" command
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_ack_transfer_data_s
{

  /** A number between 0..65535 that uniquely identifies the tunnel that has been allocated
   * in the server triggered through the @e RequestTunnel command.
   */
  zb_uint16_t tunnel_id;             /* (M) */

  /** Indicates the number of bytes that may still be received by the initiator of this command.
   */
  zb_uint16_t number_of_bytes_left;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_tunneling_ack_transfer_data_t;


/** @e ReadyData command payload
 * @see SE spec, subclause D.6.2.4.6.2
 * @see Client @ref ZB_ZCL_TUNNELING_CLI_CMD_READY_DATA "ReadyData" command
 * @see Server @ref ZB_ZCL_TUNNELING_CLI_CMD_READY_DATA "ReadyData" command
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_ready_data_s
{

  /** A number between 0..65535 that uniquely identifies the tunnel that has
   * been allocated in the server triggered through the @e RequestTunnel command.
   */
  zb_uint16_t tunnel_id;              /* (M) */

  /** Indicates the number of octets that may be received by the initiator of this
   * command (receiver). It is most likely the remaining size of the buffer holding
   *  the data that is sent over @e TransferData.
   */
  zb_uint16_t number_of_octets_left;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_tunneling_ready_data_t;


/** @ref ZB_ZCL_TUNNELING_CLI_CMD_GET_SUPPORTED_TUNNEL_PROTOCOLS "GetSupportedTunnelProtocols" command payload
 * @see SE spec, subclause D.6.2.4.7.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_get_supported_tunnel_protocols_s
{
  /** Where there are more protocols supported than can be returned in a single
   * Supported @e TunnelProtocolsResponse command, this field allows an offset
   * to be specified on subsequent @e GetSupportedTunnelProtocols commands
   */
  zb_uint8_t protocol_offset;  /* (O) */
} ZB_PACKED_STRUCT zb_zcl_tunneling_get_supported_tunnel_protocols_t;


/** Supported Tunnel Protocols Response Command Protocol Fields
 * @see SE spec, Figure D-112
 * @see zb_zcl_tunneling_supported_tunnel_protocols_response_t::protocols
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_protocol_field_s
{

  /** A code that is allocated by the Zigbee Alliance, relating the manufacturer
   * to a device and - for tunneling - a manufacturer specific protocol.
   */
  zb_uint16_t manufacturer_code;

  /** An enumeration representing the identifier of the metering communication protocol
   * for the supported tunnel.
   * @see SE spec, Table D-124
   * @see zb_zcl_tunneling_protocol_id_t
   */
  zb_uint8_t protocol_id;
} ZB_PACKED_STRUCT zb_zcl_tunneling_protocol_field_t;


/** @ref ZB_ZCL_TUNNELING_SRV_CMD_SUPPORTED_TUNNEL_PROTOCOLS_RESPONSE "SupportedTunnelProtocolResponse"
 * command payload.
 * @see SE spec, subclause D.6.2.5.6.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_supported_tunnel_protocols_response_s
{
  /** The Protocol List Complete field is a Boolean; a value of 0(ZB_FALSE) indicates that
   * there are more supported protocols available (if more than 16 protocols are supported).
   *  A value of 1(ZB_TRUE) indicates that the list of supported protocols is complete.
   */
  zb_uint8_t protocol_list_complete;

  /** The number of Protocol fields contained in the response.
   */
  zb_uint8_t protocol_count;

  /** The payload of the response should be capable of holding up to 16 protocols.
   * @see zb_zcl_tunneling_protocol_field_s
   */
  zb_zcl_tunneling_protocol_field_t protocol_list[16];
} ZB_PACKED_STRUCT zb_zcl_tunneling_supported_tunnel_protocols_response_t;


/** @ref ZB_ZCL_TUNNELING_SRV_CMD_TUNNEL_CLOSURE_NOTIFICATION "TunnelClosureNotification"
 * command payload
 * @see SE spec, subclause D.6.2.5.7.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_tunnel_closure_notification_s
{
  /** The identifier of the tunnel that has been closed. It is the same number
   * that has been previously returned in the response to a @e RequestTunnel command.
   */
  zb_uint16_t tunnel_id;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_tunneling_tunnel_closure_notification_t;


/*********************** Tunnel API **************************/

/** @cond internal_doc  */
typedef ZB_PACKED_PRE struct zb_zcl_tunneling_cli_s
{
  zb_uint16_t tunnel_id;
  zb_uint16_t dst_addr;
  zb_uint8_t dst_ep;
  zb_uint16_t max_incoming_to_cli_transfer_size;
  zb_uint16_t max_outgoing_to_srv_transfer_size;
}
ZB_PACKED_STRUCT
zb_zcl_tunneling_cli_t;

extern zb_zcl_tunneling_cli_t tunneling_cli;

/** @endcond  */ /* internal_doc */

/*! @} */ /* ZB_ZCL_TUNNELING_COMMANDS */

/** @} */ /* addtogroup */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

/**************************** Tunnel internal **********************************/

void zb_zcl_tunneling_init_server(void);
void zb_zcl_tunneling_init_client(void);
#define ZB_ZCL_CLUSTER_ID_TUNNELING_SERVER_ROLE_INIT zb_zcl_tunneling_init_server
#define ZB_ZCL_CLUSTER_ID_TUNNELING_CLIENT_ROLE_INIT zb_zcl_tunneling_init_client

#endif /* ZB_ZCL_TUNNELING_H */
