// SPDX-License-Identifier: GPL-2.0-or-later
// LEGO Dimensions PC: extrator de PKG do PS4 para o launcher. A extracao (pkg.cpp, pfs.h, pkg_type, keys.h, crypto) e a
// do shadPS4 0.7.0; aqui so a linha de comando, o PARAM.SFO e a pasta de cada pacote:
//   jogo        -> <destino>\<TITLE_ID>
//   atualizacao -> <destino>\<TITLE_ID>-patch   (a pasta separada que o port e o launcher ja aceitam)
//   DLC         -> <dlcs>\<TITLE_ID>\<entitlement> (sem --dlcs: <destino>\addcont\<TITLE_ID>\<entitlement>)
// uso: pkg_extractor.exe --destino PASTA [--dlcs PASTA] [--lingua pt|en] arquivo.pkg [arquivo.pkg ...]
// Saida para o launcher (UTF-8): PROGRESSO n texto / EXTRAIDO ... / AVISO ... / ERRO ... e no fim JOGO, ATUALIZACAO e
// DLCS com as pastas para os campos do launcher. Codigo de saida 0 so se todos os pacotes foram extraidos.

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <windows.h>

#include "pkg.h"

namespace fs = std::filesystem;

namespace {

bool g_pt = false;

const char* T(const char* pt, const char* en) {
    return g_pt ? pt : en;
}

std::string Utf8(const std::wstring& text) {
    const int n = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    std::string out(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), out.data(), n, nullptr, nullptr);
    return out;
}

void Line(const std::string& text) {
    std::fputs((text + "\n").c_str(), stdout);
    std::fflush(stdout);
}

// PARAM.SFO (PSF): cabecalho, tabela de entradas, chaves e valores; so as strings
std::map<std::string, std::string> ReadSfo(const std::vector<u8>& sfo) {
    std::map<std::string, std::string> values;
    auto u16_at = [&](size_t p) { return static_cast<u32>(sfo[p] | (sfo[p + 1] << 8)); };
    auto u32_at = [&](size_t p) { return static_cast<u32>(sfo[p] | (sfo[p + 1] << 8) | (sfo[p + 2] << 16) | (sfo[p + 3] << 24)); };
    if (sfo.size() < 20 || std::memcmp(sfo.data(), "\0PSF", 4) != 0) {
        return values;
    }
    const u32 keys = u32_at(8), data = u32_at(12), count = u32_at(16);
    for (u32 i = 0; i < count && 20 + i * 16 + 16 <= sfo.size(); i++) {
        const size_t entry = 20 + i * 16;
        const u32 key_off = keys + u16_at(entry), fmt = u16_at(entry + 2), len = u32_at(entry + 4), data_off = data + u32_at(entry + 12);
        if (key_off >= sfo.size() || data_off + len > sfo.size() || fmt == 0x0404) {
            continue;
        }
        const char* key = reinterpret_cast<const char*>(sfo.data() + key_off);
        std::string value(reinterpret_cast<const char*>(sfo.data() + data_off), len);
        value.erase(std::find(value.begin(), value.end(), '\0'), value.end());
        values[std::string(key, strnlen(key, sfo.size() - key_off))] = value;
    }
    return values;
}

enum class Kind { kGame, kPatch, kDlc };

struct Package {
    fs::path file;
    Kind kind = Kind::kGame;
    std::string title, entitlement, version, name;
    u64 size = 0;
};

// Um PKG corrompido pode derrubar a extracao: o launcher recebe um ERRO em vez de ficar sem resposta
LONG WINAPI CrashFilter(EXCEPTION_POINTERS* e) {
    char text[160];
    std::snprintf(text, sizeof(text), T("ERRO o extrator de PKG parou (excecao 0x%08lx): o arquivo pode estar corrompido.",
                                        "ERRO the PKG extractor stopped (exception 0x%08lx): the file may be corrupted."),
                  e->ExceptionRecord->ExceptionCode);
    Line(text);
    return EXCEPTION_EXECUTE_HANDLER;
}

} // namespace

int wmain(int argc, wchar_t** argv) {
    SetUnhandledExceptionFilter(CrashFilter);
    fs::path destination, dlcs;
    std::vector<fs::path> files;
    for (int i = 1; i < argc; i++) {
        const std::wstring arg = argv[i];
        if (arg == L"--destino" && i + 1 < argc) {
            destination = argv[++i];
        } else if (arg == L"--dlcs" && i + 1 < argc) {
            dlcs = argv[++i];
        } else if (arg == L"--lingua" && i + 1 < argc) {
            g_pt = std::wstring(argv[++i]) == L"pt";
        } else {
            files.push_back(arg);
        }
    }
    if (destination.empty() || files.empty()) {
        Line(T("ERRO uso: pkg_extractor.exe --destino PASTA [--dlcs PASTA] [--lingua pt|en] arquivo.pkg ...",
               "ERRO usage: pkg_extractor.exe --destino FOLDER [--dlcs FOLDER] [--lingua pt|en] file.pkg ..."));
        return 2;
    }

    // Le os cabecalhos primeiro: jogo antes da atualizacao, DLCs por ultimo
    int failures = 0;
    std::vector<Package> packages;
    for (const fs::path& file : files) {
        std::error_code exists_ec;
        if (!fs::is_regular_file(file, exists_ec)) {
            Line(std::string("ERRO ") + T("arquivo nao encontrado: ", "file not found: ") + Utf8(file.wstring()));
            failures++;
            continue;
        }
        PKG pkg;
        std::string failreason;
        if (!pkg.Open(file, failreason)) {
            Line(std::string("ERRO ") + Utf8(file.filename().wstring()) + ": " +
                 (failreason.empty() ? T("nao e um PKG de PS4.", "not a PS4 PKG.") : failreason));
            failures++;
            continue;
        }
        auto sfo = ReadSfo(pkg.sfo);
        Package p;
        p.file = file;
        p.size = pkg.GetPkgSize();
        p.title = std::string(pkg.GetTitleID());
        if (sfo.count("TITLE_ID") && sfo["TITLE_ID"].size() == 9) {
            p.title = sfo["TITLE_ID"];
        }
        p.version = sfo.count("APP_VER") ? sfo["APP_VER"] : sfo.count("VERSION") ? sfo["VERSION"] : "";
        p.name = sfo.count("TITLE") ? sfo["TITLE"] : Utf8(file.filename().wstring());
        const std::string category = sfo.count("CATEGORY") ? sfo["CATEGORY"] : "";
        if (category == "ac") {
            p.kind = Kind::kDlc;
            const std::string content = sfo.count("CONTENT_ID") ? sfo["CONTENT_ID"] : "";
            const size_t a = content.find('-'), b = a == std::string::npos ? a : content.find('-', a + 1);
            if (b == std::string::npos || content.size() <= b + 1) {
                Line(std::string("ERRO ") + Utf8(file.filename().wstring()) + T(": DLC sem CONTENT_ID no PARAM.SFO.", ": DLC without a CONTENT_ID in PARAM.SFO."));
                failures++;
                continue;
            }
            p.entitlement = content.substr(b + 1);
        } else if (category.rfind("gp", 0) == 0 || pkg.GetPkgFlags().find("PATCH") != std::string::npos) {
            p.kind = Kind::kPatch;
        }
        packages.push_back(p);
    }
    std::stable_sort(packages.begin(), packages.end(), [](const Package& a, const Package& b) { return a.kind < b.kind; });

    u64 total = 0;
    for (const Package& p : packages) {
        total += p.size;
    }
    u64 done_bytes = 0;
    std::optional<fs::path> game_dir, patch_dir, dlc_dir;
    for (size_t n = 0; n < packages.size(); n++) {
        const Package& p = packages[n];
        fs::path target;
        const char* what;
        if (p.kind == Kind::kGame) {
            target = destination / p.title;
            what = T("jogo", "game");
        } else if (p.kind == Kind::kPatch) {
            target = destination / (p.title + "-patch");
            what = T("atualizacao", "update");
        } else {
            // o pkg.cpp poe os arquivos em <pai>\<TITLE_ID> quando o pai nao se chama TITLE_ID: a pasta das DLCs tem esse nome
            fs::path base = dlcs.empty() ? destination / "addcont" : dlcs;
            if (base.filename() != fs::path(p.title)) {
                base /= p.title;
            }
            target = base / p.entitlement;
            what = "DLC";
        }
        const std::string label = std::string(what) + " " + p.name + (p.version.empty() ? "" : " " + p.version);

        std::error_code ec;
        fs::create_directories(target, ec);
        ULARGE_INTEGER free_bytes{};
        if (GetDiskFreeSpaceExW(target.c_str(), &free_bytes, nullptr, nullptr) && free_bytes.QuadPart < p.size) {
            Line(std::string("ERRO ") + T("espaco insuficiente para ", "not enough disk space for ") + label + " (" +
                 std::to_string(p.size / (1024 * 1024)) + T(" MB necessarios).", " MB needed)."));
            failures++;
            done_bytes += p.size;
            continue;
        }

        auto percent = [&](double fraction) {
            return total ? std::clamp(static_cast<int>((done_bytes + fraction * p.size) * 100.0 / total), 1, 99) : 1;
        };
        Line("PROGRESSO " + std::to_string(percent(0)) + " " + T("Lendo ", "Reading ") + label + "...");
        PKG pkg;
        std::string failreason;
        bool ok = false;
        try {
            ok = pkg.Open(p.file, failreason) && pkg.Extract(p.file, target, failreason);
        } catch (const std::exception& e) {
            failreason = e.what();
        }
        if (!ok) {
            Line(std::string("ERRO ") + label + ": " + (failreason.empty() ? T("nao consegui ler o PKG.", "could not read the PKG.") : failreason));
            failures++;
            done_bytes += p.size;
            continue;
        }

        // Os arquivos do PFS em paralelo, como o shadPS4 0.7.0 fazia (QtConcurrent::map sobre ExtractFiles)
        const int count = static_cast<int>(pkg.GetNumberOfFiles());
        std::atomic<int> next{0}, finished{0};
        std::atomic<bool> failed{false};
        std::string error;
        std::vector<std::thread> workers;
        const int threads = std::clamp(static_cast<int>(std::thread::hardware_concurrency()), 1, 8);
        for (int t = 0; t < threads; t++) {
            workers.emplace_back([&] {
                for (int i; !failed && (i = next++) < count;) {
                    try {
                        pkg.ExtractFiles(i);
                    } catch (const std::exception& e) {
                        if (!failed.exchange(true)) {
                            error = e.what();
                        }
                    }
                    finished++;
                }
            });
        }
        int shown = -1;
        while (finished < count && !failed) {
            const int pct = percent(count ? static_cast<double>(finished) / count : 1.0);
            if (pct != shown) {
                Line("PROGRESSO " + std::to_string(pct) + " " + T("Extraindo ", "Extracting ") + label + " (" +
                     std::to_string(finished.load()) + "/" + std::to_string(count) + ")");
                shown = pct;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        for (std::thread& worker : workers) {
            worker.join();
        }
        done_bytes += p.size;
        if (failed) {
            Line(std::string("ERRO ") + label + ": " + error);
            failures++;
            continue;
        }
        Line(std::string("EXTRAIDO ") + label + " -> " + Utf8(target.wstring()));
        if (p.kind == Kind::kGame) {
            game_dir = target;
        } else if (p.kind == Kind::kPatch) {
            patch_dir = target;
        } else {
            dlc_dir = target.parent_path();
        }
    }

    if (game_dir) {
        Line("JOGO " + Utf8(game_dir->wstring()));
    }
    if (patch_dir) {
        Line("ATUALIZACAO " + Utf8(patch_dir->wstring()));
    }
    if (dlc_dir) {
        Line("DLCS " + Utf8(dlc_dir->wstring()));
    }
    Line(std::string("PROGRESSO 100 ") + (failures ? T("Extracao terminada com erros.", "Extraction finished with errors.")
                                                    : T("Extracao terminada.", "Extraction finished.")));
    return failures ? 1 : 0;
}
