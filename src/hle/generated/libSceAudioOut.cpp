// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceAudioOut   Modulo: libSceAudioOut   Simbolos: 3

#include "hle_stub.h"

namespace hle::gen::libSceAudioOut {

static constexpr const char* kLibrary = "libSceAudioOut";

// sceAudioOutInit  [JfEPXVxhFqA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAudioOutInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAudioOutInit", "JfEPXVxhFqA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAudioOutOpen  [ekNvsT22rsY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAudioOutOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAudioOutOpen", "ekNvsT22rsY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceAudioOutOutput  [QOQtbeDqsT4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceAudioOutOutput(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceAudioOutOutput", "QOQtbeDqsT4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"JfEPXVxhFqA", "sceAudioOutInit", kLibrary, "libSceAudioOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAudioOutInit), 0},
    {"ekNvsT22rsY", "sceAudioOutOpen", kLibrary, "libSceAudioOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAudioOutOpen), 0},
    {"QOQtbeDqsT4", "sceAudioOutOutput", kLibrary, "libSceAudioOut", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceAudioOutOutput), 0},
};
extern const std::size_t kSymbolCount = 3;

} // namespace hle::gen::libSceAudioOut
