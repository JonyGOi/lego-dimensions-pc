// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceAppContent   Modulo: libSceAppContentUtil   Simbolos: 10

#include "hle_stub.h"

namespace hle::gen::libSceAppContent {

static constexpr const char* kLibrary = "libSceAppContent";

// sceAppContentInitialize  [R9lA82OraNs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentInitialize", "R9lA82OraNs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentAppParamGetInt  [99b82IKXpH4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentAppParamGetInt(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentAppParamGetInt", "99b82IKXpH4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentAddcontUnmount  [3rHWaV-1KC4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentAddcontUnmount(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentAddcontUnmount", "3rHWaV-1KC4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentAddcontDelete  [ZiATpP9gEkA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentAddcontDelete(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentAddcontDelete", "ZiATpP9gEkA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentAddcontEnqueueDownload  [7gxh+5QubhY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentAddcontEnqueueDownload(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentAddcontEnqueueDownload", "7gxh+5QubhY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentGetAddcontInfoList  [xnd8BJzAxmk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentGetAddcontInfoList(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentGetAddcontInfoList", "xnd8BJzAxmk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentGetEntitlementKey  [XTWR0UXvcgs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentGetEntitlementKey(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentGetEntitlementKey", "XTWR0UXvcgs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentAddcontMount  [VANhIWcqYak]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentAddcontMount(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentAddcontMount", "VANhIWcqYak", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentGetAddcontInfo  [m47juOmH0VE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentGetAddcontInfo(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentGetAddcontInfo", "m47juOmH0VE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAppContentTemporaryDataMount2  [buYbeLOGWmA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAppContentTemporaryDataMount2(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAppContentTemporaryDataMount2", "buYbeLOGWmA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"R9lA82OraNs", "sceAppContentInitialize", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentInitialize), 0},
    {"99b82IKXpH4", "sceAppContentAppParamGetInt", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentAppParamGetInt), 0},
    {"3rHWaV-1KC4", "sceAppContentAddcontUnmount", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentAddcontUnmount), 0},
    {"ZiATpP9gEkA", "sceAppContentAddcontDelete", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentAddcontDelete), 0},
    {"7gxh+5QubhY", "sceAppContentAddcontEnqueueDownload", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentAddcontEnqueueDownload), 0},
    {"xnd8BJzAxmk", "sceAppContentGetAddcontInfoList", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentGetAddcontInfoList), 0},
    {"XTWR0UXvcgs", "sceAppContentGetEntitlementKey", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentGetEntitlementKey), 0},
    {"VANhIWcqYak", "sceAppContentAddcontMount", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentAddcontMount), 0},
    {"m47juOmH0VE", "sceAppContentGetAddcontInfo", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentGetAddcontInfo), 0},
    {"buYbeLOGWmA", "sceAppContentTemporaryDataMount2", kLibrary, "libSceAppContentUtil", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAppContentTemporaryDataMount2), 0},
};
extern const std::size_t kSymbolCount = 10;

} // namespace hle::gen::libSceAppContent
