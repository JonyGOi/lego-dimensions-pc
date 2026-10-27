// libSceFios2 - os 18 simbolos que o eboot importa, sobre o VFS de hle_filesystem.
//
// Nao ha fonte de referencia local para a ABI do Fios2 (o shadPS4 roda o libSceFios2.prx
// original), entao ela foi conferida nos call sites do eboot (FASE5_callsites_fios2.txt):
//   sceFiosFHOpenSync(attr=0, &fh, path, params)   params+0x00 = openFlags (0x0B, 0x07 ou NULL),
//                                                  params+0x10 = buffer; "nao encontrado" tem de
//                                                  ser 0x80820005, o unico erro tratado em silencio
//   sceFiosFHStatSync(attr=0, fh, stat)            stat+0x00 fileSize, +0x18 data, +0x20 statFlags
//                                                  (bit 0 = diretorio)
//   sceFiosFHReadSync(attr=0, fh, buf, len)        blocos de ate 0x40000; retorno > 0 = bytes
//   sceFiosFHSeek(fh, offset, whence)              whence 0/1/2 = SET/CUR/END
//   sceFiosDHOpen(attr=0, &dh, path, {NULL, 0})    sondagem: o jogo exige 0x80820008 em
//                                                  sceFiosOpWait e pede o tamanho do buffer em
//                                                  sceFiosOpGetActualCount; so entao chama
//   sceFiosDHOpenSync(attr, &dh, path, {buf, n})   com o buffer alocado
//   sceFiosDHReadSync(attr=0, dh, entry)           entry+0x00 fileSize, +0x08 statFlags,
//                                                  +0x10 offsetToName, +0x18 fullPath
// Tudo e executado de forma sincrona.
#include <cinttypes>
#include <cstring>
#include <map>
#include <mutex>

#include "hle/overrides/hle_filesystem.h"
#include "hle/overrides/hle_override.h"

namespace {

using namespace hle;

constexpr int kFiosErrorBadFh = static_cast<int>(0x8082000B);
constexpr int kFiosErrorBadDh = static_cast<int>(0x8082000C);
constexpr int kFiosErrorBadOp = static_cast<int>(0x8082000A);
constexpr int kFiosErrorBadPtr = static_cast<int>(0x80820006);
constexpr int kFiosErrorEof = static_cast<int>(0x80820010);
constexpr int kFiosErrorBadPath = static_cast<int>(0x80820005); // caminho/arquivo inexistente
constexpr int kFiosErrorBadSize = static_cast<int>(0x80820008); // buffer pequeno demais

// Erros do VFS que o jogo espera no vocabulario do Fios2.
int ToFiosError(int error) {
    return error == fs::kENOENT || error == fs::kENOTDIR ? kFiosErrorBadPath : error;
}

// SceFiosOpenParams::openFlags
constexpr u32 kOpenWrite = 1u << 1;
constexpr u32 kOpenAppend = 1u << 2;
constexpr u32 kOpenCreate = 1u << 3;
constexpr u32 kOpenTruncate = 1u << 4;

// SceFiosStat::statFlags
constexpr u32 kStatDirectory = 1u << 0;
constexpr u32 kStatReadable = 1u << 1;
constexpr u32 kStatWritable = 1u << 2;

struct SceFiosOpenParams {
    u32 open_flags;
    u32 op_flags;
    u32 reserved;
    void* buffer_ptr;
    u64 buffer_length;
};

struct SceFiosStat {
    s64 file_size;
    u64 access_date;
    u64 modification_date;
    u64 creation_date;
    u32 stat_flags;
    u32 reserved;
    s64 uid, gid, dev, ino, mode;
};
static_assert(sizeof(SceFiosStat) == 80);

struct SceFiosDirEntry {
    s64 file_size;
    u32 stat_flags;
    u16 name_length;
    u16 full_path_length;
    u16 offset_to_name;
    u16 reserved[3];
    char full_path[1024];
};

struct DirHandle {
    std::string guest_path;
    std::vector<fs::DirEntry> entries;
    size_t next = 0;
};

struct Operation {
    int result;
    s64 actual_count;
};

std::mutex g_mutex;
std::map<int, std::shared_ptr<fs::File>> g_files;
std::map<int, std::shared_ptr<DirHandle>> g_dirs;
std::map<int, Operation> g_ops;
int g_next_fh = 0x1000;
int g_next_dh = 0x2000;
int g_next_op = 0x3000;

std::shared_ptr<fs::File> GetFile(int fh) {
    std::lock_guard lock(g_mutex);
    const auto it = g_files.find(fh);
    return it == g_files.end() ? nullptr : it->second;
}

u64 UnixToFiosDate(s64 seconds) {
    return static_cast<u64>(seconds) * 1000000000ULL; // SceFiosDate: nanossegundos desde 1970
}

HLE_IMPL(int, sceFiosInitialize, const void* params) {
    HLE_TRACE("%p", params);
    return err::OK;
}
HLE_OVERRIDE(sceFiosInitialize);

HLE_IMPL(int, sceFiosFHOpenSync, const void* attr, int* out_fh, const char* path, const SceFiosOpenParams* params) {
    if (!out_fh || !path) {
        return kFiosErrorBadPtr;
    }
    const u32 flags = params ? params->open_flags : 0;
    fs::OpenMode mode;
    mode.write = (flags & kOpenWrite) != 0;
    mode.append = (flags & kOpenAppend) != 0;
    mode.create = (flags & kOpenCreate) != 0;
    mode.truncate = (flags & kOpenTruncate) != 0;
    int error = 0;
    auto file = fs::Open("sceFiosFHOpenSync", path, mode, &error);
    if (!file) {
        *out_fh = 0;
        HLE_QUIET("\"%s\" flags=0x%x -> error 0x%x", path, flags, ToFiosError(error));
        return ToFiosError(error);
    }
    std::lock_guard lock(g_mutex);
    *out_fh = g_next_fh++;
    g_files[*out_fh] = std::move(file);
    HLE_QUIET("\"%s\" flags=0x%x -> fh 0x%x", path, flags, *out_fh);
    return err::OK;
}
HLE_OVERRIDE(sceFiosFHOpenSync);

HLE_IMPL(int, sceFiosFHCloseSync, const void* attr, int fh) {
    HLE_QUIET("fh 0x%x", fh);
    std::lock_guard lock(g_mutex);
    return g_files.erase(fh) ? err::OK : kFiosErrorBadFh;
}
HLE_OVERRIDE(sceFiosFHCloseSync);

HLE_IMPL(int, sceFiosFHStatSync, const void* attr, int fh, SceFiosStat* out) {
    HLE_QUIET("fh 0x%x", fh);
    const auto file = GetFile(fh);
    if (!file) {
        return kFiosErrorBadFh;
    }
    if (!out) {
        return kFiosErrorBadPtr;
    }
    std::memset(out, 0, sizeof(*out));
    out->file_size = file->Size();
    out->access_date = out->modification_date = out->creation_date = UnixToFiosDate(file->ModifiedTime());
    out->stat_flags = kStatReadable | (fs::Resolve(file->guest_path().c_str()).read_only ? 0 : kStatWritable);
    return err::OK;
}
HLE_OVERRIDE(sceFiosFHStatSync);

HLE_IMPL(s64, sceFiosFHReadSync, const void* attr, int fh, void* buffer, s64 length) {
    HLE_QUIET("fh 0x%x, %p, %" PRId64, fh, buffer, length);
    const auto file = GetFile(fh);
    if (!file) {
        return kFiosErrorBadFh;
    }
    if (length < 0 || (!buffer && length)) {
        return kFiosErrorBadPtr;
    }
    const s64 n = file->Read(buffer, static_cast<u64>(length));
    return n < 0 ? fs::kEIO : n;
}
HLE_OVERRIDE(sceFiosFHReadSync);

HLE_IMPL(s64, sceFiosFHWriteSync, const void* attr, int fh, const void* buffer, s64 length) {
    HLE_QUIET("fh 0x%x, %p, %" PRId64, fh, buffer, length);
    const auto file = GetFile(fh);
    if (!file) {
        return kFiosErrorBadFh;
    }
    if (length < 0 || (!buffer && length)) {
        return kFiosErrorBadPtr;
    }
    const s64 n = file->Write(buffer, static_cast<u64>(length));
    return n < 0 ? fs::kEROFS : n;
}
HLE_OVERRIDE(sceFiosFHWriteSync);

HLE_IMPL(s64, sceFiosFHSeek, int fh, s64 offset, int whence) {
    HLE_QUIET("fh 0x%x, %" PRId64 ", %d", fh, offset, whence);
    const auto file = GetFile(fh);
    if (!file) {
        return kFiosErrorBadFh;
    }
    const s64 pos = file->Seek(offset, whence);
    return pos < 0 ? static_cast<s64>(err::kEINVAL) : pos;
}
HLE_OVERRIDE(sceFiosFHSeek);

HLE_IMPL(int, sceFiosFileDeleteSync, const void* attr, const char* path) {
    HLE_QUIET("\"%s\"", path);
    return ToFiosError(fs::Remove("sceFiosFileDeleteSync", path, false));
}
HLE_OVERRIDE(sceFiosFileDeleteSync);

HLE_IMPL(int, sceFiosDirectoryCreateSync, const void* attr, const char* path) {
    HLE_QUIET("\"%s\"", path);
    return ToFiosError(fs::MakeDirectory("sceFiosDirectoryCreateSync", path));
}
HLE_OVERRIDE(sceFiosDirectoryCreateSync);

HLE_IMPL(int, sceFiosDirectoryDeleteSync, const void* attr, const char* path) {
    HLE_QUIET("\"%s\"", path);
    return ToFiosError(fs::Remove("sceFiosDirectoryDeleteSync", path, true));
}
HLE_OVERRIDE(sceFiosDirectoryDeleteSync);

HLE_IMPL(int, sceFiosRenameSync, const void* attr, const char* from, const char* to) {
    HLE_QUIET("\"%s\" -> \"%s\"", from, to);
    return ToFiosError(fs::Rename("sceFiosRenameSync", from, to));
}
HLE_OVERRIDE(sceFiosRenameSync);

// Espaco que o Fios2 pediria para guardar a listagem; o jogo aloca exatamente isto.
u64 DirectoryBufferSize(size_t entries) {
    return (entries + 1) * sizeof(SceFiosDirEntry);
}

// Se `list` nao for nulo, devolve a listagem sem criar handle (sondagem do sceFiosDHOpen).
int OpenDirectory(const char* api, int* out_dh, const char* path, u64 buffer_size, u64* needed_size) {
    if (!out_dh || !path) {
        return kFiosErrorBadPtr;
    }
    auto dir = std::make_shared<DirHandle>();
    dir->guest_path = path;
    const int error = fs::ListDirectory(api, path, &dir->entries);
    if (error) {
        return ToFiosError(error);
    }
    const u64 needed = DirectoryBufferSize(dir->entries.size());
    if (needed_size) {
        *needed_size = needed;
    }
    if (buffer_size < needed) {
        return kFiosErrorBadSize;
    }
    std::lock_guard lock(g_mutex);
    *out_dh = g_next_dh++;
    g_dirs[*out_dh] = std::move(dir);
    return err::OK;
}

// O SceFiosBuffer (ponteiro + tamanho) passado por valor chega em dois registradores.
HLE_IMPL(int, sceFiosDHOpenSync, const void* attr, int* out_dh, const char* path, void* buffer, u64 buffer_size) {
    const int result = OpenDirectory("sceFiosDHOpenSync", out_dh, path, buffer_size, nullptr);
    HLE_QUIET("\"%s\" buffer=%" PRIu64 " -> 0x%x", path, buffer_size, result);
    return result;
}
HLE_OVERRIDE(sceFiosDHOpenSync);

HLE_IMPL(int, sceFiosDHOpen, const void* attr, int* out_dh, const char* path, void* buffer, u64 buffer_size) {
    u64 needed = 0;
    const int result = OpenDirectory("sceFiosDHOpen", out_dh, path, buffer_size, &needed);
    HLE_QUIET("\"%s\" buffer=%" PRIu64 " -> 0x%x, requires %" PRIu64, path, buffer_size, result, needed);
    std::lock_guard lock(g_mutex);
    const int op = g_next_op++;
    g_ops[op] = {result, static_cast<s64>(needed)};
    return op;
}
HLE_OVERRIDE(sceFiosDHOpen);

HLE_IMPL(int, sceFiosDHCloseSync, const void* attr, int dh) {
    HLE_QUIET("dh 0x%x", dh);
    std::lock_guard lock(g_mutex);
    return g_dirs.erase(dh) ? err::OK : kFiosErrorBadDh;
}
HLE_OVERRIDE(sceFiosDHCloseSync);

HLE_IMPL(int, sceFiosDHReadSync, const void* attr, int dh, SceFiosDirEntry* out) {
    HLE_QUIET("dh 0x%x", dh);
    std::shared_ptr<DirHandle> dir;
    {
        std::lock_guard lock(g_mutex);
        const auto it = g_dirs.find(dh);
        if (it == g_dirs.end()) {
            return kFiosErrorBadDh;
        }
        dir = it->second;
    }
    if (!out) {
        return kFiosErrorBadPtr;
    }
    if (dir->next >= dir->entries.size()) {
        return kFiosErrorEof;
    }
    const fs::DirEntry& entry = dir->entries[dir->next++];
    std::string full = dir->guest_path;
    if (full.empty() || full.back() != '/') {
        full += '/';
    }
    const size_t name_offset = full.size();
    full += entry.name;
    std::memset(out, 0, sizeof(*out));
    out->file_size = entry.size;
    out->stat_flags = kStatReadable | (entry.is_directory ? kStatDirectory : 0);
    out->name_length = static_cast<u16>(entry.name.size());
    out->full_path_length = static_cast<u16>(full.size());
    out->offset_to_name = static_cast<u16>(name_offset);
    std::strncpy(out->full_path, full.c_str(), sizeof(out->full_path) - 1);
    return err::OK;
}
HLE_OVERRIDE(sceFiosDHReadSync);

HLE_IMPL(int, sceFiosOpWait, int op) {
    HLE_QUIET("op 0x%x", op);
    std::lock_guard lock(g_mutex);
    const auto it = g_ops.find(op);
    return it == g_ops.end() ? kFiosErrorBadOp : it->second.result;
}
HLE_OVERRIDE(sceFiosOpWait);

HLE_IMPL(s64, sceFiosOpGetActualCount, int op) {
    HLE_QUIET("op 0x%x", op);
    std::lock_guard lock(g_mutex);
    const auto it = g_ops.find(op);
    return it == g_ops.end() ? 0 : it->second.actual_count;
}
HLE_OVERRIDE(sceFiosOpGetActualCount);

HLE_IMPL(void, sceFiosOpDelete, int op) {
    HLE_QUIET("op 0x%x", op);
    std::lock_guard lock(g_mutex);
    g_ops.erase(op);
}
HLE_OVERRIDE(sceFiosOpDelete);

} // namespace
