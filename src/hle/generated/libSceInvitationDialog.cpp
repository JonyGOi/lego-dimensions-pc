// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceInvitationDialog   Modulo: libSceInvitationDialog   Simbolos: 5

#include "hle_stub.h"

namespace hle::gen::libSceInvitationDialog {

static constexpr const char* kLibrary = "libSceInvitationDialog";

// sceInvitationDialogGetResult  [8XKR6wa64iQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceInvitationDialogGetResult(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceInvitationDialogGetResult", "8XKR6wa64iQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceInvitationDialogInitialize  [XvA5KS56wcs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceInvitationDialogInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceInvitationDialogInitialize", "XvA5KS56wcs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceInvitationDialogOpen  [0zU0G+wiVLA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceInvitationDialogOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceInvitationDialogOpen", "0zU0G+wiVLA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceInvitationDialogTerminate  [B6HVJtDYxEE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceInvitationDialogTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceInvitationDialogTerminate", "B6HVJtDYxEE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceInvitationDialogUpdateStatus  [9+g9iOq+7kg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceInvitationDialogUpdateStatus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceInvitationDialogUpdateStatus", "9+g9iOq+7kg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"8XKR6wa64iQ", "sceInvitationDialogGetResult", kLibrary, "libSceInvitationDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceInvitationDialogGetResult), 0},
    {"XvA5KS56wcs", "sceInvitationDialogInitialize", kLibrary, "libSceInvitationDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceInvitationDialogInitialize), 0},
    {"0zU0G+wiVLA", "sceInvitationDialogOpen", kLibrary, "libSceInvitationDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceInvitationDialogOpen), 0},
    {"B6HVJtDYxEE", "sceInvitationDialogTerminate", kLibrary, "libSceInvitationDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceInvitationDialogTerminate), 0},
    {"9+g9iOq+7kg", "sceInvitationDialogUpdateStatus", kLibrary, "libSceInvitationDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceInvitationDialogUpdateStatus), 0},
};
extern const std::size_t kSymbolCount = 5;

} // namespace hle::gen::libSceInvitationDialog
