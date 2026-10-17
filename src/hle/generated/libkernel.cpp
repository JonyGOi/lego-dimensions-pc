// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libkernel   Modulo: libkernel   Simbolos: 58

#include "hle_stub.h"

namespace hle::gen::libkernel {

static constexpr const char* kLibrary = "libkernel";

// __stack_chk_guard  [f7uOxY9mM1U]  OBJECT: dado, nao funcao. Buffer zerado de 64 bytes.
alignas(16) u8 obj___stack_chk_guard[64] = {};

// __stack_chk_fail  [Ou3iL1abvng]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___stack_chk_fail(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__stack_chk_fail", "Ou3iL1abvng", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelGetDirectMemorySize  [pO96TwzOm5E]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelGetDirectMemorySize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelGetDirectMemorySize", "pO96TwzOm5E", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelDirectMemoryQuery  [BHouLQzh0X0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelDirectMemoryQuery(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelDirectMemoryQuery", "BHouLQzh0X0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelMunmap  [cQke9UuBQOk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelMunmap(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelMunmap", "cQke9UuBQOk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelReleaseDirectMemory  [MBuItvba6z8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelReleaseDirectMemory(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelReleaseDirectMemory", "MBuItvba6z8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelCreateEqueue  [D0OdFMjp46I]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelCreateEqueue(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelCreateEqueue", "D0OdFMjp46I", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadMutexattrInit  [F8bUHwAG284]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadMutexattrInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadMutexattrInit", "F8bUHwAG284", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadMutexattrSettype  [iMp8QpE+XO4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadMutexattrSettype(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadMutexattrSettype", "iMp8QpE+XO4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadMutexInit  [cmo1RIYva9o]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadMutexInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadMutexInit", "cmo1RIYva9o", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadMutexattrDestroy  [smWEktiyyG0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadMutexattrDestroy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadMutexattrDestroy", "smWEktiyyG0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadMutexDestroy  [2Of0f+3mhhE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadMutexDestroy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadMutexDestroy", "2Of0f+3mhhE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadMutexLock  [9UK1vLZQft4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadMutexLock(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadMutexLock", "9UK1vLZQft4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadMutexTrylock  [upoVrzMHFeE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadMutexTrylock(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadMutexTrylock", "upoVrzMHFeE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadMutexUnlock  [tn3VlD0hG60]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadMutexUnlock(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadMutexUnlock", "tn3VlD0hG60", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelGetProcessTime  [4J2sUJmuHZQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelGetProcessTime(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelGetProcessTime", "4J2sUJmuHZQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// __tls_get_addr  [vNe1w4diLCs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___tls_get_addr(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__tls_get_addr", "vNe1w4diLCs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelWaitSema  [Zxa0VhQVTsk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelWaitSema(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelWaitSema", "Zxa0VhQVTsk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelClearEventFlag  [7uhBFWRAS60]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelClearEventFlag(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelClearEventFlag", "7uhBFWRAS60", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelSignalSema  [4czppHBiriw]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelSignalSema(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelSignalSema", "4czppHBiriw", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelSetEventFlag  [IOnSvHzqu6A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelSetEventFlag(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelSetEventFlag", "IOnSvHzqu6A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelWaitEventFlag  [JTvBflhYazQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelWaitEventFlag(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelWaitEventFlag", "JTvBflhYazQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelCreateEventFlag  [BpFoboUJoZU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelCreateEventFlag(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelCreateEventFlag", "BpFoboUJoZU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelCreateSema  [188x57JYp0g]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelCreateSema(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelCreateSema", "188x57JYp0g", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelUsleep  [1jfXLRVzisc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelUsleep(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelUsleep", "1jfXLRVzisc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelDeleteSema  [R1Jvn8bSCW8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelDeleteSema(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelDeleteSema", "R1Jvn8bSCW8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelDeleteEventFlag  [8mql9OcQnd4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelDeleteEventFlag(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelDeleteEventFlag", "8mql9OcQnd4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// __error  [9BcDykPmo1I]
HLE_NOINLINE u64 HLE_SYSV_ABI stub___error(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "__error", "9BcDykPmo1I", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadAttrInit  [nsYoNRywwNg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadAttrInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadAttrInit", "nsYoNRywwNg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadAttrSetstacksize  [UTXzJbWhhTE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadAttrSetstacksize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadAttrSetstacksize", "UTXzJbWhhTE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadAttrSetaffinity  [3qxgM4ezETA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadAttrSetaffinity(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadAttrSetaffinity", "3qxgM4ezETA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadAttrSetdetachstate  [-Wreprtu0Qs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadAttrSetdetachstate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadAttrSetdetachstate", "-Wreprtu0Qs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCreate  [6UgtwV+0zb4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCreate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCreate", "6UgtwV+0zb4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadAttrDestroy  [62KCwEMmzcM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadAttrDestroy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadAttrDestroy", "62KCwEMmzcM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadJoin  [onNY9Byn-W8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadJoin(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadJoin", "onNY9Byn-W8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadSetprio  [W0Hpm2X0uPE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadSetprio(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadSetprio", "W0Hpm2X0uPE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelAllocateDirectMemory  [rTXw65xmLIA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelAllocateDirectMemory(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelAllocateDirectMemory", "rTXw65xmLIA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelMapDirectMemory  [L-Q3LEjIbgA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelMapDirectMemory(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelMapDirectMemory", "L-Q3LEjIbgA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelUuidCreate  [Xjoosiw+XPI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelUuidCreate(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelUuidCreate", "Xjoosiw+XPI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelOpen  [1G3lF1Gg1k8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelOpen(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelOpen", "1G3lF1Gg1k8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelClose  [UK2Tl2DWUns]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelClose(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelClose", "UK2Tl2DWUns", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelPwrite  [nKWi-N2HBV4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelPwrite(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelPwrite", "nKWi-N2HBV4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelPread  [+r3rMFwItV4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelPread(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelPread", "+r3rMFwItV4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelUnlink  [AUXVxWeJU-A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelUnlink(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelUnlink", "AUXVxWeJU-A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelRename  [52NcYU9+lEo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelRename(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelRename", "52NcYU9+lEo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelWaitEqueue  [fzyMKs9kim0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelWaitEqueue(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelWaitEqueue", "fzyMKs9kim0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadYield  [T72hz6ffq08]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadYield(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadYield", "T72hz6ffq08", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadExit  [3kg7rT0NQIs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadExit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadExit", "3kg7rT0NQIs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelGettimeofday  [ejekcaNQNq0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelGettimeofday(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelGettimeofday", "ejekcaNQNq0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceKernelRead  [Cg4srZ6TKbU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceKernelRead(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceKernelRead", "Cg4srZ6TKbU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCondattrInit  [m5-2bsNfv7s]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCondattrInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCondattrInit", "m5-2bsNfv7s", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCondInit  [2Tb92quprl0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCondInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCondInit", "2Tb92quprl0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCondattrDestroy  [waPcxYiR3WA]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCondattrDestroy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCondattrDestroy", "waPcxYiR3WA", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCondDestroy  [g+PZd2hiacg]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCondDestroy(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCondDestroy", "g+PZd2hiacg", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCondSignal  [kDh-NfxgMtE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCondSignal(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCondSignal", "kDh-NfxgMtE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCondBroadcast  [JGgj7Uvrl+A]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCondBroadcast(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCondBroadcast", "JGgj7Uvrl+A", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCondTimedwait  [BmMjYxmew1w]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCondTimedwait(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCondTimedwait", "BmMjYxmew1w", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// scePthreadCondWait  [WKAXJ4XBPQ4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_scePthreadCondWait(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "scePthreadCondWait", "WKAXJ4XBPQ4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"f7uOxY9mM1U", "__stack_chk_guard", kLibrary, "libkernel", SymbolKind::Object, static_cast<void*>(obj___stack_chk_guard), 64},
    {"Ou3iL1abvng", "__stack_chk_fail", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub___stack_chk_fail), 0},
    {"pO96TwzOm5E", "sceKernelGetDirectMemorySize", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelGetDirectMemorySize), 0},
    {"BHouLQzh0X0", "sceKernelDirectMemoryQuery", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelDirectMemoryQuery), 0},
    {"cQke9UuBQOk", "sceKernelMunmap", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelMunmap), 0},
    {"MBuItvba6z8", "sceKernelReleaseDirectMemory", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelReleaseDirectMemory), 0},
    {"D0OdFMjp46I", "sceKernelCreateEqueue", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelCreateEqueue), 0},
    {"F8bUHwAG284", "scePthreadMutexattrInit", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadMutexattrInit), 0},
    {"iMp8QpE+XO4", "scePthreadMutexattrSettype", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadMutexattrSettype), 0},
    {"cmo1RIYva9o", "scePthreadMutexInit", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadMutexInit), 0},
    {"smWEktiyyG0", "scePthreadMutexattrDestroy", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadMutexattrDestroy), 0},
    {"2Of0f+3mhhE", "scePthreadMutexDestroy", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadMutexDestroy), 0},
    {"9UK1vLZQft4", "scePthreadMutexLock", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadMutexLock), 0},
    {"upoVrzMHFeE", "scePthreadMutexTrylock", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadMutexTrylock), 0},
    {"tn3VlD0hG60", "scePthreadMutexUnlock", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadMutexUnlock), 0},
    {"4J2sUJmuHZQ", "sceKernelGetProcessTime", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelGetProcessTime), 0},
    {"vNe1w4diLCs", "__tls_get_addr", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub___tls_get_addr), 0},
    {"Zxa0VhQVTsk", "sceKernelWaitSema", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelWaitSema), 0},
    {"7uhBFWRAS60", "sceKernelClearEventFlag", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelClearEventFlag), 0},
    {"4czppHBiriw", "sceKernelSignalSema", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelSignalSema), 0},
    {"IOnSvHzqu6A", "sceKernelSetEventFlag", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelSetEventFlag), 0},
    {"JTvBflhYazQ", "sceKernelWaitEventFlag", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelWaitEventFlag), 0},
    {"BpFoboUJoZU", "sceKernelCreateEventFlag", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelCreateEventFlag), 0},
    {"188x57JYp0g", "sceKernelCreateSema", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelCreateSema), 0},
    {"1jfXLRVzisc", "sceKernelUsleep", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelUsleep), 0},
    {"R1Jvn8bSCW8", "sceKernelDeleteSema", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelDeleteSema), 0},
    {"8mql9OcQnd4", "sceKernelDeleteEventFlag", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelDeleteEventFlag), 0},
    {"9BcDykPmo1I", "__error", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub___error), 0},
    {"nsYoNRywwNg", "scePthreadAttrInit", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadAttrInit), 0},
    {"UTXzJbWhhTE", "scePthreadAttrSetstacksize", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadAttrSetstacksize), 0},
    {"3qxgM4ezETA", "scePthreadAttrSetaffinity", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadAttrSetaffinity), 0},
    {"-Wreprtu0Qs", "scePthreadAttrSetdetachstate", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadAttrSetdetachstate), 0},
    {"6UgtwV+0zb4", "scePthreadCreate", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCreate), 0},
    {"62KCwEMmzcM", "scePthreadAttrDestroy", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadAttrDestroy), 0},
    {"onNY9Byn-W8", "scePthreadJoin", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadJoin), 0},
    {"W0Hpm2X0uPE", "scePthreadSetprio", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadSetprio), 0},
    {"rTXw65xmLIA", "sceKernelAllocateDirectMemory", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelAllocateDirectMemory), 0},
    {"L-Q3LEjIbgA", "sceKernelMapDirectMemory", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelMapDirectMemory), 0},
    {"Xjoosiw+XPI", "sceKernelUuidCreate", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelUuidCreate), 0},
    {"1G3lF1Gg1k8", "sceKernelOpen", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelOpen), 0},
    {"UK2Tl2DWUns", "sceKernelClose", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelClose), 0},
    {"nKWi-N2HBV4", "sceKernelPwrite", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelPwrite), 0},
    {"+r3rMFwItV4", "sceKernelPread", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelPread), 0},
    {"AUXVxWeJU-A", "sceKernelUnlink", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelUnlink), 0},
    {"52NcYU9+lEo", "sceKernelRename", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelRename), 0},
    {"fzyMKs9kim0", "sceKernelWaitEqueue", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelWaitEqueue), 0},
    {"T72hz6ffq08", "scePthreadYield", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadYield), 0},
    {"3kg7rT0NQIs", "scePthreadExit", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadExit), 0},
    {"ejekcaNQNq0", "sceKernelGettimeofday", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelGettimeofday), 0},
    {"Cg4srZ6TKbU", "sceKernelRead", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceKernelRead), 0},
    {"m5-2bsNfv7s", "scePthreadCondattrInit", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCondattrInit), 0},
    {"2Tb92quprl0", "scePthreadCondInit", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCondInit), 0},
    {"waPcxYiR3WA", "scePthreadCondattrDestroy", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCondattrDestroy), 0},
    {"g+PZd2hiacg", "scePthreadCondDestroy", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCondDestroy), 0},
    {"kDh-NfxgMtE", "scePthreadCondSignal", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCondSignal), 0},
    {"JGgj7Uvrl+A", "scePthreadCondBroadcast", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCondBroadcast), 0},
    {"BmMjYxmew1w", "scePthreadCondTimedwait", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCondTimedwait), 0},
    {"WKAXJ4XBPQ4", "scePthreadCondWait", kLibrary, "libkernel", SymbolKind::Function, reinterpret_cast<void*>(&stub_scePthreadCondWait), 0},
};
extern const std::size_t kSymbolCount = 58;

} // namespace hle::gen::libkernel
