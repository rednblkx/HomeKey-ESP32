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
/*  PURPOSE: ZBOSS API header
*/
#ifndef ZBOSS_API_H
#define ZBOSS_API_H 1

#include "zb_version.h"
#include "zb_channel_page.h"
#include "zboss_api_core.h"
#include "zboss_api_buf.h"
#include "zboss_api_internal.h"
#include "zboss_api_nwk.h"
#include "zboss_api_af.h"
#include "zboss_api_zdo.h"
#include "zboss_api_aps.h"
#ifdef ZB_ENABLE_HA
#include "zb_ha.h"
#endif
#include "zb_address.h"
#ifdef ZB_ENABLE_SE_MIN_CONFIG
#include "zboss_api_se.h"
#endif
#ifdef ZB_ENABLE_ZCL
#include "zboss_api_zcl.h"
#endif /* ZB_ENABLE_ZCL */
#ifdef ZB_ENABLE_ZGP
#include "zboss_api_zgp.h"
#endif

/** @cond (!DOXYGEN_ERL_SECTION) */
/*! @cond DOXYGEN_SECUR_SECTION */
/*! @addtogroup secur_api */
/*! @{ */

/** @addtogroup secur_nwk_key NWK keys settings
 * @{
 */
/** @fn void zb_secur_setup_nwk_key(zb_uint8_t *key, zb_uint8_t i)
 *  @brief Setup NWK key from the application level manually.
 *  @param key - key to be used
 *  @param i - key number (0-3)
 *  @details Call this routine at TC to change random nwk key generated from zb_ib_set_defaults
 *    to well-known key. To be used in certification tests mainly.
 *  @snippet simple_gw/simple_gw.c zb_secur_setup_preconfigured_key_value
 *  @snippet simple_gw/simple_gw.c zb_secur_setup_preconfigured_key
 */
void zb_secur_setup_nwk_key(zb_uint8_t *key, zb_uint8_t i);

#ifdef ZB_COORDINATOR_ROLE
/**
 *  Initiate procedure of NWK key switching.
 *
 *  Generate next NWK key if it not exists, broadcast new NWK key, after delay broadcast NWK key switch command.
 *  Can run at TC only.
 *
 *  @param param - work buffer ID or 0 (is zero, function allocates buffer itself)
 */
void zb_secur_nwk_key_switch_procedure(zb_uint8_t param);
#endif /* ZB_COORDINATOR_ROLE */
/** @} */ /* secur_nwk_key */

/** @addtogroup secur_tc_rejoin TC rejoin settings
 * @{
 */
/**
 *  Specifies whether unsecure Trust Center Rejoin is allowed.
 *
 *  If set to ZB_FALSE, devices that attempted unsecure rejoin will not be authorized.
 *
 *  @param enable - whether to enable or disable TC Rejoin.
 */
void zb_secur_set_tc_rejoin_enabled(zb_bool_t enable);
/**
 *  Specifies whether Trust Center Rejoin is ignored.
 *
 *  If set to ZB_TRUE, devices that attempted unsecure rejoin will be ignored.
 *
 *  @param enable - whether to enable or disable TC Rejoin ignore.
 */
void zb_secur_set_ignore_tc_rejoin(zb_bool_t enable);

/**
 *  Specifies whether Trust Center Rejoin is allowed, when there is no unique TCLK.
 *  On joiner device it could be used to perform TC rejoin with legacy ZC.
 *
 *  If set to ZB_TRUE on joiner device, it can make TC rejoin without unique TCLK.
 *
 *  @param enable - whether to enable or disable unsecured TC Rejoin.
 */
void zb_secur_set_unsecure_tc_rejoin_enabled(zb_bool_t enable);

/** @} */ /* secur_tc_rejoin */

/** @cond DOXYGEN_INTERNAL_DOC */
#ifdef ZB_COORDINATOR_ROLE
#ifndef ZB_LITE_NO_TRUST_CENTER_REQUIRE_KEY_EXCHANGE
/**
 *  Initiate procedure of NWK key switching.
 *
 *  Generate link key with device, update link key.
 *  Can run at TC only.
 *
 *  @param addr_of_interest - network address of ZC to ask ZC for KEC.
 */
void se_tc_update_link_key(zb_uint16_t addr_of_interest);
#endif
#endif
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/** @addtogroup secur_ic_usage Install code usage
 * @{
 */

/**
 * @name IC types
 * @anchor ic_types
 */
/** @{ */
#define ZB_IC_TYPE_48  0U
#define ZB_IC_TYPE_64  1U
#define ZB_IC_TYPE_96  2U
#define ZB_IC_TYPE_128 3U
#define ZB_IC_TYPE_MAX 4U
/** @} */

/**
 * @brief Type for IC types.
 *
 * Holds one of @ref ic_types. Kept only for backward compatibility as
 * @ref ic_types were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t zb_ic_types_t;

/**
 * Declares application callback that is called after installcode addition.
 *
 * @param status - status of installcode addition
 */
typedef void (*zb_secur_ic_add_cb_t)(zb_ret_t status);

#if (defined ZB_COORDINATOR_ROLE && defined ZB_SECURITY_INSTALLCODES) || (defined DOXYGEN)
/** @fn void zb_secur_ic_add(zb_ieee_addr_t address, zb_uint8_t ic_type, zb_uint8_t *ic, zb_secur_ic_add_cb_t cb)
 *  @brief Add install code for the device with specified long address
 *  @param[in]  address - long address of the device to add the install code
 *  @param[in]  ic_type - install code type as enumerated in
 *  @param[in]  cb      - callback that will be called after installcode addition
 *  @if ZB_SECUR
 *    @ref ic_types
 *  @else
 *    ic_types
 *  @endif
 *  @param[in]  ic - pointer to the install code buffer
 *  @details This call allows to add install codes for several devices into NVRAM. It makes
 *    sense for Trust Center/Coordinator devices only as usual device doesn't need to have
 *    someone's else install codes.
 *  @note This call is valid only for the TC (ZC)!
 *  @par Example
 *  @snippet ic_sample/ic_zc.c zb_secur_ic_add_usage_1
 *  @snippet ic_sample/ic_zc.c zb_secur_ic_add_usage_2
 */
void zb_secur_ic_add(zb_ieee_addr_t address, zb_uint8_t ic_type, zb_uint8_t *ic, zb_secur_ic_add_cb_t cb);
#endif /* ZB_COORDINATOR_ROLE && ZB_SECURITY_INSTALLCODES */


/** @fn zb_ret_t zb_secur_ic_set(zb_uint8_t ic_type, zb_uint8_t *ic)
 *  @brief Set install code for the device
 *  @param[in]  ic_type - install code type as enumerated in
 *  @if ZB_SECUR
 *    @ref ic_types
 *  @else
 *    ic_types
 *  @endif
 *  @param[in]  ic - pointer to the install code buffer
 *  @details This call allows to set the install code for the current device. It is assumed
 *    that device can have only one install code.
 *  @note This function is to be run at client side (not at TC).
 *  @return RET_OK on success or RET_CONVERSION_ERROR on error in install code
 *  @snippet ic_sample/ic_zr1.c zb_secur_ic_set_usage
 */
zb_ret_t zb_secur_ic_set(zb_uint8_t ic_type, zb_uint8_t *ic);

/** @brief Request for zb_secur_ic_get_list_req.
*/
typedef ZB_PACKED_PRE struct zb_secur_ic_get_list_req_s
{
  zb_uint8_t start_index;    /*!< Starting Index for the requested elements
                               * of the IC table */
  zb_callback_t response_cb; /*!< Callback that will be called on response receiving */
}
ZB_PACKED_STRUCT
zb_secur_ic_get_list_req_t;

/** @brief Response for zb_secur_ic_get_list_req.
*/
typedef ZB_PACKED_PRE struct zb_secur_ic_get_list_resp_s
{
  zb_uint8_t status;                     /*!< The status of the command.*/
  zb_uint8_t ic_table_entries;           /*!< Total number of IC table entries within the device */
  zb_uint8_t start_index;                /*!< Starting index within the IC table */
  zb_uint8_t ic_table_list_count;        /*!< Number of received IC table entries */
}
ZB_PACKED_STRUCT
zb_secur_ic_get_list_resp_t;

/** @brief IC entry type zb_secur_ic_get_list_req
*/
typedef ZB_PACKED_PRE struct zb_secur_ic_entry_s
{
  zb_ieee_addr_t  device_address;         /*!< Partner address */
  zb_uint8_t      options;                /* lowest 2 bits [0-1]: 00-48, 01-64, 10-96, 11-128 bits ic type */
  zb_uint8_t      align;
  zb_uint8_t      installcode[ZB_CCM_KEY_SIZE+ZB_CCM_KEY_CRC_SIZE]; /*!< 16b installcode +2b crc */
} ZB_PACKED_STRUCT zb_secur_ic_entry_t;

/** @brief Request for zb_secur_ic_get_by_idx_req.
  */
typedef ZB_PACKED_PRE struct zb_secur_ic_get_by_idx_req_s
{
  zb_uint8_t ic_index;        /*!< Starting Index for the requested elements
                                * of the IC table */
  zb_callback_t response_cb;  /*!< Callback that will be called on response receiving */
}
ZB_PACKED_STRUCT
zb_secur_ic_get_by_idx_req_t;

/** @brief Response for zb_secur_ic_get_by_idx_req.
  */
typedef ZB_PACKED_PRE struct zb_secur_ic_get_by_idx_resp_s
{
  zb_uint8_t status;                           /*!< The status of the command.*/
  zb_ieee_addr_t device_address;               /*!< Partner address */
  zb_ic_types_t ic_type;                       /*!< Installcode type.*/
  zb_uint8_t installcode[ZB_CCM_KEY_SIZE+ZB_CCM_KEY_CRC_SIZE];   /*!< 16b installcode +2b crc */
  zb_uint8_t ic_index;                        /*!< Starting Index for the requested elements */
}
ZB_PACKED_STRUCT
zb_secur_ic_get_by_idx_resp_t;

/** @brief Request for zb_secur_ic_remove_req.
  */
typedef ZB_PACKED_PRE struct zb_secur_ic_remove_req_s
{
  zb_ieee_addr_t device_address; /*!< Address of the device to remove installcode */
  zb_callback_t response_cb;     /*!< Callback that will be called on response receiving */
}
ZB_PACKED_STRUCT
zb_secur_ic_remove_req_t;

/** @brief Response for zb_secur_ic_remove_req.
  */
typedef ZB_PACKED_PRE struct zb_secur_ic_remove_resp_s
{
  zb_uint8_t status; /*!< The status of the command.*/
}
ZB_PACKED_STRUCT
zb_secur_ic_remove_resp_t;

/** @brief Request for zb_secur_ic_remove_all_req.
  */
typedef ZB_PACKED_PRE struct zb_secur_ic_remove_all_req_s
{
  zb_callback_t response_cb;     /*!< Callback that will be called on response receiving */
}
ZB_PACKED_STRUCT
zb_secur_ic_remove_all_req_t;

/** @brief Response for zb_secur_ic_remove_all_req.
  */
typedef ZB_PACKED_PRE struct zb_secur_ic_remove_all_resp_s
{
  zb_uint8_t status;  /*!< The status of the command.*/
}
ZB_PACKED_STRUCT
zb_secur_ic_remove_all_resp_t;

#if defined ZB_COORDINATOR_ROLE && defined ZB_SECURITY_INSTALLCODES
/**
 * @brief Get list of the install codes.
 * It is valid only for the TC (ZC).
 *
 * @param param buffer with request parameters, will be also used to store response.
 *
 *  @snippet ic_sample/ic_zc.c zb_secur_ic_get_list_req_usage
*/
void zb_secur_ic_get_list_req(zb_uint8_t param);

/**
 * @brief Get the install code by index.
 * It is valid only for the TC (ZC).
 *
 * @param param buffer with request parameters, will be also used to store response.
 *  @snippet ic_sample/ic_zc.c zb_secur_ic_get_by_idx_req_usage
*/
void zb_secur_ic_get_by_idx_req(zb_uint8_t param);

/**
 * @brief Remove the install code for the device with specified long
 * address.
 * It is valid only for the TC (ZC).
 *
 * @param param buffer with request parameters, will be also used to store response.
 * @snippet ic_sample/ic_zc.c zb_secur_ic_remove_req_usage
*/
void zb_secur_ic_remove_req(zb_uint8_t param);

/**
 * Remove the install code for all devices.
 * It is valid only for the TC (ZC).
 *
 * @param param buffer with request parameters, will be also used to store response.
 * @snippet ic_sample/ic_zc.c zb_secur_ic_remove_all_req_usage
*/
void zb_secur_ic_remove_all_req(zb_uint8_t param);
#endif /* ZB_COORDINATOR_ROLE && ZB_SECURITY_INSTALLCODES */

/** @fn zb_ret_t zb_secur_ic_str_set(char *ic_str)
 *  @brief Set install code for the device from character string
 *  @param[in]  ic_str - install code text string
 *  @details This call allows to set the install code for the current device. It is assumed
 *    that device can have only one install code.
 *  @note This function is to be run at client side (not at TC).
 *  @returns RET_OK on success.
 *  @snippet ic_sample/ic_zr2.c zb_secur_ic_str_set_usage_1
 *  @snippet ic_sample/ic_zr2.c zb_secur_ic_str_set_usage_2
 */
zb_ret_t zb_secur_ic_str_set(char *ic_str);

#ifndef ZB_USE_INTERNAL_HEADERS

/** @fn void zb_secur_ic_str_add(zb_ieee_addr_t address, char *ic_str, zb_secur_ic_add_cb_t cb)
 *  @brief Add install code for the device from character string
 *  @param[in]  address - long address of the device to add the install code for
 *  @param[in]  ic_str  - install code text string
 *  @param[in]  cb      - callback that will be called after installcode addition
 *  @details This call allows to add install codes for several devices into NVRAM. It makes
 *    sense for Trust Center/Coordinator devices only as usual device doesn't need to have
 *    someone's else install codes.
 *  @note This call is valid only for the TC (ZC)!
 *  @snippet ic_sample/ic_zc.c zb_secur_ic_str_add_usage
 */
void zb_secur_ic_str_add(zb_ieee_addr_t address, char *ic_str, zb_secur_ic_add_cb_t cb);

#endif /* ZB_USE_INTERNAL_HEADERS */

/**
   Set installcode policy flag.

   @param allow_ic_only - use ZB_TRUE value to check installcodes
   @snippet ic_sample/ic_zc.c zb_set_installcode_policy_usage
*/
void zb_set_installcode_policy(zb_bool_t allow_ic_only);
/** @} */ /* secur_ic_usage */

#ifdef ZB_ROUTER_ROLE
/** @cond DOXYGEN_INTERNAL_DOC */
/**
   Disable APS-encryption of Transport Key from the ZC.

   This valid is only for the TC (ZC).
*/
void zb_disable_transport_key_aps_encryption(void);
/**
   Enable APS-encryption of Transport Key from the ZC.

   This valid is only for the TC (ZC).
*/
void zb_enable_transport_key_aps_encryption(void);
/**
   Get the status of the APS-encryption of Transport Key from the ZC.

   This valid is only for the TC (ZC).

   @return ZB_TRUE when APS-encryption of Transport Key is enabled, ZB_FALSE otherwise
*/
zb_bool_t zb_is_transport_key_aps_encryption_enabled(void);
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
#endif /* ZB_ROUTER_ROLE */

/*! @} */ /* se_secur */
/*! @endcond */ /* DOXYGEN_SECUR_SECTION */
/*! @endcond */ /* (!DOXYGEN_ERL_SECTION) */

/*! @cond DOXYGEN_GENERAL_SECTION */
/*! @addtogroup zboss_general_api */
/*! @{ */

#ifdef DOXYGEN
/**
   ZBOSS application entry point.

   Application program using ZBOSS must use MAIN() as an entry point and do not implement function main().
   At some, but not at all platforms MAIN() is an alias for main().
 */
#define MAIN() main(a,v)


/**
   Return statement of ZBOSS application entry point.
   Corresponds to exit() call from main().
   Platform-dependent.
 */
#define MAIN_RETURN(retc) return (retc)

#endif  /* DOXYGEN */

/*! @} */ /* zboss_general_api */

/** @cond DOXYGEN_INTERNAL_DOC */
/**
   Check whether the right library is selected at application compilation time

   To be called from ZB_INIT() at start.
 */
#ifdef ZB_ED_ROLE
#define ZB_CHECK_LIBRARY()  since_you_got_that_symbol_unresolved_you_probably_use_ZB_ED_ROLE_preprocessor_define_while_linking_with_zc_library();
void since_you_got_that_symbol_unresolved_you_probably_use_ZB_ED_ROLE_preprocessor_define_while_linking_with_zc_library(void);
#else
#define ZB_CHECK_LIBRARY()  since_you_got_that_symbol_unresolved_you_probably_forget_use_ZB_ED_ROLE_preprocessor_define_while_linking_with_zed_library();
void since_you_got_that_symbol_unresolved_you_probably_forget_use_ZB_ED_ROLE_preprocessor_define_while_linking_with_zed_library(void);
#endif /* ZB_ED_ROLE */


/** @endcond*/ /* DOXYGEN_INTERNAL_DOC */

/*! @addtogroup zb_general_start */
/*! @{ */
#if defined ZB_INIT_HAS_ARGS || defined DOXYGEN
/**
   Global stack initialization.

   To be called from MAIN() at start.

   Usual initialization sequence: ZB_INIT(), then assign some IB values, then zboss_start().

   @param trace_comment - trace file name component (valid for Unix, ignored at MCU)

   @b Example:
@code
  ZB_INIT("zdo_zc");
@endcode
 */
#define ZB_INIT(trace_comment)               \
{                                            \
  ZB_CHECK_LIBRARY();                        \
  zb_init((zb_char_t *)trace_comment);       \
}
/** @cond DOXYGEN_INTERNAL_DOC */
/** @brief Global stack initialization.

    Don't call directly, use ZB_INIT() instead
*/
void zb_init(zb_char_t *trace_comment);
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
#else
/**
   Global stack initialization.

   To be called from MAIN() at start.

   Usual initialization sequence: ZB_INIT(), then assign some IB values, then zboss_start().

   @param trace_comment - trace file name component (valid for Unix, ignored at MCU)

   @b Example:
@code
  ZB_INIT("zdo_zc");
@endcode
 */
#define ZB_INIT(trace_comment)               \
{                                            \
  ZB_CHECK_LIBRARY();                        \
  zb_init();                                 \
}
void zb_init(void);
#endif  /* ZB_INIT_HAS_ARGS || defined DOXYGEN */
/*! @} */ /* zb_general_start */

/*! @addtogroup zb_general_set */
/*! @{ */
/**
   Set channel mask on device.
   @param channel_mask - Zigbee channel mask

   @b Example:
@code
  zb_set_channel((1l<<13) | (1l<<14));
@endcode
*/
void zb_set_channel_mask(zb_uint32_t channel_mask);

/** @cond DOXYGEN_BDB_SECTION */

#ifdef ZB_OSIF_CONFIGURABLE_TX_POWER
/**
   Set transmit power.
   @param tx_power - Transmit power [dBm].

   @b Example:
@code
  zb_set_tx_power(0x32);
@endcode
*/
zb_ret_t zb_set_tx_power(zb_uint8_t tx_power);
#endif

#ifdef ZB_MAC_CONFIGURABLE_TX_POWER

/**
 * @brief A struct with params for zb_get_tx_power_async and zb_set_tx_power_async
 */
typedef struct zb_tx_power_params_s
{
  zb_ret_t status;    /*!< Status of the operation. Can be RET_OK, RET_UNINITIALIZED or one of
                            RET_INVALID_PARAMETET_1, RET_INVALID_PARAMETET_2 or RET_INVALID_PARAMETET_3
                            for the following three fields respectively. */
  zb_uint8_t page;    /*!< Page number. Should be provided by the application. */
  zb_uint8_t channel; /*!< Channel number on a given page. Should be provided by the application. */
  zb_int8_t tx_power; /*!< Transceiver power for a given page and channel.
                            Should be provided by the application in case of setting the power */
  zb_callback_t cb;   /*!< Callback function to be called after the function finishes. Should be provided by the application. */
} zb_tx_power_params_t;


/**
 * @brief Get transceiver power for given page and channel asynchronously.
 *
 * This function requires param to contain @ref zb_tx_power_params_t.
 * Will return status RET_UNINITIALIZED if the channel/page storage hasn't been initialized yet.
 *
 * @param param - buffer, containing @ref zb_tx_power_params_t.
*/
void zb_get_tx_power_async(zb_bufid_t param);

/**
 * @brief Set transceiver power to a given value on a given page and channel asynchronously.
 *
 * This function requires param to contain @ref zb_tx_power_params_t.
 * If the power change is for the current channel, the function will attempt to change power immediately,
 *  otherwise it will save the change until channel switch.
 *
 * @param param - buffer, containing @ref zb_tx_power_params_t.
*/
void zb_set_tx_power_async(zb_bufid_t param);
#endif

/** @endcond */ /* DOXYGEN_BDB_SECTION */

/**
   Set RxOnWhenIdle attribute
   @param rx_on - attribute value

   @b Example:
@snippet ias_zone_sensor/src/izs_device.c set_rx_on_when_idle_attribute
*/
void zb_set_rx_on_when_idle(zb_bool_t rx_on);

/*! @} */ /* zb_general_set */

/*! @addtogroup zb_general_get */
/*! @{ */

/**
   Returns current state of RX-ON-When-Idle of Zigbee ZED

   @return ZB_TRUE if ZED is not sleepy ZED, ZB_FALSE if ZED is a sleepy ZED
 */
zb_bool_t zb_get_rx_on_when_idle(void);

/*! @} */ /* zb_general_get */

/*! @addtogroup zb_general_start */
/*! @{ */

/** @brief ZBOSS start function.
  *
  * Typical device start: init, load some parameters from NVRAM and proceed with startup.
  *
  * Startup means either Formation (for ZC), rejoin or discovery/association join.  After startup
  * complete @ref zboss_signal_handler callback is called, so application will know when to do
  * some useful things.
  *
  * Precondition: stack must be inited by @ref ZB_INIT() call. @ref ZB_INIT() sets default IB
  * parameters, so caller has a chance to change some of them. Note that NVRAM will be loaded after
  * zboss_start() call.
  * @note ZB is not looped in this routine. Instead, it schedules callback and returns.  Caller
  * must run @ref zboss_main_loop() after this routine.
  *
  * @return RET_OK on success.
  */
zb_ret_t zboss_start(void);


/*! @} */ /* zb_general_start  */

/*! @addtogroup zb_general_get */
/*! @{ */
/**
 *  @brief Get ZBOSS version.
 *  @returns Pointer to zero-terminated version string.
 */
const zb_char_t ZB_IAR_CODE *zb_get_version(void);

/**
 *  @brief Get ZBOSS numeric version.
 *  @returns (MAJOR << 24 | MINOR << 16 | REVISION)
 */
zb_uint32_t zboss_version_get(void);
/*! @} */ /* zb_general_get */

/*! @addtogroup zb_general_start */
/*! @{ */

/**
   Start ZBOSS with only minimal initialization, without starting commissioning.

   This function initializes scheduler and buffers pool, but not MAC and upper layers.
   Typically zboss_start_no_autostart() is used when application wants to do something before
   starting joining the network.

   For example, you can use this function if it is needed to enable leds, timers
   or any other devices on periphery to work with them before starting working in a network. It's
   also useful if you want to run something locally during joining.

   Application should later call ZBOSS commissioning initiation - for
   instance,
   @if zb_bdb_mode
      @ref bdb_start_top_level_commissioning().
   @else
      bdb_start_top_level_commissioning()
   @endif


   To finish node initialization without triggering commissioning call @ref
   bdb_start_top_level_commissioning() with commissioning mask 0 (no steering, no formation,
   initialization only).

   @return RET_OK on success.
 */
zb_ret_t zboss_start_no_autostart(void);

void zboss_start_continue(void);


#ifdef ZB_ZBOSS_DEINIT

#if defined ZB_MACSPLIT_HOST || defined ZB_EXTMAC
/**
   Initiate ZBOSS shut with pseudo reset procedure.

   ZBOSS shutdown with pseudo reset as well as zboss_start_shut is meaningful for Linux platform where it is necessary to stop
   or restart ZBOSS without stopping the current process.

   It is used when it is impossible to reset MAC layer on MACSPLIT architecture and resets only host.

   That function must be called after application received @ref ZB_SIGNAL_READY_TO_SHUT signal.
   It then must call @ref zboss_complete_shut() and must not use ZBOSS afterwords.

 */
void zboss_shut_with_host_reset(zb_bufid_t param);
#endif /* #if defined ZB_MACSPLIT_HOST || defined ZB_EXTMAC */

/**
   Initiate ZBOSS aborting procedure.

   ZBOSS abort as well as zboss_start_shut is meaningful for Linux platform where it is necessary to stop
   or restart ZBOSS without stopping the current process.

   Important use case is OOM situation, when it's impossible to allocate a buffer to reset MAC layer and send @ref ZB_SIGNAL_READY_TO_SHUT signal.

   The function doesn't send @ref ZB_SIGNAL_READY_TO_SHUT signal to application and calls @ref zboss_complete_shut() directly.
 */
void zboss_abort(void);

/**
   Initiate ZBOSS shut procedure.

   ZBOSS shutdown is meaningful for Linux platform where it is necessary to stop
   or restart ZBOSS without stopping the current process.

   When ZBOSS is ready to be shut, application receives @ref ZB_SIGNAL_READY_TO_SHUT signal.
   It then must call @ref zboss_complete_shut() and must not use ZBOSS afterwords.
 */
void zboss_start_shut(zb_bufid_t param);

/**
   Complete ZBOSS shut procedure.

   ZBOSS shutdown is meaningful for Linux platform where it is necessary to stop
   or restart ZBOSS without stopping the current process.

   That function must be called after application received @ref
   ZB_SIGNAL_READY_TO_SHUT signal.
 */
void zboss_complete_shut(void);
#endif  /* #ifdef ZB_ZBOSS_DEINIT */

#ifdef ZB_INTERPAN_PREINIT
/**
   Start ZBOSS pre-init to be able to send inter-pan before Formation/Join

   That function is to be called after ZB_ZDO_SIGNAL_SKIP_STARTUP signal received, before commissioning start.
   After pre-init complete ZBOSS issues ZB_SIGNAL_INTERPAN_PREINIT.
   After that it is possible to use zb_intrp_data_request_with_chan_change() API.

   @param param - work buffer
*/
void zboss_preinit_for_interpan(zb_bufid_t param);
#endif  /* #ifdef ZB_INTERPAN_PREINIT */

#ifdef ZB_PROMISCUOUS_MODE

/**
   Start ZBOSS in the sniffer mode

   Initialize ZBOSS MAC layer to work as a sniffer.
   Once ZBOSS is initialized in the sniffer mode, it can't be commissioned in the
   normal mode until reboot.
 */
zb_ret_t zboss_start_in_sniffer_mode(void);

/**
   Start sniffing

   ZBOSS must be started in the sniffer mode.

   @param data_ind_cb - callback to be called to pass data to the sniffer application
 */
void zboss_sniffer_start(zb_callback_t data_ind_cb);

/**
   Stop sniffing or do nothing if not sniffing now.

   ZBOSS must be started in the sniffer mode.
 */
void zboss_sniffer_stop(void);
#endif  /* ZB_PROMISCUOUS_MODE */

/*! @} */ /* zb_general_start */

/*! @addtogroup zb_general_get */
/*! @{ */

/** @cond DOXYGEN_INTERNAL_DOC */
zb_bool_t zb_zdo_joined(void);
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/**
   Checks that device is joined to the network.
   @return true if device is joined; false - otherwise
*/
#define ZB_JOINED()  zb_zdo_joined()

/*! @} */ /* zb_general_get */

/*! @addtogroup zb_general_main_loop */
/*! @{ */

/**
   ZBOSS main loop.
   Must be called after ZB_INIT() and zboss_start().
   Loops infinitely.

   @snippet light_sample/dimmable_light/bulb.c zboss_main_loop_init
   @snippet light_sample/dimmable_light/bulb.c zboss_main_loop
*/
void zboss_main_loop(void);

/**
   ZBOSS main loop iteration.

   This function is useful if application wants to do something special in the
   main loop without involving ZBOSS scheduler.

   Must be called after ZB_INIT() and zboss_start() / zboss_start_no_autostart()
   inside the application's main cycle.
   @snippet onoff_server/on_off_switch_zed.c zboss_main_loop_iteration_declaring
   @snippet onoff_server/on_off_switch_zed.c zboss_main_loop_iteration_usage
*/

void zboss_main_loop_iteration(void);

/*! @} */ /* zb_general_main_loop */

/*! @addtogroup zb_general_set */
/*! @{ */

/**
   Set 64-bit long address
   @param addr - long address structure
*/
void zb_set_long_address(const zb_ieee_addr_t addr);

/** @fn void zb_set_pan_id(zb_uint16_t pan_id)
 *  @brief Set 16-bit PAN ID
 *  @param[in] pan_id - PAN ID
 */
void zb_set_pan_id(zb_uint16_t pan_id);


/**
 * Declares callback that is called after setting manufacturer code
 *
 * @param status - status of manufacturer code setting
 */
typedef void (*zb_set_manufacturer_code_cb_t)(zb_ret_t status);

/**
 * Set Manufacturer code of Node Descriptor Request
 *
 * @param manuf_code - manufacturer code to set
 * @param cb         - callback that is called after manufacturer code setting
*/
void zb_set_node_descriptor_manufacturer_code_req(zb_uint16_t manuf_code, zb_set_manufacturer_code_cb_t cb);

/**
   Set Extended Pan ID (apsUseExtendedPANID attribute)
   @param ext_pan_id - Long (64-bit) Extended Pan ID
*/
void zb_set_extended_pan_id(const zb_ext_pan_id_t ext_pan_id);

/*! @} */ /* zb_general_set */

/*! @addtogroup zb_general_get */
/*! @{ */

/**
   Get 64-bit long address
   @param addr - pointer to memory where result will be stored
 */
void zb_get_long_address(zb_ieee_addr_t addr);

/**
    Get 16-bit PAN ID
*/
zb_uint16_t zb_get_pan_id(void);

/** Get current short address of the device
 */
zb_uint16_t zb_get_short_address(void);

/**
   Get Extended Pan ID (nwkExtendedPANId attribute)
   @param ext_pan_id - pointer to memory where result will be stored
 */
void zb_get_extended_pan_id(zb_ext_pan_id_t ext_pan_id);

/**
   Get the currently used channel page.
*/
zb_uint8_t zb_get_current_page(void);

/**
   Get the currently used channel.
*/
zb_uint8_t zb_get_current_channel(void);

#ifdef ZB_ENABLE_PTA
/**
   Enable or disable PTA

   This API can be called before zboss_start but after ZB_INIT or after stack is
   started already.

   Ok to call it between ZB_INIT and zboss_start/zboss_start_no_autostart or after
   receiving signals ZB_ZDO_SIGNAL_SKIP_STARTUP or
   ZB_BDB_SIGNAL_DEVICE_FIRST_START.  Do not call it when ZBOSS start is already
   initiated by, for example, zboss_start_continue() or
   bdb_start_top_level_commissioning() but commissioning is not completed yet
   (ZB_BDB_SIGNAL_DEVICE_FIRST_START is not received), else there is a risk of race
   conditions.

   @param state: 0 - disable PTA, 1 - enable PTA
*/
void zb_enable_pta(zb_uint8_t state);
#endif

/*! @} */ /* zb_general_get */

/*! @addtogroup zb_general_set */
/*! @{ */

#ifdef ZB_COORDINATOR_ROLE
/**
   Initiate device as a Zigbee 3.0 (not SE!) coordinator
   @param channel_mask - Zigbee channel mask

   @note BDB channel sets that are set using @ref zb_set_bdb_primary_channel_set() and @ref
   zb_set_bdb_secondary_channel_set(), are always reset to zero after changing network role of the device.
*/
void zb_set_network_coordinator_role(zb_uint32_t channel_mask);
#endif /* ZB_COORDINATOR_ROLE */

#if defined ZB_ROUTER_ROLE && defined ZB_BDB_MODE && !defined BDB_OLD
/**
   Initiate device as a Zigbee Zigbee 3.0 (not SE!) router
   @param channel_mask - Zigbee channel mask

   @note BDB channel sets that are set using @ref zb_set_bdb_primary_channel_set() and @ref
   zb_set_bdb_secondary_channel_set(), are always reset to zero after changing network role of the device.
*/
void zb_set_network_router_role(zb_uint32_t channel_mask);
#endif /* ZB_ROUTER_ROLE && ZB_BDB_MODE && !BDB_OLD */

#if defined ZB_ED_FUNC && defined ZB_BDB_MODE && !defined BDB_OLD
/**
   Initiate device as a Zigbee Zigbee 3.0 (not SE!) End Device
   @param channel_mask - Zigbee channel mask

   @note BDB channel sets that are set using @ref zb_set_bdb_primary_channel_set() and @ref
   zb_set_bdb_secondary_channel_set(), are always reset to zero after changing network role of the device.
*/
void zb_set_network_ed_role(zb_uint32_t channel_mask);
#endif /* ZB_ED_FUNC && ZB_BDB_MODE && !BDB_OLD */

#ifndef ZB_USE_INTERNAL_HEADERS

#ifdef ZB_COORDINATOR_ROLE
/**
   Initiate device as a legacy (pre-r21) Zigbee coordinator
   @param channel_mask - Zigbee channel mask
*/
void zb_set_network_coordinator_role_legacy(zb_uint32_t channel_mask);
#endif /* ZB_COORDINATOR_ROLE */

#ifdef ZB_ROUTER_ROLE
/**
   Initiate device as a legacy (pre-r21) Zigbee router
   @param channel_mask - Zigbee channel mask
*/
void zb_set_network_router_role_legacy(zb_uint32_t channel_mask);
#endif /* ZB_ROUTER_ROLE */

#ifdef ZB_ED_FUNC
/**
   Initiate device as a legacy (pre-r21) Zigbee End Device
   @param channel_mask - Zigbee channel mask
*/
void zb_set_network_ed_role_legacy(zb_uint32_t channel_mask);
#endif /* ZB_ED_FUNC */

#endif /* ZB_USE_INTERNAL_HEADERS */

/** @cond DOXYGEN_SUBGHZ_FEATURE */
#ifdef ZB_COORDINATOR_ROLE
/**
   Initiate device as a Zigbee 3.0 BDB coordinator with channel list.
   Provides functionality to set mask for Sub-GHz and 2.4GHz page.
   @param channel_list - Zigbee channels list
*/
void zb_set_network_coordinator_role_ext(zb_channel_list_t channel_list);
#endif /* ZB_COORDINATOR_ROLE */

#if defined ZB_ROUTER_ROLE && defined ZB_BDB_MODE && !defined BDB_OLD
/**
   Initiate device as a Zigbee 3.0 BDB router with channel list.
   Provides functionality to set mask for Sub-GHz and 2.4GHz page.
   @param channel_list - Zigbee channels list
*/
void zb_set_network_router_role_ext(zb_channel_list_t channel_list);
#endif /* ZB_ROUTER_ROLE && ZB_BDB_MODE && !BDB_OLD */

#if defined ZB_ED_FUNC && defined ZB_BDB_MODE && !defined BDB_OLD
/**
   Initiate device as a Zigbee 3.0 BDB End Device with channel list.
   Provides functionality to set mask for Sub-GHz and 2.4GHz page.
   @param channel_list - Zigbee channels list
*/
void zb_set_network_ed_role_ext(zb_channel_list_t channel_list);
#endif /* ZB_ED_FUNC && ZB_BDB_MODE && !BDB_OLD */
/** @endcond */ /* DOXYGEN_SUBGHZ_FEATURE */

/** @} */

/*! @addtogroup zb_general_get */
/*! @{ */

/**
 * Returns the device role ID
 *
 * @return - zb_nwk_device_type_t device_role_id
 */
zb_nwk_device_type_t zb_get_network_role(void);

/**
 * Returns the maximum number of children allowed
 */
zb_uint8_t zb_get_max_children(void);

/*! @} */ /* zb_general_get */

/** @addtogroup zb_general_set
@{
*/

/** @cond DOXYGEN_SE_SECTION */
#if defined ZB_SE_COMMISSIONING && defined ZB_SE_BDB_MIXED
void zb_se_set_bdb_mode_enabled(zb_bool_t enabled);
#endif /* ZB_SE_COMMISSIONING && ZB_SE_BDB_MIXED */
/** @endcond */ /* DOXYGEN_SE_SECTION */

/**
 * @name Channel pages' numbers
 * @anchor channel_pages_numbers
 *
 * Note: These values were members of `enum zb_channel_page_num_e` type but were converted to a
 * set of macros due to MISRA violations.
 */
/** @{ */
#define ZB_CHANNEL_PAGE0_2_4_GHZ   0U
#define ZB_CHANNEL_PAGE23_SUB_GHZ  23U
#define ZB_CHANNEL_PAGE24_SUB_GHZ  24U
#define ZB_CHANNEL_PAGE25_SUB_GHZ  25U
#define ZB_CHANNEL_PAGE26_SUB_GHZ  26U
#define ZB_CHANNEL_PAGE27_SUB_GHZ  27U
#define ZB_CHANNEL_PAGE28_SUB_GHZ  28U
#define ZB_CHANNEL_PAGE29_SUB_GHZ  29U
#define ZB_CHANNEL_PAGE30_SUB_GHZ  30U
#define ZB_CHANNEL_PAGE31_SUB_GHZ  31U
/** @} */

/**
   Initialize a channel list
   @param channel_list - pointer to a channel list
 */
void zb_channel_list_init(zb_channel_list_t channel_list);


/**
   Add channel mask for a specified channel page in a channel list
   @param channel_list - pointer to a channel list
   @param page_num - channel page number - @ref channel_pages_numbers
   @param channel_mask - Zigbee channel mask

   @return RET_OK if ok, else error code
 */
zb_ret_t zb_channel_list_add(zb_channel_list_t channel_list, zb_uint8_t page_num, zb_uint32_t channel_mask);

/**
   Set the maximum number of connected devices. Used for coordinators
   and routers
   @param max_children - maximum number of connected devices.
*/
void zb_set_max_children(zb_uint8_t max_children);

/**
 * @name End Device (ED) timeout request
 * @anchor nwk_requested_timeout
 */
/** @{ */
#define ED_AGING_TIMEOUT_10SEC    0U  /*!< 10 second timeout */
#define ED_AGING_TIMEOUT_2MIN     1U  /*!< 2 minutes */
#define ED_AGING_TIMEOUT_4MIN     2U  /*!< 4 minutes */
#define ED_AGING_TIMEOUT_8MIN     3U  /*!< 8 minutes */
#define ED_AGING_TIMEOUT_16MIN    4U  /*!< 16 minutes */
#define ED_AGING_TIMEOUT_32MIN    5U  /*!< 32 minutes */
#define ED_AGING_TIMEOUT_64MIN    6U  /*!< 64 minutes */
#define ED_AGING_TIMEOUT_128MIN   7U  /*!< 128 minutes */
#define ED_AGING_TIMEOUT_256MIN   8U  /*!< 256 minutes */
#define ED_AGING_TIMEOUT_512MIN   9U  /*!< 512 minutes */
#define ED_AGING_TIMEOUT_1024MIN  10U /*!< 1024 minutes */
#define ED_AGING_TIMEOUT_2048MIN  11U /*!< 2048 minutes */
#define ED_AGING_TIMEOUT_4096MIN  12U /*!< 4096 minutes */
#define ED_AGING_TIMEOUT_8192MIN  13U /*!< 8192 minutes */
#define ED_AGING_TIMEOUT_16384MIN 14U /*!< 16384 minutes */
/** @} */

/**
 * @brief Type for End Device (ED) timeout request
 *
 * @deprecated holds one of @ref nwk_requested_timeout. Kept only for backward compatibility as
 * @ref nwk_requested_timeout were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t nwk_requested_timeout_t;

/**
   Set end device timeout.

   Use it on End Device to specify End Device timeout period used in ED Timeout request.
   @param to - index in @ref nwk_requested_timeout
*/
void zb_set_ed_timeout(zb_uint_t to);

/**
   Set keep alive timeout.

   Use it on End Device to specify how often device should poll its parent.
   @param to - timeout, in beacon intervals
*/
void zb_set_keepalive_timeout(zb_uint_t to);

/**
 * @name ZC/ZR supported keepalive methods
 * @anchor keepalive_supported_method
 */
/** @{ */
#define ED_KEEPALIVE_DISABLED        0U  /*!< ZC/ZR doesn't support Keepalive feature */
#define MAC_DATA_POLL_KEEPALIVE      1U  /*!< ZC/ZR supports Keepalive feature by means of MAC Data Poll */
#define ED_TIMEOUT_REQUEST_KEEPALIVE 2U  /*!< ZC/ZR supports Keepalive feature by means of ED Timeout Request */
#define BOTH_KEEPALIVE_METHODS       3U  /*!< ZC/ZR supports both MAC Data Poll and ED Timeout Request as Keepalive methods */
/** @} */

/**
 * @brief Type for ZC/ZR supported keepalive methods.
 *
 * Holds one of @ref keepalive_supported_method. Kept only for backward compatibility as
 * @ref keepalive_supported_method were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t nwk_keepalive_supported_method_t;


/**
   Set keep alive mode.

   Use it to set which method a device should use in poll context.
   @param mode - the keepalive mode a device wants to set
*/
void zb_set_keepalive_mode(nwk_keepalive_supported_method_t mode);

/** @} */ /* zb_general_set */


/**
 *  @addtogroup zb_comm_signals
 * @{
 */


#ifndef ZB_USE_INTERNAL_HEADERS

/**
   ZBOSS application signal handler.

   Mandatory for all applications implemented on the top of ZBOSS stack.
   See @ref zdo_app_signal_type.

   @param param - reference to the buffer which contains signal. See @ref zb_get_app_signal.
*/
void zboss_signal_handler(zb_uint8_t param);

#endif /* ZB_USE_INTERNAL_HEADERS */

/** @} */ /* zb_comm_signals */

/*! @endcond */ /* DOXYGEN_GENERAL_SECTION */

/*! @cond DOXYGEN_LL_SECTION */
/*! @addtogroup zboss_nvram */
/** @{ */

/**
 * Enum All NVRAM dataset types
 *
 * ZB_NVRAM_DATASET_NUMBER - count of dataset types. Not a real dataset type!
 */
typedef enum zb_nvram_dataset_types_e
{
  /* Do not use 0 as dataset type: it can be used internally */
  ZB_NVRAM_RESERVED              = 0, /**< Reserved value */
  ZB_NVRAM_COMMON_DATA           = 1, /**< Dataset, contains common Zigbee data */
  ZB_NVRAM_HA_DATA               = 2, /**< Dataset, contains HA profile Zigbee data */
  ZB_NVRAM_ZCL_REPORTING_DATA    = 3, /**< Dataset, contains ZCL reporting data */
  ZB_NVRAM_APS_SECURE_DATA_GAP   = 4, /**< Reserved value */
  ZB_NVRAM_APS_BINDING_DATA_GAP  = 5, /**< Reserved value */
  ZB_NVRAM_HA_POLL_CONTROL_DATA  = 6, /**< Dataset, contains HA POLL CONTROL data */
  ZB_IB_COUNTERS                 = 7, /**< Dataset, contains NIB outgoing frame counter */
  ZB_NVRAM_DATASET_GRPW_DATA     = 8, /**< Green Power dataset */
  ZB_NVRAM_APP_DATA1             = 9, /**< Application-specific data #1 */
  ZB_NVRAM_APP_DATA2             = 10, /**< Application-specific data #2 */
  ZB_NVRAM_ADDR_MAP              = 11, /**< Dataset stores address map info */
  ZB_NVRAM_NEIGHBOUR_TBL         = 12, /**< Dataset stores Neighbor table info */
  ZB_NVRAM_INSTALLCODES          = 13, /**< Dataset contains APS installcodes data */
  ZB_NVRAM_APS_SECURE_DATA       = 14, /**< Dataset, contains APS secure keys data */
  ZB_NVRAM_APS_BINDING_DATA      = 15, /**< Dataset, contains APS binding data */
  ZB_NVRAM_DATASET_GP_PRPOXYT    = 16, /**< Green Power Proxy table */
  ZB_NVRAM_DATASET_GP_SINKT      = 17, /**< Green Power Sink table */
  ZB_NVRAM_DATASET_GP_CLUSTER    = 18, /**< Green Power Cluster data */
  ZB_NVRAM_APS_GROUPS_DATA       = 19, /**< Dataset, contains APS groups data */
  ZB_NVRAM_DATASET_SE_CERTDB     = 20, /**< Smart Energy Dataset - Certificates DataBase */
  ZB_NVRAM_ZCL_WWAH_DATA         = 21, /**< Dataset, contains ZCL WWAH data */
  ZB_NVRAM_DATASET_GP_APP_TBL    = 22, /**< Dataset, contains ZCL WWAH data */
  /* Note: added new app_data datasets down and created a hole for new system datasets.
   */
  ZB_NVRAM_APP_DATA3             = 27, /**< Application-specific data #3 */
  ZB_NVRAM_APP_DATA4             = 28, /**< Application-specific data #4 */
  ZB_NVRAM_KE_WHITELIST          = 29,
  ZB_NVRAM_ZDO_DIAGNOSTICS_DATA  = 31, /**< Dataset of the Diagnostics cluster */
  ZB_NVRAM_DATASET_NUMBER        = 32, /**< Count of Dataset */
  ZB_NVRAM_DATA_SET_TYPE_PAGE_HDR = 30, /**< Special internal dataset type  */
} zb_nvram_dataset_types_t;

#define ZB_NVRAM_APP_DATASET_NUMBER 4U

/**
 * @brief Structure that contains report about any NVRAM failure.
 *  Pointer to this structure will be set as additional info in error_app_handler
 *    with error code ERROR_CODE(ERROR_CATEGORY_NVRAM, ZB_ERROR_NVRAM_WRITE_VALIDATION_FAILURE).
 *  Upon receiving of this error, user can decide what to do:
 *  1. Do nothing - stack will be reset immediately,
 *  all not corrupted datasets will be copied to another page.
 *  2. Clear nvram.
 *  3. Manually implement function that implements user logic in this case.
 */
typedef struct zb_nvram_failure_report_s
{
  zb_uint32_t pos;            /*!< Position of dataset payload that haven't been written */
  zb_uint16_t ds_type;        /*!< Dataset type @see zb_nvram_dataset_types_t */
  zb_uint8_t page;            /*!< Page of dataset payload that haven't been written */
  zb_uint8_t reserved[1];     /*!< Reserved for future use */
} zb_nvram_failure_report_t;

/**
 * Declares application callback used for reading application datasets from NVRAM.
 *
 * @param page - page in NVRAM from data will be read
 * @param pos - offset in page
 * @param payload_length - number of bytes to read (aligned to 4)
 */
typedef void (*zb_nvram_read_app_data_t)(zb_uint8_t page, zb_uint32_t pos, zb_uint16_t payload_length);

/**
 * Declares application callback used for writing application datasets into NVRAM.
 *
 * @param page - page in NVRAM where data will be stored
 * @param pos - offset in page
 *
 * @return - RET_OK on successful write
 */
typedef zb_ret_t (*zb_nvram_write_app_data_t)(zb_uint8_t page, zb_uint32_t pos);

/**
 * Returns size of application datasets
 *
 * @return - size of application dataset in bytes
 */
typedef zb_uint16_t (*zb_nvram_get_app_data_size_t)(void);

/**
 * Register application callback for reading @ref ZB_NVRAM_APP_DATA1 from NVRAM
 *
 * @param cb - callback that will be called upon reading dataset
 */
void zb_nvram_register_app1_read_cb(zb_nvram_read_app_data_t cb);

/**
 * Register application callback for reading @ref ZB_NVRAM_APP_DATA2 from NVRAM
 *
 * @param cb - callback that will be called upon reading dataset
 */
void zb_nvram_register_app2_read_cb(zb_nvram_read_app_data_t cb);

/**
 * Register application callback for reading @ref ZB_NVRAM_APP_DATA3 from NVRAM
 *
 * @param cb - callback that will be called upon reading dataset
 */
void zb_nvram_register_app3_read_cb(zb_nvram_read_app_data_t cb);

/**
 * Register application callback for reading @ref ZB_NVRAM_APP_DATA4 from NVRAM
 *
 * @param cb - callback that will be called upon reading dataset
 */
void zb_nvram_register_app4_read_cb(zb_nvram_read_app_data_t cb);

/**
 * Register application callback for writing @ref ZB_NVRAM_APP_DATA1 to NVRAM
 *
 * @param wcb - callback that will be called upon writing
 * @param gcb - this callback will be called upon writing to determine dataset size
 */
void zb_nvram_register_app1_write_cb(
    zb_nvram_write_app_data_t wcb,
    zb_nvram_get_app_data_size_t gcb);

/**
 * Register application callback for writing @ref ZB_NVRAM_APP_DATA2 to NVRAM
 *
 * @param wcb - callback that will be called upon writing
 * @param gcb - this callback will be called upon writing to determine dataset size
 */
void zb_nvram_register_app2_write_cb(
    zb_nvram_write_app_data_t wcb,
    zb_nvram_get_app_data_size_t gcb);

/**
 * Register application callback for writing @ref ZB_NVRAM_APP_DATA3 to NVRAM
 *
 * @param wcb - callback that will be called upon writing
 * @param gcb - this callback will be called upon writing to determine dataset size
 */
void zb_nvram_register_app3_write_cb(
    zb_nvram_write_app_data_t wcb,
    zb_nvram_get_app_data_size_t gcb);

/**
 * Register application callback for writing @ref ZB_NVRAM_APP_DATA4 to NVRAM
 *
 * @param wcb - callback that will be called upon writing
 * @param gcb - this callback will be called upon writing to determine dataset size
 */
void zb_nvram_register_app4_write_cb(
    zb_nvram_write_app_data_t wcb,
    zb_nvram_get_app_data_size_t gcb);

/**
 * Write specified dataset into NVRAM
 *
 * @param t - dataset index, see @ref zb_nvram_dataset_types_e
 * @return Status of operation
 *
 * @b Example
 * @snippet light_sample/dimmable_light/bulb.c nvram_usage_example
 */
zb_ret_t zb_nvram_write_dataset(zb_nvram_dataset_types_t t);

/** @cond DOXYGEN_INTERNAL_DOC */
/**
 * Clears all datasets except @ref ZB_IB_COUNTERS and application datasets.
 */
void zb_nvram_clear(void);

/**
   Disable using NVRAM till ZBOSS restart
 */
void zb_nvram_disable(void);
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

#ifndef ZB_USE_INTERNAL_HEADERS
/**
   Erase Informational Bases to NVRAM or other persistent storage
 */
void zb_nvram_erase(void);
#endif /* ZB_USE_INTERNAL_HEADERS */

/**
   Enable or disable NVRAM erasing on every application startup.
   @param erase - 1 to enable erasing; 0 - disable.
   Erasing is disabled by default.
*/
void zb_set_nvram_erase_at_start(zb_bool_t erase);

/**
   Start NVRAM transaction.

   Start writing multiple datasets at once.
   A transaction must finished by calling zb_nvram_transaction_commit().
 */
void zb_nvram_transaction_start(void);

/**
   Commit NVRAM transaction.

   Finish writing multiple datasets at once.
   A transaction must started by calling zb_nvram_transaction_start().
 */
void zb_nvram_transaction_commit(void);

/**
 * Reads a portion of some dataset.
 *
 * @param page - an NVRAM page to read from
 * @param pos - a position on the NVRAM page
 * @param buf - a buffer where read data will be saved
 * @param len - a data length to read
 *
 * @return Status of operation
 */
zb_ret_t zb_nvram_read_data(zb_uint8_t page, zb_uint32_t pos, zb_uint8_t *buf, zb_uint16_t len);


/**
 * Writes a portion of some dataset.
 *
 * @param page - an NVRAM page to write
 * @param pos - a position on the NVRAM page
 * @param buf - a buffer with data to be written
 * @param len - a buffer size
 *
 * @return Status of operation
 */
zb_ret_t zb_nvram_write_data(zb_uint8_t page, zb_uint32_t pos, zb_uint8_t *buf, zb_uint16_t len);

/** @} */ /* zboss_nvram */
/*! @endcond */ /* DOXYGEN_LL_SECTION */

/** @cond DOXYGEN_INTERNAL_DOC */
#define ZB_INVALID_TX_POWER_VALUE     0x7F
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

#ifdef ZB_PRODUCTION_CONFIG

/** @cond DOXYGEN_INTERNAL_DOC */
/** Maximum size of production configuration */
#define ZB_PRODUCTION_CONFIG_APP_MAX_SIZE 128U

typedef ZB_PACKED_PRE struct zb_production_config_hdr_s
{
  zb_uint32_t crc; /*!< crc for the whole production configuration (including application specific part) */
  zb_uint16_t len; /*!< length of production configuration (including application specific part) */
  zb_uint16_t version; /*!< Version of production configuration (reserved for future changes) */
}
ZB_PACKED_STRUCT zb_production_config_hdr_t;

typedef ZB_PACKED_PRE struct zb_production_config_ver_1_s
{
  zb_production_config_hdr_t hdr; /*!< Header */
  zb_uint32_t aps_channel_mask;   /*!< APS Channel Mask  */
  zb_64bit_addr_t extended_address; /*!< IEEE address */
  zb_int8_t mac_tx_power[16]; /*!< Tx power specified for every possible channel */
  zb_uint8_t install_code[ZB_CCM_KEY_SIZE+ZB_CCM_KEY_CRC_SIZE]; /*!< Installation code + its own crc */
}
ZB_PACKED_STRUCT zb_production_config_ver_1_t;

/* NOTE: ZB_PROD_CFG_V2_APS_CHANNEL_LIST_SIZE support only 4 subghz pages 28-31 and 2.4. */
#define ZB_PROD_CFG_V2_APS_CHANNEL_LIST_SIZE  5U
/* NOTE: ZB_PROD_CFG_V3_APS_CHANNEL_LIST_SIZE for prod config with support 9 subghz pages 23-31 and 2.4. */
#define ZB_PROD_CFG_V3_APS_CHANNEL_LIST_SIZE  (ZB_PROD_CFG_V2_APS_CHANNEL_LIST_SIZE + 5U)
#define ZB_PROD_CFG_MAC_TX_POWER_CHANNEL_N 27U

#define ZB_PROD_CFG_OPTIONS_IC_TYPE_MASK      0x03U
#define ZB_PROD_CFG_OPTIONS_CERT_PRESENT_MASK 0x80U

typedef ZB_PACKED_PRE struct zb_production_config_ver_2_s
{
  zb_production_config_hdr_t hdr; /*!< Header */
  zb_channel_page_t aps_channel_mask_list[ZB_PROD_CFG_V2_APS_CHANNEL_LIST_SIZE]; /*!< Channel mask for device to operate on */
  zb_64bit_addr_t extended_address; /*!< IEEE address */
  zb_int8_t mac_tx_power[ZB_PROD_CFG_V2_APS_CHANNEL_LIST_SIZE][ZB_PROD_CFG_MAC_TX_POWER_CHANNEL_N]; /*! < Tx power specified for every possible channel */
  zb_uint8_t options;/*low 2 bits - ic_type field *//*7th bit - certificates block presents*/
  zb_uint8_t install_code[ZB_CCM_KEY_SIZE+ZB_CCM_KEY_CRC_SIZE]; /*!< Installation code + its own crc */
}
ZB_PACKED_STRUCT zb_production_config_ver_2_t;

typedef ZB_PACKED_PRE struct zb_production_config_ver_3_s
{
  zb_production_config_hdr_t hdr; /*!< Header */
  zb_channel_page_t aps_channel_mask_list[ZB_PROD_CFG_V3_APS_CHANNEL_LIST_SIZE]; /*!< Channel mask for device to operate on */
  zb_64bit_addr_t extended_address; /*!< IEEE address */
  zb_int8_t mac_tx_power[ZB_PROD_CFG_V3_APS_CHANNEL_LIST_SIZE][ZB_PROD_CFG_MAC_TX_POWER_CHANNEL_N]; /*! < Tx power specified for every possible channel */
  zb_uint8_t options;/*low 2 bits - ic_type field *//*7th bit - certificates block presents*/
  zb_uint8_t install_code[ZB_CCM_KEY_SIZE+ZB_CCM_KEY_CRC_SIZE]; /*!< Installation code + its own crc */
}
ZB_PACKED_STRUCT zb_production_config_ver_3_t;

typedef union zb_production_config_e
{
  zb_production_config_ver_2_t v2;
  zb_production_config_ver_3_t v3;
} zb_production_config_t;

#define ZB_CS1_PUBLISHER_PUBLIC_KEY_SIZE 22
#define ZB_CS1_CERTIFICATE_SIZE 48
#define ZB_CS1_PRIVATE_KEY_SIZE 21

#define ZB_CS2_PUBLISHER_PUBLIC_KEY_SIZE 37
#define ZB_CS2_CERTIFICATE_SIZE 74
#define ZB_CS2_PRIVATE_KEY_SIZE 36

typedef ZB_PACKED_PRE struct zb_cs1_key_material_s
{
  zb_uint8_t publisher_public_key[ZB_CS1_PUBLISHER_PUBLIC_KEY_SIZE];
  zb_uint8_t certificate[ZB_CS1_CERTIFICATE_SIZE];
  zb_uint8_t private_key[ZB_CS1_PRIVATE_KEY_SIZE];
} ZB_PACKED_STRUCT zb_cs1_key_material_t;

typedef ZB_PACKED_PRE struct zb_cs2_key_material_s
{
  zb_uint8_t publisher_public_key[ZB_CS2_PUBLISHER_PUBLIC_KEY_SIZE];
  zb_uint8_t certificate[ZB_CS2_CERTIFICATE_SIZE];
  zb_uint8_t private_key[ZB_CS2_PRIVATE_KEY_SIZE];
} ZB_PACKED_STRUCT zb_cs2_key_material_t;

typedef ZB_PACKED_PRE struct zb_cs_key_material_header_s
{
  zb_uint16_t certificate_mask;
} ZB_PACKED_STRUCT zb_cs_key_material_header_t;

#define ZB_PROD_CFG_IS_PRESENT_CERT_ANY(v) ((v->options) & 0x80U)
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/**
 * Disable usage of production configuration at runtime.
 *
 * @param val - ZB_TRUE means production configuration is disabled, ZB_FALSE - enabled.
 */
void zb_production_config_disable(zb_bool_t val);

/**
 * Get state of production configuration disabling.
 *
 * @return ZB_TRUE if production configuration is disabled, ZB_FALSE - enabled.
 */
zb_bool_t zb_is_production_config_disabled(void);

zb_ret_t zb_production_config_check(zb_production_config_hdr_t *prod_cfg_hdr);

#endif /* ZB_PRODUCTION_CONFIG */




#ifdef ZB_USE_SLEEP
/*! @cond DOXYGEN_LL_SECTION */
/*! @addtogroup sleep_api */
/*! @{ */

/** Default sleep threshold. Do not sleep when it is less then 1 Beacon Interval to wake up. */
#define ZB_SCHED_SLEEP_THRESHOLD_MS 20U

/** Maximum sleep threshold. */
#define ZB_MAXIMUM_SLEEP_THRESHOLD_MS 86400000U

/**
  Set sleep threshold on device; when scheduler detects that device can be put in sleep mode
  it will notify application with signal @see ZB_COMMON_SIGNAL_CAN_SLEEP.
  Device can not be put into sleep mode when sleep interval less than this threshold.

  @param threshold_ms - sleep threshold in milliseconds
             If threshold is 0, means zero threshold, application will be notified each time when stack is ready to sleep
             (no immediate callbacks in queue).
  @return RET_OK if new threshold is valid and applied
  @return RET_ERROR if user wants to set threshold greater that @ref ZB_MAXIMUM_SLEEP_THRESHOLD_MS or lesser than ZB_SCHED_SLEEP_THRESHOLD_MS.
*/
zb_ret_t zb_sleep_set_threshold(zb_uint32_t threshold_ms);

/**
  Get sleep threshold value from stack.
  @return threshold value in milliseconds
*/
zb_uint32_t zb_get_sleep_threshold(void);

/**
  Blocking function responsible for putting device into sleep mode.

*/
void zb_sleep_now(void);

/*! @} */ /* sleep_api */
/*! @endcond */ /* DOXYGEN_LL_SECTION */
#endif /* ZB_USE_SLEEP */

#if defined ZB_JOINING_LIST_SUPPORT

/**
 *  \addtogroup zdo_joining_lists
 *  @{
 *    @details
 *    The API executes only one operation at a time.
 *    It is necessary to account for that, issuing another operation
 *    only on completion of the previous one. See examples.
 */



/** Parameters for @ref zb_ieee_joining_list_add. */
typedef ZB_PACKED_PRE struct zb_ieee_joining_list_add_params_s
{
  zb_callback_t callback;   /**< Callback to be scheduled on completion of adding. */

  zb_ieee_addr_t address;   /**< 64-bit address to add. */

} ZB_PACKED_STRUCT zb_ieee_joining_list_add_params_t;


/** Parameters for @ref zb_ieee_joining_list_delete. */
typedef ZB_PACKED_PRE struct zb_ieee_joining_list_delete_params_s
{
  zb_callback_t callback;  /**< Callback to be scheduled on completion of deleting. */

  zb_ieee_addr_t address;  /**< 64-bit address to delete. */

} ZB_PACKED_STRUCT zb_ieee_joining_list_delete_params_t;


/**
 * @name MAC joining policy
 * @anchor mac_joining_policy
 * @brief MAC joining policy, see Table D-4 of Zigbee r22 spec.
 */
/** @{ */
#define ZB_MAC_JOINING_POLICY_ALL_JOIN       0x0U /*!< Any device is allowed to join. */
#define ZB_MAC_JOINING_POLICY_IEEELIST_JOIN  0x1U /*!< Only devices on the mibJoiningIeeeList are allowed to join.*/
#define ZB_MAC_JOINING_POLICY_NO_JOIN        0x2U /*!< No device is allowed to join. */
/** @} */

/**
 * @brief Type for MAC joining policy.
 *
 * Holds one of @ref mac_joining_policy. Kept for backward compatibility as
 * @ref mac_joining_policy were declared previously as enum.
 */
typedef zb_uint8_t zb_mac_joining_policy_t;

/** Parameters for @ref zb_ieee_joining_list_clear. */
typedef ZB_PACKED_PRE struct zb_ieee_joining_list_clear_params_s
{
  zb_callback_t callback;  /**< Callback to be scheduled on completion
                              of clearing IEEE joining list. */

  zb_mac_joining_policy_t new_joining_policy;   /**< Joining list policy to set on the emptied list.
                                                   See @ref mac_joining_policy for possible
                                                   values.*/

} ZB_PACKED_STRUCT zb_ieee_joining_list_clear_params_t;


/** Parameters for @ref zb_ieee_joining_list_set_policy. */
typedef ZB_PACKED_PRE struct zb_ieee_joining_list_set_policy_s
{
  zb_callback_t callback;  /**< Callback to be scheduled on completion
                              of updating IEEE joining list policy. */

  zb_mac_joining_policy_t new_joining_policy;   /**< Joining list policy to set on the emptied list.
                                                   See @ref mac_joining_policy for possible
                                                   values.*/

} ZB_PACKED_STRUCT zb_ieee_joining_list_set_policy_t;


/** Parameters for @ref zb_ieee_joining_list_announce. */
typedef ZB_PACKED_PRE struct zb_ieee_joining_list_announce_s
{
  zb_callback_t callback;   /**< Callback to be scheduled on completion
                               of clearing IEEE joining list. */

  zb_bool_t silent;         /**< If set to ZB_TRUE, no broadcast happens. */
} ZB_PACKED_STRUCT zb_ieee_joining_list_announce_t;


/** Parameters for @ref zb_ieee_joining_list_request. */
typedef ZB_PACKED_PRE struct zb_ieee_joining_list_request_s
{
  zb_callback_t callback;   /**< Callback to be scheduled on completion of the operation. */
} ZB_PACKED_STRUCT zb_ieee_joining_list_request_t;


/**
 * @name IEEE joining list result status
 * @anchor ieee_joining_list_result_status
 */
/** @{ */

/** Operation completed successfully. */
#define ZB_IEEE_JOINING_LIST_RESULT_OK                 0U
/** Operation failed due to problems within ZBOSS. */
#define ZB_IEEE_JOINING_LIST_RESULT_INTERNAL_ERROR     1U
/** Operation failed due to incorrect behavior of the opposite side. */
#define ZB_IEEE_JOINING_LIST_RESULT_BAD_RESPONSE       2U
/** Basic conditions for execution of the operation are not satisfied (for example, it must be used
 * by routers only, etc). */
#define ZB_IEEE_JOINING_LIST_RESULT_PERMISSION_DENIED  3U
/** The situation forces the command to be restarted.*/
#define ZB_IEEE_JOINING_LIST_RESULT_RESTART_LATER      4U
/** The device does not have storage space to support the requested operation. */
#define ZB_IEEE_JOINING_LIST_RESULT_INSUFFICIENT_SPACE 5U

/** @} */

/**
 * @brief Type for IEEE joining list result status.
 *
 * Holds one of @ref ieee_joining_list_result_status. Kept for backward
 * compatibility as @ref ieee_joining_list_result_status were declared previously as enum.
 */
typedef zb_uint8_t zb_ieee_joining_list_result_status_t;


/** Structure passed as a parameter to operation completion callbacks. */
typedef ZB_PACKED_PRE struct zb_ieee_joining_list_result_s
{
  zb_ieee_joining_list_result_status_t status;
} ZB_PACKED_STRUCT zb_ieee_joining_list_result_t;

/**
 * Add an address to IEEE joining list.
 *
 * For coordinators only.
 *
 * @param param - Reference to buffer containing @ref zb_ieee_joining_list_add_params_t structure as a parameter.
 *
 * @b Example:
   @code

void function_add_cb(zb_uint8_t param)
{
  zb_ieee_joining_list_result_t *res;
  zb_bufid_t buf = ZB_GET_BUF_FROM_REF(param);

  res = ZB_BUF_GET_PARAM(buf, zb_ieee_joining_list_result_t);
  if (res->status == ZB_IEEE_JOINING_LIST_RESULT_OK)
  {
    TRACE_MSG(TRACE_APP1, "Address has been added", (FMT__0));
  }

  zb_free_buf(buf);
}

/&lowast; 00:00:00:00:00:00:0E:01 &lowast;/
static zb_ieee_addr_t new_addr = {0x01, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void function_add(zb_uint8_t param)
{
  zb_ieee_joining_list_add_params_t *add_params;
  zb_bufid_t buf = ZB_GET_BUF_FROM_REF(param);

  add_params = ZB_BUF_GET_PARAM(buf, zb_ieee_joining_list_set_policy_t);
  ZB_MEMCPY(add_params->address, &new_addr, sizeof(new_addr));
  add_params->callback = &function_add_cb;

  zb_ieee_joining_list_add(param);
}
   @endcode
 */
void zb_ieee_joining_list_add(zb_uint8_t param);


/**
 * Remove an address from IEEE joining list.
 *
 * For coordinators only.
 *
 * @param param - Reference to buffer containing @ref zb_ieee_joining_list_delete_params_t structure as a parameter.
 */
void zb_ieee_joining_list_delete(zb_uint8_t param);


/**
 * Clear IEEE joining list.
 *
 * For coordinators only.
 *
 * @param param - Reference to buffer containing @ref zb_ieee_joining_list_clear_params_t structure as a parameter.
 *
 * @b Example:
   @code

void function_clear_cb(zb_uint8_t param)
{
  zb_ieee_joining_list_result_t *res;
  zb_bufid_t buf = ZB_GET_BUF_FROM_REF(param);

  res = ZB_BUF_GET_PARAM(buf, zb_ieee_joining_list_result_t);
  if (res->status == ZB_IEEE_JOINING_LIST_RESULT_OK)
  {
    TRACE_MSG(TRACE_APP1, "IEEE joining list is empty now", (FMT__0));
  }

  zb_free_buf(buf);
}

void function_clear(zb_uint8_t param)
{
  zb_ieee_joining_list_clear_params_t *clear_params;
  zb_bufid_t buf = ZB_GET_BUF_FROM_REF(param);

  clear_params = ZB_BUF_GET_PARAM(buf, zb_ieee_joining_list_clear_params_t);
  clear_params->new_joining_policy = ZB_MAC_JOINING_POLICY_NO_JOIN;
  clear_params->callback = &function_clear_cb;

  zb_ieee_joining_list_clear(param);
}
   @endcode
 */
void zb_ieee_joining_list_clear(zb_uint8_t param);


/**
 * Set joining policy for IEEE joining list.
 *
 * For coordinators only.
 *
 * @param param - Reference to buffer containing @ref zb_ieee_joining_list_set_policy_t structure as a parameter.
 *
 * @b Example:
   @code

void function_policy_cb(zb_uint8_t param)
{
  zb_ieee_joining_list_result_t *res;
  zb_bufid_t buf = ZB_GET_BUF_FROM_REF(param);

  res = ZB_BUF_GET_PARAM(buf, zb_ieee_joining_list_result_t);
  if (res->status == ZB_IEEE_JOINING_LIST_RESULT_OK)
  {
    TRACE_MSG(TRACE_APP1, "New policy has been set", (FMT__0));
  }

  zb_free_buf(buf);
}

void function_policy(zb_uint8_t param)
{
  zb_ieee_joining_list_set_policy_t *policy_params;
  zb_bufid_t buf = ZB_GET_BUF_FROM_REF(param);

  policy_params = ZB_BUF_GET_PARAM(buf, zb_ieee_joining_list_set_policy_t);
  policy_params->new_joining_policy = ZB_MAC_JOINING_POLICY_NO_JOIN;
  policy_params->callback = &function_policy_cb;

  zb_ieee_joining_list_set_policy(param);
}
   @endcode
 */
void zb_ieee_joining_list_set_policy(zb_uint8_t param);


/**
 * Increases update_id, marks IEEE joining list as consistent and broadcasts changes.
 *
 * For coordinators only.
 *
 * @param param - Reference to buffer containing @ref zb_ieee_joining_list_announce_t structure as a parameter.
 */
void zb_ieee_joining_list_announce(zb_uint8_t param);


/**
 * Request IEEE joining list from the Trust Center.
 *
 * For routers only.
 *
 * If the Trust Center updates its list during zb_ieee_joining_list_request execution,
 * zb_ieee_joining_list_request fails with ZB_IEEE_JOINING_LIST_RESULT_RESTART_LATER status.
 *
 * @param param - Reference to buffer containing @ref zb_ieee_joining_list_request_t structure as a parameter.
 */
void zb_ieee_joining_list_request(zb_uint8_t param);
/*!@} */ /* zdo_joining_lists */

#endif /* defined ZB_JOINING_LIST_SUPPORT */

#ifdef ZB_SECURITY_INSTALLCODES
/**
  Set using of install codes by TC

  @param use_ic - enable/disable install code usage
*/
void zb_tc_set_use_installcode(zb_uint8_t use_ic);
#endif /* ZB_SECURITY_INSTALLCODES */

#if defined ZB_ED_FUNC && defined ZB_CONTROL4_NETWORK_SUPPORT
/**
  Permit joining Control4 Network
*/
void zb_permit_control4_network(void);

/**
   Return if joining Control4 Network is allowed
*/
zb_bool_t zb_control4_network_permitted(void);

#endif /* defined ZB_ED_FUNC && defined ZB_CONTROL4_NETWORK_SUPPORT */

/**
 * @brief Set the 'macTransactionPersistenceTime' attribute in the MAC PIB
 *
 * @param ms - value in milliseconds
 */
void zb_set_mac_transaction_persistence_time(zb_uint16_t ms);

#ifdef ZB_ALLOW_PROVISIONAL_KEY_AS_TCLK
/**
 * @brief Allow joining to trust centers that claim revision r21 or higher
 *        but sends provisional key as a trust center link key
 *
 * @note That behaviour is against specification and disabled by default
 */
void zb_allow_provisional_key_as_tclk(void);

/**
 * @brief Disallow joining to trust centers that claim revision r21 or higher
 *        but sends provisional key as a trust center link key
 *
 * @note That behaviour is against specification and disabled by default
 */
void zb_disallow_provisional_key_as_tclk(void);
#endif /* ZB_ALLOW_PROVISIONAL_KEY_AS_TCLK */

#endif /*ZBOSS_API_H*/
