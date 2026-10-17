// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceNetCtl   Modulo: libSceNetCtl   Simbolos: 9

#include "hle_stub.h"

namespace hle::gen::libSceNetCtl {

static constexpr const char* kLibrary = "libSceNetCtl";

// sceNetCtlInit  [gky0+oaNM4k]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlInit", "gky0+oaNM4k", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetCtlCheckCallback  [iQw3iQPhvUQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlCheckCallback(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlCheckCallback", "iQw3iQPhvUQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetCtlGetResult  [0cBgduPRR+M]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlGetResult(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlGetResult", "0cBgduPRR+M", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetCtlGetNatInfo  [JO4yuTuMoKI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlGetNatInfo(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlGetNatInfo", "JO4yuTuMoKI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetCtlGetInfo  [obuxdTiwkF8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlGetInfo(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlGetInfo", "obuxdTiwkF8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetCtlGetState  [uBPlr0lbuiI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlGetState(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlGetState", "uBPlr0lbuiI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetCtlRegisterCallback  [UJ+Z7Q+4ck0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlRegisterCallback(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlRegisterCallback", "UJ+Z7Q+4ck0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetCtlTerm  [Z4wwCFiBELQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlTerm(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlTerm", "Z4wwCFiBELQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetCtlUnregisterCallback  [Rqm2OnZMCz0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetCtlUnregisterCallback(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetCtlUnregisterCallback", "Rqm2OnZMCz0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"gky0+oaNM4k", "sceNetCtlInit", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlInit), 0},
    {"iQw3iQPhvUQ", "sceNetCtlCheckCallback", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlCheckCallback), 0},
    {"0cBgduPRR+M", "sceNetCtlGetResult", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlGetResult), 0},
    {"JO4yuTuMoKI", "sceNetCtlGetNatInfo", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlGetNatInfo), 0},
    {"obuxdTiwkF8", "sceNetCtlGetInfo", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlGetInfo), 0},
    {"uBPlr0lbuiI", "sceNetCtlGetState", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlGetState), 0},
    {"UJ+Z7Q+4ck0", "sceNetCtlRegisterCallback", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlRegisterCallback), 0},
    {"Z4wwCFiBELQ", "sceNetCtlTerm", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlTerm), 0},
    {"Rqm2OnZMCz0", "sceNetCtlUnregisterCallback", kLibrary, "libSceNetCtl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetCtlUnregisterCallback), 0},
};
extern const std::size_t kSymbolCount = 9;

} // namespace hle::gen::libSceNetCtl
