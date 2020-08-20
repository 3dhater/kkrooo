// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_TYPES_H__
#define __KKROO_TYPES_H__

#include <cstdint>

using s8  = int8_t;
using u8  = uint8_t;
using s16 = int16_t;
using u16 = uint16_t;
using s32 = int32_t;
using u32 = uint32_t;
using s64 = int64_t;
using u64 = uint64_t;

using f32   = float;
using f64   = double;

#ifdef KK_BIT_64
using kkAddressType = u64;
#else
using kkAddressType = u32;
#endif

#define KK_X _f32[0]
#define KK_Y _f32[1]
#define KK_Z _f32[2]
#define KK_W _f32[3]

#endif