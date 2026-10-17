// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceHttp   Modulo: libSceHttp   Simbolos: 17

#include "hle_stub.h"

namespace hle::gen::libSceHttp {

static constexpr const char* kLibrary = "libSceHttp";

// sceHttpInit  [A9cVMUtEp4Y]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpInit", "A9cVMUtEp4Y", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpTerm  [Ik-KpLTlf7Q]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpTerm(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpTerm", "Ik-KpLTlf7Q", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpDeleteRequest  [qe7oZ+v4PWA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpDeleteRequest(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpDeleteRequest", "qe7oZ+v4PWA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpDeleteConnection  [P6A3ytpsiYc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpDeleteConnection(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpDeleteConnection", "P6A3ytpsiYc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpDeleteTemplate  [4I8vEpuEhZ8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpDeleteTemplate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpDeleteTemplate", "4I8vEpuEhZ8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpCreateTemplate  [0gYjPTR-6cY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpCreateTemplate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpCreateTemplate", "0gYjPTR-6cY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpsDisableOption  [mSQCxzWTwVI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpsDisableOption(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpsDisableOption", "mSQCxzWTwVI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpsSetSslCallback  [htyBOoWeS58]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpsSetSslCallback(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpsSetSslCallback", "htyBOoWeS58", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpAddRequestHeader  [EY28T2bkN7k]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpAddRequestHeader(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpAddRequestHeader", "EY28T2bkN7k", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpCreateConnectionWithURL  [qgxDBjorUxs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpCreateConnectionWithURL(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpCreateConnectionWithURL", "qgxDBjorUxs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpCreateRequestWithURL2  [Cnp77podkCU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpCreateRequestWithURL2(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpCreateRequestWithURL2", "Cnp77podkCU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpSendRequest  [1e2BNwI-XzE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpSendRequest(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpSendRequest", "1e2BNwI-XzE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpGetStatusCode  [0a2TBNfE3BU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpGetStatusCode(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpGetStatusCode", "0a2TBNfE3BU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpGetResponseContentLength  [yuO2H2Uvnos]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpGetResponseContentLength(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpGetResponseContentLength", "yuO2H2Uvnos", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpReadData  [P5pdoykPYTk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpReadData(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpReadData", "P5pdoykPYTk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpCreateRequestWithURL  [Aeu5wVKkF9w]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpCreateRequestWithURL(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpCreateRequestWithURL", "Aeu5wVKkF9w", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceHttpUriEscape  [YuOW3dDAKYc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceHttpUriEscape(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceHttpUriEscape", "YuOW3dDAKYc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"A9cVMUtEp4Y", "sceHttpInit", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpInit), 0},
    {"Ik-KpLTlf7Q", "sceHttpTerm", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpTerm), 0},
    {"qe7oZ+v4PWA", "sceHttpDeleteRequest", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpDeleteRequest), 0},
    {"P6A3ytpsiYc", "sceHttpDeleteConnection", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpDeleteConnection), 0},
    {"4I8vEpuEhZ8", "sceHttpDeleteTemplate", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpDeleteTemplate), 0},
    {"0gYjPTR-6cY", "sceHttpCreateTemplate", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpCreateTemplate), 0},
    {"mSQCxzWTwVI", "sceHttpsDisableOption", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpsDisableOption), 0},
    {"htyBOoWeS58", "sceHttpsSetSslCallback", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpsSetSslCallback), 0},
    {"EY28T2bkN7k", "sceHttpAddRequestHeader", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpAddRequestHeader), 0},
    {"qgxDBjorUxs", "sceHttpCreateConnectionWithURL", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpCreateConnectionWithURL), 0},
    {"Cnp77podkCU", "sceHttpCreateRequestWithURL2", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpCreateRequestWithURL2), 0},
    {"1e2BNwI-XzE", "sceHttpSendRequest", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpSendRequest), 0},
    {"0a2TBNfE3BU", "sceHttpGetStatusCode", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpGetStatusCode), 0},
    {"yuO2H2Uvnos", "sceHttpGetResponseContentLength", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpGetResponseContentLength), 0},
    {"P5pdoykPYTk", "sceHttpReadData", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpReadData), 0},
    {"Aeu5wVKkF9w", "sceHttpCreateRequestWithURL", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpCreateRequestWithURL), 0},
    {"YuOW3dDAKYc", "sceHttpUriEscape", kLibrary, "libSceHttp", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceHttpUriEscape), 0},
};
extern const std::size_t kSymbolCount = 17;

} // namespace hle::gen::libSceHttp
