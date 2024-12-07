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
/*  PURPOSE: Public Multi-MAC layer API
*/
#ifndef ZBOSS_API_MM_H
#define ZBOSS_API_MM_H 1

/** \addtogroup mm_api */
/** @{ */

enum zb_mac_interfaces_counter_e {
  #if defined(ZB_MAC_MONOLITHIC)
    ZB_MAC_INTERFACE_MONOLITHIC,
  #endif
  #if defined(ZB_MACSPLIT_HOST)
    ZB_MAC_INTERFACE_MACSPLIT_HOST,
  #endif
  #if defined(ZB_MAC_BLE)
    ZB_MAC_INTERFACE_BLE,
  #endif
  #if defined(ZB_MACSPLIT_DEVICE)
    ZB_MAC_INTERFACE_MACSPLIT_DEVICE,
  #endif
  #if defined(ZB_MAC_SUBGHZ)
    ZB_MAC_INTERFACE_SUBGHZ,
  #endif
  #if defined(ZB_EXTMAC)
    ZB_MAC_INTERFACE_EXTMAC,
  #endif
    ZB_MAC_INTERFACE_MAX
};

#if defined(ZB_MAC_MONOLITHIC)
  #define ZB_MAC_INTERFACE_MONOLITHIC_ID ((zb_uint8_t)ZB_MAC_INTERFACE_MONOLITHIC)
#endif
#if defined(ZB_MACSPLIT_HOST)
  #define ZB_MAC_INTERFACE_MACSPLIT_HOST_ID ((zb_uint8_t)ZB_MAC_INTERFACE_MACSPLIT_HOST)
#endif
#if defined(ZB_EXTMAC)
  #define ZB_MAC_INTERFACE_EXTMAC_ID ((zb_uint8_t)ZB_MAC_INTERFACE_EXTMAC)
#endif
#if defined(ZB_MAC_BLE)
  #define ZB_MAC_INTERFACE_BLE_ID ((zb_uint8_t)ZB_MAC_INTERFACE_BLE)
#endif
#if defined(ZB_MACSPLIT_DEVICE)
  #define ZB_MAC_INTERFACE_MACSPLIT_DEVICE_ID ((zb_uint8_t)ZB_MAC_INTERFACE_MACSPLIT_DEVICE)
#endif
#if defined(ZB_MAC_SUBGHZ)
  #define ZB_MAC_INTERFACE_SUBGHZ_ID ((zb_uint8_t)ZB_MAC_INTERFACE_SUBGHZ)
#endif

/* C-STAT does not allow to assign ZB_MAC_INTERFACE_MAX to ZB_NWK_MAC_IFACE_TBL_SIZE,
 * so check configurations manually */
#if defined(ZB_MAC_MONOLITHIC) && defined(ZB_MACSPLIT_HOST)
  #define ZB_NWK_MAC_IFACE_TBL_SIZE 2U
#elif defined(ZB_MAC_MONOLITHIC) && defined(ZB_MAC_BLE)
  #define ZB_NWK_MAC_IFACE_TBL_SIZE 2U
#elif defined(ZB_MAC_MONOLITHIC) && !defined(ZB_MACSPLIT_HOST) && !defined(ZB_MAC_BLE) && !defined(ZB_MACSPLIT_DEVICE) && !defined(ZB_MAC_SUBGHZ)
  #define ZB_NWK_MAC_IFACE_TBL_SIZE 1U
#elif defined(ZB_MACSPLIT_HOST) && !defined(ZB_MAC_MONOLITHIC) && !defined(ZB_MAC_BLE) && !defined(ZB_MACSPLIT_DEVICE) && !defined(ZB_MAC_SUBGHZ)
  #define ZB_NWK_MAC_IFACE_TBL_SIZE 1U
#elif defined(ZB_MACSPLIT_DEVICE) && !defined(ZB_MAC_MONOLITHIC) && !defined(ZB_MAC_BLE) && !defined(ZB_MACSPLIT_HOST) && !defined(ZB_MAC_SUBGHZ)
  #define ZB_NWK_MAC_IFACE_TBL_SIZE 1U
  /* MAC-split SoC uses default MAC interface, so for building device with only SoC interface just declare monolithic MAC */
  #define ZB_MAC_INTERFACE_MONOLITHIC_ID ZB_MAC_INTERFACE_MACSPLIT_DEVICE_ID
#elif defined(NCP_MODE_HOST)
  /* Preserve default table size to keep previous stack behaviour, but
   * do not use any interfaces in that case */
  #define ZB_NWK_MAC_IFACE_TBL_SIZE 1U
#elif defined(ZB_EXTMAC)
  #define ZB_NWK_MAC_IFACE_TBL_SIZE 1U
#else
  #error Multi-MAC configuration is not supported, at least one interface should be enabled
#endif

#if ZB_NWK_MAC_IFACE_TBL_SIZE > 1U && defined(ZB_MAC_INTERFACE_SINGLE)
  #error ZB_MAC_INTERFACE_SINGLE requires to enable only one interface
#endif

/**
 * @brief Flag that is used to perform some action on all interfaces together
 */
#define ZB_NWK_MULTIMAC_ALL_INTERFACES 0xFFU

/** @} */ /* mm_api */

#endif /*ZBOSS_API_MM_H*/
