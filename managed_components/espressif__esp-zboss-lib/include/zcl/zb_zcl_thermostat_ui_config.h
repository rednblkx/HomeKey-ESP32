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
/* PURPOSE: Thermostat UI Configuration cluster definitions
*/

#ifndef ZB_ZCL_THERMOSTAT_UI_CONFIG_H
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/* Cluster ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG */

/*! @addtogroup ZB_ZCL_THERMOSTAT_UI_CONFIG
    @{
    @name Thermostat UI Configuration cluster attributes
    @{
*/

/*! @brief Thermostat UI Configuration cluster attribute identifiers
    @see ZCL spec, subclause 6.6.2.2
*/
enum zb_zcl_thermostat_ui_config_attr_e
{
  /** @brief Temperature Display Mode attribute */
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID        = 0x0000,
  /** @brief Keypad Lockout attribute */
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID                  = 0x0001,
  /** The ScheduleProgrammingVisibility attribute is used to hide the weekly
   *  schedule programming functionality or menu on a thermostat from a user
   *  to prevent local user programming of the weekly schedule. */
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_SCHEDULE_PROGRAMMING_VISIBILITY_ID = 0x0002,
};

/*! @brief Values for Temperature Display Mode attribute,
    @see ZCL spec, subclause 6.6.2.2.1 */
enum zb_zcl_thermostat_ui_config_temperature_display_mode_e
{
  /*! Temperature in C value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_IN_C     = 0x00,
  /*! Temperature in F value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_IN_F     = 0x01,

  ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_RESERVED = 0x02
};

/*! @brief Values for Keypad Lockout attribute,
    @see ZCL spec, subclause 6.6.2.2.2 */
enum zb_zcl_thermostat_ui_config_keypad_lockout_e
{
  /*! No Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_NO_LOCKOUT = 0x00,
  /*! Level 1 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_1    = 0x01,
  /*! Level 2 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_2    = 0x02,
  /*! Level 3 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_3    = 0x03,
  /*! Level 4 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_4    = 0x04,
  /*! Level 5 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_5    = 0x05,

  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_RESERVED   = 0x06
};

/** @brief Default value for Thermostat UI Configuration cluster revision global attribute */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Default value for Temperature Display Mode attribute */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_DEFAULT_VALUE 0x00

/** @brief Default value for Keypad Lockout attribute */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_DEFAULT_VALUE 0x00

/** @brief Default value for ScheduleProgrammingVisibility attribute */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_SCHEDULE_PROGRAMMING_VISIBILITY_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Declare attribute list for Thermostat UI Configuration cluster
    @param attr_list - attribute list name
    @param temperature_display_mode - pointer to variable to store Temperature Display Mode attribute value
    @param keypad_lockout - pointer to variable to store Keypad Lockout attribute value
*/
#define ZB_ZCL_DECLARE_THERMOSTAT_UI_CONFIG_ATTRIB_LIST(attr_list, temperature_display_mode, keypad_lockout)     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_THERMOSTAT)                                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID, (temperature_display_mode)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID, (keypad_lockout))                     \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Thermostat UI Configuration cluster attributes */

/*! @name Thermostat UI Configuration cluster commands
    @{
*/

/*! @} */ /* Thermostat UI Configuration cluster commands */

/*! @cond internals_doc
    @internal @name Thermostat UI Configuration cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID(data_ptr) \
{                                                                                                     \
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                 \
  (void*) data_ptr                                                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID(data_ptr)           \
{                                                                                                     \
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                 \
  (void*) data_ptr                                                                                    \
}

/*! @internal Number of attributes mandatory for reporting in Thermostat UI Configuration cluster */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_REPORT_ATTR_COUNT 0

/*! @}
    @endcond */ /* Thermostat UI Configuration cluster internals */

/*! @} */ /* ZCL HA Thermostat UI Configuration cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_thermostat_ui_config_init_server(void);
void zb_zcl_thermostat_ui_config_init_client(void);
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG_SERVER_ROLE_INIT zb_zcl_thermostat_ui_config_init_server
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG_CLIENT_ROLE_INIT zb_zcl_thermostat_ui_config_init_client

#endif /* ZB_ZCL_THERMOSTAT_UI_CONFIG_H */
