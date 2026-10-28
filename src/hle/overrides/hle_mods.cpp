// Fase 64: carregador de mods (ver hle_mods.h).
#include "hle/overrides/hle_mods.h"

#include <cstdio>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "hle/overrides/hle_anomaly.h"
#include "hle/overrides/hle_filesystem.h"

namespace hle::mods {

namespace {

namespace stdfs = std::filesystem;

std::vector<std::string> SplitTabs(const std::string& line) {
    std::vector<std::string> out;
    size_t start = 0;
    for (size_t tab; (tab = line.find('\t', start)) != std::string::npos; start = tab + 1) {
        out.push_back(line.substr(start, tab - start));
    }
    out.push_back(line.substr(start));
    return out;
}

stdfs::path Utf8Path(const std::string& text) { return stdfs::path(std::u8string(text.begin(), text.end())); }

} // namespace

int Setup(const stdfs::path& manifest) {
    std::ifstream in(manifest);
    if (!in) {
        return 0;
    }
    std::vector<stdfs::path> overlays;
    std::vector<fs::PackRedirect> redirects;
    std::vector<std::string> anomaly_names;
    std::map<std::string, stdfs::path> anomaly_files;
    std::string line;
    std::error_code ec;
    while (std::getline(in, line)) {
        while (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        const std::vector<std::string> f = SplitTabs(line);
        if (f.size() == 2 && f[0] == "overlay") {
            const stdfs::path dir = Utf8Path(f[1]);
            if (stdfs::is_directory(dir, ec)) {
                overlays.push_back(dir);
                std::printf("[MODS] loose files overlaying game: %s\n", f[1].c_str());
            }
        } else if (f.size() == 7 && f[0] == "pacote") {
            // pacote <TAB> nome <TAB> .HDR do jogo <TAB> .HDR mudado <TAB> .DAT do jogo <TAB> tamanho dele <TAB> cauda
            fs::PackRedirect r;
            r.pack = f[1];
            r.real_hdr = Utf8Path(f[2]);
            r.cache_hdr = Utf8Path(f[3]);
            r.real_dat = Utf8Path(f[4]);
            r.real_size = std::strtoull(f[5].c_str(), nullptr, 10);
            r.tail = Utf8Path(f[6]);
            // o cache vale para o .DAT daquele tamanho; se o jogo mudou (outra pasta, outra versao) o launcher refaz
            if (!stdfs::is_regular_file(r.cache_hdr, ec) || !stdfs::is_regular_file(r.tail, ec) ||
                stdfs::file_size(r.real_dat, ec) != r.real_size) {
                std::printf("[MODS] %s: stale cache, mods disabled (open via launcher to rebuild)\n", r.pack.c_str());
                continue;
            }
            std::printf("[MODS] %s: %llu bytes of mod files\n", r.pack.c_str(),
                        static_cast<unsigned long long>(stdfs::file_size(r.tail, ec)));
            redirects.push_back(std::move(r));
        } else if (f.size() == 4 && f[0] == "anomaly") {
            // Fase 67: anomaly <TAB> nome <TAB> .HDR do mod <TAB> .DAT do mod (o Anomaly abre /app0/mods/<nome>)
            const stdfs::path hdr = Utf8Path(f[2]);
            const stdfs::path dat = Utf8Path(f[3]);
            if (!stdfs::is_regular_file(hdr, ec) || !stdfs::is_regular_file(dat, ec)) {
                std::printf("[MODS] Anomaly: %s disappeared from mods folder, skipped\n", f[1].c_str());
                continue;
            }
            std::string key = f[1];
            for (char& c : key) {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
            anomaly_files["mods/" + key + ".hdr"] = hdr;
            anomaly_files["mods/" + key + ".dat"] = dat;
            anomaly_names.push_back(f[1]);
            std::printf("[MODS] Anomaly: pack %s (%s)\n", f[1].c_str(), f[3].c_str());
        }
    }
    const int active = static_cast<int>(overlays.size() + redirects.size() + anomaly_names.size());
    fs::SetModOverlays(std::move(overlays));
    fs::SetPackRedirects(std::move(redirects));
    fs::SetAnomalyFiles(std::move(anomaly_files));
    anomaly::SetMods(std::move(anomaly_names));
    return active;
}

} // namespace hle::mods
