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
/* PURPOSE: ZigBee trace. Application should include it.
*/
#ifndef ZB_LOGGER_H
#define ZB_LOGGER_H 1

#ifdef ZB_TRACE_USE_VA_LIST
#include <stdarg.h>
#endif

/** @cond DOXYGEN_DEBUG_SECTION */
/**
 * @addtogroup ZB_TRACE Debug trace
 * @{
 */
/** @cond DSR_TRACE */
/**
 * @addtogroup ZB_TRACE_CONFIG Trace configuration
 * @{
 */
/** @endcond */ /* DSR_TRACE */

#if defined(ZB_TRACE_LEVEL)
/** @cond DOXYGEN_INTERNAL_DOC */
extern zb_uint8_t g_trace_level, g_o_trace_level;
extern zb_uint32_t g_trace_mask;
extern zb_uint_t g_trace_inside_intr;
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
/**
   Set trace level at runtime

   That macro can switch trace level on and off. Trace level must be included
   into build at compile time by setting ZB_TRACE_LEVEL compiler time define.

   @param l - new trace level.

   @par Example
   @snippet thermostat/thermostat_zr/thermostat_zr.c set_trace
   @par
 */
#define ZB_SET_TRACE_LEVEL(l) g_trace_level = (l)

/**
   Switch off all trace at runtime
 */
#define ZB_SET_TRACE_OFF() g_o_trace_level = g_trace_level, g_trace_level = 0U

/**
   Switch on trace at runtime

   That macro enables trace which was active before call to ZB_SET_TRACE_OFF().

   @snippet light_sample/dimmable_light/bulb.c switch_trace_on
 */
#define ZB_SET_TRACE_ON() g_trace_level = g_o_trace_level

/**
   Set trace mask at runtime

   That macro can switch trace mask on and off. Trace mask must be included
   into build at compile time by setting ZB_TRACE_MASK compiler time define.

   @param m - new trace mask.

   @par Example
   @snippet thermostat/thermostat_zr/thermostat_zr.c set_trace
   @par
 */
#define ZB_SET_TRACE_MASK(m) g_trace_mask = (m)

#else

#define ZB_SET_TRACE_OFF()
#define ZB_SET_TRACE_LEVEL(l)
#define ZB_SET_TRACE_ON()
#define ZB_SET_TRACE_MASK(m)

#endif /* defined(ZB_TRACE_LEVEL) */

/** @cond DSR_TRACE */
/** @} */ /* ZB_TRACE_CONFIG */
/** @endcond */ /* DSR_TRACE */

/** @cond DSR_TRACE */
/**
 *  @addtogroup TRACE_SUBSYSTEMS
 *  @{
 */
#define TRACE_SUBSYSTEM_COMMON    0x0001U  /**< Common subsystem. */
#define TRACE_SUBSYSTEM_MEM       0x0002U  /**< MEM subsystem (buffers pool). */
#define TRACE_SUBSYSTEM_MAC       0x0004U  /**< MAC subsystem. */
#define TRACE_SUBSYSTEM_NWK       0x0008U  /**< NWK subsystem. */

#define TRACE_SUBSYSTEM_APS       0x0010U  /**< APS subsystem. */
#define TRACE_SUBSYSTEM_ZSE       0x0020U  /**< ZSE subsystem. */
#define TRACE_SUBSYSTEM_ZDO       0x0040U  /**< ZDO subsystem. */
#define TRACE_SUBSYSTEM_SECUR     0x0080U  /**< Security subsystem. */

#define TRACE_SUBSYSTEM_ZCL       0x0100U  /**< ZCL subsystem. */
/** @cond DOXYGEN_TOUCHLINK_FEATURE */
#define TRACE_SUBSYSTEM_ZLL       0x0200U  /**< ZLL/Touchlink subsystem. */
/** @endcond */ /* DOXYGEN_TOUCHLINK_FEATURE */
/** @cond DOXYGEN_INTERNAL_DOC */
#define TRACE_SUBSYSTEM_SSL       0x0400U  /**< SSL subsystem - not really used */
#define TRACE_SUBSYSTEM_NCP_TRANSPORT  TRACE_SUBSYSTEM_SSL
#define TRACE_SUBSYSTEM_MACSPLIT  TRACE_SUBSYSTEM_NCP_TRANSPORT
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
/** @endcond */ /* DSR_TRACE */
#define TRACE_SUBSYSTEM_APP       0x0800U  /**< User Application */

/** @cond DOXYGEN_INTERNAL_DOC */
#define TRACE_SUBSYSTEM_LWIP      0x1000U  /* LWIP is used, else free */
#define TRACE_SUBSYSTEM_ALIEN     0x2000U  /* Some special debug */
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
#define TRACE_SUBSYSTEM_ZGP       0x4000U  /**< ZGP subsystem */
/** @cond DOXYGEN_INTERNAL_DOC */
#define TRACE_SUBSYSTEM_MAC_API   0x8000U    /**< MAC API subsystem */
#define TRACE_SUBSYSTEM_MACLL     0x10000U   /**< MAC LL subsystem */
#define TRACE_SUBSYSTEM_SPECIAL1  0x20000U   /**< Special subsystem */
#define TRACE_SUBSYSTEM_BATTERY   0x40000U   /**< Battery subsystem */
#define TRACE_SUBSYSTEM_OTA       0x80000U   /**< OTA subsystem */
#define TRACE_SUBSYSTEM_TRANSPORT 0x100000U  /**< Transport subsystem */
#define TRACE_SUBSYSTEM_USB       0x200000U  /**< USB subsystem */
#define TRACE_SUBSYSTEM_SPI       0x400000U  /**< SPI subsystem */
#define TRACE_SUBSYSTEM_UART      0x800000U  /**< UART subsystem */
#define TRACE_SUBSYSTEM_JSON      0x1000000U /**< JSON subsystem */
#define TRACE_SUBSYSTEM_HTTP      0x2000000U /**< HTTP subsystem */
#define TRACE_SUBSYSTEM_CLOUD     0x4000000U /**< Interface to the Cloud */
#define TRACE_SUBSYSTEM_ZBDIRECT  0x8000000U /**< Zigbee Direct subsystem */
#define TRACE_SUBSYSTEM_DIAGNOSTIC  0x10000000U /**< Diagnostic subsystem */
#define TRACE_SUBSYSTEM_NS          0x20000000U /**< Network simulator subsystem */
#define TRACE_SUBSYSTEM_TEST        0x40000000U /**< Subsystem for tests and CI */
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

#define TRACE_SUBSYSTEM_INFO      ((zb_uint_t)-1)  /**< Common subsystem */

/* to be continued... */

/** @cond DSR_TRACE */
/** @} */ /* TRACE_SUBSYSTEMS */
/** @endcond */ /* DSR_TRACE */
/** @} */ /* ZB_TRACE */

/**
 * @addtogroup ZB_TRACE Debug trace
 * @{
 */
#if defined ZB_TRACE_LEVEL || defined DOXYGEN
/**
 * @addtogroup ZB_TRACE_CONFIG Trace configuration
 * @{
 */

#ifndef DOXYGEN
#if defined ESP_ZIGBEE_TRACE
#define TRACE_ENABLED_(mask,lev) ((lev) <= ZB_TRACE_LEVEL && ((mask) & ZB_TRACE_MASK))
#else
#define TRACE_ENABLED_(mask,lev) 0
#endif
#endif /* DOXYGEN */

/**
 *  @brief Check that trace is enabled for provided level.
 *  To be used in constructions like:
 *  @code
 *  if (TRACE_ENABLED(TRACE_APS3))
 *  {
 *    call_some_complex_trace();
 *  }
 *  @endcode
 *  @param m - trace level macro.
 *  @return 1 if enabled, 0 if disabled.
 */
#define TRACE_ENABLED(m) TRACE_ENABLED_(m)

#ifndef DOXYGEN
zb_uint32_t zb_trace_get_counter(void);
#endif /* DOXYGEN */

#ifdef DOXYGEN
/**
   Trace file ID used by win_com_dump to identify source file.

   Must be first define in the .c file, just before first include directive.
  @par Example
  @snippet light_sample/dimmable_light/bulb.c trace_file_id
  @par
*/
#define ZB_TRACE_FILE_ID 12345U
#endif
/** @} */ /* ZB_TRACE_CONFIG */

/** @cond DOXYGEN_INTERNAL_DOC */
#ifdef ZB_INTERRUPT_SAFE_CALLBACKS
/* If HW can detect that we are inside ISR, let's use it and do not trace from ISR. */
zb_bool_t zb_osif_is_inside_isr(void);
#define ZB_HW_IS_INSIDE_ISR() zb_osif_is_inside_isr()
#else
#define ZB_HW_IS_INSIDE_ISR() 0
#endif

#define TRACE_ENTER_INT() g_trace_inside_intr = 1U
#define TRACE_LEAVE_INT() g_trace_inside_intr = 0U
#define ZB_TRACE_INSIDE_INTR() (g_trace_inside_intr || ZB_HW_IS_INSIDE_ISR())

#ifndef ZB_TRACE_FROM_INTR
#define ZB_TRACE_INSIDE_INTR_BLOCK() ZB_TRACE_INSIDE_INTR()
#else
#define ZB_TRACE_INSIDE_INTR_BLOCK() 0
#endif
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/**
 *  @brief A universal way to print raw string as trace message.
 */
void zb_trace_str(const zb_char_t *str);

#if defined ZB_TRACE_TO_FILE || defined ZB_TRACE_TO_SYSLOG || defined DOXYGEN
/** @cond DOXYGEN_INTERNAL_DOC */
/**
   \par Trace to file means trace to disk file using printf() or its analog.
   Tricks to decrease code size by excluding format strings are not used.
 */
void zb_trace_init_file(zb_char_t *name);
void zb_trace_deinit_file(void);
void zb_trace_file_commit(void);
void zb_trace_disable_deinit(void);
zb_bool_t zb_trace_is_disable_deinit(void);
void zb_trace_file_flush(void);
#ifdef ZB_USE_LOGFILE_ROTATE
void zb_trace_check_rotate(void);
#endif

/**

 Initialize trace subsystem

 @param name - trace file name component
*/
#define TRACE_INIT(name)   zb_trace_init_file(name)
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/**
 Deinitialize trace subsystem
*/
#define TRACE_DEINIT zb_trace_deinit_file

/** @cond DOXYGEN_INTERNAL_DOC */
#define ZB_T0_TRACE(...) __VA_ARGS__

#if defined ZB_TRACE_USE_VA_LIST && (defined ZB_BINARY_AND_TEXT_TRACE_MODE || defined ZB_TRACE_TO_SYSLOG || !defined ZB_BINARY_TRACE)
/**
 *  @brief Print trace message. Option with va_list
 */
void zb_trace_msg_txt_file_vl(
    zb_uint_t mask,
    zb_uint_t level,
    const zb_char_t *format,
    const zb_char_t *file_name,
    zb_int_t line_number,
    zb_int_t args_size,
    va_list arglist);
#endif

#if defined ZB_BINARY_TRACE && !defined ZB_TRACE_TO_SYSLOG

/**
 *  @brief Print binary trace message.
 */
void zb_trace_msg_bin_file(
    zb_uint_t mask,
    zb_uint_t level,
#if defined ZB_BINARY_AND_TEXT_TRACE_MODE
    zb_char_t *file_name,
#endif
    zb_uint16_t file_id,
    zb_int_t line_number,
    zb_int_t args_size, ...);

#ifdef ZB_TRACE_USE_VA_LIST
/**
 *  @brief Print binary trace message. Option with va_list
 */
void zb_trace_msg_bin_file_vl(
    zb_uint_t mask,
    zb_uint_t level,
    zb_uint16_t file_id,
    zb_int_t line_number,
    zb_int_t args_size,
    va_list arglist);
#endif

#if defined ZB_BINARY_AND_TEXT_TRACE_MODE
#define ZB_TRACE_MODE_BINARY 0U
#define ZB_TRACE_MODE_TEXT   1U
extern zb_uint8_t g_trace_text_mode;
void zb_trace_set_mode(zb_uint8_t mode);

/**
 *  @brief Print trace message.
 */
void zb_trace_msg_txt_file(
    zb_uint_t mask,
    zb_uint_t level,
    const zb_char_t *format,
    const zb_char_t *file_name,
    zb_uint16_t file_id,
    zb_int_t line_number,
    zb_int_t args_size, ...);

#define ZB_T1_TRACE(s, l, fmts, args)                  \
  if ((zb_int_t)ZB_TRACE_LEVEL>=(zb_int_t)l && \
      ((s) & ZB_TRACE_MASK))                   \
  {                                            \
    if (g_trace_text_mode == 0U)               \
    {                                          \
      zb_trace_msg_bin_file(s, l, ZB_T0_TRACE args);   \
    }                                          \
    else                                       \
    {                                          \
      zb_trace_msg_txt_file(s, l, fmts, ZB_T0_TRACE args); \
    }                                          \
  }
#else
#define ZB_T1_TRACE(s, l, fmts, args) if ((zb_int_t)ZB_TRACE_LEVEL>=(zb_int_t)l && ((s) & ZB_TRACE_MASK)) zb_trace_msg_bin_file(s, l, ZB_T0_TRACE args)
#endif
#else
/**
 *  @brief Print trace message.
 */
void zb_trace_msg_txt_file(
    zb_uint_t mask,
    zb_uint_t level,
    const zb_char_t *format,
    const zb_char_t *file_name,
    zb_int_t line_number,
    zb_int_t args_size, ...);

#define ZB_T1_TRACE(s, l, fmts, args) if ((zb_int_t)ZB_TRACE_LEVEL>=(zb_int_t)l && ((s) & ZB_TRACE_MASK)) zb_trace_msg_txt_file(s, l, fmts, ZB_T0_TRACE args)
#endif
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/**
 *  @brief Put trace output.
 *  @param lm - trace subsystem and level marker.
 *  @param fmts - printf()-like format string.
 *  @param args - format string substitution parameters with a size marker.
 *
 *  @par Example
 *  @snippet light_sample/dimmable_light/bulb.c trace_msg
 *  @par
 */
#define TRACE_MSG(lm, fmts, args) \
  do { \
    ZB_T1_TRACE(lm, fmts, args); \
  } while (0)

/** @cond DOXYGEN_INTERNAL_DOC */
#elif (defined ZB_TRACE_TO_PORT || defined ZB_TRACE_OVER_SIF)
/*
  8051 trace does not use format string in the code to save code space.

- will modify trace at device only, Linux will work as before
- trace implementation will hex dump all arguments as set of bytes
- external utility will parse dump, divide trace arguments dump into separate arguments and
convert hex-int, unsigned etc.
- utility will get argument strings from the source files (trace macros) and find it
by file:line
- Add one more parameter to the trace macro: sum of the trace argument sizes.
Define readable constants like
#define FMT_D_HD_X 5
- create script/program to modify existing trace calls
- combine dump parse utility functionality with win_com_dump, so it will produce human-readable trace

 */

#if defined ZB_TRACE_OVER_SIF
#define TRACE_INIT(name) zb_osif_sif_init()
#elif ! defined ZB_SERIAL_FOR_TRACE || defined ZB_TRACE_OVER_JTAG
#define TRACE_INIT(name)
#else
#define TRACE_INIT(name) zb_serial_trace_init(name)
#endif /* defined ZB_TRACE_OVER_SIF */

/* No trace deinit */
#define TRACE_DEINIT()

#ifndef ZB_BINARY_TRACE
void zb_trace_msg_port(
    const zb_char_t ZB_IAR_CODE *file_name,
    zb_int_t line_number,
    zb_uint8_t args_size, ...);

#else

#ifndef ZB_TRACE_FILE_ID
#ifndef ZB_DONT_NEED_TRACE_FILE_ID
#warning ZB_TRACE_FILE_ID undefined! Insert #define ZB_TRACE_FILE_ID NNN before 1-st #include
#endif
#define ZB_TRACE_FILE_ID 0
#else
ZB_ASSERT_COMPILE_DECL(ZB_TRACE_FILE_ID < ZB_UINT16_MAX);
#endif

void zb_trace_msg_port(
  zb_uint_t mask,
  zb_uint_t level,
  zb_uint16_t file_id,
  zb_uint16_t line_number,
  zb_uint_t args_size, ...);

#ifdef ZB_TRACE_USE_VA_LIST
/* Option with va_list */
void zb_trace_msg_port_vl(
  zb_uint_t mask,
  zb_uint_t level,
  zb_uint16_t file_id,
  zb_uint16_t line_number,
  zb_uint_t args_size,
  va_list arglist);
#endif

#endif


#if defined ZB_BINARY_TRACE && defined ESP_ZIGBEE_TRACE
void esp_zb_trace_msg_port(
  zb_uint_t mask,
  zb_uint_t level,
  zb_char_t *fmt,
  const zb_char_t *file_name,
  zb_uint16_t line_number,
  zb_uint_t args_size, ...);
#endif /* ZB_BINARY_TRACE && defined ESP_ZIGBEE_TRACE */


#ifdef ZB_BINARY_TRACE
#define ZB_T0_TRACE(...) __VA_ARGS__
#if defined ESP_ZIGBEE_TRACE
#define ZB_T1_TRACE(s, l, fmt, args) if ((zb_int_t)g_trace_level>=(zb_int_t)l && ((s) == (zb_uint_t)-1 || (s) & g_trace_mask) && !ZB_TRACE_INSIDE_INTR_BLOCK()) esp_zb_trace_msg_port(s, l, fmt, ZB_T0_TRACE args)
#else
#define ZB_T1_TRACE(s, l, args) 
#endif /* ESP_ZIGBEE_TRACE */
#else
#define ZB_T1_TRACE(s, l, args) \
  if ((zb_int_t)ZB_TRACE_LEVEL>=(zb_int_t)l && ((s) == -1 || ((s) & ZB_TRACE_MASK)) && !ZB_TRACE_INSIDE_INTR_BLOCK()) zb_trace_msg_port args
#endif

#if defined ESP_ZIGBEE_TRACE
#define TRACE_MSG(lm, fmt, args) \
  do { \
    ZB_T1_TRACE(lm, fmt, args); \
  } while (0)
#else
#define TRACE_MSG(lm, fmt, args) \
  do { \
    ZB_T1_TRACE(lm, args); \
  } while (0)
#endif /* ESP_ZIGBEE_TRACE*/

#else

#error Must define either ZB_TRACE_TO_FILE or ZB_TRACE_TO_PORT || ZB_TRACE_OVER_SIF

#endif  /* trace type */


#else  /* if trace off */

#ifndef KEIL
#define TRACE_MSG(...) ((void)0)
#else
/* Keil does not support vararg macros */
#define TRACE_MSG(a,b,c) ((void)0)
#endif

#define TRACE_INIT(name)
#define TRACE_DEINIT(c)

#define TRACE_ENABLED(m) ZB_FALSE

#define TRACE_ENTER_INT()
#define TRACE_LEAVE_INT()

#endif  /* trace on/off */
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/** @cond DSR_TRACE */
/**
 *  @addtogroup TRACE_DATA_FORMAT_ADDITIONAL Trace data format for keys
 *  @{
 */
#if defined ZB_TRACE_TO_FILE || defined ZB_TRACE_TO_SYSLOG || defined DOXYGEN || defined ESP_ZIGBEE_TRACE
/**
   Trace format for 64-bit address.

   @snippet light_sample/dimmable_light/bulb.c trace_64_example_variable
   @snippet light_sample/dimmable_light/bulb.c trace_64_example
*/
#define TRACE_FORMAT_64 "%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx"

/**
 Trace format for 128-bit number (key etc).

 @snippet se/energy_service_interface/se_esi_zc.c TRACE_128
*/
#define TRACE_FORMAT_128 "%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx:%02hx"

/**
   Trace format arguments for 64-bit address.

   @snippet light_sample/dimmable_light/bulb.c trace_64_example_variable
   @snippet light_sample/dimmable_light/bulb.c trace_64_example
*/
#define TRACE_ARG_64(a) (zb_uint8_t)((a)[7]),(zb_uint8_t)((a)[6]),(zb_uint8_t)((a)[5]),(zb_uint8_t)((a)[4]),(zb_uint8_t)((a)[3]),(zb_uint8_t)((a)[2]),(zb_uint8_t)((a)[1]),(zb_uint8_t)((a)[0])

/**
   Trace format arguments for 128-bit number.

   @snippet se/energy_service_interface/se_esi_zc.c TRACE_128
*/
#define TRACE_ARG_128(a) (zb_uint8_t)((a)[0]),(zb_uint8_t)((a)[1]),(zb_uint8_t)((a)[2]),(zb_uint8_t)((a)[3]),(zb_uint8_t)((a)[4]),(zb_uint8_t)((a)[5]),(zb_uint8_t)((a)[6]),(zb_uint8_t)((a)[7]),(zb_uint8_t)((a)[8]),(zb_uint8_t)((a)[9]),(zb_uint8_t)((a)[10]),(zb_uint8_t)((a)[11]),(zb_uint8_t)((a)[12]),(zb_uint8_t)((a)[13]),(zb_uint8_t)((a)[14]),(zb_uint8_t)((a)[15])


#else

/** @brief Trace format for 64-bit address - single argument for 8051. */
#define TRACE_FORMAT_64 "%A"
#define TRACE_FORMAT_128 "%A:%A"

/** @endcond */ /* DSR_TRACE */
/** @cond internals_doc */
typedef struct zb_addr64_struct_s
{
  zb_64bit_addr_t addr;
} ZB_PACKED_STRUCT zb_addr64_struct_t;

typedef struct zb_byte128_struct_s
{
  zb_uint8_t d[16];
} ZB_PACKED_STRUCT zb_byte128_struct_t;

/* Pass 8-bytes address as structure by value */
#define TRACE_ARG_64(a) *((zb_addr64_struct_t *)a)
/* Pass 16-bytes key as structure by value */
#define TRACE_ARG_128(a) *((zb_byte128_struct_t *)a)
/** @endcond */ /* internals_doc */
/** @cond DSR_TRACE */

#endif  /* ti file / !to file */
/** @} */ /* TRACE_DATA_FORMAT_ADDITIONAL */
/** @endcond */ /* DSR_TRACE */


/** @cond DSR_TRACE */
/**
 *  @addtogroup TRACE_GENERAL_MESSAGES
 *  @{
 */
/** @brief Error message. */
#define TRACE_ERROR TRACE_SUBSYSTEM_INFO, 0U
/** @brief Information message level 1. */
#define TRACE_INFO1 TRACE_SUBSYSTEM_INFO, 1U
/** @brief Information message level 2. */
#define TRACE_INFO2 TRACE_SUBSYSTEM_INFO, 2U
/** @brief Information message level 3. */
#define TRACE_INFO3 TRACE_SUBSYSTEM_INFO, 3U

/** @} */ /* TRACE_GENERAL_MESSAGES */

#ifdef ZB_TRACE_LEVEL
#ifndef ZB_TRACE_MASK
#define ZB_TRACE_MASK ((zb_uint_t)-1)
#endif  /* if not defined trace_mask */
#endif  /* if defined trace level */

/**
 *  @addtogroup TRACE_DATA_FORMAT
 *  @{
 */

/*
  Trace format constants for serial trace
*/

/* Keil and sdcc put byte values to the stack as is, but IAR/8051 casts it to 16-bit
 * integers, so constant lengths differs */

/**
 *  @brief Calculates trace argument size.
 *  @param n_h - number of Hs/Cs.
 *  @param n_d - number of Ds.
 *  @param n_l - number of Ls.
 *  @param n_p - number of Ps.
 *  @param n_a - number of As.
 *  @hideinitializer
 */
#ifdef KEIL

/* Keil pass 1-byte to varargs as is, pointer is 3-bytes, short is 2-bytes */

#ifndef ZB_BINARY_TRACE
#define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) __FILE__,__LINE__, (n_h + n_d*2 + n_l*4 + n_p*3 + n_a*8)
#else
#define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) ZB_TRACE_FILE_ID,__LINE__, (n_h + n_d*2 + n_l*4 + n_p*3 + n_a*8)
#endif

#elif defined ZB_PLATFORM_XAP5

#ifndef __XAP5_NEAR__
/* XAP5 passes bytes as shorts */
#ifndef ZB_BINARY_TRACE
#define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) __FILE__,__LINE__, (n_h*2 + n_d*2 + n_l*4 + n_p*4 + n_a*8)
#else
#define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) ZB_TRACE_FILE_ID,__LINE__, (n_h*2 + n_d*2 + n_l*4 + n_p*4 + n_a*8)
#endif
#else
#ifndef ZB_BINARY_TRACE
#define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) __FILE__,__LINE__, (n_h*2 + n_d*2 + n_l*4 + n_p*2 + n_a*8)
#else
#define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) ZB_TRACE_FILE_ID,__LINE__, (n_h*2 + n_d*2 + n_l*4 + n_p*2 + n_a*8)
#endif
#endif

#else  /* IAR & GCC - 32-bit */
/* IAR for Cortex passes 1-byte abd 2-bytes arguments as 4-bytes to vararg functions.
 * Pointers are 4-bytes. */

#if defined ZB_BINARY_TRACE && !defined ZB_TRACE_TO_SYSLOG && !defined ESP_ZIGBEE_TRACE
#if defined ZB_BINARY_AND_TEXT_TRACE_MODE
  #define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) __FILE__,ZB_TRACE_FILE_ID,__LINE__, (n_h*4 + n_d*4 + n_l*4 + n_p*4 + n_a*8)
#else
#define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) ZB_TRACE_FILE_ID,__LINE__, (n_h*4 + n_d*4 + n_l*4 + n_p*4 + n_a*8)
#endif
#else
#define TRACE_ARG_SIZE(n_h, n_d, n_l, n_p, n_a) __FILE__,__LINE__, (n_h*4 + n_d*4 + n_l*4 + n_p*4 + n_a*8)
#endif

#endif

#define FMT__0                                          TRACE_ARG_SIZE(0,0,0,0,0)
#define FMT__A                                          TRACE_ARG_SIZE(0,0,0,0,1)
#define FMT__A_A                                        TRACE_ARG_SIZE(0,0,0,0,2)
#define FMT__A_A_A_A                                    TRACE_ARG_SIZE(0,0,0,0,4)
#define FMT__A_D_A_P                                    TRACE_ARG_SIZE(0,1,0,1,2)
#define FMT__A_D_D                                      TRACE_ARG_SIZE(0,2,0,0,1)
#define FMT__A_D_D_D                                    TRACE_ARG_SIZE(0,3,0,0,1)
#define FMT__A_D_D_P_H                                  TRACE_ARG_SIZE(1,2,0,1,1)
#define FMT__A_D_H                                      TRACE_ARG_SIZE(1,1,0,0,1)
#define FMT__A_D_H_H                                    TRACE_ARG_SIZE(2,1,0,0,1)
#define FMT__A_D_P_H_H_H                                TRACE_ARG_SIZE(3,1,0,1,1)
#define FMT__A_H                                        TRACE_ARG_SIZE(1,0,0,0,1)
#define FMT__A_H_H                                      TRACE_ARG_SIZE(2,0,0,0,1)
#define FMT__A_H_H_H                                    TRACE_ARG_SIZE(3,0,0,0,1)
#define FMT__A_D_H_H_H                                  TRACE_ARG_SIZE(3,1,0,0,1)
#define FMT__A_P                                        TRACE_ARG_SIZE(0,0,0,1,1)
#define FMT__AA                                         TRACE_ARG_SIZE(0,0,0,0,2)
#define FMT__C                                          TRACE_ARG_SIZE(1,0,0,0,0)
#define FMT__C_C                                        TRACE_ARG_SIZE(2,0,0,0,0)
#define FMT__C_C_C                                      TRACE_ARG_SIZE(3,0,0,0,0)
#define FMT__C_C_C_C                                    TRACE_ARG_SIZE(4,0,0,0,0)
#define FMT__C_C_C_C_C                                  TRACE_ARG_SIZE(5,0,0,0,0)
#define FMT__C_C_H_H                                    TRACE_ARG_SIZE(4,0,0,0,0)
#define FMT__C_H                                        TRACE_ARG_SIZE(2,0,0,0,0)
#define FMT__D                                          TRACE_ARG_SIZE(0,1,0,0,0)
#define FMT__D_A                                        TRACE_ARG_SIZE(0,1,0,0,1)
#define FMT__D_A_A_H                                    TRACE_ARG_SIZE(1,1,0,0,2)
#define FMT__D_A_D_D_D_D_D_D_D_D                        TRACE_ARG_SIZE(0,9,0,0,1)
#define FMT__D_A_D_P_H_H_H                              TRACE_ARG_SIZE(3,2,0,1,1)
#define FMT__D_A_H_H                                    TRACE_ARG_SIZE(2,1,0,0,1)
#define FMT__D_A_P                                      TRACE_ARG_SIZE(0,1,0,1,1)
#define FMT__D_C                                        TRACE_ARG_SIZE(1,1,0,0,0)
#define FMT__D_D                                        TRACE_ARG_SIZE(0,2,0,0,0)
#define FMT__D_D_A                                      TRACE_ARG_SIZE(0,2,0,0,1)
#define FMT__D_D_A_A                                    TRACE_ARG_SIZE(0,2,0,0,2)
#define FMT__D_D_A_D                                    TRACE_ARG_SIZE(0,3,0,0,1)
#define FMT__D_D_A_D_D_D_D                              TRACE_ARG_SIZE(0,6,0,0,1)
#define FMT__D_D_D                                      TRACE_ARG_SIZE(0,3,0,0,0)
#define FMT__D_D_D_C                                    TRACE_ARG_SIZE(1,3,0,0,0)
#define FMT__D_D_D_D                                    TRACE_ARG_SIZE(0,4,0,0,0)
#define FMT__D_D_D_D_D                                  TRACE_ARG_SIZE(0,5,0,0,0)
#define FMT__D_D_D_D_D_D                                TRACE_ARG_SIZE(0,6,0,0,0)
#define FMT__D_D_D_D_D_D_D                              TRACE_ARG_SIZE(0,7,0,0,0)
#define FMT__D_D_D_D_D_D_D_D                            TRACE_ARG_SIZE(0,8,0,0,0)
#define FMT__D_D_D_D_D_D_D_D_D_D_D                      TRACE_ARG_SIZE(0,11,0,0,0)
#define FMT__D_D_D_D_D_D_D_D_D_D_D_D_D_D                TRACE_ARG_SIZE(0,14,0,0,0)
#define FMT__D_D_D_D_D_D_D_D_D_D_D_D_D_D_D              TRACE_ARG_SIZE(0,15,0,0,0)
#define FMT__D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D_D          TRACE_ARG_SIZE(0,17,0,0,0)
#define FMT__D_D_D_D_D_D_P                              TRACE_ARG_SIZE(0,6,0,1,0)
#define FMT__D_D_D_D_H                                  TRACE_ARG_SIZE(1,4,0,0,0)
#define FMT__D_D_D_H                                    TRACE_ARG_SIZE(1,3,0,0,0)
#define FMT__D_D_D_H_H                                  TRACE_ARG_SIZE(2,3,0,0,0)
#define FMT__D_D_D_L                                    TRACE_ARG_SIZE(0,3,1,0,0)
#define FMT__D_D_D_L_L                                  TRACE_ARG_SIZE(0,3,2,0,0)
#define FMT__D_D_D_P                                    TRACE_ARG_SIZE(0,3,0,1,0)
#define FMT__D_D_H                                      TRACE_ARG_SIZE(1,2,0,0,0)
#define FMT__D_D_H_D_H                                  TRACE_ARG_SIZE(2,3,0,0,0)
#define FMT__D_D_H_H                                    TRACE_ARG_SIZE(2,2,0,0,0)
#define FMT__D_D_H_P_D                                  TRACE_ARG_SIZE(1,3,0,1,0)
#define FMT__D_D_L                                      TRACE_ARG_SIZE(0,2,1,0,0)
#define FMT__D_D_L_P                                    TRACE_ARG_SIZE(0,2,1,1,0)
#define FMT__D_D_P                                      TRACE_ARG_SIZE(0,2,0,1,0)
#define FMT__D_D_P_D                                    TRACE_ARG_SIZE(0,3,0,1,0)
#define FMT__D_P_P_D                                    TRACE_ARG_SIZE(0,2,0,2,0)
#define FMT__D_P_P_P                                    TRACE_ARG_SIZE(0,1,0,3,0)
#define FMT__D_D_P_D_D                                  TRACE_ARG_SIZE(0,4,0,1,0)
#define FMT__D_D_P_D_L                                  TRACE_ARG_SIZE(0,3,1,1,0)
#define FMT__D_D_P_H                                    TRACE_ARG_SIZE(1,2,0,1,0)
#define FMT__D_D_P_H_H_H                                TRACE_ARG_SIZE(3,2,0,1,0)
#define FMT__D_D_P_P_P                                  TRACE_ARG_SIZE(0,2,0,3,0)
#define FMT__D_H                                        TRACE_ARG_SIZE(1,1,0,0,0)
#define FMT__D_H_A_D_D_D_H                              TRACE_ARG_SIZE(2,4,0,0,1)
#define FMT__D_H_D                                      TRACE_ARG_SIZE(1,2,0,0,0)
#define FMT__D_H_D_D                                    TRACE_ARG_SIZE(1,3,0,0,0)
#define FMT__D_H_D_D_H                                  TRACE_ARG_SIZE(2,3,0,0,0)
#define FMT__D_H_D_D_H_D                                TRACE_ARG_SIZE(2,4,0,0,0)
#define FMT__D_H_D_H                                    TRACE_ARG_SIZE(2,2,0,0,0)
#define FMT__D_H_D_H_H                                  TRACE_ARG_SIZE(3,2,0,0,0)
#define FMT__D_H_D_P                                    TRACE_ARG_SIZE(1,2,0,1,0)
#define FMT__D_H_D_P_D                                  TRACE_ARG_SIZE(1,3,0,1,0)
#define FMT__D_H_H                                      TRACE_ARG_SIZE(2,1,0,0,0)
#define FMT__D_H_H_D                                    TRACE_ARG_SIZE(2,2,0,0,0)
#define FMT__D_H_H_D_H                                  TRACE_ARG_SIZE(3,2,0,0,0)
#define FMT__D_H_H_D_H_D                                TRACE_ARG_SIZE(3,3,0,0,0)
#define FMT__D_H_H_D_D_D_D                              TRACE_ARG_SIZE(2,5,0,0,0)
#define FMT__D_H_H_H                                    TRACE_ARG_SIZE(3,1,0,0,0)
#define FMT__D_H_H_H_H                                  TRACE_ARG_SIZE(4,1,0,0,0)
#define FMT__D_H_H_H_H_H                                TRACE_ARG_SIZE(5,1,0,0,0)
#define FMT__D_H_H_H_H_H_H_D_D_D_D                      TRACE_ARG_SIZE(6,5,0,0,0)
#define FMT__D_H_L                                      TRACE_ARG_SIZE(1,1,1,0,0)
#define FMT__D_H_P                                      TRACE_ARG_SIZE(1,1,0,1,0)
#define FMT__D_L                                        TRACE_ARG_SIZE(0,1,1,0,0)
#define FMT__D_L_L                                      TRACE_ARG_SIZE(0,1,2,0,0)
#define FMT__D_L_L_D                                    TRACE_ARG_SIZE(0,2,2,0,0)
#define FMT__D_L_L_H                                    TRACE_ARG_SIZE(1,1,2,0,0)
#define FMT__D_L_L_L                                    TRACE_ARG_SIZE(0,1,3,0,0)
#define FMT__D_P                                        TRACE_ARG_SIZE(0,1,0,1,0)
#define FMT__D_P_D                                      TRACE_ARG_SIZE(0,2,0,1,0)
#define FMT__D_P_D_D                                    TRACE_ARG_SIZE(0,3,0,1,0)
#define FMT__D_P_H_H_D_D                                TRACE_ARG_SIZE(2,3,0,1,0)
#define FMT__D_P_H_H_D_H_H                              TRACE_ARG_SIZE(4,2,0,1,0)
#define FMT__D_P_H_H_H_H                                TRACE_ARG_SIZE(4,1,0,1,0)
#define FMT__D_P_H_H_H_H_H                              TRACE_ARG_SIZE(5,1,0,1,0)
#define FMT__D_P_H_H_H_H_H_H                            TRACE_ARG_SIZE(6,1,0,1,0)
#define FMT__D_P_H_P                                    TRACE_ARG_SIZE(1,1,0,1,0)
#define FMT__D_P_P                                      TRACE_ARG_SIZE(0,1,0,2,0)
#define FMT__D_P_P_D_D                                  TRACE_ARG_SIZE(0,3,0,2,0)
#define FMT__D_P_P_D_D_P                                TRACE_ARG_SIZE(0,3,0,3,0)
#define FMT__D_P_P_D_D_H_H                              TRACE_ARG_SIZE(2,3,0,2,0)
#define FMT__D_P_P_H                                    TRACE_ARG_SIZE(1,1,0,2,0)
#define FMT__H                                          TRACE_ARG_SIZE(1,0,0,0,0)
#define FMT__H_A                                        TRACE_ARG_SIZE(1,0,0,0,1)
#define FMT__H_A_A                                      TRACE_ARG_SIZE(1,0,0,0,2)
#define FMT__H_A_A_H_H                                  TRACE_ARG_SIZE(3,0,0,0,2)
#define FMT__H_A_H                                      TRACE_ARG_SIZE(2,0,0,0,1)
#define FMT__H_A_H_H_H                                  TRACE_ARG_SIZE(4,0,0,0,1)
#define FMT__H_A_H_H_H_H_H_H_H_H                        TRACE_ARG_SIZE(9,0,0,0,1)
#define FMT__H_C_D_C                                    TRACE_ARG_SIZE(3,1,0,0,0)
#define FMT__H_C_H_C                                    TRACE_ARG_SIZE(4,0,0,0,0)
#define FMT__H_D                                        TRACE_ARG_SIZE(1,1,0,0,0)
#define FMT__H_D_L                                      TRACE_ARG_SIZE(1,1,1,0,0)
#define FMT__H_D_A                                      TRACE_ARG_SIZE(1,1,0,0,1)
#define FMT__H_D_A_H_D                                  TRACE_ARG_SIZE(2,2,0,0,1)
#define FMT__H_D_A_H_H                                  TRACE_ARG_SIZE(3,1,0,0,1)
#define FMT__H_D_A_H_H_H_H                              TRACE_ARG_SIZE(5,1,0,0,1)
#define FMT__H_D_D                                      TRACE_ARG_SIZE(1,2,0,0,0)
#define FMT__H_D_D_D                                    TRACE_ARG_SIZE(1,3,0,0,0)
#define FMT__H_D_D_D_D                                  TRACE_ARG_SIZE(1,4,0,0,0)
#define FMT__H_D_D_D_D_H                                TRACE_ARG_SIZE(2,4,0,0,0)
#define FMT__H_D_D_D_H_H_D                              TRACE_ARG_SIZE(3,4,0,0,0)
#define FMT__H_D_D_H                                    TRACE_ARG_SIZE(2,2,0,0,0)
#define FMT__H_D_D_H_D                                  TRACE_ARG_SIZE(2,3,0,0,0)
#define FMT__H_D_D_H_D_H                                TRACE_ARG_SIZE(3,3,0,0,0)
#define FMT__H_D_D_H_H                                  TRACE_ARG_SIZE(3,2,0,0,0)
#define FMT__H_D_D_H_H_H                                TRACE_ARG_SIZE(4,2,0,0,0)
#define FMT__H_D_D_H_H_H_H                              TRACE_ARG_SIZE(5,2,0,0,0)
#define FMT__H_D_D_H_P                                  TRACE_ARG_SIZE(2,2,0,1,0)
#define FMT__H_D_D_P                                    TRACE_ARG_SIZE(1,2,0,1,0)
#define FMT__H_D_H                                      TRACE_ARG_SIZE(2,1,0,0,0)
#define FMT__H_D_H_D                                    TRACE_ARG_SIZE(2,2,0,0,0)
#define FMT__H_D_H_D_D                                  TRACE_ARG_SIZE(2,3,0,0,0)
#define FMT__H_D_H_D_H                                  TRACE_ARG_SIZE(3,2,0,0,0)
#define FMT__H_D_H_D_H_H                                TRACE_ARG_SIZE(4,2,0,0,0)
#define FMT__H_D_H_A                                    TRACE_ARG_SIZE(2,1,0,0,1)
#define FMT__H_D_H_H                                    TRACE_ARG_SIZE(3,1,0,0,0)
#define FMT__H_D_H_P_D                                  TRACE_ARG_SIZE(2,2,0,1,0)
#define FMT__H_D_H_H_H                                  TRACE_ARG_SIZE(4,1,0,0,0)
#define FMT__H_D_H_H_H_H                                TRACE_ARG_SIZE(5,1,0,0,0)
#define FMT__H_D_H_H_H_H_H_H                            TRACE_ARG_SIZE(7,1,0,0,0)
#define FMT__H_D_P_H_H_H_H_H                            TRACE_ARG_SIZE(6,1,0,1,0)
#define FMT__H_D_P_H_H_H_H_H_H_H                        TRACE_ARG_SIZE(8,1,0,1,0)
#define FMT__H_D_P_P_P                                  TRACE_ARG_SIZE(1,1,0,3,0)
#define FMT__H_H                                        TRACE_ARG_SIZE(2,0,0,0,0)
#define FMT__H_H_A                                      TRACE_ARG_SIZE(2,0,0,0,1)
#define FMT__H_H_A_A                                    TRACE_ARG_SIZE(2,0,0,0,2)
#define FMT__H_H_D                                      TRACE_ARG_SIZE(2,1,0,0,0)
#define FMT__H_H_D_A                                    TRACE_ARG_SIZE(2,1,0,0,1)
#define FMT__H_H_D_D                                    TRACE_ARG_SIZE(2,2,0,0,0)
#define FMT__H_H_D_D_D                                  TRACE_ARG_SIZE(2,3,0,0,0)
#define FMT__H_H_D_H                                    TRACE_ARG_SIZE(3,1,0,0,0)
#define FMT__H_H_D_H_H_H_H_H_H_H_H                      TRACE_ARG_SIZE(10,1,0,0,0)
#define FMT__D_H_H_H_H_H_H_H_H                          TRACE_ARG_SIZE(8,1,0,0,0)
#define FMT__H_H_D_H_P                                  TRACE_ARG_SIZE(3,1,0,1,0)
#define FMT__H_D_P                                      TRACE_ARG_SIZE(1,1,0,1,0)
#define FMT__H_H_D_P                                    TRACE_ARG_SIZE(2,1,0,1,0)
#define FMT__H_H_H                                      TRACE_ARG_SIZE(3,0,0,0,0)
#define FMT__H_H_H_D                                    TRACE_ARG_SIZE(3,1,0,0,0)
#define FMT__H_H_H_D_D                                  TRACE_ARG_SIZE(3,2,0,0,0)
#define FMT__H_H_H_D_D_H_A_H_A                          TRACE_ARG_SIZE(5,2,0,0,2)
#define FMT__H_D_H_H_H_D_D_H_A_H_A                      TRACE_ARG_SIZE(6,3,0,0,2)
#define FMT__D_H_H_H_D_D_H_A_H_A                        TRACE_ARG_SIZE(5,3,0,0,2)
#define FMT__H_H_H_D_D_D                                TRACE_ARG_SIZE(3,3,0,0,0)
#define FMT__H_H_H_D_H                                  TRACE_ARG_SIZE(4,1,0,0,0)
#define FMT__H_H_H_D_H_P                                TRACE_ARG_SIZE(4,1,0,1,0)
#define FMT__H_H_H_H                                    TRACE_ARG_SIZE(4,0,0,0,0)
#define FMT__H_H_H_H_D                                  TRACE_ARG_SIZE(4,1,0,0,0)
#define FMT__H_H_H_H_D_H                                TRACE_ARG_SIZE(5,1,0,0,0)
#define FMT__H_H_H_H_H                                  TRACE_ARG_SIZE(5,0,0,0,0)
#define FMT__H_H_H_H_H_H                                TRACE_ARG_SIZE(6,0,0,0,0)
#define FMT__H_H_H_H_H_H_H                              TRACE_ARG_SIZE(7,0,0,0,0)
#define FMT__H_H_H_H_H_H_H_H                            TRACE_ARG_SIZE(8,0,0,0,0)
#define FMT__H_H_H_H_H_H_H_H_H                          TRACE_ARG_SIZE(9,0,0,0,0)
#define FMT__H_H_H_H_H_H_H_H_P_P                        TRACE_ARG_SIZE(8,0,0,2,0)
#define FMT__H_H_H_H_H_H_P_P                            TRACE_ARG_SIZE(6,0,0,2,0)
#define FMT__H_H_H_H_H_H_P_P_P_P                        TRACE_ARG_SIZE(6,2,0,4,0)
#define FMT__H_H_H_H_P_P_P                              TRACE_ARG_SIZE(4,0,0,3,0)
#define FMT__H_H_H_P                                    TRACE_ARG_SIZE(3,0,0,1,0)
#define FMT__H_D_H_P                                    TRACE_ARG_SIZE(2,1,0,1,0)
#define FMT__H_H_L                                      TRACE_ARG_SIZE(2,0,1,0,0)
#define FMT__H_H_L_H                                    TRACE_ARG_SIZE(3,0,1,0,0)
#define FMT__H_H_H_L                                    TRACE_ARG_SIZE(3,0,1,0,0)
#define FMT__H_H_H_H_L                                  TRACE_ARG_SIZE(4,0,1,0,0)
#define FMT__H_H_L_H_P                                  TRACE_ARG_SIZE(3,0,1,4,0)
#define FMT__H_H_P                                      TRACE_ARG_SIZE(2,0,0,1,0)
#define FMT__H_H_P_D                                    TRACE_ARG_SIZE(2,1,0,1,0)
#define FMT__H_H_P_P_P                                  TRACE_ARG_SIZE(2,0,0,3,0)
#define FMT__H_L                                        TRACE_ARG_SIZE(1,0,1,0,0)
#define FMT__H_L_A                                      TRACE_ARG_SIZE(1,0,1,0,1)
#define FMT__H_L_D                                      TRACE_ARG_SIZE(1,1,1,0,0)
#define FMT__H_L_L                                      TRACE_ARG_SIZE(1,0,2,0,0)
#define FMT__H_L_H                                      TRACE_ARG_SIZE(2,0,1,0,0)
#define FMT__H_L_D_D                                    TRACE_ARG_SIZE(1,2,1,0,0)
#define FMT__H_L_D_D_D                                  TRACE_ARG_SIZE(1,3,1,0,0)
#define FMT__H_L_D_P                                    TRACE_ARG_SIZE(1,1,1,1,0)
#define FMT__H_L_H_D                                    TRACE_ARG_SIZE(2,1,1,0,0)
#define FMT__H_L_H_D_D                                  TRACE_ARG_SIZE(2,2,1,0,0)
#define FMT__H_L_H_H_H_H_H_H_H_H_H_H_H_H                TRACE_ARG_SIZE(13,0,1,0,0)
#define FMT__H_D_H_H_H_H_H_H_H_H_H_H_H_H                TRACE_ARG_SIZE(13,1,0,0,0)
#define FMT__H_L_H_H_P                                  TRACE_ARG_SIZE(3,0,1,4,0)
#define FMT__H_L_L_H_A                                  TRACE_ARG_SIZE(2,0,2,0,1)
#define FMT__H_L_D_D_H                                  TRACE_ARG_SIZE(2,2,1,0,0)
#define FMT__H_P_D_P                                    TRACE_ARG_SIZE(1,1,0,2,0)
#define FMT__H_P_P_D_P                                  TRACE_ARG_SIZE(1,1,0,3,0)
#define FMT__H_P_D_D_H                                  TRACE_ARG_SIZE(2,2,0,1,0)
#define FMT__H_P                                        TRACE_ARG_SIZE(1,0,0,1,0)
#define FMT__H_P_D                                      TRACE_ARG_SIZE(1,1,0,1,0)
#define FMT__H_P_D_D_H_P                                TRACE_ARG_SIZE(2,2,0,2,0)
#define FMT__H_P_D_D_P                                  TRACE_ARG_SIZE(1,2,0,2,0)
#define FMT__H_P_H                                      TRACE_ARG_SIZE(2,0,0,1,0)
#define FMT__H_P_H_D                                    TRACE_ARG_SIZE(2,1,0,1,0)
#define FMT__H_P_H_D_H                                  TRACE_ARG_SIZE(3,1,0,1,0)
#define FMT__H_P_H_H                                    TRACE_ARG_SIZE(3,0,0,1,0)
#define FMT__H_P_H_H_H                                  TRACE_ARG_SIZE(4,0,0,1,0)
#define FMT__H_P_H_H_D                                  TRACE_ARG_SIZE(3,1,0,1,0)
#define FMT__H_P_H_H_H_H                                TRACE_ARG_SIZE(5,0,0,1,0)
#define FMT__H_P_H_P                                    TRACE_ARG_SIZE(2,0,0,2,0)
#define FMT__H_P_H_P_H                                  TRACE_ARG_SIZE(3,0,0,2,0)
#define FMT__H_P_H_P_H_H                                TRACE_ARG_SIZE(4,0,0,2,0)
#define FMT__H_P_H_P_H_H_P                              TRACE_ARG_SIZE(4,0,0,3,0)
#define FMT__H_P_L_H                                    TRACE_ARG_SIZE(2,0,1,1,0)
#define FMT__H_P_P_P                                    TRACE_ARG_SIZE(1,0,0,3,0)
#define FMT__H_P_P_P_D_P_P                              TRACE_ARG_SIZE(1,1,0,5,0)
#define FMT__H_P_P_P_P                                  TRACE_ARG_SIZE(1,0,0,4,0)
#define FMT__H_P_P_P_P_P                                TRACE_ARG_SIZE(1,0,0,5,0)
#define FMT__L                                          TRACE_ARG_SIZE(0,0,1,0,0)
#define FMT__L_A                                        TRACE_ARG_SIZE(0,0,1,0,1)
#define FMT__L_D                                        TRACE_ARG_SIZE(0,1,1,0,0)
#define FMT__L_D_D                                      TRACE_ARG_SIZE(0,2,1,0,0)
#define FMT__L_D_D_D                                    TRACE_ARG_SIZE(0,3,1,0,0)
#define FMT__L_H                                        TRACE_ARG_SIZE(1,0,1,0,0)
#define FMT__L_H_H                                      TRACE_ARG_SIZE(2,0,1,0,0)
#define FMT__L_H_H_H                                    TRACE_ARG_SIZE(3,0,1,0,0)
#define FMT__L_L                                        TRACE_ARG_SIZE(0,0,2,0,0)
#define FMT__L_L_L                                      TRACE_ARG_SIZE(0,0,3,0,0)
#define FMT__L_L_L_H                                    TRACE_ARG_SIZE(1,0,3,0,0)
#define FMT__L_L_A_A                                    TRACE_ARG_SIZE(0,0,2,0,2)
#define FMT__L_D_A                                      TRACE_ARG_SIZE(0,1,1,0,1)
#define FMT__L_D_A_A                                    TRACE_ARG_SIZE(0,1,1,0,2)
#define FMT__L_L_A_A_H_L_H_H_H_H_H_H_H_H_H_H_H_H        TRACE_ARG_SIZE(13,0,3,0,2)
#define FMT__L_L_D_D                                    TRACE_ARG_SIZE(0,2,2,0,0)
#define FMT__L_L_H_L_H_H_H_H_H_H_H_H_H_H_H_H            TRACE_ARG_SIZE(13,0,3,0,0)
#define FMT__L_L_L_D                                    TRACE_ARG_SIZE(0,1,3,0,0)
#define FMT__L_L_L_D_L_D                                TRACE_ARG_SIZE(0,2,4,0,0)
#define FMT__L_L_L_L                                    TRACE_ARG_SIZE(0,0,4,0,0)
#define FMT__L_L_L_L_L                                  TRACE_ARG_SIZE(0,0,5,0,0)
#define FMT__L_L_L_L_L_L                                TRACE_ARG_SIZE(0,0,6,0,0)
#define FMT__L_L_L_L_L_L_L                              TRACE_ARG_SIZE(0,0,7,0,0)
#define FMT__L_P                                        TRACE_ARG_SIZE(0,0,1,1,0)
#define FMT__L_P_L                                      TRACE_ARG_SIZE(0,0,2,1,0)
#define FMT__P                                          TRACE_ARG_SIZE(0,0,0,1,0)
#define FMT__P_A                                        TRACE_ARG_SIZE(0,0,0,1,1)
#define FMT__P_D                                        TRACE_ARG_SIZE(0,1,0,1,0)
#define FMT__P_D_D                                      TRACE_ARG_SIZE(0,2,0,1,0)
#define FMT__P_D_D_D                                    TRACE_ARG_SIZE(0,3,0,1,0)
#define FMT__P_D_D_D_D                                  TRACE_ARG_SIZE(0,4,0,1,0)
#define FMT__P_D_D_H_D                                  TRACE_ARG_SIZE(1,3,0,1,0)
#define FMT__P_D_D_D_D_D                                TRACE_ARG_SIZE(0,5,0,1,0)
#define FMT__P_D_D_D_D_D_D                              TRACE_ARG_SIZE(0,6,0,1,0)
#define FMT__P_D_D_D_D_D_D_D                            TRACE_ARG_SIZE(0,7,0,1,0)
#define FMT__P_D_D_D_H_D                                TRACE_ARG_SIZE(1,4,0,1,0)
#define FMT__P_D_D_H                                    TRACE_ARG_SIZE(1,2,0,1,0)
#define FMT__P_D_D_L_L_P                                TRACE_ARG_SIZE(0,2,2,2,0)
#define FMT__P_D_H                                      TRACE_ARG_SIZE(1,1,0,1,0)
#define FMT__P_D_P                                      TRACE_ARG_SIZE(0,1,0,2,0)
#define FMT__P_D_L                                      TRACE_ARG_SIZE(0,1,1,1,0)
#define FMT__P_D_P_D                                    TRACE_ARG_SIZE(0,2,0,2,0)
#define FMT__P_D_P_P                                    TRACE_ARG_SIZE(0,1,0,3,0)
#define FMT__P_H                                        TRACE_ARG_SIZE(1,0,0,1,0)
#define FMT__P_H_D                                      TRACE_ARG_SIZE(1,1,0,1,0)
#define FMT__P_H_D_D                                    TRACE_ARG_SIZE(1,2,0,1,0)
#define FMT__P_H_D_D_H                                  TRACE_ARG_SIZE(2,2,0,1,0)
#define FMT__P_H_D_P                                    TRACE_ARG_SIZE(1,1,0,2,0)
#define FMT__P_H_D_H                                    TRACE_ARG_SIZE(2,1,0,1,0)
#define FMT__P_H_D_L                                    TRACE_ARG_SIZE(1,1,1,1,0)
#define FMT__P_H_H                                      TRACE_ARG_SIZE(2,0,0,1,0)
#define FMT__P_H_H_D_D                                  TRACE_ARG_SIZE(2,2,0,1,0)
#define FMT__P_H_H_H                                    TRACE_ARG_SIZE(3,0,0,1,0)
#define FMT__P_H_H_H_A                                  TRACE_ARG_SIZE(3,0,0,1,1)
#define FMT__P_H_H_H_D                                  TRACE_ARG_SIZE(3,1,0,1,0)
#define FMT__P_H_H_H_H                                  TRACE_ARG_SIZE(4,0,0,1,0)
#define FMT__P_H_H_H_H_D                                TRACE_ARG_SIZE(4,1,0,1,0)
#define FMT__P_H_H_H_H_H_H                              TRACE_ARG_SIZE(6,0,0,1,0)
#define FMT__P_H_H_H_H_H_H_H                            TRACE_ARG_SIZE(7,0,0,1,0)
#define FMT__P_H_H_H_L                                  TRACE_ARG_SIZE(3,0,1,1,0)
#define FMT__P_H_H_L                                    TRACE_ARG_SIZE(2,0,1,1,0)
#define FMT__P_H_L                                      TRACE_ARG_SIZE(1,0,1,1,0)
#define FMT__P_H_L_H_L                                  TRACE_ARG_SIZE(2,0,2,1,0)
#define FMT__P_H_P                                      TRACE_ARG_SIZE(1,0,0,2,0)
#define FMT__P_H_P_D                                    TRACE_ARG_SIZE(1,1,0,2,0)
#define FMT__P_H_P_D_D                                  TRACE_ARG_SIZE(1,2,0,2,0)
#define FMT__P_H_P_D_D_D                                TRACE_ARG_SIZE(1,3,0,2,0)
#define FMT__P_H_P_H                                    TRACE_ARG_SIZE(2,0,0,2,0)
#define FMT__P_H_P_H_L                                  TRACE_ARG_SIZE(2,0,1,2,0)
#define FMT__P_H_P_P                                    TRACE_ARG_SIZE(1,0,0,3,0)
#define FMT__P_H_P_L                                    TRACE_ARG_SIZE(1,0,1,2,0)
#define FMT__P_H_P_P_H                                  TRACE_ARG_SIZE(2,0,0,3,0)
#define FMT__P_H_P_P_P                                  TRACE_ARG_SIZE(1,0,0,4,0)
#define FMT__P_L                                        TRACE_ARG_SIZE(0,0,1,1,0)
#define FMT__P_L_D                                      TRACE_ARG_SIZE(0,1,1,1,0)
#define FMT__P_L_H                                      TRACE_ARG_SIZE(1,0,1,1,0)
#define FMT__P_L_P                                      TRACE_ARG_SIZE(0,0,1,2,0)
#define FMT__P_L_L_D                                    TRACE_ARG_SIZE(0,1,2,1,0)
#define FMT__P_L_L_L                                    TRACE_ARG_SIZE(0,0,3,1,0)
#define FMT__P_L_P_L                                    TRACE_ARG_SIZE(0,0,2,2,0)
#define FMT__P_L_P_P_P                                  TRACE_ARG_SIZE(0,0,1,4,0)
#define FMT__P_P                                        TRACE_ARG_SIZE(0,0,0,2,0)
#define FMT__P_P_L                                      TRACE_ARG_SIZE(0,0,1,2,0)
#define FMT__P_P_D                                      TRACE_ARG_SIZE(0,1,0,2,0)
#define FMT__P_P_D_D                                    TRACE_ARG_SIZE(0,2,0,2,0)
#define FMT__P_P_D_D_H                                  TRACE_ARG_SIZE(1,2,0,2,0)
#define FMT__P_P_D_D_P                                  TRACE_ARG_SIZE(0,2,0,3,0)
#define FMT__P_P_D_H_H                                  TRACE_ARG_SIZE(2,1,0,2,0)
#define FMT__P_P_D_H_P                                  TRACE_ARG_SIZE(1,1,0,3,0)
#define FMT__P_P_D_P                                    TRACE_ARG_SIZE(0,1,0,3,0)
#define FMT__P_P_D_P_D                                  TRACE_ARG_SIZE(0,2,0,3,0)
#define FMT__P_P_D_P_H                                  TRACE_ARG_SIZE(1,1,0,3,0)
#define FMT__P_P_H                                      TRACE_ARG_SIZE(1,0,0,2,0)
#define FMT__P_P_H_D                                    TRACE_ARG_SIZE(1,1,0,2,0)
#define FMT__P_P_H_H                                    TRACE_ARG_SIZE(2,0,0,2,0)
#define FMT__P_P_H_H_P                                  TRACE_ARG_SIZE(2,0,0,3,0)
#define FMT__P_P_H_H_H                                  TRACE_ARG_SIZE(3,0,0,2,0)
#define FMT__P_P_H_P                                    TRACE_ARG_SIZE(1,0,0,3,0)
#define FMT__P_P_H_P_D_P                                TRACE_ARG_SIZE(1,2,0,4,0)
#define FMT__P_P_H_P_H                                  TRACE_ARG_SIZE(2,0,0,3,0)
#define FMT__P_P_L_D                                    TRACE_ARG_SIZE(0,1,1,2,0)
#define FMT__P_P_L_L_H                                  TRACE_ARG_SIZE(1,0,2,2,0)
#define FMT__P_P_P                                      TRACE_ARG_SIZE(0,0,0,3,0)
#define FMT__P_P_P_H                                    TRACE_ARG_SIZE(1,0,0,3,0)
#define FMT__P_P_P_D                                    TRACE_ARG_SIZE(0,1,0,3,0)
#define FMT__P_P_P_D_P                                  TRACE_ARG_SIZE(0,1,0,4,0)
#define FMT__P_P_P_H_H                                  TRACE_ARG_SIZE(2,0,0,3,0)
#define FMT__P_P_P_L_H                                  TRACE_ARG_SIZE(1,0,1,3,0)
#define FMT__P_P_P_L_D_H                                TRACE_ARG_SIZE(1,1,1,3,0)
#define FMT__P_P_P_P                                    TRACE_ARG_SIZE(0,0,0,4,0)
#define FMT__P_L_H_H                                    TRACE_ARG_SIZE(2,0,1,1,0)
#define FMT__P_P_P_P_P                                  TRACE_ARG_SIZE(0,0,0,5,0)
#define FMT__D_D_L_D                                    TRACE_ARG_SIZE(0,3,1,0,0)
#define FMT__D_D_L_L                                    TRACE_ARG_SIZE(0,2,2,0,0)
#define FMT__D_D_D_D_L_L                                TRACE_ARG_SIZE(0,4,2,0,0)
#define FMT__L_D_D_D_D_L_L                              TRACE_ARG_SIZE(0,4,3,0,0)
#define FMT__L_L_L_D_D                                  TRACE_ARG_SIZE(0,2,3,0,0)
#define FMT__D_H_P_H                                    TRACE_ARG_SIZE(2,1,0,1,0)
#define FMT__H_D_H_H_D                                  TRACE_ARG_SIZE(3,2,0,0,0)
#define FMT__D_A_B_H_H                                  TRACE_ARG_SIZE(2,1,0,0,3)
#define FMT__A_A_P                                      TRACE_ARG_SIZE(0,0,0,1,2)
#define FMT__B                                          TRACE_ARG_SIZE(0,0,0,0,2)
#define FMT__B_H_B                                      TRACE_ARG_SIZE(1,0,0,0,4)
#define FMT__B_B_B                                      TRACE_ARG_SIZE(0,0,0,0,6)
#define FMT__D_H_D_B                                    TRACE_ARG_SIZE(1,2,0,0,2)
#define FMT__D_H_H_B                                    TRACE_ARG_SIZE(2,1,0,0,2)
#define FMT__H_H_A_B                                    TRACE_ARG_SIZE(2,0,0,0,3)
#define FMT__A_D                                        TRACE_ARG_SIZE(0,1,0,0,1)
#define FMT__L_D_P_H                                    TRACE_ARG_SIZE(1,1,1,1,0)
#define FMT__D_D_H_H_H                                  TRACE_ARG_SIZE(3,2,0,0,0)
#define FMT__P_B_D_D                                    TRACE_ARG_SIZE(0,2,0,1,2)
#define FMT__H_B_H_B                                    TRACE_ARG_SIZE(2,0,0,0,4)
#define FMT__B_A                                        TRACE_ARG_SIZE(0,0,0,0,3)
#define FMT__D_H_A                                      TRACE_ARG_SIZE(1,1,0,0,1)
#define FMT__D_H_A_H                                    TRACE_ARG_SIZE(2,1,0,0,1)
#define FMT__D_A_H                                      TRACE_ARG_SIZE(1,1,0,0,1)
#define FMT__D_D_D_A_B                                  TRACE_ARG_SIZE(0,3,0,0,3)
#define FMT__D_B_A                                      TRACE_ARG_SIZE(0,1,0,0,3)
#define FMT__H_B_A                                      TRACE_ARG_SIZE(1,0,0,0,3)
#define FMT__D_D_H_H_D_D_D_D                            TRACE_ARG_SIZE(2,6,0,0,0)
#define FMT__P_D_H_H_H_A                                TRACE_ARG_SIZE(3,1,0,1,1)
#define FMT__H_A_H_H_H_H_H                              TRACE_ARG_SIZE(6,0,0,0,1)
#define FMT__H_D_A_H                                    TRACE_ARG_SIZE(2,1,0,0,1)
#define FMT__D_A_H_D                                    TRACE_ARG_SIZE(1,2,0,0,1)
#define FMT__D_D_A_H                                    TRACE_ARG_SIZE(1,2,0,0,1)
#define FMT__P_H_H_H_H_H_H_H_H                          TRACE_ARG_SIZE(8,0,0,1,0)
#define FMT__D_D_H_L_H_H                                TRACE_ARG_SIZE(3,2,1,0,0)
#define FMT__D_H_L_H_H                                  TRACE_ARG_SIZE(3,1,1,0,0)
#define FMT__D_H_L_L_D_D_D                              TRACE_ARG_SIZE(1,4,2,0,0)
#define FMT__H_D_H_L_H_H                                TRACE_ARG_SIZE(4,1,1,0,0)
#define FMT__D_H_D_H_L_H_H                              TRACE_ARG_SIZE(4,2,1,0,0)
#define FMT__D_D_H_H_H_D_D_D_D                          TRACE_ARG_SIZE(3,6,0,0,0)
#define FMT__H_D_H_L                                    TRACE_ARG_SIZE(2,1,1,0,0)
#define FMT__H_H_L_L                                    TRACE_ARG_SIZE(2,0,2,0,0)
#define FMT__D_D_H_H_D_H_D_D_D_D                        TRACE_ARG_SIZE(3,7,0,0,0)
#define FMT__D_A_D_A                                    TRACE_ARG_SIZE(0,2,0,0,2)
#define FMT__H_H_H_L_H_H_H                              TRACE_ARG_SIZE(6,0,1,0,0)
#define FMT__H_D_D_D_D_D                                TRACE_ARG_SIZE(1,5,0,0,0)
#define FMT__P_P_D_D_L                                  TRACE_ARG_SIZE(0,2,1,2,0)
#define FMT__P_L_L                                      TRACE_ARG_SIZE(0,0,2,1,0)
#define FMT__P_P_L_L                                    TRACE_ARG_SIZE(0,0,2,2,0)
#define FMT__D_D_H_D_D_H                                TRACE_ARG_SIZE(2,4,0,0,0)
#define FMT__NC(N)                                      TRACE_ARG_SIZE(N,0,0,0,0)
#define FMT__H_L_H_H                                    TRACE_ARG_SIZE(3,0,1,0,0)
#define FMT__A_B                                        TRACE_ARG_SIZE(0,0,0,1,1)
#define FMT__D_H_H_L_D_D_D                              TRACE_ARG_SIZE(2,4,1,0,0)
#define FMT__H_H_D_D_H                                  TRACE_ARG_SIZE(3,3,0,0,0)
#define FMT__L_D_H_H_H_H                                TRACE_ARG_SIZE(4,1,1,0,0)
#define FMT__P_A_A                                      TRACE_ARG_SIZE(0,0,0,1,2)
#define FMT__D_D_P_H_H_H_H                              TRACE_ARG_SIZE(4,2,1,0,0)
#define FMT__P_P_D_D_D                                  TRACE_ARG_SIZE(0,3,0,2,0)
#define FMT__P_P_D_D_D_P                                TRACE_ARG_SIZE(0,3,0,3,0)
#define FMT__P_P_P_D_D_D_P                              TRACE_ARG_SIZE(0,3,0,4,0)
#define FMT__D_D_H_H_H_H                                TRACE_ARG_SIZE(2,4,0,0,0)
#define FMT__P_D_P_D_P_P                                TRACE_ARG_SIZE(4,2,0,0,0)
#define FMT__D_A_D                                      TRACE_ARG_SIZE(0,2,0,0,1)
#define FMT__D_H_D_D_D_D                                TRACE_ARG_SIZE(1,5,0,0,0)
#define FMT__H_D_D_D_H_H                                TRACE_ARG_SIZE(3,3,0,0,0)
#define FMT__P_H_D_D_D_H_H                              TRACE_ARG_SIZE(3,3,1,0,0)
#define FMT__H_H_H_D_H_L_H_H                            TRACE_ARG_SIZE(6,1,1,0,0)
#define FMT__D_D_P_P                                    TRACE_ARG_SIZE(0,2,0,2,0)
#define FMT__D_D_P_P_D_D                                TRACE_ARG_SIZE(0,4,0,2,0)
#define FMT__H_P_P_H_H                                  TRACE_ARG_SIZE(3,0,0,2,0)
#define FMT__H_P_H_H_P                                  TRACE_ARG_SIZE(3,0,0,2,0)
#define FMT__H_P_P                                      TRACE_ARG_SIZE(1,0,0,2,0)
#define FMT__H_P_P_H                                    TRACE_ARG_SIZE(2,0,0,2,0)
#define FMT__D_P_D_D_P                                  TRACE_ARG_SIZE(0,3,0,2,0)
#define FMT__D_D_D_A_A                                  TRACE_ARG_SIZE(0,3,0,0,2)
#define FMT__H_D_A_D_D_D                                TRACE_ARG_SIZE(1,4,0,0,1)
#define FMT__H_D_A_P                                    TRACE_ARG_SIZE(1,1,0,1,1)
#define FMT__H_H_H_H_H_H_H_H_H_H                        TRACE_ARG_SIZE(10,0,0,0,0)

/** @} */ /* TRACE_DATA_FORMAT */

/**
 *  @addtogroup TRACE_FIRST_ARG
 *  @{
 */
#define TRACE_COMMON1 TRACE_SUBSYSTEM_COMMON, 1U
#define TRACE_COMMON2 TRACE_SUBSYSTEM_COMMON, 2U
#define TRACE_COMMON3 TRACE_SUBSYSTEM_COMMON, 3U
#define TRACE_COMMON4 TRACE_SUBSYSTEM_COMMON, 4U

/* osif subsystem is nearly not used. Place it to the same with common and free
 * 1 bit for buffers. */
#define TRACE_OSIF1 TRACE_SUBSYSTEM_COMMON, 1U
#define TRACE_OSIF2 TRACE_SUBSYSTEM_COMMON, 2U
#define TRACE_OSIF3 TRACE_SUBSYSTEM_COMMON, 3U
#define TRACE_OSIF4 TRACE_SUBSYSTEM_COMMON, 4U

#define TRACE_MAC1 TRACE_SUBSYSTEM_MAC, 1U
#define TRACE_MAC2 TRACE_SUBSYSTEM_MAC, 2U
#define TRACE_MAC3 TRACE_SUBSYSTEM_MAC, 3U
#define TRACE_MAC4 TRACE_SUBSYSTEM_MAC, 4U

#define TRACE_MACSPLIT1 TRACE_SUBSYSTEM_MACSPLIT, 1U
#define TRACE_MACSPLIT2 TRACE_SUBSYSTEM_MACSPLIT, 2U
#define TRACE_MACSPLIT3 TRACE_SUBSYSTEM_MACSPLIT, 3U
#define TRACE_MACSPLIT4 TRACE_SUBSYSTEM_MACSPLIT, 4U

#define TRACE_MACLL1 TRACE_SUBSYSTEM_MACLL, 1U
#define TRACE_MACLL2 TRACE_SUBSYSTEM_MACLL, 2U
#define TRACE_MACLL3 TRACE_SUBSYSTEM_MACLL, 3U
#define TRACE_MACLL4 TRACE_SUBSYSTEM_MACLL, 4U

#define TRACE_NWK1 TRACE_SUBSYSTEM_NWK, 1U
#define TRACE_NWK2 TRACE_SUBSYSTEM_NWK, 2U
#define TRACE_NWK3 TRACE_SUBSYSTEM_NWK, 3U
#define TRACE_NWK4 TRACE_SUBSYSTEM_NWK, 4U

#define TRACE_APS1 TRACE_SUBSYSTEM_APS, 1U
#define TRACE_APS2 TRACE_SUBSYSTEM_APS, 2U
#define TRACE_APS3 TRACE_SUBSYSTEM_APS, 3U
#define TRACE_APS4 TRACE_SUBSYSTEM_APS, 4U

#define TRACE_AF1 TRACE_SUBSYSTEM_AF, 1U
#define TRACE_AF2 TRACE_SUBSYSTEM_AF, 2U
#define TRACE_AF3 TRACE_SUBSYSTEM_AF, 3U
#define TRACE_AF4 TRACE_SUBSYSTEM_AF, 4U

#define TRACE_ZDO1 TRACE_SUBSYSTEM_ZDO, 1U
#define TRACE_ZDO2 TRACE_SUBSYSTEM_ZDO, 2U
#define TRACE_ZDO3 TRACE_SUBSYSTEM_ZDO, 3U
#define TRACE_ZDO4 TRACE_SUBSYSTEM_ZDO, 4U

#define TRACE_SECUR1 TRACE_SUBSYSTEM_SECUR, 1U
#define TRACE_SECUR2 TRACE_SUBSYSTEM_SECUR, 2U
#define TRACE_SECUR3 TRACE_SUBSYSTEM_SECUR, 3U
#define TRACE_SECUR4 TRACE_SUBSYSTEM_SECUR, 4U

#define TRACE_ZCL1 TRACE_SUBSYSTEM_ZCL, 1U
#define TRACE_ZCL2 TRACE_SUBSYSTEM_ZCL, 2U
#define TRACE_ZCL3 TRACE_SUBSYSTEM_ZCL, 3U
#define TRACE_ZCL4 TRACE_SUBSYSTEM_ZCL, 4U

#define TRACE_ZLL1 TRACE_SUBSYSTEM_ZLL, 1U
#define TRACE_ZLL2 TRACE_SUBSYSTEM_ZLL, 2U
#define TRACE_ZLL3 TRACE_SUBSYSTEM_ZLL, 3U
#define TRACE_ZLL4 TRACE_SUBSYSTEM_ZLL, 4U

#define TRACE_ZGP1 TRACE_SUBSYSTEM_ZGP, 1U
#define TRACE_ZGP2 TRACE_SUBSYSTEM_ZGP, 2U
#define TRACE_ZGP3 TRACE_SUBSYSTEM_ZGP, 3U
#define TRACE_ZGP4 TRACE_SUBSYSTEM_ZGP, 4U

#define TRACE_ZSE1 TRACE_SUBSYSTEM_ZSE, 1U
#define TRACE_ZSE2 TRACE_SUBSYSTEM_ZSE, 2U
#define TRACE_ZSE3 TRACE_SUBSYSTEM_ZSE, 3U
#define TRACE_ZSE4 TRACE_SUBSYSTEM_ZSE, 4U

#ifndef DOXYGEN
#define TRACE_SPI1 TRACE_SUBSYSTEM_SPI, 1U
#define TRACE_SPI2 TRACE_SUBSYSTEM_SPI, 2U
#define TRACE_SPI3 TRACE_SUBSYSTEM_SPI, 3U
#define TRACE_SPI4 TRACE_SUBSYSTEM_SPI, 4U

#define TRACE_SSL1 TRACE_SUBSYSTEM_SSL, 1U
#define TRACE_SSL2 TRACE_SUBSYSTEM_SSL, 2U
#define TRACE_SSL3 TRACE_SUBSYSTEM_SSL, 3U
#define TRACE_SSL4 TRACE_SUBSYSTEM_SSL, 4U
#endif /* DOXYGEN */

#define TRACE_APP1 TRACE_SUBSYSTEM_APP, 1U
#define TRACE_APP2 TRACE_SUBSYSTEM_APP, 2U
#define TRACE_APP3 TRACE_SUBSYSTEM_APP, 3U
#define TRACE_APP4 TRACE_SUBSYSTEM_APP, 4U

#ifndef DOXYGEN
#define TRACE_SPECIAL1 TRACE_SUBSYSTEM_SPECIAL1, 1U
#define TRACE_SPECIAL2 TRACE_SUBSYSTEM_SPECIAL1, 2U
#define TRACE_SPECIAL3 TRACE_SUBSYSTEM_SPECIAL1, 3U
#define TRACE_SPECIAL4 TRACE_SUBSYSTEM_SPECIAL1, 4U

#define TRACE_USB1 TRACE_SUBSYSTEM_USB, 1U
#define TRACE_USB2 TRACE_SUBSYSTEM_USB, 2U
#define TRACE_USB3 TRACE_SUBSYSTEM_USB, 3U
#define TRACE_USB4 TRACE_SUBSYSTEM_USB, 4U

#define TRACE_MEMDBG1 TRACE_SUBSYSTEM_MEM, 1U
#define TRACE_MEMDBG2 TRACE_SUBSYSTEM_MEM, 2U
#define TRACE_MEMDBG3 TRACE_SUBSYSTEM_MEM, 3U
#define TRACE_MEMDBG4 TRACE_SUBSYSTEM_MEM, 4U

#define TRACE_CLOUD1 TRACE_SUBSYSTEM_CLOUD, 1U
#define TRACE_CLOUD2 TRACE_SUBSYSTEM_CLOUD, 2U
#define TRACE_CLOUD3 TRACE_SUBSYSTEM_CLOUD, 3U
#define TRACE_CLOUD4 TRACE_SUBSYSTEM_CLOUD, 4U

#define TRACE_HTTP1 TRACE_SUBSYSTEM_HTTP, 1U
#define TRACE_HTTP2 TRACE_SUBSYSTEM_HTTP, 2U
#define TRACE_HTTP3 TRACE_SUBSYSTEM_HTTP, 3U
#define TRACE_HTTP4 TRACE_SUBSYSTEM_HTTP, 4U

#define TRACE_JSON1 TRACE_SUBSYSTEM_JSON, 1U
#define TRACE_JSON2 TRACE_SUBSYSTEM_JSON, 2U
#define TRACE_JSON3 TRACE_SUBSYSTEM_JSON, 3U
#define TRACE_JSON4 TRACE_SUBSYSTEM_JSON, 4U

#define TRACE_LWIP1 TRACE_SUBSYSTEM_LWIP, 1U
#define TRACE_LWIP2 TRACE_SUBSYSTEM_LWIP, 2U
#define TRACE_LWIP3 TRACE_SUBSYSTEM_LWIP, 3U
#define TRACE_LWIP4 TRACE_SUBSYSTEM_LWIP, 4U

#define TRACE_VENSTAR1 TRACE_SUBSYSTEM_VENSTAR, 1U
#define TRACE_VENSTAR2 TRACE_SUBSYSTEM_VENSTAR, 2U
#define TRACE_VENSTAR3 TRACE_SUBSYSTEM_VENSTAR, 3U
#define TRACE_VENSTAR4 TRACE_SUBSYSTEM_VENSTAR, 4U

#define TRACE_TRANSPORT1 TRACE_SUBSYSTEM_TRANSPORT, 1U
#define TRACE_TRANSPORT2 TRACE_SUBSYSTEM_TRANSPORT, 2U
#define TRACE_TRANSPORT3 TRACE_SUBSYSTEM_TRANSPORT, 3U
#define TRACE_TRANSPORT4 TRACE_SUBSYSTEM_TRANSPORT, 4U

#define TRACE_UART1 TRACE_SUBSYSTEM_UART, 1U
#define TRACE_UART2 TRACE_SUBSYSTEM_UART, 2U
#define TRACE_UART3 TRACE_SUBSYSTEM_UART, 3U
#define TRACE_UART4 TRACE_SUBSYSTEM_UART, 4U

#define TRACE_BATTERY1 TRACE_SUBSYSTEM_BATTERY, 1U
#define TRACE_BATTERY2 TRACE_SUBSYSTEM_BATTERY, 2U
#define TRACE_BATTERY3 TRACE_SUBSYSTEM_BATTERY, 3U
#define TRACE_BATTERY4 TRACE_SUBSYSTEM_BATTERY, 4U

#define TRACE_OTA1 TRACE_SUBSYSTEM_OTA, 1U
#define TRACE_OTA2 TRACE_SUBSYSTEM_OTA, 2U
#define TRACE_OTA3 TRACE_SUBSYSTEM_OTA, 3U
#define TRACE_OTA4 TRACE_SUBSYSTEM_OTA, 4U

#define TRACE_MAC_API1 TRACE_SUBSYSTEM_MAC_API, 1U
#define TRACE_MAC_API2 TRACE_SUBSYSTEM_MAC_API, 2U
#define TRACE_MAC_API3 TRACE_SUBSYSTEM_MAC_API, 3U
#define TRACE_MAC_API4 TRACE_SUBSYSTEM_MAC_API, 4U

#define TRACE_ZBDIRECT1 TRACE_SUBSYSTEM_ZBDIRECT, 1U
#define TRACE_ZBDIRECT2 TRACE_SUBSYSTEM_ZBDIRECT, 2U
#define TRACE_ZBDIRECT3 TRACE_SUBSYSTEM_ZBDIRECT, 3U
#define TRACE_ZBDIRECT4 TRACE_SUBSYSTEM_ZBDIRECT, 4U

#define TRACE_DIAG1 TRACE_SUBSYSTEM_DIAGNOSTIC, 1U
#define TRACE_DIAG2 TRACE_SUBSYSTEM_DIAGNOSTIC, 2U
#define TRACE_DIAG3 TRACE_SUBSYSTEM_DIAGNOSTIC, 3U
#define TRACE_DIAG4 TRACE_SUBSYSTEM_DIAGNOSTIC, 4U

#define TRACE_NS1 TRACE_SUBSYSTEM_NS, 1U
#define TRACE_NS2 TRACE_SUBSYSTEM_NS, 2U
#define TRACE_NS3 TRACE_SUBSYSTEM_NS, 3U
#define TRACE_NS4 TRACE_SUBSYSTEM_NS, 4U

#define TRACE_TEST1 TRACE_SUBSYSTEM_TEST, 1U
#define TRACE_TEST2 TRACE_SUBSYSTEM_TEST, 2U
#define TRACE_TEST3 TRACE_SUBSYSTEM_TEST, 3U
#define TRACE_TEST4 TRACE_SUBSYSTEM_TEST, 4U
#endif /* DOXYGEN */

#ifndef ZB_SET_TRACE_LEVEL

/* empty definitions if not implemented */
#define ZB_SET_TRACE_LEVEL(l)
#define ZB_SET_TRACE_MASK(m)
#define ZB_SET_TRACE_OFF()
#define ZB_SET_TRACE_ON()

#endif

/** @} */ /* TRACE_FIRST_ARG */
/** @endcond */ /* DSR_TRACE */
/** @} */ /* Debug trace */

#if defined ZB_TRAFFIC_DUMP_ON || defined DOXYGEN
/**
   @addtogroup DUMP_ON_OFF
   @{
*/
/** @cond DOXYGEN_INTERNAL_DOC */
extern zb_uint8_t g_traf_dump;
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

static ZB_INLINE zb_uint8_t zb_get_traf_dump_state(void)
{
  return g_traf_dump;
}

/**
 * Switch Zigbee traffic dump OFF
 */
#define ZB_SET_TRAF_DUMP_OFF() g_traf_dump = 0U
/**
 * Switch Zigbee traffic dump ON
 *
 * That function works only if traffic dump is enabled at compile time.
 */
#define ZB_SET_TRAF_DUMP_ON() g_traf_dump = 1U
#define ZB_GET_TRAF_DUMP_STATE()  zb_get_traf_dump_state()
/** @} */ /* DUMP_ON_OFF */
#else
#define ZB_SET_TRAF_DUMP_OFF()
#define ZB_SET_TRAF_DUMP_ON()
#define ZB_GET_TRAF_DUMP_STATE()  0U
#endif

/** @endcond */ /* DOXYGEN_DEBUG_SECTION */

#endif /* ZB_LOGGER_H */
