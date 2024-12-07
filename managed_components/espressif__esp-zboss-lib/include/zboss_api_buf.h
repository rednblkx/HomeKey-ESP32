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
/*  PURPOSE: Public buffer pool API (new version)
*/
#ifndef ZBOSS_API_BUF_H
#define ZBOSS_API_BUF_H 1

/*! \addtogroup buf */
/*! @{ */

#include "zboss_api_core.h"

/*
  Moved here buffer structure to implement configurable mem without enabling legacy buffers
 */

#define ZB_RESERVED_BUF_TO_ALIGN_HDR_SIZE    1u

/**
   Packet buffer header.
 */
typedef ZB_PACKED_PRE struct zb_buf_hdr_s
{
  zb_ret_t status;            /*!< some status to be passed with packet  */
  zb_uint16_t len;              /*!< current layer buffer length  */
  zb_uint16_t data_offset;      /*!< data offset in buffer buf*/
  zb_uint8_t  multiplicity;     /*!< if greater that 1, then the following (multiplicity - 1) buffers
                                 * are occupied with payload data and should not be treated as
                                 * having valid headers, etc.
                                 */
  zb_uint8_t handle;           /*!< The handle associated with the NSDU to be
                                * transmitted by the NWK layer entity.  */
  zb_bitfield_t is_in_buf:1;    /*!< if 1, this is input buffer */

  zb_bitfield_t encrypt_type:3; /*!< payload must be encrypted before send, if
                                 * !0. \see zb_secur_buf_encr_type_e.
                                 */
  zb_bitfield_t use_same_key:1;    /*!< if 1, use same nwk key packet was
                                    * encrypted by */
  zb_bitfield_t zdo_cmd_no_resp:1; /*!< if 1, this is ZDO command with no
                                    * response - call callback at confirm  */
  zb_bitfield_t is_rx_buf:1;        /*!< if 1, this is buffer with received packet and
                                     * nwk_mac_addrs_t is at buffer tail */
  zb_bitfield_t has_aps_payload:1;  /*!< if 1, than packet comes from APS, the flag is needed
                                     * to increase APS packets counter in diagnostic data on packet sending
                                     */
  zb_bitfield_t has_aps_user_payload:1;   /*!< if 1, than packet comes with APS user's payload */

  zb_bitfield_t tail_len:7;    /*!< the length of the perameters section  */
} ZB_PACKED_STRUCT zb_buf_hdr_t;

/* if there is a platform with failed assertion, ZB_RESERVED_BUF_TO_ALIGN_HDR_SIZE
   should be changed to appropriate value*/
ZB_ASSERT_COMPILE_DECL((sizeof(zb_buf_hdr_t) % sizeof(zb_uint32_t)) == 0);

#ifdef ZB_DEBUG_BUFFERS_EXT
#define ZB_LINE_IS_UNDEF 0xFFFFU

/**
   Extended buffer debug: tracking buffer access
 */

typedef struct zb_buf_usage_s
{
  /* In most cases zb_time_t is zb_uint32_t.
   * To save memory we we use zb_uint16_t so store time. */
  zb_uint16_t time;
  zb_uint16_t file;
  zb_uint16_t line;
}
zb_buf_usage_t;
#endif


/**
   Packet buffer
 */
typedef struct zb_mult_buf_s
{
#ifdef ZB_BUF_SHIELD
  zb_uint8_t hdr_signature;
#endif
/* 07/12/2019 EE CR:MINOR Lagecy code will access u.hdr, so need to keep u as union or structure
   AN: Legacy code inside ZBOSS will use zb_leg_buf_t instead of zb_mult_buf_t.
   EE: I mean legacy API for the new bufs implementation.
 */
  zb_buf_hdr_t hdr;                   /*!< Buffer header, if buffer is used */
  zb_uint8_t   buf[ZB_IO_BUF_SIZE];   /*!< Data*/
#ifdef ZB_DEBUG_BUFFERS_EXT
  zb_buf_usage_t buf_allocation;
  zb_buf_usage_t buf_usages[ZB_DEBUG_BUFFERS_EXT_USAGES_COUNT];
#endif
#ifdef ZB_BUF_SHIELD
  zb_uint8_t tail_signature;
#endif
} zb_mult_buf_t;

/**
   Packet buffer (legacy)
 */
typedef struct zb_leg_buf_s
{
#ifdef ZB_BUF_SHIELD
  zb_uint8_t hdr_signature;
#endif
  union
  {
    zb_buf_hdr_t hdr;                   /*!< Buffer header, if buffer is used */
#ifndef ZB_USE_STD_MALLOC
    struct zb_buf_s *next;              /*!< Ref to next free buffer, if buffer is not used*/
#endif
  } u; /*!< Buffer header */
#if defined ZB_USE_STD_MALLOC && defined ZB_USE_ALLOC_FOR_PAYLOAD_DATA
  zb_uint8_t   *buf;                    /*!< Dynamically allocated buffer for payload */
#else
  zb_uint8_t   buf[ZB_IO_BUF_SIZE];     /*!< Data*/
#endif
#ifdef ZB_DEBUG_BUFFERS_EXT
  zb_buf_usage_t buf_allocation;
  zb_buf_usage_t buf_usages[ZB_DEBUG_BUFFERS_EXT_USAGES_COUNT];
#endif
#ifdef ZB_BUF_SHIELD
  zb_uint8_t tail_signature;
#endif
} zb_leg_buf_t;

#ifdef ZB_LEGACY_BUFS
typedef zb_leg_buf_t zb_buf_ent_t;
#else
typedef zb_mult_buf_t zb_buf_ent_t;
#endif

/**
 * @name Buffer type (direction)
 * @anchor buffer_types
 */
/** @{ */
#define ZB_OUT_BUFFER 0U /*!< Out buffer */
#define ZB_IN_BUFFER  1U /*!< In buffer */
/** @} */

/**
 * @brief Type for Buffer type (direction).
 *
 * Holds one of @ref buffer_types. Kept for backward compatibility as
 * @ref buffer_types were declared previously as enum.
 */
typedef zb_uint8_t zb_buffer_types_t;

/**
   @typedef zb_uint8_t zb_bufid_t
   Buffer handler
 */
typedef zb_uint8_t zb_bufid_t;

#define ZB_BUF_INVALID 0U
#define ZB_UNDEFINED_BUFFER 0U

#ifdef ZB_DEBUG_BUFFERS
#define TRACE_PROTO_VOID zb_uint16_t from_file, zb_uint16_t from_line
#define TRACE_PROTO TRACE_PROTO_VOID ,
#define TRACE_CALL_VOID  ZB_TRACE_FILE_ID, __LINE__
#define TRACE_CALL  TRACE_CALL_VOID ,
#else
#define TRACE_PROTO_VOID void
#define TRACE_PROTO
#define TRACE_CALL_VOID
#define TRACE_CALL
#endif  /* ZB_DEBUG_BUFFERS */

#ifdef ZB_DEBUG_BUFFERS_EXT
  void zb_trace_bufs_usage(void);
  #define ZB_TRACE_BUFS_USAGE() zb_trace_bufs_usage()
#else
  #define ZB_TRACE_BUFS_USAGE()
#endif

/** @cond internals_doc */
zb_bufid_t zb_buf_get_func(TRACE_PROTO zb_bool_t is_in, zb_uint_t max_size);
zb_bufid_t zb_buf_get_out_func(TRACE_PROTO_VOID);
zb_bufid_t zb_buf_get_any_func(TRACE_PROTO_VOID);
zb_bufid_t zb_buf_get_hipri_func(TRACE_PROTO zb_bool_t is_in);
zb_uint_t zb_buf_get_max_size_func(TRACE_PROTO zb_bufid_t buf);
zb_ret_t zb_buf_get_out_delayed_func(TRACE_PROTO zb_callback_t callback);
zb_ret_t zb_buf_get_in_delayed_func(TRACE_PROTO zb_callback_t callback);
zb_ret_t zb_buf_get_out_delayed_ext_func(TRACE_PROTO zb_callback2_t callback, zb_uint16_t arg, zb_uint_t max_size);
zb_ret_t zb_buf_get_in_delayed_ext_func(TRACE_PROTO zb_callback2_t callback, zb_uint16_t arg, zb_uint_t max_size);
void zb_buf_free_func(TRACE_PROTO zb_bufid_t buf);
void* zb_buf_begin_func(TRACE_PROTO zb_bufid_t buf);
void* zb_buf_end_func(TRACE_PROTO zb_bufid_t buf);
zb_uint_t zb_buf_len_func(TRACE_PROTO zb_bufid_t buf);
void zb_buf_copy_func(TRACE_PROTO zb_bufid_t dst_buf, zb_bufid_t src_buf);
void *zb_buf_initial_alloc_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t size);
void *zb_buf_reuse_func(TRACE_PROTO zb_bufid_t buf);
zb_uint_t zb_buf_get_ptr_off_func(TRACE_PROTO zb_bufid_t buf, zb_uint8_t *ptr);
void *zb_buf_alloc_tail_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t size);
void *zb_buf_get_tail_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t size);
void zb_buf_cut_right_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t size);
void *zb_buf_cut_left_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t size);
void *zb_buf_alloc_right_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t size);
void *zb_buf_alloc_left_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t size);
/** @endcond */ /* internals_doc */

/**
   Synchronous buffer allocation.
   If no buffers available, does not block.
   Note: this is low-level API. Usually user calls blocked alloc API.

   @param is_in if ZB_TRUE, allocate IN buffer, else allocate OUT buffer
   @param max_size required maximum buffer payload size (in bytes). It can be bigger or smaller than
   the default buffer size. Depending on the specific value, the buffer pool may decide to use
   a fraction of buffer or long buffers. Special value 0 means "single default buffer".
   @return buffer ID or ZB_BUF_INVALID if no buffers available
 */
#define zb_buf_get(is_in,max_size) zb_buf_get_func(TRACE_CALL (is_in), (max_size))

/**
   Allocate OUT buffer of the default size.
 */
#define zb_buf_get_out() zb_buf_get_out_func(TRACE_CALL_VOID)

/**
   Allocate IN or OUT buffer balancing @ of allocated IN and OUT

   To be used at ZBOSS init time.
 */
#define zb_buf_get_any() zb_buf_get_any_func(TRACE_CALL_VOID)

/**
   Synchronous allocation of the high-priority buffer
 */
#define zb_buf_get_hipri(a) zb_buf_get_hipri_func(TRACE_CALL (a))

/**
   Return maximum data size for that buffer.
 */
#define zb_buf_get_max_size(a) zb_buf_get_max_size_func(TRACE_CALL (a))

/**
 * @brief Allocate OUT buffer, call a callback when the buffer is available.
 *
 * Use default buffer size _func(alloc single standard buffer).
 * If buffer is available, schedules callback for execution immediately. If no buffers are available now,
 * schedule callback later, when buffer will be available.
 *
 * @param callback - callback to call.
 * @return RET_OK or error code.
 */
#ifndef zb_buf_get_out_delayed
#define zb_buf_get_out_delayed(callback) zb_buf_get_out_delayed_func(TRACE_CALL (callback))
#endif /* zb_buf_get_out_delayed */

/**
 * @brief Allocate IN buffer, call a callback when the buffer is available.
 *
 * Use default buffer size _func(alloc single standard buffer).
 * If buffer is available, schedules callback for execution immediately. If no buffers are available now,
 * schedule callback later, when buffer will be available.
 *
 * @param callback - callback to call.
 * @return RET_OK or error code.
 */
#ifndef zb_buf_get_in_delayed
#define zb_buf_get_in_delayed(callback) zb_buf_get_in_delayed_func(TRACE_CALL (callback))
#endif /* zb_buf_get_in_delayed */

/**
 * @brief Allocate OUT buffer, call a callback when the buffer is available.
 *
 * If buffer is available, schedules callback for execution immediately. If no buffers are available now,
 * schedule callback later, when buffer will be available.
 *
 * @param callback - callback to call.
 * @param arg - second argument for a callback
 * @param max_size - required maximum buffer payload size (in bytes). The buffer payload size
 * is the sum of both data and parameters, so the max_size parameter should
 * reflect this. It can be bigger or smaller than the default buffer size.
 * Depending on the specific value, the buffer pool may decide to use a fraction
 * of buffer or long buffers. If the value is set to 0, the payload size will be equal
 * to the size of a single default buffer.
 * @return RET_OK if allocation has successfully scheduled.
 *         RET_ERROR if there is no room to schedule buffer allocation.
 *         RET_OUT_OF_RANGE if buffer with such payload size can't be allocated.
 */
#ifndef zb_buf_get_out_delayed_ext
#define zb_buf_get_out_delayed_ext(callback,arg,max_size) zb_buf_get_out_delayed_ext_func(TRACE_CALL (callback),(arg),(max_size))
#endif /* zb_buf_get_out_delayed_ext */

/**
 * @brief Allocate IN buffer, call a callback when the buffer is available.
 *
 * If buffer is available, schedules callback for execution immediately. If no buffers are available now,
 * schedule callback later, when buffer will be available.
 *
 * @param callback - callback to call.
 * @param arg - second argument for a callback
 * @param max_size required maximum buffer payload size (in bytes). The buffer payload size
 * is the sum of both data and parameters, so the max_size parameter should
 * reflect this. It can be bigger or smaller than the default buffer size.
 * Depending on the specific value, the buffer pool may decide to use a fraction
 * of buffer or long buffers. If the value is set to 0, the payload size will be equal
 * to the size of a single default buffer.
 * @return RET_OK if allocation has successfully scheduled.
 *         RET_ERROR if there is no room to schedule buffer allocation.
 *         RET_OUT_OF_RANGE if buffer with such payload size can't be allocated.
 */
#ifndef zb_buf_get_in_delayed_ext
#define zb_buf_get_in_delayed_ext(callback,arg,max_size) zb_buf_get_in_delayed_ext_func(TRACE_CALL (callback),(arg),(max_size))
#endif /* zb_buf_get_in_delayed_ext */

/**
 * @brief Free packet buffer and put it into free list.
 *
 * Can be called from the main loop.
 *
 * @param buf - buffer ID
 *
 */
#define zb_buf_free(buf) zb_buf_free_func(TRACE_CALL (buf))

/**
 * Return pointer to data stored in buffer
 *
 * @param buf - buffer ID
 *
 * @return pointer to beginning of data in buffer
 *
 */
#define zb_buf_begin(buf) zb_buf_begin_func(TRACE_CALL (buf))

/**
  Return pointer to the data section end

  @param buf - buffer ID

  @return pointer to the first byte after data in the buffer
*/
#define zb_buf_end(buf) zb_buf_end_func(TRACE_CALL (buf))

/**
 * Return current buffer length
 *
 * @param buf - buffer ID
 *
 * @return size of data placed in buffer
 */
#define zb_buf_len(buf) zb_buf_len_func(TRACE_CALL (buf))


/**
   Copy one buffer to another

   @param src_buf - source buffer
   @param dst_buf - destination buffer
 */
#define zb_buf_copy(dst_buf,src_buf) zb_buf_copy_func(TRACE_CALL (dst_buf),(src_buf))

/**
   @brief Initial data space allocation in buffer.

   Allocate space in the buffer center (keep space in both buffer head
   and tail). Use it at upper layers before filling buffer by data.
   Old buffer contents is lost.

   @param buf - buffer ID
   @param size  - size to allocate
   @return pointer to buffer data begin
 */
#define zb_buf_initial_alloc(buf,size) zb_buf_initial_alloc_func(TRACE_CALL (buf),(size))

/**
   Reuse buffer data space by setting data start and length to 0 and zeroing buffer contents

   @param buf - buffer ID

   @return pointer to the buf data buffer begin
 */
#define zb_buf_reuse(buf) zb_buf_reuse_func(TRACE_CALL (buf))
/** @cond internals_doc */
/**
   Calculate distance between ptr and buffer's data buffer start.

   That function used internally in ZCL. It is not recommended to be used by the application.

   @param buf - buffer ID
   @param ptr - user's ptr inside the buffer

   @return offset between ptr and buffer start returned by zb_buf_reuse()
 */
#define zb_buf_get_ptr_off(buf,ptr) zb_buf_get_ptr_off_func(TRACE_CALL (buf),(ptr))
/** @endcond */ /* internals_doc */
/**
   Alloc buffer tail of size 'size', initialize by zero.

   Usually used to place external information (some parameters) to the buffer.

   @param buf - buffer ID
   @param size - requested size. Must met already allocated size.

   @return pointer to the buffer tail or NULL is buffer has no parameter of such size.
*/
#define zb_buf_alloc_tail(buf,size) zb_buf_alloc_tail_func(TRACE_CALL (buf),(size))

/**
   Get or allocate buffer tail of size 'size'. Do not initialize.

   Usually used to get or place external information (some parameters) to the buffer.

   @param buf - buffer ID
   @param size - requested size

   @return pointer to the buffer tail
 */
#define zb_buf_get_tail(buf,size) zb_buf_get_tail_func(TRACE_CALL (buf),(size))

/**
   Get buffer tail of size sizeof(type)

   Usually used to place external information _func(some parameters) to the buffer

   @param[in] buf - buffer ID
   @param[in] type - data type that will be placed at the buffer end

   @return pointer to the buffer tail casted to _func(type*)

   @b Example
   @snippet linky_sample/erl_gw/erl_gw.c zb_buf_get_param_snippet
 */
/* Note: MISRA C-STAT analysis gives false positive for rule 20.7 violation on ZB_BUF_GET_PARAM()
 * due to missing parenthesis around "type" on pointer type cast. This is a misinterpretation
 * of the rule by C-STAT tool, since this rule refers only to expressions, and not data type casts
 * like it is used here. */
#define ZB_BUF_GET_PARAM(buf, type) ((type *)zb_buf_get_tail_func(TRACE_CALL (buf), sizeof(type)))

/**
 * Cut space at the end of buffer
 *
 * @param buf - buffer ID
 * @param size  - size to cut
 */
#define zb_buf_cut_right(buf,size) zb_buf_cut_right_func(TRACE_CALL (buf),(size))

/**
 * Cut space at the beginning of buffer
 *
 * @param buf - buffer ID
 * @param size  - size to cut
 * @return pointer to the new data begin
*/
#define zb_buf_cut_left(buf,size) zb_buf_cut_left_func(TRACE_CALL (buf),(size))

/**
 * Allocate space at buffer end
 *
 * @param buf - buffer ID
 * @param size  - size to allocate
 * @return pointer to allocated data data begin
 */
#define zb_buf_alloc_right(buf,size) zb_buf_alloc_right_func(TRACE_CALL (buf),(size))

/**
 * Allocate space at the beginning of buffer
 *
 * @param buf - buffer ID
 * @param size  - size to allocate
 * @return pointer to new data begin
 */
#define zb_buf_alloc_left(buf,size) zb_buf_alloc_left_func(TRACE_CALL (buf),(size))

/**
 * @name Buffer's internals flags bitmask
 * @anchor buf_flags_bm
 */
/** @{ */
#define ZB_BUF_SECUR_NO_ENCR    0U        /*!< No encryption  */
#define ZB_BUF_IS_IN            (1U << 0)
/* Encrypt flags. That enum is actually bitmask. */
#define ZB_BUF_SECUR_NWK_ENCR   (1U << 1) /*!< NWK frame encryption  */
#define ZB_BUF_SECUR_APS_ENCR   (1U << 2) /*!< APS encryption. Analyze APS header to define which key to use  */
#define ZB_BUF_SECUR_MAC_ENCR   (1U << 3) /*!< MAC encryption - for 802.15.4 certification only */
#define ZB_BUF_SECUR_ALL_ENCR   (ZB_BUF_SECUR_NWK_ENCR | ZB_BUF_SECUR_APS_ENCR | ZB_BUF_SECUR_MAC_ENCR)
#define ZB_BUF_USE_SAME_KEY     (1U << 4)
#define ZB_BUF_ZDO_CMD_NO_RESP  (1U << 5)
#define ZB_BUF_HAS_APS_PAYLOAD  (1U << 6) /*!< Flag to indicate whether the buffer contains any APS payload */
#define ZB_BUF_HAS_APS_USER_PAYLOAD  (1U << 7) /*!< Flag to indicate whether the buffer contains APS user payload */
/** @} */

/**
 * @brief Type for buffer's internals flags bitmask.
 *
 * @deprecated holds one of @ref buf_flags_bm. Kept only for backward compatibility as
 * @ref buf_flags_bm were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t zb_buf_flags_bm_t;

/** @cond internals_doc */
void zb_buf_flags_or_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t val);
void zb_buf_flags_clr_func(TRACE_PROTO zb_bufid_t buf, zb_uint_t mask);
void zb_buf_flags_clr_encr_func(TRACE_PROTO zb_bufid_t buf);
zb_uint_t zb_buf_flags_get_func(TRACE_PROTO zb_bufid_t buf);
/** @endcond */

/**
   Add bits to the buffer's flags

   @param buf - buffer ID
   @param val - value to be ORed with buffer' flags
 */
#define zb_buf_flags_or(buf,val) zb_buf_flags_or_func(TRACE_CALL (buf),(val))

/**
   Clear buffer' flags by mask by doing flags = flags & ~mask

   @param buf - buffer ID
   @param mask - value to be cleared from the flags - @see @ref buf_flags_bm
 */
#define zb_buf_flags_clr(buf,mask) zb_buf_flags_clr_func(TRACE_CALL (buf),(mask))


/**
   Clear buffer's flags related to encryption

   That function calls zb_buf_flags_clr(buf, ZB_BUF_SECUR_ALL_ENCR)
   @param buf - buffer ID
 */
#define zb_buf_flags_clr_encr(buf) zb_buf_flags_clr_encr_func(TRACE_CALL (buf))

/**
   Get buffer's flags byte

   @param buf - buffer ID
   @return flags value - @see @ref buf_flags_bm
 */
#define zb_buf_flags_get(buf) zb_buf_flags_get_func(TRACE_CALL (buf))

/**
   Check if buffer pool is in Out Of Memory (OOM) state

   @return ZB_TRUE if ZBOSS is in OOM state
 */
zb_bool_t zb_buf_is_oom_state(void);

#ifdef ZB_TRACE_LEVEL
/**
   Trace buffer statistics into ZBOSS trace
 */
void zb_buf_oom_trace(void);
#endif /* ZB_TRACE_LEVEL */

#ifdef ZB_REDUCE_NWK_LOAD_ON_LOW_MEMORY
/**
   Check if buffer pool is close to Out Of Memory (OOM) state

   @return ZB_TRUE if ZBOSS is nearly in OOM state
 */
zb_bool_t zb_buf_memory_close_to_low(void);
#endif /* ZB_REDUCE_NWK_LOAD_ON_LOW_MEMORY */

/**
   Check if buffer pool is close to Out Of Memory (OOM) state

   @return ZB_TRUE if ZBOSS is nearly in OOM state
 */
zb_bool_t zb_buf_memory_low(void);

/** @cond internals_doc */
zb_ret_t zb_buf_get_status_func(TRACE_PROTO zb_bufid_t buf);
void zb_buf_set_status_func(TRACE_PROTO zb_bufid_t buf, zb_ret_t status);
zb_uint8_t zb_buf_get_handle_func(TRACE_PROTO zb_bufid_t buf);
void zb_buf_set_handle_func(TRACE_PROTO zb_bufid_t buf, zb_uint8_t handle);
/** @endcond */

/**
   Get 'status' field of the buffer's header

   @param buf - buffer ID
   @return status field value
 */
#define zb_buf_get_status(buf) zb_buf_get_status_func(TRACE_CALL (buf))

/**
   Set 'status' field of the buffer's header

   @param buf - buffer ID
   @param status - new status field value
 */
#define zb_buf_set_status(buf,status) zb_buf_set_status_func(TRACE_CALL (buf), (zb_ret_t)(status))

/**
   Get 'handle' field of the buffer's header

   @param buf - buffer ID
   @return handle field value
 */
#define zb_buf_get_handle(buf) zb_buf_get_handle_func(TRACE_CALL (buf))

/**
   Set 'handle' field of the buffer's header

   @param buf - buffer ID
   @param handle - 'handle' field value
 */
#define zb_buf_set_handle(buf,handle) zb_buf_set_handle_func(TRACE_CALL (buf),(handle))

/**
   Set or reset "mac needs more buffers" flag.

   To be used internally by ZBOSS.

   @param needs - new "mac needs more buffers" flag value
 */
void zb_buf_set_mac_rx_need(zb_bool_t needs);

/**
   Get  "mac needs more buffers" flag.

   To be used internally by ZBOSS.

   @return "mac needs more buffers" flag value
 */
zb_bool_t zb_buf_get_mac_rx_need(void);

zb_bool_t zb_buf_have_rx_bufs(void);

#define ZB_BUF_COPY_FLAG_APS_PAYLOAD(dst, src)                          \
  do {                                                                  \
    if ((zb_buf_flags_get((src)) & ZB_BUF_HAS_APS_PAYLOAD) != 0U)       \
    {                                                                   \
      zb_buf_flags_or((dst), ZB_BUF_HAS_APS_PAYLOAD);                   \
                                                                        \
      if ((zb_buf_flags_get((src)) & ZB_BUF_HAS_APS_USER_PAYLOAD) != 0U)\
      {                                                                 \
        zb_buf_flags_or((dst), ZB_BUF_HAS_APS_USER_PAYLOAD);            \
      }                                                                 \
    }                                                                   \
  } while(0)

/*! @} */

#endif /* ZBOSS_API_BUF_H */
