// libSceSaveData (9 simbolos), libSceSaveDataDialog (7), libSceMsgDialog (4) e
// libSceCommonDialog (1).
//
// Cada save e um diretorio em assets/savedata0/<dirName>/. sceSaveDataMount o expoe ao jogo
// como /savedata0 (ou /savedata1.. se houver mais de um montado) pelo VFS. Parametros e icone
// ficam em <dirName>/sce_sys/.
//
// Call sites (FASE7_callsites_savedata_dialogs.txt):
//   sceSaveDataDirNameSearch(cond, result)  cond+0x00 userId, +0x18 chave, +0x1C ordem;
//                                           result+0x08 buffer de nomes (32 bytes cada),
//                                           +0x10 capacidade; o jogo le result+0x00 (hitNum)
//   sceSaveDataMount(mount, result)         mount+0x00 userId, +0x10 dirName, +0x20 blocos,
//                                           +0x28 modo (2 = abrir, 6 = criar); o jogo trata
//                                           0x809F000F (save corrompido) a parte
//   sceSaveDataGetMountInfo(point, info)    o jogo compara info+0x00 (blocos) com 0x140
//
// Dialogos: nao ha interface para mostrar. Todo dialogo aberto termina na hora com resultado
// "OK" e o texto vai para o log.
#include <algorithm>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <cwchar>
#include <string>
#include <thread>
#include <vector>

#include <windows.h>

#include "hle/overrides/hle_filesystem.h"
#include "hle/overrides/hle_override.h"
#include "runtime/lang.h"
#include "hle/overrides/hle_pad_sdl.h"
#include "hle/overrides/hle_videoout.h"

namespace {

using namespace hle;

constexpr int kErrorParameter = static_cast<int>(0x809F0000);
constexpr int kErrorNotInitialized = static_cast<int>(0x809F0001);
constexpr int kErrorBusy = static_cast<int>(0x809F0003);
constexpr int kErrorNotMounted = static_cast<int>(0x809F0004);
constexpr int kErrorExists = static_cast<int>(0x809F0007);
constexpr int kErrorNotFound = static_cast<int>(0x809F0008);
constexpr int kErrorInternal = static_cast<int>(0x809F000B);

constexpr u32 kModeCreate = 0x04;
constexpr u32 kModeCreate2 = 0x20;

// Blocos de 32 KiB. O espaco "do console" e arbitrario: 1 GiB.
constexpr u64 kBlockSize = 0x8000;
constexpr u64 kTotalBlocks = 0x8000;

struct DirName {
    char data[32];
};

struct MountPoint {
    char data[16];
};

struct Mount {
    s32 user_id;
    s32 pad;
    const void* title_id;
    const DirName* dir_name;
    const void* fingerprint;
    u64 blocks;
    u32 mount_mode;
    u8 reserved[32];
};

struct MountResult {
    MountPoint mount_point;
    u64 required_blocks;
    u32 unused;
    u32 mount_status;
    u8 reserved[28];
    s32 pad;
};
static_assert(sizeof(MountResult) == 0x40);

struct MountInfo {
    u64 blocks;
    u64 free_blocks;
    u8 reserved[32];
};

struct Param {
    char title[128];
    char sub_title[128];
    char detail[1024];
    u32 user_param;
    s32 pad;
    s64 mtime;
    u8 reserved[32];
};
static_assert(sizeof(Param) == 1328);

struct SearchCond {
    s32 user_id;
    s32 pad;
    const void* title_id;
    const DirName* dir_name;
    u32 key;
    u32 order;
    u8 reserved[32];
};

struct SearchInfo {
    u64 blocks;
    u64 free_blocks;
    u8 reserved[32];
};

struct SearchResult {
    u32 hit_num;
    s32 pad;
    DirName* dir_names;
    u32 dir_names_num;
    u32 set_num;
    Param* params;
    SearchInfo* infos;
    u8 reserved[12];
    s32 pad2;
};

struct Icon {
    void* buf;
    u64 buf_size;
    u64 data_size;
    u8 reserved[32];
};

struct Delete {
    s32 user_id;
    s32 pad;
    const void* title_id;
    const DirName* dir_name;
    u32 unused;
    u8 reserved[32];
    s32 pad2;
};

std::mutex g_mutex;
bool g_initialized = false;
std::map<std::string, std::string> g_mounted; // "/savedata0" -> dirName
// Fase 32: slot que o jogador escolheu na lista de "salvar" (Novo Jogo) e que ja existe. O jogo
// tenta cria-lo de novo; o save antigo e MOVIDO para uma pasta de backup e o slot nasce vazio.
std::string g_overwrite_slot;
std::string g_saves_dir_name = "savedata0"; // --saves-dir: pasta dos saves dentro de assets (testes usam uma copia)

std::filesystem::path SavesRoot() {
    return fs::WritableRoot() / g_saves_dir_name;
}

bool ValidDirName(const DirName* name) {
    if (!name || !name->data[0] || !std::memchr(name->data, 0, sizeof(name->data))) {
        return false;
    }
    for (const char* c = name->data; *c; ++c) {
        if (*c == '/' || *c == '\\' || *c == ':' || (c[0] == '.' && c[1] == '.')) {
            return false;
        }
    }
    return true;
}

u64 DirectoryBlocks(const std::filesystem::path& dir) {
    std::error_code ec;
    u64 bytes = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir, ec)) {
        if (entry.is_regular_file(ec)) {
            bytes += entry.file_size(ec);
        }
    }
    return (bytes + kBlockSize - 1) / kBlockSize;
}

Param LoadParam(const std::filesystem::path& dir) {
    Param param{};
    std::ifstream in(dir / "sce_sys" / "param.bin", std::ios::binary);
    in.read(reinterpret_cast<char*>(&param), sizeof(param));
    return param;
}

bool StoreParam(const std::filesystem::path& dir, const Param& param) {
    std::error_code ec;
    std::filesystem::create_directories(dir / "sce_sys", ec);
    std::ofstream out(dir / "sce_sys" / "param.bin", std::ios::binary | std::ios::trunc);
    out.write(reinterpret_cast<const char*>(&param), sizeof(param));
    return out.good();
}

// Diretorio do host de um mount point ativo (vazio se nao estiver montado).
std::filesystem::path MountedDir(const MountPoint* point) {
    if (!point || !std::memchr(point->data, 0, sizeof(point->data))) {
        return {};
    }
    const auto it = g_mounted.find(point->data);
    return it == g_mounted.end() ? std::filesystem::path{} : SavesRoot() / it->second;
}

HLE_IMPL(int, sceSaveDataInitialize, const void* param) {
    std::lock_guard lock(g_mutex);
    g_initialized = true;
    std::error_code ec;
    std::filesystem::create_directories(SavesRoot(), ec);
    HLE_TRACE("saves at %s", SavesRoot().string().c_str());
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataInitialize);

HLE_IMPL(int, sceSaveDataDirNameSearch, const SearchCond* cond, SearchResult* result) {
    std::lock_guard lock(g_mutex);
    if (!g_initialized) {
        return kErrorNotInitialized;
    }
    if (!cond || !result) {
        return kErrorParameter;
    }
    std::vector<std::string> names;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(SavesRoot(), ec)) {
        const std::string name = entry.path().filename().string();
        if (entry.is_directory(ec) && name.size() < sizeof(DirName::data)) {
            names.push_back(name);
        }
    }
    std::sort(names.begin(), names.end());
    result->hit_num = static_cast<u32>(names.size());
    result->set_num = 0;
    for (u32 i = 0; result->dir_names && i < result->dir_names_num && i < names.size(); ++i) {
        std::memset(&result->dir_names[i], 0, sizeof(DirName));
        std::strcpy(result->dir_names[i].data, names[i].c_str());
        const std::filesystem::path dir = SavesRoot() / names[i];
        if (result->params) {
            result->params[i] = LoadParam(dir);
        }
        if (result->infos) {
            std::memset(&result->infos[i], 0, sizeof(SearchInfo));
            result->infos[i].blocks = DirectoryBlocks(dir);
            result->infos[i].free_blocks = kTotalBlocks;
        }
        ++result->set_num;
    }
    HLE_TRACE("user=%d capacity=%u -> %u save(s) found", cond->user_id, result->dir_names_num,
              result->hit_num);
    log::Tagged("GAME STATE", "save search: %u found", result->hit_num);
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataDirNameSearch);

HLE_IMPL(int, sceSaveDataMount, const Mount* mount, MountResult* result) {
    int error = err::OK;
    bool created = false;
    std::string point;
    const char* name = (mount && ValidDirName(mount->dir_name)) ? mount->dir_name->data : "";
    {
        std::lock_guard lock(g_mutex);
        if (!g_initialized) {
            error = kErrorNotInitialized;
        } else if (!mount || !result || !*name) {
            error = kErrorParameter;
        } else {
            const std::filesystem::path dir = SavesRoot() / name;
            std::error_code ec;
            bool exists = std::filesystem::is_directory(dir, ec);
            const bool wants_create = (mount->mount_mode & (kModeCreate | kModeCreate2)) != 0;
            if (wants_create && exists && g_overwrite_slot == name) {
                std::time_t now = std::time(nullptr);
                std::tm local{};
                localtime_s(&local, &now);
                char stamp[40];
                std::strftime(stamp, sizeof(stamp), "%Y%m%d_%H%M%S", &local);
                const std::filesystem::path backup = fs::WritableRoot() / (std::string("savedata_backup_novojogo_") + stamp);
                std::filesystem::create_directories(backup, ec);
                std::filesystem::rename(dir, backup / name, ec);
                if (!ec) {
                    exists = false;
                    log::Tagged("GAME STATE", "New Game in slot %s: previous save MOVED to %s", name, (backup / name).string().c_str());
                } else {
                    log::Tagged("GAME STATE", "could not move previous save from %s (%s); slot remains unchanged", name,
                                ec.message().c_str());
                }
                g_overwrite_slot.clear();
            }
            for (const auto& [mounted_point, mounted_name] : g_mounted) {
                if (mounted_name == name) {
                    error = kErrorBusy; // ja montado
                }
            }
            if (error) {
            } else if (wants_create && exists) {
                error = kErrorExists;
            } else if (!wants_create && !exists) {
                error = kErrorNotFound;
            } else {
                if (!exists) {
                    created = std::filesystem::create_directories(dir / "sce_sys", ec);
                    if (!created) {
                        error = kErrorInternal;
                    }
                }
                for (int slot = 0; !error && slot < 16; ++slot) {
                    const std::string candidate = "/savedata" + std::to_string(slot);
                    if (!g_mounted.count(candidate)) {
                        point = candidate;
                        break;
                    }
                }
                if (!error && point.empty()) {
                    error = kErrorBusy;
                }
                if (!error) {
                    g_mounted[point] = name;
                    fs::SetMount(point, dir, false);
                    std::memset(result, 0, sizeof(*result));
                    std::strcpy(result->mount_point.data, point.c_str());
                    result->mount_status = created ? 1 : 0;
                }
            }
        }
    }
    HLE_TRACE("\"%s\" mode=0x%x blocks=%" PRIu64 " -> 0x%x %s%s", name, mount ? mount->mount_mode : 0,
              mount ? mount->blocks : 0, error, point.c_str(), created ? " (created)" : "");
    if (!error) {
        log::Tagged("GAME STATE", "SAVE %s: \"%s\" mounted at %s  [%s]", created ? "CREATED" : "opened", name,
                    point.c_str(), (SavesRoot() / name).string().c_str());
    } else {
        log::Tagged("GAME STATE", "sceSaveDataMount(\"%s\", mode 0x%x) failed: 0x%x%s", name,
                    mount ? mount->mount_mode : 0, error,
                    error == kErrorNotFound ? " (save does not exist)" : error == kErrorExists ? " (already exists)" : "");
    }
    return error;
}
HLE_OVERRIDE(sceSaveDataMount);

HLE_IMPL(int, sceSaveDataUmount, const MountPoint* point) {
    int error = kErrorNotMounted;
    u64 files = 0;
    {
        std::lock_guard lock(g_mutex);
        if (point && std::memchr(point->data, 0, sizeof(point->data))) {
            const auto it = g_mounted.find(point->data);
            if (it != g_mounted.end()) {
                // Registra a hora da ultima gravacao, que o jogo le de volta por GetParam.
                const std::filesystem::path dir = SavesRoot() / it->second;
                Param param = LoadParam(dir);
                param.mtime = std::time(nullptr);
                StoreParam(dir, param);
                std::error_code ec;
                for (const auto& entry : std::filesystem::recursive_directory_iterator(dir, ec)) {
                    files += entry.is_regular_file(ec);
                }
                fs::RemoveMount(it->first);
                g_mounted.erase(it);
                error = err::OK;
            }
        }
    }
    HLE_TRACE("\"%s\" -> 0x%x", point ? point->data : "", error);
    if (!error) {
        log::Tagged("GAME STATE", "SAVE unmounted: %s (%" PRIu64 " file(s) in folder)", point->data, files);
    }
    return error;
}
HLE_OVERRIDE(sceSaveDataUmount);

HLE_IMPL(int, sceSaveDataGetMountInfo, const MountPoint* point, MountInfo* info) {
    std::lock_guard lock(g_mutex);
    const std::filesystem::path dir = MountedDir(point);
    if (dir.empty()) {
        return kErrorNotMounted;
    }
    if (!info) {
        return kErrorParameter;
    }
    const u64 used = DirectoryBlocks(dir);
    std::memset(info, 0, sizeof(*info));
    info->blocks = kTotalBlocks;
    info->free_blocks = used < kTotalBlocks ? kTotalBlocks - used : 0;
    HLE_TRACE("\"%s\" -> %" PRIu64 " blocks, %" PRIu64 " free", point->data, info->blocks, info->free_blocks);
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataGetMountInfo);

// param_type: 0 = struct inteira, 1 = titulo, 2 = subtitulo, 3 = detalhe, 4 = userParam, 5 = mtime
bool ParamField(Param& param, u32 type, void** field, u64* size) {
    switch (type) {
    case 0: *field = &param; *size = sizeof(Param); return true;
    case 1: *field = param.title; *size = sizeof(param.title); return true;
    case 2: *field = param.sub_title; *size = sizeof(param.sub_title); return true;
    case 3: *field = param.detail; *size = sizeof(param.detail); return true;
    case 4: *field = &param.user_param; *size = sizeof(param.user_param); return true;
    case 5: *field = &param.mtime; *size = sizeof(param.mtime); return true;
    default: return false;
    }
}

HLE_IMPL(int, sceSaveDataGetParam, const MountPoint* point, u32 param_type, void* buffer, u64 buffer_size,
         u64* got_size) {
    std::lock_guard lock(g_mutex);
    const std::filesystem::path dir = MountedDir(point);
    HLE_TRACE("\"%s\" type=%u", point ? point->data : "", param_type);
    if (dir.empty()) {
        return kErrorNotMounted;
    }
    Param param = LoadParam(dir);
    void* field;
    u64 size;
    if (!buffer || !ParamField(param, param_type, &field, &size) || buffer_size < size) {
        return kErrorParameter;
    }
    std::memcpy(buffer, field, size);
    if (got_size) {
        *got_size = size;
    }
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataGetParam);

HLE_IMPL(int, sceSaveDataSetParam, const MountPoint* point, u32 param_type, const void* buffer, u64 buffer_size) {
    std::lock_guard lock(g_mutex);
    const std::filesystem::path dir = MountedDir(point);
    if (dir.empty()) {
        return kErrorNotMounted;
    }
    Param param = LoadParam(dir);
    void* field;
    u64 size;
    if (!buffer || !ParamField(param, param_type, &field, &size) || buffer_size > size) {
        return kErrorParameter;
    }
    std::memcpy(field, buffer, buffer_size);
    param.mtime = std::time(nullptr);
    param.title[sizeof(param.title) - 1] = param.sub_title[sizeof(param.sub_title) - 1] = 0;
    HLE_TRACE("\"%s\" type=%u title=\"%s\" subtitle=\"%s\"", point->data, param_type, param.title,
              param.sub_title);
    return StoreParam(dir, param) ? err::OK : kErrorInternal;
}
HLE_OVERRIDE(sceSaveDataSetParam);

HLE_IMPL(int, sceSaveDataSaveIcon, const MountPoint* point, const Icon* icon) {
    std::lock_guard lock(g_mutex);
    const std::filesystem::path dir = MountedDir(point);
    HLE_TRACE("\"%s\" %" PRIu64 " bytes", point ? point->data : "", icon ? icon->data_size : 0);
    if (dir.empty()) {
        return kErrorNotMounted;
    }
    if (!icon || !icon->buf || icon->data_size > icon->buf_size) {
        return kErrorParameter;
    }
    std::error_code ec;
    std::filesystem::create_directories(dir / "sce_sys", ec);
    std::ofstream out(dir / "sce_sys" / "icon0.png", std::ios::binary | std::ios::trunc);
    out.write(static_cast<const char*>(icon->buf), static_cast<std::streamsize>(icon->data_size));
    log::Tagged("GAME STATE", "SAVE icon written: %s/sce_sys/icon0.png (%" PRIu64 " bytes)", point->data,
                icon->data_size);
    return out.good() ? err::OK : kErrorInternal;
}
HLE_OVERRIDE(sceSaveDataSaveIcon);

HLE_IMPL(int, sceSaveDataDelete, const Delete* del) {
    int error = err::OK;
    const char* name = (del && ValidDirName(del->dir_name)) ? del->dir_name->data : "";
    {
        std::lock_guard lock(g_mutex);
        const std::filesystem::path dir = SavesRoot() / name;
        std::error_code ec;
        if (!g_initialized) {
            error = kErrorNotInitialized;
        } else if (!*name) {
            error = kErrorParameter;
        } else if (!std::filesystem::is_directory(dir, ec)) {
            error = kErrorNotFound;
        } else {
            for (const auto& [mounted_point, mounted_name] : g_mounted) {
                if (mounted_name == name) {
                    error = kErrorBusy;
                }
            }
            if (!error) {
                std::filesystem::remove_all(dir, ec);
                error = ec ? kErrorInternal : err::OK;
            }
        }
    }
    HLE_TRACE("\"%s\" -> 0x%x", name, error);
    return error;
}
HLE_OVERRIDE(sceSaveDataDelete);

// ================================ dialogos ==================================
constexpr int kStatusNone = 0;
constexpr int kStatusInitialized = 1;
constexpr int kStatusRunning = 2;
constexpr int kStatusFinished = 3;
constexpr int kDialogErrorNotInitialized = static_cast<int>(0x80B80003);
constexpr int kDialogErrorArgNull = static_cast<int>(0x80B8000D);

bool ReadableString(const char* s) {
    MEMORY_BASIC_INFORMATION mbi;
    return s && VirtualQuery(s, &mbi, sizeof(mbi)) && mbi.State == MEM_COMMIT &&
           !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD));
}

HLE_IMPL(int, sceCommonDialogInitialize) {
    HLE_TRACE("");
    return err::OK;
}
HLE_OVERRIDE(sceCommonDialogInitialize);

// ---- MsgDialog --------------------------------------------------------------
struct MsgDialogParam {
    u8 base_param[0x30];
    u64 size;
    s32 mode; // 1 = mensagem do usuario, 2 = barra de progresso, 3 = mensagem do sistema
    s32 pad;
    const struct MsgDialogUserMessage* user_msg;
};

struct MsgDialogUserMessage {
    s32 button_type;
    s32 pad;
    const char* msg;
};

int g_msg_status = kStatusNone;

HLE_IMPL(int, sceMsgDialogInitialize) {
    HLE_TRACE("");
    g_msg_status = kStatusInitialized;
    return err::OK;
}
HLE_OVERRIDE(sceMsgDialogInitialize);

HLE_IMPL(int, sceMsgDialogOpen, const MsgDialogParam* param) {
    if (g_msg_status == kStatusNone) {
        return kDialogErrorNotInitialized;
    }
    if (!param) {
        return kDialogErrorArgNull;
    }
    const char* text = (param->mode == 1 && param->user_msg && ReadableString(param->user_msg->msg))
                           ? param->user_msg->msg
                           : "";
    HLE_TRACE("mode=%d \"%.300s\" -> closed automatically with OK", param->mode, text);
    log::Tagged("GAME STATE", "message dialog (mode %d): \"%.300s\"", param->mode, text);
    g_msg_status = kStatusFinished;
    return err::OK;
}
HLE_OVERRIDE(sceMsgDialogOpen);

HLE_IMPL(int, sceMsgDialogUpdateStatus) {
    HLE_QUIET("-> %d", g_msg_status);
    return g_msg_status;
}
HLE_OVERRIDE(sceMsgDialogUpdateStatus);

HLE_IMPL(int, sceMsgDialogTerminate) {
    HLE_TRACE("");
    g_msg_status = kStatusNone;
    return err::OK;
}
HLE_OVERRIDE(sceMsgDialogTerminate);

// ---- SaveDataDialog ---------------------------------------------------------
struct SaveDataDialogNewItem {
    const char* title;
    void* icon_buf;
    u64 icon_size;
    u8 reserved[32];
};

struct SaveDataDialogItems {
    s32 user_id;
    s32 pad;
    const void* title_id;
    const DirName* dir_names;
    u32 dir_names_num;
    s32 pad2;
    const SaveDataDialogNewItem* new_item;
    u32 focus_pos; // 0 = inicio da lista, 6 = o dirName de focus_dir_name
    s32 pad3;
    const DirName* focus_dir_name;
    u32 item_style;
    u8 reserved[32];
};

struct SaveDataDialogParam {
    u8 base_param[0x30];
    s32 size;
    s32 mode;      // 1 = lista, 2 = mensagem do usuario, 3 = do sistema, 4 = erro, 5 = progresso
    s32 disp_type; // 1 = salvar, 2 = carregar, 3 = apagar
    s32 pad;
    void* anim_param;
    const SaveDataDialogItems* items;
    void* user_msg_param;
    void* sys_msg_param;
    void* error_code_param;
    void* progress_bar_param;
    void* user_data;
    void* option_param;
    u8 reserved[24];
};

struct SaveDataDialogResult {
    s32 mode;
    s32 result;
    s32 button_id;
    s32 pad;
    DirName* dir_name;
    Param* param;
    void* user_data;
    u8 reserved[32];
};

std::atomic<int> g_save_dialog_status{kStatusNone};
s32 g_save_dialog_mode = 0;

// ---- Fase 31: a lista de saves do sistema (sceSaveDataDialog, modo lista) como janela do port ----
// O PS4 mostra a lista de saves e devolve o dirName escolhido; ate a Fase 30 o port respondia OK
// sem tocar no dirName, e o jogo usava sempre o primeiro (Slot00). Controle: direcional escolhe,
// X confirma, O volta. Teclado: setas, Enter, Esc. Mouse: clique escolhe, duplo clique confirma.
struct DialogEntry {
    std::string dir;       // "" = item "novo salvamento"
    std::wstring line1, line2;
};
struct DialogUi {
    std::wstring title;
    std::vector<DialogEntry> entries;
    int selected = 0;
    bool finished = false, canceled = false;
    u32 last_buttons = ~0u; // botoes do controle na leitura anterior (~0 = ainda nao leu)
    int confirm_armed = 0;  // 1 = X apertado, 2 = O apertado: fecha quando o botao soltar
};
bool g_save_ui_enabled = true; // --no-save-ui
std::string g_dialog_choice;
bool g_dialog_canceled = false;
void* g_dialog_user_data = nullptr;
bool g_dialog_was_list = false; // a janela foi mostrada nesta abertura
s32 g_dialog_disp_type = 0;      // 1 = salvar, 2 = carregar, 3 = apagar
DialogUi* g_dialog_ui = nullptr; // so a thread do dialogo mexe

constexpr int kRowHeight = 74, kHeaderHeight = 70, kFooterHeight = 46, kDialogWidth = 760;

std::wstring Widen(const std::string& text) {
    if (text.empty()) {
        return {};
    }
    const int size = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring wide(static_cast<size_t>(std::max(size, 0)), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wide.data(), size);
    return wide;
}

// Data da ultima gravacao dentro da pasta do save (o arquivo mais recente).
std::wstring LastWrite(const std::filesystem::path& dir) {
    std::error_code ec;
    std::filesystem::file_time_type latest{};
    bool any = false;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir, ec)) {
        if (entry.is_regular_file(ec)) {
            const auto time = entry.last_write_time(ec);
            if (!any || time > latest) {
                latest = time;
                any = true;
            }
        }
    }
    if (!any) {
        return lang::T(L"vazio", L"empty");
    }
    const auto system = std::chrono::time_point_cast<std::chrono::system_clock::duration>(latest - std::filesystem::file_time_type::clock::now() +
                                                                                           std::chrono::system_clock::now());
    const std::time_t seconds = std::chrono::system_clock::to_time_t(system);
    std::tm local{};
    localtime_s(&local, &seconds);
    wchar_t text[64];
    std::wcsftime(text, 64, lang::T(L"%d/%m/%Y %H:%M", L"%Y-%m-%d %H:%M"), &local);
    return text;
}

void DialogFinish(HWND hwnd, bool canceled) {
    DialogUi& ui = *g_dialog_ui;
    ui.finished = true;
    ui.canceled = canceled;
    DestroyWindow(hwnd);
}

LRESULT CALLBACK SaveDialogProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    DialogUi* ui = g_dialog_ui;
    if (!ui) {
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
    const int count = static_cast<int>(ui->entries.size());
    auto move = [&](int delta) {
        if (count) {
            ui->selected = (ui->selected + delta + count) % count;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
    };
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT client;
        GetClientRect(hwnd, &client);
        HBRUSH background = CreateSolidBrush(RGB(18, 22, 34));
        FillRect(dc, &client, background);
        DeleteObject(background);
        SetBkMode(dc, TRANSPARENT);
        HFONT big = CreateFontW(30, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        HFONT normal = CreateFontW(22, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        HFONT small = CreateFontW(17, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        HGDIOBJ old = SelectObject(dc, big);
        SetTextColor(dc, RGB(255, 255, 255));
        RECT header{24, 16, client.right - 24, kHeaderHeight};
        DrawTextW(dc, ui->title.c_str(), -1, &header, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        for (int i = 0; i < count; ++i) {
            RECT row{16, kHeaderHeight + i * kRowHeight, client.right - 16, kHeaderHeight + (i + 1) * kRowHeight - 8};
            HBRUSH fill = CreateSolidBrush(i == ui->selected ? RGB(0, 112, 204) : RGB(34, 40, 58));
            FillRect(dc, &row, fill);
            DeleteObject(fill);
            SelectObject(dc, normal);
            SetTextColor(dc, RGB(255, 255, 255));
            RECT line1{row.left + 16, row.top + 8, row.right - 16, row.top + 36};
            DrawTextW(dc, ui->entries[i].line1.c_str(), -1, &line1, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
            SelectObject(dc, small);
            SetTextColor(dc, i == ui->selected ? RGB(225, 238, 255) : RGB(160, 170, 190));
            RECT line2{row.left + 16, row.top + 38, row.right - 16, row.bottom - 4};
            DrawTextW(dc, ui->entries[i].line2.c_str(), -1, &line2, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        if (!count) {
            SelectObject(dc, normal);
            SetTextColor(dc, RGB(200, 205, 220));
            RECT empty{24, kHeaderHeight, client.right - 24, kHeaderHeight + kRowHeight};
            DrawTextW(dc, lang::T(L"Nenhum salvamento encontrado.", L"No saves found."), -1, &empty, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        }
        SelectObject(dc, small);
        SetTextColor(dc, RGB(160, 170, 190));
        RECT footer{24, client.bottom - kFooterHeight, client.right - 24, client.bottom - 8};
        DrawTextW(dc, lang::T(L"Direcional / setas: escolher      X / Enter: confirmar      O / Esc: voltar",
                          L"D-pad / arrows: choose      X / Enter: confirm      O / Esc: back"), -1, &footer,
                  DT_LEFT | DT_SINGLELINE | DT_VCENTER);
        SelectObject(dc, old);
        DeleteObject(big);
        DeleteObject(normal);
        DeleteObject(small);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_KEYDOWN:
        if (wparam == VK_UP) {
            move(-1);
        } else if (wparam == VK_DOWN) {
            move(1);
        } else if ((wparam == VK_RETURN || wparam == VK_SPACE) && count) {
            DialogFinish(hwnd, false);
        } else if (wparam == VK_ESCAPE) {
            DialogFinish(hwnd, true);
        }
        return 0;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK: {
        const int row = (static_cast<int>(static_cast<short>(HIWORD(lparam))) - kHeaderHeight) / kRowHeight;
        if (static_cast<int>(static_cast<short>(HIWORD(lparam))) >= kHeaderHeight && row >= 0 && row < count) {
            ui->selected = row;
            InvalidateRect(hwnd, nullptr, FALSE);
            if (msg == WM_LBUTTONDBLCLK) {
                DialogFinish(hwnd, false);
            }
        }
        return 0;
    }
    case WM_TIMER: {
        padsdl::State pad;
        if (!padsdl::Read(&pad)) {
            return 0;
        }
        // mascaras do Orbis: cima 0x10, baixo 0x40, O 0x2000, X 0x4000
        const u32 pressed = ui->last_buttons == ~0u ? 0 : pad.buttons & ~ui->last_buttons;
        ui->last_buttons = pad.buttons;
        if (ui->confirm_armed) { // fecha quando o botao soltar, para o jogo nao ver o mesmo aperto
            if (!(pad.buttons & (ui->confirm_armed == 1 ? 0x4000u : 0x2000u))) {
                DialogFinish(hwnd, ui->confirm_armed == 2);
            }
            return 0;
        }
        if (pressed & 0x10) {
            move(-1);
        } else if (pressed & 0x40) {
            move(1);
        } else if ((pressed & 0x4000) && count) {
            ui->confirm_armed = 1;
        } else if (pressed & 0x2000) {
            ui->confirm_armed = 2;
        }
        return 0;
    }
    case WM_CLOSE:
        DialogFinish(hwnd, true);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
}

void SaveDialogThread(DialogUi ui) {
    g_dialog_ui = &ui;
    static bool registered = false;
    if (!registered) {
        registered = true;
        WNDCLASSW wc{};
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = SaveDialogProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"LegoDimensionsPcSaveDialog";
        wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)); // IDC_ARROW
        RegisterClassW(&wc);
    }
    // Testes automaticos: LD_SAVE_UI_AUTO=<dirName> escolhe esse item sem abrir a janela.
    const char* automatic = std::getenv("LD_SAVE_UI_AUTO");
    const HWND owner = static_cast<HWND>(videoout::WindowHandle());
    const int rows = std::max<int>(1, static_cast<int>(ui.entries.size()));
    const int width = kDialogWidth, height = kHeaderHeight + rows * kRowHeight + kFooterHeight;
    RECT area{0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
    if (owner) {
        POINT origin{0, 0};
        RECT client;
        GetClientRect(owner, &client);
        ClientToScreen(owner, &origin);
        area = {origin.x, origin.y, origin.x + client.right, origin.y + client.bottom};
    }
    const HWND hwnd = CreateWindowExW(WS_EX_TOPMOST, L"LegoDimensionsPcSaveDialog", lang::T(L"Salvamentos", L"Saves"), WS_POPUP | WS_BORDER,
                                      area.left + (area.right - area.left - width) / 2, area.top + (area.bottom - area.top - height) / 2,
                                      width, height, owner, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (automatic) {
        for (size_t i = 0; i < ui.entries.size(); ++i) {
            if (ui.entries[i].dir == automatic) {
                ui.selected = static_cast<int>(i);
            }
        }
        ui.finished = true;
        if (hwnd) {
            DestroyWindow(hwnd);
        }
    } else if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
        SetTimer(hwnd, 1, 30, nullptr);
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        ui.finished = true; // sem janela: responde como antes
    }
    g_dialog_ui = nullptr;
    {
        std::lock_guard lock(g_mutex);
        g_dialog_canceled = ui.canceled || ui.entries.empty();
        g_dialog_choice = g_dialog_canceled ? std::string() : ui.entries[static_cast<size_t>(ui.selected)].dir;
        // Fase 32, "Novo Jogo": o jogo NAO usa o dirName devolvido - ele mesmo decide onde gravar (o
        // primeiro slot livre; com todos ocupados, o primeiro da lista). Para a escolha do jogador
        // valer, as pastas sao reorganizadas antes de o jogo continuar: o save escolhido e MOVIDO
        // para uma pasta de backup e, se nao era o primeiro, o primeiro passa a ocupar o nome dele.
        if (g_dialog_disp_type == 1 && !g_dialog_canceled && !g_dialog_choice.empty()) {
            std::string first;
            bool has_new = false;
            for (const DialogEntry& entry : ui.entries) {
                has_new |= entry.dir.empty();
                if (first.empty() && !entry.dir.empty()) {
                    first = entry.dir;
                }
            }
            std::time_t now = std::time(nullptr);
            std::tm local{};
            localtime_s(&local, &now);
            char stamp[40];
            std::strftime(stamp, sizeof(stamp), "%Y%m%d_%H%M%S", &local);
            const std::filesystem::path backup = fs::WritableRoot() / (std::string("savedata_backup_novojogo_") + stamp);
            std::error_code ec;
            std::filesystem::create_directories(backup, ec);
            std::filesystem::rename(SavesRoot() / g_dialog_choice, backup / g_dialog_choice, ec);
            if (ec) {
                log::Tagged("GAME STATE", "New Game: could not move %s to backup (%s); nothing changed", g_dialog_choice.c_str(),
                            ec.message().c_str());
                g_dialog_canceled = true;
            } else {
                log::Tagged("GAME STATE", "New Game: save %s MOVED to %s", g_dialog_choice.c_str(), (backup / g_dialog_choice).string().c_str());
                if (!has_new && first != g_dialog_choice) {
                    std::filesystem::rename(SavesRoot() / first, SavesRoot() / g_dialog_choice, ec);
                    log::Tagged("GAME STATE", "New Game: save %s renamed to %s (%s); new game goes in %s", first.c_str(),
                                g_dialog_choice.c_str(), ec ? ec.message().c_str() : "ok", first.c_str());
                }
            }
        }
    }
    log::Tagged("GAME STATE", "save list: %s", g_dialog_canceled ? "cancelled by player"
                                                     : g_dialog_choice.empty() ? "NEW save selected"
                                                                               : ("selected " + g_dialog_choice).c_str());
    if (owner) {
        SetForegroundWindow(owner);
    }
    g_save_dialog_status = kStatusFinished;
}

// Monta a lista a partir do pedido do jogo. false = nada a mostrar (o chamador responde OK).
bool BuildSaveList(const SaveDataDialogParam* param, DialogUi* ui) {
    const SaveDataDialogItems* items = param->items;
    if (!items) {
        return false;
    }
    ui->title = param->disp_type == 2   ? lang::T(L"Carregar jogo", L"Load game")
                : param->disp_type == 3 ? lang::T(L"Apagar salvamento", L"Delete save")
                                        : lang::T(L"Salvar jogo \u2014 escolha o salvamento", L"Save game \u2014 choose a save");
    std::string focus;
    if (items->focus_pos == 6 && items->focus_dir_name && std::memchr(items->focus_dir_name->data, 0, sizeof(DirName::data))) {
        focus = items->focus_dir_name->data;
    }
    for (u32 i = 0; items->dir_names && i < items->dir_names_num && i < 64; ++i) {
        if (!ValidDirName(&items->dir_names[i])) {
            continue;
        }
        const std::string name = items->dir_names[i].data;
        const std::filesystem::path dir = SavesRoot() / name;
        std::error_code ec;
        if (!std::filesystem::is_directory(dir, ec)) {
            continue; // o sistema so lista os saves que existem
        }
        const Param saved = LoadParam(dir);
        const std::string title(saved.title, strnlen(saved.title, sizeof(saved.title)));
        const std::string sub(saved.sub_title, strnlen(saved.sub_title, sizeof(saved.sub_title)));
        DialogEntry entry;
        entry.dir = name;
        entry.line1 = Widen(name) + (title.empty() ? L"" : L"  \u2014  " + Widen(title));
        entry.line2 = (sub.empty() ? L"" : Widen(sub) + L"   \u00B7   ") + lang::T(L"\u00DAltima grava\u00E7\u00E3o: ", L"Last saved: ") + LastWrite(dir);
        if (param->disp_type == 1) {
            entry.line2 += lang::T(L"   \u00B7   Novo jogo no lugar deste (ele vai para uma pasta de backup)",
                                   L"   \u00B7   New game in place of this one (it is moved to a backup folder)");
        }
        if (name == focus) {
            ui->selected = static_cast<int>(ui->entries.size());
        }
        ui->entries.push_back(std::move(entry));
    }
    if (items->new_item) {
        DialogEntry entry;
        const char* title = items->new_item->title;
        entry.line1 = title && *title ? Widen(title) : lang::T(L"Novo salvamento", L"New save");
        entry.line2 = lang::T(L"Cria um salvamento novo, sem apagar os outros", L"Creates a new save without deleting the others");
        if (items->focus_pos == 0 || ui->entries.empty()) {
            ui->entries.insert(ui->entries.begin(), std::move(entry)); // como no console: o item novo vem primeiro
            ui->selected = items->focus_pos == 0 ? 0 : ui->selected + 1;
        } else {
            ui->entries.insert(ui->entries.begin(), std::move(entry));
            ++ui->selected;
        }
    }
    return true;
}

HLE_IMPL(int, sceSaveDataDialogInitialize) {
    HLE_TRACE("");
    g_save_dialog_status = kStatusInitialized;
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataDialogInitialize);

HLE_IMPL(int, sceSaveDataDialogOpen, const SaveDataDialogParam* param) {
    if (g_save_dialog_status == kStatusNone) {
        return kDialogErrorNotInitialized;
    }
    if (!param) {
        return kDialogErrorArgNull;
    }
    g_save_dialog_mode = param->mode;
    {
        std::lock_guard lock(g_mutex);
        g_dialog_choice.clear();
        g_dialog_canceled = false;
        g_dialog_was_list = false;
        g_dialog_disp_type = param->disp_type;
        g_dialog_user_data = param->user_data;
    }
    DialogUi ui;
    if (param->mode == 1 && g_save_ui_enabled && videoout::WindowHandle() && BuildSaveList(param, &ui)) {
        HLE_TRACE("mode=1 (list) type=%d: %zu item(s)%s -> selection window opened", param->disp_type, ui.entries.size(),
                  param->items->new_item ? ", with \"new save\"" : "");
        g_save_dialog_status = kStatusRunning;
        g_dialog_was_list = true;
        std::thread(SaveDialogThread, std::move(ui)).detach();
        return err::OK;
    }
    const u32 system_message = param->mode == 3 && param->sys_msg_param ? *static_cast<const u32*>(param->sys_msg_param) : 0;
    HLE_TRACE("mode=%d type=%d (system message %u) -> closed automatically with OK", param->mode, param->disp_type, system_message);
    log::Tagged("GAME STATE", "save dialog (mode %d, type %d) answered with OK", param->mode, param->disp_type);
    g_save_dialog_status = kStatusFinished;
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataDialogOpen);

HLE_IMPL(int, sceSaveDataDialogUpdateStatus) {
    HLE_QUIET("-> %d", g_save_dialog_status.load());
    return g_save_dialog_status;
}
HLE_OVERRIDE(sceSaveDataDialogUpdateStatus);

HLE_IMPL(int, sceSaveDataDialogGetResult, SaveDataDialogResult* result) {
    HLE_TRACE("status=%d", g_save_dialog_status.load());
    if (g_save_dialog_status != kStatusFinished) {
        return kDialogErrorNotInitialized;
    }
    if (!result) {
        return kDialogErrorArgNull;
    }
    std::lock_guard lock(g_mutex);
    result->mode = g_save_dialog_mode;
    result->result = g_dialog_canceled ? 1 : 0; // 0 = OK, 1 = cancelado pelo usuario
    result->button_id = g_dialog_canceled ? 0 : 1; // OK / SIM
    result->user_data = g_dialog_user_data;
    // Lista (Fase 31): o dirName escolhido vai para o buffer do chamador ("" = novo salvamento). Nos
    // outros modos os buffers ficam como estao.
    // Na lista de "salvar" (Novo Jogo) o dirName fica como o jogo deixou: ele decide o slot sozinho
    // (ver SaveDialogThread).
    if (g_save_dialog_mode == 1 && g_save_ui_enabled && !g_dialog_canceled && result->dir_name && g_dialog_disp_type != 1 &&
        (!g_dialog_choice.empty() || g_dialog_was_list)) {
        std::memset(result->dir_name, 0, sizeof(DirName));
        std::memcpy(result->dir_name->data, g_dialog_choice.data(), std::min(g_dialog_choice.size(), sizeof(DirName::data) - 1));
        if (result->param && !g_dialog_choice.empty()) {
            *result->param = LoadParam(SavesRoot() / g_dialog_choice);
        }
        g_overwrite_slot.clear();
    }
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataDialogGetResult);

HLE_IMPL(int, sceSaveDataDialogProgressBarSetValue, int target, u32 rate) {
    HLE_QUIET("%u%%", rate);
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataDialogProgressBarSetValue);

HLE_IMPL(int, sceSaveDataDialogClose) {
    HLE_TRACE("");
    if (g_save_dialog_status != kStatusNone && g_save_dialog_status != kStatusRunning) {
        g_save_dialog_status = kStatusFinished;
    }
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataDialogClose);

HLE_IMPL(int, sceSaveDataDialogTerminate) {
    HLE_TRACE("");
    g_save_dialog_status = kStatusNone;
    return err::OK;
}
HLE_OVERRIDE(sceSaveDataDialogTerminate);

} // namespace

namespace hle {
void SetSavesDirName(const char* name) {
    g_saves_dir_name = name;
}

void SetSaveUi(bool enabled) {
    g_save_ui_enabled = enabled;
}

// --test-save-ui: abre a lista de saves sem o jogo (os slots que existem + "novo salvamento") e
// imprime a escolha. Serve para conferir a janela.
int TestSaveUi() {
    static DirName names[4];
    for (int i = 0; i < 4; ++i) {
        std::snprintf(names[i].data, sizeof(names[i].data), "Slot%02d", i);
    }
    static SaveDataDialogNewItem new_item{};
    static SaveDataDialogItems items{};
    items.dir_names = names;
    items.dir_names_num = 4;
    items.new_item = &new_item;
    SaveDataDialogParam param{};
    param.mode = 1;
    param.disp_type = 1;
    param.items = &items;
    DialogUi ui;
    BuildSaveList(&param, &ui);
    std::printf("save list: %zu item(s)\n", ui.entries.size());
    std::fflush(stdout);
    g_save_dialog_status = kStatusRunning;
    std::thread(SaveDialogThread, std::move(ui)).join();
    std::printf("result: %s\n", g_dialog_canceled ? "cancelled" : g_dialog_choice.empty() ? "new save" : g_dialog_choice.c_str());
    return 0;
}
} // namespace hle
