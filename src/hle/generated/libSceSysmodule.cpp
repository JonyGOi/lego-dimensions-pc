// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceSysmodule   Modulo: libSceSysmodule   Simbolos: 3

#include "hle_stub.h"

namespace hle::gen::libSceSysmodule {

static constexpr const char* kLibrary = "libSceSysmodule";

// sceSysmoduleIsLoaded  [fMP5NHUOaMk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSysmoduleIsLoaded(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSysmoduleIsLoaded", "fMP5NHUOaMk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSysmoduleLoadModule  [g8cM39EUZ6o]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSysmoduleLoadModule(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSysmoduleLoadModule", "g8cM39EUZ6o", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSysmoduleUnloadModule  [eR2bZFAAU0Q]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSysmoduleUnloadModule(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSysmoduleUnloadModule", "eR2bZFAAU0Q", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"fMP5NHUOaMk", "sceSysmoduleIsLoaded", kLibrary, "libSceSysmodule", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSysmoduleIsLoaded), 0},
    {"g8cM39EUZ6o", "sceSysmoduleLoadModule", kLibrary, "libSceSysmodule", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSysmoduleLoadModule), 0},
    {"eR2bZFAAU0Q", "sceSysmoduleUnloadModule", kLibrary, "libSceSysmodule", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSysmoduleUnloadModule), 0},
};
extern const std::size_t kSymbolCount = 3;

} // namespace hle::gen::libSceSysmodule
