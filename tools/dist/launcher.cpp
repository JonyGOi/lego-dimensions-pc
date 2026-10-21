// Fase 49/50: launcher do LEGO Dimensions PC.
// Pede a pasta do jogo (dump do PS4, CUSA01176) e a das DLCs, mostra a arte do proprio dump
// (sce_sys\pic1.png), roda a preparacao na primeira vez (tools\setup\preparar.py: eboot + shaders AOT)
// e abre o port (build\lego_dimensions.exe) com as opcoes escolhidas. Nada do jogo vem com o port.
#define UNICODE
#define _UNICODE
#include "ui.h"

#include <shellapi.h>
#include <uxtheme.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <tlhelp32.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' "      \
                        "processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace fs = std::filesystem;
using ui::S;

namespace {

constexpr int kW = 980, kH = 840, kHero = 330; // Fase 61: arte 50 mais baixa para a linha de resolucao / idioma
constexpr UINT WM_APP_LINE = WM_APP + 1, WM_APP_DONE = WM_APP + 2;
enum Id { kGameEdit = 100, kGameBrowse, kDlcEdit, kDlcBrowse, kPrepare, kPlay, kFps, kConsole, kLog, kPatchEdit, kPatchBrowse, kFullscreen,
          kResolution, kLanguage, kSkipCutscenes, kUpdate };

HWND g_main, g_game, g_patch, g_dlc, g_log, g_prepare, g_play, g_fps, g_console, g_fullscreen, g_focus;
HWND g_resolution, g_language, g_skip, g_update; // Fase 61

// Fase 61: o que o Python em segundo plano esta fazendo (preparar, procurar atualizacao, atualizar)
enum class Task { kPrepare, kCheck, kUpdate } g_task = Task::kPrepare;
std::wstring g_version, g_new_version, g_notes;

// Fase 61: tamanhos da janela do jogo (o jogo desenha em 1920x1080 e a imagem e escalada para a janela)
const wchar_t* const kResolutions[] = {L"1280x720", L"1600x900", L"1920x1080", L"2560x1440", L"3840x2160"};
// Idioma do jogo e do port: codigo do --lang, nome no menu (pt / en). Fora pt / en, o port fala ingles.
struct Language {
    const wchar_t *code, *pt, *en;
};
const Language kLanguages[] = {
    {L"auto", L"Autom\u00e1tico (Windows)", L"Automatic (Windows)"},
    {L"pt", L"Portugu\u00eas (Brasil)", L"Portugu\u00eas (Brasil)"},
    {L"en", L"English", L"English"},
    {L"es", L"Espa\u00f1ol", L"Espa\u00f1ol"},
    {L"fr", L"Fran\u00e7ais", L"Fran\u00e7ais"},
    {L"de", L"Deutsch", L"Deutsch"},
    {L"it", L"Italiano", L"Italiano"},
    {L"nl", L"Nederlands", L"Nederlands"},
    {L"da", L"Dansk", L"Dansk"},
};
std::wstring g_res_value = L"1280x720", g_lang_value = L"auto";
std::unique_ptr<Gdiplus::Bitmap> g_art;
HICON g_icon;
HFONT g_font, g_small, g_label, g_title, g_subtitle, g_button, g_play_font;
HBRUSH g_field_brush, g_bg_brush;
fs::path g_root;
bool g_busy = false;
int g_pct = 0;
std::wstring g_step;
struct Status {
    std::wstring game, patch, dlcs, shaders;
    COLORREF game_c, patch_c, dlcs_c, shaders_c;
} g_status;

fs::path Ini() { return g_root / L"launcher.ini"; }

std::wstring GetText(HWND h) {
    std::wstring s(GetWindowTextLengthW(h) + 1, L'\0');
    GetWindowTextW(h, s.data(), static_cast<int>(s.size()));
    s.resize(wcslen(s.c_str()));
    while (!s.empty() && (s.back() == L' ' || s.back() == L'\\' || s.back() == L'"')) {
        s.pop_back();
    }
    while (!s.empty() && s.front() == L'"') {
        s.erase(s.begin());
    }
    return s;
}

std::wstring ReadIni(const wchar_t* key, const wchar_t* fallback = L"") {
    wchar_t buffer[2048];
    GetPrivateProfileStringW(L"launcher", key, fallback, buffer, 2048, Ini().c_str());
    return buffer;
}

void WriteIni(const wchar_t* key, const std::wstring& value) { WritePrivateProfileStringW(L"launcher", key, value.c_str(), Ini().c_str()); }

void AppendLog(const std::wstring& text) {
    if (GetWindowTextLengthW(g_log) > 60000) {
        SendMessageW(g_log, EM_SETSEL, 0, 20000);
        SendMessageW(g_log, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(L""));
    }
    const int end = GetWindowTextLengthW(g_log);
    SendMessageW(g_log, EM_SETSEL, end, end);
    SendMessageW(g_log, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>((text + L"\r\n").c_str()));
}

void InvalidateCard() {
    RECT r{0, S(kHero - 70), S(kW), S(kH)};
    InvalidateRect(g_main, &r, FALSE);
}

// ---- arte do jogo: PNG do proprio dump ----
void LoadArt(const fs::path& game) {
    g_art.reset();
    for (const wchar_t* name : {L"pic1.png", L"pic0.png", L"icon0.png"}) {
        std::error_code ec;
        const fs::path file = game / L"sce_sys" / name;
        if (!game.empty() && fs::is_regular_file(file, ec)) {
            auto bitmap = std::make_unique<Gdiplus::Bitmap>(file.c_str());
            if (bitmap->GetLastStatus() == Gdiplus::Ok && bitmap->GetWidth()) {
                g_art = std::move(bitmap);
                break;
            }
        }
    }
    RECT r{0, 0, S(kW), S(kHero)};
    InvalidateRect(g_main, &r, FALSE);
}

void PaintHero(HDC dc, Gdiplus::Graphics& g) {
    const int w = S(kW), h = S(kHero);
    if (g_art) {
        // "cobrir": escala para preencher e corta o excesso no meio
        const float scale = std::max(static_cast<float>(w) / g_art->GetWidth(), static_cast<float>(h) / g_art->GetHeight());
        const float sw = w / scale, sh = h / scale;
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.DrawImage(g_art.get(), Gdiplus::RectF(0, 0, static_cast<float>(w), static_cast<float>(h)), (g_art->GetWidth() - sw) / 2,
                    (g_art->GetHeight() - sh) / 2, sw, sh, Gdiplus::UnitPixel);
    } else {
        Gdiplus::LinearGradientBrush bg(Gdiplus::Point(0, 0), Gdiplus::Point(w, h), ui::C(RGB(14, 24, 64)), ui::C(RGB(70, 22, 110)));
        g.FillRectangle(&bg, 0, 0, w, h);
        for (int k = 0; k < 6; ++k) { // aneis do portal, de enfeite
            Gdiplus::Pen pen(ui::C(k % 2 ? ui::kBlue : RGB(120, 220, 255), static_cast<BYTE>(70 - k * 9)), static_cast<float>(S(10 - k)));
            const int r = S(110.0f + k * 38);
            g.DrawEllipse(&pen, w - S(260) - r, S(170) - r, 2 * r, 2 * r);
        }
    }
    // degrade: escurece embaixo (funde com o cartao) e na esquerda (titulo legivel)
    Gdiplus::LinearGradientBrush down(Gdiplus::Point(0, h / 3), Gdiplus::Point(0, h + 1), ui::C(ui::kBg, 0), ui::C(ui::kBg, 255));
    g.FillRectangle(&down, 0, h / 3, w, h - h / 3 + 1);
    Gdiplus::LinearGradientBrush left(Gdiplus::Point(0, 0), Gdiplus::Point(w * 2 / 3, 0), ui::C(ui::kBg, 200), ui::C(ui::kBg, 0));
    g.FillRectangle(&left, 0, 0, w * 2 / 3, h);
    g.Flush();
    DrawIconEx(dc, S(40), S(34), g_icon, S(56), S(56), 0, nullptr, DI_NORMAL);
    RECT title{S(40), S(118), w, S(186)}, sub{S(42), S(184), w, S(214)};
    ui::Text(dc, L"LEGO® DIMENSIONS", title, g_title, RGB(255, 255, 255));
    ui::Text(dc, ui::T(L"Port para PC   •   60 FPS   •   DLCs   •   shaders prontos antes de jogar", L"PC port   •   60 FPS   •   DLCs   •   shaders ready before you play"), sub, g_subtitle, ui::kMuted);
    if (!g_art) {
        RECT hint{S(42), S(222), S(620), S(262)};
        ui::Text(dc, ui::T(L"Escolha a pasta do jogo abaixo para ver a arte do LEGO Dimensions aqui.", L"Choose the game folder below to see the LEGO Dimensions art here."), hint, g_small, ui::kMuted);
    }
}

void PaintCard(HDC dc, Gdiplus::Graphics& g) {
    RECT card{S(24), S(kHero - 64), S(kW - 24), S(kH - 104)};
    Gdiplus::SolidBrush panel(ui::C(ui::kPanel, 245));
    ui::FillRound(g, card, static_cast<float>(S(16)), panel);
    ui::StrokeRound(g, card, static_cast<float>(S(16)), ui::kBorder, 1.0f, 160);
    ui::FieldBox(g, g_main, g_game, g_focus == g_game);
    ui::FieldBox(g, g_main, g_patch, g_focus == g_patch);
    ui::FieldBox(g, g_main, g_dlc, g_focus == g_dlc);
    ui::FieldBox(g, g_main, g_log, false);
    RECT bar{S(48), S(kH - 196), S(kW - 120), S(kH - 188)};
    ui::Progress(g, bar, g_pct);
    g.Flush();
    RECT l1{S(48), S(kHero - 46), S(900), S(kHero - 24)}, lp{S(48), S(kHero + 34), S(900), S(kHero + 56)},
        l2{S(48), S(kHero + 114), S(900), S(kHero + 136)};
    ui::Text(dc, ui::T(L"PASTA DO JOGO  —  dump do PS4 (CUSA01176), a pasta com eboot.bin", L"GAME FOLDER  —  PS4 dump (CUSA01176), the folder with eboot.bin"), l1, g_label, ui::kMuted);
    ui::Text(dc, ui::T(L"ATUALIZA\u00c7\u00c3O  —  s\u00f3 se ela estiver numa pasta separada (CUSA01176-patch); achada sozinha", L"UPDATE  —  only if it is in a separate folder (CUSA01176-patch); found automatically"), lp, g_label,
             ui::kMuted);
    ui::Text(dc, ui::T(L"PASTA DAS DLCs  —  opcional, uma subpasta por DLC (como a addcont do shadPS4)", L"DLC FOLDER  —  optional, one subfolder per DLC (like the shadPS4 addcont)"), l2, g_label, ui::kMuted);
    int x = S(48);
    const int y = S(kHero + 198);
    x += ui::Pill(dc, g, x, y, g_status.game, g_status.game_c, g_small) + S(10);
    x += ui::Pill(dc, g, x, y, g_status.patch, g_status.patch_c, g_small) + S(10);
    x += ui::Pill(dc, g, x, y, g_status.dlcs, g_status.dlcs_c, g_small) + S(10);
    ui::Pill(dc, g, x, y, g_status.shaders, g_status.shaders_c, g_small);
    RECT pct{S(kW - 110), S(kH - 204), S(kW - 48), S(kH - 180)};
    ui::Text(dc, g_pct ? std::to_wstring(g_pct) + L"%" : L"", pct, g_small, ui::kMuted, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    RECT step{S(48), S(kH - 226), S(kW - 120), S(kH - 202)};
    ui::Text(dc, g_step, step, g_small, ui::kText);
}

void Paint(HDC target) {
    RECT client;
    GetClientRect(g_main, &client);
    ui::Buffered buffer(target, client);
    HBRUSH bg = CreateSolidBrush(ui::kBg);
    FillRect(buffer.dc, &client, bg);
    DeleteObject(bg);
    Gdiplus::Graphics g(buffer.dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    PaintHero(buffer.dc, g);
    PaintCard(buffer.dc, g);
}

// ---- estado ----
bool Prepared() {
    std::error_code ec;
    return fs::is_regular_file(g_root / L"assets" / L"decrypted_elfs" / L"eboot.bin.elf", ec);
}

bool AotReady() {
    std::error_code ec;
    return fs::is_regular_file(g_root / L"assets" / L"shaders_aot" / L"aot_pairs.pak", ec);
}

// Fase 58: atualizacao separada (formato do shadPS4), dentro da pasta do jogo ou ao lado dela.
fs::path FindPatch(const fs::path& game) {
    std::error_code ec;
    if (game.empty()) {
        return {};
    }
    const std::wstring name = game.filename().wstring();
    for (const fs::path& dir : {game / (name + L"-patch"), game.parent_path() / (name + L"-patch"), game / L"CUSA01176-patch",
                                game.parent_path() / L"CUSA01176-patch"}) {
        if (fs::is_regular_file(dir / L"eboot.bin", ec) || fs::is_regular_file(dir / L"sce_sys" / L"param.sfo", ec)) {
            return dir;
        }
    }
    return {};
}

// Arquivos .DAT da atualizacao (PATCH*.DAT) na pasta: o dump ja traz a atualizacao junto?
bool HasPatchDats(const fs::path& dir) {
    std::error_code ec;
    for (fs::directory_iterator chunk(dir / L"data", ec), end; !ec && chunk != end; chunk.increment(ec)) {
        std::error_code ec2;
        for (fs::directory_iterator f(chunk->path(), ec2), e2; !ec2 && f != e2; f.increment(ec2)) {
            const std::wstring n = f->path().filename().wstring();
            if (n.size() > 5 && _wcsnicmp(n.c_str(), L"PATCH", 5) == 0) {
                return true;
            }
        }
    }
    return false;
}

int CountDlcs(const fs::path& dir) {
    int n = 0;
    std::error_code ec;
    if (dir.empty() || !fs::is_directory(dir, ec)) {
        return 0;
    }
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
        n += fs::is_regular_file(it->path() / L"sce_sys" / L"param.sfo", ec);
    }
    return n;
}

void UpdateStatus() {
    const fs::path game = GetText(g_game), dlc = GetText(g_dlc);
    std::error_code ec;
    const bool game_ok = !game.empty() && fs::is_regular_file(game / L"eboot.bin", ec) && fs::is_regular_file(game / L"sce_sys" / L"param.sfo", ec);
    g_status.game = game.empty() ? ui::T(L"Escolha a pasta do jogo", L"Choose the game folder") : game_ok ? ui::T(L"Jogo encontrado", L"Game found") : ui::T(L"Pasta sem eboot.bin", L"Folder without eboot.bin");
    g_status.game_c = game.empty() ? ui::kAmber : game_ok ? ui::kGreen : ui::kRed;
    const fs::path patch = GetText(g_patch);
    const bool patch_ok = !patch.empty() && (fs::is_regular_file(patch / L"eboot.bin", ec) || fs::is_directory(patch / L"data", ec));
    const bool merged = game_ok && HasPatchDats(game); // atualizacao ja junto do jogo (como no dump mesclado)
    g_status.patch = patch_ok ? ui::T(L"Atualiza\u00e7\u00e3o separada", L"Separate update") : !patch.empty() ? ui::T(L"Atualiza\u00e7\u00e3o: pasta inv\u00e1lida", L"Update: invalid folder")
                     : merged  ? ui::T(L"Atualiza\u00e7\u00e3o no jogo", L"Update included in the game")
                     : game_ok ? ui::T(L"Sem atualiza\u00e7\u00e3o (1.24 necess\u00e1ria)", L"No update (1.24 required)")
                               : ui::T(L"Atualiza\u00e7\u00e3o", L"Update");
    g_status.patch_c = patch_ok || merged ? ui::kGreen : !patch.empty() ? ui::kRed : game_ok ? ui::kAmber : ui::kMuted;
    const int dlcs = CountDlcs(dlc);
    g_status.dlcs = dlcs ? std::to_wstring(dlcs) + (dlcs == 1 ? L" DLC" : L" DLCs") : ui::T(L"Sem DLCs", L"No DLCs");
    g_status.dlcs_c = dlcs ? ui::kBlue : ui::kMuted;
    g_status.shaders = !Prepared() ? ui::T(L"Preparar pendente", L"Prepare pending") : AotReady() ? ui::T(L"Shaders AOT prontos", L"AOT shaders ready") : ui::T(L"Sem shaders AOT", L"No AOT shaders");
    g_status.shaders_c = !Prepared() ? ui::kAmber : AotReady() ? ui::kGreen : ui::kAmber;
    if (!g_busy && (g_step.empty() || g_step.rfind(L"Clique", 0) == 0 || g_step.rfind(L"Tudo", 0) == 0 || g_step.rfind(L"Click", 0) == 0 || g_step.rfind(L"All set", 0) == 0)) { // dica do proximo passo
        g_step = !game_ok ? ui::T(L"Clique em Procurar e escolha a pasta do jogo.", L"Click Browse and choose the game folder.") : !Prepared() || !AotReady()
                     ? ui::T(L"Clique em Preparar: gera os arquivos e traduz os shaders a partir do seu jogo (s\u00f3 na primeira vez).", L"Click Prepare: it builds the files and translates the shaders from your game (first time only).")
                     : ui::T(L"Tudo pronto. Clique em JOGAR!", L"All set. Click PLAY!");
    }
    EnableWindow(g_play, !g_busy && game_ok);
    EnableWindow(g_prepare, !g_busy && game_ok);
    EnableWindow(g_update, !g_busy);
    InvalidateCard();
}

std::wstring BrowseFolder(const wchar_t* title, const std::wstring& start) {
    std::wstring result;
    IFileOpenDialog* dialog = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog)))) {
        DWORD options = 0;
        dialog->GetOptions(&options);
        dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
        dialog->SetTitle(title);
        IShellItem* folder = nullptr;
        if (!start.empty() && SUCCEEDED(SHCreateItemFromParsingName(start.c_str(), nullptr, IID_PPV_ARGS(&folder)))) {
            dialog->SetFolder(folder);
            folder->Release();
        }
        if (SUCCEEDED(dialog->Show(g_main))) {
            IShellItem* item = nullptr;
            if (SUCCEEDED(dialog->GetResult(&item))) {
                PWSTR path = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
                    result = path;
                    CoTaskMemFree(path);
                }
                item->Release();
            }
        }
        dialog->Release();
    }
    return result;
}

void SaveSettings() {
    WriteIni(L"jogo", GetText(g_game));
    WriteIni(L"atualizacao", GetText(g_patch));
    WriteIni(L"dlcs", GetText(g_dlc));
    WriteIni(L"fps60", ui::Checked(g_fps) ? L"1" : L"0");
    WriteIni(L"console", ui::Checked(g_console) ? L"1" : L"0");
    WriteIni(L"telacheia", ui::Checked(g_fullscreen) ? L"1" : L"0");
    WriteIni(L"resolucao", g_res_value);
    WriteIni(L"idioma", g_lang_value);
    WriteIni(L"pularcenas", ui::Checked(g_skip) ? L"1" : L"0");
}

const Language& CurrentLanguage() {
    for (const Language& language : kLanguages) {
        if (g_lang_value == language.code) {
            return language;
        }
    }
    return kLanguages[0];
}

void UpdateChoiceLabels() {
    SetWindowTextW(g_resolution, (ui::T(L"Resolu\u00e7\u00e3o:  ", L"Resolution:  ") + g_res_value + L"  \u25be").c_str());
    SetWindowTextW(g_language, (ui::T(L"Idioma:  ", L"Language:  ") + std::wstring(ui::T(CurrentLanguage().pt, CurrentLanguage().en)) + L"  \u25be").c_str());
    InvalidateRect(g_resolution, nullptr, FALSE);
    InvalidateRect(g_language, nullptr, FALSE);
}

// Menu suspenso logo abaixo do botao; devolve o indice escolhido ou -1.
int PopupChoice(HWND button, const std::vector<std::wstring>& items, int current) {
    HMENU menu = CreatePopupMenu();
    for (size_t i = 0; i < items.size(); ++i) {
        AppendMenuW(menu, MF_STRING | (static_cast<int>(i) == current ? MF_CHECKED : 0), i + 1, items[i].c_str());
    }
    RECT r;
    GetWindowRect(button, &r);
    const int picked = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY, r.left, r.bottom + S(2), 0, g_main, nullptr);
    DestroyMenu(menu);
    return picked - 1;
}

void PickResolution() {
    std::vector<std::wstring> items;
    int current = -1;
    for (const wchar_t* res : kResolutions) {
        if (g_res_value == res) {
            current = static_cast<int>(items.size());
        }
        items.push_back(std::wstring(res) + (std::wstring(res) == L"1280x720" ? ui::T(L"   (padr\u00e3o)", L"   (default)") : L""));
    }
    const int picked = PopupChoice(g_resolution, items, current);
    if (picked >= 0) {
        g_res_value = kResolutions[picked];
        UpdateChoiceLabels();
        SaveSettings();
    }
}

void PickLanguage() {
    std::vector<std::wstring> items;
    int current = 0;
    for (const Language& language : kLanguages) {
        if (g_lang_value == language.code) {
            current = static_cast<int>(items.size());
        }
        items.push_back(ui::T(language.pt, language.en));
    }
    const int picked = PopupChoice(g_language, items, current);
    if (picked < 0 || g_lang_value == kLanguages[picked].code) {
        return;
    }
    g_lang_value = kLanguages[picked].code;
    SaveSettings();
    // Textos do launcher: portugues so com "pt" (ou automatico num Windows em portugues). Se mudar, reabre.
    const bool pt = g_lang_value == L"pt" || (g_lang_value == L"auto" && PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_PORTUGUESE);
    if (pt == ui::Pt() || g_busy) {
        UpdateChoiceLabels();
        return;
    }
    SetEnvironmentVariableW(L"LD_LANG", pt ? L"pt" : L"en");
    wchar_t exe[MAX_PATH * 4];
    GetModuleFileNameW(nullptr, exe, MAX_PATH * 4);
    std::wstring command = L"\"" + std::wstring(exe) + L"\"";
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    if (CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, 0, nullptr, g_root.c_str(), &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        DestroyWindow(g_main);
    } else {
        UpdateChoiceLabels();
    }
}

std::wstring Quote(const std::wstring& s) { return L"\"" + s + L"\""; }

// ---- preparacao: python tools\setup\preparar.py, saida lida por uma thread ----
// Roda um script de tools\setup no Python portatil; a saida chega linha a linha em OnLine e o fim em WM_APP_DONE.
bool RunPython(const wchar_t* script, const std::wstring& args, Task task) {
    fs::path python = g_root / L"tools" / L"python" / L"python.exe";
    std::error_code ec;
    std::wstring command = fs::is_regular_file(python, ec) ? Quote(python.wstring()) : L"python";
    command += L" -u " + Quote((g_root / L"tools" / L"setup" / script).wstring()) + args;
    SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
    HANDLE read_pipe = nullptr, write_pipe = nullptr;
    CreatePipe(&read_pipe, &write_pipe, &sa, 0);
    SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOW si{sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = si.hStdError = write_pipe;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW | BELOW_NORMAL_PRIORITY_CLASS, nullptr, g_root.c_str(),
                        &si, &pi)) {
        AppendLog(ui::T(L"ERRO: nao consegui abrir o Python.", L"ERROR: could not start Python."));
        CloseHandle(read_pipe);
        CloseHandle(write_pipe);
        return false;
    }
    CloseHandle(write_pipe);
    g_busy = true;
    g_task = task;
    g_pct = 1;
    std::thread([read_pipe, pi] {
        std::string pending;
        char buffer[4096];
        DWORD got = 0;
        while (ReadFile(read_pipe, buffer, sizeof(buffer), &got, nullptr) && got) {
            pending.append(buffer, got);
            for (size_t nl; (nl = pending.find('\n')) != std::string::npos;) {
                std::string line = pending.substr(0, nl);
                pending.erase(0, nl + 1);
                while (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                const int n = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), static_cast<int>(line.size()), nullptr, 0);
                auto* text = new std::wstring(n, L'\0');
                MultiByteToWideChar(CP_UTF8, 0, line.c_str(), static_cast<int>(line.size()), text->data(), n);
                PostMessageW(g_main, WM_APP_LINE, 0, reinterpret_cast<LPARAM>(text));
            }
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD code = 1;
        GetExitCodeProcess(pi.hProcess, &code);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(read_pipe);
        PostMessageW(g_main, WM_APP_DONE, code, 0);
    }).detach();
    return true;
}

void RunPrepare() {
    SaveSettings();
    std::wstring args = L" --app0 " + Quote(GetText(g_game));
    const std::wstring dlc = GetText(g_dlc);
    if (!dlc.empty()) {
        args += L" --addcont " + Quote(dlc);
    }
    if (const std::wstring patch = GetText(g_patch); !patch.empty()) {
        args += L" --patch " + Quote(patch); // Fase 58
    }
    if (RunPython(L"preparar.py", args, Task::kPrepare)) {
        g_step = ui::T(L"Preparando... (a tradu\u00e7\u00e3o dos shaders leva de 20 a 60 minutos, s\u00f3 na primeira vez)", L"Preparing... (translating the shaders takes 20 to 60 minutes, first time only)");
        UpdateStatus();
    }
}

// ---- Fase 61: atualizacao pelas releases do GitHub (tools\setup\atualizar.py) ----
bool GameRunning() {
    bool running = false;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W entry{sizeof(entry)};
    for (BOOL ok = Process32FirstW(snap, &entry); ok && !running; ok = Process32NextW(snap, &entry)) {
        running = _wcsicmp(entry.szExeFile, L"lego_dimensions.exe") == 0;
    }
    CloseHandle(snap);
    return running;
}

void CheckUpdate() {
    if (g_busy) {
        return;
    }
    g_new_version.clear();
    g_notes.clear();
    if (RunPython(L"atualizar.py", L" --check", Task::kCheck)) {
        g_step = ui::T(L"Procurando atualiza\u00e7\u00f5es...", L"Looking for updates...");
        UpdateStatus();
    }
}

void ApplyUpdate() {
    if (GameRunning()) {
        MessageBoxW(g_main, ui::T(L"Feche o jogo antes de atualizar.", L"Close the game before updating."), L"LEGO Dimensions PC", MB_ICONINFORMATION);
        return;
    }
    SaveSettings();
    if (RunPython(L"atualizar.py", L" --apply", Task::kUpdate)) {
        g_step = ui::T(L"Atualizando...", L"Updating...");
        UpdateStatus();
    }
}

void RestartLauncher() {
    wchar_t exe[MAX_PATH * 4];
    GetModuleFileNameW(nullptr, exe, MAX_PATH * 4);
    std::wstring command = Quote(exe);
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    if (CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, 0, nullptr, g_root.c_str(), &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        DestroyWindow(g_main);
    }
}

// Sobras de uma atualizacao: arquivos que estavam em uso foram renomeados para *.antigo
void CleanOldFiles() {
    std::error_code ec;
    for (auto it = fs::recursive_directory_iterator(g_root, fs::directory_options::skip_permission_denied, ec); it != fs::recursive_directory_iterator(); it.increment(ec)) {
        if (it->path().filename() == L"assets" && it.depth() == 0) {
            it.disable_recursion_pending();
            continue;
        }
        const std::wstring ext = it->path().extension().wstring();
        if (ext.rfind(L".antigo", 0) == 0 || ext == L".novo") {
            fs::remove(it->path(), ec);
        }
    }
}

std::wstring ReadVersion() {
    std::ifstream in(g_root / L"version.txt");
    std::string text;
    std::getline(in, text);
    while (!text.empty() && (text.back() == '\r' || text.back() == ' ')) {
        text.pop_back();
    }
    return text.empty() ? L"?" : std::wstring(text.begin(), text.end());
}

void OnLine(const std::wstring& line) {
    if (line.rfind(L"VERSAO_NOVA ", 0) == 0) { // Fase 61
        g_new_version = line.substr(12, line.find(L' ', 12) - 12);
        return;
    }
    if (line.rfind(L"NOTA ", 0) == 0) {
        g_notes += L"\u2022 " + line.substr(5) + L"\n";
        return;
    }
    if (line.rfind(L"SEM_ATUALIZACAO", 0) == 0) {
        return;
    }
    if (line.rfind(L"ATUALIZADO ", 0) == 0 || line.rfind(L"arquivos trocados", 0) == 0 || line.rfind(L"files replaced", 0) == 0) {
        AppendLog(line);
        return;
    }
    if (line.rfind(L"PROGRESSO ", 0) == 0) {
        g_pct = std::max(1, _wtoi(line.c_str() + 10));
        const size_t space = line.find(L' ', 10);
        g_step = space == std::wstring::npos ? L"" : line.substr(space + 1);
        g_step[0] = towupper(g_step.empty() ? L' ' : g_step[0]);
        InvalidateCard();
        return;
    }
    if (line.rfind(L"ERRO ", 0) == 0) {
        AppendLog(std::wstring(ui::T(L"ERRO: ", L"ERROR: ")) + line.substr(5));
        g_step = std::wstring(ui::T(L"Erro: ", L"Error: ")) + line.substr(5);
        InvalidateCard();
        MessageBoxW(g_main, line.substr(5).c_str(), L"LEGO Dimensions PC", MB_ICONERROR);
        return;
    }
    if (line.find(L"EXTRAIDO") != std::wstring::npos || line.find(L"COMPILADO") != std::wstring::npos || line.find(L"PLANO") != std::wstring::npos ||
        line.find(L"EXTRACTED") != std::wstring::npos || line.find(L"COMPILED") != std::wstring::npos || line.find(L"PLAN:") == 0 ||
        line.find(L"jogo:") == 0 || line.find(L"game:") == 0 || line.find(L"DLC") != std::wstring::npos || line.find(L"AVISO") == 0 ||
        line.find(L"WARNING") == 0 || line.find(L"eboot") == 0) {
        AppendLog(line);
    }
}

// ---- jogar ----
void RunGame() {
    SaveSettings();
    if (!Prepared()) {
        MessageBoxW(g_main, ui::T(L"Primeiro clique em Preparar: o port precisa gerar os arquivos a partir do seu jogo.", L"First click Prepare: the port needs to build its files from your game."), L"LEGO Dimensions PC", MB_ICONINFORMATION);
        return;
    }
    if (!AotReady() && MessageBoxW(g_main,
                                   ui::T(L"Os shaders AOT ainda nao foram preparados: o jogo vai compilar os shaders na hora (mais pop-in e "
                                         L"travamentos na primeira vez). Jogar assim mesmo?",
                                         L"The AOT shaders have not been prepared yet: the game will compile shaders on the fly "
                                         L"(more pop-in and stutter the first time). Play anyway?"),
                                   L"LEGO Dimensions PC", MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return;
    }
    std::wstring command = Quote((g_root / L"build" / L"lego_dimensions.exe").wstring()) + L" --app0 " + Quote(GetText(g_game));
    const std::wstring dlc = GetText(g_dlc);
    command += dlc.empty() ? L" --no-dlc" : L" --addcont " + Quote(dlc);
    if (const std::wstring patch = GetText(g_patch); !patch.empty()) {
        command += L" --patch " + Quote(patch); // Fase 58
    }
    if (ui::Checked(g_fps)) {
        command += L" --fps60";
    }
    if (ui::Checked(g_fullscreen)) {
        command += L" --fullscreen"; // Fase 60: tela cheia sem bordas (F11 alterna dentro do jogo)
    }
    command += L" --resolution " + g_res_value; // Fase 61: tamanho da janela
    if (g_lang_value != L"auto") {
        command += L" --lang " + g_lang_value; // Fase 61: idioma do jogo (e do port)
    }
    if (ui::Checked(g_skip)) {
        command += L" --skip-cutscenes"; // Fase 61: qualquer cutscene pode ser pulada, mesmo sem 100%
    }
    command += L" --hitch-log";
    const bool console = ui::Checked(g_console);
    STARTUPINFOW si{sizeof(si)};
    HANDLE log = INVALID_HANDLE_VALUE;
    if (!console) { // sem console: a saida vai para build\logs\sessao_*.log (ajuda a achar problemas)
        std::error_code ec;
        fs::create_directories(g_root / L"build" / L"logs", ec);
        SYSTEMTIME t;
        GetLocalTime(&t);
        wchar_t name[64];
        swprintf(name, 64, L"sessao_%04u%02u%02u_%02u%02u%02u.log", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
        SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
        log = CreateFileW((g_root / L"build" / L"logs" / name).c_str(), GENERIC_WRITE, FILE_SHARE_READ, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (log != INVALID_HANDLE_VALUE) {
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdOutput = si.hStdError = log;
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        }
    }
    PROCESS_INFORMATION pi{};
    if (CreateProcessW(nullptr, command.data(), nullptr, nullptr, log != INVALID_HANDLE_VALUE, console ? 0 : CREATE_NO_WINDOW, nullptr, g_root.c_str(),
                       &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        g_step = ui::T(L"Jogo aberto. Bom jogo!", L"Game started. Have fun!");
        InvalidateCard();
        ShowWindow(g_main, SW_MINIMIZE);
    } else {
        AppendLog(ui::T(L"ERRO: nao consegui abrir build\\lego_dimensions.exe", L"ERROR: could not start build\\lego_dimensions.exe"));
    }
    if (log != INVALID_HANDLE_VALUE) {
        CloseHandle(log);
    }
}

void CreateControls() {
    const int field_h = S(24);
    g_game = ui::Edit(g_main, S(58), S(kHero - 12), S(kW - 300), field_h, kGameEdit, g_font);
    ui::Button(g_main, ui::T(L"Procurar...", L"Browse..."), ui::Style::kSecondary, S(kW - 216), S(kHero - 22), S(168), S(44), kGameBrowse, g_button, ui::kPanel);
    g_patch = ui::Edit(g_main, S(58), S(kHero + 68), S(kW - 300), field_h, kPatchEdit, g_font);
    ui::Button(g_main, ui::T(L"Procurar...", L"Browse..."), ui::Style::kSecondary, S(kW - 216), S(kHero + 58), S(168), S(44), kPatchBrowse, g_button, ui::kPanel);
    g_dlc = ui::Edit(g_main, S(58), S(kHero + 148), S(kW - 300), field_h, kDlcEdit, g_font);
    ui::Button(g_main, ui::T(L"Procurar...", L"Browse..."), ui::Style::kSecondary, S(kW - 216), S(kHero + 138), S(168), S(44), kDlcBrowse, g_button, ui::kPanel);
    g_log = ui::Edit(g_main, S(58), S(kH - 170), S(kW - 116), S(60), kLog, g_small, ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL);
    SetWindowTheme(g_log, L"DarkMode_Explorer", nullptr); // barra de rolagem escura (Windows 10 1809+)
    // Fase 61: resolucao, idioma e o mod de pular cutscenes, entre as etiquetas de estado e o progresso
    g_resolution = ui::Button(g_main, L"", ui::Style::kSecondary, S(48), S(kHero + 234), S(250), S(40), kResolution, g_font, ui::kPanel);
    g_language = ui::Button(g_main, L"", ui::Style::kSecondary, S(310), S(kHero + 234), S(300), S(40), kLanguage, g_font, ui::kPanel);
    g_version = ReadVersion();
    g_update = ui::Button(g_main, (ui::T(L"Atualizar   \u2022   v", L"Check updates  \u2022  v") + g_version).c_str(), ui::Style::kSecondary, S(kW - 250), S(kHero + 195),
                          S(202), S(32), kUpdate, g_font, ui::kPanel);
    g_skip = ui::Button(g_main, ui::T(L"Pular todas as cutscenes", L"Skip all cutscenes"), ui::Style::kToggle, S(632), S(kHero + 234), S(300), S(40),
                        kSkipCutscenes, g_font, ui::kPanel);
    g_fps = ui::Button(g_main, L"60 FPS", ui::Style::kToggle, S(40), S(kH - 74), S(140), S(40), kFps, g_font);
    g_console = ui::Button(g_main, ui::T(L"Mostrar console", L"Show console"), ui::Style::kToggle, S(196), S(kH - 74), S(200), S(40), kConsole, g_font);
    g_fullscreen = ui::Button(g_main, ui::T(L"Tela cheia", L"Fullscreen"), ui::Style::kToggle, S(408), S(kH - 74), S(140), S(40), kFullscreen, g_font);
    g_prepare = ui::Button(g_main, ui::T(L"Preparar", L"Prepare"), ui::Style::kSecondary, S(kW - 420), S(kH - 82), S(170), S(54), kPrepare, g_button);
    g_play = ui::Button(g_main, ui::T(L"JOGAR", L"PLAY"), ui::Style::kPrimary, S(kW - 236), S(kH - 82), S(212), S(54), kPlay, g_play_font);
}

void LoadSettings() {
    std::wstring game = ReadIni(L"jogo");
    std::wstring dlc = ReadIni(L"dlcs", L"\x01");
    if (dlc == L"\x01") { // primeira vez: a pasta de DLCs do shadPS4, se existir
        dlc.clear();
        wchar_t appdata[MAX_PATH];
        if (GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH)) {
            const fs::path candidate = fs::path(appdata) / L"shadPS4" / L"addcont" / L"CUSA01176";
            std::error_code ec;
            if (fs::is_directory(candidate, ec)) {
                dlc = candidate.wstring();
            }
        }
    }
    SetWindowTextW(g_game, game.c_str());
    std::wstring patch = ReadIni(L"atualizacao");
    if (patch.empty()) {
        patch = FindPatch(game).wstring(); // Fase 58
    }
    SetWindowTextW(g_patch, patch.c_str());
    SetWindowTextW(g_dlc, dlc.c_str());
    ui::SetChecked(g_fps, ReadIni(L"fps60", L"1") == L"1");
    ui::SetChecked(g_console, ReadIni(L"console", L"0") == L"1");
    ui::SetChecked(g_fullscreen, ReadIni(L"telacheia", L"0") == L"1");
    ui::SetChecked(g_skip, ReadIni(L"pularcenas", L"0") == L"1");
    g_res_value = ReadIni(L"resolucao", L"1280x720");
    if (std::find(std::begin(kResolutions), std::end(kResolutions), g_res_value) == std::end(kResolutions)) {
        g_res_value = L"1280x720";
    }
    g_lang_value = ReadIni(L"idioma", L"auto");
    g_lang_value = CurrentLanguage().code;
    UpdateChoiceLabels();
    LoadArt(game);
    AppendLog(ui::T(L"Bem-vindo! Escolha as pastas, clique em Preparar na primeira vez e depois em JOGAR.", L"Welcome! Choose the folders, click Prepare the first time and then PLAY."));
    UpdateStatus();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        Paint(dc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_DRAWITEM:
        ui::DrawButton(reinterpret_cast<const DRAWITEMSTRUCT*>(lp));
        return TRUE;
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC: {
        HDC dc = reinterpret_cast<HDC>(wp);
        SetBkColor(dc, ui::kField);
        SetTextColor(dc, reinterpret_cast<HWND>(lp) == g_log ? ui::kMuted : ui::kText);
        return reinterpret_cast<LRESULT>(g_field_brush);
    }
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case kGameBrowse: {
            const std::wstring folder = BrowseFolder(ui::T(L"Pasta do jogo (dump CUSA01176)", L"Game folder (CUSA01176 dump)"), GetText(g_game));
            if (!folder.empty()) {
                SetWindowTextW(g_game, folder.c_str());
                LoadArt(folder);
                if (GetText(g_patch).empty()) { // Fase 58: acha a pasta da atualizacao sozinha
                    SetWindowTextW(g_patch, FindPatch(folder).wstring().c_str());
                }
                SaveSettings();
            }
            break;
        }
        case kPatchBrowse: {
            const std::wstring folder = BrowseFolder(ui::T(L"Pasta da atualiza\u00e7\u00e3o (CUSA01176-patch)", L"Update folder (CUSA01176-patch)"), GetText(g_patch).empty() ? GetText(g_game) : GetText(g_patch));
            if (!folder.empty()) {
                SetWindowTextW(g_patch, folder.c_str());
                SaveSettings();
            }
            break;
        }
        case kDlcBrowse: {
            const std::wstring folder = BrowseFolder(ui::T(L"Pasta das DLCs (uma subpasta por DLC)", L"DLC folder (one subfolder per DLC)"), GetText(g_dlc));
            if (!folder.empty()) {
                SetWindowTextW(g_dlc, folder.c_str());
                SaveSettings();
            }
            break;
        }
        case kGameEdit:
        case kPatchEdit:
        case kDlcEdit:
            if (HIWORD(wp) == EN_SETFOCUS || HIWORD(wp) == EN_KILLFOCUS) {
                g_focus = HIWORD(wp) == EN_SETFOCUS ? reinterpret_cast<HWND>(lp) : nullptr;
                if (HIWORD(wp) == EN_KILLFOCUS) {
                    if (LOWORD(wp) == kGameEdit) {
                        LoadArt(GetText(g_game));
                        if (GetText(g_patch).empty()) {
                            SetWindowTextW(g_patch, FindPatch(GetText(g_game)).wstring().c_str());
                        }
                    }
                    SaveSettings();
                }
                InvalidateCard();
            }
            if (HIWORD(wp) == EN_CHANGE) {
                UpdateStatus();
            }
            break;
        case kPrepare:
            if (!Prepared() || MessageBoxW(hwnd, ui::T(L"Ja esta preparado. Preparar de novo (refaz os shaders AOT)?", L"Already prepared. Prepare again (rebuilds the AOT shaders)?"), L"LEGO Dimensions PC",
                                           MB_YESNO | MB_ICONQUESTION) == IDYES) {
                RunPrepare();
            }
            break;
        case kPlay:
            RunGame();
            break;
        case kResolution:
            PickResolution();
            break;
        case kUpdate:
            CheckUpdate();
            break;
        case kLanguage:
            PickLanguage();
            break;
        case kFps:
        case kConsole:
        case kFullscreen:
        case kSkipCutscenes:
            ui::Toggle(reinterpret_cast<HWND>(lp));
            SaveSettings();
            break;
        }
        return 0;
    case WM_APP_LINE: {
        std::wstring* line = reinterpret_cast<std::wstring*>(lp);
        OnLine(*line);
        delete line;
        return 0;
    }
    case WM_APP_DONE:
        g_busy = false;
        if (g_task == Task::kCheck) { // Fase 61
            g_pct = 0;
            g_step.clear();
            UpdateStatus();
            if (wp == 2) {
                MessageBoxW(hwnd, (ui::T(L"Voc\u00ea j\u00e1 tem a vers\u00e3o mais nova (", L"You already have the latest version (") + g_version + L").").c_str(),
                            L"LEGO Dimensions PC", MB_ICONINFORMATION);
            } else if (wp == 0 && !g_new_version.empty()) {
                std::wstring text = ui::T(L"A vers\u00e3o ", L"Version ") + g_new_version + ui::T(L" est\u00e1 dispon\u00edvel (voc\u00ea tem a ", L" is available (you have ") +
                                    g_version + L").\n\n" + g_notes + (g_notes.empty() ? L"" : L"\n") +
                                    ui::T(L"Baixar e instalar agora? Seus saves, os arquivos preparados e as op\u00e7\u00f5es ficam como est\u00e3o; o launcher reabre no fim.",
                                          L"Download and install it now? Your saves, the prepared files and the options stay as they are; the launcher reopens at the end.");
                if (MessageBoxW(hwnd, text.c_str(), L"LEGO Dimensions PC", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    ApplyUpdate();
                }
            }
            return 0;
        }
        if (g_task == Task::kUpdate) {
            UpdateStatus();
            if (wp == 0) {
                MessageBoxW(hwnd, ui::T(L"Atualizado! O launcher vai reabrir.", L"Updated! The launcher will reopen."), L"LEGO Dimensions PC", MB_ICONINFORMATION);
                RestartLauncher();
            } else {
                AppendLog(ui::T(L"A atualiza\u00e7\u00e3o falhou (veja acima). Nada do que j\u00e1 estava funcionando foi apagado.",
                                L"The update failed (see above). Nothing that was already working was removed."));
            }
            return 0;
        }
        if (wp == 0) {
            g_pct = 100;
            g_step = ui::T(L"Pronto! Clique em JOGAR.", L"Done! Click PLAY.");
            AppendLog(ui::T(L"Preparacao concluida.", L"Preparation finished."));
        } else {
            AppendLog(ui::T(L"A preparacao terminou com erro (veja acima).", L"The preparation failed (see above)."));
        }
        UpdateStatus();
        return 0;
    case WM_CLOSE:
        if (g_busy && MessageBoxW(hwnd, ui::T(L"A preparacao ainda esta rodando. Fechar mesmo assim?", L"The preparation is still running. Close anyway?"), L"LEGO Dimensions PC", MB_YESNO) != IDYES) {
            return 0;
        }
        SaveSettings();
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ui::InitProcess();
    wchar_t exe[MAX_PATH * 4];
    GetModuleFileNameW(nullptr, exe, MAX_PATH * 4);
    g_root = fs::path(exe).parent_path();
    // Fase 61: idioma escolhido no launcher (launcher.ini); "auto" segue LD_LANG / Windows
    if (const std::wstring choice = ReadIni(L"idioma", L"auto"); choice != L"auto" && !choice.empty()) {
        SetEnvironmentVariableW(L"LD_LANG", choice == L"pt" ? L"pt" : L"en");
    }
    SetEnvironmentVariableW(L"LD_LANG", ui::Pt() ? L"pt" : L"en"); // Fase 60
    SetEnvironmentVariableW(L"PYTHONIOENCODING", L"utf-8"); // Fase 60: o log da preparacao chega em UTF-8 (nomes de DLC)
    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
    const RECT place = ui::FitWindow(kW, kH, style); // Fase 61: cabe em telas pequenas
    g_font = ui::Font(15);
    g_small = ui::Font(13);
    g_label = ui::Font(11.5f, FW_SEMIBOLD);
    g_title = ui::Font(44, FW_BLACK);
    g_subtitle = ui::Font(15, FW_SEMIBOLD);
    g_button = ui::Font(15, FW_SEMIBOLD);
    g_play_font = ui::Font(21, FW_BLACK);
    g_field_brush = CreateSolidBrush(ui::kField);
    g_bg_brush = CreateSolidBrush(ui::kBg);
    g_icon = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(1), IMAGE_ICON, S(56), S(56), 0));
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = g_bg_brush;
    wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1));
    wc.hIconSm = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(1), IMAGE_ICON, S(16), S(16), 0));
    wc.lpszClassName = L"LegoDimensionsPcLauncher";
    RegisterClassExW(&wc);
    g_main = CreateWindowExW(0, wc.lpszClassName, L"LEGO Dimensions PC", style, place.left, place.top, place.right, place.bottom,
                             nullptr, nullptr, instance, nullptr);
    ui::DarkTitle(g_main);
    CleanOldFiles(); // Fase 61
    CreateControls();
    LoadSettings();
    ShowWindow(g_main, show);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        if (!IsDialogMessageW(g_main, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    CoUninitialize();
    return 0;
}
