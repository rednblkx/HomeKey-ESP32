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
/* PURPOSE: Binary Input cluster
*/

#ifndef ZB_ZCL_BINARY_INPUT_H
#define ZB_ZCL_BINARY_INPUT_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_BINARY_INPUT
 *  @{
 *    @details
 *    According to ZCL spec, subclause 3.14.4, Binary Input (Basic) cluster has no cluster-specific
 *    commands. Cluster attributes could be queried with
 *    @ref ZB_ZCL_COMMANDS "general ZCL commands".
 *
 *
 */

/* ZB_ZCL_CLUSTER_ID_BINARY_INPUT = 0x000f defined in zb_zcl_common.h ZCL spec 3.14.4 */

/*! @name Binary Input cluster attributes
    @{
*/

/** @brief Binary Input cluster attribute identifiers. */
enum zb_zcl_binary_input_attr_e
{
  /** This attribute, of type Character string, MAY be used to hold a human readable
   *  description of the ACTIVE state of a binary PresentValue. */
  ZB_ZCL_ATTR_BINARY_INPUT_ACTIVE_TEXT_ID               = 0x0004,
  /** The Description attribute, of type Character string, MAY be used to hold a
   *  description of the usage of the input, output or value, as appropriate
   *  to the cluster. */
  ZB_ZCL_ATTR_BINARY_INPUT_DESCRIPTION_ID               = 0x001C,
  /** This attribute, of type Character string, MAY be used to hold a human readable
   *  description of the INACTIVE state of a binary PresentValue. */
  ZB_ZCL_ATTR_BINARY_INPUT_INACTIVE_TEXT_ID             = 0x002E,

  /** @brief OutOfService attribute */
  ZB_ZCL_ATTR_BINARY_INPUT_OUT_OF_SERVICE_ID            = 0x0051,
  /** This attribute, of type enumeration, indicates the relationship between
   *  the physical state of the input (or output as appropriate for the cluster)
   *  and the logical state represented by a binary PresentValue attribute,
   *  when OutOfService is FALSE. */
  ZB_ZCL_ATTR_BINARY_INPUT_POLARITY_ID                  = 0x0054,

  /** @brief PresentValue attribute */
  ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID             = 0x0055,

// adding GP specific attributes
  ZB_ZCL_ATTR_GP_ATTR_BINARY_INPUT_BATTERY_VALUE_ID     = 0x0056,
  ZB_ZCL_ATTR_GP_ATTR_BINARY_INPUT_TEMPERATURE_VALUE_ID = 0x0057,
  ZB_ZCL_ATTR_GP_ATTR_BINARY_INPUT_TAMPER_VALUE_ID      = 0x0058,
// end GP specific attributes
  /** The Reliability attribute, of type 8-bit enumeration, provides an indication
   *  of whether the PresentValue or the operation of the physical input,
   *  output or value in question (as appropriate for the cluster) is reliable
   *  as far as can be determined and, if not, why not. */
  ZB_ZCL_ATTR_BINARY_INPUT_RELIABILITY_ID               = 0x0067,

  /** @brief StatusFlag attribute */
  ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID               = 0x006F,
  /** The ApplicationType attribute is an unsigned 32-bit integer that indicates
   *  the specific application usage for this cluster. */
  ZB_ZCL_ATTR_BINARY_INPUT_APPLICATION_TYPE_ID          = 0x0100,
};

/**
 *  @brief StatusFlag attribute values.
 *  @see ZCL spec 3.14.10.3.
 */
enum zb_zcl_binary_input_status_flag_value_e
{
  ZB_ZCL_BINARY_INPUT_STATUS_FLAG_NORMAL    = 0x00,       /**< Normal (default) state. */
  ZB_ZCL_BINARY_INPUT_STATUS_FLAG_IN_ALARM  = 0x01,       /**< In alarm bit. */
  ZB_ZCL_BINARY_INPUT_STATUS_FLAG_FAULT     = 0x02,       /**< Fault bit. */
  ZB_ZCL_BINARY_INPUT_STATUS_FLAG_OVERRIDEN = 0x04,       /**< Overridden bit. */
  ZB_ZCL_BINARY_INPUT_STATUS_FLAG_OUT_OF_SERVICE = 0x08,  /**< Out of service bit. */
};

/** @brief Default value for Binary Input cluster revision global attribute */
#define ZB_ZCL_BINARY_INPUT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Default value for ActiveText attribute */
#define ZB_ZCL_BINARY_INPUT_ACTIVE_TEXT_DEFAULT_VALUE {0}

/** @brief Default value for Description attribute */
#define ZB_ZCL_BINARY_INPUT_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for InactiveText attribute */
#define ZB_ZCL_BINARY_INPUT_INACTIVE_TEXT_DEFAULT_VALUE {0}

/*! @brief OutOfService attribute default value */
#define ZB_ZCL_BINARY_INPUT_OUT_OF_SERVICE_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for Polarity attribute */
#define ZB_ZCL_BINARY_INPUT_POLARITY_DEFAULT_VALUE ((zb_uint8_t)0)

/** @brief Default value for Reliability attribute */
#define ZB_ZCL_BINARY_INPUT_RELIABILITY_DEFAULT_VALUE ((zb_uint8_t)0x00)

/*! @brief StatusFlag attribute default value */
#define ZB_ZCL_BINARY_INPUT_STATUS_FLAG_DEFAULT_VALUE ZB_ZCL_BINARY_INPUT_STATUS_FLAG_NORMAL

/*! @brief StatusFlag attribute minimum value */
#define ZB_ZCL_BINARY_INPUT_STATUS_FLAG_MIN_VALUE 0

/*! @brief StatusFlag attribute maximum value */
#define ZB_ZCL_BINARY_INPUT_STATUS_FLAG_MAX_VALUE 0x0F

/** @brief Declare attribute list for Binary Input cluster
    @param attr_list - attribute list name
    @param out_of_service - pointer to variable to store OutOfService attribute value
    @param present_value -  pointer to variable to store PresentValue attribute value
    @param status_flag -  pointer to variable to store StatusFlag attribute value
*/
#define ZB_ZCL_DECLARE_BINARY_INPUT_ATTRIB_LIST(                                     \
    attr_list, out_of_service, present_value, status_flag)                           \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_BINARY_INPUT)  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BINARY_INPUT_OUT_OF_SERVICE_ID, (out_of_service)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID, (present_value))   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID, (status_flag))       \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Binary Input cluster attributes */

/*! @name Binary input attribute value manipulation API
    @{
*/

/** @brief Set normal operating mode
    @param ep - endpoint number
*/
#define ZB_ZCL_BINARY_INPUT_SET_NORMAL_MODE(ep)                           \
{                                                                         \
  zb_uint8_t val;                                                         \
                                                                          \
  val = ZB_FALSE;                                                         \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                        \
                       ZB_ZCL_ATTR_BINARY_INPUT_OUT_OF_SERVICE_ID,        \
                       &val, ZB_FALSE);                                   \
                                                                          \
  val = ZB_ZCL_BINARY_INPUT_STATUS_FLAG_NORMAL;                           \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                        \
                       ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID,           \
                       &val, ZB_FALSE);                                   \
}

/** @brief Set Out of service operating mode
    @param ep - endpoint number
*/
#define ZB_ZCL_BINARY_INPUT_SET_OUT_OF_SERVICE(ep)                                          \
{                                                                                           \
  zb_zcl_attr_t *attr_desc;                                                                 \
  zb_uint8_t val;                                                                           \
                                                                                            \
  val = ZB_TRUE;                                                                            \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                                  \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
                       ZB_ZCL_ATTR_BINARY_INPUT_OUT_OF_SERVICE_ID,                          \
                       &val, ZB_FALSE);                                                     \
                                                                                            \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                    \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,        \
                                     ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID); \
  if (attr_desc)                                                                            \
  {                                                                                         \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_BINARY_INPUT_STATUS_FLAG_OUT_OF_SERVICE; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                                \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                    \
                         ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID,                           \
                         &val, ZB_FALSE);                                                   \
  }                                                                                         \
}

/** @brief Set overridden operating mode
   @param ep - endpoint number
*/
#define ZB_ZCL_BINARY_INPUT_SET_OVERRIDEN_MODE(ep)                                     \
{                                                                                      \
  zb_zcl_attr_t *attr_desc;                                                            \
  zb_uint8_t val;                                                                      \
                                                                                       \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_BINARY_INPUT,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,        \
                         ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID);                     \
  if (attr_desc)                                                                       \
  {                                                                                    \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_BINARY_INPUT_STATUS_FLAG_OVERRIDEN; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_INPUT,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                    \
                         ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID,                      \
                         &val, ZB_FALSE);                                              \
  }                                                                                    \
}

/*! @} */ /* Binary input cluster commands */

/** @cond internals_doc */
/** @name Binary Input cluster internals
    Internal structures for Binary Input cluster
    @{
*/

/* Optionally, access to this attribute may be changed to READ_WRITE */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BINARY_INPUT_OUT_OF_SERVICE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_BINARY_INPUT_OUT_OF_SERVICE_ID,                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

/* Optionally, access to this attribute may be changed to READ_WRITE */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID(data_ptr) \
{                                                                                                  \
  ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID,                                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL | ZB_ZCL_ATTR_ACCESS_REPORTING, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                              \
  (void*) data_ptr                                                                                 \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_BINARY_INPUT_STATUS_FLAG_ID,                          \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

/*! Number of attributes mandatory for reporting in Binary Input cluster */
#define ZB_ZCL_BINARY_INPUT_REPORT_ATTR_COUNT 2

/*! @}
 *  @endcond */ /* Binary Input cluster internals */


/*! @} */ /* ZCL Binary Input cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_binary_input_init_server(void);
void zb_zcl_binary_input_init_client(void);
#define ZB_ZCL_CLUSTER_ID_BINARY_INPUT_SERVER_ROLE_INIT zb_zcl_binary_input_init_server
#define ZB_ZCL_CLUSTER_ID_BINARY_INPUT_CLIENT_ROLE_INIT zb_zcl_binary_input_init_client

#endif /* ZB_ZCL_BINARY_INPUT_H */
