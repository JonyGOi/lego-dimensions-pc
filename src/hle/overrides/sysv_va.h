#pragma once

#include <cmath>
#include <cstring>

#include <intrin.h>

#include "common/types.h"

// Argumentos variadicos na ABI System V dentro de um binario Windows.
//
// O Clang nao aceita va_start em funcoes __attribute__((sysv_abi)) quando o alvo e Windows
// ("'va_start' used in System V ABI function") e nao tem __builtin_sysv_va_list. Entao as
// funcoes variadicas do jogo (printf, sscanf...) sao declaradas SEM "...": recebem os 6
// registradores inteiros e os 8 XMM como parametros comuns (HLE_VA_PARAMS), e os argumentos
// que sobraram na pilha sao lidos logo acima do endereco de retorno. Com isso monta-se um va_list
// identico ao do System V, que tambem e o formato que o jogo passa para vsnprintf/vprintf.
namespace hle {

struct SysVVaList {
    u32 gp_offset;
    u32 fp_offset;
    u8* overflow_arg_area;
    u8* reg_save_area;
};

struct VaCapture {
    alignas(16) u64 save[6 + 8 * 2]; // 6 GPR + 8 XMM (16 bytes cada)
    SysVVaList list;
};

inline u64 VaNextInt(SysVVaList& v) {
    u64 value;
    if (v.gp_offset < 48) {
        std::memcpy(&value, v.reg_save_area + v.gp_offset, 8);
        v.gp_offset += 8;
    } else {
        std::memcpy(&value, v.overflow_arg_area, 8);
        v.overflow_arg_area += 8;
    }
    return value;
}

inline double VaNextDouble(SysVVaList& v) {
    double value;
    if (v.fp_offset < 176) {
        std::memcpy(&value, v.reg_save_area + v.fp_offset, 8);
        v.fp_offset += 16;
    } else {
        std::memcpy(&value, v.overflow_arg_area, 8);
        v.overflow_arg_area += 8;
    }
    return value;
}

// long double do System V: x87 de 80 bits, sempre na pilha, alinhado em 16.
inline double VaNextLongDouble(SysVVaList& v) {
    v.overflow_arg_area = reinterpret_cast<u8*>(AlignUp(reinterpret_cast<u64>(v.overflow_arg_area), 16));
    u64 mantissa;
    u16 sign_exp;
    std::memcpy(&mantissa, v.overflow_arg_area, 8);
    std::memcpy(&sign_exp, v.overflow_arg_area + 8, 2);
    v.overflow_arg_area += 16;
    const int exponent = (sign_exp & 0x7FFF) - 16383 - 63;
    const double value = std::ldexp(static_cast<double>(mantissa), exponent);
    return (sign_exp & 0x8000) ? -value : value;
}

} // namespace hle

#define HLE_VA_PARAMS                                                                              \
    ::u64 rdi, ::u64 rsi, ::u64 rdx, ::u64 rcx, ::u64 r8, ::u64 r9, double xmm0, double xmm1,      \
        double xmm2, double xmm3, double xmm4, double xmm5, double xmm6, double xmm7

// `fixed_gp` = quantos parametros inteiros nomeados a funcao original tem (ex.: printf = 1).
// Tem de ser expandida no corpo da propria funcao sysv: usa o endereco de retorno dela.
#define HLE_VA_CAPTURE(ctx, fixed_gp)                                                              \
    ::hle::VaCapture ctx;                                                                          \
    do {                                                                                           \
        const ::u64 hle_gp_[6] = {rdi, rsi, rdx, rcx, r8, r9};                                     \
        const double hle_fp_[8] = {xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7};                \
        std::memcpy(ctx.save, hle_gp_, sizeof(hle_gp_));                                           \
        for (int hle_i_ = 0; hle_i_ < 8; ++hle_i_) {                                               \
            std::memcpy(&ctx.save[6 + hle_i_ * 2], &hle_fp_[hle_i_], 8);                           \
        }                                                                                          \
        ctx.list.gp_offset = (fixed_gp) * 8;                                                       \
        ctx.list.fp_offset = 48;                                                                   \
        /* O primeiro argumento de pilha fica logo acima do endereco de retorno. Nao serve */      \
        /* __builtin_frame_address: em alvo Win64 o LLVM nao garante rbp no topo do frame. */      \
        ctx.list.overflow_arg_area = static_cast<::u8*>(_AddressOfReturnAddress()) + 8;            \
        ctx.list.reg_save_area = reinterpret_cast<::u8*>(ctx.save);                                \
    } while (0)
