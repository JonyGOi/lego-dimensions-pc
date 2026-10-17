// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceSaveData   Modulo: libSceSaveData   Simbolos: 9

#include "hle_stub.h"

namespace hle::gen::libSceSaveData {

static constexpr const char* kLibrary = "libSceSaveData";

// sceSaveDataInitialize  [ZkZhskCPXFw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataInitialize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataInitialize", "ZkZhskCPXFw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataDirNameSearch  [dyIhnXq-0SM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDirNameSearch(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDirNameSearch", "dyIhnXq-0SM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataUmount  [BMR4F-Uek3E]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataUmount(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataUmount", "BMR4F-Uek3E", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataGetMountInfo  [65VH0Qaaz6s]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataGetMountInfo(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataGetMountInfo", "65VH0Qaaz6s", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataGetParam  [XgvSuIdnMlw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataGetParam(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataGetParam", "XgvSuIdnMlw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataSetParam  [85zul--eGXs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataSetParam(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataSetParam", "85zul--eGXs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataMount  [32HQAQdwM2o]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataMount(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataMount", "32HQAQdwM2o", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataDelete  [S1GkePI17zQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataDelete(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataDelete", "S1GkePI17zQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSaveDataSaveIcon  [c88Yy54Mx0w]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSaveDataSaveIcon(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSaveDataSaveIcon", "c88Yy54Mx0w", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"ZkZhskCPXFw", "sceSaveDataInitialize", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataInitialize), 0},
    {"dyIhnXq-0SM", "sceSaveDataDirNameSearch", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDirNameSearch), 0},
    {"BMR4F-Uek3E", "sceSaveDataUmount", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataUmount), 0},
    {"65VH0Qaaz6s", "sceSaveDataGetMountInfo", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataGetMountInfo), 0},
    {"XgvSuIdnMlw", "sceSaveDataGetParam", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataGetParam), 0},
    {"85zul--eGXs", "sceSaveDataSetParam", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataSetParam), 0},
    {"32HQAQdwM2o", "sceSaveDataMount", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataMount), 0},
    {"S1GkePI17zQ", "sceSaveDataDelete", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataDelete), 0},
    {"c88Yy54Mx0w", "sceSaveDataSaveIcon", kLibrary, "libSceSaveData", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSaveDataSaveIcon), 0},
};
extern const std::size_t kSymbolCount = 9;

} // namespace hle::gen::libSceSaveData
