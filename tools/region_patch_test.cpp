#include "common/cutscene_patch.h"
#include "common/game_region.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <stdexcept>

void check(bool value) { if (!value) throw std::runtime_error("region/patch check failed"); }
void sfo(const std::filesystem::path& root, const std::string& tid) {
    std::filesystem::create_directories(root / "sce_sys");
    std::array<unsigned char, 55> bytes{};
    const auto put = [&](size_t at, uint32_t value) { std::memcpy(bytes.data()+at, &value, 4); };
    std::memcpy(bytes.data(), "\0PSF", 4); put(4,0x101); put(8,36); put(12,45); put(16,1);
    bytes[22]=4; bytes[23]=2; put(24,10); put(28,10);
    std::memcpy(bytes.data()+36,"TITLE_ID",9); std::memcpy(bytes.data()+45,tid.c_str(),10);
    std::ofstream(root / "sce_sys" / "param.sfo", std::ios::binary).write(reinterpret_cast<const char*>(bytes.data()),bytes.size());
}
int main() {
    const auto root = std::filesystem::temp_directory_path() / ("ld-region-unit-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    try {
        for (const std::string tid : {"CUSA01176","CUSA00935"}) {
            const auto dir = root / tid, game = dir / "renamed-game", patch = dir / (tid+"-patch");
            sfo(game,tid); sfo(patch,tid);
            check(game_region::TitleId(game)==tid && game_region::FindPatch(game)==patch);
            const auto opposite = tid=="CUSA01176" ? "CUSA00935" : "CUSA01176";
            sfo(patch,opposite); check(game_region::FindPatch(game).empty());
            const auto dlc = dir / "shadPS4" / "addcont" / tid;
            std::filesystem::create_directories(dlc);
            check(game_region::Addcont(game,dir)==dlc);
            std::filesystem::remove(dlc);
            std::filesystem::create_directories(dir/"shadPS4"/"addcont"/opposite);
            check(game_region::Addcont(game,dir).empty());
            std::ofstream(game/"sce_sys"/"param.sfo",std::ios::binary)<<"bad";
            check(game_region::TitleId(game).empty() && game_region::FindPatch(game).empty());
        }
        constexpr std::array<uint8_t,8> signature{0x8A,0x83,0xE8,0,0,0,0x24,1};
        std::array<uint8_t,32> bytes{};
        std::array<std::span<uint8_t>,2> segments{std::span(bytes).first(16),std::span(bytes).subspan(16)};
        check(!cutscene_patch::Apply(segments));
        std::copy(signature.begin(),signature.end(),bytes.begin()+24);
        check(cutscene_patch::Apply(segments) && bytes[24]==0xB0 && bytes[25]==1);
        std::copy(signature.begin(),signature.end(),bytes.begin());
        std::copy(signature.begin(),signature.end(),bytes.begin()+24);
        const auto before=bytes; check(!cutscene_patch::Apply(segments) && bytes==before);
        std::array<std::span<uint8_t>,1> short_segment{std::span(bytes).first(7)};
        check(!cutscene_patch::Apply(short_segment) && bytes==before);
        std::filesystem::remove_all(root);
        std::cout<<"Region discovery and bounded cutscene patch checks passed\n";
        return 0;
    } catch (...) { std::filesystem::remove_all(root); throw; }
}
