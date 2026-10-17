// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceCoredump   Modulo: libkernel   Simbolos: 2

#include "hle_stub.h"

namespace hle::gen::libSceCoredump {

static constexpr const char* kLibrary = "libSceCoredump";

// sceCoredumpRegisterCoredumpHandler  [8zLSfEfW5AU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceCoredumpRegisterCoredumpHandler(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceCoredumpRegisterCoredumpHandler", "8zLSfEfW5AU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceCoredumpWriteUserData  [Dbbkj6YHWdo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceCoredumpWriteUserData(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceCoredumpWriteUserData", "Dbbkj6YHWdo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"8zLSfEfW5AU", "sceCoredumpRegisterCoredumpHandler", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceCoredumpRegisterCoredumpHandler), 0},
    {"Dbbkj6YHWdo", "sceCoredumpWriteUserData", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceCoredumpWriteUserData), 0},
};
extern const std::size_t kSymbolCount = 2;

} // namespace hle::gen::libSceCoredump
