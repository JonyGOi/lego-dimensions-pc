#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "common/types.h"

// Sistema de arquivos virtual do Orbis, compartilhado por libkernel (sceKernelOpen...),
// libSceFios2 e fopen da libc. Todo acesso gera uma linha "[FS]" no log.
//
//   /app0/...       -> diretorio do jogo (--app0). SOMENTE LEITURA: e o dump original.
//   /temp0/...      -> assets/temp0/
//   /savedataN/...  -> assets/savedata0/<dirName>/, so enquanto sceSaveDataMount estiver ativo
//   /download0/...  -> assets/download0/
namespace hle::fs {

// Erros do Orbis usados pelo VFS (0x80020000 | errno do FreeBSD)
constexpr int kENOENT = static_cast<int>(0x80020002);
constexpr int kEBADF = static_cast<int>(0x80020009);
constexpr int kEEXIST = static_cast<int>(0x80020011);
constexpr int kENOTDIR = static_cast<int>(0x80020014);
constexpr int kEISDIR = static_cast<int>(0x80020015);
constexpr int kEROFS = static_cast<int>(0x8002001E);
constexpr int kEIO = static_cast<int>(0x80020005);

struct Resolved {
    std::filesystem::path host;
    bool mounted = false;   // false: caminho fora de qualquer mount conhecido
    bool read_only = false; // /app0
};

struct OpenMode {
    bool write = false;
    bool create = false;
    bool truncate = false;
    bool append = false;
};

struct DirEntry {
    std::string name;
    bool is_directory;
    s64 size;
};

struct StatInfo {
    bool is_directory;
    s64 size;
    s64 mtime; // segundos desde 1970
};

// Arquivo aberto no host. Leitura/escrita posicional nao mexe no cursor, como pread/pwrite.
class File {
public:
    File(void* handle, std::string guest_path, bool writable);
    ~File();
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    s64 Read(void* buffer, u64 size);
    s64 Write(const void* buffer, u64 size);
    s64 Pread(void* buffer, u64 size, s64 offset);
    s64 Pwrite(const void* buffer, u64 size, s64 offset);
    s64 Seek(s64 offset, int whence); // 0=SET 1=CUR 2=END; devolve a nova posicao ou -1
    s64 Size();
    s64 ModifiedTime();
    const std::string& guest_path() const { return guest_path_; }

private:
    void* handle_;
    std::string guest_path_;
    bool writable_;
    s64 position_ = 0;
    std::mutex mutex_;
};

// `app0` e o dump do jogo; `writable_root` recebe temp0/, savedata0/ e download0/ (criados se faltarem).
void Configure(const std::filesystem::path& app0, const std::filesystem::path& writable_root);
const std::filesystem::path& App0();
// Fase 58: atualizacao numa pasta separada (formato do shadPS4: CUSA01176-patch). Os arquivos dela vencem
// os do jogo base no /app0; as listagens de pasta juntam as duas.
void SetApp0Patch(const std::filesystem::path& patch_dir);
const std::filesystem::path& App0Patch();
// Raiz dos mounts gravaveis (assets/).
const std::filesystem::path& WritableRoot();

// Mounts criados em tempo de execucao (saves). SetMount substitui um prefixo ja existente.
void SetMount(const std::string& prefix, const std::filesystem::path& host, bool read_only);
void RemoveMount(const std::string& prefix);

// Quantas aberturas bem-sucedidas de arquivos .DAT (pacotes de dados do jogo) ja ocorreram.
u64 DatFilesOpened();

Resolved Resolve(const char* guest_path);

// `api` e o nome da funcao do Orbis, so para o log. Em erro devolve nullptr e preenche *error.
std::shared_ptr<File> Open(const char* api, const char* guest_path, OpenMode mode, int* error);
int Stat(const char* api, const char* guest_path, StatInfo* out);
int ListDirectory(const char* api, const char* guest_path, std::vector<DirEntry>* out);
int Remove(const char* api, const char* guest_path, bool directory);
int Rename(const char* api, const char* from, const char* to);
int MakeDirectory(const char* api, const char* guest_path);

} // namespace hle::fs
