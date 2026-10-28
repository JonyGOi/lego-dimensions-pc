#pragma once
#include <string_view>

namespace shader_metadata {
// Serialized cache vocabulary, independent of the UI/log language. Existing
// AOT packs and the released compiler use "real sim" / "real nao". Keep new
// output canonical; also read "real yes" emitted by the early English beta.
constexpr const char* DescriptorFlag(bool specialized) {
    return specialized ? "sim" : "nao";
}

inline bool HasActualDescriptors(std::string_view header) {
    header = header.substr(0, header.find_first_of("\r\n"));
    if (!header.starts_with("pair ")) return false;
    bool seen = false, actual = false;
    const auto next = [&header]() {
        const auto begin = header.find_first_not_of(" \t");
        if (begin == std::string_view::npos) { header = {}; return std::string_view{}; }
        header.remove_prefix(begin);
        const auto end = header.find_first_of(" \t");
        const auto token = header.substr(0, end);
        header.remove_prefix(end == std::string_view::npos ? header.size() : end);
        return token;
    };
    while (!header.empty()) {
        if (next() != "real") continue;
        if (seen) return false;
        seen = true;
        const auto flag = next();
        actual = flag == "sim" || flag == "yes";
    }
    return seen && actual;
}
} // namespace shader_metadata
