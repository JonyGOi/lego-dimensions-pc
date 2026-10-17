// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceGameLiveStreaming   Modulo: libSceGameLiveStreaming   Simbolos: 1

#include "hle_stub.h"

namespace hle::gen::libSceGameLiveStreaming {

static constexpr const char* kLibrary = "libSceGameLiveStreaming";

// sceGameLiveStreamingPermitServerSideRecording  [-EHnU68gExU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceGameLiveStreamingPermitServerSideRecording(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceGameLiveStreamingPermitServerSideRecording", "-EHnU68gExU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"-EHnU68gExU", "sceGameLiveStreamingPermitServerSideRecording", kLibrary, "libSceGameLiveStreaming", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceGameLiveStreamingPermitServerSideRecording), 0},
};
extern const std::size_t kSymbolCount = 1;

} // namespace hle::gen::libSceGameLiveStreaming
