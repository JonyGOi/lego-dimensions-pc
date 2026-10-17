#pragma once

#include <cstddef>
#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

constexpr u64 operator""_MiB(unsigned long long v) {
    return v << 20;
}

constexpr u64 AlignUp(u64 value, u64 align) {
    return (value + align - 1) & ~(align - 1);
}

constexpr u64 AlignDown(u64 value, u64 align) {
    return value & ~(align - 1);
}
