// Fase 49: instalador do LEGO Dimensions PC. O .zip do port vai colado no fim do proprio .exe
// (rodape: "LDPCPAY1" + tamanho em 8 bytes). Extrai com o tar.exe do Windows, cria os atalhos e
// registra a desinstalacao em HKCU (sem precisar de administrador). Com /uninstall vira o desinstalador.
// Nao traz nada do jogo: o launcher pede a pasta do dump e das DLCs de quem instalou.
#define UNICODE
#define _UNICODE
#include "ui.h"

#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' "      \
                        "processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace fs = std::filesystem;
using ui::S;

namespace {

constexpr char kMagic[8] = {'L', 'D', 'P', 'C', 'P', 'A', 'Y', '1'};
constexpr wchar_t kAppName[] = L"LEGO Dimensions PC";
constexpr wchar_t kUninstallKey[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LegoDimensionsPC";
constexpr UINT WM_APP_DONE = WM_APP + 1, WM_APP_STEP = WM_APP + 2;
enum Id { kDir = 100, kBrowse, kDesktop, kInstall, kCancel };
constexpr int kW = 760, kH = 520, kHeader = 200;

HWND g_main, g_dir, g_desktop, g_install, g_browse, g_cancel;
HFONT g_font, g_small, g_label, g_title, g_subtitle, g_button, g_big_button;
HBRUSH g_field_brush, g_bg_brush;
HICON g_icon;
std::wstring g_self;
bool g_busy = false, g_done = false, g_focus = false;
int g_pct = 0;
std::wstring g_step = ui::T(L"Pronto para instalar.", L"Ready to install."), g_launcher;
// (os textos com acento usam \u: o .cpp e compilado sem depender da pagina de codigo)

std::wstring GetText(HWND h) {
    std::wstring s(GetWindowTextLengthW(h) + 1, L'\0');
    GetWindowTextW(h, s.data(), static_cast<int>(s.size()));
    s.resize(wcslen(s.c_str()));
    while (!s.empty() && (s.back() == L'\\' || s.back() == L' ')) {
        s.pop_back();
    }
    return s;
}

// Onde o .zip esta dentro deste .exe (offset, tamanho); {0,0} se nao houver (desinstalador).
std::pair<uint64_t, uint64_t> FindPayload() {
    std::ifstream f(fs::path(g_self), std::ios::binary | std::ios::ate);
    const uint64_t size = static_cast<uint64_t>(f.tellg());
    if (size < 16) {
        return {0, 0};
    }
    char footer[16];
    f.seekg(static_cast<std::streamoff>(size - 16));
    f.read(footer, 16);
    if (memcmp(footer, kMagic, 8) != 0) {
        return {0, 0};
    }
    uint64_t payload = 0;
    memcpy(&payload, footer + 8, 8);
    if (payload + 16 > size) {
        return {0, 0};
    }
    return {size - 16 - payload, payload};
}

void Step(const std::wstring& text, int pct) {
    PostMessageW(g_main, WM_APP_STEP, pct, reinterpret_cast<LPARAM>(new std::wstring(text)));
}

bool Shortcut(const fs::path& link, const fs::path& target, const fs::path& dir) {
    IShellLinkW* shell_link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shell_link)))) {
        return false;
    }
    shell_link->SetPath(target.c_str());
    shell_link->SetWorkingDirectory(dir.c_str());
    shell_link->SetDescription(kAppName);
    IPersistFile* file = nullptr;
    bool ok = false;
    if (SUCCEEDED(shell_link->QueryInterface(IID_PPV_ARGS(&file)))) {
        ok = SUCCEEDED(file->Save(link.c_str(), TRUE));
        file->Release();
    }
    shell_link->Release();
    return ok;
}

fs::path KnownFolder(REFKNOWNFOLDERID id) {
    PWSTR path = nullptr;
    fs::path out;
    if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &path))) {
        out = path;
        CoTaskMemFree(path);
    }
    return out;
}

void SetRegString(HKEY key, const wchar_t* name, const std::wstring& value) {
    RegSetValueExW(key, name, 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
}

// Thread de instalacao: copia o .zip para %TEMP%, extrai com tar, desinstalador, atalhos, registro.
bool g_test_mode = false; // /teste: so extrai (sem atalhos nem registro), para conferir o instalador

void Install(fs::path dest, bool desktop) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    auto fail = [](const std::wstring& why) {
        if (g_test_mode) { // /teste: o motivo vai para %TEMP%\ldpc_setup_teste.txt
            wchar_t temp_dir[MAX_PATH];
            GetTempPathW(MAX_PATH, temp_dir);
            std::wofstream(fs::path(temp_dir) / L"ldpc_setup_teste.txt") << why;
        }
        PostMessageW(g_main, WM_APP_DONE, 1, reinterpret_cast<LPARAM>(new std::wstring(why)));
    };
    const auto [offset, size] = FindPayload();
    if (!size) {
        fail(ui::T(L"Este instalador esta incompleto (sem os arquivos do port).", L"This installer is incomplete (the port files are missing)."));
        return;
    }
    std::error_code ec;
    fs::create_directories(dest, ec);
    if (ec) {
        fail(std::wstring(ui::T(L"Nao consegui criar a pasta de destino: ", L"Could not create the destination folder: ")) + dest.wstring());
        return;
    }
    Step(ui::T(L"Copiando os arquivos...", L"Copying files..."), 5);
    wchar_t temp[MAX_PATH];
    GetTempPathW(MAX_PATH, temp);
    const fs::path zip = fs::path(temp) / L"lego_dimensions_pc_payload.zip";
    {
        std::ifstream in(fs::path(g_self), std::ios::binary);
        std::ofstream out(zip, std::ios::binary | std::ios::trunc);
        in.seekg(static_cast<std::streamoff>(offset));
        std::vector<char> buffer(4 << 20);
        uint64_t left = size;
        while (left && in) {
            const size_t chunk = static_cast<size_t>(std::min<uint64_t>(left, buffer.size()));
            in.read(buffer.data(), static_cast<std::streamsize>(chunk));
            out.write(buffer.data(), static_cast<std::streamsize>(chunk));
            left -= chunk;
            Step(ui::T(L"Copiando os arquivos...", L"Copying files..."), 5 + static_cast<int>(35 * (size - left) / size));
        }
        if (left) {
            fail(ui::T(L"Erro ao ler o instalador.", L"Error reading the installer."));
            return;
        }
    }
    Step(ui::T(L"Extraindo (pode levar um minuto)...", L"Extracting (this can take a minute)..."), 45);
    wchar_t system[MAX_PATH];
    GetSystemDirectoryW(system, MAX_PATH);
    std::wstring command = L"\"" + (fs::path(system) / L"tar.exe").wstring() + L"\" -xf \"" + zip.wstring() + L"\" -C \"" + dest.wstring() + L"\"";
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        fail(ui::T(L"Nao achei o tar.exe do Windows (Windows 10 1803 ou mais novo).", L"Windows tar.exe was not found (Windows 10 1803 or newer is required)."));
        return;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    DeleteFileW(zip.c_str());
    if (code != 0 || !fs::is_regular_file(dest / L"LegoDimensionsLauncher.exe", ec)) {
        fail(std::wstring(ui::T(L"A extracao falhou (codigo ", L"Extraction failed (code ")) + std::to_wstring(code) + ui::T(L"). Ha espaco no disco?", L"). Is there enough disk space?"));
        return;
    }
    Step(ui::T(L"Criando o desinstalador e os atalhos...", L"Creating the uninstaller and shortcuts..."), 85);
    { // desinstalador = este .exe sem o .zip
        std::ifstream in(fs::path(g_self), std::ios::binary);
        std::ofstream out(dest / L"Desinstalar.exe", std::ios::binary | std::ios::trunc);
        std::vector<char> head(static_cast<size_t>(offset));
        in.read(head.data(), static_cast<std::streamsize>(head.size()));
        out.write(head.data(), static_cast<std::streamsize>(head.size()));
    }
    const fs::path launcher = dest / L"LegoDimensionsLauncher.exe";
    if (g_test_mode) {
        PostMessageW(g_main, WM_APP_DONE, 0, reinterpret_cast<LPARAM>(new std::wstring(launcher.wstring())));
        return;
    }
    const fs::path menu = KnownFolder(FOLDERID_Programs) / kAppName;
    fs::create_directories(menu, ec);
    Shortcut(menu / L"LEGO Dimensions PC.lnk", launcher, dest);
    Shortcut(menu / L"LD PC Transfer.lnk", dest / L"LD_PC_Transfer.exe", dest); // Fase 63
    Shortcut(menu / ui::T(L"Desinstalar LEGO Dimensions PC.lnk", L"Uninstall LEGO Dimensions PC.lnk"), dest / L"Desinstalar.exe", dest);
    if (desktop) {
        Shortcut(KnownFolder(FOLDERID_Desktop) / L"LEGO Dimensions PC.lnk", launcher, dest);
    }
    HKEY key;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kUninstallKey, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr) == ERROR_SUCCESS) {
        SetRegString(key, L"DisplayName", kAppName);
        std::string version; // Fase 61: version.txt do pacote
        std::ifstream version_file(dest / L"version.txt");
        std::getline(version_file, version);
        while (!version.empty() && (version.back() == 13 || version.back() == 32)) {
            version.pop_back();
        }
        SetRegString(key, L"DisplayVersion", std::wstring(version.begin(), version.end()));
        SetRegString(key, L"Publisher", ui::T(L"Port nao oficial (feito em casa)", L"Unofficial fan port"));
        SetRegString(key, L"InstallLocation", dest.wstring());
        SetRegString(key, L"DisplayIcon", launcher.wstring());
        SetRegString(key, L"UninstallString", L"\"" + (dest / L"Desinstalar.exe").wstring() + L"\" /uninstall");
        DWORD one = 1;
        RegSetValueExW(key, L"NoModify", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&one), sizeof(one));
        RegSetValueExW(key, L"NoRepair", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&one), sizeof(one));
        RegCloseKey(key);
    }
    PostMessageW(g_main, WM_APP_DONE, 0, reinterpret_cast<LPARAM>(new std::wstring(launcher.wstring())));
    CoUninitialize();
}

// Desinstalar: atalhos, registro e a pasta. Os saves (assets\savedata0) ficam, a menos que o usuario peca.
int Uninstall() {
    const fs::path dir = fs::path(g_self).parent_path();
    if (!g_test_mode &&
        MessageBoxW(nullptr, (std::wstring(ui::T(L"Desinstalar o LEGO Dimensions PC de\n", L"Uninstall LEGO Dimensions PC from\n")) + dir.wstring() + L" ?").c_str(), kAppName, MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return 0;
    }
    const bool keep_saves =
        g_test_mode || MessageBoxW(nullptr, ui::T(L"Manter os seus saves (assets\\savedata0) na pasta?", L"Keep your saves (assets\\savedata0) in the folder?"), kAppName, MB_YESNO | MB_ICONQUESTION) == IDYES;
    std::error_code ec;
    fs::remove_all(KnownFolder(FOLDERID_Programs) / kAppName, ec);
    fs::remove(KnownFolder(FOLDERID_Desktop) / L"LEGO Dimensions PC.lnk", ec);
    RegDeleteTreeW(HKEY_CURRENT_USER, kUninstallKey);
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
        const fs::path p = it->path();
        if (p.filename() == L"Desinstalar.exe" || (keep_saves && (p.filename() == L"saves_backup" || p.filename() == L"mods" || p.filename() == L"jogadores.txt"))) { // Fase 63/64/65: backups do LD PC Transfer, mods, apelidos
            continue;
        }
        if (keep_saves && p.filename() == L"assets") {
            for (fs::directory_iterator a(p, ec), aend; !ec && a != aend; a.increment(ec)) {
                if (a->path().filename() != L"savedata0") {
                    std::error_code e2;
                    fs::remove_all(a->path(), e2);
                }
            }
            continue;
        }
        std::error_code e2;
        fs::remove_all(p, e2);
    }
    // o proprio desinstalador (e a pasta, se ficou vazia) saem depois que este processo fechar
    std::wstring command = L"cmd.exe /c ping 127.0.0.1 -n 3 >nul & del /f /q \"" + g_self + L"\" & rmdir \"" + dir.wstring() + L"\"";
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    if (CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    if (!g_test_mode) {
        MessageBoxW(nullptr, keep_saves ? ui::T(L"Desinstalado. Os saves ficaram em assets\\savedata0.", L"Uninstalled. Your saves were kept in assets\\savedata0.") : ui::T(L"Desinstalado.", L"Uninstalled."), kAppName, MB_ICONINFORMATION);
    }
    return 0;
}

std::wstring BrowseFolder(const std::wstring& start) {
    std::wstring result;
    IFileOpenDialog* dialog = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog)))) {
        DWORD options = 0;
        dialog->GetOptions(&options);
        dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
        dialog->SetTitle(ui::T(L"Onde instalar o LEGO Dimensions PC", L"Where to install LEGO Dimensions PC"));
        (void)start;
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

// Espaco livre no disco de destino (o instalado ocupa ~100 MB; com os shaders preparados ~1,7 GB).
std::wstring FreeSpace(const fs::path& dest) {
    fs::path probe = dest;
    std::error_code ec;
    while (!probe.empty() && !fs::exists(probe, ec)) {
        probe = probe.parent_path();
    }
    ULARGE_INTEGER free_bytes{};
    if (probe.empty() || !GetDiskFreeSpaceExW(probe.c_str(), &free_bytes, nullptr, nullptr)) {
        return L"";
    }
    wchar_t text[64];
    swprintf(text, 64, ui::T(L"%.1f GB livres", L"%.1f GB free"), free_bytes.QuadPart / 1073741824.0);
    return text;
}

void Paint(HDC target) {
    RECT client;
    GetClientRect(g_main, &client);
    ui::Buffered buffer(target, client);
    HDC dc = buffer.dc;
    HBRUSH bg = CreateSolidBrush(ui::kBg);
    FillRect(dc, &client, bg);
    DeleteObject(bg);
    const int w = S(kW), header = S(kHeader);
    {
        Gdiplus::Graphics g(dc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        // faixa do topo: degrade azul -> roxo com os aneis do portal
        Gdiplus::LinearGradientBrush top(Gdiplus::Point(0, 0), Gdiplus::Point(w, header), ui::C(RGB(14, 26, 70)), ui::C(RGB(78, 24, 116)));
        g.FillRectangle(&top, 0, 0, w, header);
        for (int k = 0; k < 7; ++k) {
            Gdiplus::Pen pen(ui::C(k % 2 ? ui::kBlue : RGB(120, 220, 255), static_cast<BYTE>(80 - k * 10)), static_cast<float>(S(9 - k)));
            const int r = S(60.0f + k * 34);
            g.DrawEllipse(&pen, w - S(150) - r, S(100) - r, 2 * r, 2 * r);
        }
        Gdiplus::LinearGradientBrush fade(Gdiplus::Point(0, header - S(60)), Gdiplus::Point(0, header + 1), ui::C(ui::kBg, 0), ui::C(ui::kBg, 255));
        g.FillRectangle(&fade, 0, header - S(60), w, S(61));
        // cartao do corpo
        RECT card{S(24), header - S(24), S(kW - 24), S(kH - 96)};
        Gdiplus::SolidBrush panel(ui::C(ui::kPanel, 245));
        ui::FillRound(g, card, static_cast<float>(S(16)), panel);
        ui::StrokeRound(g, card, static_cast<float>(S(16)), ui::kBorder, 1.0f, 160);
        ui::FieldBox(g, g_main, g_dir, g_focus);
        RECT bar{S(48), S(kH - 122), S(kW - 48), S(kH - 114)};
        ui::Progress(g, bar, g_pct);
    }
    DrawIconEx(dc, S(36), S(40), g_icon, S(96), S(96), 0, nullptr, DI_NORMAL);
    RECT title{S(152), S(44), w, S(100)}, sub{S(154), S(98), w, S(126)};
    ui::Text(dc, L"LEGO\u00ae DIMENSIONS PC", title, g_title, RGB(255, 255, 255));
    ui::Text(dc, ui::T(L"Instalador do port   \u2022   o jogo e as DLCs v\u00eam do seu pr\u00f3prio dump", L"Port installer   \u2022   the game and DLCs come from your own dump"), sub, g_subtitle, ui::kMuted);
    RECT label{S(48), header - S(8), S(kW - 48), header + S(14)};
    ui::Text(dc, ui::T(L"PASTA DE INSTALA\u00c7\u00c3O", L"INSTALL FOLDER"), label, g_label, ui::kMuted);
    RECT space{S(kW - 260), header - S(8), S(kW - 48), header + S(14)};
    ui::Text(dc, FreeSpace(GetText(g_dir)), space, g_label, ui::kMuted, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    RECT info{S(48), header + S(118), S(kW - 48), header + S(164)};
    ui::Text(dc,
             ui::T(L"Depois de instalar: abra o launcher, escolha a pasta do jogo (dump do PS4, CUSA01176) e a das DLCs, clique em Preparar "
                   L"(s\u00f3 na primeira vez: traduz os shaders do seu jogo para Vulkan) e depois em JOGAR.",
                   L"After installing: open the launcher, choose the game folder (PS4 dump, CUSA01176) and the DLC folder, click Prepare "
                   L"(first time only: translates your game's shaders to Vulkan) and then PLAY."),
             info, g_small, ui::kMuted, DT_LEFT | DT_WORDBREAK);
    RECT step{S(48), S(kH - 154), S(kW - 120), S(kH - 130)}, pct{S(kW - 120), S(kH - 154), S(kW - 48), S(kH - 130)};
    ui::Text(dc, g_step, step, g_small, g_done ? ui::kGreen : ui::kText);
    ui::Text(dc, g_pct ? std::to_wstring(g_pct) + L"%" : L"", pct, g_small, ui::kMuted, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
}

void Invalidate() { InvalidateRect(g_main, nullptr, FALSE); }

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
        SetTextColor(dc, ui::kText);
        return reinterpret_cast<LRESULT>(g_field_brush);
    }
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case kDir:
            if (HIWORD(wp) == EN_SETFOCUS || HIWORD(wp) == EN_KILLFOCUS) {
                g_focus = HIWORD(wp) == EN_SETFOCUS;
                Invalidate();
            } else if (HIWORD(wp) == EN_CHANGE) {
                Invalidate();
            }
            break;
        case kBrowse: {
            const std::wstring folder = BrowseFolder(GetText(g_dir));
            if (!folder.empty()) {
                SetWindowTextW(g_dir, (fs::path(folder) / kAppName).c_str());
            }
            break;
        }
        case kDesktop:
            ui::Toggle(g_desktop);
            break;
        case kCancel:
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            break;
        case kInstall: {
            if (g_done) { // depois de instalar o botao abre o launcher
                ShellExecuteW(nullptr, L"open", g_launcher.c_str(), nullptr, fs::path(g_launcher).parent_path().c_str(), SW_SHOWNORMAL);
                DestroyWindow(hwnd);
                break;
            }
            if (g_busy) {
                break;
            }
            const fs::path dest = GetText(g_dir);
            if (dest.empty()) {
                break;
            }
            g_busy = true;
            for (HWND h : {g_install, g_browse, g_dir, g_desktop, g_cancel}) {
                EnableWindow(h, FALSE);
            }
            std::thread(Install, dest, ui::Checked(g_desktop)).detach();
            break;
        }
        }
        return 0;
    case WM_APP_STEP: {
        std::wstring* text = reinterpret_cast<std::wstring*>(lp);
        g_step = *text;
        g_pct = static_cast<int>(wp);
        delete text;
        Invalidate();
        return 0;
    }
    case WM_APP_DONE: {
        std::wstring* text = reinterpret_cast<std::wstring*>(lp);
        g_busy = false;
        if (wp == 0) {
            g_done = true;
            g_pct = 100;
            g_launcher = *text;
            g_step = ui::T(L"Instalado! Abra o launcher, escolha a pasta do jogo e clique em Preparar.", L"Installed! Open the launcher, choose the game folder and click Prepare.");
            SetWindowTextW(g_install, ui::T(L"Abrir o launcher", L"Open the launcher"));
            SetWindowTextW(g_cancel, ui::T(L"Fechar", L"Close"));
            EnableWindow(g_install, TRUE);
            EnableWindow(g_cancel, TRUE);
        } else {
            g_step = std::wstring(ui::T(L"Falhou: ", L"Failed: ")) + *text;
            for (HWND h : {g_install, g_browse, g_dir, g_desktop, g_cancel}) {
                EnableWindow(h, TRUE);
            }
            MessageBoxW(hwnd, text->c_str(), kAppName, MB_ICONERROR);
        }
        delete text;
        Invalidate();
        return 0;
    }
    case WM_CLOSE:
        if (g_busy) {
            return 0;
        }
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR command_line, int show) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    wchar_t self[MAX_PATH * 4];
    GetModuleFileNameW(nullptr, self, MAX_PATH * 4);
    g_self = self;
    if (command_line && wcsstr(command_line, L"/uninstall")) {
        g_test_mode = wcsstr(command_line, L"/teste") != nullptr; // /teste: sem perguntas, mantem os saves
        return Uninstall();
    }
    if (const wchar_t* dir = command_line ? wcsstr(command_line, L"/dir=") : nullptr; dir && wcsstr(command_line, L"/teste")) {
        g_test_mode = true;
        std::wstring dest = dir + 5;
        dest.erase(std::remove(dest.begin(), dest.end(), L'"'), dest.end());
        while (!dest.empty() && (dest.back() == L' ' || dest.back() == L'\\' || dest.back() == L'\t')) {
            dest.pop_back();
        }
        Install(dest, false);
        return fs::is_regular_file(fs::path(dest) / L"LegoDimensionsLauncher.exe") ? 0 : 1;
    }
    if (!FindPayload().second) {
        MessageBoxW(nullptr, ui::T(L"Este e o desinstalador. Para desinstalar use \"Desinstalar LEGO Dimensions PC\" no menu Iniciar.", L"This is the uninstaller. To uninstall, use \"Uninstall LEGO Dimensions PC\" in the Start menu."), kAppName, MB_ICONINFORMATION);
        return 0;
    }
    ui::InitProcess();
    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
    const RECT place = ui::FitWindow(kW, kH, style); // Fase 61: cabe em telas pequenas
    g_font = ui::Font(15);
    g_small = ui::Font(13);
    g_label = ui::Font(11.5f, FW_SEMIBOLD);
    g_title = ui::Font(38, FW_BLACK);
    g_subtitle = ui::Font(13.5f, FW_SEMIBOLD);
    g_button = ui::Font(15, FW_SEMIBOLD);
    g_big_button = ui::Font(17, FW_BLACK);
    g_field_brush = CreateSolidBrush(ui::kField);
    g_bg_brush = CreateSolidBrush(ui::kBg);
    g_icon = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(1), IMAGE_ICON, S(96), S(96), 0));
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = g_bg_brush;
    wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1));
    wc.hIconSm = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(1), IMAGE_ICON, S(16), S(16), 0));
    wc.lpszClassName = L"LegoDimensionsPcSetup";
    RegisterClassExW(&wc);
    g_main = CreateWindowExW(0, wc.lpszClassName, ui::T(L"Instalar LEGO Dimensions PC", L"Install LEGO Dimensions PC"), style, place.left, place.top, place.right,
                             place.bottom, nullptr, nullptr, instance, nullptr);
    ui::DarkTitle(g_main);
    const int header = S(kHeader);
    g_dir = ui::Edit(g_main, S(58), header + S(26), S(kW - 300), S(24), kDir, g_font);
    SetWindowTextW(g_dir, L"C:\\Games\\LEGO Dimensions PC");
    g_browse = ui::Button(g_main, ui::T(L"Procurar...", L"Browse..."), ui::Style::kSecondary, S(kW - 216), header + S(16), S(168), S(44), kBrowse, g_button, ui::kPanel);
    g_desktop = ui::Button(g_main, ui::T(L"Criar atalho na \u00e1rea de trabalho", L"Create a desktop shortcut"), ui::Style::kToggle, S(48), header + S(72), S(360), S(40), kDesktop, g_font,
                           ui::kPanel);
    ui::SetChecked(g_desktop, true);
    g_cancel = ui::Button(g_main, ui::T(L"Cancelar", L"Cancel"), ui::Style::kSecondary, S(kW - 400), S(kH - 76), S(150), S(52), kCancel, g_button);
    g_install = ui::Button(g_main, ui::T(L"INSTALAR", L"INSTALL"), ui::Style::kPrimary, S(kW - 236), S(kH - 76), S(212), S(52), kInstall, g_big_button);
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
