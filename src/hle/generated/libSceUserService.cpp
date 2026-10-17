// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceUserService   Modulo: libSceUserService   Simbolos: 7

#include "hle_stub.h"

namespace hle::gen::libSceUserService {

static constexpr const char* kLibrary = "libSceUserService";

// sceUserServiceGetEvent  [yH17Q6NWtVg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUserServiceGetEvent(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUserServiceGetEvent", "yH17Q6NWtVg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUserServiceInitialize  [j3YMu1MVNNo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUserServiceInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUserServiceInitialize", "j3YMu1MVNNo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUserServiceTerminate  [bwFjS+bX9mA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUserServiceTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUserServiceTerminate", "bwFjS+bX9mA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUserServiceGetLoginUserIdList  [fPhymKNvK-A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUserServiceGetLoginUserIdList(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUserServiceGetLoginUserIdList", "fPhymKNvK-A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUserServiceGetUserName  [1xxcMiGu2fo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUserServiceGetUserName(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUserServiceGetUserName", "1xxcMiGu2fo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUserServiceGetUserColor  [lUoqwTQu4Go]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUserServiceGetUserColor(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUserServiceGetUserColor", "lUoqwTQu4Go", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUserServiceGetInitialUser  [CdWp0oHWGr0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUserServiceGetInitialUser(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUserServiceGetInitialUser", "CdWp0oHWGr0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"yH17Q6NWtVg", "sceUserServiceGetEvent", kLibrary, "libSceUserService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUserServiceGetEvent), 0},
    {"j3YMu1MVNNo", "sceUserServiceInitialize", kLibrary, "libSceUserService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUserServiceInitialize), 0},
    {"bwFjS+bX9mA", "sceUserServiceTerminate", kLibrary, "libSceUserService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUserServiceTerminate), 0},
    {"fPhymKNvK-A", "sceUserServiceGetLoginUserIdList", kLibrary, "libSceUserService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUserServiceGetLoginUserIdList), 0},
    {"1xxcMiGu2fo", "sceUserServiceGetUserName", kLibrary, "libSceUserService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUserServiceGetUserName), 0},
    {"lUoqwTQu4Go", "sceUserServiceGetUserColor", kLibrary, "libSceUserService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUserServiceGetUserColor), 0},
    {"CdWp0oHWGr0", "sceUserServiceGetInitialUser", kLibrary, "libSceUserService", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUserServiceGetInitialUser), 0},
};
extern const std::size_t kSymbolCount = 7;

} // namespace hle::gen::libSceUserService
