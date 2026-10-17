// libkernel: tempo, errno, TLS dinamico, UUID e protecao de pilha.
#include <chrono>
#include <cinttypes>
#include <cstring>
#include <random>
#include <thread>

#include <windows.h>

#include "hle/overrides/hle_override.h"
#include "hle_symbol_table.h"
#include "runtime/crash_handler.h"
#include "runtime/orbis_tls.h"

namespace {

using namespace hle;
using Clock = std::chrono::steady_clock;

const Clock::time_point g_process_start = Clock::now();

struct OrbisTimeval {
    s64 tv_sec;
    s64 tv_usec;
};

struct OrbisTimespec {
    s64 tv_sec;
    s64 tv_nsec;
};

struct TlsIndex {
    u64 module;
    u64 offset;
};

u64 WallClockMicros() {
    using namespace std::chrono;
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

HLE_IMPL(u64, sceKernelGetProcessTime) {
    const u64 us = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - g_process_start).count();
    HLE_QUIET("-> %" PRIu64 " us", us);
    return us;
}
HLE_OVERRIDE(sceKernelGetProcessTime);

HLE_IMPL(int, sceKernelUsleep, u32 micros) {
    HLE_QUIET("%u us", micros);
    runtime::WaitScope waiting("sceKernelUsleep");
    std::this_thread::sleep_for(std::chrono::microseconds(micros));
    return err::OK;
}
HLE_OVERRIDE(sceKernelUsleep);

HLE_IMPL(int, sceKernelGettimeofday, OrbisTimeval* tv) {
    HLE_QUIET("%p", static_cast<void*>(tv));
    if (!tv) {
        return err::kEINVAL;
    }
    const u64 us = WallClockMicros();
    tv->tv_sec = us / 1000000;
    tv->tv_usec = us % 1000000;
    return err::OK;
}
HLE_OVERRIDE(sceKernelGettimeofday);

HLE_IMPL(int, clock_gettime, int clock_id, OrbisTimespec* ts) {
    HLE_QUIET("clock=%d", clock_id);
    if (!ts) {
        return -1;
    }
    u64 ns;
    if (clock_id == 0) { // CLOCK_REALTIME
        ns = WallClockMicros() * 1000;
    } else { // monotonic, uptime, process time...
        ns = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - g_process_start).count();
    }
    ts->tv_sec = ns / 1000000000;
    ts->tv_nsec = ns % 1000000000;
    return 0;
}
HLE_OVERRIDE(clock_gettime);

HLE_IMPL(int*, __error) {
    static thread_local int guest_errno = 0;
    HLE_QUIET("");
    return &guest_errno;
}
HLE_OVERRIDE(__error);

HLE_IMPL(void*, __tls_get_addr, TlsIndex* index) {
    HLE_QUIET("mod=%" PRIu64 " off=0x%" PRIx64, index->module, index->offset);
    // So existe o modulo 1 (o eboot): dtv[0]=geracao, dtv[1]=contagem, dtv[2]=bloco do modulo 1.
    runtime::OrbisTcb* tcb = runtime::CurrentTcb();
    if (!tcb || index->module != 1) {
        log::Fatal("__tls_get_addr: modulo TLS %" PRIu64 " inexistente (tcb=%p)", index->module,
                   static_cast<void*>(tcb));
    }
    return tcb->tcb_dtv[2].pointer + index->offset;
}
HLE_OVERRIDE(__tls_get_addr);

HLE_IMPL(int, sceKernelUuidCreate, u8* uuid) {
    if (!uuid) {
        return err::kEINVAL;
    }
    std::random_device rd;
    for (int i = 0; i < 16; i += 4) {
        const u32 r = rd();
        std::memcpy(uuid + i, &r, 4);
    }
    uuid[6] = (uuid[6] & 0x0F) | 0x40; // versao 4
    uuid[8] = (uuid[8] & 0x3F) | 0x80;
    HLE_TRACE("%p", static_cast<void*>(uuid));
    return err::OK;
}
HLE_OVERRIDE(sceKernelUuidCreate);

HLE_IMPL(void, __stack_chk_fail) {
    log::Fatal("__stack_chk_fail: canario de pilha corrompido (chamado de 0x%" PRIx64 ")",
               (u64)HLE_RETURN_ADDRESS());
}
HLE_OVERRIDE(__stack_chk_fail);

} // namespace

namespace hle {

void InitKernelDataObjects() {
    if (const HleSymbol* guard = FindSymbolByName("__stack_chk_guard")) {
        const u64 canary = 0x5AFE'C0DE'D00D'F00DULL;
        std::memcpy(guard->address, &canary, sizeof(canary));
    }
}

} // namespace hle
