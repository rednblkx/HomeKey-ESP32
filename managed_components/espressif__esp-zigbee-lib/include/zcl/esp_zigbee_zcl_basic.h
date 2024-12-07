/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief Basic cluster information attribute set identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID                  = 0x0000,                 /*!<ZCL version attribute */
    ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID          = 0x0001,                 /*!< Application version attribute */
    ESP_ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID                = 0x0002,                 /*!< Stack version attribute */
    ESP_ZB_ZCL_ATTR_BASIC_HW_VERSION_ID                   = 0x0003,                 /*!< Hardware version attribute */
    ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID            = 0x0004,                 /*!< Manufacturer name attribute */
    ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID             = 0x0005,                 /*!< Model identifier attribute */
    ESP_ZB_ZCL_ATTR_BASIC_DATE_CODE_ID                    = 0x0006,                 /*!< Date code attribute */
    ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID                 = 0x0007,                 /*!< Power source attribute */
    ESP_ZB_ZCL_ATTR_BASIC_GENERIC_DEVICE_CLASS_ID         = 0x0008,                 /*!< The GenericDeviceClass attribute defines the field of application of the  GenericDeviceType attribute. */
    ESP_ZB_ZCL_ATTR_BASIC_GENERIC_DEVICE_TYPE_ID          = 0x0009,                 /*!< The GenericDeviceType attribute allows an application to show an icon on a rich user interface (e.g. smartphone app). */
    ESP_ZB_ZCL_ATTR_BASIC_PRODUCT_CODE_ID                 = 0x000a,                 /*!< The ProductCode attribute allows an application to specify a code for the product. */
    ESP_ZB_ZCL_ATTR_BASIC_PRODUCT_URL_ID                  = 0x000b,                 /*!< The ProductURL attribute specifies a link to a web page containing specific product information. */
    ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_VERSION_DETAILS_ID = 0x000c,                 /*!< Vendor specific human readable (displayable) string representing the versions of one of more program images supported on the device. */
    ESP_ZB_ZCL_ATTR_BASIC_SERIAL_NUMBER_ID                = 0x000d,                 /*!< Vendor specific human readable (displayable) serial number. */
    ESP_ZB_ZCL_ATTR_BASIC_PRODUCT_LABEL_ID                = 0x000e,                 /*!< Vendor specific human readable (displayable) product label. */
    ESP_ZB_ZCL_ATTR_BASIC_LOCATION_DESCRIPTION_ID         = 0x0010,                 /*!< Location description attribute */
    ESP_ZB_ZCL_ATTR_BASIC_PHYSICAL_ENVIRONMENT_ID         = 0x0011,                 /*!< Physical environment attribute */
    ESP_ZB_ZCL_ATTR_BASIC_DEVICE_ENABLED_ID               = 0x0012,                 /*!< Device enabled attribute */
    ESP_ZB_ZCL_ATTR_BASIC_ALARM_MASK_ID                   = 0x0013,                 /*!< Alarm mask attribute */
    ESP_ZB_ZCL_ATTR_BASIC_DISABLE_LOCAL_CONFIG_ID         = 0x0014,                 /*!< Disable local config attribute */
    ESP_ZB_ZCL_ATTR_BASIC_SW_BUILD_ID                     = 0x4000                  /*!< Manufacturer-specific reference to the version of the software. */
} esp_zb_zcl_basic_attr_t;

/** @brief Default value for ZCL version attribute */
#define ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE ((uint8_t)0x08)

/** @brief Default value for Application version attribute */
#define ESP_ZB_ZCL_BASIC_APPLICATION_VERSION_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for Stack version attribute */
#define ESP_ZB_ZCL_BASIC_STACK_VERSION_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for Hardware version attribute */
#define ESP_ZB_ZCL_BASIC_HW_VERSION_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for Manufacturer name attribute */
#define ESP_ZB_ZCL_BASIC_MANUFACTURER_NAME_DEFAULT_VALUE {0}

/** @brief Default value for Model identifier attribute */
#define ESP_ZB_ZCL_BASIC_MODEL_IDENTIFIER_DEFAULT_VALUE {0}

/** @brief Default value for Date code attribute */
#define ESP_ZB_ZCL_BASIC_DATE_CODE_DEFAULT_VALUE {0}

/** @brief Default value for Power source attribute */
#define ESP_ZB_ZCL_BASIC_POWER_SOURCE_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for GenericDeviceClass attribute */
#define ESP_ZB_ZCL_BASIC_GENERIC_DEVICE_CLASS_DEFAULT_VALUE ((uint8_t)0xFF)

/** @brief Default value for GenericDeviceType attribute */
#define ESP_ZB_ZCL_BASIC_GENERIC_DEVICE_TYPE_DEFAULT_VALUE ((uint8_t)0xFF)

/** @brief Default value for ProductCode attribute */
#define ESP_ZB_ZCL_BASIC_PRODUCT_CODE_DEFAULT_VALUE {0}

/** @brief Default value for ProductURL attribute */
#define ESP_ZB_ZCL_BASIC_PRODUCT_URL_DEFAULT_VALUE {0}

/** @brief Default value for ManufacturerVersionDetails attribute */
#define ESP_ZB_ZCL_BASIC_MANUFACTURER_VERSION_DETAILS_DEFAULT_VALUE {0}

/** @brief Default value for SerialNumber attribute */
#define ESP_ZB_ZCL_BASIC_SERIAL_NUMBER_DEFAULT_VALUE {0}

/** @brief Default value for ProductLabel attribute */
#define ESP_ZB_ZCL_BASIC_PRODUCT_LABEL_DEFAULT_VALUE {0}

/** @brief Default value for location description attribute */
#define ESP_ZB_ZCL_BASIC_LOCATION_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for Physical environment attribute */
#define ESP_ZB_ZCL_BASIC_PHYSICAL_ENVIRONMENT_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for Device enabled attribute */
#define ESP_ZB_ZCL_BASIC_DEVICE_ENABLED_DEFAULT_VALUE ((bool)0x01)

/** @brief Default value for Alarm mask attribute */
#define ESP_ZB_ZCL_BASIC_ALARM_MASK_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for Disable local config attribute */
#define ESP_ZB_ZCL_BASIC_DISABLE_LOCAL_CONFIG_DEFAULT_VALUE ((uint8_t)0x00)

/**
 *  @brief Default value for SWBuildId attribute.
 *  @note Default value supposes that attribute will be stored as Pascal-style string (i. e.
 *  length-byte, then content).
 */
#define ESP_ZB_ZCL_BASIC_SW_BUILD_ID_DEFAULT_VALUE  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/*! @brief Basic cluster command identifiers
*/
typedef enum {
    ESP_ZB_ZCL_CMD_BASIC_RESET_ID       = 0x00 /*!< "Reset to Factory Defaults" command. */
} esp_zb_zcl_basic_cmd_id_t;

#ifdef __cplusplus
}
#endif
