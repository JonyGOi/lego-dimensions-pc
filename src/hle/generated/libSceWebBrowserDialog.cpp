// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceWebBrowserDialog   Modulo: libSceWebBrowserDialog   Simbolos: 4

#include "hle_stub.h"

namespace hle::gen::libSceWebBrowserDialog {

static constexpr const char* kLibrary = "libSceWebBrowserDialog";

// sceWebBrowserDialogUpdateStatus  [h1dR-t5ISgg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceWebBrowserDialogUpdateStatus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceWebBrowserDialogUpdateStatus", "h1dR-t5ISgg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceWebBrowserDialogTerminate  [ocHtyBwHfys]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceWebBrowserDialogTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceWebBrowserDialogTerminate", "ocHtyBwHfys", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceWebBrowserDialogInitialize  [jqb7HntFQFc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceWebBrowserDialogInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceWebBrowserDialogInitialize", "jqb7HntFQFc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceWebBrowserDialogOpen  [FraP7debcdg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceWebBrowserDialogOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceWebBrowserDialogOpen", "FraP7debcdg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"h1dR-t5ISgg", "sceWebBrowserDialogUpdateStatus", kLibrary, "libSceWebBrowserDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceWebBrowserDialogUpdateStatus), 0},
    {"ocHtyBwHfys", "sceWebBrowserDialogTerminate", kLibrary, "libSceWebBrowserDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceWebBrowserDialogTerminate), 0},
    {"jqb7HntFQFc", "sceWebBrowserDialogInitialize", kLibrary, "libSceWebBrowserDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceWebBrowserDialogInitialize), 0},
    {"FraP7debcdg", "sceWebBrowserDialogOpen", kLibrary, "libSceWebBrowserDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceWebBrowserDialogOpen), 0},
};
extern const std::size_t kSymbolCount = 4;

} // namespace hle::gen::libSceWebBrowserDialog
