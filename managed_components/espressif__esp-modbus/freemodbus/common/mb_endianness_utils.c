/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>

#include "mb_endianness_utils.h"

#define INLINE inline __attribute__((always_inline))

static INLINE int16_t mb_get_int16_generic(int n0, int n1, val_16_arr *psrc)
{
    val_16_arr *pv = psrc;
    union {
        val_16_arr arr;
        int16_t value;
    } bov;
    bov.arr[n0] = (*pv)[MB_BO16_0];
    bov.arr[n1] = (*pv)[MB_BO16_1];
    return (bov.value);
}

static INLINE uint16_t mb_get_uint16_generic(int n0, int n1, val_16_arr *psrc)
{
    val_16_arr *pv = psrc;
    union {
        val_16_arr arr;
        uint16_t value;
    } bov;
    bov.arr[n0] = (*pv)[MB_BO16_0];
    bov.arr[n1] = (*pv)[MB_BO16_1];
    return (bov.value);
}

static INLINE uint16_t mb_set_uint16_generic(int n0, int n1, val_16_arr *pdest, uint16_t val)
{
    val_16_arr *pv = pdest;
    union {
        val_16_arr arr;
        uint16_t value;
    } bov;
    bov.value = val;
    (*pv)[MB_BO16_0] = bov.arr[n0];
    (*pv)[MB_BO16_1] = bov.arr[n1];
    return (*((uint16_t *)pv));
}

static INLINE int16_t mb_set_int16_generic(int n0, int n1, val_16_arr *pdest, int16_t val)
{
    val_16_arr *pv = pdest;
    union {
        val_16_arr arr;
        int16_t value;
    } bov;
    bov.value = val;
    (*pv)[MB_BO16_0] = bov.arr[n0];
    (*pv)[MB_BO16_1] = bov.arr[n1];
    return (*((uint16_t *)pv));
}

static INLINE uint32_t mb_get_uint32_generic(int n0, int n1, int n2, int n3, val_32_arr *psrc)
{
    val_32_arr *pv = psrc;
    union {
        val_32_arr arr;
        uint32_t value;
    } bov;
    bov.arr[n0] = (*pv)[MB_BO32_0];
    bov.arr[n1] = (*pv)[MB_BO32_1];
    bov.arr[n2] = (*pv)[MB_BO32_2];
    bov.arr[n3] = (*pv)[MB_BO32_3];
    return (bov.value);
}

static INLINE int32_t mb_get_int32_generic(int n0, int n1, int n2, int n3, val_32_arr *psrc)
{
    val_32_arr *pv = psrc;
    union {
        val_32_arr arr;
        int32_t value;
    } bov;
    bov.arr[n0] = (*pv)[MB_BO32_0];
    bov.arr[n1] = (*pv)[MB_BO32_1];
    bov.arr[n2] = (*pv)[MB_BO32_2];
    bov.arr[n3] = (*pv)[MB_BO32_3];
    return (bov.value);
}

static INLINE float mb_get_float_generic(int n0, int n1, int n2, int n3, val_32_arr *psrc)
{
    val_32_arr *pv = psrc;
    union {
        val_32_arr arr;
        float value;
    } bov;
    bov.arr[n0] = (*pv)[MB_BO32_0];
    bov.arr[n1] = (*pv)[MB_BO32_1];
    bov.arr[n2] = (*pv)[MB_BO32_2];
    bov.arr[n3] = (*pv)[MB_BO32_3];
    return (bov.value);
}

static INLINE uint32_t mb_set_int32_generic(int n0, int n1, int n2, int n3, val_32_arr *pdest, int32_t val)
{
    val_32_arr *pv = pdest;
    union {
        val_32_arr arr;
        int32_t value;
    } bov;
    bov.value = val;
    (*pv)[MB_BO32_0] = bov.arr[n0];
    (*pv)[MB_BO32_1] = bov.arr[n1];
    (*pv)[MB_BO32_2] = bov.arr[n2];
    (*pv)[MB_BO32_3] = bov.arr[n3];
    return (*((uint32_t *)pv));
}

static INLINE uint32_t mb_set_uint32_generic(int n0, int n1, int n2, int n3, val_32_arr *pdest, uint32_t val)
{
    val_32_arr *pv = pdest;
    union {
        val_32_arr arr;
        uint32_t value;
    } bov;
    bov.value = val;
    (*pv)[MB_BO32_0] = bov.arr[n0];
    (*pv)[MB_BO32_1] = bov.arr[n1];
    (*pv)[MB_BO32_2] = bov.arr[n2];
    (*pv)[MB_BO32_3] = bov.arr[n3];
    return (*((uint32_t *)pv));
}

static INLINE uint32_t mb_set_float_generic(int n0, int n1, int n2, int n3, val_32_arr *pdest, float val)
{
    val_32_arr *pv = pdest;
    union {
        val_32_arr arr;
        float value;
    } bov;
    bov.value = val;
    (*pv)[MB_BO32_0] = bov.arr[n0];
    (*pv)[MB_BO32_1] = bov.arr[n1];
    (*pv)[MB_BO32_2] = bov.arr[n2];
    (*pv)[MB_BO32_3] = bov.arr[n3];
    return (*((uint32_t *)pv));
}

static INLINE int64_t mb_get_int64_generic(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, val_64_arr *psrc)
{
    val_64_arr *pv64 = psrc;
    union {
        val_64_arr arr;
        int64_t value;
    } bo64;
    bo64.arr[n0] = (*pv64)[MB_BO64_0];
    bo64.arr[n1] = (*pv64)[MB_BO64_1];
    bo64.arr[n2] = (*pv64)[MB_BO64_2];
    bo64.arr[n3] = (*pv64)[MB_BO64_3];
    bo64.arr[n4] = (*pv64)[MB_BO64_4];
    bo64.arr[n5] = (*pv64)[MB_BO64_5];
    bo64.arr[n6] = (*pv64)[MB_BO64_6];
    bo64.arr[n7] = (*pv64)[MB_BO64_7];
    return (bo64.value);
}

static INLINE uint64_t mb_get_uint64_generic(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, val_64_arr *psrc)
{
    val_64_arr *pv64 = psrc;
    union {
        val_64_arr arr;
        uint64_t value;
    } bo64;
    bo64.arr[n0] = (*pv64)[MB_BO64_0];
    bo64.arr[n1] = (*pv64)[MB_BO64_1];
    bo64.arr[n2] = (*pv64)[MB_BO64_2];
    bo64.arr[n3] = (*pv64)[MB_BO64_3];
    bo64.arr[n4] = (*pv64)[MB_BO64_4];
    bo64.arr[n5] = (*pv64)[MB_BO64_5];
    bo64.arr[n6] = (*pv64)[MB_BO64_6];
    bo64.arr[n7] = (*pv64)[MB_BO64_7];
    return (bo64.value);
}

static INLINE double mb_get_double_generic(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, val_64_arr *psrc)
{
    val_64_arr *pv64 = psrc;
    union {
        val_64_arr arr;
        double value;
    } bo64;
    bo64.arr[n0] = (*pv64)[MB_BO64_0];
    bo64.arr[n1] = (*pv64)[MB_BO64_1];
    bo64.arr[n2] = (*pv64)[MB_BO64_2];
    bo64.arr[n3] = (*pv64)[MB_BO64_3];
    bo64.arr[n4] = (*pv64)[MB_BO64_4];
    bo64.arr[n5] = (*pv64)[MB_BO64_5];
    bo64.arr[n6] = (*pv64)[MB_BO64_6];
    bo64.arr[n7] = (*pv64)[MB_BO64_7];
    return (bo64.value);
}

static INLINE uint64_t mb_set_int64_generic(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, val_64_arr *pdest, int64_t val)
{
    val_64_arr *pv = pdest;
    union {
        val_64_arr arr;
        int64_t value;
    } bo64;
    bo64.value = val;
    (*pv)[MB_BO64_0] = bo64.arr[n0];
    (*pv)[MB_BO64_1] = bo64.arr[n1];
    (*pv)[MB_BO64_2] = bo64.arr[n2];
    (*pv)[MB_BO64_3] = bo64.arr[n3];
    (*pv)[MB_BO64_4] = bo64.arr[n4];
    (*pv)[MB_BO64_5] = bo64.arr[n5];
    (*pv)[MB_BO64_6] = bo64.arr[n6];
    (*pv)[MB_BO64_7] = bo64.arr[n7];
    return (*((uint64_t *)pv));
}

static INLINE uint64_t mb_set_uint64_generic(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, val_64_arr *pdest, uint64_t val)
{
    val_64_arr *pv = pdest;
    union {
        val_64_arr arr;
        uint64_t value;
    } bo64;
    bo64.value = val;
    (*pv)[MB_BO64_0] = bo64.arr[n0];
    (*pv)[MB_BO64_1] = bo64.arr[n1];
    (*pv)[MB_BO64_2] = bo64.arr[n2];
    (*pv)[MB_BO64_3] = bo64.arr[n3];
    (*pv)[MB_BO64_4] = bo64.arr[n4];
    (*pv)[MB_BO64_5] = bo64.arr[n5];
    (*pv)[MB_BO64_6] = bo64.arr[n6];
    (*pv)[MB_BO64_7] = bo64.arr[n7];
    return (*((uint64_t *)pv));
}

static INLINE uint64_t mb_set_double_generic(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, val_64_arr *pdest, double val)
{
    val_64_arr *pv = pdest;
    union {
        val_64_arr arr;
        double value;
    } bo64;
    bo64.value = val;
    (*pv)[MB_BO64_0] = bo64.arr[n0];
    (*pv)[MB_BO64_1] = bo64.arr[n1];
    (*pv)[MB_BO64_2] = bo64.arr[n2];
    (*pv)[MB_BO64_3] = bo64.arr[n3];
    (*pv)[MB_BO64_4] = bo64.arr[n4];
    (*pv)[MB_BO64_5] = bo64.arr[n5];
    (*pv)[MB_BO64_6] = bo64.arr[n6];
    (*pv)[MB_BO64_7] = bo64.arr[n7];
    return (*((uint64_t *)pv));
}

int8_t mb_get_int8_a(pi16)
val_16_arr *pi16;
{
    return((int8_t)(*pi16)[MB_BO16_0]);
}

uint16_t mb_set_int8_a(pi16, i8)
val_16_arr *pi16;
int8_t i8;
{
    (*pi16)[MB_BO16_0] = (uint8_t)i8;
    (*pi16)[MB_BO16_1] = 0;
    return (*((uint16_t *)pi16));
}

int8_t mb_get_int8_b(pi16)
val_16_arr *pi16;
{
    return((int8_t)(*pi16)[MB_BO16_1]);
}

uint16_t mb_set_int8_b(pi16, i8)
val_16_arr *pi16;
int8_t i8;
{
    (*pi16)[MB_BO16_0] = 0;
    (*pi16)[MB_BO16_1] = (int8_t)i8;
    return (*((uint16_t *)pi16));
}

uint8_t mb_get_uint8_a(pu16)
val_16_arr *pu16;
{
    return((uint8_t)(*pu16)[MB_BO16_0]);
}

uint16_t mb_set_uint8_a(pu16, u8)
val_16_arr *pu16;
uint8_t u8;
{
    (*pu16)[MB_BO16_0] = (uint8_t)u8;
    (*pu16)[MB_BO16_1] = 0;
    return (*((uint16_t *)pu16));
}

uint8_t mb_get_uint8_b(pu16)
val_16_arr *pu16;
{
    return((uint8_t)(*pu16)[MB_BO16_1]);
}

uint16_t mb_set_uint8_b(pu16, u8)
val_16_arr *pu16;
uint8_t u8;
{
    (*pu16)[MB_BO16_0] = 0;
    (*pu16)[MB_BO16_1] = (uint8_t)u8;
    return (*((uint16_t *)pu16));
}

int16_t mb_get_int16_ab(pi16)
val_16_arr *pi16;
{
    return mb_get_int16_generic(0, 1, pi16);
}

uint16_t mb_set_int16_ab(pi16, i16)
val_16_arr *pi16;
int16_t i16;
{
    return mb_set_int16_generic(0, 1, pi16, i16);
}

uint16_t mb_get_uint16_ab(pu16)
val_16_arr *pu16;
{
    return mb_get_uint16_generic(0, 1, pu16);
}

uint16_t mb_set_uint16_ab(pu16, u16)
val_16_arr *pu16;
uint16_t u16;
{
    return mb_set_uint16_generic(0, 1, pu16, u16);
}

int16_t mb_get_int16_ba(pi16)
val_16_arr *pi16;
{
    return mb_get_int16_generic(1, 0, pi16);
}

uint16_t mb_set_int16_ba(pi16, i16)
val_16_arr *pi16;
int16_t i16;
{
    return mb_set_int16_generic(1, 0, pi16, i16);
}

uint16_t mb_get_uint16_ba(pu16)
val_16_arr *pu16;
{
    return mb_get_int16_generic(1, 0, pu16);
}

uint16_t mb_set_uint16_ba(pu16, u16)
val_16_arr *pu16;
uint16_t u16;
{
    return mb_set_int16_generic(1, 0, pu16, u16);
}

int32_t mb_get_int32_abcd(pi32)
val_32_arr *pi32;
{
    return mb_get_int32_generic(0, 1, 2, 3, pi32);
}

uint32_t mb_set_int32_abcd(pi32, i32)
val_32_arr *pi32;
int32_t i32;
{
    return mb_set_int32_generic(0, 1, 2, 3, pi32, i32);
}

uint32_t mb_get_uint32_abcd(pu32)
val_32_arr *pu32;
{
    return mb_get_uint32_generic(0, 1, 2, 3, pu32);
}

uint32_t mb_set_uint32_abcd(pu32, u32)
val_32_arr *pu32;
uint32_t u32;
{
    return mb_set_uint32_generic(0, 1, 2, 3, pu32, u32);
}

int32_t mb_get_int32_badc(pi32)
val_32_arr *pi32;
{
    return mb_get_int32_generic(1, 0, 3, 2, pi32);
}

uint32_t mb_set_int32_badc(pi32, i32)
val_32_arr *pi32;
int32_t i32;
{
    return mb_set_int32_generic(1, 0, 3, 2, pi32, i32);
}

uint32_t mb_get_uint32_badc(pu32)
val_32_arr *pu32;
{
    return mb_get_uint32_generic(1, 0, 3, 2, pu32);
}

uint32_t mb_set_uint32_badc(pu32, u32)
val_32_arr *pu32;
uint32_t u32;
{
    return mb_set_uint32_generic(1, 0, 3, 2, pu32, u32);
}

int32_t mb_get_int32_cdab(pi32)
val_32_arr *pi32;
{
    return mb_get_int32_generic(2, 3, 0, 1, pi32);
}

uint32_t mb_set_int32_cdab(pi32, i32)
val_32_arr *pi32;
int32_t i32;
{
    return mb_set_int32_generic(2, 3, 0, 1, pi32, i32);
}

uint32_t mb_get_uint32_cdab(pu32)
val_32_arr *pu32;
{
    return mb_get_uint32_generic(2, 3, 0, 1, pu32);
}

uint32_t mb_set_uint32_cdab(pu32, u32)
val_32_arr *pu32;
uint32_t u32;
{
    return mb_set_uint32_generic(2, 3, 0, 1, pu32, u32);
}

int32_t mb_get_int32_dcba(pi32)
val_32_arr *pi32;
{
    return mb_get_int32_generic(3, 2, 1, 0, pi32);
}

uint32_t mb_set_int32_dcba(pi32, i32)
val_32_arr *pi32;
int32_t i32;
{
    return mb_set_int32_generic(3, 2, 1, 0, pi32, i32);
}

uint32_t mb_get_uint32_dcba(pu32)
val_32_arr *pu32;
{
    return mb_get_uint32_generic(3, 2, 1, 0, pu32);
}

uint32_t mb_set_uint32_dcba(pu32, u32)
val_32_arr *pu32;
uint32_t u32;
{
    return mb_set_uint32_generic(3, 2, 1, 0, pu32, u32);
}

float mb_get_float_abcd(pf)
val_32_arr *pf;
{
    return mb_get_float_generic(0, 1, 2, 3, pf);
}

uint32_t mb_set_float_abcd(pf, f)
val_32_arr *pf;
float f;
{
    return mb_set_float_generic(0, 1, 2, 3, pf, f);
}

float mb_get_float_badc(pf)
val_32_arr *pf;
{
    return mb_get_float_generic(1, 0, 3, 2, pf);
}

uint32_t mb_set_float_badc(pf, f)
val_32_arr *pf;
float f;
{
    return mb_set_float_generic(1, 0, 3, 2, pf, f);
}

float mb_get_float_cdab(pf)
val_32_arr *pf;
{
    return mb_get_float_generic(2, 3, 0, 1, pf);
}

uint32_t mb_set_float_cdab(pf, f)
val_32_arr *pf;
float f;
{
    return mb_set_float_generic(2, 3, 0, 1, pf, f);
}

float mb_get_float_dcba(pf)
val_32_arr *pf;
{
    return mb_get_float_generic(3, 2, 1, 0, pf);
}

uint32_t mb_set_float_dcba(pf, f)
val_32_arr *pf;
float f;
{
    return mb_set_float_generic(3, 2, 1, 0, pf, f);
}

double mb_get_double_abcdefgh(pd)
val_64_arr *pd;
{
    return mb_get_double_generic(0, 1, 2, 3, 4, 5, 6, 7, pd);
}

uint64_t mb_set_double_abcdefgh(pd, d)
val_64_arr *pd;
double d;
{
    return mb_set_double_generic(0, 1, 2, 3, 4, 5, 6, 7, pd, d);
}

double mb_get_double_hgfedcba(pd)
val_64_arr *pd;
{
    return mb_get_double_generic(7, 6, 5, 4, 3, 2, 1, 0, pd);
}

uint64_t mb_set_double_hgfedcba(pd, d)
val_64_arr *pd;
double d;
{
    return mb_set_double_generic(7, 6, 5, 4, 3, 2, 1, 0, pd, d);
}

double mb_get_double_ghefcdab(pd)
val_64_arr *pd;
{
    return mb_get_double_generic(6, 7, 4, 5, 2, 3, 0, 1, pd);
}

uint64_t mb_set_double_ghefcdab(pd, d)
val_64_arr *pd;
double d;
{
    return mb_set_double_generic(6, 7, 4, 5, 2, 3, 0, 1, pd, d);
}

double mb_get_double_badcfehg(pd)
val_64_arr *pd;
{
    return mb_get_double_generic(1, 0, 3, 2, 5, 4, 7, 6, pd);
}

uint64_t mb_set_double_badcfehg(pd, d)
val_64_arr *pd;
double d;
{
    return mb_set_double_generic(1, 0, 3, 2, 5, 4, 7, 6, pd, d);
}

int64_t mb_get_int64_abcdefgh(pi64)
val_64_arr *pi64;
{
    return mb_get_int64_generic(0, 1, 2, 3, 4, 5, 6, 7, pi64);
}

uint64_t mb_set_int64_abcdefgh(pi, i)
val_64_arr *pi;
int64_t i;
{
    return mb_set_int64_generic(0, 1, 2, 3, 4, 5, 6, 7, pi, i);
}

int64_t mb_get_int64_hgfedcba(pi64)
val_64_arr *pi64;
{
    return mb_get_int64_generic(7, 6, 5, 4, 3, 2, 1, 0, pi64);
}

uint64_t mb_set_int64_hgfedcba(pi, i)
val_64_arr *pi;
int64_t i;
{
    return mb_set_int64_generic(7, 6, 5, 4, 3, 2, 1, 0, pi, i);
}

int64_t mb_get_int64_ghefcdab(pi64)
val_64_arr *pi64;
{
    return mb_get_int64_generic(6, 7, 4, 5, 2, 3, 0, 1, pi64);
}

uint64_t mb_set_int64_ghefcdab(pi, i)
val_64_arr *pi;
int64_t i;
{
    return mb_set_int64_generic(6, 7, 4, 5, 2, 3, 0, 1, pi, i);
}

int64_t mb_get_int64_badcfehg(pi64)
val_64_arr *pi64;
{
    return mb_get_int64_generic(1, 0, 3, 2, 5, 4, 7, 6, pi64);
}

uint64_t mb_set_int64_badcfehg(pi, i)
val_64_arr *pi;
int64_t i;
{
    return mb_set_int64_generic(1, 0, 3, 2, 5, 4, 7, 6, pi, i);
}

uint64_t mb_get_uint64_abcdefgh(pui)
val_64_arr *pui;
{
    return mb_get_uint64_generic(0, 1, 2, 3, 4, 5, 6, 7, pui);
}

uint64_t mb_set_uint64_abcdefgh(pui, ui)
val_64_arr *pui;
uint64_t ui;
{
    return mb_set_uint64_generic(0, 1, 2, 3, 4, 5, 6, 7, pui, ui);
}

uint64_t mb_get_uint64_hgfedcba(pui)
val_64_arr *pui;
{
    return mb_get_uint64_generic(7, 6, 5, 4, 3, 2, 1, 0, pui);
}

uint64_t mb_set_uint64_hgfedcba(pui, ui)
val_64_arr *pui;
uint64_t ui;
{
    return mb_set_uint64_generic(7, 6, 5, 4, 3, 2, 1, 0, pui, ui);
}

uint64_t mb_get_uint64_ghefcdab(pui)
val_64_arr *pui;
{
    return mb_get_uint64_generic(6, 7, 4, 5, 2, 3, 0, 1, pui);
}

uint64_t mb_set_uint64_ghefcdab(pui, ui)
val_64_arr *pui;
uint64_t ui;
{
    return mb_set_uint64_generic(6, 7, 4, 5, 2, 3, 0, 1, pui, ui);
}

uint64_t mb_get_uint64_badcfehg(pui)
val_64_arr *pui;
{
    return mb_get_int64_generic(1, 0, 3, 2, 5, 4, 7, 6, pui);
}

uint64_t mb_set_uint64_badcfehg(pui, ui)
val_64_arr *pui;
uint64_t ui;
{
    return mb_set_uint64_generic(1, 0, 3, 2, 5, 4, 7, 6, pui, ui);
}
