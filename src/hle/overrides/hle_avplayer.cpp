#include <atomic>
#include <cinttypes>
#include <cstring>
#include <string>

#include "common/types.h"
#include "hle/hle_log.h"
#include "hle/overrides/hle_override.h"

namespace {

using namespace hle;

struct AvPlayer {
    void* user_data = nullptr;
    void* callback = nullptr;
    std::string source;
    bool looping = false;
    std::atomic<bool> active{false};
};

using CallbackFn = void (HLE_SYSV_ABI*)(void* user_data, int event_id, int source_id, void* event_data);

HLE_IMPL(u64, sceAvPlayerInit, const void* init_data) {
    auto* player = new AvPlayer();
    if (init_data) {
        // Layout de SceAvPlayerInitData no PS4/eboot:
        // [init_data + 0x50] = pUserData, [init_data + 0x58] = eventCallback
        player->user_data = *reinterpret_cast<void* const*>(reinterpret_cast<uintptr_t>(init_data) + 0x50);
        player->callback = *reinterpret_cast<void* const*>(reinterpret_cast<uintptr_t>(init_data) + 0x58);
    }
    log::Tagged("AVPLAYER", "sceAvPlayerInit(%p) -> player=%p (user_data=%p callback=%p)",
                init_data, player, player->user_data, player->callback);
    return reinterpret_cast<u64>(player);
}
HLE_OVERRIDE(sceAvPlayerInit);

HLE_IMPL(int, sceAvPlayerPostInit, AvPlayer* player, const void* reserved) {
    log::Tagged("AVPLAYER", "sceAvPlayerPostInit(%p)", player);
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerPostInit);

HLE_IMPL(int, sceAvPlayerAddSource, AvPlayer* player, const char* filename) {
    log::Tagged("AVPLAYER", "sceAvPlayerAddSource(%p, \"%s\")", player, filename ? filename : "");
    if (player) {
        if (filename) {
            player->source = filename;
        }
        if (player->user_data) {
            // Desbloqueia diretamente o loop de espera do eboot na thread NuBgProc-ET (while(![rbx + 0x28]) usleep(1000))
            *reinterpret_cast<u8*>(reinterpret_cast<uintptr_t>(player->user_data) + 0x28) = 1;
        }
        if (player->callback) {
            auto fn = reinterpret_cast<CallbackFn>(player->callback);
            // Evento 2 = SCE_AVPLAYER_STATE_READY -> configura os streams
            log::Tagged("AVPLAYER", "Disparando evento READY (2) para callback %p (user_data=%p)", player->callback, player->user_data);
            fn(player->user_data, 2, 0, nullptr);
        }
    }
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerAddSource);

HLE_IMPL(int, sceAvPlayerStreamCount, AvPlayer* player) {
    log::Tagged("AVPLAYER", "sceAvPlayerStreamCount(%p) -> 1", player);
    return 1;
}
HLE_OVERRIDE(sceAvPlayerStreamCount);

HLE_IMPL(int, sceAvPlayerGetStreamInfo, AvPlayer* player, u32 stream_index, void* info) {
    log::Tagged("AVPLAYER", "sceAvPlayerGetStreamInfo(%p, stream=%u, info=%p)", player, stream_index, info);
    if (info) {
        std::memset(info, 0, 128);
        *reinterpret_cast<u32*>(info) = 0; // tipo 0 = video
        *reinterpret_cast<u32*>(static_cast<char*>(info) + 8) = 1920; // largura
        *reinterpret_cast<u32*>(static_cast<char*>(info) + 12) = 1080; // altura
        *reinterpret_cast<u64*>(static_cast<char*>(info) + 24) = 1000; // duracao
    }
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerGetStreamInfo);

HLE_IMPL(int, sceAvPlayerEnableStream, AvPlayer* player, u32 stream_index) {
    log::Tagged("AVPLAYER", "sceAvPlayerEnableStream(%p, %u)", player, stream_index);
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerEnableStream);

HLE_IMPL(int, sceAvPlayerSetLooping, AvPlayer* player, u32 loop) {
    log::Tagged("AVPLAYER", "sceAvPlayerSetLooping(%p, %u)", player, loop);
    if (player) {
        player->looping = (loop != 0);
    }
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerSetLooping);

HLE_IMPL(int, sceAvPlayerStart, AvPlayer* player) {
    log::Tagged("AVPLAYER", "sceAvPlayerStart(%p)", player);
    if (player) {
        player->active = true;
        if (player->user_data) {
            *reinterpret_cast<u8*>(reinterpret_cast<uintptr_t>(player->user_data) + 0x28) = 1;
        }
        if (player->callback) {
            auto fn = reinterpret_cast<CallbackFn>(player->callback);
            // Dispara STATE_PLAY (3) e imediatamente STATE_EOF (4) para avisar o jogo que o trailer terminou
            fn(player->user_data, 3, 0, nullptr);
            fn(player->user_data, 4, 0, nullptr);
        }
    }
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerStart);

HLE_IMPL(int, sceAvPlayerIsActive, AvPlayer* player) {
    // Retorna 0 para indicar que o playback terminou, permitindo ao jogo retornar imediatamente ao portal
    return 0;
}
HLE_OVERRIDE(sceAvPlayerIsActive);

HLE_IMPL(int, sceAvPlayerGetVideoData, AvPlayer* player, void* frame_info) {
    return 0; // SCE_FALSE
}
HLE_OVERRIDE(sceAvPlayerGetVideoData);

HLE_IMPL(int, sceAvPlayerGetAudioData, AvPlayer* player, void* audio_info) {
    return 0; // SCE_FALSE
}
HLE_OVERRIDE(sceAvPlayerGetAudioData);

HLE_IMPL(u64, sceAvPlayerCurrentTime, AvPlayer* player) {
    return 0;
}
HLE_OVERRIDE(sceAvPlayerCurrentTime);

HLE_IMPL(int, sceAvPlayerStop, AvPlayer* player) {
    log::Tagged("AVPLAYER", "sceAvPlayerStop(%p)", player);
    if (player) {
        player->active = false;
        if (player->callback) {
            auto fn = reinterpret_cast<CallbackFn>(player->callback);
            // Evento 1 = SCE_AVPLAYER_STATE_STOP
            fn(player->user_data, 1, 0, nullptr);
        }
    }
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerStop);

HLE_IMPL(int, sceAvPlayerPause, AvPlayer* player) {
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerPause);

HLE_IMPL(int, sceAvPlayerResume, AvPlayer* player) {
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerResume);

HLE_IMPL(int, sceAvPlayerClose, AvPlayer* player) {
    log::Tagged("AVPLAYER", "sceAvPlayerClose(%p)", player);
    if (player) {
        delete player;
    }
    return err::OK;
}
HLE_OVERRIDE(sceAvPlayerClose);

} // namespace
