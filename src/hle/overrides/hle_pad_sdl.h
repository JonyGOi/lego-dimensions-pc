// Controle pelo SDL3 (Fase 24.5): DualShock 4 / DualSense / qualquer gamepad que o SDL reconheca,
// com touchpad. Uma thread propria inicia o SDL, abre os controles que aparecerem (Fase 61: ate 4, um por
// jogador) e guarda o estado mais recente de cada um; scePadReadState so copia esse estado.
#pragma once

#include "common/types.h"

namespace hle::padsdl {

struct State {
    bool connected = false;
    u32 buttons = 0; // ja nas mascaras do Orbis
    u8 lx = 0x80, ly = 0x80, rx = 0x80, ry = 0x80;
    u8 l2 = 0, r2 = 0;
    u8 touches = 0;
    struct {
        u16 x, y; // 0..1919, 0..941 (resolucao do touchpad do DualShock 4)
        u8 id;
    } touch[2]{};
};

// Fase 61 (co-op local): ate kSlots controles, cada um numa vaga (0 = jogador 1). Um controle novo ocupa a
// primeira vaga livre; desconectado, a vaga fica livre de novo.
constexpr int kSlots = 4;

void SetEnabled(bool enabled);              // --no-sdl-pad
void Start();                               // idempotente; chamado em scePadInit
bool Read(State* out, int slot = 0);        // false se a vaga nao tem controle aberto pelo SDL
int Count();                                // quantos controles o SDL tem abertos
void Rumble(u8 large_motor, u8 small_motor, int slot = 0);
const char* Name(int slot = 0);             // nome do controle da vaga ("" se nenhum)

} // namespace hle::padsdl
