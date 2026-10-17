// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libScePlayGo   Modulo: libScePlayGo   Simbolos: 7

#include "hle_stub.h"

namespace hle::gen::libScePlayGo {

static constexpr const char* kLibrary = "libScePlayGo";

// scePlayGoInitialize  [ts6GlZOKRrE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePlayGoInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePlayGoInitialize", "ts6GlZOKRrE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePlayGoOpen  [M1Gma1ocrGE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePlayGoOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePlayGoOpen", "M1Gma1ocrGE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePlayGoClose  [Uco1I0dlDi8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePlayGoClose(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePlayGoClose", "Uco1I0dlDi8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePlayGoTerminate  [MPe0EeBGM-E]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePlayGoTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePlayGoTerminate", "MPe0EeBGM-E", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePlayGoGetLocus  [uWIYLFkkwqk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePlayGoGetLocus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePlayGoGetLocus", "uWIYLFkkwqk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePlayGoPrefetch  [-Q1-u1a7p0g]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePlayGoPrefetch(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePlayGoPrefetch", "-Q1-u1a7p0g", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePlayGoGetProgress  [-RJWNMK3fC8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePlayGoGetProgress(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePlayGoGetProgress", "-RJWNMK3fC8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"ts6GlZOKRrE", "scePlayGoInitialize", kLibrary, "libScePlayGo", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePlayGoInitialize), 0},
    {"M1Gma1ocrGE", "scePlayGoOpen", kLibrary, "libScePlayGo", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePlayGoOpen), 0},
    {"Uco1I0dlDi8", "scePlayGoClose", kLibrary, "libScePlayGo", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePlayGoClose), 0},
    {"MPe0EeBGM-E", "scePlayGoTerminate", kLibrary, "libScePlayGo", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePlayGoTerminate), 0},
    {"uWIYLFkkwqk", "scePlayGoGetLocus", kLibrary, "libScePlayGo", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePlayGoGetLocus), 0},
    {"-Q1-u1a7p0g", "scePlayGoPrefetch", kLibrary, "libScePlayGo", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePlayGoPrefetch), 0},
    {"-RJWNMK3fC8", "scePlayGoGetProgress", kLibrary, "libScePlayGo", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePlayGoGetProgress), 0},
};
extern const std::size_t kSymbolCount = 7;

} // namespace hle::gen::libScePlayGo
