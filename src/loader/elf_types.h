#pragma once

#include "common/types.h"

namespace loader {

struct Elf64_Ehdr {
    u8 e_ident[16];
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u64 e_entry;
    u64 e_phoff;
    u64 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
};
static_assert(sizeof(Elf64_Ehdr) == 0x40);

struct Elf64_Phdr {
    u32 p_type;
    u32 p_flags;
    u64 p_offset;
    u64 p_vaddr;
    u64 p_paddr;
    u64 p_filesz;
    u64 p_memsz;
    u64 p_align;
};
static_assert(sizeof(Elf64_Phdr) == 0x38);

struct Elf64_Dyn {
    s64 d_tag;
    u64 d_val;
};

struct Elf64_Sym {
    u32 st_name;
    u8 st_info;
    u8 st_other;
    u16 st_shndx;
    u64 st_value;
    u64 st_size;
};
static_assert(sizeof(Elf64_Sym) == 0x18);

struct Elf64_Rela {
    u64 r_offset;
    u64 r_info;
    s64 r_addend;
};
static_assert(sizeof(Elf64_Rela) == 0x18);

constexpr u16 ET_SCE_EXEC = 0xFE00;
constexpr u16 ET_SCE_DYNEXEC = 0xFE10;
constexpr u16 EM_X86_64 = 62;

constexpr u32 PT_LOAD = 1;
constexpr u32 PT_DYNAMIC = 2;
constexpr u32 PT_TLS = 7;
constexpr u32 PT_SCE_DYNLIBDATA = 0x61000000;
constexpr u32 PT_SCE_PROCPARAM = 0x61000001;
constexpr u32 PT_SCE_RELRO = 0x61000010;

constexpr u32 PF_X = 1;
constexpr u32 PF_W = 2;
constexpr u32 PF_R = 4;

constexpr s64 DT_NULL = 0;
constexpr s64 DT_INIT = 0x0C;
constexpr s64 DT_FINI = 0x0D;
constexpr s64 DT_SCE_JMPREL = 0x61000029;
constexpr s64 DT_SCE_PLTRELSZ = 0x6100002D;
constexpr s64 DT_SCE_RELA = 0x6100002F;
constexpr s64 DT_SCE_RELASZ = 0x61000031;
constexpr s64 DT_SCE_STRTAB = 0x61000035;
constexpr s64 DT_SCE_STRSZ = 0x61000037;
constexpr s64 DT_SCE_SYMTAB = 0x61000039;
constexpr s64 DT_SCE_SYMTABSZ = 0x6100003F;

constexpr u32 R_X86_64_64 = 1;
constexpr u32 R_X86_64_GLOB_DAT = 6;
constexpr u32 R_X86_64_JUMP_SLOT = 7;
constexpr u32 R_X86_64_RELATIVE = 8;
constexpr u32 R_X86_64_DTPMOD64 = 16;
constexpr u32 R_X86_64_DTPOFF64 = 17;
constexpr u32 R_X86_64_TPOFF64 = 18;

constexpr u8 STB_WEAK = 2;

} // namespace loader
