// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceMsgDialog   Modulo: libSceMsgDialog   Simbolos: 4

#include "hle_stub.h"

namespace hle::gen::libSceMsgDialog {

static constexpr const char* kLibrary = "libSceMsgDialog";

// sceMsgDialogInitialize  [lDqxaY1UbEo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceMsgDialogInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceMsgDialogInitialize", "lDqxaY1UbEo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceMsgDialogOpen  [b06Hh0DPEaE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceMsgDialogOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceMsgDialogOpen", "b06Hh0DPEaE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceMsgDialogUpdateStatus  [6fIC3XKt2k0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceMsgDialogUpdateStatus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceMsgDialogUpdateStatus", "6fIC3XKt2k0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceMsgDialogTerminate  [ePw-kqZmelo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceMsgDialogTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceMsgDialogTerminate", "ePw-kqZmelo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"lDqxaY1UbEo", "sceMsgDialogInitialize", kLibrary, "libSceMsgDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceMsgDialogInitialize), 0},
    {"b06Hh0DPEaE", "sceMsgDialogOpen", kLibrary, "libSceMsgDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceMsgDialogOpen), 0},
    {"6fIC3XKt2k0", "sceMsgDialogUpdateStatus", kLibrary, "libSceMsgDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceMsgDialogUpdateStatus), 0},
    {"ePw-kqZmelo", "sceMsgDialogTerminate", kLibrary, "libSceMsgDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceMsgDialogTerminate), 0},
};
extern const std::size_t kSymbolCount = 4;

} // namespace hle::gen::libSceMsgDialog
