// Fase 63: LD PC Transfer. Converte saves do LEGO Dimensions entre o port / PS4 / shadPS4, Xbox 360 (Xenia),
// Wii U (Cemu) e PS3 (RPCS3). A conversao e do tools\setup\ld_transfer.py; esta janela so escolhe as pastas,
// mostra o que achou na origem e guarda num backup o que o destino ja tinha.
#define UNICODE
#define _UNICODE
#include "ui.h"

#include <shlobj.h>
#include <shobjidl.h>
#include <uxtheme.h>

#include <filesystem>
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

constexpr int kW = 820, kH = 660;
constexpr UINT WM_APP_LINE = WM_APP + 1, WM_APP_DONE = WM_APP + 2;
enum Id { kSrcEdit = 100, kSrcBrowse, kTarget, kSlot, kDestEdit, kDestBrowse, kLog, kConvert };

// Destinos: formato do ld_transfer.py, nome no menu e a dica de onde fica a pasta do save nessa plataforma.
struct Target {
    const wchar_t *format, *pt, *en, *hint_pt, *hint_en;
    bool port;
};
const Target kTargets[] = {
    {L"ps4", L"Este port (LEGO Dimensions PC)", L"This port (LEGO Dimensions PC)",
     L"Grava num slot do port. Depois é só abrir o jogo e escolher Carregar Jogo.",
     L"Writes into a slot of the port. Then open the game and choose Load Game.", true},
    {L"ps4", L"PS4 / shadPS4", L"PS4 / shadPS4",
     L"shadPS4: user\\savedata\\1\\CUSA01176\\Slot00 (ou CUSA00935). Num PS4 de verdade o save precisa ser reassinado (Apollo / Save Wizard).",
     L"shadPS4: user\\savedata\\1\\CUSA01176\\Slot00 (or CUSA00935). A real PS4 needs the save re-signed (Apollo / Save Wizard).", false},
    {L"x360", L"Xbox 360 (Xenia)", L"Xbox 360 (Xenia)",
     L"Xenia ou qualquer save no formato do Xbox 360: a pasta do save do LEGO Dimensions (a que tem GAME01 e V2GAME01).",
     L"Xenia or any save in the Xbox 360 format: the LEGO Dimensions save folder (the one with GAME01 and V2GAME01).", false},
    {L"wiiu", L"Wii U (Cemu)", L"Wii U (Cemu)", L"Cemu: mlc01\\usr\\save\\00050000\\10195d00\\user\\80000001\\Slot1 (jogo com as atualizações).",
     L"Cemu: mlc01\\usr\\save\\00050000\\10195d00\\user\\80000001\\Slot1 (game with the updates).", false},
    {L"ps3", L"PS3 (RPCS3)  —  experimental", L"PS3 (RPCS3)  —  experimental",
     L"RPCS3: dev_hdd0\\home\\00000001\\savedata\\BLUS31473000. O formato do PS3 foi deduzido dos outros e não foi testado.",
     L"RPCS3: dev_hdd0\\home\\00000001\\savedata\\BLUS31473000. The PS3 layout was inferred from the others and is untested.", false},
};
constexpr int kSlots = 4;

enum class Task { kDetect, kConvert } g_task = Task::kDetect;
HWND g_main, g_src, g_target, g_slot, g_dest, g_dest_browse, g_log, g_convert, g_focus;
HFONT g_font, g_small, g_label, g_title, g_subtitle, g_button, g_big;
HBRUSH g_field_brush, g_bg_brush;
HICON g_icon;
fs::path g_root;
int g_target_index = 0, g_slot_index = 0, g_written = 0;
bool g_busy = false, g_detect_again = false;
std::wstring g_detect_text, g_backup;
COLORREF g_detect_color = ui::kAmber;

fs::path Ini() { return g_root / L"launcher.ini"; }
std::wstring ReadIni(const wchar_t* key, const wchar_t* fallback = L"") {
    wchar_t buffer[2048];
    GetPrivateProfileStringW(L"transfer", key, fallback, buffer, 2048, Ini().c_str());
    return buffer;
}
void WriteIni(const wchar_t* key, const std::wstring& value) { WritePrivateProfileStringW(L"transfer", key, value.c_str(), Ini().c_str()); }

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

std::wstring Quote(const std::wstring& s) { return L"\"" + s + L"\""; }

void AppendLog(const std::wstring& text) {
    const int end = GetWindowTextLengthW(g_log);
    SendMessageW(g_log, EM_SETSEL, end, end);
    SendMessageW(g_log, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>((text + L"\r\n").c_str()));
}

const Target& CurrentTarget() { return kTargets[g_target_index]; }
fs::path PortSaves() { return g_root / L"assets" / L"savedata0"; }
fs::path SlotDir(int slot) {
    wchar_t name[16];
    swprintf_s(name, L"Slot%02d", slot);
    return PortSaves() / name;
}
bool SlotUsed(int slot) {
    std::error_code ec;
    return fs::is_regular_file(SlotDir(slot) / L"V2GAME01", ec) || fs::is_regular_file(SlotDir(slot) / L"GAME01", ec);
}
fs::path Destination() { return CurrentTarget().port ? SlotDir(g_slot_index) : fs::path(GetText(g_dest)); }

// ---- desenho ----
void Paint(HDC target) {
    RECT client;
    GetClientRect(g_main, &client);
    ui::Buffered buffer(target, client);
    HDC dc = buffer.dc;
    HBRUSH bg = CreateSolidBrush(ui::kBg);
    FillRect(dc, &client, bg);
    DeleteObject(bg);
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    {
        // faixa do topo: degrade do portal, como o launcher sem a arte do jogo
        Gdiplus::LinearGradientBrush top(Gdiplus::Point(0, 0), Gdiplus::Point(S(kW), S(120)), ui::C(RGB(14, 24, 64)), ui::C(RGB(70, 22, 110)));
        g.FillRectangle(&top, 0, 0, S(kW), S(120));
        Gdiplus::LinearGradientBrush fade(Gdiplus::Point(0, S(40)), Gdiplus::Point(0, S(121)), ui::C(ui::kBg, 0), ui::C(ui::kBg, 255));
        g.FillRectangle(&fade, 0, S(40), S(kW), S(81));
        RECT card{S(24), S(112), S(kW - 24), S(kH - 104)};
        Gdiplus::SolidBrush panel(ui::C(ui::kPanel, 245));
        ui::FillRound(g, card, static_cast<float>(S(16)), panel);
        ui::StrokeRound(g, card, static_cast<float>(S(16)), ui::kBorder, 1.0f, 160);
        ui::FieldBox(g, g_main, g_src, g_focus == g_src);
        if (!CurrentTarget().port) {
            ui::FieldBox(g, g_main, g_dest, g_focus == g_dest);
        }
        ui::FieldBox(g, g_main, g_log, false);
        g.Flush();
    }
    DrawIconEx(dc, S(32), S(26), g_icon, S(48), S(48), 0, nullptr, DI_NORMAL);
    RECT title{S(96), S(20), S(kW), S(62)}, sub{S(98), S(60), S(kW - 24), S(86)};
    ui::Text(dc, L"LD PC Transfer", title, g_title, RGB(255, 255, 255));
    ui::Text(dc, ui::T(L"Leva o progresso do LEGO Dimensions entre o port, PS4, shadPS4, Xbox 360, Wii U e PS3",
                       L"Moves LEGO Dimensions progress between the port, PS4, shadPS4, Xbox 360, Wii U and PS3"),
             sub, g_subtitle, ui::kMuted);
    RECT l1{S(48), S(130), S(kW - 48), S(150)}, l2{S(48), S(258), S(kW - 48), S(278)};
    ui::Text(dc, ui::T(L"DE ONDE  —  a pasta do save (ou uma pasta acima dela)", L"FROM  —  the save folder (or a folder above it)"), l1, g_label, ui::kMuted);
    ui::Text(dc, ui::T(L"PARA ONDE", L"TO"), l2, g_label, ui::kMuted);
    ui::Pill(dc, g, S(48), S(210), g_detect_text, g_detect_color, g_small);
    const int hint_y = CurrentTarget().port ? 344 : 388; // no port nao tem a linha da pasta de destino
    RECT hint{S(48), S(hint_y), S(kW - 48), S(hint_y + 44)};
    ui::Text(dc, ui::T(CurrentTarget().hint_pt, CurrentTarget().hint_en), hint, g_small, ui::kMuted, DT_LEFT | DT_TOP | DT_WORDBREAK);
    RECT note{S(40), S(kH - 78), S(kW - 270), S(kH - 30)};
    ui::Text(dc, ui::T(L"Só o progresso é convertido; as opções (volume, legendas...) voltam ao padrão. O que o destino já tinha vai para a pasta saves_backup.",
                       L"Only the progress is converted; options (volume, subtitles...) go back to default. Whatever the destination had goes to the saves_backup folder."),
             note, g_small, ui::kMuted, DT_LEFT | DT_TOP | DT_WORDBREAK);
}

void Invalidate() { InvalidateRect(g_main, nullptr, FALSE); }

void UpdateLabels() {
    SetWindowTextW(g_target, (std::wstring(ui::T(CurrentTarget().pt, CurrentTarget().en)) + L"  ▾").c_str());
    std::wstring slot = ui::T(L"Slot ", L"Slot ") + std::to_wstring(g_slot_index + 1) +
                        (SlotUsed(g_slot_index) ? ui::T(L"  (tem save: vai para o backup)", L"  (has a save: goes to the backup)") : ui::T(L"  (vazio)", L"  (empty)")) + L"  ▾";
    SetWindowTextW(g_slot, slot.c_str());
    const bool port = CurrentTarget().port;
    ShowWindow(g_slot, port ? SW_SHOW : SW_HIDE);
    ShowWindow(g_dest, port ? SW_HIDE : SW_SHOW);
    ShowWindow(g_dest_browse, port ? SW_HIDE : SW_SHOW);
    const bool ready = !g_busy && g_detect_color == ui::kGreen && (port || !GetText(g_dest).empty());
    EnableWindow(g_convert, ready);
    InvalidateRect(g_target, nullptr, FALSE);
    InvalidateRect(g_slot, nullptr, FALSE);
    Invalidate();
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

// ---- Python (o mesmo esquema do launcher: saida linha a linha em WM_APP_LINE, fim em WM_APP_DONE) ----
bool RunPython(const std::wstring& args, Task task) {
    fs::path python = g_root / L"tools" / L"python" / L"python.exe";
    std::error_code ec;
    std::wstring command = fs::is_regular_file(python, ec) ? Quote(python.wstring()) : L"python";
    command += L" -u " + Quote((g_root / L"tools" / L"setup" / L"ld_transfer.py").wstring()) + args + (ui::Pt() ? L" --lingua pt" : L" --lingua en");
    SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
    HANDLE read_pipe = nullptr, write_pipe = nullptr;
    CreatePipe(&read_pipe, &write_pipe, &sa, 0);
    SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOW si{sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = si.hStdError = write_pipe;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, g_root.c_str(), &si, &pi)) {
        CloseHandle(read_pipe);
        CloseHandle(write_pipe);
        return false;
    }
    CloseHandle(write_pipe);
    g_busy = true;
    g_task = task;
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

void Detect() {
    const std::wstring src = GetText(g_src);
    if (src.empty()) {
        g_detect_text = ui::T(L"Escolha a pasta do save de origem", L"Choose the source save folder");
        g_detect_color = ui::kAmber;
        UpdateLabels();
        return;
    }
    if (g_busy) {
        g_detect_again = true; // termina o que esta rodando e detecta de novo
        return;
    }
    g_detect_text = ui::T(L"Lendo o save...", L"Reading the save...");
    g_detect_color = ui::kBlue;
    if (!RunPython(L" --detectar " + Quote(src), Task::kDetect)) {
        g_detect_text = ui::T(L"Não consegui abrir o Python", L"Could not start Python");
        g_detect_color = ui::kRed;
    }
    UpdateLabels();
}

const wchar_t* PlatformName(const std::wstring& key) {
    if (key == L"ps4") {
        return ui::T(L"PS4 / shadPS4 / este port", L"PS4 / shadPS4 / this port");
    }
    if (key == L"x360") {
        return L"Xbox 360";
    }
    if (key == L"wiiu") {
        return L"Wii U";
    }
    return L"PS3";
}

void OnLine(const std::wstring& line) {
    if (line.rfind(L"ORIGEM ", 0) == 0) {
        // ORIGEM <formato> <versao do bloco> <DLCs>
        wchar_t key[16] = {};
        int version = 0, dlcs = 0;
        swscanf_s(line.c_str() + 7, L"%15s %d %d", key, static_cast<unsigned>(std::size(key)), &version, &dlcs);
        g_detect_text = std::wstring(ui::T(L"Save de ", L"Save from ")) + PlatformName(key) + L"   •   " + std::to_wstring(dlcs) +
                        ui::T(L" arquivos de DLC", L" DLC files");
        g_detect_color = ui::kGreen;
        if (g_task == Task::kConvert) {
            AppendLog(g_detect_text);
        }
    } else if (line.rfind(L"ERRO ", 0) == 0) {
        g_detect_text = line.substr(5);
        g_detect_color = ui::kRed;
        if (g_task == Task::kConvert) {
            AppendLog(ui::T(L"ERRO: ", L"ERROR: ") + line.substr(5));
            MessageBoxW(g_main, line.substr(5).c_str(), L"LD PC Transfer", MB_ICONERROR);
        }
    } else if (line.rfind(L"BACKUP ", 0) == 0) {
        g_backup = line.substr(7);
        AppendLog(ui::T(L"O que o destino tinha foi guardado em: ", L"What the destination had was kept in: ") + g_backup);
    } else if (line.rfind(L"GRAVADO ", 0) == 0) {
        ++g_written;
    } else if (line.rfind(L"OK ", 0) != 0 && !line.empty()) {
        AppendLog(line); // mensagem inesperada do Python (erro de verdade): mostra como veio
    }
    UpdateLabels();
}

void Convert() {
    const fs::path dest = Destination();
    const std::wstring src = GetText(g_src);
    std::error_code ec;
    if (fs::equivalent(fs::path(src), dest, ec)) {
        MessageBoxW(g_main, ui::T(L"A origem e o destino são a mesma pasta.", L"Source and destination are the same folder."), L"LD PC Transfer", MB_ICONWARNING);
        return;
    }
    if (CurrentTarget().port && SlotUsed(g_slot_index) &&
        MessageBoxW(g_main, ui::T(L"Esse slot já tem um save. Ele vai para a pasta saves_backup e o convertido entra no lugar. Continuar?",
                                  L"That slot already has a save. It goes to the saves_backup folder and the converted one takes its place. Continue?"),
                    L"LD PC Transfer", MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return;
    }
    g_written = 0;
    g_backup.clear();
    AppendLog(std::wstring(ui::T(L"Convertendo para ", L"Converting to ")) + ui::T(CurrentTarget().pt, CurrentTarget().en) + L":  " + dest.wstring());
    const std::wstring args = L" --origem " + Quote(src) + L" --destino " + Quote(dest.wstring()) + L" --formato " + CurrentTarget().format +
                              L" --backup " + Quote((g_root / L"saves_backup").wstring());
    if (!RunPython(args, Task::kConvert)) {
        AppendLog(ui::T(L"ERRO: não consegui abrir o Python.", L"ERROR: could not start Python."));
    }
    UpdateLabels();
}

void SaveSettings() {
    WriteIni(L"origem", GetText(g_src));
    WriteIni(L"destino", GetText(g_dest));
    WriteIni(L"alvo", std::to_wstring(g_target_index));
    WriteIni(L"slot", std::to_wstring(g_slot_index));
}

void CreateControls() {
    const int field_h = S(24);
    g_src = ui::Edit(g_main, S(58), S(166), S(kW - 300), field_h, kSrcEdit, g_font);
    ui::Button(g_main, ui::T(L"Procurar...", L"Browse..."), ui::Style::kSecondary, S(kW - 216), S(156), S(168), S(44), kSrcBrowse, g_button, ui::kPanel);
    g_target = ui::Button(g_main, L"", ui::Style::kSecondary, S(48), S(286), S(330), S(40), kTarget, g_small, ui::kPanel);
    g_slot = ui::Button(g_main, L"", ui::Style::kSecondary, S(392), S(286), S(380), S(40), kSlot, g_small, ui::kPanel);
    g_dest = ui::Edit(g_main, S(58), S(348), S(kW - 300), field_h, kDestEdit, g_font);
    g_dest_browse = ui::Button(g_main, ui::T(L"Procurar...", L"Browse..."), ui::Style::kSecondary, S(kW - 216), S(338), S(168), S(44), kDestBrowse, g_button, ui::kPanel);
    g_log = ui::Edit(g_main, S(58), S(446), S(kW - 116), S(84), kLog, g_small, ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL);
    SetWindowTheme(g_log, L"DarkMode_Explorer", nullptr);
    g_convert = ui::Button(g_main, ui::T(L"CONVERTER", L"CONVERT"), ui::Style::kPrimary, S(kW - 236), S(kH - 82), S(212), S(54), kConvert, g_big);
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
        case kSrcBrowse: {
            const std::wstring folder = BrowseFolder(ui::T(L"Pasta do save de origem", L"Source save folder"), GetText(g_src));
            if (!folder.empty()) {
                SetWindowTextW(g_src, folder.c_str());
                SaveSettings();
                Detect();
            }
            break;
        }
        case kDestBrowse: {
            const std::wstring folder = BrowseFolder(ui::T(L"Pasta do save de destino", L"Destination save folder"), GetText(g_dest));
            if (!folder.empty()) {
                SetWindowTextW(g_dest, folder.c_str());
                SaveSettings();
                UpdateLabels();
            }
            break;
        }
        case kTarget: {
            std::vector<std::wstring> items;
            for (const Target& t : kTargets) {
                items.push_back(ui::T(t.pt, t.en));
            }
            if (const int picked = PopupChoice(g_target, items, g_target_index); picked >= 0) {
                g_target_index = picked;
                SaveSettings();
                UpdateLabels();
            }
            break;
        }
        case kSlot: {
            std::vector<std::wstring> items;
            for (int i = 0; i < kSlots; ++i) {
                items.push_back(ui::T(L"Slot ", L"Slot ") + std::to_wstring(i + 1) + (SlotUsed(i) ? ui::T(L"  (tem save)", L"  (has a save)") : ui::T(L"  (vazio)", L"  (empty)")));
            }
            if (const int picked = PopupChoice(g_slot, items, g_slot_index); picked >= 0) {
                g_slot_index = picked;
                SaveSettings();
                UpdateLabels();
            }
            break;
        }
        case kSrcEdit:
        case kDestEdit:
            if (HIWORD(wp) == EN_SETFOCUS || HIWORD(wp) == EN_KILLFOCUS) {
                g_focus = HIWORD(wp) == EN_SETFOCUS ? reinterpret_cast<HWND>(lp) : nullptr;
                if (HIWORD(wp) == EN_KILLFOCUS) {
                    SaveSettings();
                    if (LOWORD(wp) == kSrcEdit) {
                        Detect();
                    }
                }
                UpdateLabels();
            } else if (HIWORD(wp) == EN_CHANGE && LOWORD(wp) == kDestEdit) {
                UpdateLabels();
            }
            break;
        case kConvert:
            Convert();
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
        if (g_task == Task::kConvert && wp == 0) {
            std::wstring text = ui::T(L"Pronto! ", L"Done! ") + std::to_wstring(g_written) + ui::T(L" arquivos gravados em:\n", L" files written to:\n") + Destination().wstring();
            if (CurrentTarget().port) {
                text += ui::T(L"\n\nAbra o jogo e escolha Carregar Jogo.", L"\n\nOpen the game and choose Load Game.");
            }
            AppendLog(ui::T(L"Pronto: ", L"Done: ") + std::to_wstring(g_written) + ui::T(L" arquivos gravados.", L" files written."));
            MessageBoxW(hwnd, text.c_str(), L"LD PC Transfer", MB_ICONINFORMATION);
        }
        if (g_detect_again) {
            g_detect_again = false;
            Detect();
        }
        UpdateLabels();
        return 0;
    case WM_CLOSE:
        if (g_busy && g_task == Task::kConvert) {
            return 0; // nao fecha no meio da gravacao
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
    // mesmo idioma escolhido no launcher
    wchar_t lang[16] = {};
    GetPrivateProfileStringW(L"launcher", L"idioma", L"auto", lang, 16, Ini().c_str());
    if (std::wstring(lang) != L"auto" && lang[0] && !GetEnvironmentVariableW(L"LD_LANG", nullptr, 0)) {
        SetEnvironmentVariableW(L"LD_LANG", std::wstring(lang) == L"pt" ? L"pt" : L"en");
    }
    SetEnvironmentVariableW(L"PYTHONIOENCODING", L"utf-8");
    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
    const RECT place = ui::FitWindow(kW, kH, style);
    g_font = ui::Font(15);
    g_small = ui::Font(13);
    g_label = ui::Font(11.5f, FW_SEMIBOLD);
    g_title = ui::Font(30, FW_BLACK);
    g_subtitle = ui::Font(13.5f, FW_SEMIBOLD);
    g_button = ui::Font(15, FW_SEMIBOLD);
    g_big = ui::Font(19, FW_BLACK);
    g_field_brush = CreateSolidBrush(ui::kField);
    g_bg_brush = CreateSolidBrush(ui::kBg);
    g_icon = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(1), IMAGE_ICON, S(48), S(48), 0));
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = g_bg_brush;
    wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1));
    wc.hIconSm = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(1), IMAGE_ICON, S(16), S(16), 0));
    wc.lpszClassName = L"LdPcTransfer";
    RegisterClassExW(&wc);
    g_main = CreateWindowExW(0, wc.lpszClassName, L"LD PC Transfer", style, place.left, place.top, place.right, place.bottom, nullptr, nullptr,
                             instance, nullptr);
    ui::DarkTitle(g_main);
    CreateControls();
    SetWindowTextW(g_src, ReadIni(L"origem").c_str());
    SetWindowTextW(g_dest, ReadIni(L"destino").c_str());
    g_target_index = std::clamp(_wtoi(ReadIni(L"alvo", L"0").c_str()), 0, static_cast<int>(std::size(kTargets)) - 1);
    g_slot_index = std::clamp(_wtoi(ReadIni(L"slot", L"0").c_str()), 0, kSlots - 1);
    AppendLog(ui::T(L"Escolha a pasta do save de origem e para onde ele vai. O original nunca é alterado.",
                    L"Choose the source save folder and where it goes. The original is never changed."));
    Detect();
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
