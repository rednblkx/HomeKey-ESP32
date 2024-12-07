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
/* PURPOSE: ZCL Basic Cluster definitions
*/
#ifndef ZB_ZCL_BASIC_H
#define ZB_ZCL_BASIC_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_BASIC
  * @{
  *   @details
  *   According to ZCL spec, clause 3.2, Basic cluster contains attributes only. These attributes
  *   could be queried with @ref ZB_ZCL_COMMANDS "general ZCL commands".
  */

/** @name Basic cluster attributes
  * @{
  */

/** @brief Basic cluster information attribute set identifiers
    @see ZCL spec, subclauses 3.2.2.2.1 to 3.2.2.2.9
*/
enum zb_zcl_basic_info_attr_e
{
  /** ZCL version attribute */
  ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID                  = 0x0000,
  /** Application version attribute */
  ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID          = 0x0001,
  /** Stack version attribute */
  ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID                = 0x0002,
  /** Hardware version attribute */
  ZB_ZCL_ATTR_BASIC_HW_VERSION_ID                   = 0x0003,
  /** Manufacturer name attribute */
  ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID            = 0x0004,
  /** Model identifier attribute */
  ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID             = 0x0005,
  /** Date code attribute */
  ZB_ZCL_ATTR_BASIC_DATE_CODE_ID                    = 0x0006,
  /** Power source attribute */
  ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID                 = 0x0007,
  /** The GenericDeviceClass attribute defines the field of application of the
   *  GenericDeviceType attribute. */
  ZB_ZCL_ATTR_BASIC_GENERIC_DEVICE_CLASS_ID         = 0x0008,
  /** The GenericDeviceType attribute allows an application to show an icon on
   *  a rich user interface (e.g. smartphone app). */
  ZB_ZCL_ATTR_BASIC_GENERIC_DEVICE_TYPE_ID          = 0x0009,
  /** The ProductCode attribute allows an application to specify a code for
   *  the product. */
  ZB_ZCL_ATTR_BASIC_PRODUCT_CODE_ID                 = 0x000a,
  /** The ProductURL attribute specifies a link to a web page containing specific
   *  product information. */
  ZB_ZCL_ATTR_BASIC_PRODUCT_URL_ID                  = 0x000b,
  /** Vendor specific human readable (displayable) string representing the versions
   *  of one of more program images supported on the device. */
  ZB_ZCL_ATTR_BASIC_MANUFACTURER_VERSION_DETAILS_ID = 0x000c,
  /** Vendor specific human readable (displayable) serial number. */
  ZB_ZCL_ATTR_BASIC_SERIAL_NUMBER_ID                = 0x000d,
  /** Vendor specific human readable (displayable) product label. */
  ZB_ZCL_ATTR_BASIC_PRODUCT_LABEL_ID                = 0x000e,
  /** Manufacturer-specific reference to the version of the software. */
  ZB_ZCL_ATTR_BASIC_SW_BUILD_ID                     = 0x4000,
};

/** @brief Basic cluster settings attribute set identifiers
    @see ZCL spec, subclauses 3.2.2.2.10 to 3.2.2.2.15
*/
enum zb_zcl_basic_settings_attr_e
{
  /*! Location description attribute */
  ZB_ZCL_ATTR_BASIC_LOCATION_DESCRIPTION_ID = 0x0010,
  /*! Physical environment attribute */
  ZB_ZCL_ATTR_BASIC_PHYSICAL_ENVIRONMENT_ID = 0x0011,
  /*! Device enabled attribute */
  ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID       = 0x0012,
  /*! Alarm mask attribute */
  ZB_ZCL_ATTR_BASIC_ALARM_MASK_ID           = 0x0013,
  /*! Disable local config attribute */
  ZB_ZCL_ATTR_BASIC_DISABLE_LOCAL_CONFIG_ID = 0x0014
};

/**
 *  @brief Permitted values for "Power source" attribute.
 *  @see ZCL spec, subclauses 3.2.2.2.9.
 */
enum zb_zcl_basic_power_source_e
{
  ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN                = 0x00,  /**< Power source unknown. */
  ZB_ZCL_BASIC_POWER_SOURCE_MAINS_SINGLE_PHASE     = 0x01,  /**< Single-phase mains. */
  ZB_ZCL_BASIC_POWER_SOURCE_MAINS_THREE_PHASE      = 0x02,  /**< 3-phase mains. */
  ZB_ZCL_BASIC_POWER_SOURCE_BATTERY                = 0x03,  /**< Battery source. */
  ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE              = 0x04,  /**< DC source. */
  ZB_ZCL_BASIC_POWER_SOURCE_EMERGENCY_MAINS_CONST  = 0x05,  /**< Emergency mains constantly
                                                              powered. */
  ZB_ZCL_BASIC_POWER_SOURCE_EMERGENCY_MAINS_TRANSF = 0x06   /**< Emergency mains and transfer
                                                              switch. */
};

/** @brief Basic permitted values for "Physical environment" attribute.
    @note most of values are profile-specific.
    @see ZCL spec, subclause 3.2.2.2.12
*/
enum zb_zcl_basic_physical_env_e
{
  /*! Environment unspecified */
  ZB_ZCL_BASIC_ENV_UNSPECIFIED = 0,
  /*! Environment unknown */
  ZB_ZCL_BASIC_ENV_UNKNOWN     = 0xff
};

/** @brief Permitted bits for "Alarm mask" attribute
    @see ZCL spec, subclause 3.2.2.2.14
*/
enum zb_zcl_basic_alarm_mask_e
{
  /*! General hardware fault */
  ZB_ZCL_BASIC_ALARM_MASK_HW_FAULT = 0,
  /*! General software fault */
  ZB_ZCL_BASIC_ALARM_MASK_SW_FAULT = 1
};

/** @brief Value structure for "Disable local config" attribute
    @see ZCL spec, subclause 3.2.2.2.15
*/
typedef struct zb_zcl_basic_disable_local_conf_s
{
  /*! Reset to factory defaults disabled */
  zb_bitfield_t reset_disabled  : 1;
  /*! Device configuration enabled */
  zb_bitfield_t config_enabled  : 1;
  /*! Reserved bits */
  zb_bitfield_t reserved        : 6;
} zb_zcl_basic_disable_local_conf_t;

/** @brief Default value for Basic cluster revision global attribute */
#define ZB_ZCL_BASIC_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/** @brief Default value for ZCL version attribute */
#define ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE ((zb_uint8_t)ZB_ZCL_VERSION)

/** @brief Default value for Application version attribute */
#define ZB_ZCL_BASIC_APPLICATION_VERSION_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for Stack version attribute */
#define ZB_ZCL_BASIC_STACK_VERSION_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for Hardware version attribute */
#define ZB_ZCL_BASIC_HW_VERSION_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for Manufacturer name attribute */
#define ZB_ZCL_BASIC_MANUFACTURER_NAME_DEFAULT_VALUE {0}

/** @brief Default value for Model identifier attribute */
#define ZB_ZCL_BASIC_MODEL_IDENTIFIER_DEFAULT_VALUE {0}

/** @brief Default value for Date code attribute */
#define ZB_ZCL_BASIC_DATE_CODE_DEFAULT_VALUE {0}

/** @brief Default value for Power source attribute */
#define ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE (ZB_ZCL_BASIC_POWER_SOURCE_UNKNOWN)

/** @brief Default value for GenericDeviceClass attribute */
#define ZB_ZCL_BASIC_GENERIC_DEVICE_CLASS_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for GenericDeviceType attribute */
#define ZB_ZCL_BASIC_GENERIC_DEVICE_TYPE_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for ProductCode attribute */
#define ZB_ZCL_BASIC_PRODUCT_CODE_DEFAULT_VALUE {0}

/** @brief Default value for ProductURL attribute */
#define ZB_ZCL_BASIC_PRODUCT_URL_DEFAULT_VALUE {0}
/** @brief Default value for ManufacturerVersionDetails attribute */

#define ZB_ZCL_BASIC_MANUFACTURER_VERSION_DETAILS_DEFAULT_VALUE {0}
/** @brief Default value for SerialNumber attribute */

#define ZB_ZCL_BASIC_SERIAL_NUMBER_DEFAULT_VALUE {0}
/** @brief Default value for ProductLabel attribute */

#define ZB_ZCL_BASIC_PRODUCT_LABEL_DEFAULT_VALUE {0}

/** @brief Default value for location description attribute */
#define ZB_ZCL_BASIC_LOCATION_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for Physical environment attribute */
#define ZB_ZCL_BASIC_PHYSICAL_ENVIRONMENT_DEFAULT_VALUE (ZB_ZCL_BASIC_ENV_UNSPECIFIED)

/** @brief Default value for Device enabled attribute */
#define ZB_ZCL_BASIC_DEVICE_ENABLED_DEFAULT_VALUE ((zb_uint8_t)0x01)

/** @brief Default value for Alarm mask attribute */
#define ZB_ZCL_BASIC_ALARM_MASK_DEFAULT_VALUE (ZB_ZCL_BASIC_ALARM_MASK_HW_FAULT)

/** @brief Default value for Disable local config attribute */
#define ZB_ZCL_BASIC_DISABLE_LOCAL_CONFIG_DEFAULT_VALUE ((zb_zcl_basic_disable_local_conf_t){0,0,0})

/**
 *  @brief Default value for SWBuildId attribute.
 *  @note Default value supposes that attribute will be stored as Pascal-style string (i. e.
 *  length-byte, then content).
 */
#define ZB_ZCL_BASIC_SW_BUILD_ID_DEFAULT_VALUE  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}


/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID(data_ptr) \
{                                                                         \
  ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                     \
  (void*) data_ptr                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID(data_ptr) \
{                                                                                 \
  ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
  (void*) data_ptr                                                                \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID(data_ptr) \
{                                                                           \
  ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                       \
  (void*) data_ptr                                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_HW_VERSION_ID(data_ptr) \
{                                                                        \
  ZB_ZCL_ATTR_BASIC_HW_VERSION_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                    \
  (void*) data_ptr                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID(data_ptr) \
{                                                                               \
  ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID,                                       \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) data_ptr                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID(data_ptr) \
{                                                                              \
  ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID,                                       \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                          \
  (void*) data_ptr                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_DATE_CODE_ID(data_ptr) \
{                                                                       \
  ZB_ZCL_ATTR_BASIC_DATE_CODE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                   \
  (void*) data_ptr                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID(data_ptr) \
{                                                                          \
  ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                            \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                      \
  (void*) data_ptr                                                         \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_LOCATION_DESCRIPTION_ID(data_ptr) \
{                                                                                  \
  ZB_ZCL_ATTR_BASIC_LOCATION_DESCRIPTION_ID,                                       \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                              \
  (void*) data_ptr                                                                 \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_PHYSICAL_ENVIRONMENT_ID(data_ptr) \
{                                                                                  \
  ZB_ZCL_ATTR_BASIC_PHYSICAL_ENVIRONMENT_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                              \
  (void*) data_ptr                                                                 \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID(data_ptr) \
{                                                                            \
  ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID,                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                        \
  (void*) data_ptr                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID(data_ptr)          \
{                                                                               \
  ZB_ZCL_ATTR_BASIC_SW_BUILD_ID,                                                \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) data_ptr                                                              \
}

#if defined ZB_ZCL_SUPPORT_CLUSTER_SCENES
/*! Scenes field set length for Basic cluster */
#define ZB_ZCL_CLUSTER_ID_BASIC_SCENE_FIELD_SETS_LENGTH 0
#endif /* defined ZB_ZCL_SUPPORT_CLUSTER_SCENES */

/** @endcond */ /* internals_doc */

/** @brief Declare attribute list for Basic cluster
    @param attr_list - attribute list name
    @param zcl_version - pointer to variable to store zcl version  attribute value
    @param power_source - pointer to variable to store power source attribute value

    @snippet lighting/dimmable_light_tl/light_device_zr.c BASIC_CLUSTER_DECLARE
*/
#define ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(attr_list, zcl_version, power_source) \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_BASIC) \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, (zcl_version))      \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, (power_source))    \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @brief Declare attribute list for Basic cluster
    @param attr_list - attribute list name
    @param zcl_version - pointer to variable to store zcl version  attribute value
    @param power_source - pointer to variable to store power source attribute value
    @param device_enabled - pointer to variable to store device enabled attribute value
*/
#define ZB_ZCL_DECLARE_BASIC_WITH_DEVICE_ENABLED_ATTRIB_LIST(attr_list, zcl_version, power_source, device_enabled) \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                    \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, (zcl_version))          \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, (power_source))        \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID, (device_enabled))    \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**
 *  @brief Declare attribute list for Basic cluster (extended attribute set).
 *  @param attr_list [IN] - attribute list name.
 *  @param zcl_version [IN] - pointer to variable storing ZCL version  attribute value.
 *  @param app_version [IN] - pointer to the variable storing application version.
 *  @param stack_version [IN] - pointer to the variable storing stack version.
 *  @param hardware_version [IN] - pointer to the variable storing hardware version.
 *  @param manufacturer_name [IN] - pointer to the variable storing manufacturer name.
 *  @param model_id [IN] - pointer to the variable storing model identifier.
 *  @param date_code [IN] - pointer to the variable storing date code.
 *  @param power_source [IN] - pointer to variable storing power source attribute value.
 *  @param location_id [IN] - pointer to variable storing location description attribute value.
 *  @param ph_env [IN] - pointer to variable storing physical environment attribute value.
 *  @param sw_build_id [IN] - pointer to the variable storing software version reference.
 */
#define ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(                                       \
  attr_list,                                                                        \
  zcl_version,                                                                      \
  app_version,                                                                      \
  stack_version,                                                                    \
  hardware_version,                                                                 \
  manufacturer_name,                                                                \
  model_id,                                                                         \
  date_code,                                                                        \
  power_source,                                                                     \
  location_id,                                                                      \
  ph_env,                                                                           \
  sw_build_id)                                                                      \
  zb_bool_t device_enable_##attr_list = ZB_TRUE;                                    \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_BASIC)      \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, (zcl_version))           \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, (app_version))   \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID, (stack_version))       \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_HW_VERSION_ID, (hardware_version))       \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (manufacturer_name))\
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (model_id))         \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_DATE_CODE_ID, (date_code))               \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, (power_source))         \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_SW_BUILD_ID, (sw_build_id))              \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID,                       \
                         &(device_enable_##attr_list))                              \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_LOCATION_DESCRIPTION_ID, (location_id))  \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_PHYSICAL_ENVIRONMENT_ID, (ph_env))       \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**
 *  @brief Declare attribute list for Basic cluster (extended attribute set).
 *  @param attr_list [IN] - attribute list name.
 *  @param zcl_version [IN] - pointer to variable storing ZCL version  attribute value.
 *  @param app_version [IN] - pointer to the variable storing application version.
 *  @param stack_version [IN] - pointer to the variable storing stack version.
 *  @param hardware_version [IN] - pointer to the variable storing hardware version.
 *  @param manufacturer_name [IN] - pointer to the variable storing manufacturer name.
 *  @param model_id [IN] - pointer to the variable storing model identifier.
 *  @param date_code [IN] - pointer to the variable storing date code.
 *  @param power_source [IN] - pointer to variable storing power source attribute value.
 *  @param location_id [IN] - pointer to variable storing location description attribute value.
 *  @param ph_env [IN] - pointer to variable storing physical environment attribute value.
 *  @param sw_build_id [IN] - pointer to the variable storing software version reference.
 *  @param device_enabled - pointer to the variable storing device enabled reference.
 */
#define ZB_ZCL_DECLARE_BASIC_WITH_DEVICE_ENABLED_ATTRIB_LIST_EXT(                   \
  attr_list,                                                                        \
  zcl_version,                                                                      \
  app_version,                                                                      \
  stack_version,                                                                    \
  hardware_version,                                                                 \
  manufacturer_name,                                                                \
  model_id,                                                                         \
  date_code,                                                                        \
  power_source,                                                                     \
  location_id,                                                                      \
  ph_env,                                                                           \
  sw_build_id,                                                                      \
  device_enabled)                                                                   \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, (zcl_version))           \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, (app_version))   \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID, (stack_version))       \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_HW_VERSION_ID, (hardware_version))       \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (manufacturer_name))\
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (model_id))         \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_DATE_CODE_ID, (date_code))               \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, (power_source))         \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_SW_BUILD_ID, (sw_build_id))              \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID, (device_enabled))     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_LOCATION_DESCRIPTION_ID, (location_id))  \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_PHYSICAL_ENVIRONMENT_ID, (ph_env))       \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! Maximum length of ManufacturerName string field */
#define ZB_ZCL_CLUSTER_ID_BASIC_MANUFACTURER_NAME_MAX_LEN 32

/*! Maximum length of ModelIdentifier string field */
#define ZB_ZCL_CLUSTER_ID_BASIC_MODEL_IDENTIFIER_MAX_LEN  32

/**
 *  @brief Basic cluster attributes
 */
typedef struct zb_zcl_basic_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID
   * @see ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID
   */
  zb_uint8_t zcl_version;

  /** @copydoc ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID
   * @see ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID
   */
  zb_uint8_t power_source;
} zb_zcl_basic_attrs_t;


/** @brief Declare attribute list for Basic cluster cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of @ref zb_zcl_basic_attrs_t type (containing Basic cluster attributes)
 */
#define ZB_ZCL_DECLARE_BASIC_ATTR_LIST(attr_list, attrs) \
  ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST(attr_list, &attrs.zcl_version, &attrs.power_source)

/** @} */ /* Basic cluster attributes */

/*! @name Alarms cluster commands
    @{
*/

/*! @brief Basic cluster command identifiers
    @see ZCL spec, subclause 3.2.2.3
*/
enum zb_zcl_basic_cmd_e
{
  ZB_ZCL_CMD_BASIC_RESET_ID       = 0x00, /**< "Reset to Factory Defaults" command. */
};

/** @cond internals_doc */
/* Basic cluster commands lists : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_BASIC_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CMD_BASIC_RESET_ID

#define ZB_ZCL_CLUSTER_ID_BASIC_CLIENT_ROLE_GENERATED_CMD_LIST ZB_ZCL_CLUSTER_ID_BASIC_SERVER_ROLE_RECEIVED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

////////////////////////////////////////

/*! @brief Send Reset to Factory Defaults command
    @param buffer to put data to
    @param addr - address of the device to send command to
    @param dst_addr_mode - addressing mode
    @param dst_ep destination endpoint
    @param ep - current endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - "Disable default response" flag
    @param cb - callback to call to report send status
*/
#define ZB_ZCL_BASIC_SEND_RESET_REQ(                                                      \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, cb)                 \
{                                                                                         \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                          \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)             \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_BASIC_RESET_ID);  \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                              \
      buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_BASIC, cb);     \
}


/** Call device callback in user application to reset application settings to defaults.
 * @param[in] buffer - pointer to buffer for callback
 * @param[in] ep - current endpoint
 * @param[out] result - callback status
 */
#define ZB_ZCL_RESET_TO_FACTORY_DEFAULTS_USER_APP(buffer, ep, result) \
{                                                                   \
  TRACE_MSG(TRACE_ZCL1, "ZB_ZCL_RESET_TO_FACTORY_DEFAULTS_USER_APP", (FMT__0)); \
  if (ZCL_CTX().device_cb)                                          \
  {                                                                 \
    zb_zcl_device_callback_param_t *user_app_data =                 \
      ZB_BUF_GET_PARAM(buffer, zb_zcl_device_callback_param_t);     \
    user_app_data->device_cb_id = ZB_ZCL_BASIC_RESET_CB_ID;         \
    user_app_data->endpoint = (ep);                                   \
    user_app_data->status = RET_OK;                                 \
    (ZCL_CTX().device_cb)(param);                                   \
    result = user_app_data->status;                                 \
  }                                                                 \
}

/*! @brief Check Device Enabled attribute value and should the stack process command or not.
    @see ZCL spec, subclause 3.2.2.2.18 DeviceEnabled Attribute
    @param ep_id Endpoint ID
    @param cmd_id Command ID
    @param cluster_id Cluster ID
    @param is_common_command Is command common or cluster specific
    @return ZB_TRUE if command should be processed or sent, ZB_FALSE otherwise
*/
zb_bool_t zb_zcl_check_is_device_enabled(zb_uint8_t ep_id, zb_uint8_t cmd_id, zb_uint16_t cluster_id, zb_bool_t is_common_command);

/** @} */ /* Basic cluster commands */

/*! @} */ /* ZCL Basic cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_basic_init_server(void);
void zb_zcl_basic_init_client(void);

#define ZB_ZCL_CLUSTER_ID_BASIC_SERVER_ROLE_INIT zb_zcl_basic_init_server
#define ZB_ZCL_CLUSTER_ID_BASIC_CLIENT_ROLE_INIT zb_zcl_basic_init_client

#endif /* ZB_ZCL_BASIC_H */
