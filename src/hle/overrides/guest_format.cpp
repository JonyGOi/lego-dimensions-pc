#include "hle/overrides/guest_format.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace hle {

namespace {

enum class Len { None, HH, H, L, LL, BigL };

Len ParseLength(const char*& p) {
    switch (*p) {
    case 'h':
        ++p;
        if (*p == 'h') {
            ++p;
            return Len::HH;
        }
        return Len::H;
    case 'l':
        ++p;
        if (*p == 'l') {
            ++p;
            return Len::LL;
        }
        return Len::L;
    case 'j':
    case 'z':
    case 't':
    case 'q':
        ++p;
        return Len::LL;
    case 'L':
        ++p;
        return Len::BigL;
    default:
        return Len::None;
    }
}

template <typename T>
void AppendFormatted(std::string& out, const std::string& spec, T value) {
    const int n = std::snprintf(nullptr, 0, spec.c_str(), value);
    if (n <= 0) {
        return;
    }
    const size_t old = out.size();
    out.resize(old + n + 1);
    std::snprintf(&out[old], n + 1, spec.c_str(), value);
    out.resize(old + n);
}

bool IsSpace(char c) {
    return std::isspace(static_cast<unsigned char>(c)) != 0;
}

bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

} // namespace

std::string GuestFormat(const char* fmt, SysVVaList& ap) {
    std::string out;
    if (!fmt) {
        return out;
    }
    for (const char* p = fmt; *p;) {
        if (*p != '%') {
            out += *p++;
            continue;
        }
        const char* start = p++;
        if (*p == '%') {
            out += '%';
            ++p;
            continue;
        }
        std::string spec = "%";
        while (*p && std::strchr("-+ #0", *p)) {
            spec += *p++;
        }
        if (*p == '*') {
            ++p;
            int width = static_cast<int>(VaNextInt(ap));
            if (width < 0) {
                spec += '-';
                width = -width;
            }
            spec += std::to_string(width);
        } else {
            while (IsDigit(*p)) {
                spec += *p++;
            }
        }
        if (*p == '.') {
            ++p;
            if (*p == '*') {
                ++p;
                const int precision = static_cast<int>(VaNextInt(ap));
                if (precision >= 0) {
                    spec += '.' + std::to_string(precision);
                }
            } else {
                spec += '.';
                while (IsDigit(*p)) {
                    spec += *p++;
                }
            }
        }
        const Len len = ParseLength(p);
        const char conv = *p;
        if (!conv) {
            out.append(start);
            break;
        }
        ++p;

        switch (conv) {
        case 'd':
        case 'i': {
            s64 v = static_cast<s64>(VaNextInt(ap));
            if (len == Len::HH) {
                v = static_cast<signed char>(v);
            } else if (len == Len::H) {
                v = static_cast<short>(v);
            } else if (len == Len::None) {
                v = static_cast<s32>(v);
            }
            AppendFormatted(out, spec + "lld", static_cast<long long>(v));
            break;
        }
        case 'u':
        case 'x':
        case 'X':
        case 'o': {
            u64 v = VaNextInt(ap);
            if (len == Len::HH) {
                v = static_cast<u8>(v);
            } else if (len == Len::H) {
                v = static_cast<u16>(v);
            } else if (len == Len::None) {
                v = static_cast<u32>(v);
            }
            AppendFormatted(out, spec + "ll" + conv, static_cast<unsigned long long>(v));
            break;
        }
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A': {
            const double v = len == Len::BigL ? VaNextLongDouble(ap) : VaNextDouble(ap);
            AppendFormatted(out, spec + conv, v);
            break;
        }
        case 'c':
            AppendFormatted(out, spec + 'c', static_cast<int>(VaNextInt(ap)));
            break;
        case 's': {
            const void* ptr = reinterpret_cast<const void*>(VaNextInt(ap));
            std::string narrow;
            const char* s = static_cast<const char*>(ptr);
            if (!ptr) {
                s = "(null)";
            } else if (len == Len::L) { // wchar_t do Orbis tem 32 bits
                for (const u32* w = static_cast<const u32*>(ptr); *w; ++w) {
                    narrow += *w < 0x80 ? static_cast<char>(*w) : '?';
                }
                s = narrow.c_str();
            }
            AppendFormatted(out, spec + 's', s);
            break;
        }
        case 'p':
            AppendFormatted(out, std::string("0x%llx"), static_cast<unsigned long long>(VaNextInt(ap)));
            break;
        case 'n': {
            void* dst = reinterpret_cast<void*>(VaNextInt(ap));
            const s64 count = static_cast<s64>(out.size());
            if (dst) {
                const size_t bytes = len == Len::HH ? 1 : len == Len::H ? 2 : len == Len::None ? 4 : 8;
                std::memcpy(dst, &count, bytes);
            }
            break;
        }
        default:
            out.append(start, p); // conversao desconhecida: copia literalmente
            break;
        }
    }
    return out;
}

int GuestScan(const char* input, const char* fmt, SysVVaList& ap) {
    if (!input || !fmt) {
        return -1;
    }
    const char* in = input;
    int assigned = 0;

    for (const char* f = fmt; *f;) {
        if (IsSpace(*f)) {
            while (IsSpace(*f)) {
                ++f;
            }
            while (IsSpace(*in)) {
                ++in;
            }
            continue;
        }
        if (*f != '%') {
            if (*in != *f) {
                break;
            }
            ++in;
            ++f;
            continue;
        }
        ++f;
        if (*f == '%') {
            if (*in != '%') {
                break;
            }
            ++in;
            ++f;
            continue;
        }
        bool suppress = false;
        if (*f == '*') {
            suppress = true;
            ++f;
        }
        size_t width = 0;
        while (IsDigit(*f)) {
            width = width * 10 + (*f++ - '0');
        }
        const Len len = ParseLength(f);
        const char conv = *f;
        if (!conv) {
            break;
        }
        ++f;

        if (conv == 'n') {
            if (!suppress) {
                const s32 consumed = static_cast<s32>(in - input);
                std::memcpy(reinterpret_cast<void*>(VaNextInt(ap)), &consumed, sizeof(consumed));
            }
            continue;
        }
        if (conv != 'c' && conv != '[') {
            while (IsSpace(*in)) {
                ++in;
            }
        }
        if (!*in) {
            return assigned ? assigned : -1;
        }

        switch (conv) {
        case 'd':
        case 'i':
        case 'u':
        case 'x':
        case 'X':
        case 'o': {
            char tmp[72];
            const size_t max = width && width < sizeof(tmp) - 1 ? width : sizeof(tmp) - 1;
            std::strncpy(tmp, in, max);
            tmp[max] = 0;
            const int base = conv == 'i' ? 0 : (conv == 'x' || conv == 'X') ? 16 : conv == 'o' ? 8 : 10;
            char* end;
            const u64 v = (conv == 'd' || conv == 'i') ? static_cast<u64>(_strtoi64(tmp, &end, base))
                                                       : _strtoui64(tmp, &end, base);
            if (end == tmp) {
                return assigned;
            }
            in += end - tmp;
            if (!suppress) {
                const size_t bytes = len == Len::HH ? 1 : len == Len::H ? 2 : len == Len::None ? 4 : 8;
                std::memcpy(reinterpret_cast<void*>(VaNextInt(ap)), &v, bytes);
                ++assigned;
            }
            break;
        }
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A': {
            char tmp[128];
            const size_t max = width && width < sizeof(tmp) - 1 ? width : sizeof(tmp) - 1;
            std::strncpy(tmp, in, max);
            tmp[max] = 0;
            char* end;
            const double v = std::strtod(tmp, &end);
            if (end == tmp) {
                return assigned;
            }
            in += end - tmp;
            if (!suppress) {
                void* dst = reinterpret_cast<void*>(VaNextInt(ap));
                if (len == Len::None) {
                    const float fv = static_cast<float>(v);
                    std::memcpy(dst, &fv, sizeof(fv));
                } else {
                    std::memcpy(dst, &v, sizeof(v));
                }
                ++assigned;
            }
            break;
        }
        case 's': {
            char* dst = suppress ? nullptr : reinterpret_cast<char*>(VaNextInt(ap));
            size_t n = 0;
            while (*in && !IsSpace(*in) && (!width || n < width)) {
                if (dst) {
                    dst[n] = *in;
                }
                ++n;
                ++in;
            }
            if (dst) {
                dst[n] = 0;
                ++assigned;
            }
            break;
        }
        case 'c': {
            char* dst = suppress ? nullptr : reinterpret_cast<char*>(VaNextInt(ap));
            const size_t count = width ? width : 1;
            for (size_t n = 0; n < count; ++n) {
                if (!*in) {
                    return assigned ? assigned : -1;
                }
                if (dst) {
                    dst[n] = *in;
                }
                ++in;
            }
            if (dst) {
                ++assigned;
            }
            break;
        }
        case '[': {
            bool negate = false;
            bool set[256] = {};
            if (*f == '^') {
                negate = true;
                ++f;
            }
            if (*f == ']') {
                set[static_cast<u8>(']')] = true;
                ++f;
            }
            while (*f && *f != ']') {
                if (f[1] == '-' && f[2] && f[2] != ']') {
                    for (int c = static_cast<u8>(f[0]); c <= static_cast<u8>(f[2]); ++c) {
                        set[c] = true;
                    }
                    f += 3;
                } else {
                    set[static_cast<u8>(*f++)] = true;
                }
            }
            if (*f == ']') {
                ++f;
            }
            char* dst = suppress ? nullptr : reinterpret_cast<char*>(VaNextInt(ap));
            size_t n = 0;
            while (*in && set[static_cast<u8>(*in)] != negate && (!width || n < width)) {
                if (dst) {
                    dst[n] = *in;
                }
                ++n;
                ++in;
            }
            if (!n) {
                return assigned;
            }
            if (dst) {
                dst[n] = 0;
                ++assigned;
            }
            break;
        }
        default:
            return assigned;
        }
    }
    return assigned;
}

} // namespace hle
