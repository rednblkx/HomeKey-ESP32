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
/* PURPOSE: Scenes cluster definitions
*/

#ifndef ZB_ZCL_SCENES_H
#define ZB_ZCL_SCENES_H 1

/** @cond DOXYGEN_ZCL_SECTION */

/*! @defgroup ZB_ZCL_SCENES ZCL Scenes cluster
    @ingroup ZB_ZCL_CLUSTERS
    @addtogroup ZB_ZCL_SCENES
    ZCL Scenes cluster definitions
    @{
*/

/* ZB_ZCL_CLUSTER_ID_SCENES = 0x0005 */

/*! @name Scenes cluster attributes
    @{
*/

/*! @brief Scene cluster attribute identifiers
    @see ZCL specification, subclause  3.7.2.2
 */
enum zb_zcl_scene_attr_e
{
  /*! Number of scenes currently in the device's scene table */
  ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID   = 0x0000,
  /*! Scene ID of the scene last invoked */
  ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID = 0x001,
  /*! @brief Group ID of the scene last invoked

      Can hold 0x0000 if the scene last invoked is not associated with a group
  */
  ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID = 0x002,
  /*! @brief Indicates whether the state of the device corresponds to
      that associated with the CurrentScene and CurrentGroup attributes
      @li TRUE indicates that these attributes are valid
      @li FALSE indicates that they are not valid
      
      SceneValid attribute has to be maintained by the application according to ZCL spec snippet below:
      
      > Before a scene has been stored or recalled, this attribute is set to FALSE. After a successful Store Scene or
      > Recall Scene command it is set to TRUE. If, after a scene is stored or recalled, the state of the device is
      > modified, this attribute is set to FALSE.
      
  */
  ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID   = 0x003,
  /* TODO Remove default value for Name support attribute when scene name
     support will be provided
  */
  /*! @brief Indicates support for scene names

      The most significant bit of the NameSupport attribute indicates whether or
      not scene names are supported
      @li 1 indicates that they are supported
      @li 0 indicates that they are not supported
      @note Currently scene names are not supported
  */
  ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID  = 0x004,
  /*! @brief specifies the IEEE address of the device that last configured the
      scene table

      The value 0xffffffffffffffff indicates that the device has not been
      configured, or that the address of the device that last configured the
      scenes cluster is not known
      @attention The attribute is not supported!
  */
  ZB_ZCL_ATTR_SCENES_LAST_CONFIGURED_BY_ID = 0x005
};

/** @brief Default value for Scenes cluster revision global attribute */
#define ZB_ZCL_SCENES_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/** @brief Maximal value for implemented Scenes cluster revision global attribute */
#define ZB_ZCL_SCENES_CLUSTER_REVISION_MAX ZB_ZCL_SCENES_CLUSTER_REVISION_DEFAULT

/** @brief Mask to extract Name support bit */
#define ZB_ZCL_SCENES_NAME_SUPPORT_BIT_MASK 0x80

/*! Minimal permissible value for Current group attribute */
#define ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_MIN_VALUE 0x0001
/*! Maximal permissible value for Current group attribute */
#define ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_MAX_VALUE 0xfff7
/*! Maximal number of scene table records */
#define ZB_ZCL_MAX_SCENE_TABLE_RECORDS 16

/** @brief Default value for Scene count attribute */
#define ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE 0

/** @brief Default value for Current scene attribute */
#define ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE 0

/** @brief Default value for Current group attribute */
#define ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE 0

/** @brief Default value for Scene valid attribute */
#define ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE 0

/** @brief "Not used" value for Recall scene transition time */
#define ZB_ZCL_RECALL_SCENES_TRANSITION_TIME_NOT_USED_VALUE 0xffff

/*
 * TODO: Remove default value for Scenes.NameSupport attribute when scene names
 * will be supported.
 */
/** @brief Default value for Name support attribute
 *  The Stack does not support scene names, the default value for the attribute can be declared until name support will be added
*/
#define ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE 0

/** @brief Global Scene - group identifier
    see ZLL spec 6.5.1.3.1*/
#define ZB_ZCL_SCENE_GLOBAL_SCENE_GROUP_ID 0x00

/** @brief Global Scene - scene identifier
    see ZLL spec 6.5.1.3.1*/
#define ZB_ZCL_SCENE_GLOBAL_SCENE_ID 0x00


/*! @brief Fixed-size part of the Scene table */
typedef ZB_PACKED_PRE struct zb_zcl_scene_table_record_fixed_s
{
  /*! @brief Group identifier for the scene

      Can be zero if the scene is not associated with any group @attention Since
      value of 0xffff is not appropriate as a group identifier, it is being used
      as a free record marker in a form of @ref ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD
      macro
  */
  zb_uint16_t group_id;
  /*! @brief Scene transition time */
  zb_uint16_t transition_time;
  /*! @brief Scene identifier */
  zb_uint8_t scene_id;
  /*! @brief Field set length

      Actual length of the field sets. While the maximal length of the field
      sets is known, its actual length should be figured out in order to finish reading at
      a proper point. E. g.: as stated in ZCL spec, subclause 3.7.2.4.1.1, it is
      not mandatory for a field set to be included in the command for every
      cluster on the endpoint that has a defined field set. This field can provide
      and actual length of the field sets thus being able to read
      only the info required.
  */
  zb_uint8_t field_sets_length;
  zb_uint8_t transition_time_100ms;
} ZB_PACKED_STRUCT zb_zcl_scene_table_record_fixed_t;

/*! @brief Declares a "free record" marker for a scene table
   Used as a value of a group_id field in a corresponding table record
*/
#define ZB_ZCL_SCENES_FREE_SCENE_TABLE_RECORD 0xffff

/** @brief Default value for scene transition time */
#define ZB_ZCL_SCENES_TRANSITION_TIME_DEFAULT_VALUE 0

/*! @brief Common part of the field set definition */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_fieldset_common_s
{
  zb_uint16_t cluster_id;     /*!< Cluster identifier */
  zb_uint8_t fieldset_length; /*!< Length of the field set's variable part */
} ZB_PACKED_STRUCT zb_zcl_scenes_fieldset_common_t;

/*! @brief Declares Scenes attribute list
    @param attr_list Attribute list name
    @param scene_count @ref ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID placeholder
    @param current_scene @ref ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID placeholder
    @param current_group @ref ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID placeholder
    @param scene_valid @ref ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID placeholder
    @param name_support @ref ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID placeholder
*/
#define ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(                                    \
    attr_list,                                                                \
    scene_count,                                                              \
    current_scene,                                                            \
    current_group,                                                            \
    scene_valid,                                                              \
    name_support)                                                             \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_SCENES) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID, (scene_count))      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID, (current_scene))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID, (current_group))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID, (scene_valid))      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID, (name_support))    \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @cond internals_doc */
/** @brief Removes All scenes corresponding to a group identifiers in a whole device.
    @param param - buffer to get data from.
    @param group_id - group identifier for which all scenes must be removed.
*/
void zb_zcl_scenes_remove_all_scenes_in_all_endpoints_by_group_id(zb_uint8_t param, zb_uint16_t group_id);
void zb_zcl_scenes_remove_all_scenes_in_all_endpoints(zb_uint8_t param);
/*! @}
 *  @endcond */ /* internals_doc */

/*! @} */ /* Scenes cluster attributes */

/** @cond internals_doc */
/*! @name Scenes cluster internals
    Internal structures for Scenes cluster
    @internal
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID(data_ptr) \
{                                                                          \
  ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                            \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                      \
  (void*) data_ptr                                                         \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID(data_ptr) \
{                                                                            \
  ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                        \
  (void*) data_ptr                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID(data_ptr) \
{                                                                            \
  ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                        \
  (void*) data_ptr                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID(data_ptr) \
{                                                                          \
  ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID,                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                            \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                      \
  (void*) data_ptr                                                         \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID(data_ptr) \
{                                                                           \
  ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                       \
  (void*) data_ptr                                                          \
}

/*! @} */ /* Scenes cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/*! @name Scenes cluster commands
    @{
*/

/*! @brief Command identifiers for Scenes Cluster
    @see ZCL spec, subclause 3.7.2.4
*/
enum zb_zcl_scenes_cmd_e
{
  /** Add scene command */
  ZB_ZCL_CMD_SCENES_ADD_SCENE             = 0x00,
  /** View scene command */
  ZB_ZCL_CMD_SCENES_VIEW_SCENE            = 0x01,
  /** Remove scene command */
  ZB_ZCL_CMD_SCENES_REMOVE_SCENE          = 0x02,
  /** Remove all scenes command */
  ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES     = 0x03,
  /** Store scene command */
  ZB_ZCL_CMD_SCENES_STORE_SCENE           = 0x04,
  /** Recall scene command */
  ZB_ZCL_CMD_SCENES_RECALL_SCENE          = 0x05,
  /** Get scene membership command */
  ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP  = 0x06,
  /** The Enhanced Add Scene command allows a scene to be added using a finer scene
   *  transition time than the Add Scene command. */
  ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE    = 0x40,
  /** The Enhanced View Scene command allows a scene to be retrieved using
   *  a finer scene transition time than the View Scene command. */
  ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE   = 0x41,
  /** The Copy Scene command allows a device to efficiently copy scenes from
   *  one group/scene identifier pair to another group/scene identifier pair. */
  ZB_ZCL_CMD_SCENES_COPY_SCENE            = 0x42,
};

/*! @brief Command response identifiers for Scenes Cluster
    @see ZCL spec, subclause 3.7.2.5
*/
enum zb_zcl_scenes_cmd_resp_e
{
  /** Add scene response */
  ZB_ZCL_CMD_SCENES_ADD_SCENE_RESPONSE            = 0x00,
  /** View scene response */
  ZB_ZCL_CMD_SCENES_VIEW_SCENE_RESPONSE           = 0x01,
  /** Remove scene response */
  ZB_ZCL_CMD_SCENES_REMOVE_SCENE_RESPONSE         = 0x02,
  /** Remove all scenes response */
  ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES_RESPONSE    = 0x03,
  /** Store scene response */
  ZB_ZCL_CMD_SCENES_STORE_SCENE_RESPONSE          = 0x04,
  /** Get scene membership response */
  ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP_RESPONSE = 0x06,
  /** The Enhanced Add Scene Response command allows a device to respond
   *  to an Enhanced Add Scene command.*/
  ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE_RESPONSE   = 0x40,
  /** The Enhanced View Scene Response command allows a device to respond
   *  to an Enhanced View Scene command using a finer scene transition time
   *  that was available in the ZCL. */
  ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE_RESPONSE  = 0x41,
  /** The Copy Scene Response command allows a device to respond
   *  to a Copy Scene command.*/
  ZB_ZCL_CMD_SCENES_COPY_SCENE_RESPONSE           = 0x42,
};

/** @cond internals_doc */
/* Scenes cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_SCENES_SERVER_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_CMD_SCENES_ADD_SCENE_RESPONSE,        \
                                      ZB_ZCL_CMD_SCENES_VIEW_SCENE_RESPONSE,       \
                                      ZB_ZCL_CMD_SCENES_REMOVE_SCENE_RESPONSE,     \
                                      ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES_RESPONSE, \
                                      ZB_ZCL_CMD_SCENES_STORE_SCENE_RESPONSE,      \
                                      ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP_RESPONSE

#define ZB_ZCL_CLUSTER_ID_SCENES_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_SCENES_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_SCENES_CLIENT_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_CMD_SCENES_ADD_SCENE,                 \
                                      ZB_ZCL_CMD_SCENES_VIEW_SCENE,                \
                                      ZB_ZCL_CMD_SCENES_REMOVE_SCENE,              \
                                      ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES,         \
                                      ZB_ZCL_CMD_SCENES_STORE_SCENE,               \
                                      ZB_ZCL_CMD_SCENES_RECALL_SCENE,              \
                                      ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP

#define ZB_ZCL_CLUSTER_ID_SCENES_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_SCENES_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/**
 *  Inform User App about Scene cluster Add Scene command.
 *
 */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_add_scene_req_s
{
  zb_uint16_t group_id;         /**< Scene group identifier */
  zb_uint8_t scene_id;          /**< Scene identifier */
  zb_uint16_t transition_time;  /**< Scene transition time */
  /*! @brief Scene name

      Has zero length (for compatibility) as unsupported attribute.
  */
  zb_uint8_t scene_name[1];
} ZB_PACKED_STRUCT zb_zcl_scenes_add_scene_req_t;


/*! @brief Common macro to start filling Add scene command frame

    Fills the constant-structure part of the command payload only.

    Command can contain no fieldsets defining empty scene. The new scene can be
    used, for example, for storing fieldsets with Store scene command.
    @note According to a 3.7.2.4.2. subclause of ZCL spec, the command shall be
    addressed to a single device (not a group).
    @param buffer to put packet to
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    command payload (fieldsets) should be placed.
    @param dis_default_resp - enable/disable default response
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
    @param transition_time - scene transition time
    @param add_scene_cmd_id - a scene cmd ID (Add Scene or Enh Add Scene)
*/
#define ZB_ZCL_SCENES_INIT_ADD_SCENE_REQ_COMMON(                     \
    buffer,                                                          \
    cmd_struct_ptr,                                                  \
    dis_default_resp,                                                \
    group_id,                                                        \
    scene_id,                                                        \
    transition_time,                                                 \
    add_scene_cmd_id)                                                \
  {                                                                  \
    cmd_struct_ptr = ZB_ZCL_START_PACKET_REQ(buffer)                 \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(             \
      cmd_struct_ptr,                                                \
      dis_default_resp)                                              \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                             \
      cmd_struct_ptr,                                                \
      ZB_ZCL_GET_SEQ_NUM(),                                          \
      add_scene_cmd_id);                                             \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_struct_ptr, (group_id));        \
    ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (scene_id));             \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_struct_ptr, (transition_time)); \
    ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, 0);                      \
  }

/*! @brief Start filling Add scene command frame

    Fills the constant-structure part of the command payload only.

    Command can contain no fieldsets defining empty scene. The new scene can be
    used, for example, for storing fieldsets with Store scene command.
    @note According to a 3.7.2.4.2. subclause of ZCL spec, the command shall be
    addressed to a single device (not a group).
    @param buffer to put packet to
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    command payload (fieldsets) should be placed.
    @param dis_default_resp - enable/disable default response
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
    @param transition_time - scene transition time (in seconds)
*/
#define ZB_ZCL_SCENES_INIT_ADD_SCENE_REQ(                            \
    buffer,                                                          \
    cmd_struct_ptr,                                                  \
    dis_default_resp,                                                \
    group_id,                                                        \
    scene_id,                                                        \
    transition_time)                                                 \
  ZB_ZCL_SCENES_INIT_ADD_SCENE_REQ_COMMON(                           \
    (buffer),                                                        \
    (cmd_struct_ptr),                                                \
    (dis_default_resp),                                              \
    (group_id),                                                      \
    (scene_id),                                                      \
    (transition_time),                                               \
    (ZB_ZCL_CMD_SCENES_ADD_SCENE))

#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED

/*! @brief Start filling Enhanced Add scene command frame

    Fills the constant-structure part of the command payload only.

    Command can contain no fieldsets defining empty scene. The new scene can be
    used, for example, for storing fieldsets with Store scene command.
    @note According to a 3.7.2.4.2. subclause of ZCL spec, the command shall be
    addressed to a single device (not a group).
    @param buffer to put packet to
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    command payload (fieldsets) should be placed.
    @param dis_default_resp - enable/disable default response
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
    @param transition_time - scene transition time (in tenths of a second)
*/
#define ZB_ZCL_SCENES_INIT_ENHANCED_ADD_SCENE_REQ(                   \
    buffer,                                                          \
    cmd_struct_ptr,                                                  \
    dis_default_resp,                                                \
    group_id,                                                        \
    scene_id,                                                        \
    transition_time)                                                 \
  ZB_ZCL_SCENES_INIT_ADD_SCENE_REQ_COMMON(                           \
    (buffer),                                                        \
    (cmd_struct_ptr),                                                \
    (dis_default_resp),                                              \
    (group_id),                                                      \
    (scene_id),                                                      \
    (transition_time),                                               \
    (ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE))

#endif /* !ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED */

/*! @brief Sends Add scene command

    @note According to a 3.7.2.4.2. subclause of ZCL spec, the command shall be
    addressed to a single device (not a group).
    @param buffer - with prepared command payload
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
*/
#define ZB_ZCL_SCENES_SEND_ADD_SCENE_REQ(         \
    buffer,                                       \
    cmd_struct_ptr,                               \
    dst_addr,                                     \
    dst_ep,                                       \
    ep,                                           \
    prof_id,                                      \
    callback)                                     \
  {                                               \
    ZB_ZCL_FINISH_PACKET(buffer, cmd_struct_ptr)  \
    ZB_ZCL_SEND_COMMAND_SHORT(                    \
        buffer,                                   \
        dst_addr,                                 \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,         \
        dst_ep,                                   \
        ep,                                       \
        prof_id,                                  \
        ZB_ZCL_CLUSTER_ID_SCENES,                 \
        callback);                                \
  }

/** @brief Adds fieldset into the buffer for sending command
    @param cmd_struct_ptr - pointer to the place in the buffer to put data to
    @param cluster identifier the fieldset under consideration belongs to
    @param fs_length - summary length of the attributes in the fieldset
    @attention The order of the attribute values in the fieldset is significant
*/
#define ZB_ZCL_SCENES_INIT_FIELDSET(cmd_struct_ptr, cluster, fs_length) \
  {                                                                     \
    ZB_ZCL_PACKET_PUT_DATA16_VAL((cmd_struct_ptr), (cluster));          \
    ZB_ZCL_PACKET_PUT_DATA8((cmd_struct_ptr), (fs_length));             \
  }


/** @brief Parse common part of Add Scene command
    @param buffer containing Store scene command payload
    @param cmd_struct_ptr - pointer to the request representation structure of type zb_zcl_scenes_add_scene_req_t or NULL pointer if payload size is
    too small
    @attention The macro changes content of the buffer
    @note The macro ignores scene name in the buffer, and, if it has non-zero
    size, skips it and directly sets length of the scene name to zero in the
    command representation structure
*/
#define ZB_ZCL_SCENES_GET_ADD_SCENE_REQ_COMMON(buffer, cmd_struct_ptr) \
  {                                                                    \
    if (sizeof(zb_zcl_scenes_add_scene_req_t) > zb_buf_len((buffer)))  \
    {                                                                  \
      (cmd_struct_ptr) = NULL;                                         \
    }                                                                  \
    else                                                               \
    {                                                                  \
      (cmd_struct_ptr) =                                               \
        (zb_zcl_scenes_add_scene_req_t*)zb_buf_begin(buffer);          \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);             \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->transition_time);      \
      (void)zb_buf_cut_left(                                           \
          (buffer),                                                    \
          sizeof(zb_zcl_scenes_add_scene_req_t) +                      \
            (cmd_struct_ptr)->scene_name[0]);                          \
      (cmd_struct_ptr)->scene_name[0] = 0;                             \
    }                                                                  \
  }

/** @brief Parses fieldset description
    @param buffer containing fieldset
    @param cmd_struct_ptr - pointer to the fieldset description representation
    structure (of type @ref zb_zcl_scenes_fieldset_common_s) or NULL pointer if
    payload size is too small
    @param content_length - variable to contain size of the fieldset content
    left unprocessed
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_ADD_SCENE_REQ_NEXT_FIELDSET_DESC(             \
    buffer,                                                             \
    cmd_struct_ptr,                                                     \
    content_length)                                                     \
  {                                                                     \
    if (sizeof(zb_zcl_scenes_fieldset_common_t) > zb_buf_len((buffer))) \
    {                                                                   \
      (cmd_struct_ptr) = NULL;                                          \
    }                                                                   \
    else                                                                \
    {                                                                   \
      (cmd_struct_ptr) = (zb_zcl_scenes_fieldset_common_t *)zb_buf_begin(buffer); \
      (content_length) = sizeof(zb_zcl_scenes_fieldset_common_t) +      \
          ( (zb_zcl_scenes_fieldset_common_t*)                          \
            (cmd_struct_ptr))->fieldset_length;                         \
      if ((content_length) > zb_buf_len((buffer)))                      \
      {                                                                 \
        (cmd_struct_ptr) = NULL;                                        \
        (content_length) = 0;                                           \
      }                                                                 \
      else                                                              \
      {                                                                 \
        /* Cludgy: decoding cluster_id in the fieldset directly */      \
        ZB_ZCL_HTOLE16_INPLACE((cmd_struct_ptr));                       \
        (void)zb_buf_cut_left((buffer), (content_length));              \
      }                                                                 \
    }                                                                   \
  }

/** @brief Copies parsed fieldset description into designated memory area
    @param src_data_ptr - pointer to the memory area containing parsed fieldset
    description
    @param dst_data_ptr - pointer to the memory area to put fieldset description
    to
    @param content_length - remaining length of the unprocessed fieldset content
*/
#define ZB_ZCL_SCENES_GET_ADD_SCENE_REQ_STORE_FIELDSET_COMMON(               \
    src_data_ptr,                                                            \
    dst_data_ptr,                                                            \
    content_length)                                                          \
  {                                                                          \
    ZB_MEMCPY(                                                               \
        (dst_data_ptr),                                                      \
        (src_data_ptr),                                                      \
        sizeof(zb_zcl_scenes_fieldset_common_t));                            \
    ((zb_zcl_scenes_fieldset_common_t*)(dst_data_ptr))->fieldset_length = 0; \
    content_length -= sizeof(zb_zcl_scenes_fieldset_common_t);               \
  }

/*! @brief View scene command payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_view_scene_req_s
{
  zb_uint16_t group_id; /*!< Scene group identifier */
  zb_uint8_t scene_id;  /*!< Scene identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_view_scene_req_t;

/*! @brief Common macro to send View Scene commands

    @note According to a 3.7.2.4.2. subclause of ZCL spec, the command shall be
    addressed to a single device (not a group).
    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
    @param view_scene_cmd_id - command ID (View Scene or Enhanced View Scene)
*/
#define ZB_ZCL_SCENES_SEND_VIEW_SCENE_REQ_COMMON(        \
    buffer,                                              \
    dst_addr,                                            \
    dst_ep,                                              \
    ep,                                                  \
    prof_id,                                             \
    dis_default_resp,                                    \
    callback,                                            \
    group_id,                                            \
    scene_id,                                            \
    view_scene_cmd_id)                                   \
  {                                                      \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)      \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL( \
        ptr,                                             \
        dis_default_resp)                               \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                     \
        ptr,                                             \
        ZB_ZCL_GET_SEQ_NUM(),                            \
        view_scene_cmd_id);                              \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));       \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (scene_id));            \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                   \
    ZB_ZCL_SEND_COMMAND_SHORT(                           \
        buffer,                                          \
        dst_addr,                                        \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                \
        dst_ep,                                          \
        ep,                                              \
        prof_id,                                         \
        ZB_ZCL_CLUSTER_ID_SCENES,                        \
        callback);                                       \
  }

/*! @brief Send View Scene commands

    @note According to a 3.7.2.4.2. subclause of ZCL spec, the command shall be
    addressed to a single device (not a group).
    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_VIEW_SCENE_REQ(               \
  buffer,                                                \
  dst_addr,                                              \
  dst_ep,                                                \
  ep,                                                    \
  prof_id,                                               \
  dis_default_resp,                                      \
  callback,                                              \
  group_id,                                              \
  scene_id)                                              \
  ZB_ZCL_SCENES_SEND_VIEW_SCENE_REQ_COMMON(              \
    buffer,                                              \
    dst_addr,                                            \
    dst_ep,                                              \
    ep,                                                  \
    prof_id,                                             \
    dis_default_resp,                                    \
    callback,                                            \
    group_id,                                            \
    scene_id,                                            \
    ZB_ZCL_CMD_SCENES_VIEW_SCENE)

#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED

/*! @brief Send Enhanced View Scene command

    @note According to a 3.7.2.4.2. subclause of ZCL spec, the command shall be
    addressed to a single device (not a group).
    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_ENHANCED_VIEW_SCENE_REQ(      \
  buffer,                                                \
  dst_addr,                                              \
  dst_ep,                                                \
  ep,                                                    \
  prof_id,                                               \
  dis_default_resp,                                      \
  callback,                                              \
  group_id,                                              \
  scene_id)                                              \
  ZB_ZCL_SCENES_SEND_VIEW_SCENE_REQ_COMMON(              \
    buffer,                                              \
    dst_addr,                                            \
    dst_ep,                                              \
    ep,                                                  \
    prof_id,                                             \
    dis_default_resp,                                    \
    callback,                                            \
    group_id,                                            \
    scene_id,                                            \
    ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE)

#endif /* !ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED */

/** @brief Parses Scenes.ViewScene command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_view_scene_req_s) or NULL pointer if payload size is
    too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_VIEW_SCENE_REQ(buffer, cmd_struct_ptr)       \
  {                                                                    \
    if (sizeof(zb_zcl_scenes_view_scene_req_t) > zb_buf_len((buffer))) \
    {                                                                  \
      (cmd_struct_ptr) = NULL;                                         \
    }                                                                  \
    else                                                               \
    {                                                                  \
      (cmd_struct_ptr) =                                               \
        (zb_zcl_scenes_view_scene_req_t*)zb_buf_begin(buffer);         \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);             \
    }                                                                  \
  }

/*! @brief Remove scene command payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_remove_scene_req_s
{
  zb_uint16_t group_id; /*!< Scene group identifier */
  zb_uint8_t scene_id;  /*!< Scene identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_remove_scene_req_t;

/*! @brief Send Remove scene command

    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode. Either @ref
    ZB_APS_ADDR_MODE_16_ENDP_PRESENT or @ref
    ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT will fit
    @param dst_ep - destination endpoint (if sendting to a particular device)
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_REMOVE_SCENE_REQ(             \
    buffer,                                              \
    dst_addr,                                            \
    dst_addr_mode,                                       \
    dst_ep,                                              \
    ep,                                                  \
    prof_id,                                             \
    dis_default_resp,                                    \
    callback,                                            \
    group_id,                                            \
    scene_id)                                            \
  {                                                      \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)       \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL( \
        ptr,                                             \
        dis_default_resp)                               \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                     \
        ptr,                                             \
        ZB_ZCL_GET_SEQ_NUM(),                            \
        ZB_ZCL_CMD_SCENES_REMOVE_SCENE);                 \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));       \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (scene_id));            \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                   \
    ZB_ZCL_SEND_COMMAND_SHORT(                           \
        buffer,                                          \
        dst_addr,                                        \
        dst_addr_mode,                                   \
        dst_ep,                                          \
        ep,                                              \
        prof_id,                                         \
        ZB_ZCL_CLUSTER_ID_SCENES,                        \
        callback);                                       \
  }

/** @brief Parses Scenes.RemoveScene command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_remove_scene_req_s) or NULL pointer if payload size
    is too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_REMOVE_SCENE_REQ(buffer, cmd_struct_ptr)       \
  {                                                                      \
    if (sizeof(zb_zcl_scenes_remove_scene_req_t) > zb_buf_len((buffer))) \
    {                                                                    \
      (cmd_struct_ptr) = NULL;                                           \
    }                                                                    \
    else                                                                 \
    {                                                                    \
      (cmd_struct_ptr) =                                                 \
        (zb_zcl_scenes_remove_scene_req_t*)zb_buf_begin(buffer);         \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);               \
    }                                                                    \
  }

/*! @brief Remove all scenes command payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_remove_all_scenes_req_s
{
  zb_uint16_t group_id; /*!< Scene group identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_remove_all_scenes_req_t;

/*! @brief Send Remove all scenes command

    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode. Either @ref
    ZB_APS_ADDR_MODE_16_ENDP_PRESENT or @ref
    ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT will fit
    @param dst_ep - destination endpoint (if sendting to a particular device)
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_REMOVE_ALL_SCENES_REQ(        \
    buffer,                                              \
    dst_addr,                                            \
    dst_addr_mode,                                       \
    dst_ep,                                              \
    ep,                                                  \
    prof_id,                                             \
    dis_default_resp,                                    \
    callback,                                            \
    group_id)                                            \
  {                                                      \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)       \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL( \
        ptr,                                             \
        dis_default_resp)                               \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                     \
        ptr,                                             \
        ZB_ZCL_GET_SEQ_NUM(),                            \
        ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES);            \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));       \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                   \
    ZB_ZCL_SEND_COMMAND_SHORT(                           \
        buffer,                                          \
        dst_addr,                                        \
        dst_addr_mode,                                   \
        dst_ep,                                          \
        ep,                                              \
        prof_id,                                         \
        ZB_ZCL_CLUSTER_ID_SCENES,                        \
        callback);                                       \
  }

/** @brief Parses Scenes.RemoveScene command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_remove_scene_req_s) or NULL pointer if payload size
    is too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_REMOVE_ALL_SCENES_REQ(buffer, cmd_struct_ptr) \
  {                                                                     \
    if (  sizeof(zb_zcl_scenes_remove_all_scenes_req_t)                 \
        > zb_buf_len((buffer)))                                         \
    {                                                                   \
      (cmd_struct_ptr) = NULL;                                          \
    }                                                                   \
    else                                                                \
    {                                                                   \
      (cmd_struct_ptr) =                                                \
        (zb_zcl_scenes_remove_all_scenes_req_t*)zb_buf_begin(buffer);   \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);              \
    }                                                                   \
  }

/*! @brief Store scene command payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_store_scene_req_s
{
  zb_uint16_t group_id; /*!< Group identifier */
  zb_uint8_t scene_id;  /*!< Scene identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_store_scene_req_t;

/*! @brief Send Store Scene command

    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode. Either @ref
    ZB_APS_ADDR_MODE_16_ENDP_PRESENT or @ref
    ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT will fit
    @param dst_ep - destination endpoint (if sendting to a particular device)
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_STORE_SCENE_REQ(              \
    buffer,                                              \
    dst_addr,                                            \
    dst_addr_mode,                                       \
    dst_ep,                                              \
    ep,                                                  \
    prof_id,                                             \
    dis_default_resp,                                    \
    callback,                                            \
    group_id,                                            \
    scene_id)                                            \
  {                                                      \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)       \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL( \
        ptr,                                             \
        dis_default_resp)                               \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                     \
        ptr,                                             \
        ZB_ZCL_GET_SEQ_NUM(),                            \
        ZB_ZCL_CMD_SCENES_STORE_SCENE);                  \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));       \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (scene_id));            \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                   \
    ZB_ZCL_SEND_COMMAND_SHORT(                           \
        buffer,                                          \
        dst_addr,                                        \
        dst_addr_mode,                                   \
        dst_ep,                                          \
        ep,                                              \
        prof_id,                                         \
        ZB_ZCL_CLUSTER_ID_SCENES,                        \
        callback);                                       \
  }

/** @brief Parse Store Scene command
    @param buffer containing Store scene command payload
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_store_scene_req_s) or NULL pointer if payload size
    is too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_STORE_SCENE_REQ(buffer, cmd_struct_ptr)        \
  {                                                                      \
    if (sizeof(zb_zcl_scenes_store_scene_req_t) > zb_buf_len((buffer)))  \
    {                                                                    \
      (cmd_struct_ptr) = NULL;                                           \
    }                                                                    \
    else                                                                 \
    {                                                                    \
      ZB_ZCL_HTOLE16_INPLACE(zb_buf_begin((buffer)));                    \
      (cmd_struct_ptr) =                                                 \
        (zb_zcl_scenes_store_scene_req_t*)zb_buf_begin(buffer);          \
      (void)zb_buf_cut_left(                                             \
          (buffer),                                                      \
          sizeof(zb_zcl_scenes_store_scene_req_t));                      \
    }                                                                    \
  }

/*! @brief Recall scene command payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_recall_scene_req_s
{
  zb_uint16_t group_id; /*!< Scene group identifier */
  zb_uint8_t scene_id;  /*!< Scene identifier */
  zb_uint16_t transition_time; /*!< Transition Time identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_recall_scene_req_t;

/*! @brief Recall scene command transition_time payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_recall_scene_transition_time_req_s
{
  zb_uint16_t transition_time; /*!< Determines how long the transition takes from the old cluster state to the new cluster state */
} ZB_PACKED_STRUCT zb_zcl_scenes_recall_scene_transition_time_req_t;

/*! @brief Send Recall scene command
    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode. Either @ref
    ZB_APS_ADDR_MODE_16_ENDP_PRESENT or @ref
    ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT will fit
    @param dst_ep - destination endpoint (if sendting to a particular device)
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
    @param transition_time - determines how long the transition takes from the old cluster state to the new cluster state
*/
void zb_zcl_scenes_send_recall_scene_req_zcl8(zb_bufid_t buffer,
                                              const zb_addr_u *dst_addr,
                                              zb_uint8_t dst_addr_mode,
                                              zb_uint8_t dst_ep,
                                              zb_uint8_t ep,
                                              zb_uint16_t prof_id,
                                              zb_uint8_t dis_default_resp,
                                              zb_callback_t callback,
                                              zb_uint16_t group_id,
                                              zb_uint8_t scene_id,
                                              zb_uint16_t transition_time);

/*! @brief Send Recall scene command (pre-ZCL8)
    Use @ref zb_zcl_scenes_send_recall_scene_req_zcl8 for ZCL8 revision call.
    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode. Either @ref
    ZB_APS_ADDR_MODE_16_ENDP_PRESENT or @ref
    ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT will fit
    @param dst_ep - destination endpoint (if sendting to a particular device)
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
void zb_zcl_scenes_send_recall_scene_req(zb_bufid_t buffer,
                                         const zb_addr_u *dst_addr,
                                         zb_uint8_t dst_addr_mode,
                                         zb_uint8_t dst_ep,
                                         zb_uint8_t ep,
                                         zb_uint16_t prof_id,
                                         zb_uint8_t dis_default_resp,
                                         zb_callback_t callback,
                                         zb_uint16_t group_id,
                                         zb_uint8_t scene_id);

/** Macro for call @ref zb_zcl_scenes_send_recall_scene_req_zcl8 function
 */
#define ZB_ZCL_SCENES_SEND_RECALL_SCENE_REQ_ZCL8( \
    buffer,                                       \
    dst_addr,                                     \
    dst_addr_mode,                                \
    dst_ep,                                       \
    ep,                                           \
    prof_id,                                      \
    dis_default_resp,                             \
    callback,                                     \
    group_id,                                     \
    scene_id,                                     \
    transition_time)                              \
  {                                               \
    zb_zcl_scenes_send_recall_scene_req_zcl8(     \
    buffer,                                       \
    ZB_ADDR_U_CAST(dst_addr),                     \
    dst_addr_mode,                                \
    dst_ep,                                       \
    ep,                                           \
    prof_id,                                      \
    dis_default_resp,                             \
    callback,                                     \
    group_id,                                     \
    scene_id,                                     \
    transition_time);                             \
  }

/** Macro for calling @ref zb_zcl_scenes_send_recall_scene_req function
 */
#define ZB_ZCL_SCENES_SEND_RECALL_SCENE_REQ( \
    buffer,                                  \
    dst_addr,                                \
    dst_addr_mode,                           \
    dst_ep,                                  \
    ep,                                      \
    prof_id,                                 \
    dis_default_resp,                        \
    callback,                                \
    group_id,                                \
    scene_id)                                \
  {                                          \
    zb_zcl_scenes_send_recall_scene_req(     \
    buffer,                                  \
    ZB_ADDR_U_CAST(dst_addr),                \
    dst_addr_mode,                           \
    dst_ep,                                  \
    ep,                                      \
    prof_id,                                 \
    dis_default_resp,                        \
    callback,                                \
    group_id,                                \
    scene_id);                               \
  }


/** @brief Recall scene payload length macro */
#define ZB_ZCL_SCENES_RECALL_SCENE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_scenes_recall_scene_req_t)

/** @brief Transition Time field of Recall scene payload length macro */
#define ZB_ZCL_SCENES_RECALL_SCENE_REQ_TRANSITION_TIME_PAYLOAD_LEN \
  sizeof(zb_uint16_t)

/** @brief Transition Time field invalid value */
#define ZB_ZCL_SCENES_RECALL_SCENE_REQ_TRANSITION_TIME_INVALID_VALUE \
  (zb_uint16_t)0xFFFF

/** @brief Parse Recall Scene command
    @param buffer containing Store scene command payload
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_recall_scene_req_s) or NULL pointer if payload size
    is too small
    @param req_len - length of ZCL command
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_RECALL_SCENE_REQ(buffer, cmd_struct_ptr, req_len)                                     \
{                                                                                                               \
  zb_zcl_scenes_recall_scene_req_t *recall_scene_req_ptr;                                                       \
  req_len = zb_buf_len(buffer);                                                                                 \
  (recall_scene_req_ptr) = (req_len) >=                                                                         \
    (ZB_ZCL_SCENES_RECALL_SCENE_REQ_PAYLOAD_LEN - ZB_ZCL_SCENES_RECALL_SCENE_REQ_TRANSITION_TIME_PAYLOAD_LEN) ? \
    (zb_zcl_scenes_recall_scene_req_t*)zb_buf_begin(buffer) : NULL;                                             \
  if (recall_scene_req_ptr)                                                                                     \
  {                                                                                                             \
      ZB_ZCL_HTOLE16_INPLACE(zb_buf_begin((buffer)));                                                           \
      (cmd_struct_ptr) =                                                                                        \
        (zb_zcl_scenes_recall_scene_req_t*)zb_buf_begin(buffer);                                                \
      (void)zb_buf_cut_left(                                                                                    \
          (buffer),                                                                                             \
          (req_len));                                                                                           \
  }                                                                                                             \
  else                                                                                                          \
  {                                                                                                             \
    cmd_struct_ptr = NULL;                                                                                      \
  }                                                                                                             \
}

/*! @brief Get scene membership command payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_get_scene_membership_req_s
{
  zb_uint16_t group_id; /*!< Scene group identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_get_scene_membership_req_t;

/*! @brief Send Remove all scenes command

    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode. Either @ref
    ZB_APS_ADDR_MODE_16_ENDP_PRESENT or @ref
    ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT will fit
    @param dst_ep - destination endpoint (if sendting to a particular device)
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param group_id - group identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_GET_SCENE_MEMBERSHIP_REQ(     \
    buffer,                                              \
    dst_addr,                                            \
    dst_addr_mode,                                       \
    dst_ep,                                              \
    ep,                                                  \
    prof_id,                                             \
    dis_default_resp,                                    \
    callback,                                            \
    group_id)                                            \
  {                                                      \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)       \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL( \
        ptr,                                             \
        dis_default_resp)                               \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                     \
        ptr,                                             \
        ZB_ZCL_GET_SEQ_NUM(),                            \
        ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP);         \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));       \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                   \
    ZB_ZCL_SEND_COMMAND_SHORT(                           \
        buffer,                                          \
        dst_addr,                                        \
        dst_addr_mode,                                   \
        dst_ep,                                          \
        ep,                                              \
        prof_id,                                         \
        ZB_ZCL_CLUSTER_ID_SCENES,                        \
        callback);                                       \
  }

#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED

/*! @brief Send Copy scene command

    The command can be sent to device or group of devices either
    @param buffer to put packet to
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint (if sendting to a particular device)
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - enable/disable default response
    @param callback for getting command send status
    @param mode - a scene copying mode
    @param group_id_from - group identifier for the scene to copy from
    @param scene_id_from - scene identifier to copy from
    @param group_id_to - group identifier for the scene to copy to
    @param scene_id_to - scene identifier to copy to
*/
#define ZB_ZCL_SCENES_SEND_COPY_SCENE_REQ(                      \
  buffer,                                                       \
  dst_addr,                                                     \
  dst_ep,                                                       \
  ep,                                                           \
  prof_id,                                                      \
  dis_default_resp,                                             \
  callback,                                                     \
  mode,                                                         \
  group_id_from,                                                \
  scene_id_from,                                                \
  group_id_to,                                                  \
  scene_id_to)                                                  \
  {                                                             \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)           \
      ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(      \
        ptr,                                                    \
        dis_default_resp)                                       \
      ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                      \
        ptr,                                                    \
        ZB_ZCL_GET_SEQ_NUM(),                                   \
        ZB_ZCL_CMD_SCENES_COPY_SCENE);                          \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (mode));                       \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id_from));         \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (scene_id_from));              \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id_to));           \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (scene_id_to));                \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                           \
      ZB_ZCL_SEND_COMMAND_SHORT(                                \
        buffer,                                                 \
        dst_addr,                                               \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                       \
        dst_ep,                                                 \
        ep,                                                     \
        prof_id,                                                \
        ZB_ZCL_CLUSTER_ID_SCENES,                               \
        callback);                                              \
  }

/*! @brief Send Copy scene Response command

    The command can be sent to device or group of devices either

    @attention - Application must send reply for this command.

    @param buffer to put packet to
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    command payload (fieldsets) should be placed.
    @param seq_num - ZCL sequence number
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint (if sendting to a particular device)
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback - for getting command send status
    @param status - copying status
    @param group_id_from - a group ID to copy from
    @param scene_id_from - a scene ID to copy from
*/
#define ZB_ZCL_SCENES_SEND_COPY_SCENE_RES(                              \
  buffer,                                                               \
  cmd_struct_ptr,                                                       \
  seq_num,                                                              \
  dst_addr,                                                             \
  dst_ep,                                                               \
  ep,                                                                   \
  prof_id,                                                              \
  callback,                                                             \
  status,                                                               \
  group_id_from,                                                        \
  scene_id_from)                                                        \
  {                                                                     \
    cmd_struct_ptr = ZB_ZCL_START_PACKET(buffer);                       \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(                \
      cmd_struct_ptr);                                                  \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                    \
      cmd_struct_ptr,                                                   \
      seq_num,                                                          \
      ZB_ZCL_CMD_SCENES_COPY_SCENE_RESPONSE);                           \
    ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (status));                  \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_struct_ptr, (group_id_from));      \
    ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (scene_id_from));           \
    ZB_ZCL_FINISH_PACKET(buffer, cmd_struct_ptr)                        \
      ZB_ZCL_SEND_COMMAND_SHORT(                                        \
        buffer,                                                         \
        dst_addr,                                                       \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                               \
        dst_ep,                                                         \
        ep,                                                             \
        prof_id,                                                        \
        ZB_ZCL_CLUSTER_ID_SCENES,                                       \
        callback);                                                      \
  }

#endif /* !ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED */

/** @brief Parses Scenes.GetSceneMembership command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_get_scene_membership_req_s) or NULL pointer if payload size
    is too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_GET_SCENE_MEMBERSHIP_REQ(buffer, cmd_struct_ptr) \
  {                                                                        \
    if (  sizeof(zb_zcl_scenes_get_scene_membership_req_t)                 \
        > zb_buf_len((buffer)))                                            \
    {                                                                      \
      (cmd_struct_ptr) = NULL;                                             \
    }                                                                      \
    else                                                                   \
    {                                                                      \
      (cmd_struct_ptr) =                                                   \
        (zb_zcl_scenes_get_scene_membership_req_t*)zb_buf_begin(buffer);   \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);                 \
    }                                                                      \
  }

#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED

#define ZB_ZCL_COPY_ALL_SCENES_ENABLED 0x01U
#define ZB_ZCL_COPY_ALL_SCENES_DISABLED 0x00U

/*! @brief Scene copying mode bitfield */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_copy_scene_mode_s
{
  zb_bitfield_t copy_all_scenes:1; /*!< The Copy All Scenes subfield is 1-bit in length and
                                     indicates whether all scenes are to be copied. If this value
                                     is set to 1, all scenes are to be copied and the Scene Identifier From
                                     and Scene Identifier To fields SHALL be ignored.
                                     Otherwise this field is set to 0. */
  zb_bitfield_t reserved:7;        /*!< Reserved field */
} ZB_PACKED_STRUCT zb_zcl_scenes_copy_scene_mode_t;

/*! @brief Copy scene command payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_copy_scene_req_s
{
  zb_zcl_scenes_copy_scene_mode_t mode;  /*!< The mode field is 8-bits in length and contains
                                           information of how the scene copy is to proceed.
                                           See @ref zb_zcl_scenes_copy_scene_mode_s */
  zb_uint16_t group_id_from;             /*!< Scene group identifier from */
  zb_uint8_t scene_id_from;              /*!< Scene identifier from */
  zb_uint16_t group_id_to;               /*!< Scene group identifier to */
  zb_uint8_t scene_id_to;                /*!< Scene identifier to */
} ZB_PACKED_STRUCT zb_zcl_scenes_copy_scene_req_t;

#define ZB_ZCL_SCENES_GET_COPY_SCENE_REQ(buffer, cmd_struct_ptr)         \
  {                                                                      \
    if (sizeof(zb_zcl_scenes_copy_scene_req_t) > zb_buf_len((buffer)))   \
    {                                                                    \
      (cmd_struct_ptr) = NULL;                                           \
    }                                                                    \
    else                                                                 \
    {                                                                    \
      (cmd_struct_ptr) =                                                 \
        (zb_zcl_scenes_copy_scene_req_t*)zb_buf_begin(buffer);           \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id_from);          \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id_to);            \
    }                                                                    \
  }

#endif /* !ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED */

/*! @brief Add scene response payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_add_scene_res_s
{
  zb_uint8_t status;    /*!< Add scene status */
  zb_uint16_t group_id; /*!< Scene group identifier */
  zb_uint8_t scene_id;  /*!< Scene identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_add_scene_res_t;

/*! @brief Common macro to send Add Scene response

    @param buffer to put packet to
    @param seq_num - ZCL sequence number
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
    @param status of the corresponding Add scene command execution. Appropriate
    values (defined in @ref zcl_status enumeration)are:
    @li @ref ZB_ZCL_STATUS_SUCCESS for successful result
    @li @ref ZB_ZCL_STATUS_INSUFF_SPACE if the scene table is full
    @li @ref ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    Group Table
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
    @param cmd_id - a response cmd ID (Add Scene or Enh Add Scene)
*/
#define ZB_ZCL_SCENES_SEND_ADD_SCENE_RES_COMMON(              \
    buffer,                                                   \
    seq_num,                                                  \
    dst_addr,                                                 \
    dst_ep,                                                   \
    ep,                                                       \
    prof_id,                                                  \
    callback,                                                 \
    status,                                                   \
    group_id,                                                 \
    scene_id,                                                 \
    cmd_id)                                                   \
  {                                                           \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);            \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr); \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                          \
        ptr,                                                  \
        seq_num,                                              \
        cmd_id);                                              \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (status));                   \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));            \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (scene_id));                 \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                        \
    ZB_ZCL_SEND_COMMAND_SHORT(                                \
        buffer,                                               \
        dst_addr,                                             \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                     \
        dst_ep,                                               \
        ep,                                                   \
        prof_id,                                              \
        ZB_ZCL_CLUSTER_ID_SCENES,                             \
        callback);                                            \
  }

/*! @brief Send Add Scene response

    @param buffer to put packet to
    @param seq_num - ZCL sequence number
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
    @param status of the corresponding Add scene command execution. Appropriate
    values (defined in @ref zcl_status enumeration)are:
    @li @ref ZB_ZCL_STATUS_SUCCESS for successful result
    @li @ref ZB_ZCL_STATUS_INSUFF_SPACE if the scene table is full
    @li @ref ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    Group Table
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_ADD_SCENE_RES(                     \
  buffer,                                                     \
  seq_num,                                                    \
  dst_addr,                                                   \
  dst_ep,                                                     \
  ep,                                                         \
  prof_id,                                                    \
  callback,                                                   \
  status,                                                     \
  group_id,                                                   \
  scene_id)                                                   \
  ZB_ZCL_SCENES_SEND_ADD_SCENE_RES_COMMON(                    \
    buffer,                                                   \
    seq_num,                                                  \
    dst_addr,                                                 \
    dst_ep,                                                   \
    ep,                                                       \
    prof_id,                                                  \
    callback,                                                 \
    status,                                                   \
    group_id,                                                 \
    scene_id,                                                 \
    ZB_ZCL_CMD_SCENES_ADD_SCENE_RESPONSE)

#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED

/*! @brief Send Enhanced Add Scene response

    @param buffer to put packet to
    @param seq_num - ZCL sequence number
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
    @param status of the corresponding Add scene command execution. Appropriate
    values (defined in @ref zcl_status enumeration)are:
    @li @ref ZB_ZCL_STATUS_SUCCESS for successful result
    @li @ref ZB_ZCL_STATUS_INSUFF_SPACE if the scene table is full
    @li @ref ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    Group Table
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_ENHANCED_ADD_SCENE_RES(            \
  buffer,                                                     \
  seq_num,                                                    \
  dst_addr,                                                   \
  dst_ep,                                                     \
  ep,                                                         \
  prof_id,                                                    \
  callback,                                                   \
  status,                                                     \
  group_id,                                                   \
  scene_id)                                                   \
  ZB_ZCL_SCENES_SEND_ADD_SCENE_RES_COMMON(                    \
    buffer,                                                   \
    seq_num,                                                  \
    dst_addr,                                                 \
    dst_ep,                                                   \
    ep,                                                       \
    prof_id,                                                  \
    callback,                                                 \
    status,                                                   \
    group_id,                                                 \
    scene_id,                                                 \
    ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE_RESPONSE)

#endif /* !ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED */

/** @brief Parses Scenes.AddSceneResponse command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_add_scene_res_s) or NULL pointer if payload size is
    too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_ADD_SCENE_RES(buffer, cmd_struct_ptr)       \
  {                                                                   \
    if (sizeof(zb_zcl_scenes_add_scene_res_t) > zb_buf_len((buffer))) \
    {                                                                 \
      (cmd_struct_ptr) = NULL;                                        \
    }                                                                 \
    else                                                              \
    {                                                                 \
      (cmd_struct_ptr) =                                              \
        (zb_zcl_scenes_add_scene_res_t*)zb_buf_begin(buffer);         \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);            \
    }                                                                 \
  }

/** @brief Mandatory part of the Scenes.ViewSceneResponse payload */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_view_scene_res_fixed_size_s
{
  zb_uint8_t status;    /*!< Command execution status */
  zb_uint16_t group_id; /*!< Scene group identifier */
  zb_uint8_t scene_id;  /*!< Scene identifier */
  /** @brief Scene transition time
      @attention Valid if status is @ref ZB_ZCL_STATUS_SUCCESS only
  */
  zb_uint16_t transition_time;
  /** @brief Scene name field.

      Since scene names are not supported, will contain only zero length byte on
      formation. If received packet with scene name filled from some other
      stack, the fields will be skipped during packet parsing, and length byte
      will be set to zero.
      @attention Valid if status is @ref ZB_ZCL_STATUS_SUCCESS only
  */
  zb_uint8_t scene_name[1];
} ZB_PACKED_STRUCT zb_zcl_scenes_view_scene_res_fixed_size_t;

/*! @brief Common macro to start filling View scene response frame

    Fills the mandatory part of the View scene response payload.

    @param buffer to put packet to
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    command payload (fieldsets) should be placed.
    @param seq_num - ZCL sequence number
    @param status of the command execution. Appropriate values (defined in @ref
    zcl_status enumeration) are:
    @li ZB_ZCL_STATUS_SUCCESS for successful result
    @li ZB_ZCL_STATUS_INSUFF_SPACE if fieldsets don't fit into packet
    @li ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
    @param view_res_cmd_id - a command ID (View Response or Enh View Response)
*/
#define ZB_ZCL_SCENES_INIT_VIEW_SCENE_RES_COMMON(             \
    buffer,                                                   \
    cmd_struct_ptr,                                           \
    seq_num,                                                  \
    status,                                                   \
    group_id,                                                 \
    scene_id,                                                 \
    view_res_cmd_id)                                          \
  {                                                           \
    cmd_struct_ptr = ZB_ZCL_START_PACKET(buffer);             \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(      \
        cmd_struct_ptr);                                      \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                          \
        cmd_struct_ptr,                                       \
        seq_num,                                              \
        view_res_cmd_id);                                     \
    ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (status));        \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_struct_ptr, (group_id)); \
    ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (scene_id));      \
  }

/*! @brief Start filling View scene response frame

    Fills the mandatory part of the View scene response payload.

    @param buffer to put packet to
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    command payload (fieldsets) should be placed.
    @param seq_num - ZCL sequence number
    @param status of the command execution. Appropriate values (defined in @ref
    zcl_status enumeration) are:
    @li ZB_ZCL_STATUS_SUCCESS for successful result
    @li ZB_ZCL_STATUS_INSUFF_SPACE if fieldsets don't fit into packet
    @li ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_INIT_VIEW_SCENE_RES(                    \
  buffer,                                                     \
  cmd_struct_ptr,                                             \
  seq_num,                                                    \
  status,                                                     \
  group_id,                                                   \
  scene_id)                                            \
  ZB_ZCL_SCENES_INIT_VIEW_SCENE_RES_COMMON(                   \
    buffer,                                                   \
    cmd_struct_ptr,                                           \
    seq_num,                                                  \
    status,                                                   \
    group_id,                                                 \
    scene_id,                                                 \
    ZB_ZCL_CMD_SCENES_VIEW_SCENE_RESPONSE)


#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED

/*! @brief Start filling Enhanced View scene response frame

    Fills the mandatory part of the View scene response payload.

    @param buffer to put packet to
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    command payload (fieldsets) should be placed.
    @param seq_num - ZCL sequence number
    @param status of the command execution. Appropriate values (defined in @ref
    zcl_status enumeration) are:
    @li ZB_ZCL_STATUS_SUCCESS for successful result
    @li ZB_ZCL_STATUS_INSUFF_SPACE if fieldsets don't fit into packet
    @li ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_INIT_ENHANCED_VIEW_SCENE_RES(           \
  buffer,                                                     \
  cmd_struct_ptr,                                             \
  seq_num,                                                    \
  status,                                                     \
  group_id,                                                   \
  scene_id)                                                   \
  ZB_ZCL_SCENES_INIT_VIEW_SCENE_RES_COMMON(                   \
    buffer,                                                   \
    cmd_struct_ptr,                                           \
    seq_num,                                                  \
    status,                                                   \
    group_id,                                                 \
    scene_id,                                                 \
    ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE_RESPONSE)

#endif /* !ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED */

/*! @brief Sends View scene response

    @param buffer with prepared command payload
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
*/
#define ZB_ZCL_SCENES_SEND_VIEW_SCENE_RES(        \
    buffer,                                       \
    cmd_struct_ptr,                               \
    dst_addr,                                     \
    dst_ep,                                       \
    ep,                                           \
    prof_id,                                      \
    callback)                                     \
  {                                               \
    ZB_ZCL_FINISH_PACKET(buffer, cmd_struct_ptr) \
    ZB_ZCL_SEND_COMMAND_SHORT(                    \
        buffer,                                   \
        dst_addr,                                 \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,         \
        dst_ep,                                   \
        ep,                                       \
        prof_id,                                  \
        ZB_ZCL_CLUSTER_ID_SCENES,                 \
        callback);                                \
  }

/** @brief Adds transition time into Scenes.ViewSceneResponse payload.
    @param cmd_struct_ptr - pointer to the place in buffer to put data to
    @param trans_time - transition time to put
*/
#define ZB_ZCL_SCENES_ADD_TRANSITION_TIME_VIEW_SCENE_RES(     \
    cmd_struct_ptr,                                           \
    trans_time)                                               \
    ZB_ZCL_PACKET_PUT_DATA16((cmd_struct_ptr), &(trans_time))

/** @brief Adds scene name into Scenes.ViewSceneResponse payload.

    Since the Stack does not support scene names, the define puts only zero scene name length byte
    to preserve payload format.
    @param cmd_struct_ptr - pointer to the place in buffer to put data to
    @param scene_name - scene name to put (currently ignored)
*/
#define ZB_ZCL_SCENES_ADD_SCENE_NAME_VIEW_SCENE_RES( \
    cmd_struct_ptr,                                  \
    scene_name)                                      \
    ZB_ZCL_PACKET_PUT_DATA8((cmd_struct_ptr), 0)

/* TODO Fix possible issue with buffer length */

/** @brief Parses common part of the Scenes.ViewSceneResponse
    @param buffer containing data
    @param response_ptr - pointer to the extracted data
*/
#define ZB_ZCL_SCENES_GET_VIEW_SCENE_RES_COMMON(buffer, response_ptr)                  \
  {                                                                                    \
    if (zb_buf_len((buffer)) <                                                         \
        (2 * sizeof(zb_uint8_t) + sizeof(zb_uint16_t)))                                \
    {                                                                                  \
      (response_ptr) = NULL;                                                           \
    }                                                                                  \
    else                                                                               \
    {                                                                                  \
      (response_ptr) =                                                                 \
          (zb_zcl_scenes_view_scene_res_fixed_size_t*)                                 \
          zb_buf_begin((buffer));                                                      \
      ZB_ZCL_HTOLE16_INPLACE(&((response_ptr)->group_id));                             \
      if ((response_ptr)->status == ZB_ZCL_STATUS_SUCCESS)                             \
      {                                                                                \
        ZB_ZCL_HTOLE16_INPLACE(&((response_ptr)->transition_time));                    \
        (void)zb_buf_cut_left((buffer), sizeof(zb_zcl_scenes_view_scene_res_fixed_size_t) + \
            (response_ptr)->scene_name[0]);                                            \
        (response_ptr)->scene_name[0] = 0;                                             \
      }                                                                                \
      else                                                                             \
      {                                                                                \
        (void)zb_buf_cut_left(                                                         \
            (buffer),                                                                  \
            (2 * sizeof(zb_uint8_t) + sizeof(zb_uint16_t)));                           \
      }                                                                                \
    }                                                                                  \
  }


/*! @brief Remove scene response payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_remove_scene_res_s
{
  zb_uint8_t status;    /** Command execution status */
  zb_uint16_t group_id; /** Scene group identifier */
  zb_uint8_t scene_id;  /** Scene identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_remove_scene_res_t;

/*! @brief Send Remove Scene response

    @param buffer to put packet to
    @param seq_num - ZCL sequence number
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
    @param status of the corresponding Add scene command execution. Appropriate
    values (defined in @ref zcl_status enumeration)are:
    @li @ref ZB_ZCL_STATUS_SUCCESS for successful result
    @li @ref ZB_ZCL_STATUS_NOT_FOUND if the scene is not present in the scene table
    @li @ref ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    Group Table
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_REMOVE_SCENE_RES(                  \
    buffer,                                                   \
    seq_num,                                                  \
    dst_addr,                                                 \
    dst_ep,                                                   \
    ep,                                                       \
    prof_id,                                                  \
    callback,                                                 \
    status,                                                   \
    group_id,                                                 \
    scene_id)                                                 \
  {                                                           \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);            \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr); \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                          \
        ptr,                                                  \
        seq_num,                                              \
        ZB_ZCL_CMD_SCENES_REMOVE_SCENE_RESPONSE);             \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (status));                   \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));            \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (scene_id));                 \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                        \
    ZB_ZCL_SEND_COMMAND_SHORT(                                \
        buffer,                                               \
        dst_addr,                                             \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                     \
        dst_ep,                                               \
        ep,                                                   \
        prof_id,                                              \
        ZB_ZCL_CLUSTER_ID_SCENES,                             \
        callback);                                            \
  }

/** @brief Parses Scenes.RemoveSceneResponse command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_remove_scene_res_s) or NULL pointer if payload
    size is too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_REMOVE_SCENE_RES(buffer, cmd_struct_ptr)       \
  {                                                                      \
    if (sizeof(zb_zcl_scenes_remove_scene_res_t) > zb_buf_len((buffer))) \
    {                                                                    \
      (cmd_struct_ptr) = NULL;                                           \
    }                                                                    \
    else                                                                 \
    {                                                                    \
      (cmd_struct_ptr) =                                                 \
        (zb_zcl_scenes_remove_scene_res_t*)zb_buf_begin(buffer);         \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);               \
    }                                                                    \
  }

/** @brief Remove all scenes response payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_remove_all_scenes_res_s
{
  zb_uint8_t status;    /** Command execution status */
  zb_uint16_t group_id; /** Group identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_remove_all_scenes_res_t;

/*! @brief Send Remove all scenes response

    @param buffer to put packet to
    @param seq_num - ZCL sequence number
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
    @param status of the corresponding Add scene command execution. Appropriate
    values (defined in @ref zcl_status enumeration)are:
    @li @ref ZB_ZCL_STATUS_SUCCESS for successful result
    @li @ref ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    Group Table
    @param group_id - group identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_REMOVE_ALL_SCENES_RES(             \
    buffer,                                                   \
    seq_num,                                                  \
    dst_addr,                                                 \
    dst_ep,                                                   \
    ep,                                                       \
    prof_id,                                                  \
    callback,                                                 \
    status,                                                   \
    group_id)                                                 \
  {                                                           \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);            \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr); \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                          \
        ptr,                                                  \
        seq_num,                                              \
        ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES_RESPONSE);        \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (status));                   \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));            \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                         \
    ZB_ZCL_SEND_COMMAND_SHORT(                                \
        buffer,                                               \
        dst_addr,                                             \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                     \
        dst_ep,                                               \
        ep,                                                   \
        prof_id,                                              \
        ZB_ZCL_CLUSTER_ID_SCENES,                             \
        callback);                                            \
  }

/** @brief Parses Scenes.RemoveAllScenesResponse command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_remove_all_scenes_res_s) or NULL pointer if payload
    size is too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_REMOVE_ALL_SCENES_RES(buffer, cmd_struct_ptr) \
  {                                                                     \
    if (  sizeof(zb_zcl_scenes_remove_all_scenes_res_t)                 \
        > zb_buf_len((buffer)))                                         \
    {                                                                   \
      (cmd_struct_ptr) = NULL;                                          \
    }                                                                   \
    else                                                                \
    {                                                                   \
      (cmd_struct_ptr) =                                                \
        (zb_zcl_scenes_remove_all_scenes_res_t*)zb_buf_begin(buffer);   \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);              \
    }                                                                   \
  }

/** @brief Store scene response payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_store_scene_res_s
{
  zb_uint8_t status;    /*!< Command execution status */
  zb_uint16_t group_id; /*!< Scene group identifier */
  zb_uint8_t scene_id;  /*!< Scene identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_store_scene_res_t;

/*! @brief Send Store scene response

    @param buffer to put packet to
    @param seq_num - ZCL sequence number
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
    @param status of the corresponding Add scene command execution. Appropriate
    values (defined in @ref zcl_status enumeration)are:
    @li @ref ZB_ZCL_STATUS_SUCCESS for successful result
    @li ZB_ZCL_STATUS_INSUFF_SPACE if fieldsets don't fit into packet
    @li @ref ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    Group Table
    @param group_id - group identifier for the scene to store
    @param scene_id - scene identifier for the scene to store
*/
#define ZB_ZCL_SCENES_SEND_STORE_SCENE_RES(                   \
    buffer,                                                   \
    seq_num,                                                  \
    dst_addr,                                                 \
    dst_ep,                                                   \
    ep,                                                       \
    prof_id,                                                  \
    callback,                                                 \
    status,                                                   \
    group_id,                                                 \
    scene_id)                                                 \
  {                                                           \
    zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);            \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr); \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                          \
        ptr,                                                  \
        seq_num,                                              \
        ZB_ZCL_CMD_SCENES_STORE_SCENE_RESPONSE);              \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (status));                   \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (group_id));            \
    ZB_ZCL_PACKET_PUT_DATA8(ptr, (scene_id));                 \
    ZB_ZCL_FINISH_PACKET(buffer, ptr)                        \
    ZB_ZCL_SEND_COMMAND_SHORT(                                \
        buffer,                                               \
        dst_addr,                                             \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,                     \
        dst_ep,                                               \
        ep,                                                   \
        prof_id,                                              \
        ZB_ZCL_CLUSTER_ID_SCENES,                             \
        callback);                                            \
  }

/** @brief Parses Scenes.StoreScene command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_store_scene_res_s) or NULL pointer if payload size
    is too small
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_STORE_SCENE_RES(buffer, cmd_struct_ptr)       \
  {                                                                     \
    if (sizeof(zb_zcl_scenes_store_scene_res_t) > zb_buf_len((buffer))) \
    {                                                                   \
      (cmd_struct_ptr) = NULL;                                          \
    }                                                                   \
    else                                                                \
    {                                                                   \
      (cmd_struct_ptr) =                                                \
        (zb_zcl_scenes_store_scene_res_t*)zb_buf_begin(buffer);         \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->group_id);              \
    }                                                                   \
  }

/** @brief Scene table capacity: at least one scene can be added

    Exact number is unknown
*/
#define ZB_ZCL_SCENES_CAPACITY_ONE_MORE ((zb_uint8_t)0xfe)

/** @brief Scene table capacity unknown */
#define ZB_ZCL_SCENES_CAPACITY_UNKNOWN ((zb_uint8_t)0xff)

/** @brief Get scene membership response fixed-structure payload part
   representation */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_get_scene_membership_res_mandatory_s
{
  /** @brief Command execution status

      Appropriate values are:
      @li ZB_ZCL_STATUS_SUCCESS - successfully executed
      @li ZB_ZCL_STATUS_INVALID_FIELD - group is not present in the Group Table
      see @ref zcl_status enumeration
  */
  zb_uint8_t status;
  /** @brief Scene table capacity

      Permitted values are:
      @li 0..0xfd direct scene table capacity
      @li @ref ZB_ZCL_SCENES_CAPACITY_ONE_MORE at least one more scene can be added
      @li @ref ZB_ZCL_SCENES_CAPACITY_UNKNOWN scene table capacity is unknown
  */
  zb_uint8_t capacity;
  zb_uint16_t group_id; /**< Group identifier */
} ZB_PACKED_STRUCT zb_zcl_scenes_get_scene_membership_res_mandatory_t;

/** @brief Get scene membership response optional payload part
   representation */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_get_scene_membership_res_optional_s
{
  /** @brief Number of scenes in the response.
      @note Valid if status has ZB_ZCL_STATUS_SUCCESS value only.
  */
  zb_uint8_t scene_count;
  /** @brief Array of scenes corresponding to the group identifier.

      Array contains strictly @ref scene_count elements.
      @note Valid if status has ZB_ZCL_STATUS_SUCCESS value only.
  */
  zb_uint8_t scene_list[1];
} ZB_PACKED_STRUCT zb_zcl_scenes_get_scene_membership_res_optional_t;

/** @brief Get scene membership response */
typedef ZB_PACKED_PRE struct zb_zcl_scenes_get_scene_membership_res_s
{
  /** @brief Mandatory payload part */
  zb_zcl_scenes_get_scene_membership_res_mandatory_t mandatory;
  /** @brief Optional payload part */
  zb_zcl_scenes_get_scene_membership_res_optional_t optional;
} ZB_PACKED_STRUCT zb_zcl_scenes_get_scene_membership_res_t;

/** @brief Start filling Get scene membership response frame

    Fills the mandatory part of the Get scene membership scene response payload.

    @param buffer to put packet to
    @param cmd_struct_ptr - pointer to the place variable-structure part of the
    command payload (fieldsets) should be placed.
    @param seq_num - ZCL sequence number
    @param cap_ptr - pointer to the capacity fields of the response
    @param status of the command execution. Appropriate values (defined in @ref
    zcl_status enumeration) are:
    @li ZB_ZCL_STATUS_SUCCESS for successful result
    @li ZB_ZCL_STATUS_INVALID_FIELD if the group is not present in the
    @param capacity of the scene table
    @param group_id - group identifier for the scene to store
*/
#define ZB_ZCL_SCENES_INIT_GET_SCENE_MEMBERSHIP_RES(          \
    buffer,                                                   \
    cmd_struct_ptr,                                           \
    seq_num,                                                  \
    cap_ptr,                                                  \
    status,                                                   \
    capacity,                                                 \
    group_id)                                                 \
  {                                                           \
    cmd_struct_ptr = ZB_ZCL_START_PACKET(buffer);             \
    ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(      \
        cmd_struct_ptr);                                      \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                          \
        cmd_struct_ptr,                                       \
        seq_num,                                              \
        ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP_RESPONSE);     \
    ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (status));        \
    (cap_ptr) = (cmd_struct_ptr);                             \
    ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (capacity));      \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_struct_ptr, (group_id)); \
  }

/** @brief Appends Scene count value to Get scene membership response
    constant-structure payload part
    @note Should not be used if command status is not ZB_ZCL_STATUS_SUCCESS
    @param cmd_struct_ptr - pointer to the memory area SceneCount value should be put to
    @param scene_count - number of scenes in the scene list
*/
#define ZB_ZCL_SCENES_ADD_SCENE_COUNT_GET_SCENE_MEMBERSHIP_RES( \
    cmd_struct_ptr,                                             \
    scene_count)                                                \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (scene_count))

/** @brief Appends a scene identifier to the Scene list in Get scene membership
    response payload
    @note Should be preceded with @ref
    ZB_ZCL_SCENES_ADD_SCENE_COUNT_GET_SCENE_MEMBERSHIP_RES macro invocation.
    @param cmd_struct_ptr - pointer to the memory area scene identifier should
    be put to
    @param scene_id - scene identifier
*/
#define ZB_ZCL_SCENES_ADD_SCENE_ID_GET_SCENE_MEMBERSHIP_RES( \
    cmd_struct_ptr,                                          \
    scene_id)                                                \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_struct_ptr, (scene_id))

/*! @brief Sends Get scene membership response

    @param buffer with prepared command payload
    @param cmd_struct_ptr - pointer to the place directly after the payload
    @param dst_addr - address to send packet to
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param callback for getting command send status
*/
#define ZB_ZCL_SCENES_SEND_GET_SCENE_MEMBERSHIP_RES( \
    buffer,                                          \
    cmd_struct_ptr,                                  \
    dst_addr,                                        \
    dst_ep,                                          \
    ep,                                              \
    prof_id,                                         \
    callback)                                        \
  {                                                  \
    ZB_ZCL_FINISH_PACKET(buffer, cmd_struct_ptr)    \
    ZB_ZCL_SEND_COMMAND_SHORT(                       \
        buffer,                                      \
        dst_addr,                                    \
        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,            \
        dst_ep,                                      \
        ep,                                          \
        prof_id,                                     \
        ZB_ZCL_CLUSTER_ID_SCENES,                    \
        callback);                                   \
  }

/** @brief Parses Scenes.RemoveAllScenesResponse command payload
    @param buffer to get data from
    @param cmd_struct_ptr - pointer to the request representation structure (of
    type @ref zb_zcl_scenes_get_scene_membership_res_s) or NULL pointer if
    payload size is too small to contain mandatory part
    @attention The macro changes content of the buffer
*/
#define ZB_ZCL_SCENES_GET_GET_SCENE_MEMBERSHIP_RES(buffer, cmd_struct_ptr) \
  {                                                                        \
    if (zb_buf_len((buffer)) <                                             \
        sizeof(zb_zcl_scenes_get_scene_membership_res_t))                  \
    {                                                                      \
      (cmd_struct_ptr) = NULL;                                             \
    }                                                                      \
    else                                                                   \
    {                                                                      \
      (cmd_struct_ptr) =                                                   \
        (zb_zcl_scenes_get_scene_membership_res_t*)zb_buf_begin((buffer)); \
      ZB_ZCL_HTOLE16_INPLACE(&(cmd_struct_ptr)->mandatory.group_id);       \
    }                                                                      \
  }

/** @cond internals_doc */

zb_uint8_t zb_zcl_scenes_process_store_scene(zb_uint8_t param, zb_zcl_scenes_store_scene_req_t* req, const zb_zcl_parsed_hdr_t *cmd_info);

zb_uint8_t zb_zcl_scenes_process_recall_scene(zb_uint8_t param, zb_zcl_scenes_recall_scene_req_t* req, const zb_zcl_parsed_hdr_t *cmd_info);

zb_uint16_t zb_zcl_scenes_process_remove_all_scenes(zb_uint8_t param, zb_zcl_scenes_remove_all_scenes_req_t* req, const zb_zcl_parsed_hdr_t *cmd_info);

/** @endcond */

/*! @} */ /* Scenes cluster commands */

/*! @} */ /* ZCL Scenes cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_scenes_init_server(void);
void zb_zcl_scenes_init_client(void);
#define ZB_ZCL_CLUSTER_ID_SCENES_SERVER_ROLE_INIT zb_zcl_scenes_init_server
#define ZB_ZCL_CLUSTER_ID_SCENES_CLIENT_ROLE_INIT zb_zcl_scenes_init_client

#endif /* ZB_ZCL_SCENES_H */
