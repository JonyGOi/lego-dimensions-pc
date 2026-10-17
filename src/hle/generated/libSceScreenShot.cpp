// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceScreenShot   Modulo: libSceScreenShot   Simbolos: 2

#include "hle_stub.h"

namespace hle::gen::libSceScreenShot {

static constexpr const char* kLibrary = "libSceScreenShot";

// sceScreenShotEnable  [2xxUtuC-RzE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceScreenShotEnable(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceScreenShotEnable", "2xxUtuC-RzE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceScreenShotDisable  [tIYf0W5VTi8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceScreenShotDisable(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceScreenShotDisable", "tIYf0W5VTi8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"2xxUtuC-RzE", "sceScreenShotEnable", kLibrary, "libSceScreenShot", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceScreenShotEnable), 0},
    {"tIYf0W5VTi8", "sceScreenShotDisable", kLibrary, "libSceScreenShot", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceScreenShotDisable), 0},
};
extern const std::size_t kSymbolCount = 2;

} // namespace hle::gen::libSceScreenShot
