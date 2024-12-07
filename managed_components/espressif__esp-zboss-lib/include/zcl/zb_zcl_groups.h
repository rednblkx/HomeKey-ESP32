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
/* PURPOSE: Groups cluster definitions
*/

#ifndef ZB_ZCL_GROUPS_H
#define ZB_ZCL_GROUPS_H 1

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_GROUPS
 *  @{
 *    @details
 *    Groups cluster defined in ZCL spec, clause 3.6. Currently no group name attribute support is
 *    implemented. However, this attribute must be supported for sending and receiving in commands'
 *    payload, so it is being sent as zero string (i. e. single zero byte). On receipt of the
 *    command is able to contain a scene name in the payload, this field will be ignored as a string.
 *
 *    Cluster requests and responses are documented in submodules.
 */

/** @name Groups cluster attributes
    @{
*/

/* ZB_ZCL_CLUSTER_ID_GROUPS = 0x0004 */

/** @brief Groups cluster attribute identifiers
    @see ZCL spec, subclause 3.6.2.2
*/
enum zb_zcl_groups_attr_e
{
  /** @brief NameSupport attribute */
  ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID = 0
};

/** @brief Values for NameSupport attribute */
enum zb_zcl_groups_name_support_e
{
  /** "Not supported" value */
  ZB_ZCL_ATTR_GROUPS_NAME_NOT_SUPPORTED = 0,
  /** "On" value */
  ZB_ZCL_ATTR_GROUPS_NAME_SUPPORTED  = 1 << 7 /* set to 1 the most significant bit */
};

/** @brief Default value for Groups cluster revision global attribute */
#define ZB_ZCL_GROUPS_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/**
 *  @brief Declare attribute list for Groups cluster.
 *  @param attr_list - attribute list name.
 *  @param name_support - pointer to variable to store name_support attribute value
 */
#define ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(attr_list, name_support)      \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_GROUPS) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID, (name_support)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* Groups cluster attributes */

/** @name Groups cluster command identifiers
    @{
*/

/**
 *  @brief Groups cluster command identifiers.
 *  @see ZCL spec, subclause 3.6.2.2.2.
 */
enum zb_zcl_groups_cmd_e
{
  ZB_ZCL_CMD_GROUPS_ADD_GROUP                = 0x00,  /**< Add group command identifier. */
  ZB_ZCL_CMD_GROUPS_VIEW_GROUP               = 0x01,  /**< View group command identifier. */
  ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP     = 0x02,  /**< Get group membership command identifier.
                                                        */
  ZB_ZCL_CMD_GROUPS_REMOVE_GROUP             = 0x03,  /**< Remove group command identifier. */
  ZB_ZCL_CMD_GROUPS_REMOVE_ALL_GROUPS        = 0x04,  /**< Remove all groups command identifier. */
  ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING = 0x05   /**< Add group if identifying command
                                                        identifier. */
};

/** @brief Groups cluster response command identifiers
    @see ZCL spec, subclause 3.6.2.3
*/
enum zb_zcl_groups_cmd_resp_e
{
  ZB_ZCL_CMD_GROUPS_ADD_GROUP_RES            = 0x00,  /**< Add group response command identifier. */
  ZB_ZCL_CMD_GROUPS_VIEW_GROUP_RES           = 0x01,  /**< View group response command identifier.
                                                        */
  ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP_RES = 0x02,  /**< Get group response membership command
                                                        identifier. */
  ZB_ZCL_CMD_GROUPS_REMOVE_GROUP_RES         = 0x03   /**< Remove group response command identifier.
                                                        */
};

/** @cond internals_doc */
/* GROUPS cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_GROUPS_SERVER_ROLE_GENERATED_CMD_LIST                   \
                                      ZB_ZCL_CMD_GROUPS_ADD_GROUP_RES,            \
                                      ZB_ZCL_CMD_GROUPS_VIEW_GROUP_RES,           \
                                      ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP_RES, \
                                      ZB_ZCL_CMD_GROUPS_REMOVE_GROUP_RES

#define ZB_ZCL_CLUSTER_ID_GROUPS_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_GROUPS_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_GROUPS_CLIENT_ROLE_GENERATED_CMD_LIST                   \
                                      ZB_ZCL_CMD_GROUPS_ADD_GROUP,                \
                                      ZB_ZCL_CMD_GROUPS_VIEW_GROUP,               \
                                      ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP,     \
                                      ZB_ZCL_CMD_GROUPS_REMOVE_GROUP,             \
                                      ZB_ZCL_CMD_GROUPS_REMOVE_ALL_GROUPS,        \
                                      ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING

#define ZB_ZCL_CLUSTER_ID_GROUPS_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_GROUPS_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/** @} */ /* Groups cluster command identifiers */

/************************** Add group command definitions ****************************/

/** @defgroup ZB_ZCL_GROUPS_ADD_GROUP Add group command definitions
 *  @{
 *    @details
 *    Add group request and response commands have simple payload. Response command is being filled
 *    and sent by ZCL internals.
 *
 *    @par Examples
 *
 *    Filling and sending Add Group request:
 *    @snippet HA_samples/door_lock/sample_zed.c send_add_group_req
 *
 *
 */

/** @brief Add group command structure */
typedef ZB_PACKED_PRE struct zb_zcl_groups_add_group_req_s
{
  zb_uint16_t group_id;      /**< Group id */
  zb_char_t   group_name[1]; /**< Group name, optional */
}
ZB_PACKED_STRUCT
zb_zcl_groups_add_group_req_t;

/** @cond internals_doc */

/** @internal Minimum size of Add group request (group name maybe omitted) */
#define ZB_ZCL_ADD_GROUP_REQ_SIZE sizeof(zb_uint16_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Add group command and fills in data request
    structure. If request contains invalid data, ZB_ZCL_NULL_ID is stored as group_id.
    @param data_buf - pointer to zb_buf_t buffer containing command request data
    @param add_group_req - variable to save command request
    @note data_buf buffer should contain command request payload without ZCL header.
 */
#define ZB_ZCL_GROUPS_GET_ADD_GROUP_REQ(data_buf, add_group_req)        \
{                                                                       \
  zb_zcl_groups_add_group_req_t *add_group_req_ptr;                     \
  (add_group_req_ptr) = zb_buf_len(data_buf) >= ZB_ZCL_ADD_GROUP_REQ_SIZE ? \
    (zb_zcl_groups_add_group_req_t*)zb_buf_begin(data_buf) : NULL;      \
                                                                        \
  if (add_group_req_ptr)                                                \
  {                                                                     \
    ZB_HTOLE16(&(add_group_req).group_id, &add_group_req_ptr->group_id);\
    /* group name currently is not supported - do not check it */       \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    add_group_req.group_id = ZB_ZCL_NULL_ID;                           \
  }                                                                     \
}

/** @brief Add group response command structure */
typedef ZB_PACKED_PRE struct zb_zcl_groups_add_group_res_s
{
  zb_uint8_t  status;        /**< Operation status */
  zb_uint16_t group_id;      /**< Group id */
}
ZB_PACKED_STRUCT
zb_zcl_groups_add_group_res_t;

/** @cond internals_doc */

/** @internal Add group response size */
#define ZB_ZCL_ADD_GROUP_RES_SIZE sizeof(zb_zcl_groups_add_group_res_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Add group response command and returns response data
    structure or NULL if request contains invalid data.
    @param data_buf - pointer to zb_buf_t buffer containing command response data
    @param add_group_res - out pointer to zb_zcl_groups_add_group_res_t, containing command
    response record
    @note data_buf buffer should contain response command payload without ZCL header
 */
#define ZB_ZCL_GROUPS_GET_ADD_GROUP_RES(data_buf, add_group_res)         \
{                                                                        \
  (add_group_res) = zb_buf_len(data_buf) >= ZB_ZCL_ADD_GROUP_RES_SIZE ?  \
    (zb_zcl_groups_add_group_res_t*)zb_buf_begin(data_buf) : NULL;       \
                                                                         \
  if (add_group_res)                                                     \
  {                                                                      \
    ZB_ZCL_HTOLE16_INPLACE(&(add_group_res)->group_id);                  \
    /* group name currently is not supported - do not check it */        \
  }                                                                      \
}

/** @brief Send Add group command
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param group_id - group ID to add
*/
#define ZB_ZCL_GROUPS_SEND_ADD_GROUP_REQ(                                                  \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, group_id)              \
{                                                                                          \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                           \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_GROUPS_ADD_GROUP); \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));                                           \
  /* Group name currently is not supported, put empty string */                            \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, ZB_ZCL_NULL_STRING);                                        \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                       \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                               \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_GROUPS, cb);     \
}

/** @} */ /* Add group command definitions */


/************************** View group command definitions ****************************/

/** @defgroup ZB_ZCL_GROUPS_VIEW_GROUP View group command definitions
 *  @{
 *    @details
 *    View Group command and response have simple payload. View group command is being processed,
 *    and View Group response command originates from ZCL internals.
 *
 *    @par Examples
 *    View Group request command can be formed as:
 *    @snippet HA_samples/door_lock/sample_zed.c view_group_req
 *
 */

/** @brief View group command structure */
typedef ZB_PACKED_PRE struct zb_zcl_groups_view_group_req_s
{
  zb_uint16_t group_id;      /**< Group id */
}
ZB_PACKED_STRUCT
zb_zcl_groups_view_group_req_t;

/** @cond internals_doc */

/** @internal Size of View group request */
#define ZB_ZCL_VIEW_GROUP_REQ_SIZE sizeof(zb_zcl_groups_view_group_req_t)

/** @endcond */ /* internals_doc */

/** @brief Parses View group command and fills in data request
    structure. If request contains invalid data, ZB_ZCL_NULL_ID is stored as group_id.
    @param data_buf - pointer to zb_buf_t buffer containing command request data
    @param view_group_req - variable to save command request
    @note data_buf buffer should contain command request payload without ZCL header.
 */
#define ZB_ZCL_GROUPS_GET_VIEW_GROUP_REQ(data_buf, view_group_req)            \
{                                                                             \
  zb_zcl_groups_view_group_req_t *view_group_req_ptr;                         \
  (view_group_req_ptr) = zb_buf_len(data_buf) >= ZB_ZCL_VIEW_GROUP_REQ_SIZE ? \
    (zb_zcl_groups_view_group_req_t*)zb_buf_begin(data_buf) : NULL;           \
                                                                              \
  if (view_group_req_ptr)                                                     \
  {                                                                           \
    ZB_HTOLE16(&(view_group_req).group_id, &view_group_req_ptr->group_id);    \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    view_group_req.group_id = ZB_ZCL_NULL_ID;                                 \
  }                                                                           \
}

/** @brief View group command response structure */
typedef ZB_PACKED_PRE struct zb_zcl_groups_view_group_res_s
{
  zb_uint8_t status;         /**< Operation status */
  zb_uint16_t group_id;      /**< Group id */
  zb_uint8_t group_name[1];  /**< Group name */
}
ZB_PACKED_STRUCT
zb_zcl_groups_view_group_res_t;

/** @cond internals_doc */

/** @internal View group response size */
#define ZB_ZCL_VIEW_GROUP_RES_SIZE sizeof(zb_zcl_groups_view_group_res_t)

/** @endcond */ /* internals_doc */

/** @brief Parses View group response command and returns response data
    structure or NULL if request contains invalid data.
    @param data_buf - pointer to zb_buf_t buffer containing command response data
    @param view_group_res - out pointer to zb_zcl_groups_add_group_res_t, containing command
    response record
    @note data_buf buffer should contain response command payload without ZCL header
 */
#define ZB_ZCL_GROUPS_GET_VIEW_GROUP_RES(data_buf, view_group_res)         \
{                                                                          \
  (view_group_res) = zb_buf_len(data_buf) >= ZB_ZCL_VIEW_GROUP_RES_SIZE ?  \
    (zb_zcl_groups_view_group_res_t*)zb_buf_begin(data_buf) : NULL;        \
                                                                           \
  if (view_group_res)                                                      \
  {                                                                        \
    ZB_ZCL_HTOLE16_INPLACE(&(view_group_res)->group_id);                   \
    /* group name currently is not supported - empty string is returned */ \
  }                                                                        \
}

/** @brief Send view group command
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param group_id - group ID to add
*/
#define ZB_ZCL_GROUPS_SEND_VIEW_GROUP_REQ(                                                  \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, group_id)               \
  {                                                                                         \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                       \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_GROUPS_VIEW_GROUP); \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));                                            \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                        \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_GROUPS, cb);      \
}

/** @} */ /* View group command definitions */

/************************** Get Group Membership command definitions ****************************/
/** @defgroup ZB_ZCL_GROUPS_GET_GRP_MEMB Get Group Membership command definitions
 *  @{
 *    @details
 *    Both Get Group Membership command request and response have a complex array-style payload.
 *
 *    @par Examples
 *    Get Group Membership request can be filled in as:
 *    @snippet HA_samples/door_lock/sample_zed.c send_get_group_membership_req
 *
 *
 */

/** @brief Get Group Membership command structure */
typedef ZB_PACKED_PRE struct zb_zcl_groups_get_group_membership_req_s
{
  zb_uint8_t group_count;    /**< Group  count */
  zb_uint16_t group_id[1];      /**< Group id list */
}
ZB_PACKED_STRUCT
zb_zcl_groups_get_group_membership_req_t;

/** @cond internals_doc */

/** @internal Minimum size of Get Group Membership request */
#define ZB_ZCL_GET_GROUP_MEMBERSHIP_REQ_SIZE sizeof(zb_uint8_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Get Group Membership command and returns pointer to data request
    structure. If request contains invalid data, NULL is returned
    @param data_buf - pointer to zb_buf_t buffer containing command request data
    @param group_member_req - pointer to command request
    @note data_buf buffer should contain command request payload without ZCL header.
 */
#define ZB_ZCL_GROUPS_GET_GROUP_MEMBERSHIP_REQ(data_buf, group_member_req)    \
{                                                                             \
  zb_uint8_t i_tmp;                                                           \
  zb_uint8_t cmd_size = ZB_ZCL_GET_GROUP_MEMBERSHIP_REQ_SIZE;                 \
                                                                              \
  (group_member_req) = (zb_buf_len(data_buf) >= cmd_size) ?                   \
    (zb_zcl_groups_get_group_membership_req_t*)zb_buf_begin(data_buf) : NULL; \
                                                                              \
  if (group_member_req)                                                       \
  {                                                                           \
    cmd_size += sizeof(zb_uint16_t) * (group_member_req)->group_count;        \
    if (cmd_size <= zb_buf_len(data_buf))                                     \
    {                                                                         \
      for(i_tmp = 0; i_tmp < (group_member_req)->group_count; i_tmp++)        \
      {                                                                       \
        ZB_ZCL_HTOLE16_INPLACE(&(group_member_req)->group_id[i_tmp]);         \
      }                                                                       \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      group_member_req = NULL;                                                \
    }                                                                         \
  }                                                                           \
}

/** @brief Get Group Membership command response structure */
typedef ZB_PACKED_PRE struct zb_zcl_groups_get_group_membership_res_s
{
  zb_uint8_t capacity;       /**< Capacity of group table */
  zb_uint8_t group_count;    /**< Group  count */
  zb_uint16_t group_id[1];   /**< Group id list */
}
ZB_PACKED_STRUCT
zb_zcl_groups_get_group_membership_res_t;

/** @cond internals_doc */

/** @internal Minimum size of Get Group Membership command response */
#define ZB_ZCL_GET_GROUP_MEMBERSHIP_RES_SIZE  (2*sizeof(zb_uint8_t))

/** @endcond */ /* internals_doc */

/** @brief Parses Get Group Membership command response and returns pointer to data response
    structure. If response contains invalid data, NULL is returned.
    @param data_buf - pointer to zb_buf_t buffer containing command response data
    @param group_member_res - pointer to command response
    @note data_buf buffer should contain command response payload without ZCL header.
 */
#define ZB_ZCL_GROUPS_GET_GROUP_MEMBERSHIP_RES(data_buf, group_member_res)    \
{                                                                             \
  zb_uint8_t i;                                                               \
  zb_uint8_t cmd_size = ZB_ZCL_GET_GROUP_MEMBERSHIP_RES_SIZE;                 \
                                                                              \
  (group_member_res) = (zb_buf_len(data_buf) >= cmd_size) ?                   \
    (zb_zcl_groups_get_group_membership_res_t*)zb_buf_begin(data_buf) : NULL; \
                                                                              \
  if (group_member_res)                                                       \
  {                                                                           \
    cmd_size += sizeof(zb_uint16_t) * (group_member_res)->group_count;        \
    if (cmd_size <= zb_buf_len(data_buf))                                     \
    {                                                                         \
      for(i = 0; i < (group_member_res)->group_count; i++)                    \
      {                                                                       \
        ZB_ZCL_HTOLE16_INPLACE(&(group_member_res)->group_id[i]);             \
      }                                                                       \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      group_member_res = NULL;                                                \
    }                                                                         \
  }                                                                           \
}

/** @brief Initialize Get group membership command
    @param buffer to put packet to
    @param ptr - command buffer pointer
    @param def_resp - enable/disable default response
    @param group_cnt - group count
*/
#define ZB_ZCL_GROUPS_INIT_GET_GROUP_MEMBERSHIP_REQ(buffer, ptr, def_resp, group_cnt) \
{                                                                                     \
  ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                                  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                 \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                                    \
      ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP);             \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (group_cnt));                                          \
}

/** @brief Add group id to command payload
    @param ptr - command buffer pointer
    @param group_id - group ID
*/
#define ZB_ZCL_GROUPS_ADD_ID_GET_GROUP_MEMBERSHIP_REQ(ptr, group_id)  \
{                                                                     \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));                      \
}

/** @brief Sends Get group membership command
    @param buffer to place data to
    @param ptr - pointer to the memory area after the command data end
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_GROUPS_SEND_GET_GROUP_MEMBERSHIP_REQ(                                   \
    buffer, ptr, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb)                         \
{                                                                                      \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_GROUPS, cb); \
}

/** @} */ /* Get Group Membership command definitions */


/************************** Remove Group command definitions ****************************/
/** @defgroup ZB_ZCL_GROUPS_RM_GRP Remove Group command definitions
 *  @{
 *    @details
 *    Remove Group request and response commands have simple payload. Actual request handling takes
 *    place in ZCL internals.
 *
 *    @par Examples
 *    Remove Group request sending:
 *    @snippet HA_samples/door_lock/sample_zed.c send_remove_group_req
 *
 *
 */

/** @brief Remove Group command structure */
typedef ZB_PACKED_PRE struct zb_zcl_groups_remove_group_req_s
{
  zb_uint16_t group_id;      /**< Group id */
}
ZB_PACKED_STRUCT
zb_zcl_groups_remove_group_req_t;

/** @cond internals_doc */

/** @internal Minimum size of Get Group Membership request */
#define ZB_ZCL_REMOVE_GROUP_REQ_SIZE sizeof(zb_zcl_groups_remove_group_req_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Remove group command and fills in data request
    structure. If request contains invalid data, ZB_ZCL_NULL_ID is stored as group_id.
    @param data_buf - pointer to zb_buf_t buffer containing command request data
    @param rem_group_req - variable to save command request
    @note data_buf buffer should contain command request payload without ZCL header.
 */
#define ZB_ZCL_GROUPS_GET_REMOVE_GROUP_REQ(data_buf, rem_group_req)            \
{                                                                              \
  zb_zcl_groups_remove_group_req_t *rem_group_req_ptr;                         \
  (rem_group_req_ptr) = zb_buf_len(data_buf) >= ZB_ZCL_REMOVE_GROUP_REQ_SIZE ? \
    (zb_zcl_groups_remove_group_req_t*)zb_buf_begin(data_buf) : NULL;          \
                                                                               \
  if (rem_group_req_ptr)                                                       \
  {                                                                            \
    ZB_HTOLE16(&(rem_group_req).group_id, &rem_group_req_ptr->group_id);       \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    rem_group_req.group_id = ZB_ZCL_NULL_ID;                                   \
  }                                                                            \
}

/** @brief Remove group response command structure */
typedef ZB_PACKED_PRE struct zb_zcl_groups_remove_group_res_s
{
  zb_uint8_t  status;        /**< Operation status */
  zb_uint16_t group_id;      /**< Group id */
}
ZB_PACKED_STRUCT
zb_zcl_groups_remove_group_res_t;

/** @cond internals_doc */

/** @internal Add group response size */
#define ZB_ZCL_REMOVE_GROUP_RES_SIZE sizeof(zb_zcl_groups_remove_group_res_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Remove group response command and returns response data
    structure or NULL if request contains invalid data.
    @param data_buf - pointer to zb_buf_t buffer containing command response data
    @param rem_group_res - out pointer to zb_zcl_groups_remove_group_res_t, containing command
    response record
    @note data_buf buffer should contain response command payload without ZCL header
 */
#define ZB_ZCL_GROUPS_GET_REMOVE_GROUP_RES(data_buf, rem_group_res)         \
{                                                                           \
  (rem_group_res) = zb_buf_len(data_buf) >= ZB_ZCL_REMOVE_GROUP_RES_SIZE ?  \
    (zb_zcl_groups_remove_group_res_t*)zb_buf_begin(data_buf) : NULL;       \
                                                                            \
  if (rem_group_res)                                                        \
  {                                                                         \
    ZB_ZCL_HTOLE16_INPLACE(&(rem_group_res)->group_id);                     \
  }                                                                         \
}

/** @brief Send Remove group command
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param group_id - group ID to remove
*/
#define ZB_ZCL_GROUPS_SEND_REMOVE_GROUP_REQ(                                                  \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, group_id)                 \
{                                                                                             \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                              \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                         \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_GROUPS_REMOVE_GROUP); \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));                                              \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                          \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                  \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_GROUPS, cb);        \
}

/** @} */ /* Remove Group command definitions */


/************************** Remove All Groups command definitions ****************************/
/** @defgroup ZB_ZCL_GROUPS_RM_ALL_GRPS Remove All Groups command definitions
 *  @{
 *    @details
 *    Remove All Groups command has only request form, and has no payload. Command is being
 *    processed in ZCL internals. If not disabled, command can be responded with Default Response
 *    command.
 *
 *    @par Example
 *    Remove All Groups command sending:
 *    @snippet HA_samples/door_lock/sample_zed.c send_remove_all_groups_req
 *    @par
 *
 */

/** @brief Send Remove all groups command
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_GROUPS_SEND_REMOVE_ALL_GROUPS_REQ(                                                  \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                                \
{                                                                                                  \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                                   \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                              \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_GROUPS_REMOVE_ALL_GROUPS); \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                               \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                       \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_GROUPS, cb);             \
}

/** @} */ /* Remove All Groups command definitions */

/************************** Add group if identifying command definitions **************************/

/** @defgroup ZB_ZCL_GROUPS_ADD_IDENT Add group if identifying command definitions
 *  @{
 *    @details
 *    Add Group if Identifying command has simple payload. The command is being processed in ZCL
 *    internals. Since the command has only request form, it can be responded with Default Response
 *    command frame, if not disabled explicitly.
 *
 *    @b Example:
 *    Add Group if Identifying send:
 *    @snippet HA_samples/door_lock/sample_zed.c add_group_if_ident_req
 *
 */

/** @brief Send Add group if identifying command
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param group_id - group ID to add
*/
#define ZB_ZCL_GROUPS_SEND_ADD_GROUP_IF_IDENT_REQ(                                     \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, group_id)          \
  {                                                                                    \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                       \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                                     \
      ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING);          \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));                                       \
  /* Group name currently is not supported, put empty string */                        \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, ZB_ZCL_NULL_STRING);                                    \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_GROUPS, cb); \
}

/** @} */ /* Add group if identifying command definitions */

/**
 * Handler for incoming group commands directed to server
 *
 * @param param - reference to buffer containing command
 *
 * @return ZB_TRUE - if command was processed
 *         ZB_FALSE - if command is not for server or is not supported
 */
zb_bool_t zb_zcl_process_groups_commands_srv(zb_uint8_t param);

/**
 * Handler for incoming group commands directed to client
 *
 * @param param - reference to buffer containing command
 *
 * @return ZB_TRUE - if command was processed
 *         ZB_FALSE - if command is not for client or is not supported
 */
zb_bool_t zb_zcl_process_groups_commands_cli(zb_uint8_t param);

/** @cond internals_doc
    @internal @name Groups cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID(data_ptr)  \
{                                                                 \
  ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID,                             \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                             \
  (void*) data_ptr                                                \
}

/** @internal @brief Number of attributes mandatory for reporting in Groups cluster. */
#define ZB_ZCL_GROUPS_REPORT_ATTR_COUNT 0

#if defined ZB_ZCL_SUPPORT_CLUSTER_SCENES
/** Scenes fieldset length for Groups cluster */
#define ZB_ZCL_CLUSTER_ID_GROUPS_SCENE_FIELD_SETS_LENGTH 0
#endif /* defined ZB_ZCL_SUPPORT_CLUSTER_SCENES */

/** @}
    @endcond */ /* Groups cluster internals */

/** @} */ /* ZCL Groups cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_groups_init_server(void);
void zb_zcl_groups_init_client(void);
#define ZB_ZCL_CLUSTER_ID_GROUPS_SERVER_ROLE_INIT zb_zcl_groups_init_server
#define ZB_ZCL_CLUSTER_ID_GROUPS_CLIENT_ROLE_INIT zb_zcl_groups_init_client

#endif /* ZB_ZCL_GROUPS_H */
