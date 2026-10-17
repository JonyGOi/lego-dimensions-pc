#pragma once

#include <filesystem>

#include "common/types.h"
#include "hle/hle_log.h"
#include "hle_stub.h"

// Implementacoes reais que substituem os stubs de src/hle/generated/ sem editar os arquivos
// gerados: cada HLE_OVERRIDE registra (nome do simbolo, funcao) em uma lista estatica, e
// InstallOverrides() troca os ponteiros da tabela gerada antes de o loader relocar o eboot.
//
//     HLE_IMPL(int, sceKernelUsleep, u32 micros) {
//         HLE_QUIET("%u", micros);
//         ...
//     }
//     HLE_OVERRIDE(sceKernelUsleep);
namespace hle {

struct OverrideRegistrar {
    OverrideRegistrar(const char* name, void* function);

    const char* name;
    void* function;
    OverrideRegistrar* next;
};

struct OverrideStats {
    u32 installed = 0;
    u32 not_imported = 0; // override escrito para um simbolo que o eboot nao importa
};

OverrideStats InstallOverrides();

// Preenche os objetos de dados exportados (libc: _Stdin/_Stdout/_Stderr, Need_sceLibc;
// libkernel: __stack_chk_guard) nos buffers da tabela gerada.
void InitDataObjects();

} // namespace hle

#define HLE_IMPL(ret, name, ...) HLE_NOINLINE ret HLE_SYSV_ABI impl_##name(__VA_ARGS__)

#define HLE_OVERRIDE(name)                                                                         \
    static ::hle::OverrideRegistrar hle_override_##name{#name, reinterpret_cast<void*>(&impl_##name)}

// Codigos de erro do Orbis (0x80020000 | errno do FreeBSD)
namespace hle::err {
constexpr int OK = 0;
constexpr int kEPERM = static_cast<int>(0x80020001);
constexpr int kENOMEM = static_cast<int>(0x8002000C);
constexpr int kEACCES = static_cast<int>(0x8002000D);
constexpr int kEBUSY = static_cast<int>(0x80020010);
constexpr int kEDEADLK = static_cast<int>(0x8002000B);
constexpr int kEINVAL = static_cast<int>(0x80020016);
constexpr int kEAGAIN = static_cast<int>(0x80020023);
constexpr int kETIMEDOUT = static_cast<int>(0x8002003C);
} // namespace hle::err
