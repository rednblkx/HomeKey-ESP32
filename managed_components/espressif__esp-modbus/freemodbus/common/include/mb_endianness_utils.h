/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

/**
 * @brief Defines the constant values based on native compiler byte ordering.
 */
#define MB_BO16_0 0
#define MB_BO16_1 1

#define MB_BO32_0 0
#define MB_BO32_1 1
#define MB_BO32_2 2
#define MB_BO32_3 3

#define MB_BO64_0 0
#define MB_BO64_1 1
#define MB_BO64_2 2
#define MB_BO64_3 3
#define MB_BO64_4 4
#define MB_BO64_5 5 
#define MB_BO64_6 6
#define MB_BO64_7 7

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The sized array types used for mapping of extended values
 */
typedef uint8_t val_16_arr[2];
typedef uint8_t val_32_arr[4];
typedef uint8_t val_64_arr[8];

/**
 * @brief Get int8_t (low byte) value represenatation from register
 *
 * @return
 *     - int8_t value of converted from register value
 */
int8_t mb_get_int8_a(val_16_arr *pi16);

/**
 * @brief Set i8 value to the register value pointed by pi16
 *
 * @return
 *     - uint16_t value which represents the actual hex value of the register
 */
uint16_t mb_set_int8_a(val_16_arr *pi16, int8_t i8);

/**
 * @brief Get int8_t (high byte) value from the register value pointed by pi16
 *
 * @return
 *     - uint16_t value which represents the actual hex value of the register
 */
int8_t mb_get_int8_b(val_16_arr *pi16);

/**
 * @brief Set i8 (high byte) value from the register value pointed by pi16
 *
 * @return
 *     - uint16_t value which represents the actual hex value of the register
 */
uint16_t mb_set_int8_b(val_16_arr *pi16, int8_t i8);

/**
 * @brief Get uint8_t (low byte) value represenatation from register poined by pu16
 *
 * @return
 *     - uint8_t the value of converted from register value
 */
uint8_t mb_get_uint8_a(val_16_arr *pu16);

/**
 * @brief Set u8 (low byte) value into the register value pointed by pu16
 *
 * @return
 *     - uint16_t the value which represents the actual hex value of the register
 */
uint16_t mb_set_uint8_a(val_16_arr *pu16, uint8_t u8);

/**
 * @brief Get uint8_t (high byte) value from the register value pointed by pu16
 *
 * @return
 *     - uint16_t the value which represents the actual hex value of the register
 */
uint8_t mb_get_uint8_b(val_16_arr *pu16);

/**
 * @brief Set u8 (high byte) value into the register value pointed by pu16
 *
 * @return
 *     - uint16_t the value which represents the actual hex value of the register
 */
uint16_t mb_set_uint8_b(val_16_arr *pu16, uint8_t u8);

/**
 * @brief Get int16_t value from the register value pointed by pu16 with ab endianness
 *
 * @return
 *     - int16_t the value which represents the converted value from register
 */
int16_t mb_get_int16_ab(val_16_arr *pi16);

/**
 * @brief Set i16 value to the register pointed by pi16 with ab endianness
 *
 * @return
 *     - int16_t the value which represents the converted value from register
 */
uint16_t mb_set_int16_ab(val_16_arr *pi16, int16_t i16);

/**
 * @brief Get uint16_t value from the register value pointed by pu16 with ab endianness
 *
 * @return
 *     - uint16_t value which represents the converted register value
 */
uint16_t mb_get_uint16_ab(val_16_arr *pu16);

/**
 * @brief Set u16 value to the register pointed by pu16 with ab endianness
 *
 * @return
 *     - uint16_t value which represents the converted value from register
 */
uint16_t mb_set_uint16_ab(val_16_arr *pu16, uint16_t u16);

/**
 * @brief Get int16_t value from the register value pointed by pu16 with ba endianness
 *
 * @return
 *     - int16_t value which represents the converted register value
 */
int16_t mb_get_int16_ba(val_16_arr *pi16);

/**
 * @brief Set i16 value to the register pointed by pi16 with ba endianness
 *
 * @return
 *     - uint16_t value which represents the converted value from register
 */
uint16_t mb_set_int16_ba(val_16_arr *pi16, int16_t i16);

/**
 * @brief Get uint16_t value from the register value pointed by pu16 with ba endianness
 *
 * @return
 *     - uint16_t value which represents the converted register value
 */
uint16_t mb_get_uint16_ba(val_16_arr *pu16);

/**
 * @brief Set u16 value to the register pointed by pu16 with ba endianness
 *
 * @return
 *     - uint16_t value which represents the converted value from register
 */
uint16_t mb_set_uint16_ba(val_16_arr *pu16, uint16_t u16);

/**
 * @brief Get int32_t value from the register value pointed by pi32 with abcd endianness
 *
 * @return
 *     - int32_t value which represents the converted register value
 */
int32_t mb_get_int32_abcd(val_32_arr *pi32);

/**
 * @brief Set i32 value to the register pointed by pi32 with abcd endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_int32_abcd(val_32_arr *pi32, int32_t i32);

/**
 * @brief Get uint32_t value from the register value pointed by pu32 with abcd endianness
 *
 * @return
 *     - uint32_t value which represents the converted register value
 */
uint32_t mb_get_uint32_abcd(val_32_arr *pu32);

/**
 * @brief Set u32 value to the register pointed by pu32 with abcd endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_uint32_abcd(val_32_arr *pu32, uint32_t u32);

/**
 * @brief Get int32_t value from the register value pointed by pi32 with badc endianness
 *
 * @return
 *     - int32_t value which represents the converted register value
 */
int32_t mb_get_int32_badc(val_32_arr *pi32);

/**
 * @brief Set i32 value to the register pointed by pi32 with badc endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_int32_badc(val_32_arr *pi32, int32_t i32);

/**
 * @brief Get uint32_t value from the register value pointed by pu32 with badc endianness
 *
 * @return
 *     - unt32_t value which represents the converted register value
 */
uint32_t mb_get_uint32_badc(val_32_arr *pu32);

/**
 * @brief Set u32 value to the register pointed by pu32 with badc endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_uint32_badc(val_32_arr *pu32, uint32_t u32);

/**
 * @brief Get int32_t value from the register value pointed by pi32 with cdab endianness
 *
 * @return
 *     - int32_t value which represents the converted register value
 */
int32_t mb_get_int32_cdab(val_32_arr *pi32);

/**
 * @brief Set i32 value to the register pointed by pi32 with cdab endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_int32_cdab(val_32_arr *pi32, int32_t i32);

/**
 * @brief Get uint32_t value from the register value pointed by pu32 with cdab endianness
 *
 * @return
 *     - int32_t value which represents the converted register value
 */
uint32_t mb_get_uint32_cdab(val_32_arr *pu32);

/**
 * @brief Set u32 value to the register pointed by pu32 with cdab endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_uint32_cdab(val_32_arr *pu32, uint32_t u32);

/**
 * @brief Get int32_t value from the register value pointed by pi32 with dcba endianness
 *
 * @return
 *     - int32_t value which represents the converted register value
 */
int32_t mb_get_int32_dcba(val_32_arr *pi32);

/**
 * @brief Set i32 value to the register pointed by pi32 with dcba endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_int32_dcba(val_32_arr *pi32, int32_t i32);

/**
 * @brief Get uint32_t value from the register value pointed by pu32 with dcba endianness
 *
 * @return
 *     - uint32_t value which represents the converted register value
 */
uint32_t mb_get_uint32_dcba(val_32_arr *pu32);

/**
 * @brief Set u32 value to the register pointed by pu32 with dcba endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_uint32_dcba(val_32_arr *pu32, uint32_t u32);

/**
 * @brief Get float value from the register pointed by pf with abcd endianness
 *
 * @return
 *     - float value which represents the converted register value
 */
float mb_get_float_abcd(val_32_arr *pf);

/**
 * @brief Set f value to the register pointed by pf with abcd endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_float_abcd(val_32_arr *pf, float f);

/**
 * @brief Get float value from the register pointed by pf with badc endianness
 *
 * @return
 *     - float value which represents the converted register value
 */
float mb_get_float_badc(val_32_arr *pf);

/**
 * @brief Set f value to the register pointed by pf with badc endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_float_badc(val_32_arr *pf, float f);

/**
 * @brief Get float value from the register pointed by pf with cdab endianness
 *
 * @return
 *     - float value which represents the converted register value
 */
float mb_get_float_cdab(val_32_arr *pf);

/**
 * @brief Set f value to the register pointed by pf with cdab endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_float_cdab(val_32_arr *pf, float f);

/**
 * @brief Get float value from the register pointed by pf with dcba endianness
 *
 * @return
 *     - float value which represents the converted register value
 */
float mb_get_float_dcba(val_32_arr *pf);

/**
 * @brief Set f value to the register pointed by pf with dcba endianness
 *
 * @return
 *     - uint32_t value which represents the converted value from register
 */
uint32_t mb_set_float_dcba(val_32_arr *pf, float f);

/**
 * @brief Get double value from the register pointed by pd with abcdefgh endianness
 *
 * @return
 *     - double value which represents the converted register value
 */
double mb_get_double_abcdefgh(val_64_arr *pd);

/**
 * @brief Set d value to the register pointed by pd with abcdefgh endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_double_abcdefgh(val_64_arr *pd, double d);

/**
 * @brief Get double value from the register pointed by pd with hgfedcba endianness
 *
 * @return
 *     - double value which represents the converted register value
 */
double mb_get_double_hgfedcba(val_64_arr *pd);

/**
 * @brief Set d value to the register pointed by pd with hgfedcba endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_double_hgfedcba(val_64_arr *pd, double d);

/**
 * @brief Get double value from the register pointed by pd with ghefcdab endianness
 *
 * @return
 *     - double value which represents the converted register value
 */
double mb_get_double_ghefcdab(val_64_arr *pd);

/**
 * @brief Set d value to the register pointed by pd with ghefcdab endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_double_ghefcdab(val_64_arr *pd, double d);

/**
 * @brief Get double value from the register pointed by pd with badcfehg endianness
 *
 * @return
 *     - double value which represents the converted register value
 */
double mb_get_double_badcfehg(val_64_arr *pd);

/**
 * @brief Set d value to the register pointed by pd with badcfehg endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_double_badcfehg(val_64_arr *pd, double d);

/**
 * @brief Get int64_t value from the register pointed by pi64 with abcdefgh endianness
 *
 * @return
 *     - int64_t value which represents the converted register value
 */
int64_t mb_get_int64_abcdefgh(val_64_arr *pi64);

/**
 * @brief Set i value to the register pointed by pi with abcdefgh endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_int64_abcdefgh(val_64_arr *pi, int64_t i);

/**
 * @brief Get int64_t value from the register pointed by pi64 with ghefcdab endianness
 *
 * @return
 *     - int64_t value which represents the converted register value
 */
int64_t mb_get_int64_ghefcdab(val_64_arr *pi64);

/**
 * @brief Set i value to the register pointed by pi with ghefcdab endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_int64_ghefcdab(val_64_arr *pi, int64_t i);

/**
 * @brief Get int64_t value from the register pointed by pi64 with hgfedcba endianness
 *
 * @return
 *     - int64_t value which represents the converted register value
 */
int64_t mb_get_int64_hgfedcba(val_64_arr *pi64);

/**
 * @brief Set i value to the register pointed by pi with hgfedcba endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_int64_hgfedcba(val_64_arr *pi, int64_t i);

/**
 * @brief Get int64_t value from the register pointed by pi64 with badcfehg endianness
 *
 * @return
 *     - int64_t value which represents the converted register value
 */
int64_t mb_get_int64_badcfehg(val_64_arr *pi64);

/**
 * @brief Set i value to the register pointed by pi with badcfehg endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_int64_badcfehg(val_64_arr *pi, int64_t i);

/**
 * @brief Get uint64_t value from the register pointed by pui with abcdefgh endianness
 *
 * @return
 *     - uint64_t value which represents the converted register value
 */
uint64_t mb_get_uint64_abcdefgh(val_64_arr *pui);

/**
 * @brief Set ui value to the register pointed by pi with abcdefgh endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_uint64_abcdefgh(val_64_arr *pui, uint64_t ui);

/**
 * @brief Get uint64_t value from the register pointed by pui with hgfedcba endianness
 *
 * @return
 *     - uint64_t value which represents the converted register value
 */
uint64_t mb_get_uint64_hgfedcba(val_64_arr *pui);

/**
 * @brief Set ui value to the register pointed by pui with hgfedcba endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_uint64_hgfedcba(val_64_arr *pui, uint64_t ui);

/**
 * @brief Get uint64_t value from the register pointed by pui with ghefcdab endianness
 *
 * @return
 *     - uint64_t value which represents the converted register value
 */
uint64_t mb_get_uint64_ghefcdab(val_64_arr *pui);

/**
 * @brief Set ui value to the register pointed by pui with ghefcdab endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_uint64_ghefcdab(val_64_arr *pui, uint64_t ui);

/**
 * @brief Get uint64_t value from the register pointed by pui with badcfehg endianness
 *
 * @return
 *     - uint64_t value which represents the converted register value
 */
uint64_t mb_get_uint64_badcfehg(val_64_arr *pui);

/**
 * @brief Set ui value to the register pointed by pui with badcfehg endianness
 *
 * @return
 *     - uint64_t value which represents the converted value from register
 */
uint64_t mb_set_uint64_badcfehg(val_64_arr *pui, uint64_t ui);

#ifdef __cplusplus
}
#endif
