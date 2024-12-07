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
/* PURPOSE: Shade Configuration cluster definitions
*/

#ifndef ZB_ZCL_SHADE_CONFIG_H
#define ZB_ZCL_SHADE_CONFIG_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/* Logically most opened value */
#define ZB_ZCL_SHADE_CONFIG_LOGICALLY_MOST_OPENED 0

/* Logically most closed value */
#define ZB_ZCL_SHADE_CONFIG_LOGICALLY_MOST_CLOSED 0xffff

/** @defgroup ZB_ZCL_SHADE_CONFIG ZCL Shade Configuration cluster
    @ingroup ZB_ZCL_CLUSTERS
    @addtogroup ZB_ZCL_SHADE_CONFIG
    @{
*/

/** @name Shade Configuration cluster attributes
    @{
*/

/** @brief Shade Configuration cluster information attribute set identifiers
    @see ZCL spec, subclause 7.2.2.2.1
*/
enum zb_zcl_shade_config_info_attr_e
{
  /** The PhysicalClosedLimit attribute indicates the most closed (numerically lowest)
   *  position that the shade can physically move to. */
  ZB_ZCL_ATTR_SHADE_CONFIG_PHYSICAL_CLOSED_LIMIT_ID = 0x0000,
  /** The MotorStepSize attribute indicates the angle the shade motor moves for one step,
   *  measured in 1/10th of a degree. */
  ZB_ZCL_ATTR_SHADE_CONFIG_MOTOR_STEP_SIZE_ID       = 0x0001,
  /** @brief Status attribute */
  ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_ID                = 0x0002
};

/** @brief Bit values for Status attribute
  * @see ZCL spec, subclause 7.2.2.2.1.3
  */
enum zb_zcl_shade_config_status_type_e
{
  /** Shade operational value */
  ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_SHADE_OPERATIONAL          = 0x00,
  /** Shade Adjusting value */
  ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_SHADE_ADJUSTING            = 0x01,
  /** Shade Direction value */
  ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_SHADE_DIRECTION            = 0x02,
  /** Forward Direction of Motor value */
  ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_FORWARD_DIRECTION_OF_MOTOR = 0x03,
  /** Reserved values */
  ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_RESERVED                   = 0x04
};

/** @cond internals_doc */
/** @internal Set bits from conf_var variable to type_value */
#define ZB_ZCL_SET_BIT(conf_var, type_value)                        \
{                                                                   \
  (conf_var) = (conf_var) | (type_value);                           \
}

/** @internal Get type_value bits from conf_var variable */
#define ZB_ZCL_GET_BIT(conf_var, type_value) ((conf_var) & (type_value))

/** @internal Clear type_value bits in conf_var variable */
#define ZB_ZCL_CLR_BIT(conf_var, type_value)                            \
{                                                                       \
  (conf_var) = (conf_var) & ~(type_value);                              \
}
/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Sets bits of Status parameter
    @param type_value - bit to set
    @param conf_var - config variable
*/
#define ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_SET(conf_var, type_value) \
  ZB_ZCL_SET_BIT(conf_var, type_value)

/** @brief Gets bit value of Status parameter
    @param type_value - bit value to check
    @param conf_var - config variable
*/
#define ZB_ZCL_ATTR_SHADE_CONFIG_GET_STATUS_BIT_VAL(conf_var, type_value) \
  ZB_ZCL_GET_BIT(conf_var, type_value)


/** @brief Clear bit of Status parameter
    @param type_value - bit to clear
    @param conf_var - config variable
*/
#define ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_CLEAR(conf_var, type_value) \
  ZB_ZCL_CLR_BIT(conf_var, type_value)


/** @brief Shade cluster settings attribute set identifiers
    @see ZCL spec, subclause 7.2.2.2.2
*/
enum zb_zcl_shade_config_settings_attr_e
{
  /** @brief ClosedLimit attribute */
  ZB_ZCL_ATTR_SHADE_CONFIG_CLOSED_LIMIT_ID = 0x0010,
  /** @brief Mode attribute */
  ZB_ZCL_ATTR_SHADE_CONFIG_MODE_ID         = 0x0011
};

/** @brief Permissible values for Mode attribute
    @see ZCL spec, subclause 7.2.2.2.2.2 */
enum zb_zcl_shade_config_mode_e
{
  /** Normal value */
  ZB_ZCL_ATTR_SHADE_CONFIG_MODE_NORMAL    = 0x00,
  /** Configure value */
  ZB_ZCL_ATTR_SHADE_CONFIG_MODE_CONFIGURE = 0x01,
  /** Reserved values */
  ZB_ZCL_ATTR_SHADE_CONFIG_MODE_RESERVED  = 0x02
};

/** @cond internals_doc */
typedef struct zb_zcl_shade_set_value_param_s
{
  zb_uint16_t new_value;
  zb_bool_t is_config;
} zb_zcl_shade_set_value_param_t;

typedef struct zb_zcl_shade_get_value_param_s
{
  zb_uint16_t ret_value;
} zb_zcl_shade_get_value_param_t;
/*! @}
 *  @endcond */ /* internals_doc */

 /** @brief Default value for Shade Configuration cluster revision global attribute */
#define ZB_ZCL_SHADE_CONFIG_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Shade Configuration Status attribute default value */
#define ZB_ZCL_SHADE_CONFIG_STATUS_DEFAULT_VALUE 0x00

/** @brief ClosedLimit attribute default value */
#define ZB_ZCL_SHADE_CONFIG_CLOSED_LIMIT_DEFAULT_VALUE 0x0001

/** @brief Zero Point attribute default value */
#define ZB_ZCL_SHADE_CONFIG_ZERO_POINT_DEFAULT_VALUE 0x0000

/** @brief Mode attribute default value */
#define ZB_ZCL_SHADE_CONFIG_MODE_DEFAULT_VALUE ZB_ZCL_ATTR_SHADE_CONFIG_MODE_NORMAL

/** @brief Declare attribute list for Shade Configuration cluster
    @param attr_list - attribute list name
    @param status - pointer to variable to store Status attribute value
    @param closed_limit - pointer to variable to store ClosedLimit attribute value
    @param mode - pointer to variable to store Mode attribute value
*/
#define ZB_ZCL_DECLARE_SHADE_CONFIG_ATTRIB_LIST(attr_list, status, closed_limit, mode) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_SHADE_CONFIG)    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_ID, (status))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SHADE_CONFIG_CLOSED_LIMIT_ID, (closed_limit))       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_SHADE_CONFIG_MODE_ID, (mode))                       \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* Shade Configuration cluster attributes */

/* Descriptors for server side */

/** @cond internals_doc */
/** @name Shade Configuration cluster internals
    Internal structures for Shade Configuration cluster
    @internal
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_ID(data_ptr)       \
{                                                                                 \
  ZB_ZCL_ATTR_SHADE_CONFIG_STATUS_ID,                                             \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
  (void*) data_ptr                                                                \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_SHADE_CONFIG_CLOSED_LIMIT_ID(data_ptr) \
{                                                                                 \
  ZB_ZCL_ATTR_SHADE_CONFIG_CLOSED_LIMIT_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
  (void*) data_ptr                                                                \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_SHADE_CONFIG_MODE_ID(data_ptr)         \
{                                                                                 \
  ZB_ZCL_ATTR_SHADE_CONFIG_MODE_ID,                                               \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
  (void*) data_ptr                                                                \
}

/** Number of attributes mandatory for reporting in Shade Configuration cluster */
#define ZB_ZCL_SHADE_CONFIG_REPORT_ATTR_COUNT 0


/** Macro for setting Mode attribute
    @param ep - endpoint id
    @param mode_val - mode value
*/

/** @brief Shade Configuration cluster command identifiers
    @see ZCL spec, subclause ?
*/
/*enum zb_zcl_shade_config_cmd_e
{

};*/

/** Mandatory commands defines */

/*! @internal Setting Shade Mode value macro:
    @param ep - endpoint where setting
    @param mode_val - Shade Mode value
*/

#define ZB_ZCL_SHADE_SET_MODE(ep, mode_val)              \
{                                                        \
  zb_uint8_t mode = (mode_val);                          \
  ZB_ZCL_SET_ATTRIBUTE((ep),                             \
                       ZB_ZCL_CLUSTER_ID_SHADE_CONFIG,   \
                       ZB_ZCL_ATTR_SHADE_CONFIG_MODE_ID, \
                       &mode,                            \
                       ZB_FALSE);                        \
}

/** @} */ /* Shade Configuration cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/** @name Shade Configuration cluster commands
    @{
*/


/** @} */ /* Shade Configuration cluster commands */

/** @} */ /* ZCL Shade Configuration cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_shade_config_init_server(void);
void zb_zcl_shade_config_init_client(void);
#define ZB_ZCL_CLUSTER_ID_SHADE_CONFIG_SERVER_ROLE_INIT zb_zcl_shade_config_init_server
#define ZB_ZCL_CLUSTER_ID_SHADE_CONFIG_CLIENT_ROLE_INIT zb_zcl_shade_config_init_client

#endif /* ZB_ZCL_SHADE_CONFIG_H */
