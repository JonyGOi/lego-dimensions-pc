// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceVideoRecording   Modulo: libSceVideoRecording   Simbolos: 1

#include "hle_stub.h"

namespace hle::gen::libSceVideoRecording {

static constexpr const char* kLibrary = "libSceVideoRecording";

// sceVideoRecordingSetInfo  [Fc8qxlKINYQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceVideoRecordingSetInfo(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceVideoRecordingSetInfo", "Fc8qxlKINYQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"Fc8qxlKINYQ", "sceVideoRecordingSetInfo", kLibrary, "libSceVideoRecording", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceVideoRecordingSetInfo), 0},
};
extern const std::size_t kSymbolCount = 1;

} // namespace hle::gen::libSceVideoRecording
