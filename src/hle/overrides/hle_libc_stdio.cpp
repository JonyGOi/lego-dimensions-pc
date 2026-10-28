// libc.prx - stdio: objetos FILE, saida de console do jogo ([GAME]) e familia printf/scanf.
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>

#include "hle/overrides/guest_format.h"
#include "hle/overrides/hle_filesystem.h"
#include "hle/overrides/hle_override.h"
#include "hle/overrides/sysv_va.h"
#include "hle_symbol_table.h"

namespace hle {

void InitKernelDataObjects();

namespace {

// Inicio do FILE da Dinkumware no Orbis (456 bytes no total). So os campos que identificam o
// stream sao preenchidos; os ponteiros de buffer ficam nulos, o que forca qualquer macro
// inline de getc/putc a cair na funcao - e a funcao e nossa.
struct OrbisFileHeader {
    u16 mode;
    u8 idx;
    u8 pad;
    s32 handle;
};
constexpr u16 kModeRead = 0x0001;
constexpr u16 kModeWrite = 0x0002;

// FILE devolvido por fopen: opaco para o jogo.
constexpr u32 kHostFileMagic = 0x4C494648; // "HFIL"
struct HostFile {
    u32 magic = kHostFileMagic;
    std::FILE* fp = nullptr;
    std::string path;
};

void* g_stdin = nullptr;
void* g_stdout = nullptr;
void* g_stderr = nullptr;

// 0 = stdin, 1 = stdout, 2 = stderr, -1 = outro
int StdStream(const void* stream) {
    return stream == g_stdin ? 0 : stream == g_stdout ? 1 : stream == g_stderr ? 2 : -1;
}

HostFile* AsHostFile(void* stream) {
    auto* f = static_cast<HostFile*>(stream);
    return (f && StdStream(stream) < 0 && f->magic == kHostFileMagic) ? f : nullptr;
}

int WriteStream(void* stream, const char* data, u64 size) {
    const int std_stream = StdStream(stream);
    if (std_stream == 1 || std_stream == 2) {
        log::GameText(std_stream, data, size);
        return static_cast<int>(size);
    }
    if (HostFile* f = AsHostFile(stream)) {
        return static_cast<int>(std::fwrite(data, 1, size, f->fp));
    }
    return -1;
}

// ---- saida ------------------------------------------------------------------
HLE_IMPL(u64, fwrite, const void* ptr, u64 size, u64 count, void* stream) {
    HLE_QUIET("%p, %" PRIu64 "x%" PRIu64 ", %p", ptr, size, count, stream);
    if (!size || !count) {
        return 0;
    }
    const int written = WriteStream(stream, static_cast<const char*>(ptr), size * count);
    return written < 0 ? 0 : static_cast<u64>(written) / size;
}
HLE_OVERRIDE(fwrite);

HLE_IMPL(int, fputs, const char* s, void* stream) {
    HLE_QUIET("%p", stream);
    return WriteStream(stream, s, std::strlen(s)) < 0 ? -1 : 0;
}
HLE_OVERRIDE(fputs);

HLE_IMPL(int, fputc, int c, void* stream) {
    HLE_QUIET("%d, %p", c, stream);
    const char ch = static_cast<char>(c);
    return WriteStream(stream, &ch, 1) < 0 ? -1 : static_cast<u8>(ch);
}
HLE_OVERRIDE(fputc);

HLE_IMPL(int, puts, const char* s) {
    HLE_QUIET("%p", static_cast<const void*>(s));
    log::GameText(1, s, std::strlen(s));
    log::GameText(1, "\n", 1);
    return 0;
}
HLE_OVERRIDE(puts);

HLE_IMPL(int, putchar, int c) {
    HLE_QUIET("%d", c);
    const char ch = static_cast<char>(c);
    log::GameText(1, &ch, 1);
    return static_cast<u8>(ch);
}
HLE_OVERRIDE(putchar);

HLE_IMPL(int, fflush, void* stream) {
    HLE_QUIET("%p", stream);
    if (HostFile* f = AsHostFile(stream)) {
        return std::fflush(f->fp);
    }
    return 0; // texto do console sai por linha; nada a descarregar
}
HLE_OVERRIDE(fflush);

// ---- printf -----------------------------------------------------------------
// Ver sysv_va.h: nada de "..." aqui, os argumentos chegam por HLE_VA_PARAMS.
HLE_IMPL(int, printf, HLE_VA_PARAMS) {
    HLE_VA_CAPTURE(va, 1);
    HLE_QUIET("\"%.48s\"", reinterpret_cast<const char*>(rdi));
    const std::string text = GuestFormat(reinterpret_cast<const char*>(rdi), va.list);
    log::GameText(1, text.data(), text.size());
    return static_cast<int>(text.size());
}
HLE_OVERRIDE(printf);

HLE_IMPL(int, fprintf, HLE_VA_PARAMS) {
    HLE_VA_CAPTURE(va, 2);
    HLE_QUIET("%p, \"%.48s\"", reinterpret_cast<void*>(rdi), reinterpret_cast<const char*>(rsi));
    const std::string text = GuestFormat(reinterpret_cast<const char*>(rsi), va.list);
    WriteStream(reinterpret_cast<void*>(rdi), text.data(), text.size());
    return static_cast<int>(text.size());
}
HLE_OVERRIDE(fprintf);

HLE_IMPL(int, sprintf, HLE_VA_PARAMS) {
    HLE_VA_CAPTURE(va, 2);
    HLE_QUIET("%p, \"%.48s\"", reinterpret_cast<void*>(rdi), reinterpret_cast<const char*>(rsi));
    const std::string text = GuestFormat(reinterpret_cast<const char*>(rsi), va.list);
    std::memcpy(reinterpret_cast<char*>(rdi), text.c_str(), text.size() + 1);
    return static_cast<int>(text.size());
}
HLE_OVERRIDE(sprintf);

int StoreBounded(char* dst, u64 size, const std::string& text) {
    if (dst && size) {
        const u64 n = text.size() < size - 1 ? text.size() : size - 1;
        std::memcpy(dst, text.data(), n);
        dst[n] = 0;
    }
    return static_cast<int>(text.size());
}

HLE_IMPL(int, snprintf, HLE_VA_PARAMS) {
    HLE_VA_CAPTURE(va, 3);
    HLE_QUIET("%p, %" PRIu64 ", \"%.48s\"", reinterpret_cast<void*>(rdi), rsi,
              reinterpret_cast<const char*>(rdx));
    return StoreBounded(reinterpret_cast<char*>(rdi), rsi,
                        GuestFormat(reinterpret_cast<const char*>(rdx), va.list));
}
HLE_OVERRIDE(snprintf);

// As versoes "v" recebem o va_list do jogo, que ja e um SysVVaList. Trabalha-se em uma copia,
// como va_copy, para nao avancar o va_list do chamador.
HLE_IMPL(int, vsnprintf, char* dst, u64 size, const char* fmt, const SysVVaList* ap) {
    HLE_QUIET("%p, %" PRIu64 ", \"%.48s\"", static_cast<void*>(dst), size, fmt);
    SysVVaList copy = *ap;
    return StoreBounded(dst, size, GuestFormat(fmt, copy));
}
HLE_OVERRIDE(vsnprintf);

HLE_IMPL(int, vsprintf, char* dst, const char* fmt, const SysVVaList* ap) {
    HLE_QUIET("%p, \"%.48s\"", static_cast<void*>(dst), fmt);
    SysVVaList copy = *ap;
    const std::string text = GuestFormat(fmt, copy);
    std::memcpy(dst, text.c_str(), text.size() + 1);
    return static_cast<int>(text.size());
}
HLE_OVERRIDE(vsprintf);

HLE_IMPL(int, vprintf, const char* fmt, const SysVVaList* ap) {
    HLE_QUIET("\"%.48s\"", fmt);
    SysVVaList copy = *ap;
    const std::string text = GuestFormat(fmt, copy);
    log::GameText(1, text.data(), text.size());
    return static_cast<int>(text.size());
}
HLE_OVERRIDE(vprintf);

HLE_IMPL(int, sscanf, HLE_VA_PARAMS) {
    HLE_VA_CAPTURE(va, 2);
    HLE_QUIET("\"%.32s\", \"%.32s\"", reinterpret_cast<const char*>(rdi), reinterpret_cast<const char*>(rsi));
    return GuestScan(reinterpret_cast<const char*>(rdi), reinterpret_cast<const char*>(rsi), va.list);
}
HLE_OVERRIDE(sscanf);

// ---- arquivos ---------------------------------------------------------------
HLE_IMPL(void*, fopen, const char* path, const char* mode) {
    const fs::Resolved resolved = fs::Resolve(path);
    std::string bin_mode = mode ? mode : "r";
    const bool writes = bin_mode.find_first_of("wa+") != std::string::npos;
    if (bin_mode.find('b') == std::string::npos) {
        bin_mode += 'b'; // o Orbis nao traduz fim de linha
    }
    std::FILE* fp = nullptr;
    const char* status = "NOT FOUND";
    if (!resolved.mounted) {
        status = "NO MOUNT";
    } else if (resolved.read_only && writes) {
        status = "READ ONLY (/app0)";
    } else if ((fp = _wfopen(resolved.host.c_str(), std::wstring(bin_mode.begin(), bin_mode.end()).c_str())) != nullptr) { // Fase 60: caminho largo
        status = "OK";
    }
    HostFile* file = nullptr;
    if (fp) {
        file = new HostFile;
        file->fp = fp;
        file->path = fs::PathText(resolved.host);
    }
    log::Fs("fopen \"%s\" mode \"%s\" -> %s  [%s]", path ? path : "(null)", mode ? mode : "", status,
            fs::PathText(resolved.host).c_str());
    HLE_QUIET("\"%s\", \"%s\"", path, mode);
    return file;
}
HLE_OVERRIDE(fopen);

HLE_IMPL(int, fclose, void* stream) {
    HostFile* f = AsHostFile(stream);
    HLE_TRACE("%p %s", stream, f ? f->path.c_str() : "");
    if (!f) {
        return -1;
    }
    const int result = std::fclose(f->fp);
    f->magic = 0;
    delete f;
    return result;
}
HLE_OVERRIDE(fclose);

HLE_IMPL(u64, fread, void* ptr, u64 size, u64 count, void* stream) {
    HLE_QUIET("%p, %" PRIu64 "x%" PRIu64 ", %p", ptr, size, count, stream);
    HostFile* f = AsHostFile(stream);
    return f ? std::fread(ptr, size, count, f->fp) : 0;
}
HLE_OVERRIDE(fread);

HLE_IMPL(int, fseek, void* stream, s64 offset, int whence) {
    HLE_QUIET("%p, %" PRId64 ", %d", stream, offset, whence);
    HostFile* f = AsHostFile(stream);
    return f ? _fseeki64(f->fp, offset, whence) : -1;
}
HLE_OVERRIDE(fseek);

HLE_IMPL(s64, ftell, void* stream) {
    HLE_QUIET("%p", stream);
    HostFile* f = AsHostFile(stream);
    return f ? _ftelli64(f->fp) : -1;
}
HLE_OVERRIDE(ftell);

HLE_IMPL(int, getc, void* stream) {
    HLE_QUIET("%p", stream);
    HostFile* f = AsHostFile(stream);
    return f ? std::fgetc(f->fp) : -1; // stdin: sempre EOF
}
HLE_OVERRIDE(getc);

HLE_IMPL(int, ferror, void* stream) {
    HLE_QUIET("%p", stream);
    HostFile* f = AsHostFile(stream);
    return f ? std::ferror(f->fp) : 0;
}
HLE_OVERRIDE(ferror);

HLE_IMPL(void, clearerr, void* stream) {
    HLE_QUIET("%p", stream);
    if (HostFile* f = AsHostFile(stream)) {
        std::clearerr(f->fp);
    }
}
HLE_OVERRIDE(clearerr);

HLE_IMPL(int, fileno, void* stream) {
    HLE_QUIET("%p", stream);
    const int std_stream = StdStream(stream);
    if (std_stream >= 0) {
        return std_stream;
    }
    HostFile* f = AsHostFile(stream);
    return f ? _fileno(f->fp) + 100 : -1; // fora da faixa dos descritores do kernel HLE
}
HLE_OVERRIDE(fileno);

void InitStdStream(const char* name, void*& slot, u16 mode, s32 handle) {
    const HleSymbol* sym = FindSymbolByName(name);
    if (!sym) {
        return;
    }
    slot = sym->address;
    const OrbisFileHeader header{mode, static_cast<u8>(handle), 0, handle};
    std::memcpy(slot, &header, sizeof(header));
}

} // namespace

void InitDataObjects() {
    InitStdStream("_Stdin", g_stdin, kModeRead, 0);
    InitStdStream("_Stdout", g_stdout, kModeWrite, 1);
    InitStdStream("_Stderr", g_stderr, kModeWrite, 2);
    if (const HleSymbol* need = FindSymbolByName("Need_sceLibc")) {
        const u32 one = 1;
        std::memcpy(need->address, &one, sizeof(one));
    }
    // _ZTVN10__cxxabiv117__class_type_infoE (vtable de type_info) continua zerada: so e lida
    // por dynamic_cast/typeid e pelo unwinder de excecoes, que ainda nao existem.
    InitKernelDataObjects();
}

} // namespace hle
