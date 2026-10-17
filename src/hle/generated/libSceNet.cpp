// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceNet   Modulo: libSceNet   Simbolos: 21

#include "hle_stub.h"

namespace hle::gen::libSceNet {

static constexpr const char* kLibrary = "libSceNet";

// sceNetResolverCreate  [C4UgDHHPvdw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetResolverCreate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetResolverCreate", "C4UgDHHPvdw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetResolverStartNtoa  [Nd91WaWmG2w]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetResolverStartNtoa(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetResolverStartNtoa", "Nd91WaWmG2w", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetResolverDestroy  [kJlYH5uMAWI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetResolverDestroy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetResolverDestroy", "kJlYH5uMAWI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetInit  [Nlev7Lg8k3A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetInit", "Nlev7Lg8k3A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetSend  [beRjXBn-z+o]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetSend(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetSend", "beRjXBn-z+o", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetRecv  [9wO9XrMsNhc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetRecv(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetRecv", "9wO9XrMsNhc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetGetpeername  [TCkRD0DWNLg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetGetpeername(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetGetpeername", "TCkRD0DWNLg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetSocket  [Q4qBuN-c0ZM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetSocket(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetSocket", "Q4qBuN-c0ZM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetSetsockopt  [2mKX2Spso7I]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetSetsockopt(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetSetsockopt", "2mKX2Spso7I", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetShutdown  [TSM6whtekok]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetShutdown(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetShutdown", "TSM6whtekok", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetSocketClose  [45ggEzakPJQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetSocketClose(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetSocketClose", "45ggEzakPJQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetBind  [bErx49PgxyY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetBind(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetBind", "bErx49PgxyY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetListen  [kOj1HiAGE54]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetListen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetListen", "kOj1HiAGE54", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetAccept  [PIWqhn9oSxc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetAccept(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetAccept", "PIWqhn9oSxc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetInetPton  [8Kcp5d-q1Uo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetInetPton(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetInetPton", "8Kcp5d-q1Uo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetConnect  [OXXX4mUk3uk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetConnect(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetConnect", "OXXX4mUk3uk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetSendto  [gvD1greCu0A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetSendto(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetSendto", "gvD1greCu0A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetRecvfrom  [304ooNZxWDY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetRecvfrom(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetRecvfrom", "304ooNZxWDY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetPoolCreate  [dgJBaeJnGpo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetPoolCreate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetPoolCreate", "dgJBaeJnGpo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetPoolDestroy  [K7RlrTkI-mw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetPoolDestroy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetPoolDestroy", "K7RlrTkI-mw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceNetTerm  [cTGkc6-TBlI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceNetTerm(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceNetTerm", "cTGkc6-TBlI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"C4UgDHHPvdw", "sceNetResolverCreate", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetResolverCreate), 0},
    {"Nd91WaWmG2w", "sceNetResolverStartNtoa", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetResolverStartNtoa), 0},
    {"kJlYH5uMAWI", "sceNetResolverDestroy", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetResolverDestroy), 0},
    {"Nlev7Lg8k3A", "sceNetInit", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetInit), 0},
    {"beRjXBn-z+o", "sceNetSend", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetSend), 0},
    {"9wO9XrMsNhc", "sceNetRecv", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetRecv), 0},
    {"TCkRD0DWNLg", "sceNetGetpeername", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetGetpeername), 0},
    {"Q4qBuN-c0ZM", "sceNetSocket", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetSocket), 0},
    {"2mKX2Spso7I", "sceNetSetsockopt", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetSetsockopt), 0},
    {"TSM6whtekok", "sceNetShutdown", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetShutdown), 0},
    {"45ggEzakPJQ", "sceNetSocketClose", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetSocketClose), 0},
    {"bErx49PgxyY", "sceNetBind", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetBind), 0},
    {"kOj1HiAGE54", "sceNetListen", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetListen), 0},
    {"PIWqhn9oSxc", "sceNetAccept", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetAccept), 0},
    {"8Kcp5d-q1Uo", "sceNetInetPton", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetInetPton), 0},
    {"OXXX4mUk3uk", "sceNetConnect", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetConnect), 0},
    {"gvD1greCu0A", "sceNetSendto", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetSendto), 0},
    {"304ooNZxWDY", "sceNetRecvfrom", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetRecvfrom), 0},
    {"dgJBaeJnGpo", "sceNetPoolCreate", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetPoolCreate), 0},
    {"K7RlrTkI-mw", "sceNetPoolDestroy", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetPoolDestroy), 0},
    {"cTGkc6-TBlI", "sceNetTerm", kLibrary, "libSceNet", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceNetTerm), 0},
};
extern const std::size_t kSymbolCount = 21;

} // namespace hle::gen::libSceNet
