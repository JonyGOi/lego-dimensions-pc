// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceNgs2   Modulo: libSceNgs2   Simbolos: 10

#include "hle_stub.h"

namespace hle::gen::libSceNgs2 {

static constexpr const char* kLibrary = "libSceNgs2";

// sceNgs2RackCreateWithAllocator  [U546k6orxQo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2RackCreateWithAllocator(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2RackCreateWithAllocator", "U546k6orxQo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2RackGetVoiceHandle  [MwmHz8pAdAo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2RackGetVoiceHandle(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2RackGetVoiceHandle", "MwmHz8pAdAo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2VoiceControl  [uu94irFOGpA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2VoiceControl(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2VoiceControl", "uu94irFOGpA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2RackDestroy  [lCqD7oycmIM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2RackDestroy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2RackDestroy", "lCqD7oycmIM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2SystemQueryBufferSize  [pgFAiLR5qT4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2SystemQueryBufferSize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2SystemQueryBufferSize", "pgFAiLR5qT4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2SystemCreate  [koBbCMvOKWw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2SystemCreate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2SystemCreate", "koBbCMvOKWw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2RackQueryBufferSize  [0eFLVCfWVds]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2RackQueryBufferSize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2RackQueryBufferSize", "0eFLVCfWVds", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2RackCreate  [cLV4aiT9JpA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2RackCreate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2RackCreate", "cLV4aiT9JpA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2SystemRender  [i0VnXM-C9fc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2SystemRender(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2SystemRender", "i0VnXM-C9fc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNgs2VoiceGetState  [-TOuuAQ-buE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNgs2VoiceGetState(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNgs2VoiceGetState", "-TOuuAQ-buE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"U546k6orxQo", "sceNgs2RackCreateWithAllocator", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2RackCreateWithAllocator), 0},
    {"MwmHz8pAdAo", "sceNgs2RackGetVoiceHandle", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2RackGetVoiceHandle), 0},
    {"uu94irFOGpA", "sceNgs2VoiceControl", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2VoiceControl), 0},
    {"lCqD7oycmIM", "sceNgs2RackDestroy", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2RackDestroy), 0},
    {"pgFAiLR5qT4", "sceNgs2SystemQueryBufferSize", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2SystemQueryBufferSize), 0},
    {"koBbCMvOKWw", "sceNgs2SystemCreate", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2SystemCreate), 0},
    {"0eFLVCfWVds", "sceNgs2RackQueryBufferSize", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2RackQueryBufferSize), 0},
    {"cLV4aiT9JpA", "sceNgs2RackCreate", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2RackCreate), 0},
    {"i0VnXM-C9fc", "sceNgs2SystemRender", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2SystemRender), 0},
    {"-TOuuAQ-buE", "sceNgs2VoiceGetState", kLibrary, "libSceNgs2", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNgs2VoiceGetState), 0},
};
extern const std::size_t kSymbolCount = 10;

} // namespace hle::gen::libSceNgs2
