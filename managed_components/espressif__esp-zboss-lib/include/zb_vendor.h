/*
 * Copyright (c) 2021 Espressif Systems (Shanghai) CO LTD
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

#pragma once

#include "sdkconfig.h"
#include "esp_log.h"

#if CONFIG_ZB_ZCZR || CONFIG_ZB_ZED
#include "zb_vendor_default.h"
#elif CONFIG_ZB_ZGPD
#include "zb_vendor_zgpd.h"
#endif

#if CONFIG_ZB_R22_ENABLE
#define ZB_ENABLE_SE
#define ZB_ENABLE_ZGP
#endif

#define ZB_CONFIG_ESP
/* trace */
#define ZB_TRACE_LEVEL CONFIG_ZB_TRACE_LEVEL
#define ZB_TRACE_MASK  CONFIG_ZB_TRACE_MASK

/* use assert in ZBOSS */
#define USE_ASSERT
/* Using for ZBOSS assert and change ZBOSS assert logic*/
#define ESP_ZB_USE_ASSERT

#if defined CONFIG_ESP_ZB_TRACE_ENABLE
#define ESP_ZIGBEE_TRACE
#define ESP_ZBOSS_TRACE_VPRINTF_STACK_BUFFER_SIZE 250
#endif

#ifdef ZB_TRACE_LEVEL
#define ZB_TRACE_TO_PORT
#define ZB_TRACE_FROM_INTR
#define ZB_BINARY_TRACE
#define ZB_TRAFFIC_DUMP_ON
#ifndef ZB_TRACE_OVER_JTAG
#define ZB_TRACE_OVER_USART
#define ZB_HAVE_SERIAL
#define ZB_HAVE_SERIAL_SINGLE
#endif  /* not over jtag */
#endif  /* if trace */

#if defined ZB_TRACE_OVER_USART && defined ZB_TRACE_LEVEL
#define ZB_SERIAL_FOR_TRACE
#endif

/* Macsplit SoC does not use prod cfg indeed, but that define is necessary for constants definition */
#define ZB_PRODUCTION_CONFIG
#define ZB_PROMISCUOUS_MODE

#define DEBUG 1

#if defined ZB_SUBGHZ_ONLY_MODE || defined ZB_R22_MULTIMAC_MODE
#define ZB_MAC_QUEUE_SIZE 15U
#else
#define ZB_MAC_QUEUE_SIZE 10U
#endif

/* The time allotted for adding route information using frame data at the beginning (beacon interval units) */
#define ZB_NWK_AGGRESSIVE_ROUTE_ADD_TIME 10U

/* Memory configuration */
#define ZB_CONFIGURABLE_MEM