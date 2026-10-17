// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceRtc   Modulo: libSceRtc   Simbolos: 8

#include "hle_stub.h"

namespace hle::gen::libSceRtc {

static constexpr const char* kLibrary = "libSceRtc";

// sceRtcGetCurrentClockLocalTime  [ZPD1YOKI+Kw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRtcGetCurrentClockLocalTime(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRtcGetCurrentClockLocalTime", "ZPD1YOKI+Kw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceRtcGetCurrentClock  [8lfvnRMqwEM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRtcGetCurrentClock(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRtcGetCurrentClock", "8lfvnRMqwEM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceRtcSetTime_t  [bDEVVP4bTjQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRtcSetTime_t(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRtcSetTime_t", "bDEVVP4bTjQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceRtcSetTick  [ueega6v3GUw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRtcSetTick(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRtcSetTick", "ueega6v3GUw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceRtcGetCurrentNetworkTick  [zO9UL3qIINQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRtcGetCurrentNetworkTick(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRtcGetCurrentNetworkTick", "zO9UL3qIINQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceRtcGetCurrentTick  [18B2NS1y9UU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRtcGetCurrentTick(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRtcGetCurrentTick", "18B2NS1y9UU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceRtcParseDateTime  [NxEI1KByvCI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRtcParseDateTime(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRtcParseDateTime", "NxEI1KByvCI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceRtcParseRFC3339  [99bMGglFW3I]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceRtcParseRFC3339(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceRtcParseRFC3339", "99bMGglFW3I", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"ZPD1YOKI+Kw", "sceRtcGetCurrentClockLocalTime", kLibrary, "libSceRtc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRtcGetCurrentClockLocalTime), 0},
    {"8lfvnRMqwEM", "sceRtcGetCurrentClock", kLibrary, "libSceRtc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRtcGetCurrentClock), 0},
    {"bDEVVP4bTjQ", "sceRtcSetTime_t", kLibrary, "libSceRtc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRtcSetTime_t), 0},
    {"ueega6v3GUw", "sceRtcSetTick", kLibrary, "libSceRtc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRtcSetTick), 0},
    {"zO9UL3qIINQ", "sceRtcGetCurrentNetworkTick", kLibrary, "libSceRtc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRtcGetCurrentNetworkTick), 0},
    {"18B2NS1y9UU", "sceRtcGetCurrentTick", kLibrary, "libSceRtc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRtcGetCurrentTick), 0},
    {"NxEI1KByvCI", "sceRtcParseDateTime", kLibrary, "libSceRtc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRtcParseDateTime), 0},
    {"99bMGglFW3I", "sceRtcParseRFC3339", kLibrary, "libSceRtc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceRtcParseRFC3339), 0},
};
extern const std::size_t kSymbolCount = 8;

} // namespace hle::gen::libSceRtc
