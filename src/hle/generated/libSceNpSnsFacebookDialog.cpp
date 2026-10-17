// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceNpSnsFacebookDialog   Modulo: libSceNpSnsFacebookDialog   Simbolos: 5

#include "hle_stub.h"

namespace hle::gen::libSceNpSnsFacebookDialog {

static constexpr const char* kLibrary = "libSceNpSnsFacebookDialog";

// sceNpSnsFacebookDialogGetResult  [AifdcI1aKas]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpSnsFacebookDialogGetResult(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpSnsFacebookDialogGetResult", "AifdcI1aKas", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpSnsFacebookDialogInitialize  [tdQArULbNQk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpSnsFacebookDialogInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpSnsFacebookDialogInitialize", "tdQArULbNQk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpSnsFacebookDialogOpen  [Hi3onqHSN6M]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpSnsFacebookDialogOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpSnsFacebookDialogOpen", "Hi3onqHSN6M", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpSnsFacebookDialogTerminate  [cdqOFyupRS4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpSnsFacebookDialogTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpSnsFacebookDialogTerminate", "cdqOFyupRS4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpSnsFacebookDialogUpdateStatus  [fjV7C8H0Y8k]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpSnsFacebookDialogUpdateStatus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpSnsFacebookDialogUpdateStatus", "fjV7C8H0Y8k", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"AifdcI1aKas", "sceNpSnsFacebookDialogGetResult", kLibrary, "libSceNpSnsFacebookDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpSnsFacebookDialogGetResult), 0},
    {"tdQArULbNQk", "sceNpSnsFacebookDialogInitialize", kLibrary, "libSceNpSnsFacebookDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpSnsFacebookDialogInitialize), 0},
    {"Hi3onqHSN6M", "sceNpSnsFacebookDialogOpen", kLibrary, "libSceNpSnsFacebookDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpSnsFacebookDialogOpen), 0},
    {"cdqOFyupRS4", "sceNpSnsFacebookDialogTerminate", kLibrary, "libSceNpSnsFacebookDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpSnsFacebookDialogTerminate), 0},
    {"fjV7C8H0Y8k", "sceNpSnsFacebookDialogUpdateStatus", kLibrary, "libSceNpSnsFacebookDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpSnsFacebookDialogUpdateStatus), 0},
};
extern const std::size_t kSymbolCount = 5;

} // namespace hle::gen::libSceNpSnsFacebookDialog
