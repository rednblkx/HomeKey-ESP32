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
/* PURPOSE: Zigbee scheduler: cooperative multitasking.
*/

#ifndef ZB_SCHEDULER_H
#define ZB_SCHEDULER_H 1
#include "zboss_api_core.h"
#include "zb_osif.h"
#include "zb_pooled_list.h"
#include "zboss_api_internal.h" /* zb_cb_q_ent_t */

/*! \addtogroup sched */
/*! @{ */

#include "zb_time.h"
#include "zb_ringbuffer.h"

/*! @cond internals_doc */
#define ZB_CB_SCHEDULER_EVENT() ZB_OSIF_SCHEDULER_EVENT()

#ifdef ZB_INTERRUPT_SAFE_ALARMS
#define ZB_ALARM_INT_DISABLE() ZB_OSIF_GLOBAL_LOCK()
#define ZB_ALARM_INT_ENABLE() ZB_OSIF_GLOBAL_UNLOCK()
#else /* ZB_INTERRUPT_SAFE_ALARMS */
#define ZB_ALARM_INT_DISABLE()
#define ZB_ALARM_INT_ENABLE()
#endif /* ZB_INTERRUPT_SAFE_ALARMS */

#ifdef ZB_INTERRUPT_SAFE_CALLBACKS
#define ZB_CB_INT_DISABLE() ZB_OSIF_GLOBAL_LOCK()
#define ZB_CB_INT_ENABLE() ZB_OSIF_GLOBAL_UNLOCK()
#else /* ZB_INTERRUPT_SAFE_CALLBACKS */
#define ZB_CB_INT_DISABLE()
#define ZB_CB_INT_ENABLE()
#endif /* ZB_INTERRUPT_SAFE_CALLBACKS */

/* When running in multithreaded environment, is it possible
when a callback is scheduled from another thread. 
The scheduler itself if thread-safe, so, this is possible. 
However, if scheduler is sleeping in a main ZBOSS thread now, 
it should be signalled somehow.
In this case there is a zb_scheduler_wakeup() routine shall be defined.
Since it is a platform-specific item, it shall be defined in OSIF */ 
#if defined(ZB_THREADS) && !defined(ZB_SCHEDULER_NO_AUTOWAKEUP)
#define ZB_SCHEDULER_WAKEUP() zb_scheduler_wakeup()
#else
#define ZB_SCHEDULER_WAKEUP()
#endif

#if defined ZB_NWK_STOCHASTIC_ADDRESS_ASSIGN && defined ZB_ROUTER_ROLE     /* Zigbee pro */

/**
   Callback function typedef.
   Callback is function planned to execute by another function.
   @note The callback must be declared as reentrant for dscc.

   @param param - callback parameter equals schedule record param fields
   @param param2 - callback parameter contains test value

   @return is equal.
 */
typedef zb_bool_t (ZB_CODE * zb_callback_compare_t)(zb_uint8_t param, void* param2);

#endif

#ifdef ZB_PRO_ADDRESS_ASSIGNMENT_CB
/** @endcond  */ /* internals_doc */

/**
   Callback function typedef.
   Callback is function planned to execute by another function.
   @note The callback must be declared as reentrant for dscc.

   @param param - callback parameter equals schedule record param fields
   @return short address for joining device, or -1 (0xFFFF) which means using the internal mechanisms of addresses assignment
 */
typedef zb_uint16_t (ZB_CODE * zb_addr_assignment_cb_t)(zb_ieee_addr_t ieee_addr);

/*! @cond internals_doc */
#endif

/* zb_cb_q_ent_t moved to zboss_api_internal.h */

#define ZB_SCHEDULER_SET_2PARAM_CB(i) (ZG->sched.cb_flag_bm[(i)/32U] |= (1UL << ((i)%32U)))
#define ZB_SCHEDULER_RESET_2PARAM_CB(i) (ZG->sched.cb_flag_bm[(i)/32U] &= ~(1UL << ((i)%32U)))
#define ZB_SCHEDULER_IS_2PARAM_CB(i) (ZG->sched.cb_flag_bm[(i)/32U] & (1UL << ((i)%32U)))

typedef ZB_PACKED_PRE struct zb_mac_cb_ent_s
{
 zb_callback_t func;   /* currently, it is the same as common queue, */
 zb_uint8_t param;     /* but, possibly, it is better to remove param from it */
}
ZB_PACKED_STRUCT
zb_mac_cb_ent_t;

/* zb_tm_q_ent_t moved to zboss_api_core.h */

/**
   Immediate pending callbacks queue (ring buffer)
 */
#ifndef ZB_CONFIGURABLE_MEM
ZB_RING_BUFFER_DECLARE(zb_cb_q, zb_cb_q_ent_t, ZB_SCHEDULER_Q_SIZE);
#else
/* declare a header and 1 entry */
ZB_RING_BUFFER_DECLARE(zb_cb_q, zb_cb_q_ent_t, 1);
#endif
/* Mac "clear to tx" queue. */
ZB_RING_BUFFER_DECLARE(zb_mac_tx_q, zb_mac_cb_ent_t, ZB_MAC_QUEUE_SIZE);

/* Ring buffer for storing callbacks waiting for delayed buffers */
ZB_RING_BUFFER_DECLARE(zb_delayed_cb_q, zb_delayed_buf_q_ent_t, ZB_BUF_Q_SIZE);

/**
   Data structures for the delayed execution.
 */

typedef void (ZB_CODE * zb_zdo_sleep_ind_cb_t)(zb_uint32_t sleep_tmo);

typedef zb_bool_t (* zb_sched_stopping_cb_checker_t)(zb_callback_t cb);

typedef struct zb_sched_globals_s
{
#ifndef ZB_CONFIGURABLE_MEM
  zb_cb_q_t cb_q;           /*!< immediate callbacks queue  */
#define ZB_CB_Q (&ZG->sched.cb_q)
  zb_uint32_t cb_flag_bm[(ZB_SCHEDULER_Q_SIZE + 31U)/32U];
#else
  zb_cb_q_t *cb_q;
#define ZB_CB_Q (ZG->sched.cb_q)
  zb_uint32_t *cb_flag_bm;
#endif
#ifndef ZB_ALIEN_SCHEDULER
  zb_mac_tx_q_t mac_tx_q;	/* queue of callback's waiting for tx */
#endif
#if defined( ENABLE_USB_SERIAL_IMITATOR )
  zb_callback_t usbc_rx_cb; /*!< Callback to be called on USB data presence. */
#endif /* defined( ENABLE_USB_SERIAL_IMITATOR ) */
#ifndef ZB_CONFIGURABLE_MEM
  zb_tm_q_ent_t tm_buffer[ZB_SCHEDULER_Q_SIZE]; /*!< buffer for the timer queue entries  */
#else
  zb_tm_q_ent_t *tm_buffer;
#endif
  /*  Use list macros for indexed lists and use byte instead pointer here. */
  ZB_POOLED_LIST8_DEFINE(tm_queue);    /*!< delayed callbacks queue  */
  ZB_POOLED_LIST8_DEFINE(tm_freelist); /*!< freelist of the timer queue entries  */
  zb_delayed_cb_q_t delayed_queue[2]; /*!< queue to store delayed callbacks for getting in and out buffers (@ref buffer_types)*/
  zb_uint8_t tm_buffer_usage;   /*!< Usage of timer queue  */
  zb_bool_t stop;
  zb_bool_t stopping;
  zb_sched_stopping_cb_checker_t stopping_cb_checker;
} zb_sched_globals_t;

/**
   Initialize scheduler subsystem.
 */
void zb_sched_init(void);

/**
 * Stop scheduler subsystem.
 */
void zb_sched_stop(void);

/**
   Call all callbacks.
   All cooperative multitasking done here.

   Call all callbacks from the queue. Callbacks can schedule other callbacks, so
   potentially stay here infinite.
   In practice at some point callbacks ring buffer became empty.
   Put device into asleep waiting for interrupts (8051) or wait for data from
   other source (Linux).

   This function usually placed into main loop.

   This function MUST be reentrant in Keil: must not share its xdata segment with
   functions called from it by pointers.

   @return none

   See sched sample
 */
void zb_sched_loop_iteration(void);

#ifndef ZB_ALIEN_SCHEDULER
/* Schedules a callback, that requires NORMAL_FIFO for transfer or security operations,
it will be called after current tx finished or just during next scheduler loop */
#define ZB_SCHEDULE_TX_CB(func, param) zb_schedule_tx_cb(func, param, 0)
#define ZB_SCHEDULE_TX_CB_WITH_HIGH_PRIORITY(func, param) zb_schedule_tx_cb(func, param, 1)
#else

#define ZB_SCHEDULE_TX_CB ZB_SCHEDULE_CALLBACK
#endif

#if defined ZB_TRACE_LEVEL && defined ZB_TRACE_MASK
void zb_scheduler_trace_file_line(zb_uint32_t file_id, zb_uint32_t line_number, zb_callback_t func);
#endif /* ZB_TRACE_LEVEL && ZB_TRACE_MASK */

#if defined ZB_NWK_STOCHASTIC_ADDRESS_ASSIGN && defined ZB_ROUTER_ROLE     /* Zigbee pro */

#ifndef ZB_MINIMAL_CONTEXT
/**
   Cancel scheduled alarm with test by custom compare function.

   This function cancel previously scheduled alarm. Function is identified by
   the pointer. Parameter is identified by compare func (comp) and default parameter (param).

   Record (rec) if found if rec.func==func and comp(rec.param, param)=true

   @param func - function to cancel
   @param comp - custom comparer
   @param param - default parameter for comparer
   @return param of scheduled function or 0 if not found.
 */
zb_uint8_t zb_schedule_alarm_cancel_compare(zb_callback_t func, zb_callback_compare_t comp, void* param);
#endif /* !ZB_MINIMAL_CONTEXT */

#endif

/**
   Return true if scheduler has any pending callbacks
 */
#define ZB_SCHED_HAS_PENDING_CALLBACKS() !ZB_RING_BUFFER_IS_EMPTY(&ZG->sched.cb_q)


/**
   Wait (block, go idle) until condition will not be true.

   @param condition - condition to check for
 */
#define ZB_SCHED_WAIT_COND(condition)           \
do                                              \
{                                               \
  ZB_SCHED_GLOBAL_LOCK();                       \
  while ( !(condition) )                        \
  {                                             \
    ZB_SCHED_GLOBAL_UNLOCK();                   \
    ZB_GO_IDLE();                               \
    ZB_SCHED_GLOBAL_LOCK();                     \
  }                                             \
  ZB_SCHED_GLOBAL_UNLOCK();                     \
}                                               \
while(0)


/**
   Global lock operation
   Protect manipulation with queues in the main loop by this macro.
   It disables interrupts on 8051 device and locks mutex in Linux.
 */
#define ZB_SCHED_GLOBAL_LOCK ZB_OSIF_GLOBAL_LOCK


/**
   Global unlock operation
   Protect manipulation with queues by this macro.
   It enables interrupts on 8051 device and unlocks mutex in Linux.
 */
#define ZB_SCHED_GLOBAL_UNLOCK ZB_OSIF_GLOBAL_UNLOCK

/**
   Global lock operation - call from the interrupt handler

   @return RET_OK if success, RET_BUSY if locked by userspace
 */
#define ZB_SCHED_GLOBAL_LOCK_INT() ZB_OSIF_GLOBAL_LOCK_INT

/**
   Global unlock operation - call from the interrupt handler
 */
#define ZB_SCHED_GLOBAL_UNLOCK_INT() ZB_OSIF_GLOBAL_UNLOCK_INT

#ifndef ZB_MINIMAL_CONTEXT
/**
 *  @brief Schedules a callback, that requires NORMAL_FIFO for transfer or security operations, it
 *  will be called after current tx finished or just during next scheduler loop.
 *  @param func - callback function.
 *  @param param - parameter for callback.
 *  @returns schedule status.
 */
zb_ret_t zb_schedule_tx_cb(zb_callback_t func, zb_uint8_t param, zb_uint8_t prior);
#endif /* !ZB_MINIMAL_CONTEXT */


/**
   Set a callback to be called when USB serial data is available.

   Callback is called with param 0.
   To actually read serial data, call @ref usbc_serial_data_rx.
   This function could be called, for example, after successful join.


   @param usbc_rx_cb - callback to be called. If NULL, effectively
    unregister USB rx callback.
 */
void zb_sched_register_usbc_rx_cb(zb_callback_t usbc_rx_cb);

#ifdef ZB_LWIP
/**
   Set a callback to be called when ethernet data is available.
 */
void zb_sched_register_ethernet_cb(zb_callback_t usbc_rx_cb);
#endif /* ZB_LWIP */

#define ZB_SCHED_TX_CB_HIGH_PRIOR_RESERVE 1U

/**
   Schedule callback from another thread

 *  @param func - callback function.
 *  @param param - parameter for callback.
 */
void zb_schedule_callback_from_alien(zb_callback_t func, zb_uint8_t param);

#ifdef ZB_DEBUG_BUFFERS_EXT
  void zb_schedule_trace_queue();
#define ZB_SCHEDULE_TRACE_QUEUE()       zb_schedule_trace_queue()
#else
#define ZB_SCHEDULE_TRACE_QUEUE()
#endif

zb_ret_t zb_schedule_callback(zb_callback_t func, zb_uint8_t param);

/**
   Schedule single-param callback execution.

   Schedule execution of function `func' in the main scheduler loop.

   The return was intentionally suppressed to avoid MISRA 17.7 violation.
   If its return is needed the API should be called directly

   @param func - function to execute
   @param param - callback parameter - usually, but not always ref to packet buffer

   See sched sample
 */
#define ZB_SCHEDULE_CALLBACK(func, param) (void)zb_schedule_callback(func, param)

zb_ret_t zb_schedule_callback2(zb_callback2_t func, zb_uint8_t param, zb_uint16_t user_param);

/**
   Schedule two-param callback execution.
   Schedule execution of function `func' in the main scheduler loop.

   The return was intentionally suppressed to avoid MISRA 17.7 violation.
   If its return is needed the API should be called directly

   @param func - function to execute
   @param param - zb_uint8_t callback parameter - usually, but not always ref to
   packet buffer
   @param user_param - zb_uint16_t user parameter - usually, but not
   always short address

   See sched sample
 */
#define ZB_SCHEDULE_CALLBACK2(func, param, user_param) (void)zb_schedule_callback2(func, param, user_param)

zb_ret_t zb_schedule_callback_prior(zb_callback_t func, zb_uint8_t param);

/**
   Schedule single-param high priority callback execution.

   Schedule high priority execution of function `func' in the main scheduler loop.

   The return was intentionally suppressed to avoid MISRA 17.7 violation.
   If its return is needed the API should be called directly

   @param func - function to execute
   @param param - callback parameter - usually, but not always ref to packet buffer

   See sched sample
 */
#define ZB_SCHEDULE_CALLBACK_PRIOR(func, param) (void)zb_schedule_callback_prior(func, param)

zb_ret_t zb_schedule_alarm(zb_callback_t func, zb_uint8_t param, zb_time_t timeout_bi);

/**
   Schedule alarm - callback to be executed after timeout.

   Function will be called via scheduler after timeout expired (maybe, plus some
   additional time).
   Timer resolution depends on implementation.
   Same callback can be scheduled for execution more then once.

   The return was intentionally suppressed to avoid MISRA 17.7 violation.
   If its return is needed the API should be called directly

   @param func - function to call via scheduler
   @param param - parameter to pass to the function
   @param timeout_bi - timeout, in beacon intervals

   See any sample
 */
#define ZB_SCHEDULE_ALARM(func, param, timeout_bi) (void)zb_schedule_alarm(func, param, timeout_bi)

/**
   Cancel scheduled alarm.

   This function cancel previously scheduled alarm. Function is identified by
   the pointer.

   @param func - function to cancel
   @param param - parameter to cancel. \see ZB_ALARM_ANY_PARAM. \see ZB_ALARM_ALL_CB
   @return RET_OK or error code

   See reporting_srv sample
 */
#define ZB_SCHEDULE_ALARM_CANCEL(func, param) (void)zb_schedule_alarm_cancel((func), (param), NULL)

/**
   Cancel scheduled alarm and get buffer.

   This function cancel previously scheduled alarm and returns buffer ref associated with alarm.
   Function is identified by the pointer.

   @param func - function to cancel
   @param param - parameter to cancel. \see ZB_ALARM_ANY_PARAM. \see ZB_ALARM_ALL_CB
   @param p_param - [out] pointer of ref buffer from cancelled flag: free buffer if its alarm will be cancel
   @return RET_OK or error code

   @b Example:
   @code
   {
     zb_uint8_t cancelled_param;

     ZB_SCHEDULE_ALARM_CANCEL_AND_GET_BUF(my_func1, ZB_ALARM_ANY_PARAM, &cancelled_param);
     my_func1(cancelled_param);
   }
   @endcode

   See reporting_srv sample
 */
#define ZB_SCHEDULE_ALARM_CANCEL_AND_GET_BUF(func, param, p_param) (void)zb_schedule_alarm_cancel((func), (param), (p_param))


/**
   Start ZBOSS shutdown procedure in the scheduler.

   Drop all callbacks and alarms, disable alarms planning - but keep callbacks
   planning and scheduler working.
   That is necessary to complete MAC reset at shut.
 */
void zb_scheduler_start_shutting(void);

void zb_scheduler_set_cb_checker(zb_sched_stopping_cb_checker_t checker);

/** @endcond */ /* internals_doc */
/*! @} */


#endif /* ZB_SCHEDULER_H */
