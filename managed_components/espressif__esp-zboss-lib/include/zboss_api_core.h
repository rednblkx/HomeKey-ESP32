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
/*  PURPOSE: ZBOSS core API header. Time, scheduler and memory buffers API.
*/

#ifndef ZB_ZBOSS_API_CORE_H
#define ZB_ZBOSS_API_CORE_H 1

#include "zb_vendor.h"
#include "zb_config.h"
#include "zb_types.h"
#include "zb_errors.h"

/* zb_callback_t is used in osif if we have serial API */
/*! \addtogroup sched */
/*! @{ */

/**
 *   Callback function typedef.
 *   Callback is function planned to execute by another function.
 *
 *   @param param - callback parameter - usually, but not always, ref to packet buf
 *
 */
typedef void (ZB_CODE * zb_callback_t)(zb_uint8_t param);

/**
 *   Callback function with 2 parameters typedef.
 *   Callback is function planned to execute by another function.
 *
 *   @param param - callback parameter - usually, but not always, ref to packet buf
 *   @param cb_param - additional 2-byte callback parameter, user data.
 *
 */
typedef void (ZB_CODE * zb_callback2_t)(zb_uint8_t param, zb_uint16_t cb_param);
/*! @} */


/*! \addtogroup time */
/*! @{ */

/**
   \par Timer functionality.

   The idea is: platform has some timer which can be stopped or run.
   When run, it increments (or decrements - depends on platform) some counter
   until counter overflow (underflow), then issues interrupt - wakeups main loop
   if it sleeping.
   Time stored in ticks; time resolution is platform dependent, its usual value
   is 15.36 usec - 1 beacon interval.
   @note Time type has limited capacity (usually 16 bits) and can overflow.
   Macros which works with time handles overflow. It is supposed that time values will
   not differ to more then 1/2 of the maximum time value.

   All that timer macros will not be used directly by the application code - it
   is scheduler internals. The only API for timer is ZB_SCHEDULE_ALARM() call.
 */


/**
   Timer type.
 */

typedef zb_uint64_t zb_time_t;
/**
 * Maximum timer value, if 64-bit timer is used.
 */
#define ZB_MAX_TIME_VAL ZB_UINT64_MAX
/**
 * Minimum timer value, if 64-bit timer is used.
 */
#define ZB_MIN_TIME_VAL ZB_UINT64_MIN

/**
 * A half of defined maximum timer value.
 */
#define ZB_HALF_MAX_TIME_VAL (ZB_MAX_TIME_VAL / 2U)

/** @cond internals_doc */
/**
 * @copydoc ZB_TIMER_GET()
 *
 * @note Don't call directly, use @ref ZB_TIMER_GET() instead
 */
zb_time_t zb_timer_get(void);
/** @endcond */ /* internals_doc */

/**
 * @return Get current timer value (system timer intervals)
 */
#define ZB_TIMER_GET() (zb_timer_get())

/**
   Time subtraction: subtract 'b' from 'a'

   Take overflow into account: change sign (subtraction order) if result >
   values_diapason/2.
   Suppose a always >= b, so result is never negative.
   This macro will be used to calculate, for example, amount of time to sleep
   - it is positive by definition.
   Do not use it to compare time values! Use ZB_TIME_GE() instead.
   @note Both a and b is of type @ref zb_time_t. Can't decrease time (subtract
   constant from it) using this macro.

   @param a - time to subtract from
   @param b - time to subtract
   @return subtraction result
 */
#define ZB_TIME_SUBTRACT(a, b) ((zb_time_t)((a) - (b)) < ZB_HALF_MAX_TIME_VAL ? (zb_time_t)((a) - (b)) : (zb_time_t)((b) - (a)))

/**
   Time add: add 'a' to 'b'

   Overflow is possible, but this is ok - it handled by subtraction and compare macros.

   @param a - time to add to
   @param b - value to add
   @return addition result
 */
#define ZB_TIME_ADD(a, b) (zb_time_t)((a) + (b))

/**
   Compare times a and b - check that a >= b

   Taking into account overflow and unsigned values arithmetic and supposing
   difference between a and b can't be > 1/2 of the overall time values
   diapason,
   a >= b only if a - b < values_diapason/2

   @param a - first time value to compare
   @param b - second time value to compare
   @return 1 is a >= b, 0 otherwise
 */
#define ZB_TIME_GE(a, b) ((zb_time_t)((a) - (b)) < ZB_HALF_MAX_TIME_VAL)


/**
 One second timeout
*/
#define ZB_TIME_ONE_SECOND ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(1000U)

/**
  Convert time from milliseconds to system timer intervals. Round the result up.
*/
#define ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL_CEIL(ms) (((zb_time_t)(ms) * 1000U))

/**
  Convert time from milliseconds to system timer intervals. Round the result down.
*/
#define ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL_FLOOR(ms) ((zb_time_t)(ms) * 1000U)

/**
  Convert time from milliseconds to system timer intervals.
*/
#define ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(ms) ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL_CEIL(ms)

/**
 * Beacon interval in microseconds
 *
 * 1 beacon interval = aBaseSuperframeDuration * symbol duration
 *
 * 1 symbol = 16e-6 sec (mac spec 6.5.3.2 Symbol rate) for 2.4GHz ZB.
 * 1 beacon interval = 15.360 ms.
 */
#define ZB_BEACON_INTERVAL (ZB_SYMBOL_DURATION_USEC * ZB_ABASE_SUPERFRAME_DURATION)

#define ZB_SYS_TIMER_INTERVAL_TO_MSEC(t) ((zb_time_t)(t) / 1000U)

/**
  Convert time from system timer intervals to microseconds

*/
#define ZB_SYS_TIMER_INTERVAL_TO_USEC(t) ((zb_time_t)(t))

/**
 Quarterseconds timeout
*/
#define ZB_SYS_TIMER_QUARTERECONDS(n)  (ZB_SYS_TIMER_INTERVAL_TO_MSEC((n)) / 250U)

/**
 Convert from msec to quarterseconds
*/
#define ZB_MSEC_TO_QUARTERECONDS(n)  ((n) / 250U)

/**
 Convert from quarterseconds to msec
*/
#define ZB_QUARTERECONDS_TO_MSEC(n)  250UL*(n)

/**
 Convert from quarterseconds to system timer interval
*/
#define ZB_QUARTERECONDS_TO_SYS_TIMER_INTERVAL(qsec) ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(250U * (qsec))

/**
 * Convert from seconds to milliseconds
 */
#define ZB_SECONDS_TO_MILLISECONDS(_s) (1000ul*(_s))

/**
 Convert from seconds to system timer interval

 This macro works correctly on 64-bit platform.
 The calculation was not tested on 16-bit platforms.
*/
#define ZB_SECONDS_TO_SYS_TIMER_INTERVAL(_s) ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(1000UL * (_s))

/**
 Convert from milliseconds to microseconds
*/
#define ZB_MILLISECONDS_TO_USEC(ms) ((ms) * (1000u))

/**
 Convert from microseconds to milliseconds
*/
#define ZB_USECS_TO_MILLISECONDS(usec) ((usec) / (1000u))

/*! @} */

#include "zb_osif.h"
#include "zb_debug.h"
#include "zb_trace.h"

#include "zb_pooled_list.h"

/*! \addtogroup sched */
/*! @{ */

/** @cond internals_doc */
/** Schedule single-param callback execution.
    (use ZB_SCHEDULE_APP_CALLBACK() macro instead of this function).

    Schedule execution of function `func' in the main scheduler loop.

    @param func - function to execute
    @param param - callback parameter - usually, but not always ref to packet buffer

    @return RET_OK or error code.

*/
zb_ret_t zb_schedule_app_callback(zb_callback_t func, zb_uint8_t param);
/** @endcond */ /* internals_doc */
/**
   Schedule single-param callback execution.

   Schedule execution of function `func' in the main scheduler loop.

   @param func - function to execute
   @param param - callback parameter - usually, but not always ref to packet buffer

   @return RET_OK or RET_OVERFLOW.

 */
#ifndef ZB_SCHEDULE_APP_CALLBACK
#define ZB_SCHEDULE_APP_CALLBACK(func, param) zb_schedule_app_callback(func, param)
#endif /* ZB_SCHEDULE_APP_CALLBACK */

/** @cond internals_doc */
/** Schedule two-param callback execution.
    (use ZB_SCHEDULE_APP_CALLBACK2() macro instead of this function).

    Schedule execution of function `func' in the main scheduler loop.

    @param func - function to execute
    @param param - callback parameter - usually, but not always ref to packet buffer
    @param user_param - zb_uint16_t user parameter - usually, but not always short address

    @return RET_OK or error code.
*/
zb_ret_t zb_schedule_app_callback2(zb_callback2_t func, zb_uint8_t param, zb_uint16_t user_param);
/** @endcond */ /* internals_doc */
/**
   Schedule two-param callback execution.
   Schedule execution of function `func' in the main scheduler loop.

   @param func - function to execute
   @param param - zb_uint8_t callback parameter - usually, but not always ref to
   packet buffer
   @param user_param - zb_uint16_t user parameter - usually, but not always short address

   @return RET_OK or RET_OVERFLOW.
 */
#ifndef ZB_SCHEDULE_APP_CALLBACK2
#define ZB_SCHEDULE_APP_CALLBACK2(func, param, user_param) zb_schedule_app_callback2(func, param, user_param)
#endif /* ZB_SCHEDULE_APP_CALLBACK2 */

/** @cond internals_doc */
zb_ret_t zb_schedule_app_alarm(zb_callback_t func, zb_uint8_t param, zb_time_t run_after);
/** @endcond */ /* internals_doc */

/**
   Schedule alarm - callback to be executed after timeout.

   Function will be called via scheduler after timeout expired (maybe, plus some
   additional time).
   Timer resolution depends on implementation.
   Same callback can be scheduled for execution more then once.

   @param func - function to call via scheduler
   @param param - parameter to pass to the function
   @param timer_interval - timeout, in system timer intervals
   @return RET_OK or RET_OVERFLOW

 */
#ifndef ZB_SCHEDULE_APP_ALARM
#define ZB_SCHEDULE_APP_ALARM(func, param, timer_interval) zb_schedule_app_alarm(func, param, timer_interval)
#endif /* ZB_SCHEDULE_APP_ALARM */

/**
   Special parameter for zb_schedule_alarm_cancel(): cancel alarm once without
   parameter check

   Cancel only one alarm without check for parameter
 */
#define ZB_ALARM_ANY_PARAM (zb_uint8_t)(-1)

/**
   Special parameter for zb_schedule_alarm_cancel(): cancel alarm for all
   parameters
 */
#define ZB_ALARM_ALL_CB (zb_uint8_t)(-2)
/** @cond internals_doc */
/**
   Cancel scheduled alarm (use macro ZB_SCHEDULE_APP_ALARM_CANCEL()
   instead of this function).

   This function cancel previously scheduled alarm. Function is identified by
   the pointer.

   @param func - function to cancel
   @param param - parameter to cancel. \see ZB_ALARM_ANY_PARAM. \see ZB_ALARM_ALL_CB
   @param p_param - [out] pointer of ref buffer from cancelled flag: free buffer if its alarm will be cancel
   @return RET_OK or error code

 */
zb_ret_t zb_schedule_alarm_cancel(zb_callback_t func, zb_uint8_t param, zb_uint8_t *p_param);
/** @endcond */ /* internals_doc */
/**
   Cancel scheduled alarm.

   This function cancel previously scheduled alarm. Function is identified by
   the pointer.

   @param func - function to cancel
   @param param - parameter to cancel. \see ZB_ALARM_ANY_PARAM. \see ZB_ALARM_ALL_CB
   @return RET_OK or RET_OVERFLOW

 */
#ifndef ZB_SCHEDULE_APP_ALARM_CANCEL
#define ZB_SCHEDULE_APP_ALARM_CANCEL(func, param) zb_schedule_alarm_cancel((func), (param), NULL)
#endif /* ZB_SCHEDULE_APP_ALARM_CANCEL */


/** @cond internals_doc */
/**
   Get Schedule alarm time - seek alarm and return alarm time

   @param func - function to call via scheduler
   @param param - parameter to pass to the function
   @param timer_interval - pointer on alarm timeout, in system timer intervals
   @return RET_OK or error code
 */
zb_ret_t zb_schedule_get_alarm_time(zb_callback_t func, zb_uint8_t param, zb_time_t *timer_interval);
/** @endcond */ /* internals_doc */

/**
   Get Schedule alarm time - seek alarm and return alarm time

   @param func - function to call via scheduler
   @param param - parameter to pass to the function
   @param timer_interval - pointer on alarm timeout, in system timer intervals
   @return RET_OK or error code
*/
#define ZB_SCHEDULE_GET_ALARM_TIME(func, param, timer_interval) zb_schedule_get_alarm_time(func, param, timer_interval)

/** @cond internals_doc */
/**
   Is scheduler stop - Is scheduler running now

   @return ZB_TRUE in case of scheduler is stopped or ZB_FALSE otherwise
 */
zb_bool_t zb_scheduler_is_stop(void);
/** @endcond */ /* internals_doc */

/**
   Is scheduler stop - Is scheduler running now

   @return ZB_TRUE in case of scheduler is stopped or ZB_FALSE otherwise
*/
#define ZB_SCHEDULER_IS_STOP() zb_scheduler_is_stop()

/**
   Is scheduler is going to stop/stopped - Is scheduler running now

   @return ZB_TRUE in case of scheduler is going to stop/stopped or ZB_FALSE otherwise
 */
zb_bool_t zb_scheduler_is_going_to_stop(void);
/** @endcond */ /* internals_doc */

/**
   Is scheduler is going to stop/stopped - Is scheduler running now

   @return ZB_TRUE in case of scheduler is going to stop/stopped or ZB_FALSE otherwise
*/
#define ZB_SCHEDULER_IS_GOING_TO_STOP() zb_scheduler_is_going_to_stop()

/*! @} */


/*! \addtogroup rng */
/*! @{ */

/**
 * Maximal possible value for randomly generated (32-bit) value
 */
#define ZB_RAND_MAX ((zb_uint32_t)~0U)

/**
 * Generate random 32-bit value using zb_random_val() with ZB_UINT32_MAX
 *
 * @return random value between 0 to 2^32-1
 */
zb_uint32_t zb_random(void);

/**
 * Generate random value between 0 to max_value, inclusively.
 *
 * Internal software algorithm using LFSRs
 * can be overridden by define ZB_RANDOM_HARDWARE
 *
 * @return random value between 0 and 'max_value' (32 bits).
 */
zb_uint32_t zb_random_val(zb_uint32_t max_value);

/**
 * Generate random 8-bit value using zb_random_val() with ZB_UINT8_MAX
 *
 * @return random value between 0 to 0xff
 */
zb_uint8_t zb_random_u8(void);

/**
 * Generate random 16-bit value using zb_random_val() with ZB_UINT16_MAX
 *
 * @return random value between 0 to 0xffff
 */
zb_uint16_t zb_random_u16(void);

/**
 * Equivalent of zb_random_val(). The macro is left for compatibility reasons.
 */
#define ZB_RANDOM_VALUE(max_value) zb_random_val(max_value)

/**
 * Equivalent of zb_random_u8(). The macro is left for compatibility reasons.
 */
#define ZB_RANDOM_U8() zb_random_u8()

/**
 * Equivalent of zb_random_u16(). The macro is left for compatibility reasons.
 */
#define ZB_RANDOM_U16() zb_random_u16()

/**
 * Analogue of bzero() for volatile data.
 *
 * A custom version should be implemented because there is no standard library function for that
 * purpose. Also, this function is not platform-dependent in contrast to ZB_BZERO() macro and
 * therefore can't be overridden by specific ZBOSS platform.
 *
 * The function sets individually every byte of provided memory region to zero.
 */
void zb_bzero_volatile(volatile void *s, zb_uint_t size);

/** @brief Fill in memory with PRBS9 pattern using linear-feedback shift registers.
    @param dest - Pointer to the block of memory to fill.
    @param cnt - Number of bytes to be set.
    @param seed - Random seed
 */
void zb_generate_prbs9(zb_uint8_t *dest, zb_uint16_t cnt, zb_uint16_t seed);

/*! @} */

/** @cond internals_doc */
/**
   Copy 8 byte array (i.e. long address).
 */
void zb_memcpy8(void *vptr, void *vsrc);
/** @endcond */ /* internals_doc */

/**
  Return absolute value
*/
#define ZB_ABS(a) ((a) < 0 ? -(a) : (a))

/**
 * Set of ZB_BITSx() macros return value with bits set in provided positions.
 * ZB_BITS1(0)     = 1
 * ZB_BITS2(2,3)   = 12
 * ZB_BITS3(0,1,2) = 7
 */
#define ZB_BITS1(_b) (1UL << (_b))
#define ZB_BITS2(_b1, _b2) ((1UL << (_b1)) | (1UL << (_b2)))
#define ZB_BITS3(_b1, _b2, _b3) ((1UL << (_b1)) | (1UL << (_b2)) | (1UL << (_b3)))
#define ZB_BITS4(_b1, _b2, _b3, _b4) ((1UL << (_b1)) | (1UL << (_b2)) | (1UL << (_b3)) | (1UL << (_b4)))
#define ZB_BITS5(_b1, _b2, _b3, _b4, _b5) ((1UL << (_b1)) | (1UL << (_b2)) | (1UL << (_b3)) | \
                                           (1UL << (_b4)) | (1UL << (_b5)))

#define ZB_SET_BIT_IN_BIT_VECTOR(vector, nbit) ( (vector)[ (nbit) / 8U ] |= ( 1U << ( (nbit) % 8U )) )
#define ZB_CLR_BIT_IN_BIT_VECTOR(vector, nbit) ( (vector)[ (nbit) / 8U ] &= ~( 1U << ( (nbit) % 8U )) )
#define ZB_CHECK_BIT_IN_BIT_VECTOR(vector, nbit) (ZB_U2B( (vector)[ (nbit) / 8U ] & ( 1U << ( (nbit) % 8U )) ))
#define ZB_SIZE_OF_BIT_VECTOR(bit_count) (((bit_count) + 7U) / 8U)

/**
 * Checks if the bits specified by 'bitmask' are set in the 'val'. Bit-mask may contain one or
 * several bits set.
 *
 * @return ZB_TRUE if the 'bitmask' is set in 'val'. ZB_FALSE otherwise.
 */
#define ZB_BIT_IS_SET(val, bitmask) (((val) & (bitmask)) != 0U)

#endif /* ZB_ZBOSS_API_CORE_H */
