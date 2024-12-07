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
/* PURPOSE: Zigbee address management
*/

#ifndef ZB_ADDRESS_H
#define ZB_ADDRESS_H 1

/*! \addtogroup ZB_NWK_ADDR */
/*! @{ */


/*! @cond internals_doc */

#define ZB_UNKNOWN_SHORT_ADDR 0xFFFFU

#ifdef ZB_DEBUG_ADDR
#define TRACE_ADDR_PROTO_VOID    zb_uint16_t from_file, zb_uint16_t from_line
#define TRACE_ADDR_CALL_VOID     ZB_TRACE_FILE_ID, __LINE__
#define TRACE_ADDR_FORWARD_VOID  from_file, from_line
#define TRACE_ADDR_PROTO         TRACE_ADDR_PROTO_VOID ,
#define TRACE_ADDR_CALL          TRACE_ADDR_CALL_VOID ,
#define TRACE_ADDR_FORWARD       TRACE_ADDR_FORWARD_VOID ,
#else
#define TRACE_ADDR_PROTO_VOID
#define TRACE_ADDR_CALL_VOID
#define TRACE_ADDR_FORWARD_VOID
#define TRACE_ADDR_PROTO
#define TRACE_ADDR_CALL
#define TRACE_ADDR_FORWARD
#endif  /* ZB_DEBUG_ADDR */

/**
   Compressed IEEE address. One byte  device manufacturer - reference to
   \see zb_dev_manufacturer_t array.
*/
typedef ZB_PACKED_PRE struct zb_ieee_addr_compressed_s
{
  zb_uint8_t dev_manufacturer; /*!< Index from dev manufacturer array */
  zb_uint8_t device_id[5]; /*!< Device ID */
}
ZB_PACKED_STRUCT
zb_ieee_addr_compressed_t;

/*! @endcond */


/**
   Pan ID reference

   Should be used inside protocol tables instead of 64-bit Pan ID
*/
typedef zb_uint8_t zb_address_pan_id_ref_t;


/**
   IEEE address reference

   Should be used inside protocol tables instead of 64/16-bit IEEE.
*/
typedef zb_uint8_t zb_address_ieee_ref_t;

/*! @cond internals_doc */

/**
   64-bit / 16-bit address map
*/
typedef ZB_PACKED_PRE struct zb_address_map_s
{
  zb_ieee_addr_compressed_t  ieee_addr; /*!< Compressed IEEE address */
  zb_uint16_t                addr; /*!< 16-bit device address */
  zb_address_ieee_ref_t      redirect_ref; /*!< Reference to redirected or regular record */

  zb_bitfield_t              lock_cnt:8; /*!< lock counter. not locked if 0  */
  zb_bitfield_t              used:1; /*!< if 0, this entry is free (never used)  */
  zb_bitfield_t              has_address_conflict:1; /*!< Set to 1 if device discovers address conflict
                                                      *   Cleared when conflict is resolved:
                                                      *   - Device that discovers conflict sending  Network Status
                                                      *   - or another Network Status with identical payload was received  */
  zb_bitfield_t              padding:2; /*!< Explicit padding bits  */
  zb_bitfield_t              clock:1;    /*!< clock value for the clock usage algorithm  */
  zb_bitfield_t              redirect_type:2; /*!< redirect type @ref addr_redirect_type */
  zb_bitfield_t              pending_for_delete:1;    /*!< record is pending for deletion  */

} ZB_PACKED_STRUCT zb_address_map_t;


/**
   \par work with compressed addresses
 */

/*
  * AS: Fixed wrong division 64-bit extended address into
  * manufacturer specific and device unique parts.
*/
#define ZB_ADDRESS_DECOMPRESS(address, compressed_address)               \
do                                                                      \
{                                                                       \
  if (ZB_ADDRESS_COMPRESSED_IS_UNKNOWN(compressed_address))               \
  {                                                                     \
    ZB_64BIT_ADDR_UNKNOWN(address);                                     \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    ZB_MEMCPY(&((address)[5]),                                          \
              &(ZG->addr.dev_manufacturer[(compressed_address).dev_manufacturer].device_manufacturer[0]), \
              (sizeof((address)[0]) * 3U));                              \
    ZB_MEMCPY(&((address)[0]), &((compressed_address).device_id[0]), (sizeof((address)[0]) * 5U)); \
                                                                        \
  }                                                                     \
}                                                                       \
while (0)

/*
#define ZB_ADDRESS_COMPRESSED_CMP(one, two) (             \
    (one).dev_manufacturer == (two).dev_manufacturer      \
  && (one).device_id[0] == (two).device_id[0]             \
  && (one).device_id[1] == (two).device_id[1]             \
  && (one).device_id[2] == (two).device_id[2]             \
  && (one).device_id[3] == (two).device_id[3]             \
  && (one).device_id[4] == (two).device_id[4] )
*/
zb_bool_t zb_address_compressed_cmp(zb_ieee_addr_compressed_t *one, zb_ieee_addr_compressed_t *two);
#define ZB_ADDRESS_COMPRESSED_CMP(one, two) zb_address_compressed_cmp(&one, &two)

#define ZB_ADDRESS_COMPRESSED_COPY(dest, src)                       \
  ZB_MEMCPY(&(dest).dev_manufacturer, &(src).dev_manufacturer, sizeof(zb_ieee_addr_compressed_t))

  /* g_zero_addr is declared as ZB_CONST which allows IAR to place it in CODE memory.
     Compiled this by IAR 7.60 for 8051.
     This placement changes pointer type making it unusable
     Is this cast needed here?
  */
#define ZB_ADDRESS_COMPRESSED_IS_ZERO(dest)      \
  (ZB_MEMCMP(&(dest).dev_manufacturer, (void const *)g_zero_addr, sizeof(zb_ieee_addr_compressed_t)) == 0)

#define ZB_ADDRESS_COMPRESSED_IS_UNKNOWN(dest)      \
  (ZB_MEMCMP(&(dest).dev_manufacturer, (void const *)g_unknown_ieee_addr, sizeof(zb_ieee_addr_compressed_t)) == 0)

#define ZB_ADDRESS_COMPRESS_UNKNOWN(dest)     \
  (ZB_MEMCPY(&(dest).dev_manufacturer, (void const *)g_unknown_ieee_addr, sizeof(zb_ieee_addr_compressed_t)))

/**
   Add Pan ID to address storage and return reference.

   @param short_pan_id - 16-bit Pan ID identifier
   @param pan_id - 64-bit Pan ID identifier
   @param ref - (output) reference to Pan ID.

   @return RET_OK - when success, error code otherwise.

   @b Example
@code
        zb_address_pan_id_ref_t panid_ref;
        zb_ret_t ret;

        ret = zb_address_set_pan_id(mhr.src_pan_id, beacon_payload->extended_panid, &panid_ref);
        if (ret == RET_ALREADY_EXISTS)
        {
          ret = RET_OK;
        }
@endcode

 */
zb_ret_t zb_address_set_pan_id(zb_uint16_t short_pan_id, zb_ext_pan_id_t pan_id, zb_address_pan_id_ref_t *ref);


/**
   Get extended Pan ID with reference.

   @param pan_id_ref - reference to Pan ID
   @param pan_id - (output) Pan ID.

   @return nothing

   @b Example
@code
    zb_uint8_t i;
    ZB_BUF_INITIAL_ALLOC((zb_bufid_t )ZB_BUF_FROM_REF(param),
           sizeof(*discovery_confirm) + sizeof(*network_descriptor) * ZB_PANID_TABLE_SIZE,
                           discovery_confirm);
    zb_nlme_network_descriptor_t *network_descriptor = (zb_nlme_network_descriptor_t *)(discovery_confirm + 1);
    for (i = 0 ; i < ZG->nwk.neighbor.ext_neighbor_used ; ++i)
    {
         zb_address_get_pan_id(ZG->nwk.neighbor.ext_neighbor[i].panid_ref, network_descriptor[j].extended_pan_id);
         network_descriptor[j].logical_channel = ZG->nwk.neighbor.ext_neighbor[i].logical_channel;
         ...
         n_nwk_dsc++;
    }
    discovery_confirm->network_count = n_nwk_dsc;
    discovery_confirm->status = (zb_mac_status_t)((zb_bufid_t )ZB_BUF_FROM_REF(param))->u.hdr.status;
    ZB_SCHEDULE_CALLBACK(zb_nlme_network_discovery_confirm, param);
@endcode

 */
void zb_address_get_pan_id(zb_address_pan_id_ref_t pan_id_ref, zb_ext_pan_id_t pan_id);

/**
   Clears Pan ID table except own pan_id.

   @param pan_id - (our) Pan ID.

   @return nothing

 */
void zb_address_clear_pan_id_table(zb_ext_pan_id_t pan_id);

/**
   Clears whole Pan ID table

   @return nothing

 */
void zb_address_reset_pan_id_table(void);

/*! @endcond */

/**
   Get Pan ID reference with extended Pan ID.

   @param pan_id -  Pan ID
   @param ref - (output) reference to Pan ID

   @return RET_OK - when success, error code otherwise.

   @b Example
@code
    zb_address_pan_id_ref_t my_panid_ref;
    if ( zb_address_get_pan_id_ref(ZB_NIB_EXT_PAN_ID(), &my_panid_ref) != RET_OK )
    {
      TRACE_MSG(TRACE_NWK1, "Pan ID " TRACE_FORMAT_64 " not in Pan ID arr - ?", (FMT__A,
                TRACE_ARG_64(ZB_NIB_EXT_PAN_ID())));
    }
@endcode
 */
zb_ret_t zb_address_get_pan_id_ref(zb_ext_pan_id_t pan_id, zb_address_pan_id_ref_t *ref);

/**
   Get short Pan ID with reference.

   @param pan_id_ref - reference to Pan ID
   @param pan_id_p - (output) Pan ID.


   @b Example
@code
    zb_uint16_t nt_panid;
    zb_address_get_short_pan_id(ZG->nwk.neighbor.ext_neighbor[i].panid_ref, &nt_panid);
    if (nt_panid == pan_id)
    {
        TRACE_MSG(TRACE_NWK1, "pan_id %d is on ch %hd", (FMT__D_H, pan_id, channel));
        unique_pan_id = 0;
    }
@endcode
 */
void zb_address_get_short_pan_id(zb_address_pan_id_ref_t pan_id_ref, zb_uint16_t *pan_id_p);


/**
   Compare Pan ID in the source form with Pan ID reference.

   @param pan_id_ref - Pan ID ref
   @param pan_id     - Pan ID (64-bit)

   @return ZB_TRUE if addresses are equal, ZB_FALSE otherwise

   @b Example
@code
   for (i = 0 ; i < ZG->nwk.neighbor.ext_neighbor_used ; ++i)
    {
        for (j = 0 ; j < n_nwk_dsc &&
                   !zb_address_cmp_pan_id_by_ref(ZG->nwk.neighbor.ext_neighbor[i].panid_ref, network_descriptor[j].extended_pan_id) ;
                 ++j)
        {
            ...
        }
    }
@endcode
 */
zb_bool_t zb_address_cmp_pan_id_by_ref(zb_address_pan_id_ref_t pan_id_ref, zb_ext_pan_id_t pan_id);

/**
   Update long/short address pair. Create the pair if not exist. Optionally, lock.
   Reaction on device announce etc. Long and short addresses are present. Must
   synchronize the address translation table with this information.

   @note Never call zb_address_update() with empty (zero) ieee_address or empty
   (-1) short_address.

   @param ieee_address - long address
   @param short_address - short address
   @param lock - if TRUE, lock address entry
   @param ref_p - (out) address reference

   @return RET_OK or error code

   @b Example
@code
  zb_address_ieee_ref_t addr_ref;
  zb_uint16_t nwk_addr;
  zb_zdo_nwk_addr_resp_head_t *resp = (zb_zdo_nwk_addr_resp_head_t*)zb_buf_begin(buf);
  if (resp->status == ZB_ZDP_STATUS_SUCCESS)
  {
    ZB_LETOH64(ieee_addr, resp->ieee_addr);
    ZB_LETOH16(&nwk_addr, &resp->nwk_addr);
    zb_address_update(ieee_addr, nwk_addr, ZB_TRUE, &addr_ref);
  }
@endcode

 */
zb_ret_t zb_address_update(zb_ieee_addr_t ieee_address, zb_uint16_t short_address, zb_bool_t lock, zb_address_ieee_ref_t *ref_p);

void zb_long_address_update_by_ref(zb_ieee_addr_t ieee_address, zb_address_ieee_ref_t ref);

/**
   Get address with address reference.

   Get existing IEEE (long) and short addresses with address reference. Update address alive
   time if it not locked.

   @param ieee_address  - (out) long address
   @param short_address_p - (out) short address
   @param ref - address reference

   @b Example
@code
    zb_address_ieee_ref_t addr_ref;
    zb_nlme_join_indication_t *resp = ZB_BUF_GET_PARAM((zb_bufid_t )ZB_BUF_FROM_REF(param), zb_nlme_join_indication_t);
    zb_address_by_ref(resp->extended_address, &resp->network_address, addr_ref);
@endcode

 */
void zb_address_by_ref(zb_ieee_addr_t ieee_address, zb_uint16_t *short_address_p, zb_address_ieee_ref_t ref);

/**
   Get IEEE address with address reference.

   Get existing IEEE address(long address) with address reference. Update address alive time if it not locked.

   @param ieee_address  - (out) long address
   @param ref - address reference

   @b Example
@code
void func(zb_neighbor_tbl_ent_t *nbt)
{
  zb_ieee_addr_t ieee_addr;
  zb_address_ieee_by_ref(ieee_addr, nbt->addr_ref);
  ...
}
@endcode

 */
void zb_address_ieee_by_ref(zb_ieee_addr_t ieee_address, zb_address_ieee_ref_t ref);


/**
   Get short address by address reference.

   Get existing short address with address reference. Update address alive time if it not locked.

   @param short_address_p  - (out) short address
   @param ref - address reference

   @b Example
@code
    zb_neighbor_tbl_ent_t *nbt;
    if(zb_nwk_neighbor_with_address_conflict(&nbt)==RET_OK)
    {
        zb_uint16_t addr;
        zb_address_short_by_ref(&addr, nbt->addr_ref);
        func(addr);
    }
@endcode

 */
void zb_address_short_by_ref(zb_uint16_t *short_address_p, zb_address_ieee_ref_t ref);

/**
   Get address ref by long address, optionally create if not exist, optionally lock.
   Update address alive time if not locked.
   @param ieee - IEEE device address
   @param create - if TRUE, create address entry if it does not exist
   @param lock - if TRUE, lock address entry
   @param ref_p - (out) address reference

   @note: never call zb_address_by_ieee() with empty (zero) ieee_address

   @return RET_OK or error code

   @b Example
@code
  void test_get_short_addr(zb_uint8_t param)
  {
    zb_address_ieee_ref_t ref_p;
    zb_bufid_t buf = ZB_BUF_FROM_REF(param);

    if (zb_address_by_ieee(g_ieee_addr_r2, ZB_TRUE, ZB_FALSE, &ref_p) == RET_OK)
    {
      ...
    }
  }
@endcode

 */
zb_ret_t zb_address_by_ieee(const zb_ieee_addr_t ieee, zb_bool_t create, zb_bool_t lock, zb_address_ieee_ref_t *ref_p);


/**
   Get short address by IEEE address (long).

   @param ieee_address - long address

   @return short address if ok, -1 otherwise.

   @par Example
   @snippet thermostat/thermostat_zc/thermostat_zc.c default_short_addr
   @snippet thermostat/thermostat_zc/thermostat_zc.c address_short_by_ieee
   @par

 */
zb_uint16_t zb_address_short_by_ieee(zb_ieee_addr_t ieee_address);


/**
   Get IEEE address (long) with short address.

   @param short_addr - short address
   @param ieee_address - (out)long address

   @return RET_OK or RET_NOT_FOUND

   @b Example
   @snippet light_sample_HA_1_2_bulb/light_coordinator_HA_1_2_bulb/light_zc_HA_1_2_bulb.c address_ieee_by_short

 */
zb_ret_t zb_address_ieee_by_short(zb_uint16_t short_addr, zb_ieee_addr_t ieee_address);


/**
   Get address reference with long address. Create the reference if it does not exist.
   Optionally, lock the address. Update address alive time if not locked.
   @param short_address - 16bit device address
   @param create - if TRUE, create address entry if it does not exist
   @param lock - if TRUE, lock address entry
   @param ref_p - (out) address reference

   @note Never call zb_address_by_short() with empty (-1) short_address

   @return RET_OK or error code

   @b Example
   @snippet simple_gw/simple_gw.c address_by_short

 */
zb_ret_t zb_address_by_short(zb_uint16_t short_address, zb_bool_t create, zb_bool_t lock, zb_address_ieee_ref_t *ref_p);


/*! @cond internals_doc */
/**
   Get address ref by index from short_sorted table.
   @param index - index address short_sorted table
   @param ref_p - (out) address reference

   @return RET_OK or error code

   @b Example
@code
    zb_address_ieee_ref_t ref_p;
    if( ZG->nwk.neighbor.base_neighbor_used > 0 &&
              zb_address_by_sorted_table_index(ZG->nwk.neighbor.send_link_status_index, &ref_p)==RET_OK
    )
    {
        ...
    }
@endcode
 */
zb_ret_t zb_address_by_sorted_table_index(zb_ushort_t index, zb_address_ieee_ref_t *ref_p);
/*! @endcond */

/**
   Check that address is locked (has lock counter > 0)

   @param ref - IEEE/network address pair reference

   @return ZB_TRUE if address is locked
 */
zb_bool_t zb_address_is_locked(zb_address_ieee_ref_t ref);


/**

   Increase address lock counter, when it used in some table.
   Address must be already locked.

   @param ref - IEEE/network address pair reference

   @return RET_OK or RET_ERROR
 */
#define zb_address_lock(ref) zb_address_lock_func(TRACE_ADDR_CALL ref)

zb_ret_t zb_address_lock_func(TRACE_ADDR_PROTO zb_address_ieee_ref_t ref);


/**

   Unlock address counter. Decrease lock counter.

   @param ref - IEEE/network address pair reference
 */
#define zb_address_unlock(ref) zb_address_unlock_func(TRACE_ADDR_CALL ref)

void zb_address_unlock_func(TRACE_ADDR_PROTO zb_address_ieee_ref_t ref);

/**
   Delete address.

   @return RET_OK or RET_ERROR

   @param ref - IEEE/network address pair reference
 */
zb_ret_t zb_address_delete(zb_address_ieee_ref_t ref);


/*! @cond internals_doc */

/**
   Compress long address: store manufacturer address part elsewhere

   This routine packs 8 bytes address to 6 bytes

   @param address - uncompressed address
   @param compressed_address - (out) compressed address

   @b Example
@code
  void func(zb_address_pan_id_ref_t panid_ref, zb_ieee_addr_t long_addr, zb_ext_neighbor_tbl_ent_t **enbt)
  {
    zb_ieee_addr_compressed_t compressed_addr;
    zb_ieee_addr_compress(long_addr, &compressed_addr);

    for (i = 0 ; i < ZG->nwk.neighbor.ext_neighbor_used ; ++i)
    {
      if (ZG->nwk.neighbor.ext_neighbor[i].panid_ref == panid_ref
          && ZB_ADDRESS_COMPRESSED_CMP(ZG->nwk.neighbor.ext_neighbor[i].long_addr, compressed_addr))
      {
         *enbt = &ZG->nwk.neighbor.ext_neighbor[i];
      }
    }
  }
@endcode
 */
void zb_ieee_addr_compress(const zb_ieee_addr_t address, zb_ieee_addr_compressed_t *compressed_address);


/**
   Decompress compressed long address.

   This routine unpacks 6 bytes address to 8 bytes

   @param address - (out) uncompressed address
   @param compressed_address - compressed address

   @b Example
@code
    zb_ieee_addr_t long_address;
    if (ZG->nwk.neighbor.ext_neighbor[i].short_addr != (zb_uint16_t)~0)
    {
      if (!ZB_ADDRESS_COMPRESSED_IS_UNKNOWN(ZG->nwk.neighbor.ext_neighbor[i].long_addr))
      {
        zb_ieee_addr_decompress(long_address, &ZG->nwk.neighbor.ext_neighbor[i].long_addr);
        zb_address_update(long_address, ZG->nwk.neighbor.ext_neighbor[i].short_addr, ZB_FALSE, &addr_ref);
      }
    }
@endcode
 */
void zb_ieee_addr_decompress(zb_ieee_addr_t address, zb_ieee_addr_compressed_t *compressed_address);

/**
   Check that two address refs refer to the one address.
   In this case one record is regular, second - redirect.
   Also returns returns true if addr_ref_a is equal to addr_ref_b.

   @param addr_ref_a Address ref to compare.
   @param addr_ref_b Address ref to compare.
   @return zb_bool_t ZB_FALSE if these address refs refer to different addresses.
                     ZB_TRUE otherwise.
 */
zb_bool_t zb_address_cmp_two_refs(zb_address_ieee_ref_t addr_ref_a, zb_address_ieee_ref_t addr_ref_b);

/**
 * @brief Check that address reference is used
 *
 * @param ref Address ref to check
 * @return zb_bool_t ZB_TRUE if address is used, ZB_FALSE otherwise.
 */
zb_bool_t zb_address_in_use(zb_address_ieee_ref_t ref);

/**
 * @brief Check if address tables have enough memory for the new address
 *
   @param new_addr - new uncompressed IEEE address
 * @return zb_bool_t ZB_TRUE if there is enough memory, ZB_FALSE otherwise.
 */
zb_bool_t zb_address_check_mem_for_new_addr(const zb_ieee_addr_t new_addr);
/*! @endcond */

/*! @} */


#endif /* ZB_ADDRESS_H */
