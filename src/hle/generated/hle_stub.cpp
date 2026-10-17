// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.
#include "hle_stub.h"

#include <atomic>
#include <cinttypes>
#include <cstdio>

namespace hle {
namespace {

void DefaultLogger(const StubCall& c) {
    std::fprintf(stderr,
                 "[HLE STUB] %s::%s (nid %s) ret=0x%016" PRIx64 " rdi=0x%" PRIx64 " rsi=0x%" PRIx64
                 " rdx=0x%" PRIx64 " rcx=0x%" PRIx64 " r8=0x%" PRIx64 " r9=0x%" PRIx64 "\n",
                 c.library, c.name, c.nid, c.return_address, c.rdi, c.rsi, c.rdx, c.rcx, c.r8,
                 c.r9);
}

std::atomic<StubLogger> g_logger{&DefaultLogger};

} // namespace

void SetStubLogger(StubLogger logger) {
    g_logger.store(logger ? logger : &DefaultLogger, std::memory_order_relaxed);
}

void LogStubCall(const StubCall& call) {
    g_logger.load(std::memory_order_relaxed)(call);
}

} // namespace hle
