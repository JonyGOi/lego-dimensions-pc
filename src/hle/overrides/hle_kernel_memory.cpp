// libkernel: memoria direta (sceKernel*DirectMemory) e sceKernelMunmap.
//
// A "memoria fisica" do Orbis vira so contabilidade: um espaco de offsets de 4 GiB com uma
// lista de faixas alocadas. O mapeamento e que consome memoria de verdade, via VirtualAlloc em
// uma faixa alta dedicada. Limitacao conhecida: mapear a mesma faixa fisica duas vezes produz
// duas copias independentes (nao ha aliasing) - o Orbis compartilharia as paginas.
#include <cinttypes>
#include <map>
#include <mutex>

#include <windows.h>

#include "hle/overrides/hle_override.h"

namespace {

using namespace hle;

constexpr u64 kDirectMemorySize = 0x1'0000'0000ULL; // 4 GiB
constexpr u64 kOrbisPage = 0x4000;                  // 16 KiB
constexpr u64 kWinGranularity = 0x10000;            // VirtualAlloc reserva em multiplos de 64 KiB
constexpr u64 kVirtualBase = 0x0000'0010'0000'0000ULL;
constexpr u64 kVirtualEnd = 0x0000'0040'0000'0000ULL;

constexpr int kMapFixed = 0x10;
constexpr int kProtExec = 0x04;
constexpr int kQueryFindNext = 0x1;

struct PhysRange {
    u64 size;
    int memory_type;
};

struct Mapping {
    u64 size;
    s64 phys_start; // -1 = memoria flexivel / sem lastro direto
    int prot;
};

struct DirectMemoryQueryInfo {
    s64 start;
    s64 end;
    int memory_type;
};

std::mutex g_mutex;
std::map<u64, PhysRange> g_phys;   // offset fisico -> faixa alocada
std::map<u64, Mapping> g_mappings; // endereco virtual -> mapeamento
u64 g_virtual_cursor = kVirtualBase;

bool IsPow2(u64 v) {
    return v && !(v & (v - 1));
}

DWORD ToWinProtect(int prot) {
    // Protecoes de GPU (0x10/0x20) nao tem equivalente: qualquer mapeamento vira leitura+escrita.
    return (prot & kProtExec) ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
}

// Procura um buraco em [search_start, search_end) no espaco fisico.
bool FindPhysical(u64 search_start, u64 search_end, u64 size, u64 align, u64& out) {
    u64 candidate = AlignUp(search_start, align);
    for (const auto& [start, range] : g_phys) {
        if (candidate + size <= start) {
            break;
        }
        if (start + range.size > candidate) {
            candidate = AlignUp(start + range.size, align);
        }
    }
    if (candidate + size > search_end || candidate + size > kDirectMemorySize) {
        return false;
    }
    out = candidate;
    return true;
}

void* ReserveVirtual(u64 hint, u64 size, u64 align, DWORD protect, bool fixed) {
    if (hint) {
        void* p = VirtualAlloc(reinterpret_cast<void*>(hint), size, MEM_RESERVE | MEM_COMMIT, protect);
        if (p || fixed) {
            return p;
        }
    }
    // Sem endereco pedido (ou ocupado): avanca o cursor da faixa dedicada.
    for (int attempt = 0; attempt < 64; ++attempt) {
        const u64 addr = AlignUp(g_virtual_cursor, align);
        if (addr + size > kVirtualEnd) {
            return nullptr;
        }
        g_virtual_cursor = addr + AlignUp(size, kWinGranularity);
        if (void* p = VirtualAlloc(reinterpret_cast<void*>(addr), size, MEM_RESERVE | MEM_COMMIT, protect)) {
            return p;
        }
    }
    return nullptr;
}

HLE_IMPL(u64, sceKernelGetDirectMemorySize) {
    HLE_TRACE("-> 0x%" PRIx64, kDirectMemorySize);
    return kDirectMemorySize;
}
HLE_OVERRIDE(sceKernelGetDirectMemorySize);

HLE_IMPL(int, sceKernelAllocateDirectMemory, s64 search_start, s64 search_end, u64 len, u64 alignment,
         int memory_type, s64* phys_out) {
    int result = err::OK;
    u64 phys = 0;
    if (!alignment) {
        alignment = kOrbisPage;
    }
    if (search_start < 0 || search_end <= search_start || !len || (len % kOrbisPage) ||
        !IsPow2(alignment) || (alignment % kOrbisPage) || !phys_out) {
        result = err::kEINVAL;
    } else {
        std::lock_guard lock(g_mutex);
        if (FindPhysical(search_start, search_end, len, alignment, phys)) {
            g_phys[phys] = {len, memory_type};
            *phys_out = static_cast<s64>(phys);
        } else {
            result = err::kEAGAIN;
        }
    }
    HLE_TRACE("busca=[0x%" PRIx64 ", 0x%" PRIx64 ") len=0x%" PRIx64 " (%.1f MiB) align=0x%" PRIx64
              " tipo=%d -> %s fis=0x%" PRIx64,
              (u64)search_start, (u64)search_end, len, len / 1048576.0, alignment, memory_type,
              result ? "ERRO" : "OK", phys);
    return result;
}
HLE_OVERRIDE(sceKernelAllocateDirectMemory);

HLE_IMPL(int, sceKernelMapDirectMemory, void** addr, u64 len, int prot, int flags, s64 phys_start,
         u64 alignment) {
    int result = err::OK;
    void* mapped = nullptr;
    const u64 requested = addr ? reinterpret_cast<u64>(*addr) : 0;
    if (!addr || !len || (len % kOrbisPage) || (alignment && (!IsPow2(alignment) || alignment % kOrbisPage)) ||
        phys_start < 0 || (requested % kOrbisPage)) {
        result = err::kEINVAL;
    } else {
        std::lock_guard lock(g_mutex);
        const u64 align = alignment > kWinGranularity ? alignment : kWinGranularity;
        // O Windows so reserva em fronteiras de 64 KiB; um pedido alinhado so em 16 KiB e
        // arredondado para baixo quando nao e MAP_FIXED.
        const bool fixed = (flags & kMapFixed) != 0;
        const u64 hint = fixed ? requested : AlignDown(requested, kWinGranularity);
        mapped = ReserveVirtual(hint, len, align, ToWinProtect(prot), fixed);
        if (mapped) {
            g_mappings[reinterpret_cast<u64>(mapped)] = {len, phys_start, prot};
            *addr = mapped;
        } else {
            result = err::kENOMEM;
        }
    }
    HLE_TRACE("pedido=0x%" PRIx64 " len=0x%" PRIx64 " (%.1f MiB) prot=0x%x flags=0x%x fis=0x%" PRIx64
              " align=0x%" PRIx64 " -> %s virt=%p",
              requested, len, len / 1048576.0, prot, flags, (u64)phys_start, alignment,
              result ? "ERRO" : "OK", mapped);
    return result;
}
HLE_OVERRIDE(sceKernelMapDirectMemory);

// Desfaz [addr, addr+len). Mapeamento inteiro e liberado; pedaco de um mapeamento e so
// descomprometido (o Windows nao permite liberar parte de uma reserva).
int UnmapLocked(u64 addr, u64 len) {
    auto it = g_mappings.upper_bound(addr);
    if (it == g_mappings.begin()) {
        return err::kEINVAL;
    }
    --it;
    const u64 base = it->first;
    if (addr + len > base + it->second.size) {
        return err::kEINVAL;
    }
    if (addr == base && len == it->second.size) {
        VirtualFree(reinterpret_cast<void*>(base), 0, MEM_RELEASE);
        g_mappings.erase(it);
    } else {
        VirtualFree(reinterpret_cast<void*>(addr), len, MEM_DECOMMIT);
    }
    return err::OK;
}

HLE_IMPL(int, sceKernelMunmap, void* addr, u64 len) {
    int result;
    {
        std::lock_guard lock(g_mutex);
        result = (!addr || !len) ? err::kEINVAL : UnmapLocked(reinterpret_cast<u64>(addr), len);
    }
    HLE_TRACE("addr=%p len=0x%" PRIx64 " -> %s", addr, len, result ? "ERRO" : "OK");
    return result;
}
HLE_OVERRIDE(sceKernelMunmap);

HLE_IMPL(int, sceKernelReleaseDirectMemory, s64 start, u64 len) {
    int released = 0, unmapped = 0;
    {
        std::lock_guard lock(g_mutex);
        const u64 begin = static_cast<u64>(start), end = begin + len;
        for (auto it = g_phys.begin(); it != g_phys.end();) {
            if (it->first >= begin && it->first + it->second.size <= end) {
                it = g_phys.erase(it);
                ++released;
            } else {
                ++it;
            }
        }
        // O Orbis tambem desfaz os mapeamentos que apontavam para a faixa liberada.
        for (auto it = g_mappings.begin(); it != g_mappings.end();) {
            const s64 phys = it->second.phys_start;
            if (phys >= 0 && static_cast<u64>(phys) >= begin && static_cast<u64>(phys) + it->second.size <= end) {
                VirtualFree(reinterpret_cast<void*>(it->first), 0, MEM_RELEASE);
                it = g_mappings.erase(it);
                ++unmapped;
            } else {
                ++it;
            }
        }
    }
    HLE_TRACE("fis=0x%" PRIx64 " len=0x%" PRIx64 " -> %d faixas liberadas, %d mapeamentos desfeitos",
              (u64)start, len, released, unmapped);
    return err::OK;
}
HLE_OVERRIDE(sceKernelReleaseDirectMemory);

HLE_IMPL(int, sceKernelDirectMemoryQuery, s64 offset, int flags, DirectMemoryQueryInfo* info, u64 info_size) {
    int result = err::kEACCES;
    if (!info || info_size != sizeof(DirectMemoryQueryInfo) || offset < 0) {
        result = err::kEINVAL;
    } else {
        std::lock_guard lock(g_mutex);
        for (const auto& [start, range] : g_phys) {
            const bool contains = static_cast<u64>(offset) >= start && static_cast<u64>(offset) < start + range.size;
            const bool next = (flags & kQueryFindNext) && start > static_cast<u64>(offset);
            if (contains || next) {
                info->start = static_cast<s64>(start);
                info->end = static_cast<s64>(start + range.size);
                info->memory_type = range.memory_type;
                result = err::OK;
                break;
            }
        }
    }
    HLE_TRACE("offset=0x%" PRIx64 " flags=0x%x -> %s", (u64)offset, flags, result ? "nao encontrado" : "OK");
    return result;
}
HLE_OVERRIDE(sceKernelDirectMemoryQuery);

} // namespace
