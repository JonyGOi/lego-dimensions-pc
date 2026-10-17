// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceNpCommon   Modulo: libSceNpCommon   Simbolos: 1

#include "hle_stub.h"

namespace hle::gen::libSceNpCommon {

static constexpr const char* kLibrary = "libSceNpCommon";

// sceNpCmpNpId  [i8UmXTSq7N4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpCmpNpId(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpCmpNpId", "i8UmXTSq7N4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"i8UmXTSq7N4", "sceNpCmpNpId", kLibrary, "libSceNpCommon", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpCmpNpId), 0},
};
extern const std::size_t kSymbolCount = 1;

} // namespace hle::gen::libSceNpCommon
