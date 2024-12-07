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
/* PURPOSE: simple lists operations (macros). Use 1- or 2-bytes instead
instead of pointer.
*/
#ifndef POOLED_LIST_H
#define POOLED_LIST_H 1

/*! @cond internals_doc */
/**
   @addtogroup ZB_BASE
   @{
*/


#ifndef ZB_POOLED_LIST_INCLUDED
#define ZB_POOLED_LIST_INCLUDED

#define ZP_POOLED_LIST_NULL -1

#define ZP_TYPE8( x ) ( ( zb_uint8_t ) ( x ) )
#define ZP_RIND8( x ) ( ( x ) - ZP_TYPE8( 1U ) )

#define ZP_TYPE16( x ) ( ( zb_uint16_t ) ( x ) )
#define ZP_RIND16( x ) ( ( x ) - ZP_TYPE16( 1U ) )

#define ZP_NULL8  ( ZP_TYPE8  ( ZP_POOLED_LIST_NULL ) )
#define ZP_NULL16 ( ZP_TYPE16 ( ZP_POOLED_LIST_NULL ) )

#define ZB_POOLED_LIST8_FIELD( link_field )  zb_uint8_t  link_field ## _next; zb_uint8_t  link_field ## _prev
#define ZB_POOLED_LIST16_FIELD( link_field ) zb_uint16_t link_field ## _next; zb_uint16_t link_field ## _prev

#define ZB_POOLED_LIST8_DEF( list ) zb_uint8_t list = ZP_NULL8
#define ZB_POOLED_LIST16_DEF( list ) zb_uint16_t list = ZP_NULL16

#define ZB_POOLED_LIST8_DEFINE( list ) zb_uint8_t list
#define ZB_POOLED_LIST16_DEFINE( list ) zb_uint16_t list


#define ZB_POOLED_LIST8_INIT( list ) (list) = ZP_NULL8
#define ZB_POOLED_LIST16_INIT( list ) (list) = ZP_NULL16


#define ZB_POOLED_LIST8_IS_EMPTY( list ) ( ( list ) == ZP_NULL8 )
#define ZB_POOLED_LIST8_IS_NOT_EMPTY( list ) ( ( list ) != ZP_NULL8 )

#define ZB_POOLED_LIST16_IS_EMPTY( list ) ( ( list ) == ZP_NULL16 )
#define ZB_POOLED_LIST16_IS_NOT_EMPTY( list ) ( ( list ) != ZP_NULL16 )

#define ZB_POOLED_LIST8_NEXT( base, index, link_field ) ( (  ZP_NULL8 == ( index ) ) ? ZP_NULL8 :  ( base )[ ( index ) ].link_field##_next)
#define ZB_POOLED_LIST8_PREV( base, index, link_field ) ( (  ZP_NULL8 == ( index ) ) ? ZP_NULL8 :  ( base )[ ( index ) ].link_field##_prev)

#define ZB_POOLED_LIST8_GET_HEAD( base, list, link_field ) ( list )
#define ZB_POOLED_LIST8_GET_TAIL( base, list, link_field ) ( ( ZP_NULL8 != list ) ? base[ list ].link_field##_prev : ZP_NULL8 )

#define ZB_POOLED_LIST16_GET_HEAD( base, list, link_field ) ( list )
#define ZB_POOLED_LIST16_GET_TAIL( base, list, link_field ) ( ( ZP_NULL16 != list ) ? base[ list ].link_field##_prev : ZP_NULL16 )

#define ZB_POOLED_LIST16_NEXT( base, index, link_field ) ( (  ZP_NULL16 == index ) ) ? ZP_NULL16 :  base[ index ].link_field##_next ) )
#define ZB_POOLED_LIST16_PREV( base, index, link_field ) ( (  ZP_NULL16 == index ) ) ? ZP_NULL16 :  base[ index ].link_field##_prev ) )

/**
  Remove element from the list head, and store it in removed_entry
 */

#define ZB_POOLED_LIST8_CUT_HEAD( base,list, link_field, removed_elem_index ) do \
{                                                                       \
  removed_elem_index = list;                                            \
  if( ( list ) != ZP_NULL8 )                                            \
  {                                                                     \
    if( ( base )[ list ].link_field##_next != ZP_NULL8 )                \
    {                                                                   \
      ( base )[ ( base )[ list ].link_field##_next ].link_field##_prev = \
         ( base )[ list ].link_field##_prev;                            \
    }                                                                   \
    ( base )[ list ].link_field##_prev = ZP_NULL8;                      \
    ( list )  = ( base )[ list ].link_field##_next ;                    \
  }                                                                     \
} while (0)                                                             \


/**
  Insert to the list head
 */

#define ZB_POOLED_LIST8_INSERT_HEAD( base, list, link_field, elem_index ) do \
{                                                                       \
  ZB_ASSERT( ( elem_index ) != ZP_NULL8 );                              \
  ( base )[ elem_index ].link_field##_next = ( list );                  \
  if( ( list ) != ZP_NULL8 )                                            \
 {                                                                      \
   ( base )[ ( elem_index ) ].link_field##_prev = ( base )[ list ].link_field##_prev; \
   ( base )[ ( list ) ].link_field##_prev = ( elem_index ) ;            \
 }                                                                      \
 else                                                                   \
 {                                                                      \
   ( base )[ elem_index ].link_field##_prev = ( elem_index );           \
 }                                                                      \
 ( list ) = ( elem_index );                                             \
} while( 0 )


/**
  Insert to the list tail
 */

#define ZB_POOLED_LIST8_INSERT_TAIL( base, list, link_field, elem_index ) do \
{                                                                       \
  ZB_ASSERT( ( elem_index ) != ZP_NULL8 );                              \
  ( base )[ ( elem_index ) ].link_field##_next = ZP_NULL8;              \
  if( ( list ) != ZP_NULL8 )                                            \
  {                                                                     \
    ( base )[ ( elem_index ) ].link_field##_prev = ( base )[ ( list ) ].link_field##_prev; \
    ( base )[ ( base )[ ( list ) ].link_field##_prev ].link_field##_next = ( elem_index ); \
    ( base )[ ( list ) ].link_field##_prev = ( elem_index );            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    ( base )[ ( elem_index ) ].link_field##_prev = ( elem_index );      \
    ( list ) = ( elem_index );                                          \
  }                                                                     \
} while (0)


/**
  Insert after existed elem_indexry
 */

#define ZB_POOLED_LIST8_INSERT_AFTER( base, list, link_field, elem_index, new_elem_index ) do \
{                                                                       \
  ZB_ASSERT( ( new_elem_index ) != ZP_NULL8 );                          \
  if( ( elem_index ) != ZP_NULL8  )                                         \
  {                                                                     \
    if( ( ( elem_index ) ) == ZB_POOLED_LIST8_GET_TAIL( ( base ), ( list ), link_field ) ) \
    {                                                                   \
      ZB_POOLED_LIST8_INSERT_TAIL( ( base ), ( list ), link_field, ( new_elem_index ) ); \
    }                                                                   \
    else                                                                \
    {                                                                   \
      ( base )[ new_elem_index ].link_field##_next = ( base )[ ( elem_index ) ].link_field##_next ; \
      ( base )[ new_elem_index ].link_field##_prev = ( elem_index );    \
      ( base )[ ( base )[ ( elem_index ) ].link_field##_next ].link_field##_prev = ( new_elem_index ); \
      ( base )[ ( elem_index ) ].link_field##_next = ( new_elem_index ); \
    }                                                                   \
  }                                                                     \
} while(0)


/**
  Remove from the list head
 */

#define ZB_POOLED_LIST8_REMOVE_HEAD( base, list, link_field ) do        \
{                                                                       \
  if( list != ZP_NULL8 )                                                \
  {                                                                     \
    if( base[ list ].link_field##_next != ZP_NULL8 )                    \
    {                                                                   \
      base[ base[ list ].link_field##_next ].link_field##_prev = base[ list ].link_field##_prev; \
    }                                                                   \
    base[ list ].link_field##_prev = ZP_NULL8;                          \
    list = base[ list ].link_field##_next;                              \
  }                                                                     \
} while (0)

/**
  Remove from the list tail
 */

#define ZB_POOLED_LIST8_REMOVE_TAIL( base, list, link_field ) do        \
{                                                                       \
  if( list != ZP_NULL8 )                                                \
  {                                                                     \
    if( base[ list ].link_field ## _prev == ( list ) ) /* made an empty list */ \
    {                                                                   \
      base[ list ].link_field ## _prev = ZP_NULL8;                      \
      ( list ) = ZP_NULL8;                                              \
    }                                                                   \
    else                                                                \
    {                                                                   \
      base[ base[ base[ list ].link_field##_prev ].link_field##_prev ].link_field##_next = ZP_NULL8; \
                                                                        \
      /* This is a bit tricky: we need to assign list->prev and set     \
       * tail->prev to 0, but we can't use temporary variable because   \
       * we have no type here. Use _next as temporary variable. */      \
                                                                        \
      base[ base[ list ].link_field##_prev ].link_field##_next =        \
      base[ base[ list ].link_field##_prev ].link_field##_prev;         \
                                                                        \
      base[ base[ list ].link_field##_prev ].link_field##_prev = ZP_NULL8; \
                                                                        \
      base[ list ].link_field##_prev =  base[ base[ list ].link_field##_prev  ].link_field##_next; \
    }                                                                   \
  }                                                                     \
} while (0)

#define ZB_POOLED_LIST8_ITERATE( base, list, link_field, elem_index )   \
  for( ( elem_index ) = ( list ) ; ( elem_index != ZP_NULL8 ) ; ( elem_index ) = base[ elem_index ].link_field##_next )


#define ZB_POOLED_LIST8_ITERATE_BACK( base, list, link_field, elem_index) \
  for( ( elem_index ) = ( list ) ? base[ list ].link_field##_prev : ZP_NULL8; \
       ( elem_index != ZP_NULL8 );                                                  \
       ( elem_index ) = ( ( base[ base[ elem_index ].link_field##_prev ].link_field##_next != ZP_NULL8 ) ? \
                            base[ elem_index ].link_field##_prev : ZP_NULL8 ) )

/* NOTE! elem_index is just some index from base array, but not
         ordinary number of the list. So, if you remove elem_index == 2,
         it doesn't mean that you remove third element from the list. It means
         just that you remove some element placed in base[ 2 ] from the list which can
         be actually any ordinary number of the list.
*/

#define ZB_POOLED_LIST8_REMOVE( base, list, link_field, elem_index )    \
do                                                                      \
{                                                                       \
  ZB_ASSERT( ( elem_index ) != ZP_NULL8 );                              \
  if( base[ ( elem_index ) ].link_field##_prev != ZP_NULL8 )            \
  {                                                                     \
    if( ( base )[ ( elem_index ) ].link_field##_next != ZP_NULL8 )    /* this is not a tail */ \
    {                                                                   \
      ( base )[ ( base )[ ( elem_index ) ].link_field##_next ].link_field##_prev = ( base )[ ( elem_index ) ].link_field##_prev; \
    }                                                                   \
    else                          /* this is a tail */                  \
    {                                                                   \
      ( base )[ ( list ) ].link_field##_prev = ( base )[ ( elem_index ) ].link_field##_prev; \
    }                                                                   \
    if( ( elem_index ) == ( list ) )          /* this is a head */      \
    {                                                                   \
      ( list ) = ( base )[ ( elem_index ) ].link_field##_next;          \
    }                                                                   \
    else                          /* this is not a head */              \
    {                                                                   \
      ( base )[ ( base )[ ( elem_index ) ].link_field##_prev ].link_field##_next = ( base )[ ( elem_index ) ].link_field##_next; \
    }                                                                   \
    ( base )[ ( elem_index ) ].link_field##_prev = ZP_NULL8;            \
  }                                                                     \
}                                                                       \
while( 0 )                                                              \


/**
  Remove element from the list head, and store it in removed_entry
 */
#define ZB_POOLED_LIST16_CUT_HEAD( base,list, link_field, removed_elem_index ) do \
{                                                                       \
  ZB_ASSERT( removed_elem_index != ZP_NULL16 );                         \
  removed_elem_index = list;                                            \
  if( ( list ) != ZP_NULL16 )                                           \
  {                                                                     \
    if( base[ list ].link_field##_next != ZP_NULL16 )                   \
    {                                                                   \
      base[ base[ list ].link_field##_next ].link_field##_prev =        \
         base[ list ].link_field##_prev;                                \
    }                                                                   \
    base[ list ].link_field##_prev = ZP_NULL16;                         \
   ( list ) = base[ list ].link_field##_next ;                          \
  }                                                                     \
} while (0)


/**
  Insert to the list head
 */

#define ZB_POOLED_LIST16_INSERT_HEAD( base, list, link_field, elem_index ) do \
{                                                                       \
  ZB_ASSERT( ( elem_index ) != ZP_NULL16 );                             \
  base[ elem_index ].link_field##_next = ( list );                      \
  if( ( list ) != ZP_NULL16 )                                           \
 {                                                                      \
   base[ elem_index ].link_field##_prev = base[ list ].link_field##_prev; \
   base[ list ].link_field##_prev = elem_index ;                        \
 }                                                                      \
 else                                                                   \
 {                                                                      \
   base[ elem_index ].link_field##_prev = elem_index;                   \
 }                                                                      \
 ( list ) = ( elem_index );                                             \
} while( 0 )


/**
  Insert to the list tail
 */

#define ZB_POOLED_LIST16_INSERT_TAIL( base, list, link_field, elem_index ) do \
{                                                                       \
  ZB_ASSERT( ( elem_index ) != ZP_NULL16 );                             \
  base[ elem_index ].link_field##_next = ZP_NULL16;                     \
  if( ( list ) != ZP_NULL16 )                                           \
  {                                                                     \
    base[ elem_index ].link_field##_prev = base[ list ].link_field##_prev; \
    base[ base[ list ].link_field##_prev ].link_field##_next = elem_index; \
    base[ list ].link_field##_prev = elem_index;                        \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    base[ elem_index ].link_field##_prev = elem_index;                  \
    ( list ) = ( elem_index );                                          \
  }                                                                     \
} while (0)


/**
  Insert after existed elem_indexry
 */

#define ZB_POOLED_LIST16_INSERT_AFTER( base, list, link_field, elem_index, new_elem_index ) do \
{                                                                       \
  ZB_ASSERT( new_elem_index != ZP_NULL16 );                             \
  if( elem_index != ZP_NULL16  )                                        \
  {                                                                     \
    if( elem_index == ZB_POOLED_LIST16_GET_TAIL( base, list, link_field ) ) \
    {                                                                   \
      ZB_POOLED_LIST16_INSERT_TAIL( base, list, link_field, new_elem_index ); \
    }                                                                   \
    else                                                                \
    {                                                                   \
      base[ new_elem_index ].link_field##_next = base[ elem_index ].link_field##_next ; \
      base[ new_elem_index ].link_field##_prev = elem_index;            \
      base[ base[ elem_index ].link_field##_next ].link_field##_prev = new_elem_index; \
      base[ elem_index ].link_field##_next = ( new_elem_index );        \
    }                                                                   \
  }                                                                     \
} while(0)


/**
  Remove from the list head
 */

#define ZB_POOLED_LIST16_REMOVE_HEAD( base, list, link_field ) do       \
{                                                                       \
  if( list != ZP_NULL16 )                                               \
  {                                                                     \
    if( base[ list ].link_field##_next != ZP_NULL16 )                   \
    {                                                                   \
      base[ base[ list ].link_field##_next ].link_field##_prev = base[ list ].link_field##_prev; \
    }                                                                   \
    base[ list ].link_field##_prev = ZP_NULL16;                         \
    list = base[ list ].link_field##_next;                              \
  }                                                                     \
} while (0)

/**
  Remove from the list tail
 */

#define ZB_POOLED_LIST16_REMOVE_TAIL( base, list, link_field ) do       \
{                                                                       \
  if( list != ZP_NULL16 )                                               \
  {                                                                     \
    if( base[ list ].link_field ## _prev == ( list ) ) /* made an empty list */ \
    {                                                                   \
      base[ list ].link_field ## _prev = ZP_NULL16;                     \
      ( list ) = ZP_NULL16;                                             \
    }                                                                   \
    else                                                                \
    {                                                                   \
      base[ base[ base[ list ].link_field##_prev ].link_field##_prev ].link_field##_next = ZP_NULL16; \
                                                                        \
      /* This is a bit tricky: we need to assign list->prev and set     \
       * tail->prev to 0, but we can't use temporary variable because   \
       * we have no type here. Use _next as temporary variable. */      \
                                                                        \
      base[ base[ list ].link_field##_prev ].link_field##_next =        \
      base[ base[ list ].link_field##_prev ].link_field##_prev;         \
                                                                        \
      base[ base[ list ].link_field##_prev ].link_field##_prev = ZP_NULL16; \
                                                                        \
      base[ list ].link_field##_prev =  base[ base[ list ].link_field##_prev  ].link_field##_next; \
    }                                                                   \
  }                                                                     \
} while (0)

#define ZB_POOLED_LIST16_ITERATE( base, list, link_field, elem_index )   \
  for( ( elem_index ) = ( list ) ; ( elem_index != ZP_NULL16 ) ; ( elem_index ) = base[ elem_index ].link_field##_next )


#define ZB_POOLED_LIST16_ITERATE_BACK( base, list, link_field, elem_index) \
  for( ( elem_index ) = ( list ) ? base[ list ].link_field##_prev : ZP_NULL16; \
       ( elem_index != ZP_NULL16 );                                     \
       ( elem_index ) = ( ( base[ base[ elem_index ].link_field##_prev ].link_field##_next != ZP_NULL16 ) ? \
                            base[ elem_index ].link_field##_prev : ZP_NULL16 ) )

/* NOTE! elem_index is just some index from base array, but not
         ordinary number of the list. So, if you remove elem_index == 2,
         it doesn't mean that you remove third element from the list. It means
         just that you remove some element placed in base[ 2 ] from the list which can
         be actually any ordinary number of the list.
*/

#define ZB_POOLED_LIST16_REMOVE( base, list, link_field, elem_index )   \
do                                                                      \
{                                                                       \
  ZB_ASSERT( elem_index != ZP_NULL16 );                                 \
  if( base[ elem_index ].link_field##_prev != ZP_NULL16 )               \
  {                                                                     \
    if( base[ elem_index ].link_field##_next != ZP_NULL16 )    /* this is not a tail */ \
    {                                                                   \
      base[ base[ elem_index ].link_field##_next ].link_field##_prev = base[ elem_index ].link_field##_prev; \
    }                                                                   \
    else                          /* this is a tail */                  \
    {                                                                   \
      base[ list ].link_field##_prev = base[ elem_index ].link_field##_prev; \
    }                                                                   \
    if( ( elem_index ) == ( list ) )          /* this is a head */      \
    {                                                                   \
      ( list ) = base[ elem_index ].link_field##_next;                  \
    }                                                                   \
    else                          /* this is not a head */              \
    {                                                                   \
      base[ base[ elem_index ].link_field##_prev ].link_field##_next = base[ elem_index ].link_field##_next; \
    }                                                                   \
    base[ elem_index ].link_field##_prev = ZP_NULL16;                   \
  }                                                                     \
}                                                                       \
while( 0 )


/* Single liked lists */
#define ZP_POOLED_SL_LIST8_NULL 255U


#define ZB_POOLED_SL_LIST8_DEFINE(type, list) type list ## _head; type list ## _tail

#define ZB_POOLED_SL_LIST8_INIT(list) list ## _head = ZP_POOLED_SL_LIST8_NULL; list ## _tail = ZP_POOLED_SL_LIST8_NULL


#define ZB_POOLED_SL_LIST8_FIELD(type, name) type name ## _next


#define ZB_POOLED_SL_LIST8_NEXT(base, index, name) ( ( (index) == ZP_POOLED_SL_LIST8_NULL ) ? ZP_POOLED_SL_LIST8_NULL : (base)[index].name ## _next )

#define ZB_POOLED_SL_LIST8_HEAD(list) list ## _head

#define ZB_POOLED_SL_LIST8_TAIL(list) list ## _tail

#define ZB_POOLED_SL_LIST8_IS_EMPTY(list) ( list ## _head == ZP_POOLED_SL_LIST8_NULL )
#define ZB_POOLED_SL_LIST8_IS_NOT_EMPTY(list) ( list ## _head != ZP_POOLED_SL_LIST8_NULL )


/**
  Insert to the list head
 */
#define ZB_POOLED_SL_LIST8_INSERT_HEAD(base, list, name, index)         \
do                                                                      \
{                                                                       \
  (base)[index].name ## _next = list ## _head;                          \
  if ( (list ## _head) + 1 == ZP_POOLED_SL_LIST8_NULL )                 \
  {                                                                     \
    (list ## _tail) = (index);                                          \
  }                                                                     \
  list ## _head = (index);                                              \
} while( 0 )


/**
  Insert to the list tail
 */
#define ZB_POOLED_SL_LIST8_INSERT_TAIL(base, list, name, index)         \
do                                                                      \
{                                                                       \
  (base)[index].name##_next = ZP_POOLED_SL_LIST8_NULL;                  \
  if ( (list ## _tail) != ZP_POOLED_SL_LIST8_NULL)                      \
  {                                                                     \
    (base)[list ## _tail].name##_next = index;                          \
    list ## _tail = (index);                                            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    (list ## _head) = (index);                                          \
    (list ## _tail) = (index);                                          \
  }                                                                     \
} while (0)


/**
  Remove from the list head
 */
#define ZB_POOLED_SL_LIST8_REMOVE_HEAD(base, list, name)                \
do                                                                      \
{                                                                       \
  if ( (list ## _head) != ZP_POOLED_SL_LIST8_NULL )                     \
  {                                                                     \
    list ## _head = (base)[list ## _head].name ## _next;                \
    if ( (list ## _head) == ZP_POOLED_SL_LIST8_NULL)                    \
    {                                                                   \
      (list ## _tail) = ZP_POOLED_SL_LIST8_NULL;                        \
    }                                                                   \
  }                                                                     \
} while (0)


/**
  Remove element from the list head, and store it in index
 */
#define ZB_POOLED_SL_LIST8_CUT_HEAD(base, list, name, index)            \
do                                                                      \
{                                                                       \
  if ( (list ## _head) != ZP_POOLED_SL_LIST8_NULL )                     \
  {                                                                     \
    (index) = (list ## _head);                                          \
    (list ## _head) = (base)[list ## _head].name ## _next;              \
    if ( (list ## _head) == ZP_POOLED_SL_LIST8_NULL)                    \
    {                                                                   \
      (list ## _tail) = ZP_POOLED_SL_LIST8_NULL;                        \
    }                                                                   \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    (index) = ZP_POOLED_SL_LIST8_NULL;                                  \
  }                                                                     \
} while (0)


/**
  Remove element from the list tail
 */
#define ZB_POOLED_SL_LIST8_CUT_TAIL(base, list, name, index)            \
do                                                                      \
{                                                                       \
  (index) = (list ## _tail);                                            \
  ZB_POOLED_SL_LIST8_REMOVE(base, list, name, index);                   \
} while (0)


/**
   Remove index from list
 */
#define ZB_POOLED_SL_LIST8_REMOVE(base, list, name, index)              \
do                                                                      \
{                                                                       \
  zb_uint8_t p;                                                         \
  zb_uint8_t prev = ZP_POOLED_SL_LIST8_NULL;                            \
  for (p = (list ## _head);                                             \
       p != ZP_POOLED_SL_LIST8_NULL;                                    \
       prev = p, p = (base)[p].name ## _next )                          \
  {                                                                     \
    if ( p == (index) )                                                 \
    {                                                                   \
      if ( prev != ZP_POOLED_SL_LIST8_NULL )                            \
      {                                                                 \
        (base)[prev].name ## _next = (base)[p].name ## _next;           \
        if ((list ## _tail) == index)                                   \
        {                                                               \
          (list ## _tail) = prev;                                       \
        }                                                               \
      }                                                                 \
      else                                                              \
      {                                                                 \
        (list ## _head) = (base)[p].name ## _next;                      \
      }                                                                 \
      if ( (list ## _head) == ZP_POOLED_SL_LIST8_NULL )                 \
      {                                                                 \
        (list ## _tail) = ZP_POOLED_SL_LIST8_NULL;                      \
      }                                                                 \
      break;                                                            \
    }                                                                   \
  }                                                                     \
}                                                                       \
while( 0 )


#define ZB_POOLED_SL_LIST8_ITERATE(base, list, name, index)             \
  for( (index) = (list); (index != ZP_POOLED_SL_LIST8_NULL); (index) = base[index].name##_next )

#endif

/*! @} */
/*! @endcond */

#endif
