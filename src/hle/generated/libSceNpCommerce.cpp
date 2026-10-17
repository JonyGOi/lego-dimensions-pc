// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceNpCommerce   Modulo: libSceNpCommerce   Simbolos: 5

#include "hle_stub.h"

namespace hle::gen::libSceNpCommerce {

static constexpr const char* kLibrary = "libSceNpCommerce";

// sceNpCommerceDialogGetResult  [r42bWcQbtZY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpCommerceDialogGetResult(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpCommerceDialogGetResult", "r42bWcQbtZY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpCommerceDialogInitialize  [0aR2aWmQal4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpCommerceDialogInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpCommerceDialogInitialize", "0aR2aWmQal4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpCommerceDialogOpen  [DfSCDRA3EjY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpCommerceDialogOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpCommerceDialogOpen", "DfSCDRA3EjY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpCommerceDialogTerminate  [m-I92Ab50W8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpCommerceDialogTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpCommerceDialogTerminate", "m-I92Ab50W8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNpCommerceDialogUpdateStatus  [LR5cwFMMCVE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNpCommerceDialogUpdateStatus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNpCommerceDialogUpdateStatus", "LR5cwFMMCVE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"r42bWcQbtZY", "sceNpCommerceDialogGetResult", kLibrary, "libSceNpCommerce", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpCommerceDialogGetResult), 0},
    {"0aR2aWmQal4", "sceNpCommerceDialogInitialize", kLibrary, "libSceNpCommerce", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpCommerceDialogInitialize), 0},
    {"DfSCDRA3EjY", "sceNpCommerceDialogOpen", kLibrary, "libSceNpCommerce", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpCommerceDialogOpen), 0},
    {"m-I92Ab50W8", "sceNpCommerceDialogTerminate", kLibrary, "libSceNpCommerce", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpCommerceDialogTerminate), 0},
    {"LR5cwFMMCVE", "sceNpCommerceDialogUpdateStatus", kLibrary, "libSceNpCommerce", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNpCommerceDialogUpdateStatus), 0},
};
extern const std::size_t kSymbolCount = 5;

} // namespace hle::gen::libSceNpCommerce
