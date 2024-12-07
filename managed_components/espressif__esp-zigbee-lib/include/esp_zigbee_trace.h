/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "esp_zigbee_type.h"

/**
 * @brief The esp zigbee SDK trace level value
 * @anchor esp_zb_trace_level_cfg_t
 */
typedef enum {
    ESP_ZB_TRACE_LEVEL_NONE     = -1,
    ESP_ZB_TRACE_LEVEL_CRITICAL = 0,
    ESP_ZB_TRACE_LEVEL_WARN     = 1,
    ESP_ZB_TRACE_LEVEL_INFO     = 2,
    ESP_ZB_TRACE_LEVEL_DEBUG    = 3,
    ESP_ZB_TRACE_LEVEL_VERBOSE  = 4,
} esp_zb_trace_level_cfg_t;

/** The Zigbee SDK trace mask
 * @note Different masks represent different sections of the zigbee stack
 */
#define ESP_ZB_TRACE_SUBSYSTEM_COMMON          0x0001U     /*!< Common subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_MEM             0x0002U     /*!< MEM subsystem (buffers pool) */
#define ESP_ZB_TRACE_SUBSYSTEM_MAC             0x0004U     /*!< MAC subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_NWK             0x0008U     /*!< NWK subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_APS             0x0010U     /*!< APS subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_ZSE             0x0020U     /*!< ZSE subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_ZDO             0x0040U     /*!< ZDO subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_SECUR           0x0080U     /*!< Security subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_ZCL             0x0100U     /*!< ZCL subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_ZLL             0x0200U     /*!< ZLL/Touchlink subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_SSL             0x0400U     /*!< SSL subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_NCP_TRANSPORT   ESP_ZB_TRACE_SUBSYSTEM_SSL
#define ESP_ZB_TRACE_SUBSYSTEM_MACSPLIT        ESP_ZB_TRACE_SUBSYSTEM_NCP_TRANSPORT
#define ESP_ZB_TRACE_SUBSYSTEM_APP             0x0800U     /*!< User Application */
#define ESP_ZB_TRACE_SUBSYSTEM_LWIP            0x1000U     /*!< LWIP is used, else free */
#define ESP_ZB_TRACE_SUBSYSTEM_ALIEN           0x2000U     /*!< Some special debug */
#define ESP_ZB_TRACE_SUBSYSTEM_ZGP             0x4000U     /*!< ZGP subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_MAC_API         0x8000U     /*!< MAC API subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_MACLL           0x10000U    /*!< MAC LL subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_SPECIAL1        0x20000U    /*!< Special subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_BATTERY         0x40000U    /*!< Battery subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_OTA             0x80000U    /*!< OTA subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_TRANSPORT       0x100000U   /*!< Transport subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_USB             0x200000U   /*!< USB subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_SPI             0x400000U   /*!< SPI subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_UART            0x800000U   /*!< UART subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_JSON            0x1000000U  /*!< JSON subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_HTTP            0x2000000U  /*!< HTTP subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_CLOUD           0x4000000U  /*!< Interface to the Cloud */
#define ESP_ZB_TRACE_SUBSYSTEM_ZBDIRECT        0x8000000U  /*!< Zigbee Direct subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_DIAGNOSTIC      0x10000000U /*!< Diagnostic subsystem */
#define ESP_ZB_TRACE_SUBSYSTEM_NS              0x20000000U /*!< Network simulator subsystem */

/**
 * @note Please ensure to call `esp_zb_set_trace_level_mask` before invoking `esp_zb_init`
 * @brief The Zigbee trace configuration.
 *
 * @param[in] trace_level set trace level refer to esp_zb_trace_level_cfg_t
 * @param[in] trace_mask Set trace mask
 *
 */
void esp_zb_set_trace_level_mask(esp_zb_trace_level_cfg_t trace_level, uint32_t trace_mask);

#ifdef __cplusplus
}
#endif
