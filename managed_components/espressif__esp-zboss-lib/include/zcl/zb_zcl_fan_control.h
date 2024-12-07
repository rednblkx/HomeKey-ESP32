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
/* PURPOSE: Fan Control cluster definitions
*/

#ifndef ZB_ZCL_FAN_CONTROL_H
#define ZB_ZCL_FAN_CONTROL_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */


/* Cluster ZB_ZCL_CLUSTER_ID_FAN_CONTROL */

/*! @addtogroup ZB_ZCL_FAN_CONTROL
    @{
    @name Fan Control cluster attributes
    @{
*/

/*! @brief Fan Control cluster attribute identifiers
    @see ZCL spec, subclause 6.4.2.2
*/
enum zb_zcl_fan_control_attr_e
{
  /*! @brief Fan Mode attribute */
  ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID             = 0x0000,
  /*! @brief Fan Mode Sequence attribute */
  ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID    = 0x0001
};

/*! @brief Values for Fan Mode attribute,
    @see ZCL spec, subclause 6.4.2.2.1 */
enum zb_zcl_fan_control_fan_mode_e
{
  /*! Off value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_OFF      = 0x00,
  /*! Low value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_LOW      = 0x01,
  /*! Medium value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_MEDIUM   = 0x02,
  /*! High value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_HIGH     = 0x03,
  /*! On value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_ON       = 0x04,
  /*! Auto value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_AUTO     = 0x05,
  /*! Smart value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SMART    = 0x06,

  ZB_ZCL_FAN_CONTROL_FAN_MODE_RESERVED = 0x07
};

/*! @brief Values for Fan Mode Sequence attribute,
    @see ZCL spec, subclause 6.4.2.2.2 */
enum zb_zcl_fan_control_fan_mode_sequence_e
{
  /*! Low/Med/High value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_MED_HIGH      = 0x00,
  /*! Low/High value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_HIGH          = 0x01,
  /*! Low/Med/High/Auto value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_MED_HIGH_AUTO = 0x02,
  /*! Low/High/Auto value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_LOW_HIGH_AUTO     = 0x03,
  /*! On/Auto value */
  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_ON_AUTO           = 0x04,

  ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_RESERVED          = 0x05
};

/** @brief Default value for Fan Control cluster revision global attribute */
#define ZB_ZCL_FAN_CONTROL_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Default value for Fan Mode attribute */
#define ZB_ZCL_FAN_CONTROL_FAN_MODE_DEFAULT_VALUE 0x05

/** @brief Default value for Fan Mode attribute */
#define ZB_ZCL_FAN_CONTROL_FAN_MODE_SEQUENCE_DEFAULT_VALUE 0x02

/** @brief Declare attribute list for Fan Control cluster
    @param attr_list - attribute list name
    @param fan_mode - pointer to variable to store Fan Mode attribute value
    @param fan_mode_sequence - pointer to variable to store Fan Mode Sequence attribute value
*/
#define ZB_ZCL_DECLARE_FAN_CONTROL_ATTRIB_LIST(attr_list, fan_mode, fan_mode_sequence)    \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_FAN_CONTROL)        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID, (fan_mode))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID, (fan_mode_sequence)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Fan Control cluster attributes */

/*! @name Fan Control cluster commands
    @{
*/

/*! @} */ /* Fan Control cluster commands */

/*! @cond internals_doc
    @internal @name Fan Control cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID(data_ptr) \
{                                                                            \
  ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                        \
  (void*) data_ptr                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID(data_ptr) \
{                                                                                     \
  ZB_ZCL_ATTR_FAN_CONTROL_FAN_MODE_SEQUENCE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
  (void*) data_ptr                                                                    \
}

/*! @internal Number of attributes mandatory for reporting in Fan Control cluster */
#define ZB_ZCL_FAN_CONTROL_REPORT_ATTR_COUNT 0

/*! @}
    @endcond */ /* Fan Control cluster internals */

/*! @} */ /* ZCL HA Fan Control cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_fan_control_init_server(void);
void zb_zcl_fan_control_init_client(void);
#define ZB_ZCL_CLUSTER_ID_FAN_CONTROL_SERVER_ROLE_INIT zb_zcl_fan_control_init_server
#define ZB_ZCL_CLUSTER_ID_FAN_CONTROL_CLIENT_ROLE_INIT zb_zcl_fan_control_init_client

#endif /* ZB_ZCL_FAN_CONTROL_H */
