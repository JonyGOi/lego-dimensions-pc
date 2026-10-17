// Controle pelo SDL3 (Fase 24.5). Ver hle_pad_sdl.h.
//
// O SDL enxerga o DualShock 4 direto (HIDAPI), sem DS4Windows nem driver de XInput, e entrega os
// botoes pela posicao fisica (SOUTH = cruz). Aqui roda numa thread propria: o SDL e iniciado so
// com o subsistema de gamepad, sem janela, e o estado e lido a cada 4 ms.
#include "hle/overrides/hle_pad_sdl.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

namespace hle::padsdl {

namespace {

constexpr u32 kL3 = 0x2, kR3 = 0x4, kOptions = 0x8, kUp = 0x10, kRight = 0x20, kDown = 0x40, kLeft = 0x80;
constexpr u32 kL2 = 0x100, kR2 = 0x200, kL1 = 0x400, kR1 = 0x800;
constexpr u32 kTriangle = 0x1000, kCircle = 0x2000, kCross = 0x4000, kSquare = 0x8000, kTouchPad = 0x100000;

std::atomic<bool> g_enabled{true};
std::atomic<bool> g_started{false};
std::mutex g_mutex;
State g_state;
char g_name[96] = "";
std::atomic<u32> g_rumble{0xFFFFFFFF}; // pedido pendente: (grande << 8) | pequeno

// Eixo do SDL (-32768..32767, +Y para baixo) -> eixo do Orbis (0..255, 128 = centro), com zona morta.
u8 Axis(Sint16 value) {
    int v = value;
    if (v > -7849 && v < 7849) {
        v = 0;
    }
    return static_cast<u8>((v + 32768) >> 8);
}

u8 Trigger(Sint16 value) { // 0..32767 -> 0..255
    return static_cast<u8>(value <= 0 ? 0 : (value >> 7));
}

void Poll(SDL_Gamepad* pad) {
    State s;
    s.connected = true;
    const struct {
        SDL_GamepadButton sdl;
        u32 orbis;
    } map[] = {
        {SDL_GAMEPAD_BUTTON_SOUTH, kCross},          {SDL_GAMEPAD_BUTTON_EAST, kCircle},
        {SDL_GAMEPAD_BUTTON_WEST, kSquare},          {SDL_GAMEPAD_BUTTON_NORTH, kTriangle},
        {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, kL1},     {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, kR1},
        {SDL_GAMEPAD_BUTTON_LEFT_STICK, kL3},        {SDL_GAMEPAD_BUTTON_RIGHT_STICK, kR3},
        {SDL_GAMEPAD_BUTTON_START, kOptions},        {SDL_GAMEPAD_BUTTON_TOUCHPAD, kTouchPad},
        {SDL_GAMEPAD_BUTTON_BACK, kTouchPad}, // Share / Back: o jogo nao tem botao para isso; vale como clique do touchpad
        {SDL_GAMEPAD_BUTTON_DPAD_UP, kUp},           {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, kRight},
        {SDL_GAMEPAD_BUTTON_DPAD_DOWN, kDown},       {SDL_GAMEPAD_BUTTON_DPAD_LEFT, kLeft},
    };
    for (const auto& m : map) {
        if (SDL_GetGamepadButton(pad, m.sdl)) {
            s.buttons |= m.orbis;
        }
    }
    s.lx = Axis(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTX));
    s.ly = Axis(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTY));
    s.rx = Axis(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTX));
    s.ry = Axis(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTY));
    s.l2 = Trigger(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
    s.r2 = Trigger(SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));
    if (s.l2 > 30) {
        s.buttons |= kL2;
    }
    if (s.r2 > 30) {
        s.buttons |= kR2;
    }
    // Touchpad: ate dois dedos, na resolucao do DualShock 4 (1920 x 942).
    if (SDL_GetNumGamepadTouchpads(pad) > 0) {
        const int fingers = SDL_GetNumGamepadTouchpadFingers(pad, 0);
        for (int f = 0; f < fingers && s.touches < 2; ++f) {
            bool down = false;
            float x = 0.0f, y = 0.0f, pressure = 0.0f;
            if (SDL_GetGamepadTouchpadFinger(pad, 0, f, &down, &x, &y, &pressure) && down) {
                auto& touch = s.touch[s.touches++];
                touch.x = static_cast<u16>(SDL_clamp(x, 0.0f, 1.0f) * 1919.0f);
                touch.y = static_cast<u16>(SDL_clamp(y, 0.0f, 1.0f) * 941.0f);
                touch.id = static_cast<u8>(f);
            }
        }
    }
    std::lock_guard lock(g_mutex);
    g_state = s;
}

void Thread() {
    SDL_SetMainReady();
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1"); // a janela do jogo nao e do SDL
    SDL_SetHint(SDL_HINT_JOYSTICK_THREAD, "1");
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    if (!SDL_Init(SDL_INIT_GAMEPAD)) {
        std::printf("[PAD-SDL] SDL_Init falhou: %s (fica so o XInput)\n", SDL_GetError());
        return;
    }
    std::printf("[PAD-SDL] SDL %d.%d.%d iniciado; esperando um controle\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
    SDL_Gamepad* pad = nullptr;
    SDL_JoystickID pad_id = 0;
    for (;;) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_GAMEPAD_ADDED && !pad) {
                pad = SDL_OpenGamepad(event.gdevice.which);
                if (pad) {
                    pad_id = event.gdevice.which;
                    const char* name = SDL_GetGamepadName(pad);
                    {
                        std::lock_guard lock(g_mutex);
                        std::snprintf(g_name, sizeof(g_name), "%s", name ? name : "controle");
                    }
                    std::printf("[PAD-SDL] controle aberto: %s (tipo %s, %d touchpad(s))\n", name ? name : "?",
                                SDL_GetGamepadStringForType(SDL_GetGamepadType(pad)), SDL_GetNumGamepadTouchpads(pad));
                }
            } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED && pad && event.gdevice.which == pad_id) {
                std::printf("[PAD-SDL] controle desconectado\n");
                SDL_CloseGamepad(pad);
                pad = nullptr;
                std::lock_guard lock(g_mutex);
                g_state = State{};
                g_name[0] = 0;
            }
        }
        if (pad) {
            Poll(pad);
            const u32 rumble = g_rumble.exchange(0xFFFFFFFF);
            if (rumble != 0xFFFFFFFF) {
                SDL_RumbleGamepad(pad, static_cast<Uint16>(((rumble >> 8) & 0xFF) * 257), static_cast<Uint16>((rumble & 0xFF) * 257), 500);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }
}

} // namespace

void SetEnabled(bool enabled) {
    g_enabled = enabled;
}

void Start() {
    if (!g_enabled || g_started.exchange(true)) {
        return;
    }
    std::thread(Thread).detach();
}

bool Read(State* out) {
    std::lock_guard lock(g_mutex);
    *out = g_state;
    return g_state.connected;
}

void Rumble(u8 large_motor, u8 small_motor) {
    g_rumble = (static_cast<u32>(large_motor) << 8) | small_motor;
}

const char* Name() {
    return g_name;
}

} // namespace hle::padsdl
