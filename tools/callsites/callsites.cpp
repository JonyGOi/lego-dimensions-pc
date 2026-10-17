// Desmonta, no eboot.bin.elf, os pontos onde o jogo chama funcoes importadas.
//
//   callsites <eboot.elf> <aerolib.csv> <prefixo-ou-nome> [...] [--max N] [--before N] [--after N]
//
// Para cada import cujo nome comeca com um dos prefixos:
//   1. acha o slot da GOT na .rela.plt (DT_SCE_JMPREL);
//   2. acha o stub da PLT  (jmp qword ptr [rip+slot]);
//   3. acha todo "call rel32" para o stub;
//   4. recua ate o prologo da funcao (push rbp; mov rbp, rsp) e desmonta ate depois da chamada.
// Chamadas a outros imports e strings referenciadas por LEA/MOV sao anotadas.
#include <algorithm>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <Zydis/Zydis.h>

#include "loader/elf_types.h"

using namespace loader;

namespace {

std::vector<u8> g_file;
std::vector<Elf64_Phdr> g_phdrs;

const u8* AtVaddr(u64 vaddr, u64 size = 1) {
    for (const auto& ph : g_phdrs) {
        if ((ph.p_type == PT_LOAD || ph.p_type == PT_SCE_RELRO) && vaddr >= ph.p_vaddr &&
            vaddr + size <= ph.p_vaddr + ph.p_filesz) {
            return g_file.data() + ph.p_offset + (vaddr - ph.p_vaddr);
        }
    }
    return nullptr;
}

// String C imprimivel em `vaddr` (minimo 3 caracteres), para anotar LEAs.
std::string StringAt(u64 vaddr) {
    const u8* p = AtVaddr(vaddr, 4);
    if (!p) {
        return {};
    }
    std::string s;
    for (int i = 0; i < 80; ++i) {
        const u8* c = AtVaddr(vaddr + i);
        if (!c || !*c) {
            break;
        }
        if (*c < 0x20 || *c > 0x7E) {
            if (*c == '\n') {
                s += "\\n";
                continue;
            }
            return {};
        }
        s += static_cast<char>(*c);
    }
    return s.size() >= 3 ? s : std::string{};
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::printf("uso: callsites <eboot.elf> <aerolib.csv> <prefixo> [...] [--max N] [--before N] [--after N]\n");
        return 1;
    }
    std::vector<std::string> prefixes;
    int max_sites = 3, before = 18, after = 14;
    for (int i = 3; i < argc; ++i) {
        if (!std::strcmp(argv[i], "--max") && i + 1 < argc) {
            max_sites = std::atoi(argv[++i]);
        } else if (!std::strcmp(argv[i], "--before") && i + 1 < argc) {
            before = std::atoi(argv[++i]);
        } else if (!std::strcmp(argv[i], "--after") && i + 1 < argc) {
            after = std::atoi(argv[++i]);
        } else if ((!std::strcmp(argv[i], "--at") || !std::strcmp(argv[i], "--callers")) && i + 1 < argc) {
            ++i; // tratado mais abaixo
        } else {
            prefixes.emplace_back(argv[i]);
        }
    }

    std::ifstream in(argv[1], std::ios::binary | std::ios::ate);
    if (!in) {
        std::printf("nao abriu %s\n", argv[1]);
        return 1;
    }
    g_file.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0);
    in.read(reinterpret_cast<char*>(g_file.data()), static_cast<std::streamsize>(g_file.size()));

    std::map<std::string, std::string> nid_names;
    {
        std::ifstream csv(argv[2]);
        std::string line;
        while (std::getline(csv, line)) {
            const size_t comma = line.find(',');
            if (comma != std::string::npos) {
                std::string name = line.substr(comma + 1);
                while (!name.empty() && (name.back() == '\r' || name.back() == '\n')) {
                    name.pop_back();
                }
                nid_names[line.substr(0, comma)] = name;
            }
        }
    }

    Elf64_Ehdr eh;
    std::memcpy(&eh, g_file.data(), sizeof(eh));
    g_phdrs.resize(eh.e_phnum);
    std::memcpy(g_phdrs.data(), g_file.data() + eh.e_phoff, g_phdrs.size() * sizeof(Elf64_Phdr));

    const Elf64_Phdr* dynamic = nullptr;
    const Elf64_Phdr* dynlib = nullptr;
    const Elf64_Phdr* text = nullptr;
    for (const auto& ph : g_phdrs) {
        if (ph.p_type == PT_DYNAMIC) dynamic = &ph;
        if (ph.p_type == PT_SCE_DYNLIBDATA) dynlib = &ph;
        if (ph.p_type == PT_LOAD && (ph.p_flags & PF_X)) text = &ph;
    }
    if (!dynamic || !dynlib || !text) {
        std::printf("ELF sem PT_DYNAMIC / PT_SCE_DYNLIBDATA / segmento executavel\n");
        return 1;
    }
    u64 strtab = 0, symtab = 0, jmprel = 0, pltrelsz = 0;
    for (u64 off = 0; off + 16 <= dynamic->p_filesz; off += 16) {
        Elf64_Dyn d;
        std::memcpy(&d, g_file.data() + dynamic->p_offset + off, sizeof(d));
        if (d.d_tag == DT_NULL) break;
        if (d.d_tag == DT_SCE_STRTAB) strtab = d.d_val;
        if (d.d_tag == DT_SCE_SYMTAB) symtab = d.d_val;
        if (d.d_tag == DT_SCE_JMPREL) jmprel = d.d_val;
        if (d.d_tag == DT_SCE_PLTRELSZ) pltrelsz = d.d_val;
    }

    // 1. slot da GOT -> nome do import
    std::map<u64, std::string> slot_names;
    for (u64 i = 0; i < pltrelsz / sizeof(Elf64_Rela); ++i) {
        Elf64_Rela rel;
        std::memcpy(&rel, g_file.data() + dynlib->p_offset + jmprel + i * sizeof(rel), sizeof(rel));
        Elf64_Sym sym;
        std::memcpy(&sym, g_file.data() + dynlib->p_offset + symtab + (rel.r_info >> 32) * sizeof(sym), sizeof(sym));
        std::string raw = reinterpret_cast<const char*>(g_file.data() + dynlib->p_offset + strtab + sym.st_name);
        const std::string nid = raw.substr(0, raw.find('#'));
        const auto it = nid_names.find(nid);
        slot_names[rel.r_offset] = it != nid_names.end() ? it->second : nid;
    }

    // 2. stubs da PLT: ff 25 disp32
    const u8* code = g_file.data() + text->p_offset;
    const u64 code_size = text->p_filesz;
    const u64 code_va = text->p_vaddr;
    std::map<u64, std::string> stub_names; // vaddr do stub -> nome
    for (u64 i = 0; i + 6 <= code_size; ++i) {
        if (code[i] == 0xFF && code[i + 1] == 0x25) {
            s32 disp;
            std::memcpy(&disp, code + i + 2, 4);
            const auto it = slot_names.find(code_va + i + 6 + disp);
            if (it != slot_names.end()) {
                stub_names[code_va + i] = it->second;
            }
        }
    }

    // 3. call rel32 -> stub
    std::map<std::string, std::vector<u64>> sites;
    for (u64 i = 0; i + 5 <= code_size; ++i) {
        if (code[i] == 0xE8 || code[i] == 0xE9) { // call, ou jmp de tail call
            s32 rel;
            std::memcpy(&rel, code + i + 1, 4);
            const auto it = stub_names.find(code_va + i + 5 + rel);
            if (it != stub_names.end()) {
                sites[it->second].push_back(code_va + i);
            }
        }
    }

    ZydisDecoder decoder;
    ZydisFormatter formatter;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    // "--at <hex>": desmonta em volta de um endereco qualquer (ex.: o RIP de um crash)
    for (int i = 3; i + 1 < argc; ++i) {
        if (!std::strcmp(argv[i], "--at")) {
            sites["@" + std::string(argv[i + 1])].push_back(std::strtoull(argv[i + 1], nullptr, 16));
            prefixes.emplace_back("@");
        }
    }

    // "--callers <hex>": lista todo call/jmp rel32 cujo destino e esse endereco
    for (int i = 3; i + 1 < argc; ++i) {
        if (!std::strcmp(argv[i], "--callers")) {
            const u64 target = std::strtoull(argv[i + 1], nullptr, 16);
            for (u64 k = 0; k + 5 <= code_size; ++k) {
                if (code[k] == 0xE8 || code[k] == 0xE9) {
                    s32 rel;
                    std::memcpy(&rel, code + k + 1, 4);
                    if (code_va + k + 5 + rel == target) {
                        sites["callers:" + std::string(argv[i + 1])].push_back(code_va + k);
                    }
                }
            }
            prefixes.emplace_back("callers:");
        }
    }

    for (const auto& [name, list] : sites) {
        const bool wanted = std::any_of(prefixes.begin(), prefixes.end(),
                                        [&](const std::string& p) { return name.compare(0, p.size(), p) == 0; });
        if (!wanted) {
            continue;
        }
        std::printf("\n################ %s  (%zu chamadas)\n", name.c_str(), list.size());
        int shown = 0;
        for (const u64 site : list) {
            if (shown++ >= max_sites) {
                break;
            }
            // 4. prologo mais proximo antes da chamada
            u64 start = site;
            for (u64 back = 1; back < 0x1800 && site - back >= code_va; ++back) {
                const u8* p = code + (site - back - code_va);
                if (p[0] == 0x55 && p[1] == 0x48 && p[2] == 0x89 && p[3] == 0xE5) {
                    start = site - back;
                    break;
                }
            }
            struct Line {
                u64 addr;
                std::string text;
            };
            std::vector<Line> lines;
            size_t call_index = 0;
            bool found = false;
            u64 pc = start;
            while (pc < code_va + code_size) {
                ZydisDecodedInstruction insn;
                ZydisDecodedOperand ops[ZYDIS_MAX_OPERAND_COUNT];
                const u8* p = code + (pc - code_va);
                if (ZYAN_FAILED(ZydisDecoderDecodeFull(&decoder, p, code_va + code_size - pc, &insn, ops))) {
                    break;
                }
                char buf[192];
                ZydisFormatterFormatInstruction(&formatter, &insn, ops, insn.operand_count_visible, buf, sizeof(buf),
                                                pc, ZYAN_NULL);
                std::string textline = buf;
                // anotacoes: destino de call e strings
                if (insn.mnemonic == ZYDIS_MNEMONIC_CALL && ops[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE) {
                    const u64 target = pc + insn.length + ops[0].imm.value.s;
                    const auto it = stub_names.find(target);
                    if (it != stub_names.end()) {
                        textline += "    ; " + it->second;
                    }
                }
                for (u8 i = 0; i < insn.operand_count_visible; ++i) {
                    if (ops[i].type == ZYDIS_OPERAND_TYPE_MEMORY && ops[i].mem.base == ZYDIS_REGISTER_RIP) {
                        const std::string s = StringAt(pc + insn.length + ops[i].mem.disp.value);
                        if (!s.empty() && insn.mnemonic == ZYDIS_MNEMONIC_LEA) {
                            textline += "    ; \"" + s + "\"";
                        }
                    }
                }
                lines.push_back({pc, textline});
                if (pc == site) {
                    call_index = lines.size() - 1;
                    found = true;
                }
                pc += insn.length;
                if (found && lines.size() - 1 - call_index >= static_cast<size_t>(after)) {
                    break;
                }
                if (pc > site + std::max<u64>(0x200, static_cast<u64>(after) * 15)) {
                    break;
                }
            }
            std::printf("\n---- chamada em eboot+0x%" PRIx64 " (funcao em eboot+0x%" PRIx64 ")%s\n", site, start,
                        found ? "" : "  [desmontagem dessincronizou antes da chamada]");
            const size_t first = found && call_index > static_cast<size_t>(before) ? call_index - before : 0;
            for (size_t i = first; i < lines.size(); ++i) {
                std::printf("  %s %08" PRIx64 "  %s\n", lines[i].addr == site ? "=>" : "  ", lines[i].addr,
                            lines[i].text.c_str());
            }
        }
    }
    return 0;
}
