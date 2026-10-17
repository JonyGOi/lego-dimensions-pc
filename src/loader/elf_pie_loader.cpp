#include "loader/elf_pie_loader.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <string_view>

#include <windows.h>

#include "hle_symbol_table.h"

namespace loader {

namespace {

constexpr u64 kPageSize = 0x1000;

DWORD ToWinProtect(u32 flags) {
    if (flags & PF_X) {
        return (flags & PF_W) ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
    }
    return (flags & PF_W) ? PAGE_READWRITE : PAGE_READONLY;
}

} // namespace

ElfPieLoader::~ElfPieLoader() {
    if (base_) {
        VirtualFree(base_, 0, MEM_RELEASE);
    }
}

bool ElfPieLoader::Fail(std::string msg) {
    error_ = std::move(msg);
    return false;
}

bool ElfPieLoader::Load(const std::filesystem::path& path, u64 base) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) {
        return Fail("nao foi possivel abrir " + path.string());
    }
    file_.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0);
    in.read(reinterpret_cast<char*>(file_.data()), static_cast<std::streamsize>(file_.size()));

    if (file_.size() < sizeof(Elf64_Ehdr) || std::memcmp(file_.data(), "\x7f" "ELF", 4) != 0) {
        return Fail("arquivo nao e um ELF (rode tools/check_binaries.py para extrair o FSELF)");
    }
    std::memcpy(&ehdr_, file_.data(), sizeof(ehdr_));
    if (ehdr_.e_machine != EM_X86_64 || ehdr_.e_phentsize != sizeof(Elf64_Phdr)) {
        return Fail("ELF nao e x86-64");
    }
    if (ehdr_.e_type != ET_SCE_DYNEXEC) {
        return Fail("ELF nao e PIE do Orbis (ET_SCE_DYNEXEC)");
    }
    if (ehdr_.e_phoff + u64(ehdr_.e_phnum) * sizeof(Elf64_Phdr) > file_.size()) {
        return Fail("program headers fora do arquivo");
    }
    phdrs_.resize(ehdr_.e_phnum);
    std::memcpy(phdrs_.data(), file_.data() + ehdr_.e_phoff, phdrs_.size() * sizeof(Elf64_Phdr));

    const Elf64_Phdr* dynamic = nullptr;
    for (const auto& ph : phdrs_) {
        switch (ph.p_type) {
        case PT_LOAD:
        case PT_SCE_RELRO:
            if (ph.p_offset + ph.p_filesz > file_.size() || ph.p_filesz > ph.p_memsz) {
                return Fail("segmento carregavel invalido");
            }
            segments_.push_back({ph.p_type, ph.p_flags, ph.p_vaddr, ph.p_memsz, ph.p_filesz});
            image_size_ = std::max(image_size_, ph.p_vaddr + ph.p_memsz);
            break;
        case PT_DYNAMIC:
            dynamic = &ph;
            break;
        case PT_SCE_DYNLIBDATA:
            dynlib_offset_ = ph.p_offset;
            dynlib_size_ = ph.p_filesz;
            break;
        case PT_SCE_PROCPARAM:
            proc_param_ = ph.p_vaddr;
            break;
        case PT_TLS:
            tls_ = {ph.p_vaddr, ph.p_filesz, ph.p_memsz, std::max<u64>(ph.p_align, 1)};
            break;
        default:
            break;
        }
    }
    image_size_ = AlignUp(image_size_, kPageSize);
    if (segments_.empty() || image_size_ > kReserveSize) {
        return Fail("imagem nao cabe na regiao reservada de 64 MiB");
    }
    if (!dynamic || !dynlib_size_ || dynlib_offset_ + dynlib_size_ > file_.size() ||
        dynamic->p_offset + dynamic->p_filesz > file_.size()) {
        return Fail("PT_DYNAMIC / PT_SCE_DYNLIBDATA ausentes ou invalidos");
    }

    // Uma unica regiao contigua: a imagem no inicio, o resto fica livre para trampolins.
    base_ = static_cast<u8*>(VirtualAlloc(reinterpret_cast<void*>(base), kReserveSize,
                                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!base_) {
        char buf[128];
        std::snprintf(buf, sizeof(buf), "VirtualAlloc em 0x%llx falhou (erro %lu)",
                      static_cast<unsigned long long>(base), GetLastError());
        return Fail(buf);
    }

    for (const auto& ph : phdrs_) {
        if (ph.p_type != PT_LOAD && ph.p_type != PT_SCE_RELRO) {
            continue;
        }
        // VirtualAlloc ja entrega memoria zerada: o que passa de p_filesz (.bss) fica em zero.
        std::memcpy(base_ + ph.p_vaddr, file_.data() + ph.p_offset, ph.p_filesz);
    }

    const size_t count = dynamic->p_filesz / sizeof(Elf64_Dyn);
    for (size_t i = 0; i < count; ++i) {
        Elf64_Dyn dyn;
        std::memcpy(&dyn, file_.data() + dynamic->p_offset + i * sizeof(dyn), sizeof(dyn));
        if (dyn.d_tag == DT_NULL) {
            break;
        }
        switch (dyn.d_tag) {
        case DT_SCE_STRTAB: strtab_ = dyn.d_val; break;
        case DT_SCE_STRSZ: strsz_ = dyn.d_val; break;
        case DT_SCE_SYMTAB: symtab_ = dyn.d_val; break;
        case DT_SCE_SYMTABSZ: symtabsz_ = dyn.d_val; break;
        case DT_SCE_RELA: rela_ = dyn.d_val; break;
        case DT_SCE_RELASZ: relasz_ = dyn.d_val; break;
        case DT_SCE_JMPREL: jmprel_ = dyn.d_val; break;
        case DT_SCE_PLTRELSZ: pltrelsz_ = dyn.d_val; break;
        case DT_INIT: dt_init_ = dyn.d_val; break;
        default: break;
        }
    }
    if (strtab_ + strsz_ > dynlib_size_ || symtab_ + symtabsz_ > dynlib_size_ ||
        rela_ + relasz_ > dynlib_size_ || jmprel_ + pltrelsz_ > dynlib_size_) {
        return Fail("tabelas dinamicas fora de PT_SCE_DYNLIBDATA");
    }
    return true;
}

bool ElfPieLoader::ResolveSymbol(u32 index, u64& value) {
    if (u64(index + 1) * sizeof(Elf64_Sym) > symtabsz_) {
        return false;
    }
    Elf64_Sym sym;
    std::memcpy(&sym, file_.data() + dynlib_offset_ + symtab_ + index * sizeof(sym), sizeof(sym));
    if (sym.st_shndx != 0) {
        value = base_address() + sym.st_value;
        ++stats_.local_symbol;
        return true;
    }
    if (sym.st_name >= strsz_) {
        return false;
    }
    // Nome no formato "NID#biblioteca#modulo" (ids em base64 do Orbis)
    const char* raw = reinterpret_cast<const char*>(file_.data() + dynlib_offset_ + strtab_ + sym.st_name);
    const std::string_view full(raw, strnlen(raw, strsz_ - sym.st_name));
    const std::string_view nid = full.substr(0, full.find('#'));
    if (const hle::HleSymbol* hle_sym = hle::FindSymbol(nid)) {
        value = reinterpret_cast<u64>(hle_sym->address);
        ++(hle_sym->kind == hle::SymbolKind::Function ? stats_.hle_function : stats_.hle_object);
        return true;
    }
    if ((sym.st_info >> 4) == STB_WEAK) {
        value = 0;
        return true;
    }
    if (stats_.unresolved_names.size() < 16) {
        stats_.unresolved_names.emplace_back(full);
    }
    return false;
}

bool ElfPieLoader::ApplyRelocations(u64 table_offset, u64 table_size, bool is_plt) {
    const u64 count = table_size / sizeof(Elf64_Rela);
    const u8* table = file_.data() + dynlib_offset_ + table_offset;
    // Variante II (amd64): o bloco TLS estatico fica logo abaixo do thread pointer.
    const u64 static_tls_size = AlignUp(tls_.memsz, tls_.align);

    for (u64 i = 0; i < count; ++i) {
        Elf64_Rela rel;
        std::memcpy(&rel, table + i * sizeof(rel), sizeof(rel));
        const u32 type = static_cast<u32>(rel.r_info);
        const u32 sym_index = static_cast<u32>(rel.r_info >> 32);
        ++(is_plt ? stats_.rela_plt : stats_.rela_dyn);

        if (rel.r_offset + sizeof(u64) > image_size_) {
            ++stats_.out_of_range;
            continue;
        }
        u64 value = 0;
        switch (type) {
        case R_X86_64_RELATIVE:
            value = base_address() + rel.r_addend;
            ++stats_.relative;
            break;
        case R_X86_64_64:
        case R_X86_64_GLOB_DAT:
        case R_X86_64_JUMP_SLOT:
            if (!ResolveSymbol(sym_index, value)) {
                ++stats_.unresolved;
                continue;
            }
            if (type == R_X86_64_64) {
                value += rel.r_addend;
            }
            break;
        case R_X86_64_DTPMOD64:
            value = 1; // o eboot e sempre o modulo TLS 1
            ++stats_.tls;
            break;
        case R_X86_64_DTPOFF64:
        case R_X86_64_TPOFF64: {
            Elf64_Sym sym{};
            if (u64(sym_index + 1) * sizeof(Elf64_Sym) <= symtabsz_) {
                std::memcpy(&sym, file_.data() + dynlib_offset_ + symtab_ + sym_index * sizeof(sym),
                            sizeof(sym));
            }
            value = sym.st_value + rel.r_addend;
            if (type == R_X86_64_TPOFF64) {
                value -= static_tls_size;
            }
            ++stats_.tls;
            break;
        }
        default:
            ++stats_.unknown_types[type];
            continue;
        }
        std::memcpy(base_ + rel.r_offset, &value, sizeof(value));
    }
    return true;
}

bool ElfPieLoader::Relocate() {
    if (!base_) {
        return Fail("Relocate() antes de Load()");
    }
    ApplyRelocations(rela_, relasz_, false);
    ApplyRelocations(jmprel_, pltrelsz_, true);
    return true;
}

bool ElfPieLoader::Protect() {
    for (const auto& seg : segments_) {
        const u64 begin = AlignDown(seg.vaddr, kPageSize);
        const u64 end = AlignUp(seg.vaddr + seg.memsz, kPageSize);
        const u32 flags = seg.type == PT_SCE_RELRO ? PF_R : seg.flags;
        DWORD old;
        if (!VirtualProtect(base_ + begin, end - begin, ToWinProtect(flags), &old)) {
            return Fail("VirtualProtect falhou");
        }
    }
    FlushInstructionCache(GetCurrentProcess(), base_, image_size_);
    return true;
}

} // namespace loader
