// Fase 60: idioma do que o port mostra (tela de carregamento, janela de saves) e do idioma de sistema que o
// jogo recebe. Portugues se o Windows estiver em portugues; senao ingles nos textos do port e o idioma do
// Windows no jogo (quando o jogo o tem). `--lang pt|en` ou a variavel LD_LANG escolhem na mao.
#pragma once

#include <windows.h>

#include <cstring>

namespace lang {

// -1 = automatico (Windows / LD_LANG), 0 = ingles, 1 = portugues
inline int& Choice() {
    static int choice = -1;
    return choice;
}

inline LANGID WindowsLanguage() {
    return GetUserDefaultUILanguage();
}

inline bool Pt() {
    if (Choice() >= 0) {
        return Choice() == 1;
    }
    static const bool pt = [] {
        char value[8] = {};
        if (GetEnvironmentVariableA("LD_LANG", value, sizeof(value))) {
            return _strnicmp(value, "pt", 2) == 0;
        }
        return PRIMARYLANGID(WindowsLanguage()) == LANG_PORTUGUESE;
    }();
    return pt;
}

template <typename C>
const C* T(const C* pt, const C* en) {
    return Pt() ? pt : en;
}

// Fase 61: idioma do jogo escolhido a mao (codigo do PS4); -1 = segue Choice() / Windows
inline int& GameChoice() {
    static int code = -1;
    return code;
}

// `--lang pt|en|es|fr|de|it|nl|da`: fora pt, os textos do port ficam em ingles.
inline void SetFromCode(const char* code) {
    static const struct {
        const char* name;
        int ps4;
    } kCodes[] = {{"en", 1}, {"fr", 2}, {"es", 3}, {"de", 4}, {"it", 5}, {"nl", 6}, {"da", 14}};
    if (!_strnicmp(code, "pt", 2)) {
        Choice() = 1;
        return;
    }
    for (const auto& entry : kCodes) {
        if (!_strnicmp(code, entry.name, 2)) {
            Choice() = 0;
            GameChoice() = entry.ps4;
            return;
        }
    }
    Choice() = -1;
}

// Codigo de idioma do sistema do PS4 (SCE_SYSTEM_PARAM_LANG_*) para o jogo.
inline int Ps4SystemLanguage() {
    if (GameChoice() >= 0) {
        return GameChoice();
    }
    if (Choice() == 1 || (Choice() < 0 && Pt())) {
        return SUBLANGID(WindowsLanguage()) == SUBLANG_PORTUGUESE && PRIMARYLANGID(WindowsLanguage()) == LANG_PORTUGUESE ? 7 : 17;
    }
    if (Choice() == 0) {
        return 1;
    }
    const LANGID id = WindowsLanguage();
    switch (PRIMARYLANGID(id)) {
    case LANG_ENGLISH: return SUBLANGID(id) == SUBLANG_ENGLISH_UK ? 18 : 1;
    case LANG_FRENCH: return 2;
    case LANG_SPANISH: return SUBLANGID(id) == SUBLANG_SPANISH_MODERN || SUBLANGID(id) == SUBLANG_SPANISH ? 3 : 20;
    case LANG_GERMAN: return 4;
    case LANG_ITALIAN: return 5;
    case LANG_DUTCH: return 6;
    case LANG_DANISH: return 14;
    default: return 1; // ingles (EUA); os textos do jogo (text.csv) so tem ingles, frances, alemao, espanhol,
                       // italiano, dinamarques, holandes e (fora do csv) portugues

    }
}

} // namespace lang
