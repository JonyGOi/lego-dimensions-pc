// Controle pelo SDL3 (Fase 24.5): DualShock 4 / DualSense / qualquer gamepad que o SDL reconheca,
// com touchpad. Uma thread propria inicia o SDL, abre o primeiro controle que aparecer e guarda
// o estado mais recente; scePadReadState so copia esse estado.
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

void SetEnabled(bool enabled); // --no-sdl-pad
void Start();                  // idempotente; chamado em scePadInit
bool Read(State* out);         // false se nao ha controle aberto pelo SDL
void Rumble(u8 large_motor, u8 small_motor);
const char* Name();            // nome do controle aberto ("" se nenhum)

} // namespace hle::padsdl
