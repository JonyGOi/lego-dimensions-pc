#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>

#include <filesystem>
#include <string_view>

#include "../hle/trophy_sounds.h"

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif

namespace trophy_sound {

inline void Stop() {
    PlaySoundW(nullptr, nullptr, 0);
}

inline bool EqualsAsciiNoCaseW(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        wchar_t ca = a[i];
        wchar_t cb = b[i];
        if (ca >= L'A' && ca <= L'Z') ca += (L'a' - L'A');
        if (cb >= L'A' && cb <= L'Z') cb += (L'a' - L'A');
        if (ca != cb) return false;
    }
    return true;
}

// Interrompe qualquer efeito em curso e toca (de forma assincrona) o WAV correspondente, ou retorna false se desligado.
inline bool PlayW(std::wstring_view sound_type, const std::filesystem::path& root_path) {
    Stop();
    if (EqualsAsciiNoCaseW(sound_type, L"off") ||
        EqualsAsciiNoCaseW(sound_type, L"desligado") ||
        EqualsAsciiNoCaseW(sound_type, L"none")) {
        return false;
    }

    if (!root_path.empty()) {
        const auto custom_wav = root_path / L"assets" / L"custom_trophy" / L"trophy.wav";
        std::error_code ec;
        if (std::filesystem::exists(custom_wav, ec)) {
            return PlaySoundW(custom_wav.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_NODEFAULT) != FALSE;
        }
    }

    const unsigned char* wav = kTrophySoundPlaystation;
    if (EqualsAsciiNoCaseW(sound_type, L"xbox")) {
        wav = kTrophySoundXbox;
    } else if (EqualsAsciiNoCaseW(sound_type, L"steam")) {
        wav = kTrophySoundSteam;
    }
    return PlaySoundW(reinterpret_cast<LPCWSTR>(wav), nullptr, SND_MEMORY | SND_ASYNC | SND_NODEFAULT) != FALSE;
}

inline bool PlayA(std::string_view sound_type, const std::filesystem::path& root_path) {
    std::wstring wide(sound_type.begin(), sound_type.end());
    return PlayW(wide, root_path);
}

} // namespace trophy_sound
