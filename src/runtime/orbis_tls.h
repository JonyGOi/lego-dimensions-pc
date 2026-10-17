#pragma once

#include "common/types.h"
#include "loader/elf_pie_loader.h"

namespace runtime {

union DtvEntry {
    u64 counter;
    u8* pointer;
};

// Thread Control Block do Orbis (FreeBSD amd64, TLS variante II): fs:[0] aponta para ca.
struct OrbisTcb {
    OrbisTcb* tcb_self;
    DtvEntry* tcb_dtv;
    void* tcb_thread;
    void* tcb_fiber;
};

// No Windows x64 o registrador FS nao e utilizavel, entao o ponteiro do TCB fica em um slot
// de TLS do Windows (TEB.TlsSlots) e o patcher troca "fs:[0]" por "gs:[TEB + slot]".
class OrbisTls {
public:
    static constexpr u32 kTebTlsSlotsOffset = 0x1480;
    static constexpr u32 kTebInlineSlots = 64;

    // Aloca o slot do Windows (uma vez por processo).
    bool AllocateSlot();

    // Monta [TLS estatico][TCB] para a thread atual e grava o TCB no slot.
    bool SetupCurrentThread(const loader::TlsInfo& tls, const u8* image_base);

    u32 slot() const { return slot_; }
    // Deslocamento a partir de GS que contem o ponteiro do TCB.
    u32 gs_offset() const { return kTebTlsSlotsOffset + slot_ * 8; }
    OrbisTcb* tcb() const { return tcb_; }
    u8* static_block() const { return static_block_; }
    u64 static_size() const { return static_size_; }

private:
    u32 slot_ = ~0u;
    OrbisTcb* tcb_ = nullptr;
    u8* static_block_ = nullptr;
    u64 static_size_ = 0;
};

// TCB da thread atual (nullptr se a thread nao tem TLS do Orbis montado).
OrbisTcb* CurrentTcb();

// Para threads criadas pelo jogo: monta um bloco TLS/TCB proprio e grava no mesmo slot que o
// patcher de FS le (gs:[0x1480 + slot*8]). Exige que a thread principal ja tenha sido montada.
bool SetupTlsForCurrentThread();
void ReleaseTlsForCurrentThread();

} // namespace runtime
