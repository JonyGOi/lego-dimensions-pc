// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceRemoteplay   Modulo: libSceRemoteplay   Simbolos: 2

#include "hle_stub.h"

namespace hle::gen::libSceRemoteplay {

static constexpr const char* kLibrary = "libSceRemoteplay";

// sceRemoteplayInitialize  [k1SwgkMSOM8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRemoteplayInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRemoteplayInitialize", "k1SwgkMSOM8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceRemoteplayProhibit  [mrNh78tBpmg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRemoteplayProhibit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRemoteplayProhibit", "mrNh78tBpmg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"k1SwgkMSOM8", "sceRemoteplayInitialize", kLibrary, "libSceRemoteplay", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRemoteplayInitialize), 0},
    {"mrNh78tBpmg", "sceRemoteplayProhibit", kLibrary, "libSceRemoteplay", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRemoteplayProhibit), 0},
};
extern const std::size_t kSymbolCount = 2;

} // namespace hle::gen::libSceRemoteplay
