// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceVideoOut   Modulo: libSceVideoOut   Simbolos: 8

#include "hle_stub.h"

namespace hle::gen::libSceVideoOut {

static constexpr const char* kLibrary = "libSceVideoOut";

// sceVideoOutOpen  [Up36PTk687E]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoOutOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoOutOpen", "Up36PTk687E", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceVideoOutAddFlipEvent  [HXzjK9yI30k]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoOutAddFlipEvent(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoOutAddFlipEvent", "HXzjK9yI30k", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceVideoOutSetBufferAttribute  [i6-sR91Wt-4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoOutSetBufferAttribute(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoOutSetBufferAttribute", "i6-sR91Wt-4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceVideoOutRegisterBuffers  [w3BY+tAEiQY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoOutRegisterBuffers(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoOutRegisterBuffers", "w3BY+tAEiQY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceVideoOutSetFlipRate  [CBiu4mCE1DA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoOutSetFlipRate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoOutSetFlipRate", "CBiu4mCE1DA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceVideoOutSetWindowModeMargins  [MTxxrOCeSig]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoOutSetWindowModeMargins(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoOutSetWindowModeMargins", "MTxxrOCeSig", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceVideoOutColorSettingsSetGamma_  [DYhhWbJSeRg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoOutColorSettingsSetGamma_(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoOutColorSettingsSetGamma_", "DYhhWbJSeRg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceVideoOutAdjustColor_  [pv9CI5VC+R0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoOutAdjustColor_(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoOutAdjustColor_", "pv9CI5VC+R0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"Up36PTk687E", "sceVideoOutOpen", kLibrary, "libSceVideoOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoOutOpen), 0},
    {"HXzjK9yI30k", "sceVideoOutAddFlipEvent", kLibrary, "libSceVideoOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoOutAddFlipEvent), 0},
    {"i6-sR91Wt-4", "sceVideoOutSetBufferAttribute", kLibrary, "libSceVideoOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoOutSetBufferAttribute), 0},
    {"w3BY+tAEiQY", "sceVideoOutRegisterBuffers", kLibrary, "libSceVideoOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoOutRegisterBuffers), 0},
    {"CBiu4mCE1DA", "sceVideoOutSetFlipRate", kLibrary, "libSceVideoOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoOutSetFlipRate), 0},
    {"MTxxrOCeSig", "sceVideoOutSetWindowModeMargins", kLibrary, "libSceVideoOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoOutSetWindowModeMargins), 0},
    {"DYhhWbJSeRg", "sceVideoOutColorSettingsSetGamma_", kLibrary, "libSceVideoOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoOutColorSettingsSetGamma_), 0},
    {"pv9CI5VC+R0", "sceVideoOutAdjustColor_", kLibrary, "libSceVideoOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoOutAdjustColor_), 0},
};
extern const std::size_t kSymbolCount = 8;

} // namespace hle::gen::libSceVideoOut
