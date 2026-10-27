#pragma once
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// CUSA00935 discovery adapted from JonyGOi/lego-dimensions-pc, commit 0aeb8b9.
// Read metadata rather than guessing the region from a renamed folder.
namespace game_region {
inline std::string TitleId(const std::filesystem::path& game) {
    std::ifstream file(game / "sce_sys" / "param.sfo", std::ios::binary | std::ios::ate);
    if (!file) return {};
    const auto size = file.tellg();
    if (size < 20 || size > 1024 * 1024) return {};
    std::vector<unsigned char> data(static_cast<size_t>(size));
    file.seekg(0);
    if (!file.read(reinterpret_cast<char*>(data.data()), size) || std::memcmp(data.data(), "\0PSF", 4)) return {};
    const auto u32 = [&](size_t at) { uint32_t value; std::memcpy(&value, data.data() + at, 4); return value; };
    const auto u16 = [&](size_t at) { uint16_t value; std::memcpy(&value, data.data() + at, 2); return value; };
    const size_t keys = u32(8), values = u32(12), count = u32(16);
    if (u32(4) != 0x101 || !count || count > 512 || 20 + 16 * count > keys || keys > values || values > data.size()) return {};
    std::string result;
    for (size_t i = 0; i < count; ++i) {
        const size_t row = 20 + 16 * i, key = keys + u16(row), length = u32(row + 4), capacity = u32(row + 8), offset = u32(row + 12);
        if (key >= values || length > capacity || offset > data.size() - values || capacity > data.size() - values - offset) return {};
        const auto end = static_cast<const unsigned char*>(std::memchr(data.data() + key, 0, values - key));
        if (!end) return {};
        if (std::string(reinterpret_cast<const char*>(data.data() + key), end - data.data() - key) != "TITLE_ID") continue;
        if (!result.empty() || u16(row + 2) != 0x204 || length != 10 || data[values + offset + 9]) return {};
        result.assign(reinterpret_cast<const char*>(data.data() + values + offset), 9);
        if (result != "CUSA01176" && result != "CUSA00935") return {};
    }
    return result;
}

inline std::filesystem::path FindPatch(const std::filesystem::path& game) {
    const auto tid = TitleId(game);
    if (tid.empty()) return {};
    const auto name = game.filename().wstring() + L"-patch";
    for (const auto& path : {game / name, game.parent_path() / name, game / (tid + "-patch"), game.parent_path() / (tid + "-patch")})
        if (TitleId(path) == tid) return path;
    return {};
}

inline std::filesystem::path Addcont(const std::filesystem::path& game, const std::filesystem::path& appdata) {
    const auto tid = TitleId(game);
    if (tid.empty() || appdata.empty()) return {};
    const auto path = appdata / "shadPS4" / "addcont" / tid;
    std::error_code ec;
    return std::filesystem::is_directory(path, ec) ? path : std::filesystem::path{};
}
} // namespace game_region
