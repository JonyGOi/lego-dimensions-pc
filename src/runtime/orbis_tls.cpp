#include "runtime/orbis_tls.h"

#include <cstring>

#include <windows.h>

namespace runtime {

namespace {
u32 g_slot = ~0u;
}

OrbisTcb* CurrentTcb() {
    return g_slot == ~0u ? nullptr : static_cast<OrbisTcb*>(TlsGetValue(g_slot));
}

bool OrbisTls::AllocateSlot() {
    const DWORD slot = TlsAlloc();
    // So os 64 primeiros slots ficam inline no TEB; os demais exigiriam uma indirecao extra.
    if (slot == TLS_OUT_OF_INDEXES || slot >= kTebInlineSlots) {
        return false;
    }
    slot_ = slot;
    g_slot = slot;
    return true;
}

namespace {

loader::TlsInfo g_template;
const u8* g_image_base = nullptr;

// Monta [TLS estatico][TCB][DTV] e devolve o TCB.
OrbisTcb* BuildThreadBlock(u8** static_block_out) {
    const u64 static_size = AlignUp(g_template.memsz, g_template.align);
    const u64 total = static_size + sizeof(OrbisTcb) + 3 * sizeof(DtvEntry);
    u8* block = static_cast<u8*>(VirtualAlloc(nullptr, AlignUp(total, 0x1000),
                                              MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!block) {
        return nullptr;
    }
    if (g_template.filesz) {
        std::memcpy(block, g_image_base + g_template.image_vaddr, g_template.filesz);
    }
    auto* tcb = reinterpret_cast<OrbisTcb*>(block + static_size);
    auto* dtv = reinterpret_cast<DtvEntry*>(block + static_size + sizeof(OrbisTcb));
    dtv[0].counter = 1; // geracao
    dtv[1].counter = 1; // numero de modulos
    dtv[2].pointer = block;
    tcb->tcb_self = tcb;
    tcb->tcb_dtv = dtv;
    tcb->tcb_thread = nullptr;
    tcb->tcb_fiber = nullptr;
    if (static_block_out) {
        *static_block_out = block;
    }
    return tcb;
}

} // namespace

bool OrbisTls::SetupCurrentThread(const loader::TlsInfo& tls, const u8* image_base) {
    g_template = tls;
    g_image_base = image_base;
    static_size_ = AlignUp(tls.memsz, tls.align);
    tcb_ = BuildThreadBlock(&static_block_);
    return tcb_ && TlsSetValue(slot_, tcb_) != 0;
}

bool SetupTlsForCurrentThread() {
    if (g_slot == ~0u || !g_image_base) {
        return false;
    }
    OrbisTcb* tcb = BuildThreadBlock(nullptr);
    return tcb && TlsSetValue(g_slot, tcb) != 0;
}

void ReleaseTlsForCurrentThread() {
    OrbisTcb* tcb = CurrentTcb();
    if (tcb) {
        TlsSetValue(g_slot, nullptr);
        VirtualFree(tcb->tcb_dtv[2].pointer, 0, MEM_RELEASE);
    }
}

} // namespace runtime
