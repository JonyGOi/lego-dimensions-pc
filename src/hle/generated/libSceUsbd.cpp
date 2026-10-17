// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

// Biblioteca: libSceUsbd   Modulo: libSceUsbd   Simbolos: 20

#include "hle_stub.h"

namespace hle::gen::libSceUsbd {

static constexpr const char* kLibrary = "libSceUsbd";

// sceUsbdInit  [TOhg7P6kTH4]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdInit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdInit", "TOhg7P6kTH4", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdExit  [Fq6+0Fm55xU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdExit(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdExit", "Fq6+0Fm55xU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdGetDeviceList  [8qB9Ar4P5nc]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdGetDeviceList(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdGetDeviceList", "8qB9Ar4P5nc", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdKernelDriverActive  [RLf56F-WjKQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdKernelDriverActive(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdKernelDriverActive", "RLf56F-WjKQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdDetachKernelDriver  [Y5go+ha6eDs]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdDetachKernelDriver(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdDetachKernelDriver", "Y5go+ha6eDs", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdClaimInterface  [AE+mHBHneyk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdClaimInterface(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdClaimInterface", "AE+mHBHneyk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdGetMaxPacketSize  [YJ0cMAlLuxQ]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdGetMaxPacketSize(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdGetMaxPacketSize", "YJ0cMAlLuxQ", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdAllocTransfer  [0ktE1PhzGFU]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdAllocTransfer(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdAllocTransfer", "0ktE1PhzGFU", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdFillInterruptTransfer  [t3J5pXxhJlI]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdFillInterruptTransfer(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdFillInterruptTransfer", "t3J5pXxhJlI", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdSubmitTransfer  [L0EHgZZNVas]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdSubmitTransfer(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdSubmitTransfer", "L0EHgZZNVas", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdFreeConfigDescriptor  [Hvd3S--n25w]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdFreeConfigDescriptor(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdFreeConfigDescriptor", "Hvd3S--n25w", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdResetDevice  [hvMn0QJXj5g]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdResetDevice(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdResetDevice", "hvMn0QJXj5g", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdGetDeviceDescriptor  [bhomgbiQgeo]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdGetDeviceDescriptor(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdGetDeviceDescriptor", "bhomgbiQgeo", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdGetBusNumber  [t7WE9mb1TB8]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdGetBusNumber(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdGetBusNumber", "t7WE9mb1TB8", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdGetDeviceAddress  [GjlCrU4GcIY]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdGetDeviceAddress(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdGetDeviceAddress", "GjlCrU4GcIY", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdOpenDeviceWithVidPid  [vrQXYRo1Gwk]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdOpenDeviceWithVidPid(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdOpenDeviceWithVidPid", "vrQXYRo1Gwk", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdGetConfigDescriptor  [Dkm5qe8j3XE]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdGetConfigDescriptor(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdGetConfigDescriptor", "Dkm5qe8j3XE", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdFreeDeviceList  [EQ6SCLMqzkM]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdFreeDeviceList(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdFreeDeviceList", "EQ6SCLMqzkM", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdCheckConnected  [MlW6deWfPp0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdCheckConnected(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdCheckConnected", "MlW6deWfPp0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

// sceUsbdHandleEvents  [EkqGLxWC-S0]
HLE_NOINLINE u64 HLE_SYSV_ABI stub_sceUsbdHandleEvents(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {
    LogStubCall({kLibrary, "sceUsbdHandleEvents", "EkqGLxWC-S0", HLE_RETURN_ADDRESS(),
                 rdi, rsi, rdx, rcx, r8, r9});
    return 0; // SCE_OK
}

extern const HleSymbol kSymbols[] = {
    {"TOhg7P6kTH4", "sceUsbdInit", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdInit), 0},
    {"Fq6+0Fm55xU", "sceUsbdExit", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdExit), 0},
    {"8qB9Ar4P5nc", "sceUsbdGetDeviceList", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdGetDeviceList), 0},
    {"RLf56F-WjKQ", "sceUsbdKernelDriverActive", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdKernelDriverActive), 0},
    {"Y5go+ha6eDs", "sceUsbdDetachKernelDriver", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdDetachKernelDriver), 0},
    {"AE+mHBHneyk", "sceUsbdClaimInterface", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdClaimInterface), 0},
    {"YJ0cMAlLuxQ", "sceUsbdGetMaxPacketSize", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdGetMaxPacketSize), 0},
    {"0ktE1PhzGFU", "sceUsbdAllocTransfer", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdAllocTransfer), 0},
    {"t3J5pXxhJlI", "sceUsbdFillInterruptTransfer", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdFillInterruptTransfer), 0},
    {"L0EHgZZNVas", "sceUsbdSubmitTransfer", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdSubmitTransfer), 0},
    {"Hvd3S--n25w", "sceUsbdFreeConfigDescriptor", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdFreeConfigDescriptor), 0},
    {"hvMn0QJXj5g", "sceUsbdResetDevice", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdResetDevice), 0},
    {"bhomgbiQgeo", "sceUsbdGetDeviceDescriptor", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdGetDeviceDescriptor), 0},
    {"t7WE9mb1TB8", "sceUsbdGetBusNumber", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdGetBusNumber), 0},
    {"GjlCrU4GcIY", "sceUsbdGetDeviceAddress", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdGetDeviceAddress), 0},
    {"vrQXYRo1Gwk", "sceUsbdOpenDeviceWithVidPid", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdOpenDeviceWithVidPid), 0},
    {"Dkm5qe8j3XE", "sceUsbdGetConfigDescriptor", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdGetConfigDescriptor), 0},
    {"EQ6SCLMqzkM", "sceUsbdFreeDeviceList", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdFreeDeviceList), 0},
    {"MlW6deWfPp0", "sceUsbdCheckConnected", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdCheckConnected), 0},
    {"EkqGLxWC-S0", "sceUsbdHandleEvents", kLibrary, "libSceUsbd", SymbolKind::Function, reinterpret_cast<void*>(&stub_sceUsbdHandleEvents), 0},
};
extern const std::size_t kSymbolCount = 20;

} // namespace hle::gen::libSceUsbd
