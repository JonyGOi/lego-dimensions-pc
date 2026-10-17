// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.
#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

#if defined(_MSC_VER)
#include <intrin.h>
#define HLE_RETURN_ADDRESS() (reinterpret_cast<::hle::u64>(_ReturnAddress()))
#define HLE_NOINLINE __declspec(noinline)
#else
#define HLE_RETURN_ADDRESS() (reinterpret_cast<::hle::u64>(__builtin_return_address(0)))
#define HLE_NOINLINE __attribute__((noinline))
#endif

// O codigo do jogo chama com a ABI System V (RDI, RSI, RDX, RCX, R8, R9), mesmo no Windows.
// Exige Clang/clang-cl ou GCC: o MSVC puro nao tem sysv_abi.
#define HLE_SYSV_ABI __attribute__((sysv_abi))

namespace hle {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

enum class SymbolKind : u8 {
    Function, // stub chamavel
    Object,   // dado exportado (buffer zerado do tamanho original)
};

struct HleSymbol {
    std::string_view nid;
    std::string_view name;    // nome resolvido; igual ao NID quando desconhecido
    std::string_view library; // DT_SCE_IMPORT_LIB
    std::string_view module;  // DT_SCE_NEEDED_MODULE (ja sem sufixo de debug)
    SymbolKind kind;
    void* address;
    u32 size;                // so para Object
    bool overridden = false; // true quando src/hle/overrides/ trocou o stub por uma implementacao
};

struct StubCall {
    const char* library;
    const char* name;
    const char* nid;
    u64 return_address;
    u64 rdi, rsi, rdx, rcx, r8, r9;
};

using StubLogger = void (*)(const StubCall&);

// Troca o destino do log dos stubs (padrao: stderr). nullptr restaura o padrao.
void SetStubLogger(StubLogger logger);
void LogStubCall(const StubCall& call);

} // namespace hle
