// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceGameCustomDataDialog   Modulo: libGameCustomDataDialog   Simbolos: 5

#include "hle_stub.h"

namespace hle::gen::libSceGameCustomDataDialog {

static constexpr const char* kLibrary = "libSceGameCustomDataDialog";

// sceGameCustomDataDialogGetResult  [sJptZwvs1is]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceGameCustomDataDialogGetResult(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceGameCustomDataDialogGetResult", "sJptZwvs1is", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceGameCustomDataDialogInitialize  [xtbb-2f703A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceGameCustomDataDialogInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceGameCustomDataDialogInitialize", "xtbb-2f703A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceGameCustomDataDialogOpen  [5TvttyRuU84]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceGameCustomDataDialogOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceGameCustomDataDialogOpen", "5TvttyRuU84", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceGameCustomDataDialogTerminate  [HwEtHFCpU5U]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceGameCustomDataDialogTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceGameCustomDataDialogTerminate", "HwEtHFCpU5U", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceGameCustomDataDialogUpdateStatus  [PkdLsRA4ON0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceGameCustomDataDialogUpdateStatus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceGameCustomDataDialogUpdateStatus", "PkdLsRA4ON0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"sJptZwvs1is", "sceGameCustomDataDialogGetResult", kLibrary, "libGameCustomDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceGameCustomDataDialogGetResult), 0},
    {"xtbb-2f703A", "sceGameCustomDataDialogInitialize", kLibrary, "libGameCustomDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceGameCustomDataDialogInitialize), 0},
    {"5TvttyRuU84", "sceGameCustomDataDialogOpen", kLibrary, "libGameCustomDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceGameCustomDataDialogOpen), 0},
    {"HwEtHFCpU5U", "sceGameCustomDataDialogTerminate", kLibrary, "libGameCustomDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceGameCustomDataDialogTerminate), 0},
    {"PkdLsRA4ON0", "sceGameCustomDataDialogUpdateStatus", kLibrary, "libGameCustomDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceGameCustomDataDialogUpdateStatus), 0},
};
extern const std::size_t kSymbolCount = 5;

} // namespace hle::gen::libSceGameCustomDataDialog
