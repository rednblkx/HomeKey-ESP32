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
/* PURPOSE: common definitions for ZGP profile
*/

#ifndef ZBOSS_API_ZGP_H
#define ZBOSS_API_ZGP_H 1

#include "zb_config.h"
#include "zb_address.h"
#include "zboss_api_buf.h"
#include "zb_types.h"

/********************************************************************/
/********* Type declarations specific to ZGP profile ****************/
/********************************************************************/

/**
   @addtogroup zgp_common
   @{
*/

#define ZGP_ENDPOINT 242

/** @brief ZGPD application ID */
typedef enum zb_zgp_app_id_e
{
  ZB_ZGP_APP_ID_0000    = 0x00, /**< ApplicationID value 0b000 - usage of the SrcID */
  ZB_ZGP_APP_ID_0001    = 0x01, /**< ApplicationID value 0b001 - LPED */
  ZB_ZGP_APP_ID_0010    = 0x02, /**< ApplicationID value 0b010 - usage of the GPD IEEE address.*/
  ZB_ZGP_APP_ID_INVALID = 0x07, /**< Invalid ApplicationID */
}
zb_zgp_app_id_t;

/**
 * @brief ZGPD address
 *
 * ZGPD is identified in network whether by SrcId or its IEEE address. */
typedef ZB_PACKED_PRE union zb_zgpd_addr_u
{
  zb_uint32_t     src_id;    /**< ZGPD SrcId @see ZGP spec, A.1.4.1.4 */
  zb_ieee_addr_t  ieee_addr; /**< ZGPD IEEE address */
} ZB_PACKED_STRUCT
zb_zgpd_addr_t;

enum zb_zgp_endpoint_e
{
  ZB_ZGP_COMMUNICATION_ENDPOINT,
  ZB_ZGP_ALL_ENDPOINTS = 255
};

/**
 * @brief ZGPD identification info (ZGPD ID)
 *
 * ZGPD is identified by SrcId if ApplicationID is @ref ZB_ZGP_APP_ID_0000.
 * Otherwise, ZGPD is identified by its IEEE address. */
typedef ZB_PACKED_PRE struct zb_zgpd_id_s
{
  zb_uint8_t         app_id;  /**< One of the @ref zb_zgp_app_id_t values */
  zb_uint8_t         endpoint;/**< Identifier of the GPD endpoint, which jointly with the GPD IEEE address identifies a unique logical GPD device.*/
  zb_zgpd_addr_t     addr;    /**< ZGPD SrcId or IEEE address */
} ZB_PACKED_STRUCT
zb_zgpd_id_t;

#define ZGP_ATTR_OPT_GET_REMAINING_LEN(opt) ((opt) & 0xFU)
#define ZGP_ATTR_OPT_GET_REPORTED(opt)      (((opt) >> 4U) & 0x1U)
#define ZGP_ATTR_OPT_GET_VAL_PRESENT(opt)   (((opt) >> 5U) & 0x1U)

#define ZB_APP_DESCR_ATTR_VAL_SIZE 8U
typedef struct zgp_attr_record_s
{
  zb_uint16_t id;
  zb_uint8_t  data_type;

  /*
  zb_bitfield_t remaining_len:4;

    7344 The Reported sub-field is a Boolean flag which indicates if the attribute as identified by the AttributeID
    7345 field is reported by the GPD in operation, or if it is background data required for processing of a report-
    7346 ed attribute only conveyed once at commissioning time.
    7350 If Reported = 0b1, Attribute Offset within Report field is present, otherwise it is absent

  zb_bitfield_t reported:1;
  zb_bitfield_t attr_val_present:1;
  zb_bitfield_t reserved:2;
   */
  zb_uint8_t  options;

  /*
  7358 The Attribute Offset within Report field, when present, carries the start position (in bytes) of the data
  7359 point identified by the AttributeID of the ClusterID in the report payload. The Attribute Offset within
  7360 Report = 0x00 corresponds to the octet immediately following the Report identifier field in the pay-
  7361 load of the GPD Compact Attribute Reporting command.
  */
  zb_uint8_t offset;

  /*
  7362 The Attribute value field, when present, carries the actual fixed value of that attribute; *the length and
  7363 type of this field are determined by the AttributeID of the ClusterID (in case of manufacturer-specific
  7364 attributes or clusters, corresponding to the ManufacturerID).
  */
  zb_uint8_t value[ZB_APP_DESCR_ATTR_VAL_SIZE];
}zgp_attr_record_t;

typedef struct zgp_data_point_desc_options_s
{
  zb_bitfield_t attr_records_num:3;
  zb_bitfield_t cluster_type:1; /* server == 1, client == 0 */
  zb_bitfield_t manuf_id_present:1;
  zb_bitfield_t reserved:3;
}zgp_data_point_desc_options_t;

#define ZB_APP_DESCR_ATTR_REC_SIZE 4U
typedef struct zgp_data_point_desc_s
{
  zgp_data_point_desc_options_t options;
  zb_uint16_t cluster_id;
  zb_uint16_t manuf_id;
  zgp_attr_record_t attr_records_data[ZB_APP_DESCR_ATTR_REC_SIZE]; /* contains array of not parsed zgp_attr_record_t */
}zgp_data_point_desc_t;

typedef ZB_PACKED_PRE struct zgp_report_desc_options_s
{
  zb_bitfield_t timeout_present:1;
  zb_bitfield_t reserved:7;
} ZB_PACKED_STRUCT zgp_report_desc_options_t;

typedef ZB_PACKED_PRE struct zgp_report_desc_s
{
  zgp_report_desc_options_t  options;
  zb_uint16_t timeout;
  zb_uint8_t  point_descs_data_len;
  zb_uint8_t  point_descs_data[ZB_ZGP_APP_DESCR_REPORT_DATA_SIZE]; /* contains array of not parsed zgp_data_point_desc_t */
}ZB_PACKED_STRUCT zgp_report_desc_t;

/* 11/23/2022 [VK]
 * It is required to parse GP Pairing Configuration with
 * Application Description correctly and should be used in RAM ONLY.
 *
 * We should pass a report ID to zgp_sink_handle_gp_pairing_configuration()
 * after zgp_parse_gp_pairing_configuration_app_descr() to put report descriptors
 * to a ZGPD entry by it.
 *
 * Default struct zgp_report_desc_t is used to store in NVRAM and should NOT contain report_id.
 */
typedef ZB_PACKED_PRE struct zgp_report_desc_pairing_config_s
{
  zb_uint8_t  report_id;
  zgp_report_desc_options_t  options;
  zb_uint16_t timeout;
  zb_uint8_t  point_descs_data_len;
  zb_uint8_t  point_descs_data[ZB_ZGP_APP_DESCR_REPORT_DATA_SIZE]; /* contains array of not parsed zgp_data_point_desc_t */
} ZB_PACKED_STRUCT zgp_report_desc_pairing_config_t;

#define SIZE_BY_APP_ID(app_id) (((app_id) == ZB_ZGP_APP_ID_0000) ? 4U : 8U)
#define ZGPD_ID_SIZE(zgpd_id) SIZE_BY_APP_ID((zgpd_id)->app_id)

#define ZB_ZGPD_IDS_ARE_EQUAL(id1, id2) \
  (((id1)->app_id == (id2)->app_id) && \
   (((id1)->app_id == ZB_ZGP_APP_ID_0000) ? \
   ((id1)->addr.src_id == (id2)->addr.src_id) : \
    (!ZB_MEMCMP(&(id1)->addr.ieee_addr, &(id2)->addr.ieee_addr, sizeof(zb_ieee_addr_t)) &&\
     ((id1)->endpoint == (id2)->endpoint))))

/* From ZGP spec, A. 1.4.1.4:
 * ZGPD ID value of 0x00000000 indicates unspecified.
 * ... 0xffffffff indicates all.
 */
#define ZB_ZGP_SRC_ID_UNSPECIFIED 0x00000000U
#define ZB_ZGP_SRC_ID_ALL         (zb_uint32_t)(~0u)

#define ZB_INIT_ZGPD_ID(zgpd_id) \
{ \
  (zgpd_id)->app_id = ZB_ZGP_APP_ID_0000; \
  (zgpd_id)->addr.src_id = ZB_ZGP_SRC_ID_UNSPECIFIED; \
}

#define ZB_ZGPD_IS_SPECIFIED(zgpd_id) \
  (((zgpd_id)->app_id != ZB_ZGP_APP_ID_0000) \
  || ((zgpd_id)->addr.src_id != ZB_ZGP_SRC_ID_UNSPECIFIED))

#define ZB_MAKE_ZGPD_ID(zgpd_id, s_app_id, s_endpoint, s_addr)\
  {\
    (zgpd_id).app_id = (s_app_id);\
    (zgpd_id).endpoint = (s_endpoint);\
    (zgpd_id).addr = (s_addr);\
  }

/**
 * @brief ZGP security level type
 * @see ZGP spec, A.1.5.3.2 */
enum zb_zgp_security_level_e
{
  ZB_ZGP_SEC_LEVEL_NO_SECURITY           = 0x00,  /**< No security */
  ZB_ZGP_SEC_LEVEL_REDUCED               = 0x01,  /**< 1LSB of frame counter and short (2B) MIC */
  ZB_ZGP_SEC_LEVEL_FULL_NO_ENC           = 0x02,  /**< Full (4B) frame counter and full (4B) MIC */
  ZB_ZGP_SEC_LEVEL_FULL_WITH_ENC         = 0x03,  /**< Encryption & full (4B) frame counter and
                                                       full (4B) MIC */
};

/**
 * @brief ZGP gpsSecurityLevel attribute
 * @see ZGP spec, A.3.3.2.6 */
enum zb_zgp_security_level_protection_with_gp_link_key_e
{
  ZB_ZGP_SEC_LEVEL_PROTECTION_WITHOUT_GP_LINK_KEY = 0x00,  /**< Do not used GP link key */
  ZB_ZGP_SEC_LEVEL_PROTECTION_WITH_GP_LINK_KEY    = 0x01,  /**< Use GP link key */
};

/**
 * @brief ZGP gpsSecurityLevel attribute
 * @see ZGP spec, A.3.3.2.6 */
enum zb_zgp_security_level_involve_tc_e
{
  ZB_ZGP_SEC_LEVEL_PROTECTION_DO_NOT_INVOLVE_TC = 0x00,  /**< Do not involve TC */
  ZB_ZGP_SEC_LEVEL_PROTECTION_INVOLVE_TC        = 0x01,  /**< Involve TC */
};

/**
The gpSharedSecurityKeyType attribute can take the following values from Table 48:
Table 48 - Values of gpSecurityKeyType

See also Table 12 - Mapping between the gpSecurityKeyType and the SecurityKey
sub-field of the Extended NWK Frame Control field
*/
enum zb_zgp_security_key_type_e
{
  ZB_ZGP_SEC_KEY_TYPE_NO_KEY             = 0x00,  /**< No key */
  ZB_ZGP_SEC_KEY_TYPE_NWK                = 0x01,  /**< Zigbee NWK key */
  ZB_ZGP_SEC_KEY_TYPE_GROUP              = 0x02,  /**< ZGPD group key */
  ZB_ZGP_SEC_KEY_TYPE_GROUP_NWK_DERIVED  = 0x03,  /**< NWK-key derived ZGPD group key */
  ZB_ZGP_SEC_KEY_TYPE_ZGPD_INDIVIDUAL    = 0x04,  /**< (Individual) out-of-the-box ZGPD key */
  ZB_ZGP_SEC_KEY_TYPE_DERIVED_INDIVIDUAL = 0x07,  /**< Derived individual ZGPD key */
};

/********************************************************************/
/******** ZGP Device Type declarations and configuration ************/
/********************************************************************/

/**
 * @brief Possible ZGPD device identifiers
 * @see ZGP spec, A.4.3 */
typedef enum zb_zgpd_dev_id_e
{
  ZB_ZGP_SIMPLE_GEN_1_STATE_SWITCH_DEV_ID   = 0x00, /**< Simple Generic 1-state ZGP switch */
  ZB_ZGP_SIMPLE_GEN_2_STATE_SWITCH_DEV_ID   = 0x01, /**< Simple Generic 2-state ZGP switch */
  ZB_ZGP_ON_OFF_SWITCH_DEV_ID               = 0x02, /**< ZGP On/Off switch */
  ZB_ZGP_LEVEL_CONTROL_SWITCH_DEV_ID        = 0x03, /**< ZGP Level Control Switch */
#define ZB_ZGP_LVL_CTRL_SWITCH_DEV_ID ZB_ZGP_LEVEL_CONTROL_SWITCH_DEV_ID
  ZB_ZGP_SIMPLE_SENSOR_DEV_ID               = 0x04, /**< ZGP Simple Sensor */
  ZB_ZGP_ADVANCED_GEN_1_STATE_SWITCH_DEV_ID = 0x05, /**< Advanced Generic 1-state ZGP switch */
  ZB_ZGP_ADVANCED_GEN_2_STATE_SWITCH_DEV_ID = 0x06, /**< Advanced Generic 2-state ZGP switch */
  ZB_ZGP_GEN_8_CONT_SWITCH_DEV_ID           = 0x07, /**< Generic 8-contact ZGP switch */
  ZB_ZGP_COLOR_DIMMER_SWITCH_DEV_ID         = 0x10, /**< Color Dimmer ZGP Switch */
  ZB_ZGP_LIGHT_SENSOR_DEV_ID                = 0x11, /**< ZGP Light Sensor */
  ZB_ZGP_OCCUPANCY_SENSOR_DEV_ID            = 0x12, /**< ZGP Occupancy Sensor */
  ZB_ZGP_DOOR_LOCK_CONTROLLER_DEV_ID        = 0x20, /**< ZGP Door Lock Controller */
  ZB_ZGP_TEMPERATURE_SENSOR_DEV_ID          = 0x30, /**< ZGP temperature sensor */
  ZB_ZGP_PRESSURE_SENSOR_DEV_ID             = 0x31, /**< ZGP Pressure Sensor */
  ZB_ZGP_FLOW_SENSOR_DEV_ID                 = 0x32, /**< ZGP Flow sensor */
  ZB_ZGP_ENVIRONMENT_SENSOR_DEV_ID          = 0x33, /**< ZGP Temperature + Humidity sensor */
  ZB_ZGP_MANUF_SPECIFIC_DEV_ID              = 0xfe, /**< Manufactures-specific; 2
                                                     * more fields in the
                                                     * Commissioning frame. See 4.1
                                                     * Manufacturer Specific Green
                                                     * Power Device Type Use case
                                                     * in Green Power Device
                                                     * Manufacturer Specific Device & Command
                                                     * Definition Proposal
                                                     */
  ZB_ZGP_UNDEFINED_DEV_ID                   = 0xff, /**< Undefined device type */
}
zb_zgpd_dev_id_t;

/**
 * @brief Manufacturer-specific device identifiers for GreanPeak
 * @see ZGP spec, A.4.3 */
typedef enum zb_zgpd_manuf_specific_dev_id_e
{
  /* TODO: ZB_ZGP_MS_DOOR_SENSOR_DEV_ID is deprecated. Remove it when there will be
   * no testing devices using this type. GreanPeak door sensors use
   * ZB_ZGP_MS_DOOR_WINDOW_SENSOR_DEV_ID now. */
  ZB_ZGP_MS_DOOR_SENSOR_DEV_ID            = 0x00, /**< IAS Zone - Door Sensor*/
  ZB_ZGP_MS_DOOR_WINDOW_SENSOR_DEV_ID     = 0x01, /**< IAS Zone - Door/Window Sensor*/
  ZB_ZGP_MS_LEAKAGE_SENSOR_DEV_ID         = 0x02, /**< IAS Zone - Leakage Sensor */
  ZB_ZGP_MS_HUMIDITY_SENSOR_DEV_ID        = 0x03, /**< Relative Humidity Sensor */
  ZB_ZGP_MS_MOTION_SENSOR_DEV_ID          = 0x05, /**< IAS Zone - Motion sensor */
  ZB_ZGP_MS_MOVEMENT_SENSOR_DEV_ID        = 0x07, /**< IAS Zone - Movement sensor */
  ZB_ZGP_MS_SMART_PLUG_DEV_ID             = 0X08, /**< GreenPeak Greenpower smart plug */
  ZB_ZGP_MS_KEY_FOB_DEV_ID                = 0x0A, /**< GreenPeak Greenpower IAS Zone - Key fob */
  ZB_ZGP_MS_SMOKE_DETECTOR_DEV_ID         = 0x0b, /**< IAS Zone - Smoke Detector */
  ZB_ZGP_MS_BED_SENSOR_DEV_ID             = 0x0c, /**< IAS Zone - Bed Sensor */
  ZB_ZGP_MS_PARKING_SENSOR_DEV_ID         = 0xc0, /**< Occupancy - Parking sensor */
}
zb_zgpd_manuf_specific_dev_id_t;

/**
   @}
*/

/********************************************************************/
/*********************** Sink definitions ***************************/
/********************************************************************/
#if defined ZB_ENABLE_ZGP_SINK || defined DOXYGEN
/**
   @addtogroup zgp_sink
   @{
*/
/**
 * @brief Mapping of ZGPD command ID to Zigbee ZCL command ID
 */
typedef struct zgp_to_zb_cmd_mapping_s
{
  zb_uint8_t zgp_cmd_id;  /**< ZGPD command ID */
  zb_uint8_t zb_cmd_id;   /**< Zigbee ZCL command ID */
}
zgp_to_zb_cmd_mapping_t;

#ifdef ZB_ZGP_SINK_SUPPORT_LEGACY_MATCH_INFO
/**
 * @deprecated
 * Match info is deprecated in favor of @ref zb_zgps_mapping_entry_t. Matching information functionality will be removed in August 2023.
 */
typedef struct zgps_cluster_rec_s
{
  zb_uint16_t cluster_id;   /** Cluster ID to which specified ZGPD commands are translated
                                (see @ref zcl_cluster_id) */
/**
 * Options field of cluster table entry
 *
 * [0-1]        role mask          client/server/both.
 *                                 Note: this role specifies the original cluster role, not
 *                                 the cluster role to which this command will be addressed.
 *                                 E.g. for On/Off/Toggle commands it should be client role
 *                                 (these command send from client to server).
 *
 */
  zb_uint8_t  options;
  zb_uint8_t  cmd_ids[ZB_ZGP_MATCH_TBL_MAX_CMDS_FOR_MATCH]; /**< Supported commands by sink */
}
zgps_dev_cluster_rec_t;

/** @cond DOXYGEN_INTERNAL_DOC */
#define GET_CLUSTER_ROLE(cluster) \
  (cluster->options & 0x03U)

/**
 * @deprecated
 * Match info is deprecated in favor of @ref zb_zgps_mapping_entry_t. Matching information functionality will be removed in August 2023.
 */
typedef ZB_PACKED_PRE union zgps_device_id_u
{
  /* dev_id.zgpd_dev_id matches with ZGPD Device ID from Commissioning frame @see zb_zgpd_dev_id_t */
  zb_uint8_t  zgpd_dev_id;
  /* match with app_info.manuf_model_id. */
  zb_uint16_t   zgpd_manuf_model;
}
zgps_device_id_t;

/*
 * @deprecated
 * Match info is deprecated in favor of @ref zb_zgps_mapping_entry_t. Matching information functionality will be removed in August 2023.
 */
typedef ZB_PACKED_PRE struct zgps_dev_match_rec_s
{
  /* Cluster idxes possible for that device id. Not used slots must be filled by ZB_ZCL_CLUSTER_IDX_UNDEFINED.
     Cluster idx - index in zgps_dev_cluster_rec_t clusters_tbl[].
   */
  zb_uint16_t           clusters[ZB_ZGP_TBL_MAX_CLUSTERS];
  zb_uint16_t           manuf_id;
  /* match with device info from Commissioning frame:
     if zgpd_dev_id != ZB_ZGP_MANUF_SPECIFIC_DEV_ID, match by GPD Device id
     if zgpd_dev_id == ZB_ZGP_MANUF_SPECIFIC_DEV_ID, match by app_info.manuf_model_id
  */
  zgps_device_id_t      dev_id;
}
ZB_PACKED_STRUCT zgps_dev_match_rec_t;

#define IS_STANDART_ZGPS_DEVICE(dev_match_rec) \
  (dev_match_rec->manuf_id == ZB_ZGPD_MANUF_ID_UNSPEC)
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */


/*
  Using of match table.

  Match table is a static const data declared in the application.

  During GPD commissioning, using information from Commissioning frame, ZBOSS seeks for matched entry in match_tbl.
  Match is done by device id or model id - see calls to zb_zgps_get_dev_matching_tbl_index()
  Entry index is written into the Sink table.

  Command translation (without details about attr reporting):
  - get Sink table entry by GPD address
  - get matxh_tbl entry by index in Sink table.
  - scan entire match_tbl[]: use match_tbl[i].clusters as an index in clusters_tbl.
  - in each clusters_tbl entry scan clusters_tbl[i].cmd_ids[] for matching GPD command
  - seek for appropriate clister in out local Simple desc (decide which EP to map to)
  - map GPD command to ZCL command by scanning cmd_mapping[]
 */

/**
 * @brief Necessary information for filling translation table for any ZGPD
 *        during commissioning
 *
 * Includes matching table and command ID mappings. Given some ZGPD device ID it is
 * possible to fill translation table entry with help of this structure.
 *
 * @deprecated
 * Match info is deprecated in favor of @ref zb_zgps_mapping_entry_t. Matching information functionality will be removed in August 2023.
 */
typedef struct zb_zgps_match_info_s
{
  const zb_uint8_t                       match_tbl_size;
  /* clusters list to be matched by device id or manufacturer id got from Commissioning frame. */
  const ZB_CODE zgps_dev_match_rec_t    *match_tbl;
  const zb_uint8_t                       cmd_mappings_count;
  const ZB_CODE zgp_to_zb_cmd_mapping_t *cmd_mapping;
  const zb_uint8_t                       clusters_tbl_size;
  /* clusters table used to translate ZB_GPDF_CMD_ATTR_REPORT / ZB_GPDF_CMD_MANUF_SPEC_ATTR_REPORT */
  const ZB_CODE zgps_dev_cluster_rec_t  *clusters_tbl;
}
zb_zgps_match_info_t;
#endif  /* ZB_ZGP_SINK_SUPPORT_LEGACY_MATCH_INFO */

/*
  Using of Mapping Table.

  Mapping table functionality is ZBOSS custom implementation of Translation table, described in specification.
  Currently ZBOSS handles GPD commands mapping, which MAY be placed in mapping table, provided by application.  

  Mapping Table is a specific mapping rules declared in the application.
  The specific mapping supersedes the generic one.

  The application MAY or MAY not to declare Mapping Table.
  If application declares Mapping Table it SHOULD persist this data by its own.

  Mapping Table entries SHALL be added upon successful completion of proximity and multi-hop commissioning,
  and upon reception of GP Pairing Configuration leading to Sink Table entry creation (as described in A.3.5.2.5);
  those entries SHALL then contain the ApplicationID and GPD ID type and value of the GPD ID (and GPD Endpoint,
  matching or 0x00 or 0xff, if ApplicationID = 0b010) for which they are created;
  mapping the GPD commands to their ZCL equivalents.

  Supported features:

  - There SHOULD be only one entry in the GPD Command Mapping Table for each
    (GPD ID, GPD Endpoint, GPD Command, EndPoint, Zigbee Profile, Zigbee Cluster) tuple.
  - For a single GPD ID (and GPD Endpoint, if ApplicationID = 0b010), there MAY be multiple
    entries, e.g. for multiple GPD commands.
  - For a single GPD ID (and GPD Endpoint, if ApplicationID = 0b010),
    the same GPD Command could result in different mapped Zigbee CommandIDs,
    for different EndPoint, Profile and Cluster values.
  - For a single GPD ID, if ApplicationID = 0b010, there MAY be multiple entries,
    for multiple GPD Endpoints, even for identical GPD commands.

  GPD Command:
  - If the GPD Command field is set to 0xAF, all of the following GPD sensor report commands: 0xA0 - 0xA3 are supported.
  - If the GPD Command field is set to 0xFF, it indicates all GPD commands.

  Zigbee Endpoint:
  - If the EndPoint field is set to 0xfd, there are no paired endpoints.
  - If the EndPoint field is set to 0xff, all matching endpoints are paired.
  - If the EndPoint field is set to 0xfc, the raw GPD command is passed up to the application,
    and no mapping is performed in the GPEP.

  Zigbee Cluster:
  - If the Zigbee Cluster field is set to 0xffff, the ClusterID from the triggering GPD command is to be used.
  - If the Zigbee Cluster field is set to value other than 0xffff, then for GPD command carrying
    a ClusterID field (as e.g. for the GPD commands 0xA0 - 0xA3), the two ClusterID values SHALL exactly match.

  ZCL Payload Length:
  - If the Length sub-field of the Zigbee Command payload field is set to 0x00,
    the Payload sub-field is not present, and the Zigbee command is sent without payload.
  - If the Length sub-field of the Zigbee Command payload field is set to 0xff,
    the Payload sub-field is not present, and the payload from the triggering GPD command
    is to be copied verbatim into the Zigbee command.
  - If the Length sub-field of the Zigbee Command payload field is set to 0xfe,
    the Payload sub-field is not present, and the payload from the triggering GPD command needs to be parsed.
  - For all other values of the Length sub-field, the Payload sub-field is present,
    has a length as defined in the Length sub-field and specifies the payload to be used.

  Not supported features:
  - The Additional information block field is not supported yet.
 */

#define ZB_ZGP_ZCL_CMD_ID_UNDEFINED (0xff)
#define ZB_ZGP_ZCL_CLUSTER_ID_UNDEFINED (0xffff)

#define ZB_GPDF_CMD_UNDEFINED (0xff)

#define ZB_ZGP_MAPPING_ENTRY_ENDPOINT_PASS_RAW_GPDF_TO_APP (0xfc)
#define ZB_ZGP_MAPPING_ENTRY_ENDPOINT_NO_PAIRS             (0xfd)
#define ZB_ZGP_MAPPING_ENTRY_ENDPOINT_ALL_ARE_MATCHED      (0xff)

#define ZB_ZGP_MAPPING_ENTRY_NO_PAYLOAD     (0x00)
#define ZB_ZGP_MAPPING_ENTRY_GPDF_PAYLOAD   (0xff)
#define ZB_ZGP_MAPPING_ENTRY_PARSED_PAYLOAD (0xfe)

#define ZB_ZGP_MAPPING_ENTRY_OPTIONS(app_id, add_info_present) \
  (((((add_info_present) & 0x01) << 3)) | ((app_id) & 0x07))

/* 
 * A.3.6.2.2 GPD application functionality translation
 * Figure 80 – Format of the Options field of the GPD Command Translation Table entry
 */
#define ZB_ZGP_MAPPING_ENTRY_OPTIONS_IS_ADDITIONAL_INFO_PRESENT(options) \
  (((options) >> 3) & 0x01)

/**
 * @brief Mapping table entry additional info option selector structure
 * Figure 84 – Format of the Option selector field of the Option record field of the Translation Table entry
 */
typedef ZB_PACKED_PRE struct zb_zgps_mapping_entry_additional_info_option_selector_s
{
  zb_bitfield_t option_length:4;
  zb_bitfield_t option_id:4;
} ZB_PACKED_STRUCT zb_zgps_mapping_entry_additional_info_option_selector_t;

/**
 * @brief A.3.6.2.2.1.1 OptionIDs for GPD 8-bit vector commands
 */
enum zb_zgps_mapping_entry_8bit_vector_option_ids_e
{
  ZB_ZGPS_MAPPING_ENTRY_8BIT_VECTOR_OPTION_ID_GENERIC_SWITCH_COMMAND_EXECUTION = 0U
};

/**
 * @brief A.3.6.2.2.1.2 OptionIDs for GPD Compact Attribute Reporting
 */
enum zb_zgps_mapping_entry_compact_attr_report_option_ids_e
{
  ZB_ZGPS_MAPPING_ENTRY_COMPACT_ATTR_REPORT_REPORTABLE = 0U
};

/**
 * @brief Mapping table entry additional info generic switch option data structure
 * Figure 85 – Format of the Option data of the Generic switch command execution option of the Translation Table entry
 */
typedef ZB_PACKED_PRE struct zb_zgps_mapping_entry_generic_switch_option_data_s
{
  zb_uint8_t contact_status;
  zb_uint8_t contact_bitmask;
} ZB_PACKED_STRUCT zb_zgps_mapping_entry_generic_switch_option_data_t;

/**
 * @brief Mapping table entry additional info generic switch option structure
 */
typedef ZB_PACKED_PRE struct zb_zgps_mapping_entry_generic_switch_option_s
{
  zb_zgps_mapping_entry_additional_info_option_selector_t selector;
  zb_zgps_mapping_entry_generic_switch_option_data_t      data;
} ZB_PACKED_STRUCT zb_zgps_mapping_entry_generic_switch_option_t;

/**
 * @brief Mapping table entry additional info option record structure
 * A.3.6.2.2 GPD application functionality translation
 * Figure 83 – Format of the Option record field of the Translation Table entry
 */
typedef ZB_PACKED_PRE struct zb_zgps_mapping_entry_additional_info_option_record_s
{
  zb_zgps_mapping_entry_additional_info_option_selector_t selector;
} ZB_PACKED_STRUCT zb_zgps_mapping_entry_additional_info_option_record_t;

/**
 * @brief Mapping table entry additional info structure
 * Figure 82 – Format of the Additional information block field of the Translation Table entry
 */
typedef ZB_PACKED_PRE struct zb_zgps_mapping_entry_additional_info_s
{
  zb_uint8_t length;
} ZB_PACKED_STRUCT zb_zgps_mapping_entry_additional_info_t;

/**
 * @brief Mapping table entry structure
 */
typedef ZB_PACKED_PRE struct zb_zgps_mapping_entry_s
{
  zb_uint8_t  options;   /**< Options related to this table entry */
  zb_zgpd_addr_t gpd_id; /**< Identifier of the GPD */
  zb_uint8_t  gpd_endpoint;
  zb_uint8_t  gpd_command; /**< The GPD command to be mapped */
  zb_uint8_t  endpoint;  /**< The EndPoint for which the mapping is valid */
  zb_uint16_t profile;   /**< The Profile of the command after mapping */
  zb_uint16_t cluster;   /**< The cluster of the Profile on the endpoint */
  zb_uint8_t  zcl_command; /**< The Command ID of the Cluster into which GP Command is mapped */
  zb_uint8_t  zcl_payload_length;  /**< The payload length for the Zigbee Command, payload should be located in memory after this structure */
} ZB_PACKED_STRUCT zb_zgps_mapping_entry_t;

#define ZB_ZGP_GET_MAPPING_ENTRY_ZCL_PAYLOAD_PTR(entry) (&entry->zcl_payload_length+sizeof(entry->zcl_payload_length))

/** @cond DOXYGEN_INTERNAL_DOC */
void zb_zgps_set_mapping_table(const zb_zgps_mapping_entry_t ** table, zb_uint16_t * table_size);
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/**
 * @ingroup zgp_sink
 * @brief Set mapping table information that is used to fill ZGP command - ZCL
 * cluster mapping table.
 * @param [in]  table_ptr  Mapping information, pointer to array of type @ref zb_zgps_mapping_entry_t
 * @param [in]  table_size_ptr  pointer to size of the table - max entry number what can be iterated
 */
#define ZB_ZGP_SET_MAPPING_TABLE(table_ptr, table_size_ptr)     \
{ \
  zb_zgps_set_mapping_table((table_ptr), (table_size_ptr));   \
}
/** @} */ /* zgp_sink */
#endif  /* ZB_ENABLE_ZGP_SINK */

#ifdef ZB_ENABLE_ZGP_DIRECT
enum zb_zgp_data_handle_e
{
  ZB_ZGP_HANDLE_DEFAULT_HANDLE,
  ZB_ZGP_HANDLE_REMOVE_CHANNEL_CONFIG,
  ZB_ZGP_HANDLE_REMOVE_COMMISSIONING_REPLY,
  ZB_ZGP_HANDLE_ADD_COMMISSIONING_REPLY,
  ZB_ZGP_HANDLE_ADD_CHANNEL_CONFIG,
  ZB_ZGP_HANDLE_REMOVE_AFTER_FAILED_COMM,
  ZB_ZGP_HANDLE_REMOVE_BY_USER_REQ,

/**
 * The first handle that can be used by application for gp-data.req primitives.
 * Application may use any greater or equal handle value to match request with
 * confirmation.
 *
 * @see zb_zgps_send_data().
 */
  ZB_ZGP_HANDLE_APP_DATA,
};

#define ZB_GP_TX_QUEUE_ENTRY_LIFETIME_NONE ZB_MIN_TIME_VAL
#define ZB_GP_TX_QUEUE_ENTRY_LIFETIME_INF  ZB_MAX_TIME_VAL

#define ZB_GP_DATA_REQ_USE_GP_TX_QUEUE   0x01U
#define ZB_GP_DATA_REQ_USE_CSMA_CA_BIT   0x02U
#define ZB_GP_DATA_REQ_USE_MAC_ACK_BIT   0x04U
#define ZB_GP_DATA_REQ_MAINT_FRAME_TYPE  0x08U

#define ZB_GP_DATA_REQ_FRAME_TYPE(tx_opt) \
  ((tx_opt >> 3) & 0x03)

#define ZB_GP_DATA_REQ_ACTION_REMOVE_GPDF ZB_FALSE
#define ZB_GP_DATA_REQ_ACTION_ADD_GPDF    ZB_TRUE

#define ZB_CGP_DATA_REQ_USE_CSMA_CA_BIT 0x01
#define ZB_CGP_DATA_REQ_USE_MAC_ACK_BIT 0X02

#endif  /* ZB_ENABLE_ZGP_DIRECT */

/********************************************************************/
/*********************** Proxy definitions **************************/
/********************************************************************/

/**
   @addtogroup zgp_common
   @{
*/

/** Values of gpsCommunicationMode attribute
 *  Table 27
*/
typedef enum zgp_communication_mode_e
{
  ZGP_COMMUNICATION_MODE_FULL_UNICAST = 0,
  /*Groupcast - one of the communication modes used for tunneling GPD commands between the
  proxies and sinks. In Zigbee terms, it is the APS level multicast, with NWK level broadcast to the
  RxOnWhenIdle=TRUE (0xfffd) broadcast address.*/
  ZGP_COMMUNICATION_MODE_GROUPCAST_DERIVED = 1,
  ZGP_COMMUNICATION_MODE_GROUPCAST_PRECOMMISSIONED = 2,
  ZGP_COMMUNICATION_MODE_LIGHTWEIGHT_UNICAST = 3
} zgp_communication_mode_t;

/* A.3.3.2.4 gpsCommissioningExitMode attribute */
typedef enum zgp_commissioning_exit_mode_e
{
  ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION  = (1<<0),
  ZGP_COMMISSIONING_EXIT_MODE_ON_PAIRING_SUCCESS                  = (1<<1),
  ZGP_COMMISSIONING_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT = (1<<2),
  ZGP_COMMISSIONING_EXIT_MODE_ON_CWE_OR_PS                        = (ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION |
                                                                     ZGP_COMMISSIONING_EXIT_MODE_ON_PAIRING_SUCCESS),
  ZGP_COMMISSIONING_EXIT_MODE_ON_CWE_OR_PCM                       = (ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION |
                                                                     ZGP_COMMISSIONING_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT),
  ZGP_COMMISSIONING_EXIT_MODE_ALL                                 = (ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION |
                                                                     ZGP_COMMISSIONING_EXIT_MODE_ON_PAIRING_SUCCESS |
                                                                     ZGP_COMMISSIONING_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT)
} zgp_commissioning_exit_mode_t;

/*! @} */

/********************************************************************/
/******************** Commissioning definitions *********************/
/********************************************************************/

/**
   @addtogroup zgp_common
   @{
*/

/**
 * @brief Possible commissioning result
 */
typedef enum zb_zgp_comm_status_e
{
  /** Commissioning with some device completed successfully */
  ZB_ZGP_COMMISSIONING_COMPLETED,
  /** Commissioning failed. The reason can be:
   * - parameters of connection can't be negotiated
   * - ZGPD device behaves incorrectly
   * - Maximum number of connected ZGPDs is reached */
  ZB_ZGP_COMMISSIONING_FAILED,
  /** Commissioning failed, because of timeout */
  ZB_ZGP_COMMISSIONING_TIMED_OUT,
  /** No functionality match with commissioning device is found.
   *  Maybe matching table is not provided by user application */
  ZB_ZGP_COMMISSIONING_NO_MATCH_ERROR,
  /** Commissioning failed, because some internal error occurred in stack.
   * This type of error is recoverable, so next commissioning attempt can
   * be successful */
  ZB_ZGP_COMMISSIONING_INTERNAL_ERROR,
  /** Commissioning failed, because some external error has occurred.
   * Normal functioning of ZGP subsystem is not possible
   * (e.g. physical operational channel can't be obtained) */
  ZB_ZGP_COMMISSIONING_EXTERNAL_ERROR,
  /** User cancelled commissioning by calling zb_zgps_stop_commissioning() */
  ZB_ZGP_COMMISSIONING_CANCELLED_BY_USER,
  /** ZGPD sent Decommissioning command */
  ZB_ZGP_ZGPD_DECOMMISSIONED,
}
zb_zgp_comm_status_t;

/**
 * @brief GP Sink mode change reason
 */
typedef enum zb_zgp_mode_change_reason_e
{
  ZB_ZGP_MODE_CHANGE_TRIGGERED_BY_COMMAND = 1, /* GP Sink Commissioning Mode Command */
  ZB_ZGP_MODE_CHANGE_TRIGGERED_BY_USER,  /* Application decided to change the mode */
  ZB_ZGP_MODE_CHANGE_ON_FIRST_PARING_EXIT, /* The device joined and the gpsCommissioningExitMode attribute has "On First Pairing success" bit seted */
  ZB_ZGP_MODE_CHANGE_TIMEOUT, /* It is possible if timeout is set and expired  "On CommissioningWindow expiration" bit is set Timeout parameter to user API gps_commissioning_start().*/
}
zb_zgp_mode_change_reason_t;

/** @brief Current mode of ZGP endpoint */
typedef enum zb_zgp_mode_e
{
  ZB_ZGP_OPERATIONAL_MODE,   /**< Operational mode */
  ZB_ZGP_COMMISSIONING_MODE  /**< Commissioning mode */
}
zb_zgp_mode_t;

/**
 * @brief GPD CommandID list
 */
typedef struct zb_zgp_gpd_cmds_list_s
{
  zb_uint8_t num;                                         /**< Number of GP commands */
  zb_uint8_t cmds[ZB_ZGP_MAX_PAIRED_CONF_GPD_COMMANDS];   /**< GPD CommandID list */
}
zb_zgp_gpd_cmds_list_t;

/**
 * @brief GPD Cluster List
 */
typedef struct zb_zgp_cluster_list_s
{
  zb_uint8_t server_cl_num;                                       /**< Number of server ClusterIDs */
  zb_uint8_t client_cl_num;                                       /**< Number of client ClusterIDs */
  zb_uint16_t server_clusters[ZB_ZGP_MAX_PAIRED_CONF_CLUSTERS];   /**< ClusterID List Server */
  zb_uint16_t client_clusters[ZB_ZGP_MAX_PAIRED_CONF_CLUSTERS];   /**< ClusterID List Client */
}
zb_zgp_cluster_list_t;

#if defined ZB_ENABLE_ZGP_SINK || defined DOXYGEN

/**
 * @brief Commissioning request callback type
 *
 * Commissioning request callback notifies application about commissioning
 * attempt from ZGPD. Callback is called when ZGPD sends commissioning frame.
 * Callback provides to user ZGPD ID and ZGPD Device ID. Based on these
 * parameters user application should decide whether commissioning procedure
 * should be continued or not.
 *
 * Commissioning request callback should be set during ZGP initialization using
 * @ref ZB_ZGP_REGISTER_COMM_REQ_CB macro. If callback is not registered, then
 * ZBOSS accepts all incoming commissioning attempts.
 *
 * @param zgpd_id         [in]  ZGPD ID
 * @param device_id       [in]  ZGPD device ID
 * @param manuf_id        [in]  Manufacturer ID (meaningful if device_id = 0xFE or 0xFF)
 * @param manuf_model_id  [in]  Manufacturer model ID (meaningful if device_id = 0xFE or 0xFF)
 * @param ieee_addr       [in]  ZGPD long IEEE address if available, otherwise filled with zeroes
 *
 * @see zb_zgps_accept_commissioning
 * @see ZB_IS_64BIT_ADDR_ZERO
 */
typedef void (ZB_CODE * zb_zgp_comm_req_cb_t)(
    zb_zgpd_id_t    *zgpd_id,
    zb_uint8_t       device_id,
    zb_uint16_t      manuf_id,
    zb_uint16_t      manuf_model_id,
    zb_ieee_addr_t   ieee_addr);

/*! @} */

/**
 * @brief Raw report descriptor received from the GPD
 */
typedef struct zgp_raw_report_desc_s
{
  zb_uint8_t  len;                                      /**< total len, in octets, of the current report descriptor. */
  zb_uint8_t  data[ZB_ZGP_APP_DESCR_REPORT_DATA_SIZE];  /**< array of not parsed zgp_data_point_desc_t */
} zgp_raw_report_desc_t;

/**
 * @brief GPD Commissioning params for Application approval
 */
typedef struct zgp_approve_comm_params_s
{
  zb_zgpd_id_t            zgpd_id;          /**< ZGPD ID */
  zb_uint8_t              device_id;        /**< ZGPD device ID */
  zb_uint16_t             manuf_id;         /**< Manufacturer ID (meaningful if device_id = 0xFE or 0xFF) */
  zb_uint16_t             manuf_model_id;   /**< Manufacturer model ID (meaningful if device_id = 0xFE or 0xFF) */
  zb_ieee_addr_t          ieee_addr;        /**< ZGPD long IEEE address if available, otherwise filled with zeroes */
  zb_uint8_t              pairing_endpoint; /**< Device endpoint, on which commissioning is currently active */
  zb_zgp_gpd_cmds_list_t  gpd_cmds_list;    /**< ZGPD Command list */
  zb_zgp_cluster_list_t   cluster_list;     /**< ZGPD Cluster list */
  zb_uint8_t              num_of_reports;   /**< total number of different report descriptors that GPD sent 
                                                 during the commissioning process */
  zgp_raw_report_desc_t   reports[ZB_ZGP_APP_DESCR_REPORTS_NUM];  /**< array of reports*/
  zb_bool_t               pairing_configuration;  /**< It is ZB_TRUE in case this approve signal was triggered by GP Pairing Configuration command */
  zb_uint8_t              actions;          /**< Pairing configuration actions */
  zb_uint8_t              num_of_endpoints; /**< Pairing configuration number of paired endpoints field
                                                 - 0x00 and 0xfd: there are no paired endpoints
                                                 - 0xff: all matching endpoints are to be paired
                                                 - 0xfe: paired endpoints are to be derived by the sink itself
                                                 - other values: paired_endpoints field is present and contains the list of local endpoints paired to this GPD
                                             **/
  zb_uint8_t              paired_endpoints[ZB_ZGP_MAX_PAIRED_ENDPOINTS];
} zgp_approve_comm_params_t;

/**
   @cond internals_doc
   @addtogroup zgp_internal
   @{
*/

/**
 * @brief Application confirm callback type
 *
 * Application confirm callback notifies application about data
 * request attempt to ZGPD. Callback is called when data frame is sent
 * to ZGPD. Callback provides to command ID and its status. Based on these
 * parameters user application should decide further actions.
 *
 * Application confirm callback should be set during ZGP initialization using
 * @ref ZB_ZGP_REGISTER_APP_CFM_CB macro.
 *
 * @param cmd_id          [in]  Command ID
 * @param status          [in]  Confirmation status
 * @param data_ptr        [in]  Pointer to data payload
 * @param zgpd_id         [in]  ZGPD ID
 * @param handle          [in]  ZGP handle
 */
typedef void (ZB_CODE * zb_zgp_app_cfm_cb_t)(
  zb_uint8_t    cmd_id,
  zb_int16_t    status,
  zb_uint8_t   *data_ptr,
  zb_zgpd_id_t *zgpd_id,
  zb_uint8_t    handle);

/*! @} */
/*! @endcond */


/**
   @addtogroup zgp_sink
   @{
*/

  /**
   * @brief Application callback, indication of the attempted commissioning in GPS operational mode.
   *
   * Application commissioning indication callback notifies application about commissioning
   * attempt from ZGPD. Callback is called when commissioning frame or notification is received
   * from ZGPD. Callback provides to zgpd_id and full incoming packet's data. Based on these
   * parameters user application should decide further actions.
   *
   * Application commissioning indication callback should be set during ZGP initialization using
   * @ref ZB_ZGP_REGISTER_APP_CIC_CB macro.
   *
   * @param zgpd_id         [in]  ZGPD ID
   * @param param           [in]  buffer index, containing GPDF
   */
typedef void (ZB_CODE * zb_zgp_app_comm_ind_cb_t)(
  zb_zgpd_id_t *zgpd_id,
  zb_uint8_t    param);


/*! @} */

/**
   @addtogroup zgp_sink
   @{
*/
#ifdef ZB_ZGP_SINK_SUPPORT_LEGACY_MATCH_INFO
/** @cond DOXYGEN_INTERNAL_DOC */
void zb_zgps_set_match_info(const zb_zgps_match_info_t *info);
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
/**
 * @ingroup zgp_sink
 * @brief Set matching information that is used to fill ZGP command - ZCL
 *
 * @deprecated
 * Match info is deprecated in favor of @ref zb_zgps_mapping_entry_t. Matching information functionality will be removed in August 2023.
 *
 * cluster translation table.
 * @param [in]  info  Matching information of type @ref zb_zgps_match_info_t
 */
#define ZB_ZGP_SET_MATCH_INFO(info)             \
{ \
  zb_zgps_set_match_info((info));               \
}
#endif  /* ZB_ZGP_SINK_SUPPORT_LEGACY_MATCH_INFO */

/** @cond DOXYGEN_INTERNAL_DOC */
void zb_zgps_register_comm_req_cb(zb_zgp_comm_req_cb_t cb);
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
/**
 * @ingroup zgp_sink
 * @brief Register commissioning request callback
 *
 * @param cb [in]  Commissioning request callback (@ref zb_zgp_comm_req_cb_t)
 *
 * @if DOXIGEN_INTERNAL_DOC
 * @snippet tests/zgp/gppb/test_gps_decommissioning/dut_gps.c accept_comm
 * @endif
 */
#define ZB_ZGP_REGISTER_COMM_REQ_CB(cb)         \
{ \
  zb_zgps_register_comm_req_cb((cb)); \
}

/** @cond DOXYGEN_INTERNAL_DOC */
void zb_zgps_register_app_cic_cb(zb_zgp_app_comm_ind_cb_t cb);
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
/**
 * @ingroup zgp_sink
 * @brief Register application commissioning indication callback
 *
 * @param cb [in]  Application commissioning indication callback (@ref zb_zgp_app_comm_ind_cb_t)
 */
#define ZB_ZGP_REGISTER_APP_CIC_CB(cb)          \
{ \
  zb_zgps_register_app_cic_cb((cb)); \
}

/*! @} */
/**
   @cond internals_doc
   @addtogroup zgp_internal
   @{
*/

/**
 * @brief Register application confirm callback
 *
 * @param cb [in]  Application confirm callback (@ref zb_zgp_app_cfm_cb_t)
 */
void zb_zgps_register_app_cfm_cb(zb_zgp_app_cfm_cb_t cb);
#define ZB_ZGP_REGISTER_APP_CFM_CB(cb) \
{ \
  zb_zgps_register_app_cfm_cb((cb)); \
}

/*! @} */
/*! @endcond */

/**
   @addtogroup zgp_common
   @{
*/
#endif  /* ZB_ENABLE_ZGP_SINK */

enum zb_zgpd_switch_type_e
{
  ZB_GPD_SWITCH_TYPE_BUTTON                        = 0x01, /* 0b01 */
  ZB_GPD_SWITCH_TYPE_ROCKER                        = 0x02, /* 0b10 */
};
/********************************************************************/
/********************* GPDF command IDs *****************************/
/********************************************************************/

/**
 * @brief Command identifiers sent from or to ZGPD */
enum zb_zgpd_cmd_id_e
{
  /* identify */
  ZB_GPDF_CMD_IDENTIFY                             = 0x00,
  /* 0x01 - 0x0F: Reserved */
  /* scenes */
  ZB_GPDF_CMD_RECALL_SCENE0                        = 0x10,
  ZB_GPDF_CMD_RECALL_SCENE1                        = 0x11,
  ZB_GPDF_CMD_RECALL_SCENE2                        = 0x12,
  ZB_GPDF_CMD_RECALL_SCENE3                        = 0x13,
  ZB_GPDF_CMD_RECALL_SCENE4                        = 0x14,
  ZB_GPDF_CMD_RECALL_SCENE5                        = 0x15,
  ZB_GPDF_CMD_RECALL_SCENE6                        = 0x16,
  ZB_GPDF_CMD_RECALL_SCENE7                        = 0x17,
  /* @note recall scene 8-11 are got from LCGW. But GPPB specification
   * does not define recall scenes 8-11. Only 8 scenes are
   * supported. See Table 49 Payloadless GPDF commands 6236 sent by
   * GPD */
  ZB_GPDF_CMD_RECALL_SCENE8                        = 0x18,
  ZB_GPDF_CMD_RECALL_SCENE9                        = 0x19,
  ZB_GPDF_CMD_RECALL_SCENE10                       = 0x1A,
  ZB_GPDF_CMD_RECALL_SCENE11                       = 0x1B,
#define ZB_GPDF_CMD_STORE_SCENE0 ZB_GPDF_CMD_RECALL_SCENE8
#define ZB_GPDF_CMD_STORE_SCENE1 ZB_GPDF_CMD_RECALL_SCENE9
#define ZB_GPDF_CMD_STORE_SCENE2 ZB_GPDF_CMD_RECALL_SCENE10
#define ZB_GPDF_CMD_STORE_SCENE3 ZB_GPDF_CMD_RECALL_SCENE11
  ZB_GPDF_CMD_STORE_SCENE4                         = 0x1C,
  ZB_GPDF_CMD_STORE_SCENE5                         = 0x1D,
  ZB_GPDF_CMD_STORE_SCENE6                         = 0x1E,
  ZB_GPDF_CMD_STORE_SCENE7                         = 0x1F,
  /* on/off */
  ZB_GPDF_CMD_OFF                                  = 0x20,
  ZB_GPDF_CMD_ON                                   = 0x21,
  ZB_GPDF_CMD_TOGGLE                               = 0x22,
  /* ------- */
  ZB_GPDF_CMD_RELEASE                              = 0x23,
  /* 0x24 - 0x2F: Reserved */
  /* level control */
  ZB_GPDF_CMD_MOVE_UP                              = 0x30,
  ZB_GPDF_CMD_MOVE_DOWN                            = 0x31,
  ZB_GPDF_CMD_STEP_UP                              = 0x32,
  ZB_GPDF_CMD_STEP_DOWN                            = 0x33,
  ZB_GPDF_CMD_LC_STOP                              = 0x34,
#define ZB_GPDF_CMD_LVL_CTRL_STOP ZB_GPDF_CMD_LC_STOP
  ZB_GPDF_CMD_MOVE_UP_W_ONOFF                      = 0x35,
#define ZB_GPDF_CMD_MOVE_UP_WITH_ON_OFF ZB_GPDF_CMD_MOVE_UP_W_ONOFF
  ZB_GPDF_CMD_MOVE_DOWN_W_ONOFF                    = 0x36,
#define ZB_GPDF_CMD_MOVE_DOWN_WITH_ON_OFF ZB_GPDF_CMD_MOVE_DOWN_W_ONOFF
  ZB_GPDF_CMD_STEP_UP_W_ONOFF                      = 0x37,
#define ZB_GPDF_CMD_MOVE_STEP_ON ZB_GPDF_CMD_STEP_UP_W_ONOFF
  ZB_GPDF_CMD_STEP_DOWN_W_ONOFF                    = 0x38,
#define ZB_GPDF_CMD_MOVE_STEP_OFF ZB_GPDF_CMD_STEP_DOWN_W_ONOFF
  /* 0x39 - 0x3F: Reserved */
  /* Color Control */
  ZB_GPDF_CMD_MOVE_HUE_STOP                        = 0x40,
  ZB_GPDF_CMD_MOVE_HUE_UP                          = 0x41,
  ZB_GPDF_CMD_MOVE_HUE_DOWN                        = 0x42,
  ZB_GPDF_CMD_STEP_HUE_UP                          = 0x43,
  ZB_GPDF_CMD_STEP_HUE_DOWN                        = 0x44,
  ZB_GPDF_CMD_MOVE_SATURATION_STOP                 = 0x45,
  ZB_GPDF_CMD_MOVE_SATURATION_UP                   = 0x46,
  ZB_GPDF_CMD_MOVE_SATURATION_DOWN                 = 0x47,
  ZB_GPDF_CMD_STEP_SATURATION_UP                   = 0x48,
  ZB_GPDF_CMD_STEP_SATURATION_DOWN                 = 0x49,
  ZB_GPDF_CMD_MOVE_COLOR                           = 0x4A,
  ZB_GPDF_CMD_STEP_COLOR                           = 0x4B,
  /* 0x4C - 0x4F: Reserved */
  /* Door Lock */
  ZB_GPDF_CMD_LOCK_DOOR                            = 0x50,
  ZB_GPDF_CMD_UNLOCK_DOOR                          = 0x51,
  /* 0x52 - 0x5F: Reserved */
  /* Simple Generic Switch */
  ZB_GPDF_CMD_PRESS_1_OF_1                         = 0x60,
  ZB_GPDF_CMD_RELEASE_1_OF_1                       = 0x61,
  ZB_GPDF_CMD_PRESS_1_OF_2                         = 0x62,
  ZB_GPDF_CMD_RELEASE_1_OF_2                       = 0x63,
  ZB_GPDF_CMD_PRESS_2_OF_2                         = 0x64,
  ZB_GPDF_CMD_RELEASE_2_OF_2                       = 0x65,
  ZB_GPDF_CMD_SHORT_PRESS_1_OF_1                   = 0x66,
  ZB_GPDF_CMD_SHORT_PRESS_1_OF_2                   = 0x67,
  ZB_GPDF_CMD_SHORT_PRESS_2_OF_2                   = 0x68,

  ZB_GPDF_CMD_8BIT_VECTOR_PRESS                    = 0x69,
  ZB_GPDF_CMD_8BIT_VECTOR_RELEASE                  = 0x6A,
  /* 0x6b-0x6f: Reserved */
  /* 0x70-0x9f: Reserved */
  ZB_GPDF_CMD_ATTR_REPORT                          = 0xA0,
  ZB_GPDF_CMD_MANUF_SPEC_ATTR_REPORT               = 0xA1,
  ZB_GPDF_CMD_MULTI_CLUSTER_ATTR_REPORT            = 0xA2,
  ZB_GPDF_CMD_MANUF_SPEC_MULTI_CLUSTER_ATTR_REPORT = 0xA3,
  ZB_GPDF_CMD_REQUEST_ATTRIBUTES                   = 0xA4,
  ZB_GPDF_CMD_READ_ATTR_RESP                       = 0xA5,

  ZB_GPDF_CMD_ZCL_TUNNELING_FROM_ZGPD              = 0xA6,
  /* 0xA7: Reserved */
  ZB_GPDF_CMD_COMPACT_ATTR_REPORTING               = 0xA8,
  /* 0xA9 - 0xAE: Reserved */
  ZB_GPDF_CMD_ATTR_REPORT_ANY                      = 0xAF,
  /* Manufacturer-defined GPD commands (payload is manufacturer-specific) */
  ZB_GPDF_CMD_MANUF_DEFINED_B0                     = 0xB0,
  /* 0xB1 - 0xBE: Manufacturer-defined GPD commands (payload is manufacturer-specific) */
  ZB_GPDF_CMD_MANUF_DEFINED_BF                     = 0xBF,
  /* 0xC0 - 0xDF: Reserved */
  /* commissioning from ZGPD */
  ZB_GPDF_CMD_COMMISSIONING                        = 0xE0,
  ZB_GPDF_CMD_DECOMMISSIONING                      = 0xE1,
  ZB_GPDF_CMD_SUCCESS                              = 0xE2,
  ZB_GPDF_CMD_CHANNEL_REQUEST                      = 0xE3,
  ZB_GPDF_CMD_APPLICATION_DESCR                    = 0xE4,
  /* 0xE5 - 0xEF: Reserved */
  /* GPDF commands sent to GPD */
  ZB_GPDF_CMD_COMMISSIONING_REPLY                  = 0xF0,
  ZB_GPDF_CMD_WRITE_ATTRIBUTES                     = 0xF1,
  ZB_GPDF_CMD_READ_ATTRIBUTES                      = 0xF2,
  ZB_GPDF_CMD_CHANNEL_CONFIGURATION                = 0xF3,
  /* 0xF4 - 0xF5: Reserved for other commands sent to the GPD */
  ZB_GPDF_CMD_ZCL_TUNNELING_TO_ZGPD                = 0xF6,
  /* 0xF7 - 0xFF: Reserved for other commands sent to the GPD */
};

#define ZB_GPDF_CMD_IS_SCENE_CMD(_cmd_id)    \
  ((_cmd_id) >= ZB_GPDF_CMD_RECALL_SCENE0 && \
   (_cmd_id) <= ZB_GPDF_CMD_STORE_SCENE7)

#define ZB_GPDF_CMD_IS_RECALL_SCENE(_cmd_id) \
  ((_cmd_id) >= ZB_GPDF_CMD_RECALL_SCENE0 && \
   (_cmd_id) <= ZB_GPDF_CMD_RECALL_SCENE7)

#define ZB_GPDF_CMD_IS_STORE_SCENE(_cmd_id) \
  ((_cmd_id) >= ZB_GPDF_CMD_STORE_SCENE0 && \
   (_cmd_id) <= ZB_GPDF_CMD_STORE_SCENE7)

/*! @} */

/********************************************************************/
/**************** Macros for sending GPDF packets *******************/
/********************************************************************/

/**
   @cond internal
   @addtogroup zgp_internal
   @{
*/


/**
 * @brief Start constructing GPDF packet
 *
 * @param [in] zbbuf Buffer for GPDF packet
 */
#define ZB_START_GPDF_PACKET(zbbuf)                    \
   zb_buf_reuse(zbbuf)

/**
 * @brief Put 1 byte into GPDF packet
 *
 * @param ptr [in] Destination memory address, where val should be copied
 * @param val [in] Pointer to 1-byte value to be put in packet
 */
#define ZB_GPDF_PUT_UINT8(ptr, val) \
  ( *(ptr) = (val), (ptr)++ )

/**
 * @brief Put 2 bytes into GPDF packet
 *
 * @param ptr [in] Destination memory address, where val should be copied
 * @param val [in] Pointer to 2-byte value to be put in packet
 */
#define ZB_GPDF_PUT_UINT16(ptr, val) \
  ( ZB_HTOLE16((ptr), (val)), (ptr) += 2 )


/**
 * @brief Put 4 bytes into GPDF packet
 *
 * @param ptr [in] Destination memory address, where val should be copied
 * @param val [in] Pointer to 4-byte value to be put in packet
 */
#define ZB_GPDF_PUT_UINT32(ptr, val) \
  ( ZB_HTOLE32((ptr), (val)), (ptr) += 4 )


/**
 * @brief Finish constructing GPDF frame
 *
 * @param zbbuf [in] Buffer with GPDF
 * @param ptr   [in] Pointer to the GPDF tail
 */
void zb_finish_gpdf_packet(zb_bufid_t buf_ref, zb_uint8_t** ptr);
#define ZB_FINISH_GPDF_PACKET(zbbuf, ptr) zb_finish_gpdf_packet(zbbuf, &ptr)


/**
 * @brief RX channel in the next attempt parameter of ZGPD Channel request command
 * @see ZGP spec, A.4.2.1.4
 */
#define ZB_GPDF_CHANNEL_REQ_NEXT_RX_CHANNEL(par) \
  ((par) & 0x0FU)


typedef struct zb_gpdf_comm_app_info_options_s
{
  zb_bitfield_t manuf_id_present:1;
  zb_bitfield_t manuf_model_id_present:1;
  zb_bitfield_t gpd_cmds_present:1;
  zb_bitfield_t cluster_list_present:1;
  zb_bitfield_t switch_info_present:1;
  zb_bitfield_t app_descr_flw:1;
  zb_bitfield_t reserved:2;
}zb_gpdf_comm_app_info_options_t;

typedef ZB_PACKED_PRE struct zb_gpdf_comm_switch_gen_cfg_s
{
  zb_bitfield_t num_of_contacts:4;
  zb_bitfield_t switch_type:2;
  zb_bitfield_t reserved:2;
}ZB_PACKED_STRUCT zb_gpdf_comm_switch_gen_cfg_t;

/* DEPRECATED: Typo in structure field was fixes -
 * old name, with the typo, will be removed in the next Major release */
#define num_of_contacs num_of_contacts

typedef struct zb_gpdf_comm_switch_info_s
{
  zb_uint8_t    len;                       /**< Length of switch info */
  zb_gpdf_comm_switch_gen_cfg_t configuration;
  zb_uint8_t    current_contact_status;
}zb_gpdf_comm_switch_info_t;

typedef struct zb_gpdf_comm_app_info_s
{
  zb_gpdf_comm_app_info_options_t options;

  zb_uint16_t                 manuf_id;         /**< Manufacturer ID */
  zb_uint16_t                 manuf_model_id;   /**< Manufacturer model ID */
  zb_zgp_gpd_cmds_list_t      gpd_cmds_list;    /**< GPD CommandID list */
  zb_zgp_cluster_list_t       cluster_list;     /**< Cluster list */
  zb_gpdf_comm_switch_info_t  switch_info;      /**< Switch information */
}zb_gpdf_comm_app_info_t;

/**
 * @brief ZGPD Commissioning command parameters
 * @see ZGP spec, A.4.2.1.1
 */
typedef struct zb_gpdf_comm_params_s
{
  zb_uint8_t zgpd_device_id;    /**< ZGPD Device ID */
  zb_uint8_t options;           /**< Options */
  zb_uint8_t ext_options;       /**< Extended options */
  zb_gpdf_comm_app_info_t app_info; /**< Application information */
}
zb_gpdf_comm_params_t;

/**
 * @brief ZGPD Commissioning reply parameters
 * @see ZGP spec, A.4.2.1.1
 */
typedef struct zb_gpdf_comm_reply_s
{
  zb_uint8_t     options;                       /**< Options */
  zb_uint16_t    pan_id;                        /**< Pan ID if requested */
  zb_uint8_t     security_key[ZB_CCM_KEY_SIZE]; /**< ZGPD key */
  zb_uint8_t     key_mic[ZB_CCM_M];             /**< ZGPD key MIC */
  zb_uint32_t    frame_counter;                 /**< ZGPD key encryption counter */
}
zb_gpdf_comm_reply_t;

#define ZB_GPDF_COMM_REPLY_PAN_ID_PRESENT(options) ((options) & 0x01U)
#define ZB_GPDF_COMM_REPLY_SEC_KEY_PRESENT(options) (((options) >> 1U) & 0x01U)
#define ZB_GPDF_COMM_REPLY_SEC_KEY_ENCRYPTED(options) (((options) >> 2U) & 0x01U)
#define ZB_GPDF_COMM_REPLY_SEC_LEVEL(options) (((options) >> 3U) & 0x03U)
#define ZB_GPDF_COMM_REPLY_SEC_KEY_TYPE(options) (((options) >> 5U) & 0x07U)

/**
 * @brief Construct options field of commissioning command from given values
 * @see ZGP spec, A.4.2.1.1.2
 */
#define ZB_GPDF_COMM_OPT_FLD(sn_cap, rx_cap, ms_ext, pan_id_req, \
    sec_key_req, fixed_loc, ext_opt) \
  (   (sn_cap) \
   | ((rx_cap)      << 1U) \
   | ((ms_ext)      << 2U) \
   | ((pan_id_req)  << 4U) \
   | ((sec_key_req) << 5U) \
   | ((fixed_loc)   << 6U) \
   | ((ext_opt)     << 7U) )

/**
 * @brief Value of Extended Options bit in
 * options field of commissioning command
 */
#define ZB_GPDF_COMM_EXT_OPT_PRESENT(options) \
  ((options) >> 7U)

/**
 * @brief Value of GP security Key request bit in
 * options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_SEC_KEY_REQ(options) \
  (((options) >> 5U) & 0x01U)

/**
 * @brief Value of Pan ID request bit in
 * options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_PAN_ID_REQ(options) \
  (((options) >> 4U) & 0x01U)

/**
 * @brief Value of MAC sequence number capability bit in
 * options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_SEQ_NUM_CAPS(options) \
  ((options) & 0x01U)

/**
 * @brief Value of RxOnCapability bit in
 * options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_RX_CAPABILITY(options) \
  (((options) >> 1U) & 0x01U)

/**
 * @brief Value of Fixed location bit in
 * options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_FIX_LOC(options) \
  (((options) >> 6U) & 0x01U)

/**
 * @brief Value of GPD MS extensions present bit in
 * Options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_APP_INF_PRESENT(options) \
  (((options) >> 2U) & 0x01U)

/**
 * @brief Construct Extended Options field of commissioning command
 * @see ZGP spec, A.4.2.1.1.3
 */
#define ZB_GPDF_COMM_EXT_OPT_FLD(sec_cap, key_type, key_present, \
    key_enc, out_counter) \
  (   (sec_cap) \
   | ((key_type)    << 2U) \
   | ((key_present) << 5U) \
   | ((key_enc)     << 6U) \
   | ((out_counter) << 7U))

/**
 * @brief Value of GPD Key present bit in
 * Extended options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_ZGPD_KEY_PRESENT(ext_options) \
  (((ext_options) >> 5U) & 0x01U)

/**
 * @brief Value of GPD Key encryption bit in
 * Extended options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_ZGPD_KEY_ENCRYPTED(ext_options) \
  (((ext_options) >> 6U) & 0x01U)

/**
 * @brief Value of GPD outgoing counter present bit in
 * Extended options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_ZGPD_OUT_COUNTER_PRESENT(ext_options) \
  ((ext_options) >> 7U)

/**
 * @brief Value of SecurityLevel capabilities bits in
 * Extended options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_SEC_LEVEL_CAPS(ext_options) \
  ((ext_options) & 0x03U)

/**
 * @brief Value of SecurityKey type bits in
 * Extended options field of commissioning command
 */
#define ZB_GPDF_COMM_OPT_SEC_KEY_TYPE(ext_options) \
  ((ext_options >> 2U) & 0x07U)

/**
 * @brief Construct MS extensions field of commissioning command
 * @see docs-13-0146-04-batt-green-powermanufacturer-specific-device-type-command.docx
 */
#define ZB_GPDF_COMM_MS_EXT_FLD(_manuf_id, _manuf_model_id, _gp_cmd_list, _clstr_reports) \
  (   (_manuf_id) \
   | ((_manuf_model_id) << 1U) \
   | ((_gp_cmd_list)    << 2U) \
   | ((_clstr_reports)  << 3U))

/**
 * @brief Construct Options field of commissioning reply command
 * @see ZGP spec, A.4.2.1.2.1
 */
#define ZB_GPDF_COMM_REPLY_OPT_FLD(pan_id_present, key_present, \
    key_enc, sec_level, key_type) \
  (   (pan_id_present) \
   | ((key_present) << 1U) \
   | ((key_enc)     << 2U) \
   | ((sec_level)   << 3U) \
   | ((key_type)    << 5U))

/** @} */
/*! @endcond */


/**
   @cond internals_doc
   @addtogroup zgp_internal
   @{
*/

/**
 * @brief Send commissioning GPDF with ZGPD securityLevel capabilities set to 0
 */
#define ZB_SEND_COMMISSIONING_GPDF_WITHOUT_SEC(buf, device_id, sn_cap, rx_cap) \
{ \
  zb_uint8_t* ptr = (zb_uint8_t*)ZB_START_GPDF_PACKET(buf); \
  (void)zb_buf_alloc_left(buf, 3U, ptr); \
  ZB_GPDF_PUT_UINT8(ptr, ZB_GPDF_CMD_COMMISSIONING); \
  ZB_GPDF_PUT_UINT8(ptr, device_id); \
  ZB_GPDF_PUT_UINT8(ptr, ZB_GPDF_COMM_OPT_FLD(sn_cap, rx_cap, 0U, 0U, 0U, 0U, 0U)); \
  ZB_SEND_GPDF_CMD(ZB_REF_FROM_BUF(buf)); \
}

/**
 * @brief Attribute report field of attribute reporting command
 * @see ZGP spec, A.4.2.3.1
 */
typedef struct zb_gpdf_attr_report_fld_s
{
  zb_uint16_t attr_id;   /**< Attribute ID specific to cluster */
  zb_uint8_t attr_type;  /**< Attribute type (see @ref zcl_attr_type) */
  void* data_p;     /**< Attribute data */
}
zb_gpdf_attr_report_fld_t;

/**
 * @brief Start constructing ZGPD attribute reporting command
 *
 * @param buf        [in]  Buffer for GPDF command
 * @param cluster_id [in]  Cluster ID of attributes being reported
 * @param ptr        [out] Pointer to the current tail of GPDF
 */
#define ZB_ZGPD_ATTR_REPORTING_CMD_INIT(buf, cluster_id, ptr) \
{ \
  ptr = ZB_START_GPDF_PACKET(buf); \
  ZB_GPDF_PUT_UINT8(ptr, ZB_GPDF_CMD_ATTR_REPORT); \
  ZB_GPDF_PUT_UINT16(ptr, &cluster_id); \
}

/**
 * @brief Put attribute report field into attribute reporting command
 *
 * Macro should be called only after attribute reporting command is
 * initialized with @ref ZB_ZGPD_ATTR_REPORTING_CMD_INIT
 * @param ptr   [in,out]  Pointer to the tail of attribute reporting command
 * @param attr  [in]      Attribute report field (see @zb_gpdf_attr_report_fld_t)
 */
#define ZB_ZGPD_ATTR_REPORTING_CMD_NEXT(ptr, attr) \
{ \
  ZB_GPDF_PUT_UINT16(ptr, &attr.attr_id); \
  ZB_GPDF_PUT_UINT8(ptr, attr.attr_type); \
  ptr = zb_zcl_put_value_to_packet(ptr, attr.attr_type, (zb_uint8_t *)attr.data_p); \
}

/**
 * @brief Finish constructing ZGPD attribute reporting command
 *
 * @param buf        [in] Buffer for GPDF command
 * @param ptr        [in] Pointer to the tail of GPDF
 */
#define ZB_ZGPD_ATTR_REPORTING_CMD_FINISH(buf, ptr) \
{ \
  ZB_FINISH_GPDF_PACKET(buf, ptr);      \
}

/**
 * @brief Value of multi-record bit of options field
 *        in ZGPD Request attributes or Write attributes command
 *        (ZGP spec, rev. 26 A.4.2.6.1)
 */
#define ZB_GPDF_REQUEST_ATTR_IS_MULTI_RECORD(opts) \
  (opts & 0x01U)

/**
 * @brief Value of multi-record bit of options field
 *        in ZGPD Write attributes command
 *        (ZGP spec, rev. 26 A.4.2.6.1)
 */
#define ZB_GPDF_WRITE_ATTR_IS_MULTI_RECORD \
  ZB_GPDF_REQUEST_ATTR_IS_MULTI_RECORD

/**
 * @brief Value of "manufacturer field present" bit of options field
 *        in ZGPD Request attributes command
 *        (ZGP spec, rev. 26 A.4.2.6.1)
 */
#define ZB_GPDF_REQUEST_ATTR_MANUF_FIELD_PRESENT(opts) \
  ((opts >> 1U) & 0x01U)

/**
 * @brief Value of "manufacturer field present" bit of options field
 *        in ZGPD Write attributes command
 *        (ZGP spec, rev. 26 A.4.2.6.1)
 */
#define ZB_GPDF_WRITE_ATTR_MANUF_FIELD_PRESENT \
  ZB_GPDF_REQUEST_ATTR_MANUF_FIELD_PRESENT

/**
 * @brief Construct value of options field
 *        in ZGPD Request attributes or Write attributes command
 *        (ZGP spec, rev. 26 A.4.2.6.1)
 */
#define ZB_GPDF_REQUEST_ATTR_OPTIONS_FLD(multi_record, manuf_present) \
  ((multi_record) | (manuf_present << 1U))

/**
 * @brief Construct value of options field
 *        in ZGPD Write attributes command
 *        (ZGP spec, rev. 26 A.4.2.6.3)
 */
#define ZB_GPDF_WRITE_ATTR_OPTIONS_FLD \
  ZB_GPDF_REQUEST_ATTR_OPTIONS_FLD

/**
 * @brief Parse one Cluster Record request field of
 * ZGPD Request attributes command
 * @param rec         pointer to record to parse of type zb_uint8_t*
 * @param cluster_id  Cluster ID (out)
 * @param attr_count  Attribute count (out)
 * @param attrs  Pointer to attributes list (out)
 */
#define ZB_GPDF_REQUEST_ATTR_PARSE_RECORD(rec, cluster_id, attr_count, attrs) \
{ \
  ZB_LETOH16((cluster_id), (rec)); \
  *(attr_count) = (rec)[2]/sizeof(zb_uint16_t); \
  *(attrs) = (zb_uint16_t *)(void *)(&(rec)[3]); \
}

/**
 * @brief Start constructing ZGP write attribute command
 *
 * @param buf        [in]  Buffer for GPDF command
 * @param cluster_id [in]  Cluster ID of attributes being reported
 * @param ptr        [out] Pointer to the current tail of GPDF
 */
#define ZB_ZGPD_WRITE_ATTR_CMD_INIT(buf, ptr) \
{ \
  ptr = ZB_START_GPDF_PACKET(buf); \
  ZB_GPDF_PUT_UINT8(ptr, ZB_GPDF_WRITE_ATTR_OPTIONS_FLD(0,0)); \
}

/**
 * @brief Start constructing ZGP write attribute command
 *
 * @param buf        [in]  Buffer for GPDF command
 * @param manuf_id   [in]  Manufacturer ID
 * @param ptr        [out] Pointer to the current tail of GPDF
 */
#define ZB_ZGPD_WRITE_ATTR_MANUF_CMD_INIT(buf, manuf_id, ptr) \
{ \
  ptr = ZB_START_GPDF_PACKET(buf); \
  ZB_GPDF_PUT_UINT8(ptr, ZB_GPDF_WRITE_ATTR_OPTIONS_FLD(0,1)); \
  ZB_GPDF_PUT_UINT16(ptr, &(manuf_id)); \
}

/**
 * @brief Start cluster record field in write attributes command
 *
 * Macro should be called only after write attributes command is
 * initialized with @ref ZB_ZGPD_WRITE_ATTR_MANUF_CMD_INIT or
 * @ref ZB_ZGPD_WRITE_ATTR_CMD_INIT
 * @param ptr             [in,out]  Pointer to the tail of write attributes command
 * @param cluster_id      [in]      Cluster ID
 * @param record_list_len [in]      Length in bytes of following Write Attribute Records list
 */
#define ZB_ZGPD_WRITE_ATTR_NEW_CLUSTER_RECORD(ptr, cluster_id, record_list_len) \
{ \
  ZB_GPDF_PUT_UINT16(ptr, &cluster_id); \
  ZB_GPDF_PUT_UINT8(ptr, (record_list_len)); \
}

/**
 * @brief Put write attribute record into write attributes command
 *
 * Macro should be called only after new cluster record is
 * initialized with @ref ZB_ZGPD_WRITE_ATTR_NEXT_CLUSTER_RECORD
 * @param ptr       [in,out]  Pointer to the tail of write attributes command
 * @param attr_id   [in]      Attribute ID
 * @param attr_type [in]      Attribute type
 * @param value     [in]      Pointer to attribute value
 */
#define ZB_ZGPD_WRITE_ATTR_CMD_NEXT_ATTR(ptr, attr_id, attr_type, value) \
{ \
  ZB_GPDF_PUT_UINT16(ptr, &attr_id); \
  ZB_GPDF_PUT_UINT8(ptr, attr_type); \
  ptr = zb_zcl_put_value_to_packet(ptr, attr_type, (zb_uint8_t *)(value)); \
}

/**
 * @brief Finish constructing ZGPD write attributes command
 *
 * @param buf        [in] Buffer for GPDF command
 * @param ptr        [in] Pointer to the tail of GPDF
 */
#define ZB_ZGPD_WRITE_ATTR_CMD_FINISH(buf, ptr) \
{ \
  ZB_FINISH_GPDF_PACKET(buf, ptr);      \
}

/* [AV] tmm it'll be better to name macros like this once
   and don't rename them like #def macro_write_init(...) macro_read_init(...),
   because there is no difference
*/
#define ZB_ZGPD_CLUSTER_CMD_INIT(_buf, _ptr) \
  ZB_ZGPD_WRITE_ATTR_CMD_INIT(_buf, _ptr)
#define ZB_ZGPD_CLUSTER_CMD_MANUF_INIT(_buf, _manuf_id, _ptr)   \
  ZB_ZGPD_WRITE_ATTR_MANUF_CMD_INIT(_buf, _manuf_id, _ptr)
#define ZB_ZGPD_NEW_CLUSTER_RECORD(_ptr, _cluster_id, _record_list_len) \
  ZB_ZGPD_WRITE_ATTR_NEW_CLUSTER_RECORD(_ptr, _cluster_id, _record_list_len)
#define ZB_ZGPD_READ_ATTR_CMD_NEXT_ATTR(_ptr, _attr_id) \
{ \
  ZB_GPDF_PUT_UINT16(_ptr, &_attr_id); \
}
#define ZB_ZGPD_CLUSTER_CMD_FINISH(_buf, _ptr) \
{ \
  ZB_FINISH_GPDF_PACKET(_buf, _ptr); \
}

#ifdef ZB_ENABLE_ZGP_DIRECT
typedef struct zb_zgps_send_cmd_params_s
{
  zb_uint8_t     cmd_id;
  zb_zgpd_id_t   zgpd_id;
  zb_ieee_addr_t ieee_addr;
  zb_time_t      lifetime;
  zb_uint8_t     tx_options;
  zb_uint8_t     handle;
}
zb_zgps_send_cmd_params_t;

/**
 * @brief Send provided packet to ZGPD
 *
 * Buffer data is command payload to send.
 * Other parameters are in the buffer tail (see @ref zb_zgps_send_cmd_params_t).
 *
 * @param param[in, out]    Reference to buffer.
 *
 * @note maximum length of data payload is @ref ZB_ZGP_TX_CMD_PLD_MAX_SIZE
 *
 * @note zb_gp_data_cfm is called from:
 *  - gp_data_req_send_cnf       to notify about status of adding data to tx_packet_info_queue;
 *  - notify_about_expired_entry to notify about expired entry;
 *  - zb_cgp_data_cfm            to notify about status from MAC layer.
 *
 * @note Status of confirm (ZGP TX queue is used) can be:
 *      ZB_ZGP_STATUS_ENTRY_REPLACED
 *      ZB_ZGP_STATUS_ENTRY_ADDED
 *      ZB_ZGP_STATUS_ENTRY_EXPIRED
 *      ZB_ZGP_STATUS_ENTRY_REMOVED
 *      ZB_ZGP_STATUS_TX_QUEUE_FULL
 *
 *      MAC_SUCCESS
 *
 * @note Status of confirm (ZGP TX queue is not used) can be:
 *      ZB_ZGP_STATUS_TX_QUEUE_FULL
 *
 *      MAC_SUCCESS
 *      MAC_NO_ACK
 *
 */
void zb_zgps_send_data(zb_uint8_t param);
#endif  /* ZB_ENABLE_ZGP_DIRECT */
/*! @}
 *  @endcond */

/**
   @addtogroup zgp_sink
   @{
*/

/**
 * @brief Put ZGPS into commissioning mode, the mode will be changed back to
 * operational mode if:
 *  - "On first Pairing success" is set in gpsCommissioningModeExit attribute and a
 *    successful paring occurs. See @ref ZB_ZGP_DEFAULT_COMMISSIONING_EXIT_MODE.
 *  - The timeout parameter is different from 0 and a timeout occurs.
 *  - GP Sink Commissioning Mode command with Action field set to 0x0 is received.
 * 
 * @ref ZB_ZGP_SIGNAL_MODE_CHANGE is generated when sink changes mode from 
 * Operation Mode to Commissioning Mode and when it changes from Commissioning
 * Mode to Operational Mode.
 * 
 * @ref ZB_ZGP_SIGNAL_COMMISSIONING is generated if a successful commission happens
 * or if a decommissioning happens
 * 
 * @cond DOXYGEN_INTERNAL_DOC
 * When called from GP Sink Commissioning Mode command with Action field set to
 * 0x1 the timeout must be set to gpsCommissioningWindow attribute value if
 * "On CommissioningWindow expiration" bit is set in gpsCommissioningExitMode
 * attribute or 0 if it is not set. \n
 * The mode changes back from commissioning mode to operation mode according to
 * the gpsCommissioningExitMode A.3.3.2.4 attribute and gpsCommissioningWindow
 * A.3.3.2.5.
 * @endcond
 *
 * @param timeout Maximum commissioning time in beacon intervals, 0 means no timeout.
 *
 * It is safe to call this function when device is already in
 * commissioning mode. In this case function does nothing.
 * 
 * @snippet light_sample/light_coordinator_combo/light_zc.c zgps_start_comm
 */
void zb_zgps_start_commissioning(zb_time_t timeout);

/**
 * @brief Put ZGPS into commissioning mode on a specific endpoint
 *
 * It is safe to call this function when device is already in
 * commissioning mode. In this case function does nothing.
 *
 * @param[in] ep            Endpoint to start a commissioning.
 *                      Will be used for GPD command forwarding
 *
 * @param[in] timeout       Maximum commissioning time in beacon intervals. \n
 *                      0 means no timeout. \n
 *                      If timeout occurs, then result of commissioning is
 *                      @ref ZB_ZGP_COMMISSIONING_TIMED_OUT
 *
 */
void zb_zgps_start_commissioning_on_ep(zb_uint8_t ep, zb_time_t timeout);

/**
 * @brief Switch ZGPS back to operational mode from commissioning
 *
 * @cond DOXYGEN_INTERNAL_DOC
 * After commissioning is cancelled, user is notified with
 * @ref zb_zgp_comm_completed_cb_t with ZB_ZGP_COMMISSIONING_CANCELLED_BY_USER
 * status.
 * @endcond
 *
 * @snippet light_sample/light_coordinator_combo/light_zc.c zgps_stop_comm
 */
void zb_zgps_stop_commissioning(void);

/**
 * @brief Accept/reject ZGPD commissioning attempt
 *
 * This function should be called as an answer to commissioning request made by
 * stack via signal ZB_ZGP_SIGNAL_APPROVE_COMMISSIONING or
 * @ref zb_zgp_comm_req_cb_t callback. Also, it can be called from
 * @ref zb_zgp_comm_req_cb_t callback as well as outside it.
 *
 * @param[in] accept - If ZB_TRUE, then stack will continue ongoing commissioning
 *                     process with ZGPD \n
 *                     Otherwise ongoing commissioning process will be
 *                     terminated
 * @if DOXIGEN_INTERNAL_DOC
 * @snippet simple_combo/zc_combo.c accept_comm
 * @endif
 */
void zb_zgps_accept_commissioning(zb_bool_t accept);

/**
 * @brief Remove all the information about ZGPD from stack
 *
 * In ZGP there is no way to say ZGPD to leave the network.
 * ZGPD can leave network by itself using "Decommissioning" command,
 * but ZGPS can miss this command if it was not in commissioning state.
 *
 * This function removes all the information related to specified ZGPD
 * from stack.
 *
 * @param buf_ref  reference to the free buffer
 * @param zgpd_id  identifier of ZGPD to be removed
 *
 * @note It is safe to free or overwrite memory pointed by zgpd_id
 *       after call
 */
void zb_zgps_delete_zgpd(zb_uint8_t buf_ref, zb_zgpd_id_t *zgpd_id);

/**
 * @brief Remove all the information about ALL ZGPD from stack
 *
 */
void zb_zgps_delete_all_zgpd(void);

/**
   Get LQI and RSSI last time received from that ZGPD.
 */
void zb_zgps_get_diag_data(zb_zgpd_id_t *zgpd_id, zb_uint8_t *lqi, zb_int8_t *rssi);

/*! @} */

/**
   @addtogroup zgp_sink
   @{
*/

/**
   Fill security level constant to be passed to zb_zgps_set_security_level()

   Described in A.3.3.2.6 gpsSecurityLevel attribute.

   @param sec_lvl  @ref zb_zgp_security_level_e Minimal GPD Security Level sub-field contains the minimum gpdSecurityLevel this sink accepts
   @param with_link_key 1 bit - Protection with the gpLinkKey sub-field, indicates if
   the GPDs attempting the pairing are required to support protecting the
   over-the-air exchange of the GPD Key
   @param involve_tc always zero for the current GPPB specification

   @snippet light_sample/light_coordinator_combo/light_zc.c zgps_set_secur_level
 */
#define ZB_ZGP_FILL_GPS_SECURITY_LEVEL(sec_lvl, with_link_key, involve_tc)\
  (((sec_lvl) & 3U) | ((!!(with_link_key)) << 2U) | ((!!(involve_tc)) << 3U))


/**
   Set gpsSecurityLevel GP cluster attribute of gpcb

   Described in A.3.3.2.6 gpsSecurityLevel attribute.

   @param level Security level to set

   @snippet light_sample/light_coordinator_combo/light_zc.c zgps_set_secur_level
  */
void zb_zgps_set_security_level(zb_uint_t level);

/**
   Get gpsSecurityLevel GP cluster attribute of gpcb

   Described in A.3.3.2.6 gpsSecurityLevel attribute.

   @return value of gpsSecurityLevel aыруыруttribtue
  */
zb_uint8_t zb_zgps_get_security_level(void);

/**
   Set gpSharedSecurityKeyType GP cluster attribute

   Described in A.3.3.3.1 gpSharedSecurityKeyType attribute.

   @param key_type Security key type to set (@see zb_zgp_security_key_type_e)
  */
void zb_zgp_set_shared_security_key_type(zb_uint_t key_type);

/**
   Set gpSharedSecurityKey GP cluster attribute

   Described in A.3.3.3.1 gpSharedSecurityKey attribute.

   @param key Security key to set
  */
void zb_zgp_set_shared_security_key(zb_uint8_t *key);

/**
   Set gpsCommissioningExitMode GP cluster attribute

   Described in A.3.3.2.4 gpsCommissioningExitMode attribute.

   @param cem Commissioning exit mode to set (@see zgp_commissioning_exit_mode_t)
  */
void zb_zgps_set_commissioning_exit_mode(zb_uint_t cem);

/**
   Set gpsCommunicationMode GP cluster attribute of gpcb

   Described in A.3.3.2.3 gpsCommunicationMode attribute

   @param mode @ref zgp_communication_mode_t communication mode

   @snippet light_sample/light_coordinator_combo/light_zc.c set_comm_mode
  */
void zb_zgps_set_communication_mode(zgp_communication_mode_t mode);

/**
 * Application function to override translation of 8-bit vector command (generic switch)
 *
 * If this function is not implemented in the application, then ZBOSS
 * performs a default translation as recommended by ZGP spec (see Green Power
 * Basic specification v1.1.1, tables 51, 52). If there is no default
 * translation found, then the received command is dropped.
 *
 * If this function is implemented by the the application, then application is
 * fully responsible for a translation of GPD 8-bit vector commands. For any
 * return code but RET_OK, ZBOSS will stop command processing and drop it.
 *
 * Note: The translation is done to GPDF command ID, not to ZCL command ID.
 *
 * @param[in] vector_8bit_cmd_id incoming command ID: press (0x69) or release(0x6a)
 * @param[in] switch_type switch type of the command's originator (see ZGP spec. A.4.2.1.1.10)
 * @param[in] num_of_contacts number of contacts command's originator provides
 * @param[in] contact_status contacts status from the payload of the received command
 * @param[out] zgp_cmd_out GPDF command ID to which incoming command should be translated
 * @return RET_OK if translation is successful.
 *
 * See Green Power Basic specification v1.1.1, chapters A.3.6.2.2.2, A.4.2.2.1 for more information.
 *
 * @snippet simple_combo/zc_combo.c convert_8bit_vector
 */
zb_ret_t zb_zgp_convert_8bit_vector(zb_uint8_t vector_8bit_cmd_id,      /* press or release cmd */
                                    zb_uint8_t switch_type,             /* see zb_zgpd_switch_type_e */
                                    zb_uint8_t num_of_contacts,
                                    zb_uint8_t contact_status,
                                    zb_uint8_t *zgp_cmd_out);

/**
 * @brief Application function to override allows custom handling incoming raw GPDF packet
 *
 * If this function os not implemented by the application, then ZBOSS
 * drops the buffer without any additional handling.
 *
 * If this function is implemented by the application, the application itself
 * shall carry to free this resource.
 *
 * @param buf_ref
 */
void zb_zgp_gpdf_raw_indication(zb_bufid_t buf_ref);

#ifdef ZB_ENABLE_ZGP_DIRECT
/**
   Set ZBOSS to skip all incoming GPDF.

   To be used for testing only.
   Use that function with ZB_TRUE parameter to prevent Combo device from
   receiving GPDFS thus always working thru Proxy device.

   @param skip if ZB_TRUE, skip incoming GP frames
  */

void       zb_zgp_set_skip_gpdf(zb_uint8_t skip);
zb_uint8_t zb_zgp_get_skip_gpdf(void);
void       zb_zgp_sync_pib(zb_uint8_t param);

#endif  /* ZB_ENABLE_ZGP_DIRECT */

/*! @} */

#ifndef ZB_ZGPD_ROLE

typedef enum zgp_proxy_comm_mode_action_e
{
  ZGP_PROXY_COMM_MODE_LEAVE,
  ZGP_PROXY_COMM_MODE_ENTER
} zgp_proxy_comm_mode_action_t;

typedef enum zgp_proxy_comm_mode_ch_present_e
{
  ZGP_PROXY_COMM_MODE_CHANNEL_PRESENT,
  ZGP_PROXY_COMM_MODE_CHANNEL_NOT_PRESENT
} zgp_proxy_comm_mode_ch_present_t;

typedef enum zgp_proxy_comm_mode_communication_e
{
  ZGP_PROXY_COMM_MODE_BROADCAST,
  ZGP_PROXY_COMM_MODE_UNICAST
} zgp_proxy_comm_mode_communication_t;

/**
 * @brief Setup options for zb_zgp_cluster_proxy_commissioning_mode_req()
 *
 * @param action        [in] enter/exit (see @ref zgp_proxy_comm_mode_action_t)
 * @param exit_mode     [in] exit mode (see @ref zgp_commissioning_exit_mode_t)
 * @param ch_present    [in] is channel present (see @ref zgp_proxy_comm_mode_ch_present_t)
 * @param communication [in] communication type (see @ref zgp_proxy_comm_mode_communication_t)
 */
#define ZB_ZGP_FILL_PROXY_COMM_MODE_OPTIONS(action, exit_mode, ch_present, communication) \
  ((!!(action)) | (((exit_mode) & 7) << 1) | ((!!(ch_present)) << 4) | (((communication)&3) << 5))

/**
 * @brief Perform Proxy Commissioning mode request
 *
 * @param param      [in]  Buffer reference
 * @param options    [in]  Request options (see @ref ZB_ZGP_FILL_PROXY_COMM_MODE_OPTIONS)
 * @param comm_wind  [in]  Commissioning window
 * @param channel    [in]  Channel where perform commissioning (0x0b for channel 11)
 * @param cb         [in]  Callback call if needed after send request
 *
 * @see ZGP spec, A.3.3.5.3
*/
void zb_zgp_cluster_proxy_commissioning_mode_req(zb_uint8_t    param,
                                                 zb_uint8_t    options,
                                                 zb_uint16_t   comm_wind,
                                                 zb_uint8_t    channel,
                                                 zb_callback_t cb);

/**
 * @brief Perform Proxy Commissioning mode enter request
 *
 * @param param      [in]  Buffer reference
 * @param exit_mode  [in] exit mode (see @ref zgp_commissioning_exit_mode_t)
 * @param comm_wind  [in]  Commissioning window
 * @param cb         [in]  Callback call if needed after send request
 *
 * @see ZGP spec, A.3.3.5.3
 */
void zgp_cluster_send_proxy_commissioning_mode_enter_req(zb_uint8_t    param,
                                                         zb_uint8_t    exit_mode,
                                                         zb_uint16_t   comm_window,
                                                         zb_callback_t cb);

/**
 * @brief Perform Proxy Commissioning mode leave request
 *
 * @param param      [in]  Buffer reference
 * @param cb         [in]  Callback call if needed after send request
 *
 * @see ZGP spec, A.3.3.5.3
 */
void zgp_cluster_send_proxy_commissioning_mode_leave_req(zb_uint8_t param, zb_callback_t cb);

/**
 * @brief Setup options for zb_zgp_cluster_gp_pairing_req()
 *
 * @param app_id               [in]  ZGPD application id (@see zb_zgp_app_id_t)
 * @param add_sink             [in]  Add sink flag
 * @param remove_gpd           [in]  Remove GPD flag
 * @param comm_mode            [in]  Communication mode (@see zgp_communication_mode_t)
 * @param gpd_fixed            [in]  ZGPD is fixed
 * @param gpd_mac_sec_num_cap  [in]  MAC sequence number capability
 * @param sec_lvl              [in]  SecurityLevel (@see zb_zgp_security_level_e)
 * @param sec_key_type         [in]  SecurityKeyType (@see zb_zgp_security_key_type_e)
 * @param gpd_sec_key_prsnt    [in]  Is key presented
 * @param assigned_alias_prsnt [in]  Is assigned alias presented
 * @param frwrd_radius_prsnt   [in]  Is radius in the groupcast forwarding presented
 *
 * @see ZGP spec, A.3.3.5.2
 */
#define ZB_ZGP_FILL_GP_PAIRING_OPTIONS(app_id, add_sink, remove_gpd, comm_mode,\
                                       gpd_fixed, gpd_mac_sec_num_cap, sec_lvl,\
                                       sec_key_type, frame_cnt_prsnt,\
                                       gpd_sec_key_prsnt, assigned_alias_prsnt, \
                                       frwrd_radius_prsnt)\
  (((app_id) & 7) | ((!!(add_sink)) << 3) | ((!!(remove_gpd)) << 4) | (((comm_mode) & 3) << 5) | (((!!(add_sink)) ? (!!(gpd_fixed)) : 0) << 7) | (((!!(add_sink)) ? (!!(gpd_mac_sec_num_cap)) : 0) << 8) | (((!!(add_sink)) ? ((sec_lvl) & 3) : 0) << 9) | (((!!(add_sink)) ? ((sec_key_type) & 7) : 0) << 11) | ((!!(frame_cnt_prsnt)) << 14) | (((!!(add_sink)) ? (!!(gpd_sec_key_prsnt)) : 0) << 15) | (((!!(add_sink)) ? (!!(assigned_alias_prsnt)) : 0) << 16) | (((!!(add_sink)) ? (!!(frwrd_radius_prsnt)) : 0) << 17))

/**
 * @brief Perform GP Pairing request
 *
 * @param param              [in]  Buffer reference
 * @param dst_addr           [in]  Destination address
 * @param dst_addr_mode      [in]  Destination address mode
 * @param options            [in]  Request options
 * @param gpd_id             [in]  ZGPD ID
 * @param endpoint           [in]  Endpoint of ZGPD if APP_ID eq 0010
 * @param dev_id             [in]  Device ID
 * @param sec_frame_counter  [in]  Security frame counter of ZGPD
 * @param key                [in]  Pointer to the security key
 * @param assigned_alias     [in]  Assigned alias in the groupcast forwarding of the GPDF packet
 * @param frwd_radius        [in]  Radius in the groupcast forwarding of the GPDF packet
 * @param cb                 [in]  Callback call if needed after send request
 *
 * @see ZGP spec, A.3.3.5.2
 */
void zb_zgp_cluster_gp_pairing_req(zb_uint8_t     param,
                                   zb_uint16_t    dst_addr,
                                   zb_uint8_t     dst_addr_mode,
                                   zb_uint32_t    options,
                                   zb_zgpd_addr_t gpd_id,
                                   zb_uint8_t     endpoint,
                                   zb_uint8_t     dev_id,
                                   zb_uint32_t    sec_frame_counter,
                                   zb_uint8_t    *key,
                                   zb_uint16_t    assigned_alias,
                                   zb_uint8_t     frwd_radius,
                                   zb_uint16_t    group_id,
                                   zb_callback_t  cb);

/**
 * @brief Perform send zcl gp sink table request for ZGP cluster
 *
 * @param buf_ref        [in]  Buffer reference
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param options        [in]  Request options
 * @param zgpd_id        [in]  Pointer to ZGPD ID
 * @param index          [in]  The Index field carries the index value of the Sink Table entry is requested
 * @param cb             [in]  Call callback if needed after sending request
 *
 * @see ZGP spec, A.3.3.4.7
 *
 */
void zgp_cluster_send_gp_sink_table_request(zb_uint8_t    buf_ref,
                                            zb_uint16_t   dst_addr,
                                            zb_uint8_t    dst_addr_mode,
                                            zb_uint8_t    options,
                                            zb_zgpd_id_t *zgpd_id,
                                            zb_uint8_t    index,
                                            zb_callback_t cb);

/**
 * @brief Perform send zcl gp proxy table request for ZGP cluster
 *
 * @param buf_ref        [in]  Buffer reference
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param options        [in]  Request options
 * @param zgpd_id        [in]  Pointer to ZGPD ID
 * @param index          [in]  The Index field carries the index value of the Proxy Table entry is requested
 * @param cb             [in]  Call callback if needed after sending request
 *
 * @see ZGP spec, A.3.4.3.1
 *
 */
void zgp_cluster_send_gp_proxy_table_request(zb_uint8_t    buf_ref,
                                             zb_uint16_t   dst_addr,
                                             zb_uint8_t    dst_addr_mode,
                                             zb_uint8_t    options,
                                             zb_zgpd_id_t *zgpd_id,
                                             zb_uint8_t    index,
                                             zb_callback_t cb);

typedef ZB_PACKED_PRE struct zgp_pair_group_list_s
{
  zb_uint16_t sink_group;
  zb_uint16_t alias;
}
ZB_PACKED_STRUCT zgp_pair_group_list_t;

/* >> Data structures for Application Description */
typedef enum zgp_app_descr_status_e
{
  ZGP_APP_TBL_ENT_STATUS_FREE              = 0,
  ZGP_APP_TBL_ENT_STATUS_INIT_WITH_SW_INFO = 1,
  ZGP_APP_TBL_ENT_STATUS_INIT              = 2,
  ZGP_APP_TBL_ENT_STATUS_APP_DESCR_PROCESS = 3,
  ZGP_APP_TBL_ENT_STATUS_COMPLETE          = 4
} zgp_app_descr_status_t;

typedef ZB_PACKED_PRE struct zgp_app_tbl_ent_options_s
{
  zb_bitfield_t ieee_addr_present:1;
  zb_bitfield_t switch_info_present:1;
  zb_bitfield_t reserved:6;
}ZB_PACKED_STRUCT zgp_app_tbl_ent_options_t;

typedef ZB_PACKED_PRE struct zgp_app_tbl_ent_info_s
{
  zb_zgpd_addr_t                addr;
  zgp_app_tbl_ent_options_t     options;
  zb_uint8_t                    total_reports_num;
  zb_gpdf_comm_switch_gen_cfg_t switch_info_configuration;
  zb_uint8_t                    reserved;
}ZB_PACKED_STRUCT zgp_app_tbl_ent_info_t;

typedef ZB_PACKED_PRE struct zgp_app_tbl_ent_s
{
  zgp_app_tbl_ent_info_t info;
  zgp_report_desc_t      reports[ZB_ZGP_APP_DESCR_REPORTS_NUM];
}ZB_PACKED_STRUCT zgp_app_tbl_ent_t;

typedef struct zgp_runtime_app_tbl_ent_s
{
  zb_uint8_t status; /* zgp_app_descr_status_t */
  zb_uint8_t receive_reports[ZB_SIZE_OF_BIT_VECTOR(ZB_ZGP_APP_DESCR_REPORTS_NUM)];
  zb_uint8_t reply_buf;
  zb_bool_t  need_reply;
  zgp_app_tbl_ent_t base;
}zgp_runtime_app_tbl_ent_t;

/* << Data structures for Application Description */

typedef struct zgp_tbl_ent_s
{
  zb_zgpd_addr_t   zgpd_id;             /**< ID of the paired ZGPD */
  zb_uint16_t      options;             /**< The options for the reception from ZGPD */
  zb_uint16_t      zgpd_assigned_alias; /**< The commissioned 16-bit ID to be used
                                          as alias for ZGPD */

  zb_uint32_t      security_counter; /**< The incoming security frame counter for ZGPD */
  zb_uint8_t       zgpd_key[ZB_CCM_KEY_SIZE]; /**< Security key for the GPD */
  zb_uint8_t       endpoint;                  /**< Endpoint pair of IEEE:EP if App ID is 010.  */
  zb_uint8_t       sec_options;               /**< Security options */
  zb_uint8_t       groupcast_radius;    /**< To limit the range of the groupcast */

  zb_uint8_t       is_sink;

  union
  {
    struct zgp_proxy_tbl_ent_s
    {
      zb_uint16_t ext_options;
      struct zgp_lwsink_addr_list_s
      {
        zb_address_ieee_ref_t addr_ref;
      } lwsaddr[ZB_ZGP_MAX_LW_UNICAST_ADDR_PER_GPD];
      zgp_pair_group_list_t sgrp[ZB_ZGP_MAX_SINK_GROUP_PER_GPD];
    } proxy;
    struct zgp_sink_tbl_ent_s
    {
      zb_uint8_t       device_id;           /**< ZGPD Device ID from Commissioning frame @see zb_zgpd_dev_id_t */
      zgp_pair_group_list_t sgrp[ZB_ZGP_MAX_SINK_GROUP_PER_GPD];
      zb_uint8_t match_dev_tbl_idx; /**< index in matching table matched by device_id or app_info.manuf_model_id  */
      /**
       * Extension to the table (field is not presented in specification).
       *
       * Dest IEEE address to use in GPT to GPD packets even if GPD is identified by SrcID
       */
      zb_ieee_addr_t   ieee_addr;
    } sink;
  } u;
} zgp_tbl_ent_t;

#if defined ZB_ENABLE_ZGP_SINK || defined ZGP_COMMISSIONING_TOOL
typedef zgp_tbl_ent_t zb_zgp_sink_tbl_ent_t;
#endif  /* defined ZB_ENABLE_ZGP_SINK || defined ZGP_COMMISSIONING_TOOL */
#ifdef ZB_ENABLE_ZGP_PROXY
typedef zgp_tbl_ent_t zb_zgp_proxy_tbl_ent_t;
#endif  /* ZB_ENABLE_ZGP_PROXY */

#if defined ZB_ENABLE_ZGP_SINK || defined  ZGP_COMMISSIONING_TOOL
/* TODO: update description */
/**
 * @brief Perform send zcl gp pairing configuration command for ZGP cluster
 *
 * @param buf_ref        [in]  Buffer reference
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param actions        [in]  Actions
 * @param ent            [in]  Sink table entry
 * @param def_resp       [in]  Enable ZCL default response if TRUE
 * @param cb             [in]  Call callback if needed after sending request
 *
 * @see ZGP spec, A.3.3.4.6
 *
 */
void zgp_cluster_send_pairing_configuration(zb_uint8_t             buf_ref,
                                            zb_uint16_t            dst_addr,
                                            zb_uint8_t             dst_addr_mode,
                                            zb_uint8_t             actions,
                                            zb_zgp_sink_tbl_ent_t *ent,
                                            zb_uint8_t             num_paired_endpoints,
                                            zb_uint8_t            *paired_endpoints,
                                            zb_uint8_t             app_info,
                                            zb_uint16_t            manuf_id,
                                            zb_uint16_t            model_id,
                                            zb_uint8_t             num_gpd_commands,
                                            zb_uint8_t            *gpd_commands,
                                            zb_zgp_cluster_list_t *cluster_list,
                                            zb_gpdf_comm_switch_info_t *switch_info,
                                            zb_callback_t          cb);

/**
 * @brief Perform send zcl gp pairing configuration command with action = 0b101 (Application Description)
 *
 * @param buf_ref               [in]  Buffer reference
 * @param dst_addr              [in]  Destination address
 * @param dst_addr_mode         [in]  Destination address mode
 * @param actions               [in]  Actions
 * @param ent                   [in]  Sink table entry
 * @param app_tbl_ent           [in]  App table entry
 * @param num_paired_endpoints  [in]  Number if paired endpoints
 * @param paired_endpoints      [in]  A list of endpoints corresponding to num_paired_endpoints value
 * @param report_idx            [in]  Index of report descriptor in @ref zgp_runtime_app_tbl_ent_t
 * @param cb                    [in]  Call callback if needed after sending request
 *
 * @see ZGP spec, A.3.3.4.6
 *
 */
void zgp_cluster_send_pairing_configuration_app_descr(zb_uint8_t buf_ref,
                                                      zb_uint16_t dst_addr,
                                                      zb_uint8_t dst_addr_mode,
                                                      zb_uint8_t actions,
                                                      zb_uint8_t app_info,
                                                      zb_zgp_sink_tbl_ent_t *ent,
                                                      zgp_runtime_app_tbl_ent_t *app_tbl_ent,
                                                      zb_uint8_t num_paired_endpoints,
                                                      zb_uint8_t *paired_endpoints,
                                                      zb_uint8_t report_idx,
                                                      zb_callback_t cb);
#endif  /* defined ZB_ENABLE_ZGP_SINK || defined  ZGP_COMMISSIONING_TOOL */

#ifdef ZGP_COMMISSIONING_TOOL
/**
 * @brief Perform send zcl gp sink commissioning mode command for ZGP cluster
 *
 * @param buf_ref        [in]  Buffer reference
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param options        [in]  Options
 * @param endpoint       [in]  Endpoint
 * @param cb             [in]  Call callback if needed after sending request
 *
 * @see ZGP spec, A.3.3.4.8
 *
 */
void zgp_cluster_send_gp_sink_commissioning_mode(zb_uint8_t    buf_ref,
                                                 zb_uint16_t   dst_addr,
                                                 zb_uint8_t    dst_addr_mode,
                                                 zb_uint8_t    options,
                                                 zb_uint8_t    endpoint,
                                                 zb_callback_t cb);
#endif  /* ZGP_COMMISSIONING_TOOL */

void zgp_gp_set_shared_security_key_type(enum zb_zgp_security_key_type_e type);
#define ZGP_GP_SET_SHARED_SECURITY_KEY_TYPE(type) zgp_gp_set_shared_security_key_type(type)

#ifdef ZGP_CLUSTER_TEST
/* Application specific zcl command handler for test purposes */
typedef zb_uint8_t (*zgp_cluster_app_zcl_cmd_handler_t)(zb_uint8_t buf_ref);

/**
 * @brief Setup specific application zcl command handler for test purpose
 *
 * @param handler [in]  Specific application zcl command handler pointer
 */
void zgp_cluster_set_app_zcl_cmd_handler(zgp_cluster_app_zcl_cmd_handler_t handler);
#endif  /* ZGP_CLUSTER_TEST */
#endif  /* !ZB_ZGPD_ROLE */

#endif /* ZBOSS_API_ZGP_H */
