#pragma once

#include "common/types.h"

// Fila de eventos do kernel (equeue = kqueue do FreeBSD). A parte visivel ao jogo
// (sceKernelCreateEqueue / sceKernelWaitEqueue) fica em hle_kernel_sync.cpp; esta e a
// interface para os subsistemas HLE que GERAM eventos (VideoOut: flip; GnmDriver: EOP).
namespace hle::equeue {

struct KernelEvent { // SceKernelEvent
    u64 ident;
    s16 filter;
    u16 flags;
    u32 fflags;
    s64 data;
    void* udata;
};
static_assert(sizeof(KernelEvent) == 0x20);

constexpr s16 kFilterVideoOut = -13;
constexpr s16 kFilterGraphicsCore = -14;

using Handle = void*; // SceKernelEqueue

bool IsValid(Handle queue);

// Registra (ident, filter) na fila; `udata` volta ao jogo em cada evento entregue.
bool Register(Handle queue, u64 ident, s16 filter, void* udata);
bool Unregister(Handle queue, u64 ident, s16 filter);

// Marca o evento como disparado e acorda quem estiver em sceKernelWaitEqueue. Como no kqueue,
// disparos repetidos antes da leitura se fundem em um unico evento (fica o `data` mais novo).
// Retorna false se (ident, filter) nao estiver registrado nessa fila.
bool Trigger(Handle queue, u64 ident, s16 filter, s64 data);

} // namespace hle::equeue
