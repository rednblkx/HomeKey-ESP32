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
/* PURPOSE: EN50523 Appliance events and alerts cluster definitions
*/

#ifndef ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_H
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_H 1

/** @cond DOXYGEN_ZCL_SECTION */

/*! @addtogroup ZB_ZCL_EN50523_APP_EVENTS_ALERTS
    @{
*/
/*! @name EN50523 Appliance events and alerts cluster commands
    @{
*/
/*! @brief EN50523 Appliance events and alerts cluster
    command received identifiers
    @see ZCL 6.0 spec, subclause 15.4.2.3
*/
enum zb_zcl_en50523_appl_ev_and_alerts_recv_cmd_e
{
  ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_ID = 0x00, /**< "Get alerts" command. */
};

/*! @brief EN50523 Appliance events and alerts cluster
    commands generated identifiers
    @see ZCL 6.0 spec, subclause 15.4.2.4
*/
enum zb_zcl_en50523_appl_ev_and_alerts_send_cmd_e
{
  /*! "Get alerts response" command. */
  ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_RESP_ID     = 0x00,
  /*! "Alerts notification" command. */
  ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_ALERTS_NOTIFICATION_ID = 0x01,
  /*! "Event notification" command. */
  ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_EVENT_NOTIFICATION_ID  = 0x02,
};

/** @brief Default value for EN50523 Appliance events and alerts cluster revision global attribute */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/*!
  @brief Declare attribute list for EN50523 Appliance events and alerts cluster (only cluster revision attribute)
  @param attr_list - attribute list name
*/
#define ZB_ZCL_DECLARE_EN50523_APPL_EV_AND_ALERTS_ATTR_LIST(attr_list)                            \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_EN50523_APPL_EV_AND_ALERTS) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @cond internals_doc */
/* en50523 cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_APPL_EV_AND_ALERTS_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_ID

#define ZB_ZCL_CLUSTER_ID_APPL_EV_AND_ALERTS_SERVER_ROLE_GENERATED_CMD_LIST             \
                          ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_RESP_ID,     \
                          ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_ALERTS_NOTIFICATION_ID, \
                          ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_EVENT_NOTIFICATION_ID

#define ZB_ZCL_CLUSTER_ID_APPL_EV_AND_ALERTS_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_APPL_EV_AND_ALERTS_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_APPL_EV_AND_ALERTS_CLIENT_ROLE_GENERATED_CMD_LIST ZB_ZCL_CLUSTER_ID_APPL_EV_AND_ALERTS_SERVER_ROLE_RECEIVED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/******************************* Get Alerts Command ******************************/

/*! @brief Get Alerts command, ZCL spec 15.4.2.2.1
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_GET_ALERTS_REQ(         \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)      \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_ID);            \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS, cb);              \
}

/******************************* Get Alerts Response Command ******************************/

/*! @brief Maximum number of Alerts */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_MAX_ALERTS_NUM  15

/*! @brief Type of alert */
enum zb_zcl_en50523_appl_ev_and_alerts_alerts_type_e
{
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_TYPE_UNSTRUCTURED = 0x00,
};

/*! @brief Alert category */
enum zb_zcl_en50523_appl_ev_and_alerts_alert_category_e
{
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_CATEGORY_WARNING = 0x01,
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_CATEGORY_DANGER  = 0x02,
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_CATEGORY_FAILURE = 0x03,
};

/*! @brief Alert presence recovery */
enum zb_zcl_en50523_appl_ev_and_alerts_alert_presence_e
{
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_RECOVERY = 0x00,
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_PRESENCE = 0x01,
};

/*! @brief Get Number of Alerts */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_COUNT_GET_NUM(_alerts_count)   ((_alerts_count) & 0x0F)

/*! @brief Set Number of Alerts */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_COUNT_SET_NUM(_alerts_count, num)   ((_alerts_count) |= (num & 0x0F))

/*! @brief Get Type of alert */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_COUNT_GET_TYPE(_alerts_count) (((_alerts_count) & 0xF0) >> 4)

/*! @brief Set Type of alert */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_COUNT_SET_TYPE(_alerts_count, type) ((_alerts_count) |= ((type & 0x0F) << 4))

/*! @brief Get Alert ID */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_STRUCT_GET_ID(_alert_struct)   ((_alert_struct).alert[2] & 0xFF)

/*! @brief Set Alert ID */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_STRUCT_SET_ID(_alert_struct, id)   ((_alert_struct).alert[2] = (id & 0xFF))

/*! @brief Get Alert Category */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_STRUCT_GET_CATEGORY(_alert_struct) ((_alert_struct).alert[1] & 0x0F)

/*! @brief Set Alert Category */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_STRUCT_SET_CATEGORY(_alert_struct, cat) ((_alert_struct).alert[1] |=  (cat & 0x0F))

/*! @brief Get Alert Presence recovery */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_STRUCT_GET_PRESENCE(_alert_struct) (((_alert_struct).alert[1] & 0xF0) >> 4)

/*! @brief Set Alert Presence recovery */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_STRUCT_SET_PRESENCE(_alert_struct, pres) ((_alert_struct).alert[1] |= (pres & 0x0F) << 4)

/*! @brief Get Alert Manufacturer specific bits */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_STRUCT_GET_MANUF_SPEC(_alert_struct) ((_alert_struct).alert[0] & 0xFF)

/*! @brief Set Alert Manufacturer specific bits */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERT_STRUCT_SET_MANUF_SPEC(_alert_struct, val) ((_alert_struct).alert[0] = (val & 0xFF))

/*! @brief Alert structure */
typedef ZB_PACKED_PRE struct zb_zcl_en50523_appl_ev_and_alerts_alert_struct_s
{
  /** Alert field - Alert ID, Category, Presence recovery, Manufacturer specific bits */
  zb_uint8_t alert[3];
} ZB_PACKED_STRUCT zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t;

/*! @brief Structure representation of Get Alerts Response command */
typedef ZB_PACKED_PRE struct zb_zcl_en50523_appl_ev_and_alerts_get_alerts_resp_s
{
  /** Alerts count fields - Number of Alerts, Type of alert */
  zb_uint8_t alerts_count;
  /** Alert structure fields, see zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t */
  zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t alert_struct[ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_MAX_ALERTS_NUM];

} ZB_PACKED_STRUCT zb_zcl_en50523_appl_ev_and_alerts_get_alerts_resp_t;

/*! Get Alerts Response command payload size */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_RESP_PAYLOAD_SIZE(ptr) (sizeof(((zb_zcl_en50523_appl_ev_and_alerts_get_alerts_resp_t *)(ptr))->alerts_count) + \
                                                                             (ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_COUNT_GET_NUM(((zb_zcl_en50523_appl_ev_and_alerts_get_alerts_resp_t *)(ptr))->alerts_count)* \
                                                                              sizeof(zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t)))

/*! @brief Start Get Alerts Response command, ZCL spec 15.4.2.3.1
    @param buffer - to put packet to
    @param seq - sequence
    @param alerts_count - Alerts count field
    @param ptr - [out] (zb_uint8_t*) current position for @ref ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_GET_ALERTS_RESP_ADD
    and @ref ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_GET_ALERTS_RESP_END
*/
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_GET_ALERTS_RESP_START( \
    buffer, seq, alerts_count, ptr)                                   \
{                                                                     \
  (ptr) = ZB_ZCL_START_PACKET(buffer);                                \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq,                           \
    ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_RESP_ID);        \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (alerts_count));                     \
}

/*! @brief Add Alert Structure to Get Alerts Response command, ZCL spec 15.4.2.3.1
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param alert_struct - pointer to the Alert structure to put into packet
*/
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_GET_ALERTS_RESP_ADD(ptr, alert_struct) \
{                                                                                     \
  ZB_ZCL_PACKET_PUT_DATA24(ptr, (alert_struct));                                      \
}

/*! @brief End form Get Alerts Response command and send it, ZCL spec 15.4.2.3.1
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
*/
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_GET_ALERTS_RESP_END( \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id)            \
{                                                                   \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                               \
  ZB_ZCL_SEND_COMMAND_SHORT(                                        \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,             \
      ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS, NULL);         \
}

/** @brief Macro for getting Get Alerts Response command, ZCL spec 15.4.2.3.1
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_en50523_appl_ev_and_alerts_get_alerts_resp_t
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_GET_GET_ALERTS_RESP(data_ptr, buffer, status) \
{                                                                    \
  zb_uint8_t *data = zb_buf_begin(buffer);                           \
  if (zb_buf_len((buffer)) <                                         \
      ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_RESP_PAYLOAD_SIZE(data)) \
  {                                                                  \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                          \
  }                                                                  \
  else                                                               \
  {                                                                  \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->alerts_count, data);        \
    ZB_ZCL_PACKET_GET_DATA_N((data_ptr)->alert_struct, data,         \
      ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_COUNT_GET_NUM((data_ptr)->alerts_count)*sizeof(zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t)); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                          \
  }                                                                  \
}

/******************************* Alerts Notification Command ******************************/

/*! @brief Structure representation of Alerts Notification command */
typedef ZB_PACKED_PRE struct zb_zcl_en50523_appl_ev_and_alerts_alerts_notif_s
{
  /** Alerts count fields - Number of Alerts, Type of alert */
  zb_uint8_t alerts_count;
  /** Alert structure fields, see zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t */
  zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t alert_struct[ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_MAX_ALERTS_NUM];

} ZB_PACKED_STRUCT zb_zcl_en50523_appl_ev_and_alerts_alerts_notif_t;

/*! Alerts Notification command payload size */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_NOTIF_PAYLOAD_SIZE(ptr) (sizeof(((zb_zcl_en50523_appl_ev_and_alerts_alerts_notif_t *)(ptr))->alerts_count) + \
                                                                          (ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_COUNT_GET_NUM(((zb_zcl_en50523_appl_ev_and_alerts_alerts_notif_t *)(ptr))->alerts_count)* \
                                                                           sizeof(zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t)))

/*! @brief Start Alerts Notification command, ZCL spec 15.4.2.3.2
    @param buffer - to put packet to
    @param def_resp - enable/disable default response
    @param alerts_count - Alerts count field
    @param ptr - [out] (zb_uint8_t*) current position for @ref ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_ALERTS_NOTIF_ADD
    and @ref ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_ALERTS_NOTIF_END
*/
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_ALERTS_NOTIF_START(  \
    buffer, def_resp, alerts_count, ptr)                            \
{                                                                   \
  (ptr) = zb_zcl_start_command_header(buffer,                       \
  ZB_ZCL_CONSTRUCT_FRAME_CONTROL(                                   \
      ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                           \
      ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                             \
      ZB_ZCL_FRAME_DIRECTION_TO_CLI,                                \
      (def_resp)),                                                  \
      0, /* No manuf_code */                                        \
      ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_ALERTS_NOTIFICATION_ID, \
      NULL);                                                        \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (alerts_count));                   \
}

/*! @brief Add Alert Structure to Alerts Notification command, ZCL spec 15.4.2.3.2
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param alert_struct - pointer to Alert structure to put into packet
*/
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_ALERTS_NOTIF_ADD(ptr, alert_struct) \
{                                                                                  \
  ZB_ZCL_PACKET_PUT_DATA24(ptr, (alert_struct));                                   \
}

/*! @brief End form Alerts Notification command and send it, ZCL spec 15.4.2.3.2
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_ALERTS_NOTIF_END(    \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb)        \
{                                                                   \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                               \
  ZB_ZCL_SEND_COMMAND_SHORT(                                        \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,             \
      ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS, cb);           \
}

/** @brief Macro for getting Alerts Notification command, ZCL spec 15.4.2.3.2
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_en50523_appl_ev_and_alerts_alerts_notif_t
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_NOTIF(data_ptr, buffer, status) \
{                                                                    \
  zb_uint8_t *data = zb_buf_begin(buffer);                           \
  if (zb_buf_len((buffer)) <                                         \
      ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_NOTIF_PAYLOAD_SIZE(data)) \
  {                                                                  \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                          \
  }                                                                  \
  else                                                               \
  {                                                                  \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->alerts_count, data);        \
    ZB_ZCL_PACKET_GET_DATA_N((data_ptr)->alert_struct, data,         \
      ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_COUNT_GET_NUM((data_ptr)->alerts_count)*sizeof(zb_zcl_en50523_appl_ev_and_alerts_alert_struct_t)); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                          \
  }                                                                  \
}

/******************************* Event Notification Command ******************************/

/*! @brief Event Identification */
enum zb_zcl_en50523_appl_ev_and_alerts_event_identification_e
{
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_EVENT_ID_END_OF_CYCLE    = 0x01,
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_EVENT_ID_TEMP_REACHED    = 0x04,
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_EVENT_ID_END_OF_COOKING  = 0x05,
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_EVENT_ID_SWITCHING_OFF   = 0x06,
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_EVENT_ID_WRONG_DATA      = 0xf7,
};

/*! @brief Structure representation of Event Notification command
  @param event_header - Event header field
  @param event_identification - Event Identification field, see zb_zcl_en50523_appl_ev_and_alerts_event_identification_e
*/
typedef ZB_PACKED_PRE struct zb_zcl_en50523_appl_ev_and_alerts_event_notif_s
{
  zb_uint8_t event_header;
  zb_uint8_t event_identification;

} ZB_PACKED_STRUCT zb_zcl_en50523_appl_ev_and_alerts_event_notif_t;

/*! @brief Event Notification command, ZCL spec 15.4.2.3.3
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param event_identification - Event ID
*/
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_SEND_EVENT_NOTIF(        \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,  \
  event_identification)                                            \
{                                                                  \
  zb_uint8_t* ptr = zb_zcl_start_command_header(buffer,            \
  ZB_ZCL_CONSTRUCT_FRAME_CONTROL(                                  \
      ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                          \
      ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                            \
      ZB_ZCL_FRAME_DIRECTION_TO_CLI,                               \
      (def_resp)),                                                 \
      0, /* No manuf_code */                                       \
      ZB_ZCL_CMD_EN50523_APPL_EV_AND_ALERTS_EVENT_NOTIFICATION_ID, \
      NULL);                                                       \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, 0);                                 \
  ZB_ZCL1_PACKET_PUT_DATA8(ptr, (event_identification));            \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                              \
  ZB_ZCL_SEND_COMMAND_SHORT(                                       \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,            \
      ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS, cb);          \
}

/** @brief Macro for getting Event Notification command, see ZCL spec 15.4.2.3.3
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type zb_zcl_en50523_appl_ev_and_alerts_event_notif_e
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_GET_EVENT_NOTIF(data_ptr, buffer, status) \
{                                                              \
  zb_uint8_t *data = zb_buf_begin(buffer);                     \
  if (zb_buf_len((buffer)) <                                   \
      sizeof(zb_zcl_en50523_appl_ev_and_alerts_event_notif_t)) \
  {                                                            \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                    \
  }                                                            \
  else                                                         \
  {                                                            \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->event_header, data);  \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->event_identification, data); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                    \
  }                                                            \
}
/*! @} */ /* Appliance events and alerts cluster commands */
/*! @} */ /*ZB_ZCL_EN50523_APP_EVENTS_ALERTS */
/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_en50523_appliance_events_and_alerts_init_server(void);
void zb_zcl_en50523_appliance_events_and_alerts_init_client(void);
#define ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS_SERVER_ROLE_INIT zb_zcl_en50523_appliance_events_and_alerts_init_server
#define ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS_CLIENT_ROLE_INIT zb_zcl_en50523_appliance_events_and_alerts_init_client

#endif  /* ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_H */
