// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libScePosix   Modulo: libkernel   Simbolos: 1

#include "hle_stub.h"

namespace hle::gen::libScePosix {

static constexpr const char* kLibrary = "libScePosix";

// clock_gettime  [lLMT9vJAck0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_clock_gettime(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "clock_gettime", "lLMT9vJAck0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"lLMT9vJAck0", "clock_gettime", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_clock_gettime), 0},
};
extern const std::size_t kSymbolCount = 1;

} // namespace hle::gen::libScePosix
