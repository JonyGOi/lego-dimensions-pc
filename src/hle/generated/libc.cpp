// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libc   Modulo: libc   Simbolos: 113

#include "hle_stub.h"

namespace hle::gen::libc {

static constexpr const char* kLibrary = "libc";

// _FLog  [jjjRS7l1MPM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__FLog(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_FLog", "jjjRS7l1MPM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Sin  [cCXjU72Z0Ow]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Sin(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Sin", "cCXjU72Z0Ow", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// tan  [T7uyNqP7vQA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_tan(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "tan", "T7uyNqP7vQA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// atan  [OXmauLdQ8kY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_atan(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "atan", "OXmauLdQ8kY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// asin  [7Ly52zaL44Q]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_asin(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "asin", "7Ly52zaL44Q", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// atan2  [HUbZmOnT-Dg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_atan2(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "atan2", "HUbZmOnT-Dg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// acosf  [QI-x0SL8jhw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_acosf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "acosf", "QI-x0SL8jhw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// atan2f  [EH-x713A99c]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_atan2f(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "atan2f", "EH-x713A99c", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// tanf  [ZE6RNL+eLbk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_tanf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "tanf", "ZE6RNL+eLbk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// memcmp  [DfivPArhucg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_memcmp(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "memcmp", "DfivPArhucg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strcpy  [kiZSXIWd9vg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strcpy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strcpy", "kiZSXIWd9vg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// memcpy  [Q3VBxCXhUHs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_memcpy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "memcpy", "Q3VBxCXhUHs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// memset  [8zTFvBIAIN8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_memset(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "memset", "8zTFvBIAIN8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Znam  [hdm0YfMa7TQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Znam(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Znam", "hdm0YfMa7TQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// __cxa_guard_acquire  [3GPpjQdAMTw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___cxa_guard_acquire(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__cxa_guard_acquire", "3GPpjQdAMTw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// __cxa_atexit  [tsvEmnenz48]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___cxa_atexit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__cxa_atexit", "tsvEmnenz48", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// __cxa_guard_release  [9rAeANT2tyE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___cxa_guard_release(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__cxa_guard_release", "9rAeANT2tyE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strlen  [j4ViWNHEgww]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strlen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strlen", "j4ViWNHEgww", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strcat  [Ls4tzzhimqQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strcat(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strcat", "Ls4tzzhimqQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sprintf  [tcVi5SivF7Q]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sprintf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sprintf", "tcVi5SivF7Q", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strcmp  [Ovb2dSJOAuE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strcmp(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strcmp", "Ovb2dSJOAuE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZdlPv  [z+P+xCnWLBk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZdlPv(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZdlPv", "z+P+xCnWLBk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// expf  [8zsu04XNsZ4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_expf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "expf", "8zsu04XNsZ4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strrchr  [9yDWMxEFdJU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strrchr(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strrchr", "9yDWMxEFdJU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Znwm  [fJnpuVVBbKk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Znwm(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Znwm", "fJnpuVVBbKk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// memmove  [+P6FRGH4LfA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_memmove(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "memmove", "+P6FRGH4LfA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strstr  [viiwFMaNamA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strstr(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strstr", "viiwFMaNamA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strncmp  [aesyjrHVWy4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strncmp(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strncmp", "aesyjrHVWy4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _FSin  [ZtjspkJQ+vw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__FSin(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_FSin", "ZtjspkJQ+vw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZdaPv  [MLWl90SFWNE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZdaPv(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZdaPv", "MLWl90SFWNE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _FDtest  [mnufPlYbnN0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__FDtest(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_FDtest", "mnufPlYbnN0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// snprintf  [eLdDw6l0-bU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_snprintf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "snprintf", "eLdDw6l0-bU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// vsnprintf  [Q2V+iqvjgC0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_vsnprintf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "vsnprintf", "Q2V+iqvjgC0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// __cxa_pure_virtual  [zr094EQ39Ww]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___cxa_pure_virtual(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__cxa_pure_virtual", "zr094EQ39Ww", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _init_env  [bzQExy189ZI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__init_env(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_init_env", "bzQExy189ZI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// atexit  [8G2LB+A3rzg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_atexit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "atexit", "8G2LB+A3rzg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// catchReturnFromMain  [XKRegsFpEpk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_catchReturnFromMain(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "catchReturnFromMain", "XKRegsFpEpk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// exit  [uMei1W9uyNo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_exit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "exit", "uMei1W9uyNo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// Need_sceLibc  [P330P3dFF68]  OBJECT: dado, nao funcao. Buffer zerado de 4 bytes.
alignas(16) u8 obj_Need_sceLibc[4] = {};

// strncpy  [6sJWiWSRuqk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strncpy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strncpy", "6sJWiWSRuqk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Stoul  [zlfEH8FmyUA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Stoul(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Stoul", "zlfEH8FmyUA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// vsprintf  [jbz9I9vkqkk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_vsprintf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "vsprintf", "jbz9I9vkqkk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// rand  [cpCOXWMgha0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_rand(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "rand", "cpCOXWMgha0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// printf  [hcuQgD53UxM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_printf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "printf", "hcuQgD53UxM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fflush  [MUjC4lbHrK4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fflush(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fflush", "MUjC4lbHrK4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sscanf  [1Pk0qZQGeWo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sscanf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sscanf", "1Pk0qZQGeWo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// puts  [YQ0navp+YIc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_puts(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "puts", "YQ0navp+YIc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZSt13set_terminatePFvvE  [NG1phipELJE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZSt13set_terminatePFvvE(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZSt13set_terminatePFvvE", "NG1phipELJE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Stdout  [2sWzhYqFH4E]  OBJECT: dado, nao funcao. Buffer zerado de 456 bytes.
alignas(16) u8 obj__Stdout[456] = {};

// fwrite  [MpxhMh8QFro]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fwrite(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fwrite", "MpxhMh8QFro", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fprintf  [fffwELXNVFA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fprintf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fprintf", "fffwELXNVFA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Stderr  [H8AprKeZtNg]  OBJECT: dado, nao funcao. Buffer zerado de 456 bytes.
alignas(16) u8 obj__Stderr[456] = {};

// strchr  [ob5xAW4ln-0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strchr(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strchr", "ob5xAW4ln-0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// memchr  [8u8lPzUEq+U]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_memchr(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "memchr", "8u8lPzUEq+U", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strncat  [kHg45qPC6f0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strncat(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strncat", "kHg45qPC6f0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// pow  [9LCjpWyQ5Zc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_pow(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "pow", "9LCjpWyQ5Zc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZSt11_Xbad_allocv  [eT2UsmTewbU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZSt11_Xbad_allocv(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZSt11_Xbad_allocv", "eT2UsmTewbU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// malloc_stats_fast  [KuOuD58hqn4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_malloc_stats_fast(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "malloc_stats_fast", "KuOuD58hqn4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZNSt8ios_base4InitC1Ev  [sqWytnhYdEg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZNSt8ios_base4InitC1Ev(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZNSt8ios_base4InitC1Ev", "sqWytnhYdEg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZNSt8ios_base4InitD1Ev  [kxXCvcat1cM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZNSt8ios_base4InitD1Ev(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZNSt8ios_base4InitD1Ev", "kxXCvcat1cM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZNSt6_WinitC1Ev  [-Bl9-SZ2noc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZNSt6_WinitC1Ev(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZNSt6_WinitC1Ev", "-Bl9-SZ2noc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZNSt6_WinitD1Ev  [Uw3OTZFPNt4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZNSt6_WinitD1Ev(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZNSt6_WinitD1Ev", "Uw3OTZFPNt4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZSt14_Xlength_errorPKc  [tQIo+GIPklo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZSt14_Xlength_errorPKc(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZSt14_Xlength_errorPKc", "tQIo+GIPklo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZSt13_Syserror_mapi  [PDX01ziUz+4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZSt13_Syserror_mapi(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZSt13_Syserror_mapi", "PDX01ziUz+4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// malloc  [gQX+4GDQjpM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_malloc(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "malloc", "gQX+4GDQjpM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// putchar  [m5wN+SwZOR4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_putchar(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "putchar", "m5wN+SwZOR4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// free  [tIhsqj0qsFE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_free(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "free", "tIhsqj0qsFE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// qsort  [AEJdIVZTEmo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_qsort(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "qsort", "AEJdIVZTEmo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// ldexpf  [kn0yiYeExgA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_ldexpf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "ldexpf", "kn0yiYeExgA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// exp2f  [wuAQt-j+p4o]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_exp2f(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "exp2f", "wuAQt-j+p4o", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strspn  [-kU6bB4M-+k]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strspn(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strspn", "-kU6bB4M-+k", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strcspn  [q0F6yS-rCms]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strcspn(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strcspn", "q0F6yS-rCms", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strtol  [mXlxhmLNMPg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strtol(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strtol", "mXlxhmLNMPg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Stod  [c41UEHVtiEA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Stod(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Stod", "c41UEHVtiEA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strtok  [oVkZ8W8-Q8A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strtok(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strtok", "oVkZ8W8-Q8A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fseek  [rQFVBXp-Cxg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fseek(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fseek", "rQFVBXp-Cxg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fread  [lbB+UlZqVG0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fread(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fread", "lbB+UlZqVG0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fclose  [uodLYyUip20]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fclose(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fclose", "uodLYyUip20", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// longjmp  [lKEN2IebgJ0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_longjmp(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "longjmp", "lKEN2IebgJ0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// setjmp  [gNQ1V2vfXDE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_setjmp(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "setjmp", "gNQ1V2vfXDE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// abort  [L1SBTkC+Cvw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_abort(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "abort", "L1SBTkC+Cvw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// gmtime  [1mecP7RgI2A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_gmtime(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "gmtime", "1mecP7RgI2A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// powf  [1D0H2KNjshE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_powf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "powf", "1D0H2KNjshE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Exit  [wZi5ly2guNw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Exit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Exit", "wZi5ly2guNw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fopen  [xeYO4u7uyJ0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fopen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fopen", "xeYO4u7uyJ0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fputc  [aZK8lNei-Qw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fputc(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fputc", "aZK8lNei-Qw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// ferror  [AHxyhN96dy4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_ferror(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "ferror", "AHxyhN96dy4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// ftell  [Qazy8LmXTvw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_ftell(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "ftell", "Qazy8LmXTvw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fputs  [QrZZdJ8XsX0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fputs(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fputs", "QrZZdJ8XsX0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Getpctype  [sUP1hBaouOw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Getpctype(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Getpctype", "sUP1hBaouOw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Getptolower  [1uJgoVq3bQU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Getptolower(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Getptolower", "1uJgoVq3bQU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Log  [zXCi78bYrEI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Log(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Log", "zXCi78bYrEI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// atanf  [weDug8QD-lE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_atanf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "atanf", "weDug8QD-lE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fmodf  [88Vv-AzHVj8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fmodf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fmodf", "88Vv-AzHVj8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Stdin  [1TDo-ImqkJc]  OBJECT: dado, nao funcao. Buffer zerado de 456 bytes.
alignas(16) u8 obj__Stdin[456] = {};

// getc  [8Q60JLJ6Rv4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_getc(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "getc", "8Q60JLJ6Rv4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// clearerr  [St9nbxSoezk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_clearerr(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "clearerr", "St9nbxSoezk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// fileno  [Fm-dmyywH9Q]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_fileno(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "fileno", "Fm-dmyywH9Q", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// modff  [3+UPM-9E6xY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_modff(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "modff", "3+UPM-9E6xY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// srand  [VPbJwTCgME0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_srand(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "srand", "VPbJwTCgME0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Getptoupper  [rcQCUr0EaRU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Getptoupper(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Getptoupper", "rcQCUr0EaRU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// exp  [NVadfnzQhHQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_exp(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "exp", "NVadfnzQhHQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// ldexp  [JrwFIMzKNr0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_ldexp(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "ldexp", "JrwFIMzKNr0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZSt14_Xout_of_rangePKc  [ozMAr28BwSY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZSt14_Xout_of_rangePKc(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZSt14_Xout_of_rangePKc", "ozMAr28BwSY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Stoll  [7pNKcscKrf8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Stoll(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Stoll", "7pNKcscKrf8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _Unwind_Resume  [f1zwJ3jAI2k]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__Unwind_Resume(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_Unwind_Resume", "f1zwJ3jAI2k", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// __cxa_begin_catch  [3cUUypQzMiI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___cxa_begin_catch(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__cxa_begin_catch", "3cUUypQzMiI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZSt9terminatev  [qYhnoevd9bI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub__ZSt9terminatev(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "_ZSt9terminatev", "qYhnoevd9bI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// _ZTVN10__cxxabiv117__class_type_infoE  [byV+FWlAnB4]  OBJECT: dado, nao funcao. Buffer zerado de 80 bytes.
alignas(16) u8 obj__ZTVN10__cxxabiv117__class_type_infoE[80] = {};

// __gxx_personality_v0  [XwLA5cTHjt4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___gxx_personality_v0(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__gxx_personality_v0", "XwLA5cTHjt4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// vprintf  [GMpvxPFW924]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_vprintf(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "vprintf", "GMpvxPFW924", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// memcpy_s  [NFLs+dRJGNg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_memcpy_s(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "memcpy_s", "NFLs+dRJGNg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// strnlen_s  [DQbtGaBKlaw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_strnlen_s(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "strnlen_s", "DQbtGaBKlaw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"jjjRS7l1MPM", "_FLog", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__FLog), 0},
    {"cCXjU72Z0Ow", "_Sin", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Sin), 0},
    {"T7uyNqP7vQA", "tan", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_tan), 0},
    {"OXmauLdQ8kY", "atan", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_atan), 0},
    {"7Ly52zaL44Q", "asin", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_asin), 0},
    {"HUbZmOnT-Dg", "atan2", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_atan2), 0},
    {"QI-x0SL8jhw", "acosf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_acosf), 0},
    {"EH-x713A99c", "atan2f", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_atan2f), 0},
    {"ZE6RNL+eLbk", "tanf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_tanf), 0},
    {"DfivPArhucg", "memcmp", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_memcmp), 0},
    {"kiZSXIWd9vg", "strcpy", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strcpy), 0},
    {"Q3VBxCXhUHs", "memcpy", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_memcpy), 0},
    {"8zTFvBIAIN8", "memset", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_memset), 0},
    {"hdm0YfMa7TQ", "_Znam", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Znam), 0},
    {"3GPpjQdAMTw", "__cxa_guard_acquire", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub___cxa_guard_acquire), 0},
    {"tsvEmnenz48", "__cxa_atexit", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub___cxa_atexit), 0},
    {"9rAeANT2tyE", "__cxa_guard_release", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub___cxa_guard_release), 0},
    {"j4ViWNHEgww", "strlen", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strlen), 0},
    {"Ls4tzzhimqQ", "strcat", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strcat), 0},
    {"tcVi5SivF7Q", "sprintf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sprintf), 0},
    {"Ovb2dSJOAuE", "strcmp", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strcmp), 0},
    {"z+P+xCnWLBk", "_ZdlPv", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZdlPv), 0},
    {"8zsu04XNsZ4", "expf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_expf), 0},
    {"9yDWMxEFdJU", "strrchr", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strrchr), 0},
    {"fJnpuVVBbKk", "_Znwm", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Znwm), 0},
    {"+P6FRGH4LfA", "memmove", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_memmove), 0},
    {"viiwFMaNamA", "strstr", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strstr), 0},
    {"aesyjrHVWy4", "strncmp", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strncmp), 0},
    {"ZtjspkJQ+vw", "_FSin", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__FSin), 0},
    {"MLWl90SFWNE", "_ZdaPv", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZdaPv), 0},
    {"mnufPlYbnN0", "_FDtest", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__FDtest), 0},
    {"eLdDw6l0-bU", "snprintf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_snprintf), 0},
    {"Q2V+iqvjgC0", "vsnprintf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_vsnprintf), 0},
    {"zr094EQ39Ww", "__cxa_pure_virtual", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub___cxa_pure_virtual), 0},
    {"bzQExy189ZI", "_init_env", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__init_env), 0},
    {"8G2LB+A3rzg", "atexit", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_atexit), 0},
    {"XKRegsFpEpk", "catchReturnFromMain", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_catchReturnFromMain), 0},
    {"uMei1W9uyNo", "exit", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_exit), 0},
    {"P330P3dFF68", "Need_sceLibc", kLibrary, "libc", SymbolKind::Object, static_cast<void*>(obj_Need_sceLibc), 4},
    {"6sJWiWSRuqk", "strncpy", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strncpy), 0},
    {"zlfEH8FmyUA", "_Stoul", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Stoul), 0},
    {"jbz9I9vkqkk", "vsprintf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_vsprintf), 0},
    {"cpCOXWMgha0", "rand", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_rand), 0},
    {"hcuQgD53UxM", "printf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_printf), 0},
    {"MUjC4lbHrK4", "fflush", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fflush), 0},
    {"1Pk0qZQGeWo", "sscanf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_sscanf), 0},
    {"YQ0navp+YIc", "puts", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_puts), 0},
    {"NG1phipELJE", "_ZSt13set_terminatePFvvE", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZSt13set_terminatePFvvE), 0},
    {"2sWzhYqFH4E", "_Stdout", kLibrary, "libc", SymbolKind::Object, static_cast<void*>(obj__Stdout), 456},
    {"MpxhMh8QFro", "fwrite", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fwrite), 0},
    {"fffwELXNVFA", "fprintf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fprintf), 0},
    {"H8AprKeZtNg", "_Stderr", kLibrary, "libc", SymbolKind::Object, static_cast<void*>(obj__Stderr), 456},
    {"ob5xAW4ln-0", "strchr", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strchr), 0},
    {"8u8lPzUEq+U", "memchr", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_memchr), 0},
    {"kHg45qPC6f0", "strncat", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strncat), 0},
    {"9LCjpWyQ5Zc", "pow", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_pow), 0},
    {"eT2UsmTewbU", "_ZSt11_Xbad_allocv", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZSt11_Xbad_allocv), 0},
    {"KuOuD58hqn4", "malloc_stats_fast", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_malloc_stats_fast), 0},
    {"sqWytnhYdEg", "_ZNSt8ios_base4InitC1Ev", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZNSt8ios_base4InitC1Ev), 0},
    {"kxXCvcat1cM", "_ZNSt8ios_base4InitD1Ev", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZNSt8ios_base4InitD1Ev), 0},
    {"-Bl9-SZ2noc", "_ZNSt6_WinitC1Ev", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZNSt6_WinitC1Ev), 0},
    {"Uw3OTZFPNt4", "_ZNSt6_WinitD1Ev", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZNSt6_WinitD1Ev), 0},
    {"tQIo+GIPklo", "_ZSt14_Xlength_errorPKc", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZSt14_Xlength_errorPKc), 0},
    {"PDX01ziUz+4", "_ZSt13_Syserror_mapi", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZSt13_Syserror_mapi), 0},
    {"gQX+4GDQjpM", "malloc", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_malloc), 0},
    {"m5wN+SwZOR4", "putchar", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_putchar), 0},
    {"tIhsqj0qsFE", "free", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_free), 0},
    {"AEJdIVZTEmo", "qsort", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_qsort), 0},
    {"kn0yiYeExgA", "ldexpf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_ldexpf), 0},
    {"wuAQt-j+p4o", "exp2f", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_exp2f), 0},
    {"-kU6bB4M-+k", "strspn", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strspn), 0},
    {"q0F6yS-rCms", "strcspn", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strcspn), 0},
    {"mXlxhmLNMPg", "strtol", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strtol), 0},
    {"c41UEHVtiEA", "_Stod", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Stod), 0},
    {"oVkZ8W8-Q8A", "strtok", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strtok), 0},
    {"rQFVBXp-Cxg", "fseek", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fseek), 0},
    {"lbB+UlZqVG0", "fread", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fread), 0},
    {"uodLYyUip20", "fclose", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fclose), 0},
    {"lKEN2IebgJ0", "longjmp", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_longjmp), 0},
    {"gNQ1V2vfXDE", "setjmp", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_setjmp), 0},
    {"L1SBTkC+Cvw", "abort", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_abort), 0},
    {"1mecP7RgI2A", "gmtime", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_gmtime), 0},
    {"1D0H2KNjshE", "powf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_powf), 0},
    {"wZi5ly2guNw", "_Exit", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Exit), 0},
    {"xeYO4u7uyJ0", "fopen", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fopen), 0},
    {"aZK8lNei-Qw", "fputc", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fputc), 0},
    {"AHxyhN96dy4", "ferror", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_ferror), 0},
    {"Qazy8LmXTvw", "ftell", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_ftell), 0},
    {"QrZZdJ8XsX0", "fputs", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fputs), 0},
    {"sUP1hBaouOw", "_Getpctype", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Getpctype), 0},
    {"1uJgoVq3bQU", "_Getptolower", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Getptolower), 0},
    {"zXCi78bYrEI", "_Log", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Log), 0},
    {"weDug8QD-lE", "atanf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_atanf), 0},
    {"88Vv-AzHVj8", "fmodf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fmodf), 0},
    {"1TDo-ImqkJc", "_Stdin", kLibrary, "libc", SymbolKind::Object, static_cast<void*>(obj__Stdin), 456},
    {"8Q60JLJ6Rv4", "getc", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_getc), 0},
    {"St9nbxSoezk", "clearerr", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_clearerr), 0},
    {"Fm-dmyywH9Q", "fileno", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_fileno), 0},
    {"3+UPM-9E6xY", "modff", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_modff), 0},
    {"VPbJwTCgME0", "srand", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_srand), 0},
    {"rcQCUr0EaRU", "_Getptoupper", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Getptoupper), 0},
    {"NVadfnzQhHQ", "exp", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_exp), 0},
    {"JrwFIMzKNr0", "ldexp", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_ldexp), 0},
    {"ozMAr28BwSY", "_ZSt14_Xout_of_rangePKc", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZSt14_Xout_of_rangePKc), 0},
    {"7pNKcscKrf8", "_Stoll", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Stoll), 0},
    {"f1zwJ3jAI2k", "_Unwind_Resume", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__Unwind_Resume), 0},
    {"3cUUypQzMiI", "__cxa_begin_catch", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub___cxa_begin_catch), 0},
    {"qYhnoevd9bI", "_ZSt9terminatev", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub__ZSt9terminatev), 0},
    {"byV+FWlAnB4", "_ZTVN10__cxxabiv117__class_type_infoE", kLibrary, "libc", SymbolKind::Object, static_cast<void*>(obj__ZTVN10__cxxabiv117__class_type_infoE), 80},
    {"XwLA5cTHjt4", "__gxx_personality_v0", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub___gxx_personality_v0), 0},
    {"GMpvxPFW924", "vprintf", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_vprintf), 0},
    {"NFLs+dRJGNg", "memcpy_s", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_memcpy_s), 0},
    {"DQbtGaBKlaw", "strnlen_s", kLibrary, "libc", SymbolKind::Function, reinterpret_cast<void*>(&stub_strnlen_s), 0},
};
extern const std::size_t kSymbolCount = 113;

} // namespace hle::gen::libc
