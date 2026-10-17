// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceSsl   Modulo: libSceSsl   Simbolos: 3

#include "hle_stub.h"

namespace hle::gen::libSceSsl {

static constexpr const char* kLibrary = "libSceSsl";

// sceSslInit  [hdpVEUDFW3s]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSslInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSslInit", "hdpVEUDFW3s", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSslTerm  [0K1yQ6Lv-Yc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSslTerm(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSslTerm", "0K1yQ6Lv-Yc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceSslGetSerialNumber  [DOwXL+FQMEY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceSslGetSerialNumber(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceSslGetSerialNumber", "DOwXL+FQMEY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"hdpVEUDFW3s", "sceSslInit", kLibrary, "libSceSsl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSslInit), 0},
    {"0K1yQ6Lv-Yc", "sceSslTerm", kLibrary, "libSceSsl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSslTerm), 0},
    {"DOwXL+FQMEY", "sceSslGetSerialNumber", kLibrary, "libSceSsl", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceSslGetSerialNumber), 0},
};
extern const std::size_t kSymbolCount = 3;

} // namespace hle::gen::libSceSsl
