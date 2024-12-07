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
/* PURPOSE: Common definitions for time functionality
*/

#ifndef ZB_TIME_H
#define ZB_TIME_H 1

#include "zboss_api_core.h"


/*! @cond internals_doc */
/*! \addtogroup time */
/*! @{ */

#define ZB_HALF_MAX_TIME32_VAL (ZB_UINT32_MAX / 2U)

  /**
     Time subtraction: subtract 'b' from 'a'for 32bit-time

     Take overflow into account: change sign (subtraction order) if result >
     values_diapason/2.
     Suppose a always >= b, so result is never negative.
     This macro will be used to calculate, for example, amount of time to sleep
     - it is positive by definition.
     Do not use it to compare time values! Use ZB_TIME32_GE() instead.
     @note Both a and b is of type @ref zb_time_t. Can't decrease time (subtract
     constant from it) using this macro.

     @param a - time to subtract from
     @param b - time to subtract
     @return subtraction result
   */
#define ZB_TIME32_SUBTRACT(a, b) ((zb_uint32_t)((a) - (b)) < ZB_HALF_MAX_TIME32_VAL ? (zb_uint32_t)((a) - (b)) : (zb_uint32_t)((b) - (a)))

  /**
     Time add: add 'a' to 'b' for 32-bit time

     Overflow is possible, but this is ok - it handled by subtraction and compare macros.

     @param a - time to add to
     @param b - value to add
     @return addition result
   */
#define ZB_TIME32_ADD(a, b) (zb_uint32_t)((a) + (b))

  /**
     Compare times a and b - check that a >= b for 32-bit time

     Taking into account overflow and unsigned values arithmetic and supposing
     difference between a and b can't be > 1/2 of the overall time values
     diapason,
     a >= b only if a - b < values_diapason/2

     @param a - first time value to compare
     @param b - second time value to compare
     @return 1 is a >= b, 0 otherwise
   */
#define ZB_TIME32_GE(a, b) ((zb_uint32_t)((a) - (b)) < ZB_HALF_MAX_TIME32_VAL)

#ifndef ZB_ALIEN_TIMER
/**
   Start timer - assign time to sleep

   @param timeout - time in internal format to sleep before delayed callback run
 */
void zb_timer_start(zb_time_t timeout);

/**
   Stop timer - timer stop and callback not run
 */
void zb_timer_stop(void);

#endif

/**
   Timer internals

   'timer' always ticks (if some timer is started), usually with overflow.
   Timer unit is beacon interval, for every system.
 */
typedef struct zb_timer_s
{
  zb_time_t timer; /*!< current time, measured in beacon intervals */

#ifndef ZB_ALIEN_TIMER
  zb_uint32_t sleep_tmo_remainder_ms; /*!< slept time remainder, used for timer corrections */
  zb_time_t   timer_stop; /*!< time to stop timer (disable timer interrupts etc) */
  zb_bool_t   started;  /*!< flag - timer is started */
  zb_bool_t   canstop;  /*!< flag - Can the timer be stopped? */
#endif
}
zb_timer_t;

/**
   Enable ability to stop times saving power.

   Might be used for ZED. Not sure will use it for now.
 */
void zb_timer_enable_stop(void);

/**
   Disable ability to stop times saving power.
 */
void zb_timer_disable_stop(void);

/**
   Get current transceiver time value in usec
 */
zb_time_t osif_transceiver_time_get(void);

/**
   Sleep for required interval using transceiver timer.

   Can be implemented as a busy wait or as a real sleep.

   @param interval - time to sleep in microseconds
 */
void osif_sleep_using_transc_timer(zb_time_t interval);


/*! @} */
/*! @endcond */

#endif /* ZB_TIME_H */
