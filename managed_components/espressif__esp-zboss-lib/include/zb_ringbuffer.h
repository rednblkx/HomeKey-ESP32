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
/* PURPOSE: Declare ring buffer internals
*/

#ifndef ZB_RINGBUFFER_H
#define ZB_RINGBUFFER_H 1

/*! @cond internals_doc */
/**
   @addtogroup ZB_BASE
   @{
*/


/**
   \par Generic ring buffer macros
 */

/**
   Declare ring buffer for entries of given type and capacity.
   This is typedef, not variable declaration.

   @param type_name_prefix - prefix for names (like xxx_s, xxx_t)
   @param ent_type - type of the ring buffer entry
   @param capacity - ring buffer capacity
 */
#define ZB_RING_BUFFER_DECLARE(type_name_prefix, ent_type, capacity)  \
typedef struct type_name_prefix ## _s                                 \
{                                                                     \
  zb_ushort_t    read_i;                                              \
  zb_ushort_t    write_i;                                             \
  zb_ushort_t    written;                                             \
  ent_type       ring_buf[capacity];                                  \
} type_name_prefix ## _t


/**
 * Initialize ring buffer internals
 */
#define ZB_RING_BUFFER_INIT(rb) ( (rb)->read_i = (rb)->write_i = (rb)->written = 0U)

/**
 * Return ring buffer capacity
 *
 * @param rb - ring buffer pointer.
 */
#define ZB_RING_BUFFER_CAPACITY(rb) ((sizeof((rb)->ring_buf) / sizeof((rb)->ring_buf[0])))

/**
 * Return 1 if ring buffer is full
 *
 * @param rb - ring buffer pointer.
 */
#define ZB_RING_BUFFER_IS_FULL(rb) ((zb_uint_t)(rb)->written >= ZB_RING_BUFFER_CAPACITY(rb))

/**
 * Return 1 if ring buffer is empty
 *
 * @param rb - ring buffer pointer.
 */
#define ZB_RING_BUFFER_IS_EMPTY(rb) ((rb)->written == 0U)


/**
 * Return free space available in the ring buffer
 *
 * @param rb - ring buffer pointer.
 */
#define ZB_RING_BUFFER_FREE_SPACE(rb) (ZB_RING_BUFFER_CAPACITY(rb) - (zb_uint_t)(rb)->written)

/**
 * Reserve slot in the ring buffer but do not update pointers
 *
 * @param rb -  ring buffer pointer.
 * @return Pointer to the ring buffer entry or NULL if ring buffer is full
 */
#define ZB_RING_BUFFER_PUT_RESERVE(rb)          \
(                                               \
  ZB_RING_BUFFER_IS_FULL(rb) ? NULL             \
  : (rb)->ring_buf + (rb)->write_i              \
  )

#define ZB_RING_BUFFER_PUT_HEAD_RESERVE_IDX(rb)            \
(                                                          \
  ((rb)->read_i > 0U) ?                                    \
  ((rb)->read_i - 1U) : (ZB_RING_BUFFER_CAPACITY(rb) - 1U) \
  )

#define ZB_RING_BUFFER_PUT_HEAD_RESERVE(rb)                    \
(                                                              \
  ZB_RING_BUFFER_IS_FULL(rb) ? NULL                            \
  : ((rb)->ring_buf + ZB_RING_BUFFER_PUT_HEAD_RESERVE_IDX(rb)) \
  )

/**
 * Put to the ring buffer.
 * Get free slot from the ring buffer, return pointer to it.
 *
 * @param rb -  ring buffer pointer.
 * @return nothing
 */
#define ZB_RING_BUFFER_FLUSH_PUT(rb)                                    \
(                                                                       \
  (rb)->written++,                                                      \
  (rb)->write_i = ((rb)->write_i + 1U) % ZB_RING_BUFFER_CAPACITY(rb)     \
  )

#define ZB_RING_BUFFER_FLUSH_PUT_HEAD(rb)            \
do                                                   \
{                                                    \
  (rb)->written++;                                   \
  if ((rb)->read_i > 0U)                             \
  {                                                  \
    --((rb)->read_i);                                \
  }                                                  \
  else                                               \
  {                                                  \
    (rb)->read_i = ZB_RING_BUFFER_CAPACITY(rb) - 1U; \
  }                                                  \
} while(ZB_FALSE)



/**
   Return amount of data which can be put into ring buffer tail starting from write_i

   @param rb - ring buffer pointer
   @param size - requested data size
 */
#define ZB_RING_BUFFER_LINEAR_PORTION(rb, size)         \
(                                                       \
  ZB_RING_BUFFER_CAPACITY(rb) - (rb)->write_i < size ?  \
  ZB_RING_BUFFER_CAPACITY(rb) - (rb)->write_i : size                    \
)

/**
   Return amount of data which can be put into ring buffer tail starting from write_i using external rb capacity

   @param rb - ring buffer pointer
   @param size - requested data size
 */
#define ZB_RING_BUFFER_LINEAR_PORTION_BY_CAP(rb, size, rb_cap) \
(                                                              \
  (rb_cap) - (rb)->write_i < (size) ?                          \
  (rb_cap) - (rb)->write_i : (size)                            \
  )

/**
 * Get the size of available for writing continuous portion in ring buffer.
 *
 * That portion is from write_i index to the end of the buffer or
 * from write_i index to the beginning of previously written but still not read data
 *
 * @param buf -  ring buffer pointer
 * @return size of available continuous portion
 */
#define ZB_RING_BUFFER_AVAILABLE_CONTINUOUS_PORTION(rb) \
  ZB_RING_BUFFER_LINEAR_PORTION((rb), ZB_RING_BUFFER_FREE_SPACE(rb))

/**
   Batch put data into ringbuffer

   To be used to copy from external buffer to ring buffer

   @param rb - ring buffer pointer
   @param data - data ptr
   @param size - requested data size
   @param entries_written - (out) amount of data put
 */
#define ZB_RING_BUFFER_BATCH_PUT(rb, data, size, entries_written)                    \
do                                                                                   \
{                                                                                    \
  (entries_written) = ZB_RING_BUFFER_LINEAR_PORTION((rb), (size));                   \
  ZB_MEMCPY((rb)->ring_buf + (rb)->write_i, (data), (entries_written));              \
  (rb)->written += (entries_written);                                                \
  (rb)->write_i = (((rb)->write_i + (entries_written)) % ZB_RING_BUFFER_CAPACITY(rb)); \
} while(ZB_FALSE)

/**
   Batch put data into ringbuffer using external rb capacity

   To be used to copy from external buffer to ring buffer

   @param rb - ring buffer pointer
   @param data - data ptr
   @param size - requested data size
   @param entries_written - (out) amount of data put
   @param rb_cap - cap of the ring buffer
 */
#define ZB_RING_BUFFER_BATCH_PUT_BY_CAP(rb, data, size, entries_written, rb_cap)    \
do                                                                                  \
{                                                                                   \
  (entries_written) = ZB_RING_BUFFER_LINEAR_PORTION_BY_CAP((rb), (size), (rb_cap)); \
  ZB_MEMCPY((rb)->ring_buf + (rb)->write_i, (data), (entries_written));             \
  (rb)->written += (entries_written);                                               \
  (rb)->write_i = (((rb)->write_i + (entries_written)) % (rb_cap));                 \
} while(ZB_FALSE)


/**
 * Flush after put more then 1 element to the ring buffer
 *
 * Do not check for write_i overflow!
 *
 * @param rb -  ring buffer pointer.
 * @param size - number of items put
 * @return nothing
 */
#define ZB_RING_BUFFER_FLUSH_BATCH_PUT(rb, size)                        \
(                                                                       \
  (rb)->written += size,                                                \
  (rb)->write_i = (((rb)->write_i) + size) % ZB_RING_BUFFER_CAPACITY(rb) \
  )


/**
 * Put value to the ring buffer.
 *
 * @param rb -  ring buffer pointer.
 * @param value - value to put to ring buffer
 * @return nothing
 */
#define ZB_RING_BUFFER_PUT(rb, value)                                   \
(                                                                       \
  (rb)->ring_buf[(rb)->write_i] = (value),                              \
  (rb)->written++,                                                      \
  (rb)->write_i = ((rb)->write_i + 1U) % ZB_RING_BUFFER_CAPACITY(rb)     \
  )


/**
 * Put value to the ring buffer using memcpy.
 *
 * @param rb -  ring buffer pointer.
 * @param value_ptr - pointer to value to put to ring buffer
 * @return nothing
 */
#define ZB_RING_BUFFER_PUT_PTR(rb, value_ptr)                           \
(                                                                       \
  memcpy(&((rb)->ring_buf[(rb)->write_i]), (value_ptr), sizeof((rb)->ring_buf[0])), \
  (rb)->written++,                                                      \
  (rb)->write_i = ((rb)->write_i + 1U) % ZB_RING_BUFFER_CAPACITY(rb)     \
  )


/**
 * Reinsert last value into the ring buffer using memcpy.
 *
 * @param rb -  ring buffer pointer.
 * @param value_ptr - pointer to value to put to ring buffer
 * @return nothing
 */
#define ZB_RING_BUFFER_PUT_REUSE_LAST(rb, value_ptr)                    \
(                                                                       \
  memcpy(&((rb)->ring_buf[((rb)->write_i ? (rb)->write_i - 1U : (rb)->write_i + ZB_RING_BUFFER_CAPACITY(rb) - 1U)]), \
           (value_ptr),                                                 \
           sizeof((rb)->ring_buf[0]))                                   \
    )


/**
 * Get entry from the ring buffer read pointer position
 *
 * @param rb -  ring buffer pointer.
 *
 * @return pointer to the ring buffer entry or NULL if it is empty
 */
#define ZB_RING_BUFFER_PEEK(rb)                 \
(                                               \
  ZB_RING_BUFFER_IS_EMPTY(rb) ? NULL            \
  : (rb)->ring_buf + (rb)->read_i               \
  )

/**
 * Get entry from the ring buffer read pointer position
 *
 * @param rb -  ring buffer pointer.
 *
 * @return pointer to the ring buffer entry
 */
#define ZB_RING_BUFFER_GET(rb)                 \
(                                               \
  (rb)->ring_buf + (rb)->read_i               \
  )

/**
 * Get entry from the ring buffer write pointer position
 *
 * @param rb -  ring buffer pointer.
 *
 * @return pointer to the ring buffer entry
 */
#define ZB_RING_BUFFER_GETW(rb)                 \
( (rb)->ring_buf + (rb)->write_i )

/**
 * Move ring buffer read pointer.
 *
 * To be used after ZB_RING_BUFFER_PEEK().
 * @note This macro does not check for an empty ring buffer.
 *
 * @param rb -  ring buffer pointer.
 * @return nothing
 */
#define ZB_RING_BUFFER_FLUSH_GET(rb)                                    \
(                                                                       \
  (rb)->written--,                                                      \
  ((rb)->read_i = ((rb)->read_i + 1U) % ZB_RING_BUFFER_CAPACITY(rb))     \
  )


/**
 * Get entries from the ring buffer read pointer position which can be get at once
 *
 * @param rb -  ring buffer pointer.
 * @param size - (out) number of entries which can be got
 *
 * @return pointer to the ring buffer entry
 */
#define ZB_RING_BUFFER_GET_BATCH(rb, size)                              \
  (                                                                     \
    (size) = ((rb)->written <= (ZB_RING_BUFFER_CAPACITY(rb) - (rb)->read_i) \
            ?                                                           \
            (rb)->written                                               \
            :                                                           \
            (ZB_RING_BUFFER_CAPACITY(rb) - (rb)->read_i)),              \
    (rb)->ring_buf + (rb)->read_i                                       \
  )                                                                     \



/**
 * Move ring buffer read pointer for more than 1 element
 *
 * @note This macro does not check for empty ring buffer.
 *
 * @param rb -  ring buffer pointer.
 * @param size - number of elements to mark as read
 * @return nothing
 */
#define ZB_RING_BUFFER_FLUSH_GET_BATCH(rb, size)                        \
(                                                                       \
  (rb)->written -= (size),                                              \
  ((rb)->read_i = ((rb)->read_i + (size)) % ZB_RING_BUFFER_CAPACITY((rb))) \
  )

/**
 * Move ring buffer's read and write pointers to the specified pos
 *
 * @param rb -  ring buffer pointer.
 * @param b - new position of read_i buffer's pointer
 * @param e - new position of write_i buffer's pointer
 * @return nothing
 */
#define ZB_RING_BUFFER_LOCATE(rb, b, e)               \
  ( (rb)->read_i = b,                                 \
    (rb)->write_i = e,                                \
    (rb)->written = e - b                             \
  )

/**
 * Returns number of used entries
 *
 * @param rb - ring buffer pointer.
 */

#define ZB_RING_BUFFER_USED_SPACE(rb)                                   \
(                                                                       \
  (rb)->written                                                         \
  )

#define ZB_RING_BUFFER_SEARCH_GET(_rb, _i)                      \
(                                                               \
  ZB_RING_BUFFER_IS_EMPTY(_rb) ? NULL                           \
  : (_rb)->ring_buf + ((_rb)->read_i +(_i)) % ZB_RING_BUFFER_CAPACITY(_rb)      \
  )


/**
 * This is a fake type used for type casting.
 * Represents array of bytes, used for serial trace e.t.c.
 *
 * @Example:
 * @code
 *   ZB_RING_BUFFER_DECLARE(sniffer_io_buffer, zb_uint8_t, 4096);
 *   ...
 *   zb_osif_set_user_io_buffer((zb_byte_array_t*)&gs_sniffer_io_buf, 4096);
 * @endcode
 */
ZB_RING_BUFFER_DECLARE(zb_byte_array, zb_uint8_t, 1);

/**
 * @see ZB_RING_BUFFER_IS_EMPTY
 */
#define ZB_BYTE_ARRAY_IS_EMPTY(rb) ZB_RING_BUFFER_IS_EMPTY(rb)

/**
 * @see ZB_RING_BUFFER_PEEK
 */
#define ZB_BYTE_ARRAY_PEEK(rb) ZB_RING_BUFFER_PEEK(rb)

/**
 * Return 1 if ring buffer is full
 *
 * @param rb - ring buffer pointer.
 * @param cap - ring buffer capacity (number of bytes)
 */
#define ZB_BYTE_ARRAY_IS_FULL(rb, cap) ((zb_uint_t)(rb)->written >= cap)

/**
 * Move ring buffer read pointer.
 *
 * To be used after ZB_BYTE_ARRAY_PEEK().
 * @note This macro does not check for empty ring buffer.
 *
 * @param rb -  ring buffer pointer.
 * @param cap - ring buffer capacity (number of bytes)
 * @return nothing
 */
#define ZB_BYTE_ARRAY_FLUSH_GET(rb, cap)      \
(                                            \
  (rb)->written--,                           \
  ((rb)->read_i = ((rb)->read_i + 1U) % (cap)) \
  )

/* 10/04/17 CR Ustimenko start */
/**
 * Get entries from the ring buffer read pointer position which can be get at once
 *
 * @param rb -  ring buffer pointer.
 * @param size - (out) number of entries which can be got
 * @param cap - ring buffer capacity (number of bytes)
 *
 * @return pointer to the ring buffer entry
 */
#define ZB_BYTE_ARRAY_GET_BATCH(rb, size, cap)                              \
  (                                                                     \
    size = ((rb)->written <= (cap - (rb)->read_i)  \
            ?                                                           \
            (rb)->written                                               \
            :                                                           \
            (cap - (rb)->read_i)),                 \
    (rb)->ring_buf + (rb)->read_i                                       \
  )

/**
 * Move ring buffer read pointer for more than 1 element
 *
 * This macro does not check for an empty ring buffer.
 *
 * @param rb -  ring buffer pointer.
 * @param size - number of elements to mark as read
 * @param cap - ring buffer capacity (number of bytes)
 * @return nothing
 */
#define ZB_BYTE_ARRAY_FLUSH_GET_BATCH(rb, size, cap)                        \
(                                                                       \
  (rb)->written -= size,                                                      \
    ((rb)->read_i = ((rb)->read_i + size) % cap)     \
  )
/* 10/04/17 CR Ustimenko end */

/**
 * Put value to the ring buffer.
 *
 * @param rb -  ring buffer pointer.
 * @param value - value to put to ring buffer
 * @param cap - ring buffer capacity (number of bytes)
 * @return nothing
 */
#define ZB_BYTE_ARRAY_PUT(rb, value, cap)       \
(                                              \
  (rb)->ring_buf[(rb)->write_i] = (value),     \
  (rb)->written++,                             \
  (rb)->write_i = ((rb)->write_i + 1U) % cap    \
  )

#define ZB_BYTE_ARRAY_PUT_HEAD_RESERVE_IDX(rb, cap)         \
(                                                         \
  ((rb)->read_i > 0U) ?                                   \
  ((rb)->read_i - 1U) : ((cap) - 1U) \
  )

#define ZB_BYTE_ARRAY_PUT_HEAD_RESERVE(rb, cap)                    \
(                                                              \
  ((rb)->ring_buf + ZB_BYTE_ARRAY_PUT_HEAD_RESERVE_IDX(rb, (cap)))      \
  )

#define ZB_BYTE_ARRAY_FLUSH_PUT_HEAD(rb, cap)       \
do                                                  \
{                                                   \
  (rb)->written++;                                  \
  if ((rb)->read_i > 0U)                            \
  {                                                 \
    --((rb)->read_i);                               \
  }                                                 \
  else                                              \
  {                                                 \
    (rb)->read_i = (cap) - 1U;                      \
  }                                                 \
} while(ZB_FALSE)

#define ZB_BYTE_ARRAY_FLUSH_PUT(rb, cap)                                    \
(                                                                       \
  (rb)->written++,                                                      \
  (rb)->write_i = ((rb)->write_i + 1U) % (cap)     \
  )

/*! @} */
/*! @endcond */

#endif /* ZB_RINGBUFFER_H */
