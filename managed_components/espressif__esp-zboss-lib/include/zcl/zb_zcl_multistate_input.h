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
/* PURPOSE: Multistate Input cluster definitions
*/

#ifndef ZB_ZCL_MULTISTATE_INPUT_H
#define ZB_ZCL_MULTISTATE_INPUT_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_MULTISTATE_INPUT
 *  @{
 *    @details
 *    The Multistate Input cluster provides an interface for reading the value
 *    of a multistate measurement and accessing various characteristics of that
 *    measurement. No cluster specific commands are received or generated.
 */

/* Cluster ZB_ZCL_CLUSTER_ID_MULTI_INPUT */

/** @defgroup ZB_ZCL_MULTISTATE_INPUT_ATTRS_GROUP Multistate Input cluster attributes
 *  @{
 */

/** @brief Multistate Input cluster attribute identifiers
 *  @see ZCL spec, Multistate Input (Basic) Cluster 3.14.8.4.2
 */
enum zb_zcl_multi_input_attr_e
{
  /** @brief StateText attribute, ZCL spec 3.14.11.18 */
  ZB_ZCL_ATTR_MULTI_INPUT_STATE_TEXT_ID        = 0x000e,
  /** @brief Description attribute, ZCL spec 3.14.11.4 */
  ZB_ZCL_ATTR_MULTI_INPUT_DESCRIPTION_ID       = 0x001c,
  /** @brief NumberOfStates attribute, ZCL spec 3.14.11.17 */
  ZB_ZCL_ATTR_MULTI_INPUT_NUM_OF_STATES_ID     = 0x004a,
  /** @brief OutOfService attribute, ZCL spec 3.14.11.1 */
  ZB_ZCL_ATTR_MULTI_INPUT_OUT_OF_SERVICE_ID    = 0x0051,
  /** @brief PresentValue attribute, ZCL spec 3.14.11.2 */
  ZB_ZCL_ATTR_MULTI_INPUT_PRESENT_VALUE_ID     = 0x0055,
  /** @brief Reliability attribute, ZCL spec 3.14.11.9 */
  ZB_ZCL_ATTR_MULTI_INPUT_RELIABILITY_ID       = 0x0067,
  /** @brief StatusFlags attribute, ZCL spec 3.14.11.3 */
  ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID      = 0x006f,
  /** @brief ApplicationType attribute, ZCL spec 3.14.11.19 */
  ZB_ZCL_ATTR_MULTI_INPUT_APPLICATION_TYPE_ID  = 0x0100,
};

/**
 *  @brief StatusFlags attribute values.
 *  @see ZCL spec 3.14.11.3 for details.
 */
enum zb_zcl_multi_input_status_flags_value_e
{
  ZB_ZCL_MULTI_INPUT_STATUS_FLAG_NORMAL         = 0x00,  /**< Normal (default) state. */
  ZB_ZCL_MULTI_INPUT_STATUS_FLAG_IN_ALARM       = 0x01,  /**< In alarm bit. */
  ZB_ZCL_MULTI_INPUT_STATUS_FLAG_FAULT          = 0x02,  /**< Fault bit. */
  ZB_ZCL_MULTI_INPUT_STATUS_FLAG_OVERRIDDEN     = 0x04,  /**< Overridden bit. */
  ZB_ZCL_MULTI_INPUT_STATUS_FLAG_OUT_OF_SERVICE = 0x08,  /**< Out of service bit. */
};

/** @brief Reliability attribute values.
 *  @see ZCL spec 3.14.11.9 for details.
 */
enum zb_zcl_multi_input_reliability_value_e
{
  ZB_ZCL_MULTI_INPUT_RELIABILITY_NO_FAULT_DETECTED   = 0x00,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_NO_SENSOR           = 0x01,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_OVER_RANGE          = 0x02,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_UNDER_RANGE         = 0x03,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_OPEN_LOOP           = 0x04,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_SHORTED_LOOP        = 0x05,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_NO_OUTPUT           = 0x06,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_UNRELIABLE_OTHER    = 0x07,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_PROCESS_ERROR       = 0x08,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_MULTI_STATE_FAULT   = 0x09,
  ZB_ZCL_MULTI_INPUT_RELIABILITY_CONFIGURATION_ERROR = 0x0a,
};

/** @defgroup ZB_ZCL_MULTISTATE_INPUT_APPLICATION_TYPES Multistate Input cluster application types
 *  @{
 *    @details
 *      The ApplicationType attribute indicates the specific application usage
 *      for the cluster. ApplicationType is subdivided into Group, Type and
 *      an Index number, as follows:
 *
 *      Group = Bits 24 to 31
 *        An indication of the cluster this attribute is part of. Multistate Input
 *        clusters have Group = 0x0d.
 *
 *      Type = Bits 16 to 23
 *        The physical quantity that the PresentValue attribute of the cluster
 *        represents.
 *
 *      Index = Bits 0 to 15
 *        The specific application usage of the cluster.
 */

#define ZB_ZCL_MI_GROUP_ID 0x0d
/* TODO: Consider adding common macro for all application types */
#define ZB_ZCL_MI_SET_APP_TYPE_WITH_ID(_type, _id) ((ZB_ZCL_MI_GROUP_ID << 24) | ((_type & 0xff) << 16) | (_id & 0xffff))

/** @brief Type values for Multistate Input cluster applications
 *  @see ZCL spec 3.14.11.19.7
 */
enum zb_zcl_mi_application_types_e
{
  ZB_ZCL_MI_APP_TYPE_APP_DOMAIN_HVAC,
  /* Types 0x01 to 0xfe are reserved */
  ZB_ZCL_MI_APP_TYPE_OTHER = 0xff
};

/** @defgroup ZB_ZCL_MI_APP_DOMAIN_HVAC_TYPE Multistate Input cluster representing HVAC domain
 *  @{
 */

/** @brief Values for 'Application Domain HVAC' type of Multistate Input cluster
 *  @see ZCL spec 3.14.11.19.7.1
 *  @note
 *    This application domain is specific for Heating, Ventilation, Air
 *    Conditioning (HVAC) type of applications and the enumeration lists possible
 *    application usage states.
 */
enum zb_zcl_mi_app_domain_hvac_e
{
  /** @brief On, Off, Auto states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_0 = ZB_ZCL_MI_SET_APP_TYPE_WITH_ID(ZB_ZCL_MI_APP_TYPE_APP_DOMAIN_HVAC, 0x0000),
  /** @brief Off, Low, Medium, High states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_1,
  /** @brief Auto, Heat, Cool, Off, Emergency Heat, Fan Only, Max Heat states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_2,
  /** @brief Occupied, Unoccupied, Standby, Bypass states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_3,
  /** @brief Inactive, Active, Hold states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_4,
  /** @brief
   *   Auto, Warm-up, Water Flush, Autocalibration, Shutdown Open, Shutdown
   *   Closed, Low Limit, Test and Balance states
   */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_5,
  /** @brief Off, Auto, Heat Control, Heat Only, Cool Only, Fan Only states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_6,
  /** @brief High, Normal, Low states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_7,
  /** @brief Occupied, Unoccupied, Startup, Shutdown states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_8,
  /** @brief Night, Day, Hold states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_9,
  /** @brief Off, Cool, Heat, Auto, Emergency Heat states */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_10,
  /** @brief
   *   Shutdown Closed, Shutdown Open, Satisfied, Mixing, Cooling, Heating,
   *   Supplemental Heat states
   */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_STATES_GROUP_11,
  /* 0x0200 to 0xfffe are Vendor defined */
  ZB_ZCL_MI_APP_DOMAIN_HVAC_OTHER = ZB_ZCL_MI_SET_APP_TYPE_WITH_ID(ZB_ZCL_MI_APP_TYPE_APP_DOMAIN_HVAC, 0xffff)
};

/** @} */ /* end of ZB_ZCL_MI_APP_DOMAIN_HVAC_TYPE group */

/** @} */ /* end of ZB_ZCL_MULTISTATE_INPUT_APPLICATION_TYPES */

/** @brief Default value for Multistate Input cluster revision global attribute */
#define ZB_ZCL_MULTI_INPUT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Default value for Description attribute */
#define ZB_ZCL_MULTI_INPUT_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for NumberOfStates attribute */
#define ZB_ZCL_MULTI_INPUT_NUM_OF_STATES_DEFAULT_VALUE ((zb_uint16_t)0x0000u)

/** @brief Default value for OutOfService attribute */
#define ZB_ZCL_MULTI_INPUT_OUT_OF_SERVICE_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for Reliability attribute */
#define ZB_ZCL_MULTI_INPUT_RELIABILITY_DEFAULT_VALUE ZB_ZCL_MULTI_INPUT_RELIABILITY_NO_FAULT_DETECTED

/** @brief Default value for StatusFlags attribute */
#define ZB_ZCL_MULTI_INPUT_STATUS_FLAGS_DEFAULT_VALUE ZB_ZCL_MULTI_INPUT_STATUS_FLAG_NORMAL

/** @brief StatusFlags attribute minimum value */
#define ZB_ZCL_MULTI_INPUT_STATUS_FLAGS_MIN_VALUE 0

/** @brief StatusFlags attribute maximum value */
#define ZB_ZCL_MULTI_INPUT_STATUS_FLAGS_MAX_VALUE 0x0f

/** @cond internals_doc */
/** @name Multistate Input cluster internals
 *  Internal structures for Multistate Input cluster
 *  @{
 */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_MULTI_INPUT_DESCRIPTION_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_MULTI_INPUT_DESCRIPTION_ID,                           \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_MULTI_INPUT_NUM_OF_STATES_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_MULTI_INPUT_NUM_OF_STATES_ID,                         \
  ZB_ZCL_ATTR_TYPE_U16,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_MULTI_INPUT_OUT_OF_SERVICE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_MULTI_INPUT_OUT_OF_SERVICE_ID,                        \
  ZB_ZCL_ATTR_TYPE_BOOL,                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_MULTI_INPUT_PRESENT_VALUE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_MULTI_INPUT_PRESENT_VALUE_ID,                         \
  ZB_ZCL_ATTR_TYPE_U16,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_MULTI_INPUT_RELIABILITY_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_MULTI_INPUT_RELIABILITY_ID,                           \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID,                          \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_MULTI_INPUT_APPLICATION_TYPE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_MULTI_INPUT_APPLICATION_TYPE_ID,                      \
  ZB_ZCL_ATTR_TYPE_U32,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

/** Number of attributes mandatory for reporting in Multistate Input cluster */
#define ZB_ZCL_MULTI_INPUT_REPORT_ATTR_COUNT 0

/** @} */
/** @endcond */ /* Multistate Input cluster internals */

/** @brief Declare attribute list for Multistate Input cluster
 *  @param attr_list - attribute list name
 *  @param description - pointer to variable storing Description attribute value
 *  @param num_of_states - pointer to variable storing NumberOfStates attribute value
 *  @param out_of_service - pointer to variable storing OutOfService attribute value
 *  @param present_value -  pointer to variable storing PresentValue attribute value
 *  @param reliability - pointer to variable storing Reliability attribute value
 *  @param status_flags - pointer to variable storing StatusFlags attribute value
 *  @param app_type - pointer to variable storing ApplicationType attribute value
 */
#define ZB_ZCL_DECLARE_MULTI_INPUT_ATTRIB_LIST(                                           \
    attr_list,                                                                            \
    description,                                                                          \
    num_of_states,                                                                        \
    out_of_service,                                                                       \
    present_value,                                                                        \
    reliability,                                                                          \
    status_flags,                                                                         \
    app_type)                                                                             \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_MULTI_INPUT)        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_MULTI_INPUT_DESCRIPTION_ID, (description))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_MULTI_INPUT_NUM_OF_STATES_ID, (num_of_states))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_MULTI_INPUT_OUT_OF_SERVICE_ID, (out_of_service))       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_MULTI_INPUT_PRESENT_VALUE_ID, (present_value))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_MULTI_INPUT_RELIABILITY_ID, (reliability))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID, (status_flags))           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_MULTI_INPUT_APPLICATION_TYPE_ID, (app_type))           \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* end of ZB_ZCL_MULTISTATE_INPUT_ATTRS_GROUP group */

/** @defgroup ZB_ZCL_MULTISTATE_INPUT_ATTR_API Multistate Input cluster attribute value manipulation API
 *  @{
 */

/** @brief Set normal operating mode
 *  @param ep - endpoint number
 */
#define ZB_ZCL_MULTI_INPUT_SET_NORMAL_MODE(ep)                           \
{                                                                        \
  zb_uint8_t val;                                                        \
                                                                         \
  val = ZB_FALSE;                                                        \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                       ZB_ZCL_ATTR_MULTI_INPUT_OUT_OF_SERVICE_ID,        \
                       &val, ZB_FALSE);                                  \
                                                                         \
  val = ZB_ZCL_MULTI_INPUT_STATUS_FLAG_NORMAL;                           \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                       ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID,          \
                       &val, ZB_FALSE);                                  \
}

/** @brief Set Out of service operating mode
 *  @param ep - endpoint number
 */
#define ZB_ZCL_MULTI_INPUT_SET_OUT_OF_SERVICE(ep)                                          \
{                                                                                          \
  zb_zcl_attr_t *attr_desc;                                                                \
  zb_uint8_t val;                                                                          \
                                                                                           \
  val = ZB_TRUE;                                                                           \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                                  \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                                         \
                       ZB_ZCL_ATTR_MULTI_INPUT_OUT_OF_SERVICE_ID,                          \
                       &val, ZB_FALSE);                                                    \
                                                                                           \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                    \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
                                     ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID);             \
  if (attr_desc)                                                                           \
  {                                                                                        \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_MULTI_INPUT_STATUS_FLAG_OUT_OF_SERVICE; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                                \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
                         ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID,                          \
                         &val, ZB_FALSE);                                                  \
  }                                                                                        \
}

/** @brief Set overridden operating mode
 * @param ep - endpoint number
 */
#define ZB_ZCL_MULTI_INPUT_SET_OVERRIDDEN_MODE(ep)                                    \
{                                                                                     \
  zb_zcl_attr_t *attr_desc;                                                           \
  zb_uint8_t val;                                                                     \
                                                                                      \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                      \
                                     ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID);        \
  if (attr_desc)                                                                      \
  {                                                                                   \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_MULTI_INPUT_STATUS_FLAG_OVERRIDDEN;\
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
                         ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID,                     \
                         &val, ZB_FALSE);                                             \
  }                                                                                   \
}

/** @brief Set fault status flag
 * @param ep - endpoint number
 */
#define ZB_ZCL_MULTI_INPUT_SET_FAULT_STATUS(ep)                                       \
{                                                                                     \
  zb_zcl_attr_t *attr_desc;                                                           \
  zb_uint8_t val;                                                                     \
                                                                                      \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                      \
                                     ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID);        \
  if (attr_desc)                                                                      \
  {                                                                                   \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_MULTI_INPUT_STATUS_FLAG_FAULT;     \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
                         ZB_ZCL_ATTR_MULTI_INPUT_STATUS_FLAGS_ID,                     \
                         &val, ZB_FALSE);                                             \
  }                                                                                   \
}

/** @} */ /* end of ZB_ZCL_MULTISTATE_INPUT_ATTR_API group */

/** @} */ /* ZCL Multistate Input cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_multi_input_init_server(void);
void zb_zcl_multi_input_init_client(void);
#define ZB_ZCL_CLUSTER_ID_MULTI_INPUT_SERVER_ROLE_INIT zb_zcl_multi_input_init_server
#define ZB_ZCL_CLUSTER_ID_MULTI_INPUT_CLIENT_ROLE_INIT zb_zcl_multi_input_init_client

#endif /* ZB_ZCL_MULTISTATE_INPUT_H */
