// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceNpAuth   Modulo: libSceNpAuth   Simbolos: 3

#include "hle_stub.h"

namespace hle::gen::libSceNpAuth {

static constexpr const char* kLibrary = "libSceNpAuth";

// sceNpAuthCreateRequest  [6bwFkosYRQg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpAuthCreateRequest(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpAuthCreateRequest", "6bwFkosYRQg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpAuthDeleteRequest  [H8wG9Bk-nPc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpAuthDeleteRequest(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpAuthDeleteRequest", "H8wG9Bk-nPc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpAuthGetAuthorizationCode  [KxGkOrQJTqY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpAuthGetAuthorizationCode(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpAuthGetAuthorizationCode", "KxGkOrQJTqY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"6bwFkosYRQg", "sceNpAuthCreateRequest", kLibrary, "libSceNpAuth", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpAuthCreateRequest), 0},
    {"H8wG9Bk-nPc", "sceNpAuthDeleteRequest", kLibrary, "libSceNpAuth", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpAuthDeleteRequest), 0},
    {"KxGkOrQJTqY", "sceNpAuthGetAuthorizationCode", kLibrary, "libSceNpAuth", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpAuthGetAuthorizationCode), 0},
};
extern const std::size_t kSymbolCount = 3;

} // namespace hle::gen::libSceNpAuth
