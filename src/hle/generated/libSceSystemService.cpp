// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceSystemService   Modulo: libSceSystemService   Simbolos: 5

#include "hle_stub.h"

namespace hle::gen::libSceSystemService {

static constexpr const char* kLibrary = "libSceSystemService";

// sceSystemServiceParamGetInt  [fZo48un7LK4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSystemServiceParamGetInt(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSystemServiceParamGetInt", "fZo48un7LK4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSystemServiceGetDisplaySafeAreaInfo  [1n37q1Bvc5Y]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSystemServiceGetDisplaySafeAreaInfo(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSystemServiceGetDisplaySafeAreaInfo", "1n37q1Bvc5Y", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSystemServiceHideSplashScreen  [Vo5V8KAwCmk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSystemServiceHideSplashScreen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSystemServiceHideSplashScreen", "Vo5V8KAwCmk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSystemServiceGetStatus  [rPo6tV8D9bM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSystemServiceGetStatus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSystemServiceGetStatus", "rPo6tV8D9bM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSystemServiceReceiveEvent  [656LMQSrg6U]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSystemServiceReceiveEvent(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSystemServiceReceiveEvent", "656LMQSrg6U", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"fZo48un7LK4", "sceSystemServiceParamGetInt", kLibrary, "libSceSystemService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSystemServiceParamGetInt), 0},
    {"1n37q1Bvc5Y", "sceSystemServiceGetDisplaySafeAreaInfo", kLibrary, "libSceSystemService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSystemServiceGetDisplaySafeAreaInfo), 0},
    {"Vo5V8KAwCmk", "sceSystemServiceHideSplashScreen", kLibrary, "libSceSystemService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSystemServiceHideSplashScreen), 0},
    {"rPo6tV8D9bM", "sceSystemServiceGetStatus", kLibrary, "libSceSystemService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSystemServiceGetStatus), 0},
    {"656LMQSrg6U", "sceSystemServiceReceiveEvent", kLibrary, "libSceSystemService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSystemServiceReceiveEvent), 0},
};
extern const std::size_t kSymbolCount = 5;

} // namespace hle::gen::libSceSystemService
