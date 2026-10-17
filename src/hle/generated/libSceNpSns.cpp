// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceNpSns   Modulo: libSceNpManager   Simbolos: 3

#include "hle_stub.h"

namespace hle::gen::libSceNpSns {

static constexpr const char* kLibrary = "libSceNpSns";

// sceNpSnsFacebookCreateRequest  [OoTjfxl8-wI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpSnsFacebookCreateRequest(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpSnsFacebookCreateRequest", "OoTjfxl8-wI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpSnsFacebookDeleteRequest  [pTvAKV1iQkE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpSnsFacebookDeleteRequest(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpSnsFacebookDeleteRequest", "pTvAKV1iQkE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpSnsFacebookGetAccessToken  [VtyS8XLBqNE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpSnsFacebookGetAccessToken(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpSnsFacebookGetAccessToken", "VtyS8XLBqNE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"OoTjfxl8-wI", "sceNpSnsFacebookCreateRequest", kLibrary, "libSceNpManager", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpSnsFacebookCreateRequest), 0},
    {"pTvAKV1iQkE", "sceNpSnsFacebookDeleteRequest", kLibrary, "libSceNpManager", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpSnsFacebookDeleteRequest), 0},
    {"VtyS8XLBqNE", "sceNpSnsFacebookGetAccessToken", kLibrary, "libSceNpManager", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpSnsFacebookGetAccessToken), 0},
};
extern const std::size_t kSymbolCount = 3;

} // namespace hle::gen::libSceNpSns
