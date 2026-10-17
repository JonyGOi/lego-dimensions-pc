// libSceUserService, libSceSystemService e libSceAppContent: um unico usuario logado (id 1).
// O controle fica em hle_pad.cpp.
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "hle/overrides/hle_filesystem.h"
#include "hle/overrides/hle_override.h"
#include "runtime/lang.h"

namespace {

using namespace hle;

constexpr int kUserId = 1;
constexpr int kUserIdInvalid = -1;
constexpr char kUserName[] = "Player1";

constexpr int kUserServiceErrorInvalidArgument = static_cast<int>(0x80960005);
constexpr int kUserServiceErrorNoEvent = static_cast<int>(0x80960007);
constexpr int kUserServiceErrorBufferTooShort = static_cast<int>(0x8096000A);
constexpr int kSystemServiceErrorParameter = static_cast<int>(0x80A10003);
constexpr int kSystemServiceErrorNoEvent = static_cast<int>(0x80A10004);
constexpr int kAppContentErrorParameter = static_cast<int>(0x80D90002);
constexpr int kAppContentErrorNotFound = static_cast<int>(0x80D90005);
constexpr int kAppContentErrorNoEntitlement = static_cast<int>(0x80D90007); // DRM_NO_ENTITLEMENT
constexpr u32 kSystemServiceEventEntitlementUpdate = 0x10000003;

// ============================== UserService =================================
struct UserServiceEvent {
    int event_type; // 0 = login, 1 = logout
    int user_id;
};

struct LoginUserIdList {
    int user_id[4];
};

std::atomic<bool> g_login_event_pending{true};

HLE_IMPL(int, sceUserServiceInitialize, const void* params) {
    HLE_TRACE("%p", params);
    return err::OK;
}
HLE_OVERRIDE(sceUserServiceInitialize);

HLE_IMPL(int, sceUserServiceTerminate) {
    HLE_TRACE("");
    return err::OK;
}
HLE_OVERRIDE(sceUserServiceTerminate);

HLE_IMPL(int, sceUserServiceGetInitialUser, int* user_id) {
    HLE_TRACE("-> %d", kUserId);
    if (!user_id) {
        return kUserServiceErrorInvalidArgument;
    }
    *user_id = kUserId;
    return err::OK;
}
HLE_OVERRIDE(sceUserServiceGetInitialUser);

HLE_IMPL(int, sceUserServiceGetLoginUserIdList, LoginUserIdList* list) {
    HLE_QUIET("%p", static_cast<void*>(list));
    if (!list) {
        return kUserServiceErrorInvalidArgument;
    }
    list->user_id[0] = kUserId;
    list->user_id[1] = list->user_id[2] = list->user_id[3] = kUserIdInvalid;
    return err::OK;
}
HLE_OVERRIDE(sceUserServiceGetLoginUserIdList);

HLE_IMPL(int, sceUserServiceGetUserName, int user_id, char* name, u64 size) {
    HLE_TRACE("user=%d -> \"%s\"", user_id, kUserName);
    if (!name || user_id != kUserId) {
        return kUserServiceErrorInvalidArgument;
    }
    if (size < sizeof(kUserName)) {
        return kUserServiceErrorBufferTooShort;
    }
    std::memcpy(name, kUserName, sizeof(kUserName));
    return err::OK;
}
HLE_OVERRIDE(sceUserServiceGetUserName);

HLE_IMPL(int, sceUserServiceGetUserColor, int user_id, int* color) {
    HLE_TRACE("user=%d -> azul", user_id);
    if (!color || user_id != kUserId) {
        return kUserServiceErrorInvalidArgument;
    }
    *color = 0; // azul, o primeiro jogador
    return err::OK;
}
HLE_OVERRIDE(sceUserServiceGetUserColor);

// Um unico evento de login logo apos o boot; depois, fila vazia para sempre.
HLE_IMPL(int, sceUserServiceGetEvent, UserServiceEvent* event) {
    if (!event) {
        return kUserServiceErrorInvalidArgument;
    }
    if (g_login_event_pending.exchange(false)) {
        event->event_type = 0;
        event->user_id = kUserId;
        HLE_TRACE("-> LOGIN do usuario %d", kUserId);
        return err::OK;
    }
    HLE_QUIET("-> sem evento");
    return kUserServiceErrorNoEvent;
}
HLE_OVERRIDE(sceUserServiceGetEvent);

// ============================== SystemService ===============================
struct SystemServiceStatus {
    s32 event_num;
    bool is_system_ui_overlaid;
    bool is_in_background_execution;
    bool is_cpu_mode7_cpu_normal;
    bool is_game_live_streaming_on_air;
    bool is_out_of_vr_play_area;
    u8 reserved[125];
};

struct DisplaySafeAreaInfo {
    float ratio;
    u8 reserved[128];
};

HLE_IMPL(int, sceSystemServiceParamGetInt, int param_id, int* value) {
    if (!value) {
        return kSystemServiceErrorParameter;
    }
    switch (param_id) {
    case 1: *value = lang::Ps4SystemLanguage(); break; // Fase 60: idioma do Windows (17 = portugues do Brasil, 1 = ingles)
    case 2: *value = 1; break;    // formato de data DD/MM/AAAA
    case 3: *value = 1; break;    // relogio de 24 horas
    case 4: *value = -180; break; // fuso em minutos (UTC-3)
    case 5: *value = 0; break;    // sem horario de verao
    case 7: *value = 0; break;    // controle parental desligado
    case 1000: *value = 1; break; // confirmar com X
    default:
        HLE_TRACE("id=%d -> parametro desconhecido", param_id);
        return kSystemServiceErrorParameter;
    }
    HLE_TRACE("id=%d -> %d", param_id, *value);
    return err::OK;
}
HLE_OVERRIDE(sceSystemServiceParamGetInt);

HLE_IMPL(int, sceSystemServiceGetDisplaySafeAreaInfo, DisplaySafeAreaInfo* info) {
    HLE_TRACE("-> 1.0");
    if (!info) {
        return kSystemServiceErrorParameter;
    }
    std::memset(info, 0, sizeof(*info));
    info->ratio = 1.0f;
    return err::OK;
}
HLE_OVERRIDE(sceSystemServiceGetDisplaySafeAreaInfo);

HLE_IMPL(int, sceSystemServiceHideSplashScreen) {
    HLE_TRACE("");
    return err::OK;
}
HLE_OVERRIDE(sceSystemServiceHideSplashScreen);

HLE_IMPL(int, sceSystemServiceGetStatus, SystemServiceStatus* status) {
    HLE_QUIET("%p", static_cast<void*>(status));
    if (!status) {
        return kSystemServiceErrorParameter;
    }
    // So os campos documentados: o tamanho real da area reservada nao e conhecido.
    status->event_num = 0;
    status->is_system_ui_overlaid = false;
    status->is_in_background_execution = false;
    status->is_cpu_mode7_cpu_normal = true;
    status->is_game_live_streaming_on_air = false;
    status->is_out_of_vr_play_area = false;
    return err::OK;
}
HLE_OVERRIDE(sceSystemServiceGetStatus);

// Fase 46: com DLC instalado, um evento ENTITLEMENT_UPDATE depois do sceAppContentInitialize
// (o shadPS4 faz o mesmo); fora isso, fila vazia.
std::atomic<bool> g_entitlement_event_pending{false};

HLE_IMPL(int, sceSystemServiceReceiveEvent, void* event) {
    HLE_QUIET("%p", event);
    if (!event) {
        return kSystemServiceErrorParameter;
    }
    if (g_entitlement_event_pending.exchange(false)) {
        std::memset(event, 0, 4 + 8192); // tipo + uniao de 8192 bytes
        u32* words = static_cast<u32*>(event);
        words[0] = kSystemServiceEventEntitlementUpdate;
        words[1] = kUserId; // userId
        words[2] = 0;       // np_service_label
        HLE_TRACE("-> ENTITLEMENT_UPDATE (DLC instalado)");
        return err::OK;
    }
    return kSystemServiceErrorNoEvent;
}
HLE_OVERRIDE(sceSystemServiceReceiveEvent);

// =============================== AppContent =================================
struct AppContentMountPoint {
    char data[16];
};

// (sceAppContentInitialize fica depois das funcoes de DLC, Fase 46)

HLE_IMPL(int, sceAppContentAppParamGetInt, int param_id, s32* value) {
    if (!value || param_id < 0 || param_id > 4) {
        return kAppContentErrorParameter;
    }
    // 0 = SKU_FLAG (3 = jogo completo); 1..4 = USER_DEFINED_PARAM_n do param.sfo, lidos como 0.
    *value = param_id == 0 ? 3 : 0;
    HLE_TRACE("id=%d -> %d", param_id, *value);
    return err::OK;
}
HLE_OVERRIDE(sceAppContentAppParamGetInt);

// ---- Fase 46: conteudo adicional (DLC) ----
// Cada pasta do diretorio de addcont com sce_sys/param.sfo de CATEGORY "ac" e um DLC; o rotulo
// (entitlement label) sao os 16 caracteres do CONTENT_ID depois do caractere 20
// ("EP1018-CUSA01176_00-OPUSBATMANSTORY1" -> "OPUSBATMANSTORY1"). Mesmo esquema do shadPS4, e a
// mesma pasta: %APPDATA%\shadPS4\addcont\CUSA01176 (os pacotes sao lidos de la, sem copia).
struct EntitlementLabel {
    char data[17];
    char padding[3];
};
struct AddcontInfo {
    EntitlementLabel label;
    u32 status; // 4 = instalado
};
static_assert(sizeof(AddcontInfo) == 24);

struct Addcont {
    std::string label;
    std::filesystem::path folder;
};
std::mutex g_addcont_mutex;
std::filesystem::path g_addcont_dir;
std::vector<Addcont> g_addcont;

// Campo de texto de um param.sfo (formato PSF: cabecalho, tabela de entradas, chaves, dados).
std::optional<std::string> ReadSfoString(const std::filesystem::path& path, const char* wanted) {
    std::ifstream in(path, std::ios::binary);
    const std::vector<char> b((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (b.size() < 20 || std::memcmp(b.data(), "\0PSF", 4) != 0) {
        return std::nullopt;
    }
    auto u32_at = [&](size_t at) {
        u32 v = 0;
        std::memcpy(&v, b.data() + at, 4);
        return v;
    };
    const u32 keys = u32_at(8), data = u32_at(12), count = u32_at(16);
    for (u32 i = 0; i < count && 20 + 16ull * (i + 1) <= b.size(); ++i) {
        const size_t entry = 20 + 16ull * i;
        u16 key_offset = 0, format = 0;
        std::memcpy(&key_offset, b.data() + entry, 2);
        std::memcpy(&format, b.data() + entry + 2, 2);
        const u32 length = u32_at(entry + 4), data_offset = u32_at(entry + 12);
        const size_t key_at = keys + key_offset, value_at = static_cast<size_t>(data) + data_offset;
        if (key_at >= b.size() || value_at + length > b.size()) {
            continue;
        }
        if (std::strncmp(b.data() + key_at, wanted, b.size() - key_at) == 0 && format != 0x0404) {
            std::string value(b.data() + value_at, length);
            value.erase(std::find(value.begin(), value.end(), '\0'), value.end());
            return value;
        }
    }
    return std::nullopt;
}

void ScanAddcont() {
    g_addcont.clear();
    std::error_code ec;
    if (g_addcont_dir.empty() || !std::filesystem::is_directory(g_addcont_dir, ec)) {
        log::Tagged("DLC", "pasta de DLC nao encontrada (%s): nenhum DLC", g_addcont_dir.string().c_str());
        return;
    }
    for (std::filesystem::directory_iterator it(g_addcont_dir, ec), end; !ec && it != end; it.increment(ec)) {
        if (!it->is_directory(ec)) {
            continue;
        }
        const auto sfo = it->path() / "sce_sys" / "param.sfo";
        const auto category = ReadSfoString(sfo, "CATEGORY");
        const auto content_id = ReadSfoString(sfo, "CONTENT_ID");
        if (!category || category->compare(0, 2, "ac") != 0 || !content_id || content_id->size() <= 20) {
            log::Tagged("DLC", "%s: sem param.sfo de conteudo adicional, ignorado", it->path().filename().string().c_str());
            continue;
        }
        const std::string label = content_id->substr(20, 16);
        g_addcont.push_back({label, it->path()});
        log::Tagged("DLC", "%s instalado [%s]", label.c_str(), it->path().string().c_str());
    }
}

int FindAddcont(const void* label) {
    if (!label) {
        return -1;
    }
    const char* text = static_cast<const char*>(label);
    for (size_t i = 0; i < g_addcont.size(); ++i) {
        if (std::strncmp(text, g_addcont[i].label.c_str(), 16) == 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

HLE_IMPL(int, sceAppContentGetAddcontInfoList, u32 service_label, AddcontInfo* list, u32 list_num, u32* hit_num) {
    std::lock_guard lock(g_addcont_mutex);
    if (!list || !list_num) {
        if (!hit_num) {
            return kAppContentErrorParameter;
        }
        *hit_num = static_cast<u32>(g_addcont.size());
        HLE_TRACE("label=%u -> %u DLCs (so a contagem)", service_label, *hit_num);
        return err::OK;
    }
    const u32 count = std::min<u32>(list_num, static_cast<u32>(g_addcont.size()));
    for (u32 i = 0; i < count; ++i) {
        std::memset(&list[i], 0, sizeof(list[i]));
        std::memcpy(list[i].label.data, g_addcont[i].label.c_str(), std::min<size_t>(16, g_addcont[i].label.size()));
        list[i].status = 4;
    }
    if (hit_num) {
        *hit_num = count;
    }
    HLE_TRACE("label=%u max=%u -> %u DLCs", service_label, list_num, count);
    return err::OK;
}
HLE_OVERRIDE(sceAppContentGetAddcontInfoList);

HLE_IMPL(int, sceAppContentGetAddcontInfo, u32 service_label, const EntitlementLabel* label, AddcontInfo* info) {
    if (!label || !info) {
        return kAppContentErrorParameter;
    }
    std::lock_guard lock(g_addcont_mutex);
    const int index = FindAddcont(label->data);
    HLE_TRACE("label=%u \"%.16s\" -> %s", service_label, label->data, index < 0 ? "SEM DIREITO" : "instalado");
    if (index < 0) {
        return kAppContentErrorNoEntitlement;
    }
    std::memset(info, 0, sizeof(*info));
    std::memcpy(info->label.data, g_addcont[index].label.c_str(), std::min<size_t>(16, g_addcont[index].label.size()));
    info->status = 4;
    return err::OK;
}
HLE_OVERRIDE(sceAppContentGetAddcontInfo);

HLE_IMPL(int, sceAppContentGetEntitlementKey, u32 service_label, const EntitlementLabel* label, u8* key) {
    if (!label || !key) {
        return kAppContentErrorParameter;
    }
    std::lock_guard lock(g_addcont_mutex);
    const int index = FindAddcont(label->data);
    HLE_TRACE("label=%u \"%.16s\" -> %s", service_label, label->data, index < 0 ? "SEM DIREITO" : "chave zerada");
    if (index < 0) {
        return kAppContentErrorNoEntitlement;
    }
    std::memset(key, 0, 16); // os pacotes extraidos ja estao decifrados
    return err::OK;
}
HLE_OVERRIDE(sceAppContentGetEntitlementKey);

HLE_IMPL(int, sceAppContentAddcontMount, u32 service_label, const EntitlementLabel* label, AppContentMountPoint* mount_point) {
    if (!label || !mount_point) {
        return kAppContentErrorParameter;
    }
    std::lock_guard lock(g_addcont_mutex);
    const int index = FindAddcont(label->data);
    if (index < 0) {
        HLE_TRACE("label=%u \"%.16s\" -> NAO ENCONTRADO", service_label, label->data);
        return kAppContentErrorNotFound;
    }
    std::memset(mount_point, 0, sizeof(*mount_point));
    std::snprintf(mount_point->data, sizeof(mount_point->data), "/addcont%d", index);
    fs::SetMount(mount_point->data, g_addcont[index].folder, true);
    HLE_TRACE("label=%u \"%.16s\" -> %s", service_label, label->data, mount_point->data);
    return err::OK;
}
HLE_OVERRIDE(sceAppContentAddcontMount);

HLE_IMPL(int, sceAppContentAddcontUnmount, const AppContentMountPoint* mount_point) {
    if (!mount_point) {
        return kAppContentErrorParameter;
    }
    HLE_TRACE("%.16s", mount_point->data);
    fs::RemoveMount(std::string(mount_point->data, strnlen(mount_point->data, sizeof(mount_point->data))));
    return err::OK;
}
HLE_OVERRIDE(sceAppContentAddcontUnmount);

HLE_IMPL(int, sceAppContentInitialize, const void* init_param, void* boot_param) {
    HLE_TRACE("%p, %p", init_param, boot_param);
    std::lock_guard lock(g_addcont_mutex);
    ScanAddcont(); // Fase 46
    if (!g_addcont.empty()) {
        g_entitlement_event_pending = true;
    }
    return err::OK;
}
HLE_OVERRIDE(sceAppContentInitialize);

HLE_IMPL(int, sceAppContentTemporaryDataMount2, int option, AppContentMountPoint* mount_point) {
    HLE_TRACE("opcao=%d -> /temp0", option);
    if (!mount_point) {
        return kAppContentErrorParameter;
    }
    std::memset(mount_point, 0, sizeof(*mount_point));
    std::strcpy(mount_point->data, "/temp0");
    return err::OK;
}
HLE_OVERRIDE(sceAppContentTemporaryDataMount2);

} // namespace

namespace hle {

// Fase 46: pasta com uma subpasta por DLC (vazia = sem DLC).
void SetAddcontDirectory(const std::filesystem::path& directory) {
    std::lock_guard lock(g_addcont_mutex);
    g_addcont_dir = directory;
}

// Para o selftest: recoloca o evento de login na fila.
void ResetUserServiceEvents() {
    g_login_event_pending.store(true);
}

} // namespace hle
