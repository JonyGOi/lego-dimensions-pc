// SPDX-License-Identifier: GPL-2.0-or-later
// LEGO Dimensions PC: o minimo de common/types.h e common/endian.h do shadPS4 que o extrator de PKG usa.
#pragma once

#include <cstdint>
#include <cstdlib>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

// Valor guardado em big-endian (como no arquivo); le como numero normal.
template <typename T>
struct BigEndian {
    T raw;

    static T Swap(T v) {
        if constexpr (sizeof(T) == 2) {
            return static_cast<T>(_byteswap_ushort(static_cast<u16>(v)));
        } else if constexpr (sizeof(T) == 4) {
            return static_cast<T>(_byteswap_ulong(static_cast<unsigned long>(v)));
        } else {
            return static_cast<T>(_byteswap_uint64(static_cast<u64>(v)));
        }
    }
    operator T() const {
        return Swap(raw);
    }
};

using u16_be = BigEndian<u16>;
using u32_be = BigEndian<u32>;
using u64_be = BigEndian<u64>;
