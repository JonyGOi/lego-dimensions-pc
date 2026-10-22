// Fase 64: carregador de mods (ver hle_mods.h).
#include "hle/overrides/hle_mods.h"

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

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
                std::printf("[MODS] arquivos soltos por cima do jogo: %s\n", f[1].c_str());
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
                std::printf("[MODS] %s: cache desatualizado, fica sem mods (abra pelo launcher para refazer)\n", r.pack.c_str());
                continue;
            }
            std::printf("[MODS] %s: %llu bytes de arquivos de mod\n", r.pack.c_str(),
                        static_cast<unsigned long long>(stdfs::file_size(r.tail, ec)));
            redirects.push_back(std::move(r));
        }
    }
    const int active = static_cast<int>(overlays.size() + redirects.size());
    fs::SetModOverlays(std::move(overlays));
    fs::SetPackRedirects(std::move(redirects));
    return active;
}

} // namespace hle::mods
