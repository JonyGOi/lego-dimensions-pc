// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libScePad   Modulo: libScePad   Simbolos: 7

#include "hle_stub.h"

namespace hle::gen::libScePad {

static constexpr const char* kLibrary = "libScePad";

// scePadOpen  [xk0AcarP3V4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePadOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePadOpen", "xk0AcarP3V4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePadClose  [6ncge5+l5Qs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePadClose(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePadClose", "6ncge5+l5Qs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePadReadState  [YndgXqQVV7c]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePadReadState(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePadReadState", "YndgXqQVV7c", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePadSetLightBar  [RR4novUEENY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePadSetLightBar(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePadSetLightBar", "RR4novUEENY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePadResetLightBar  [DscD1i9HX1w]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePadResetLightBar(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePadResetLightBar", "DscD1i9HX1w", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePadSetVibration  [yFVnOdGxvZY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePadSetVibration(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePadSetVibration", "yFVnOdGxvZY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePadInit  [hv1luiJrqQM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePadInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePadInit", "hv1luiJrqQM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"xk0AcarP3V4", "scePadOpen", kLibrary, "libScePad", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePadOpen), 0},
    {"6ncge5+l5Qs", "scePadClose", kLibrary, "libScePad", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePadClose), 0},
    {"YndgXqQVV7c", "scePadReadState", kLibrary, "libScePad", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePadReadState), 0},
    {"RR4novUEENY", "scePadSetLightBar", kLibrary, "libScePad", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePadSetLightBar), 0},
    {"DscD1i9HX1w", "scePadResetLightBar", kLibrary, "libScePad", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePadResetLightBar), 0},
    {"yFVnOdGxvZY", "scePadSetVibration", kLibrary, "libScePad", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePadSetVibration), 0},
    {"hv1luiJrqQM", "scePadInit", kLibrary, "libScePad", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePadInit), 0},
};
extern const std::size_t kSymbolCount = 7;

} // namespace hle::gen::libScePad
