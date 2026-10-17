#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "common/types.h"
#include "loader/elf_types.h"

namespace loader {

struct Segment {
    u32 type;
    u32 flags;
    u64 vaddr; // relativo a base
    u64 memsz;
    u64 filesz;
};

struct TlsInfo {
    u64 image_vaddr = 0; // imagem de inicializacao (.tdata), relativa a base
    u64 filesz = 0;
    u64 memsz = 0;
    u64 align = 1;
};

struct RelocStats {
    u64 rela_dyn = 0; // entradas em .rela.dyn (DT_SCE_RELA)
    u64 rela_plt = 0; // entradas em .rela.plt (DT_SCE_JMPREL)
    u64 relative = 0;
    u64 local_symbol = 0;
    u64 hle_function = 0;
    u64 hle_object = 0;
    u64 tls = 0;
    u64 unresolved = 0;
    u64 out_of_range = 0;
    std::map<u32, u64> unknown_types;
    std::vector<std::string> unresolved_names;
};

// Carrega um ELF PIE do Orbis (ET_SCE_DYNEXEC) em uma regiao reservada de tamanho fixo.
class ElfPieLoader {
public:
    static constexpr u64 kDefaultBase = 0x0000'0008'0000'0000ULL;
    static constexpr u64 kReserveSize = 64_MiB;

    ElfPieLoader() = default;
    ~ElfPieLoader();
    ElfPieLoader(const ElfPieLoader&) = delete;
    ElfPieLoader& operator=(const ElfPieLoader&) = delete;

    // Reserva a regiao e copia PT_LOAD / PT_SCE_RELRO. Tudo fica RW ate Protect().
    bool Load(const std::filesystem::path& path, u64 base = kDefaultBase);

    // Aplica .rela.dyn e .rela.plt; imports (NIDs) sao ligados a tabela de src/hle/generated.
    bool Relocate();

    // Aplica as protecoes finais de cada segmento (R-X, R--, RW-).
    bool Protect();

    const std::string& error() const { return error_; }
    u8* base() const { return base_; }
    u64 base_address() const { return reinterpret_cast<u64>(base_); }
    u64 image_size() const { return image_size_; }
    u64 entry() const { return base_address() + ehdr_.e_entry; }
    u64 entry_offset() const { return ehdr_.e_entry; }
    u16 elf_type() const { return ehdr_.e_type; }
    u64 init_offset() const { return dt_init_; }
    u64 proc_param_offset() const { return proc_param_; }
    const std::vector<Segment>& segments() const { return segments_; }
    const TlsInfo& tls() const { return tls_; }
    const RelocStats& reloc_stats() const { return stats_; }

private:
    bool Fail(std::string msg);
    bool ApplyRelocations(u64 table_offset, u64 table_size, bool is_plt);
    bool ResolveSymbol(u32 index, u64& value);

    std::string error_;
    std::vector<u8> file_;
    Elf64_Ehdr ehdr_{};
    std::vector<Elf64_Phdr> phdrs_;
    std::vector<Segment> segments_;
    TlsInfo tls_;
    RelocStats stats_;

    u8* base_ = nullptr;
    u64 image_size_ = 0;
    u64 proc_param_ = 0;

    // Offsets no arquivo (PT_SCE_DYNLIBDATA nao e mapeado em memoria)
    u64 dynlib_offset_ = 0;
    u64 dynlib_size_ = 0;
    u64 strtab_ = 0, strsz_ = 0;
    u64 symtab_ = 0, symtabsz_ = 0;
    u64 rela_ = 0, relasz_ = 0;
    u64 jmprel_ = 0, pltrelsz_ = 0;
    u64 dt_init_ = 0;
};

} // namespace loader
