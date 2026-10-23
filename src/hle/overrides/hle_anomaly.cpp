// Fase 67: Anomaly (ver hle_anomaly.h). A logica dos ganchos segue src/hooks/file_hooks.cpp e gui_hooks.cpp do
// Anomaly v1.1 de connorh315 (GPL-2.0-or-later); enderecos e remendos sao os dele, para o eboot 1.24.
// O "long" do Orbis tem 64 bits e o do Windows 32: aqui todo long do Anomaly virou s64.
#include "hle/overrides/hle_anomaly.h"

#include <windows.h>

#include <Zydis/Zydis.h>

#include <cstdio>
#include <cstring>

#include "hle/generated/hle_stub.h"

namespace hle::anomaly {

namespace {

u8* g_base = nullptr;
std::vector<std::string> g_names;
std::vector<std::string> g_paths; // "/app0/mods/<nome>": o jogo pode guardar o ponteiro, entao vivem ate o fim
int g_loaded = 0;
bool g_mods_opened = false;

// ---- ganchos no eboot ----------------------------------------------------------------------------------------
// Copia as instrucoes inteiras do comeco da funcao (>= 14 bytes) para um trampolim que volta ao resto dela, e poe
// no lugar um "jmp [rip+0]" para o gancho. As funcoes ganchadas comecam com push/mov/sub, sem enderecos relativos.
void WriteJump(u8* at, const void* to) {
    at[0] = 0xFF;
    at[1] = 0x25;
    std::memset(at + 2, 0, 4);
    const u64 target = reinterpret_cast<u64>(to);
    std::memcpy(at + 6, &target, 8);
}

void* Hook(u64 offset, const void* replacement) {
    u8* target = g_base + offset;
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
    size_t length = 0;
    while (length < 14) {
        ZydisDecodedInstruction insn;
        ZydisDecodedOperand ops[ZYDIS_MAX_OPERAND_COUNT];
        if (ZYAN_FAILED(ZydisDecoderDecodeFull(&decoder, target + length, 32, &insn, ops)) ||
            (insn.attributes & ZYDIS_ATTRIB_IS_RELATIVE)) {
            std::printf("[ANOMALY] eboot+0x%llx: comeco da funcao diferente do esperado, gancho nao posto\n",
                        static_cast<u64>(offset));
            return nullptr;
        }
        length += insn.length;
    }
    auto* trampoline = static_cast<u8*>(VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!trampoline) {
        return nullptr;
    }
    std::memcpy(trampoline, target, length);
    WriteJump(trampoline + length, target + length);
    DWORD old = 0;
    VirtualProtect(target, length, PAGE_EXECUTE_READWRITE, &old);
    WriteJump(target, replacement);
    std::memset(target + 14, 0x90, length - 14);
    VirtualProtect(target, length, old, &old);
    FlushInstructionCache(GetCurrentProcess(), target, length);
    return trampoline;
}

// Remendo de bytes no codigo (o Anomaly faz alguns com o jogo ja rodando).
void Patch(u64 offset, const char* bytes, size_t count) {
    u8* at = g_base + offset;
    DWORD old = 0;
    VirtualProtect(at, count, PAGE_EXECUTE_READWRITE, &old);
    std::memcpy(at, bytes, count);
    VirtualProtect(at, count, old, &old);
    FlushInstructionCache(GetCurrentProcess(), at, count);
}

#define PATCH(offset, literal) Patch(offset, literal, sizeof(literal) - 1)

// ---- arquivos em camadas (text.csv, collection.txt) ------------------------------------------------------------
struct LayerableFile {
    u32 hash;
    const char* path;
    bool is_active;
    int last_collisions_index;
    int last_yielded_index;
    bool is_loaded;
    bool look_ahead;
    bool is_exhausted;
};

LayerableFile g_layerable[] = {
    {0x1347d7cd, "stuff/text/text.csv", false, -1, -1, false, false, false},
    {0x874ca1b4, "chars/collection.txt", false, -1, -1, false, true, false},
};

LayerableFile* FindLayerable(u32 hash) {
    for (auto& file : g_layerable) {
        if (file.hash == hash) {
            return &file;
        }
    }
    return nullptr;
}

void HasYielded(LayerableFile* file) { file->last_yielded_index = file->last_collisions_index; }

// NuFileTable_FindHash: o ParseTextCSV pede o indice do stuff/text/text.csv; aqui ele recebe, um de cada vez, o
// indice de cada pacote que tem o arquivo (a tabela do jogo guarda os nomes repetidos com o indice de cada um).
using FindHashFn = int(HLE_SYSV_ABI*)(int*, unsigned int, char*);
FindHashFn g_find_hash = nullptr;

HLE_SYSV_ABI int FindHash(int* file_table, unsigned int file_hash, char* file_path) {
    const int string_count = *reinterpret_cast<const int*>(reinterpret_cast<char*>(file_table) + 0x20);
    const char* ptr = *reinterpret_cast<const char**>(reinterpret_cast<char*>(file_table) + 0x28);
    LayerableFile* layerable = FindLayerable(file_hash);
    if (layerable && layerable->is_active && !layerable->is_loaded) {
        if (layerable->last_yielded_index != layerable->last_collisions_index) {
            return layerable->last_collisions_index;
        }
        int prev_index = -1;
        for (int i = 0; i < string_count; i++) {
            const size_t len = std::strlen(ptr);
            if (i > layerable->last_collisions_index && !std::strcmp(ptr, layerable->path)) {
                if (layerable->look_ahead && prev_index != -1) {
                    return prev_index;
                }
                ptr += len + 1;
                if (reinterpret_cast<uintptr_t>(ptr) & 1) {
                    ptr++;
                }
                prev_index = *reinterpret_cast<const u16*>(ptr);
                layerable->last_collisions_index = i;
                ptr += 2;
                if (!layerable->look_ahead) {
                    return prev_index;
                }
                continue;
            }
            ptr += len + 1;
            if (reinterpret_cast<uintptr_t>(ptr) & 1) {
                ptr++;
            }
            ptr += 2;
        }
        if (prev_index != -1) {
            layerable->is_exhausted = true;
            return prev_index;
        }
        layerable->is_loaded = true;
    }
    return g_find_hash(file_table, file_hash, file_path);
}

// NuStringTableLoadCSV: carrega o text.csv uma vez por pacote que o tem, sem limpar a tabela entre um e outro.
using LoadCsvFn = int(HLE_SYSV_ABI*)(char*, s64*, u64, u8*, int, int, void*, int, char);
LoadCsvFn g_load_csv = nullptr;

HLE_SYSV_ABI int LoadCsv(char* path, s64* p2, u64 p3, u8* label, int p5, int p6, void* p7, int p8, char p9) {
    LayerableFile* layerable = FindLayerable(0x1347d7cd);
    if (path && !std::strcmp(path, "stuff\\text\\text.csv") && layerable && !layerable->is_active) {
        layerable->is_active = true;
        bool cleared = false;
        int result = 0;
        while (!layerable->is_loaded) {
            result = g_load_csv(path, p2, p3, label, p5, p6, p7, p8, p9);
            if (!cleared) {
                PATCH(0x004ef7b4, "\x90\x90\x90\x90\x90"); // nao limpa a tabela antes do proximo
                cleared = true;
            }
            HasYielded(layerable);
        }
        return result;
    }
    return g_load_csv(path, p2, p3, label, p5, p6, p7, p8, p9);
}

// MechCollections_ImportCollectionFile: o chars/collection.txt principal e importado de cada pacote que o tem.
using ImportCollectionFn = void(HLE_SYSV_ABI*)(s64*, char*, s64*, s64*);
ImportCollectionFn g_import_collection = nullptr;

HLE_SYSV_ABI void ImportCollection(s64* mech_collections, char* path, s64* p3, s64* dlc_collection) {
    g_import_collection(mech_collections, path, p3, dlc_collection);
    if (path && path[0] == '.' && path[1] == '/') {
        LayerableFile* layerable = FindLayerable(0x874ca1b4);
        if (layerable && !layerable->is_active) {
            layerable->is_active = true;
            PATCH(0x011c457f, "\xeb\x32");
            while (!layerable->is_loaded && !layerable->is_exhausted) {
                g_import_collection(mech_collections, path, p3, dlc_collection);
                HasYielded(layerable);
            }
        }
    }
}

// NuFileTree_GetFilename: so para o text.csv, desliga a remocao de nomes repetidos, para a tabela guardar um
// indice por pacote.
bool g_dedup_enabled = true;

void ToggleDedup(bool disable) {
    if (g_dedup_enabled == !disable) {
        return;
    }
    if (disable) {
        PATCH(0x0004da4d5, "\x48\xE9\xC5\x00\x00\x00");
    } else {
        PATCH(0x0004da4d5, "\x0F\x87\xC5\x00\x00\x00");
    }
    g_dedup_enabled = !disable;
}

using GetFilenameFn = void(HLE_SYSV_ABI*)(s64, s64*, unsigned int, char);
GetFilenameFn g_get_filename = nullptr;

HLE_SYSV_ABI void GetFilename(s64 file_tree, s64* built_path, unsigned int p3, char zero) {
    g_get_filename(file_tree, built_path, p3, zero);
    ToggleDedup(std::strcmp(reinterpret_cast<char*>(*built_path), "stuff/text/text.csv") == 0);
}

// ---- pacotes de mod ------------------------------------------------------------------------------------------
using OpenIndividualFn = s64*(HLE_SYSV_ABI*)(s64, char*, int);
OpenIndividualFn g_open_individual = nullptr;

HLE_SYSV_ABI s64* OpenIndividual(s64 device, char* filename, int counter);

void LoadMods(s64 device) {
    for (const std::string& path : g_paths) {
        s64* dat = OpenIndividual(0, const_cast<char*>(path.c_str()), -1);
        std::printf("[ANOMALY] %s: %s\n", path.c_str(), dat ? "carregado" : "o jogo nao abriu");
        if (!dat) {
            continue;
        }
        ++g_loaded;
        *reinterpret_cast<u32*>(reinterpret_cast<char*>(dat) + 0x128) = 0;
        int* count = reinterpret_cast<int*>(device + 0xe40);
        s64** list = reinterpret_cast<s64**>(device + 0xe48);
        list[*count] = dat;
        *count += 1;
    }
    std::fflush(stdout);
}

// NuFileDeviceDat_OpenIndividual: a primeira chamada (upd:PATCH) acontece antes de qualquer pacote do jogo; os
// mods entram ali, na frente de todos.
HLE_SYSV_ABI s64* OpenIndividual(s64 device, char* filename, int counter) {
    if (filename && !std::strcmp(filename, "upd:PATCH") && counter < 0 && !g_mods_opened) {
        g_mods_opened = true;
        LoadMods(device);
    }
    return g_open_individual(device, filename, counter);
}

// ---- menu principal ------------------------------------------------------------------------------------------
using FindObjectFn = s64*(HLE_SYSV_ABI*)(s64*, const char*);
using SetTextFn = void(HLE_SYSV_ABI*)(s64*, char*);
using AboutToShowFn = void(HLE_SYSV_ABI*)(s64*);
AboutToShowFn g_about_to_show = nullptr;
char g_menu_text[64];

HLE_SYSV_ABI void AboutToShow(s64* main_menu_screen) {
    g_about_to_show(main_menu_screen);
    auto find_object = reinterpret_cast<FindObjectFn>(g_base + 0xcbaf80);
    auto set_text = reinterpret_cast<SetTextFn>(g_base + 0xb74d80);
    if (s64* entry = find_object(main_menu_screen, "xml_switchprofile")) {
        std::snprintf(g_menu_text, sizeof(g_menu_text), "Anomaly (%d loaded)", g_loaded);
        set_text(entry, g_menu_text);
    }
}

} // namespace

void SetMods(std::vector<std::string> names) {
    g_names = std::move(names);
    g_paths.clear();
    for (const std::string& name : g_names) {
        g_paths.push_back("/app0/mods/" + name);
    }
}

bool Install(u8* base) {
    g_base = base;
    if (g_names.empty()) {
        return false; // sem pacotes de mod: o eboot fica como esta
    }
    // o Anomaly so vale para o eboot 1.24 (marca "_PATCH11C" no fim deste texto)
    const char* marker = reinterpret_cast<const char*>(base + 0x2421EBD);
    const size_t len = strnlen(marker, 256);
    if (len < 9 || _strnicmp(marker + len - 9, "_PATCH11C", 9) != 0) {
        std::printf("[ANOMALY] eboot que nao e o 1.24: mods do Anomaly desligados\n");
        return false;
    }
    g_open_individual = reinterpret_cast<OpenIndividualFn>(Hook(0x5adc70, reinterpret_cast<const void*>(&OpenIndividual)));
    g_find_hash = reinterpret_cast<FindHashFn>(Hook(0x4d3ec0, reinterpret_cast<const void*>(&FindHash)));
    g_get_filename = reinterpret_cast<GetFilenameFn>(Hook(0x4d7b90, reinterpret_cast<const void*>(&GetFilename)));
    g_load_csv = reinterpret_cast<LoadCsvFn>(Hook(0x4ef570, reinterpret_cast<const void*>(&LoadCsv)));
    g_import_collection = reinterpret_cast<ImportCollectionFn>(Hook(0x11c4540, reinterpret_cast<const void*>(&ImportCollection)));
    PATCH(0x0cb9286, "\x90\x90\x90\x90\x90"); // mantem a entrada "trocar perfil" do menu, que vira o aviso do Anomaly
    g_about_to_show = reinterpret_cast<AboutToShowFn>(Hook(0xcb9430, reinterpret_cast<const void*>(&AboutToShow)));
    const bool ok = g_open_individual && g_find_hash && g_get_filename && g_load_csv && g_import_collection && g_about_to_show;
    std::printf("[ANOMALY] Anomaly v1.1 (connorh315): %s, %zu pacote(s) de mod\n", ok ? "ganchos postos" : "FALHOU",
                g_names.size());
    return ok;
}

int Loaded() { return g_loaded; }

} // namespace hle::anomaly
