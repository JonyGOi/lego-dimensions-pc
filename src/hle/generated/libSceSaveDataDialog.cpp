// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceSaveDataDialog   Modulo: libSceSaveDataDialog   Simbolos: 7

#include "hle_stub.h"

namespace hle::gen::libSceSaveDataDialog {

static constexpr const char* kLibrary = "libSceSaveDataDialog";

// sceSaveDataDialogTerminate  [YuH2FA7azqQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDialogTerminate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDialogTerminate", "YuH2FA7azqQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataDialogUpdateStatus  [KK3Bdg1RWK0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDialogUpdateStatus(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDialogUpdateStatus", "KK3Bdg1RWK0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataDialogClose  [fH46Lag88XY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDialogClose(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDialogClose", "fH46Lag88XY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataDialogGetResult  [yEiJ-qqr6Cg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDialogGetResult(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDialogGetResult", "yEiJ-qqr6Cg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataDialogInitialize  [s9e3+YpRnzw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDialogInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDialogInitialize", "s9e3+YpRnzw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataDialogOpen  [4tPhsP6FpDI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDialogOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDialogOpen", "4tPhsP6FpDI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataDialogProgressBarSetValue  [hay1CfTmLyA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDialogProgressBarSetValue(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDialogProgressBarSetValue", "hay1CfTmLyA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"YuH2FA7azqQ", "sceSaveDataDialogTerminate", kLibrary, "libSceSaveDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDialogTerminate), 0},
    {"KK3Bdg1RWK0", "sceSaveDataDialogUpdateStatus", kLibrary, "libSceSaveDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDialogUpdateStatus), 0},
    {"fH46Lag88XY", "sceSaveDataDialogClose", kLibrary, "libSceSaveDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDialogClose), 0},
    {"yEiJ-qqr6Cg", "sceSaveDataDialogGetResult", kLibrary, "libSceSaveDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDialogGetResult), 0},
    {"s9e3+YpRnzw", "sceSaveDataDialogInitialize", kLibrary, "libSceSaveDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDialogInitialize), 0},
    {"4tPhsP6FpDI", "sceSaveDataDialogOpen", kLibrary, "libSceSaveDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDialogOpen), 0},
    {"hay1CfTmLyA", "sceSaveDataDialogProgressBarSetValue", kLibrary, "libSceSaveDataDialog", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDialogProgressBarSetValue), 0},
};
extern const std::size_t kSymbolCount = 7;

} // namespace hle::gen::libSceSaveDataDialog
