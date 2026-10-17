#!/usr/bin/env python3
"""
Fase 2.1 - Gerador de stubs HLE em C++20.

Le FASE1_imports.json (e FASE2_nids.json, se existir, para nomes e tamanhos) e gera em
src/hle/generated/:

  hle_stub.h             tipos, macros de ABI e a declaracao do logger
  hle_stub.cpp           logger padrao (stderr), substituivel via hle::SetStubLogger
  <biblioteca>.cpp       um por biblioteca: stubs sysv_abi + tabela local de simbolos
  hle_symbol_table.h/cpp tabela central NID -> ponteiro e aliases de modulo
  sources.cmake          lista de fontes para o CMake

Tudo em src/hle/generated/ e sobrescrito a cada execucao: implementacoes reais devem ir em
outro diretorio e substituir a entrada na tabela.
"""
from __future__ import annotations

import json
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FASE1 = ROOT / "FASE1_imports.json"
FASE2 = ROOT / "FASE2_nids.json"
OUT = ROOT / "src" / "hle" / "generated"

# Variantes de debug do SDK que o eboot pede em DT_NEEDED -> modulo retail equivalente.
MODULE_ALIASES = {
    "libSceGnmDriver_padebug": "libSceGnmDriver",
}

HEADER = "// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.\n"


def canonical(name: str) -> str:
    return MODULE_ALIASES.get(name, name)


def ident(s: str) -> str:
    return re.sub(r"\W", "_", s)


def nid_ident(nid: str) -> str:
    return "nid_" + nid.replace("+", "_p").replace("-", "_m")


def cstr(s: str) -> str:
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


HLE_STUB_H = HEADER + r"""#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

#if defined(_MSC_VER)
#include <intrin.h>
#define HLE_RETURN_ADDRESS() (reinterpret_cast<::hle::u64>(_ReturnAddress()))
#define HLE_NOINLINE __declspec(noinline)
#else
#define HLE_RETURN_ADDRESS() (reinterpret_cast<::hle::u64>(__builtin_return_address(0)))
#define HLE_NOINLINE __attribute__((noinline))
#endif

// O codigo do jogo chama com a ABI System V (RDI, RSI, RDX, RCX, R8, R9), mesmo no Windows.
// Exige Clang/clang-cl ou GCC: o MSVC puro nao tem sysv_abi.
#define HLE_SYSV_ABI __attribute__((sysv_abi))

namespace hle {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

enum class SymbolKind : u8 {
    Function, // stub chamavel
    Object,   // dado exportado (buffer zerado do tamanho original)
};

struct HleSymbol {
    std::string_view nid;
    std::string_view name;    // nome resolvido; igual ao NID quando desconhecido
    std::string_view library; // DT_SCE_IMPORT_LIB
    std::string_view module;  // DT_SCE_NEEDED_MODULE (ja sem sufixo de debug)
    SymbolKind kind;
    void* address;
    u32 size;                // so para Object
    bool overridden = false; // true quando src/hle/overrides/ trocou o stub por uma implementacao
};

struct StubCall {
    const char* library;
    const char* name;
    const char* nid;
    u64 return_address;
    u64 rdi, rsi, rdx, rcx, r8, r9;
};

using StubLogger = void (*)(const StubCall&);

// Troca o destino do log dos stubs (padrao: stderr). nullptr restaura o padrao.
void SetStubLogger(StubLogger logger);
void LogStubCall(const StubCall& call);

} // namespace hle
"""

HLE_STUB_CPP = HEADER + r"""#include "hle_stub.h"

#include <atomic>
#include <cinttypes>
#include <cstdio>

namespace hle {
namespace {

void DefaultLogger(const StubCall& c) {
    std::fprintf(stderr,
                 "[HLE STUB] %s::%s (nid %s) ret=0x%016" PRIx64 " rdi=0x%" PRIx64 " rsi=0x%" PRIx64
                 " rdx=0x%" PRIx64 " rcx=0x%" PRIx64 " r8=0x%" PRIx64 " r9=0x%" PRIx64 "\n",
                 c.library, c.name, c.nid, c.return_address, c.rdi, c.rsi, c.rdx, c.rcx, c.r8,
                 c.r9);
}

std::atomic<StubLogger> g_logger{&DefaultLogger};

} // namespace

void SetStubLogger(StubLogger logger) {
    g_logger.store(logger ? logger : &DefaultLogger, std::memory_order_relaxed);
}

void LogStubCall(const StubCall& call) {
    g_logger.load(std::memory_order_relaxed)(call);
}

} // namespace hle
"""

TABLE_H = HEADER + r"""#pragma once

#include <span>
#include <string_view>
#include <unordered_map>

#include "hle_stub.h"

namespace hle {

struct ModuleAlias {
    std::string_view needed;    // nome em DT_NEEDED, sem extensao
    std::string_view canonical; // modulo que realmente fornece os simbolos
};

// Todos os simbolos gerados, agrupados por biblioteca.
std::span<const HleSymbol> Symbols();

const HleSymbol* FindSymbolByName(std::string_view name);

// Troca o ponteiro de um simbolo por uma implementacao real (src/hle/overrides/). Precisa ser
// chamado antes de o loader aplicar as relocacoes. Retorna false se o eboot nao importa `name`.
bool OverrideSymbol(std::string_view name, void* address);

// NID -> simbolo. Os NIDs importados pelo eboot sao unicos entre bibliotecas.
const std::unordered_map<std::string_view, const HleSymbol*>& SymbolMap();

const HleSymbol* FindSymbol(std::string_view nid);

// Atalho para o linker: endereco a gravar na GOT/relocacao, ou nullptr se nao houver stub.
void* ResolveNid(std::string_view nid);

// DT_NEEDED de debug -> modulo retail (ex.: libSceGnmDriver_padebug -> libSceGnmDriver).
std::span<const ModuleAlias> ModuleAliases();
std::string_view CanonicalModule(std::string_view needed);

} // namespace hle
"""


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    fase1 = json.loads(FASE1.read_text(encoding="utf-8"))
    src = json.loads(FASE2.read_text(encoding="utf-8"))["imports"] if FASE2.is_file() else fase1["imports"]
    if not FASE2.is_file():
        print("AVISO: FASE2_nids.json ausente - stubs serao nomeados pelo NID (rode resolve_nids.py)")

    nids = [s["nid"] for s in src]
    if len(set(nids)) != len(nids):
        dup = sorted({n for n in nids if nids.count(n) > 1})
        print(f"ERRO: NIDs repetidos entre bibliotecas, a tabela NID -> ponteiro seria ambigua: {dup}")
        return 1

    # aliases realmente usados pelo eboot
    aliases = []
    for n in fase1["needed"]:
        stem = n["name"].rsplit(".", 1)[0]
        if canonical(stem) != stem:
            aliases.append((stem, canonical(stem)))

    libs: dict[str, list[dict]] = defaultdict(list)
    for s in src:
        libs[canonical(s["library"])].append({**s, "module": canonical(s["module"])})

    OUT.mkdir(parents=True, exist_ok=True)
    for old in OUT.iterdir():
        if old.is_file() and old.suffix in (".cpp", ".h", ".cmake"):
            old.unlink()

    (OUT / "hle_stub.h").write_text(HLE_STUB_H, encoding="utf-8", newline="\n")
    (OUT / "hle_stub.cpp").write_text(HLE_STUB_CPP, encoding="utf-8", newline="\n")
    (OUT / "hle_symbol_table.h").write_text(TABLE_H, encoding="utf-8", newline="\n")

    n_func = n_obj = n_named = 0
    lib_order = sorted(libs, key=str.lower)
    for lib in lib_order:
        syms = libs[lib]
        modules = sorted({s["module"] for s in syms})
        o: list[str] = [HEADER]
        o.append(f"// Biblioteca: {lib}   Modulo: {', '.join(modules)}   Simbolos: {len(syms)}")
        for needed, canon in aliases:
            if canon in modules:
                o.append(f"// O eboot pede este modulo como {needed}.prx (variante de debug do SDK).")
        o.append("")
        o.append('#include "hle_stub.h"')
        o.append("")
        o.append(f"namespace hle::gen::{ident(lib)} {{")
        o.append("")
        o.append(f"static constexpr const char* kLibrary = {cstr(lib)};")
        o.append("")
        rows = []
        used: set[str] = set()
        for s in syms:
            nid, name = s["nid"], s.get("name")
            n_named += name is not None
            base = ident(name) if name else nid_ident(nid)
            if base in used:
                base += "_" + nid_ident(nid)
            used.add(base)
            shown = name or nid
            if s["type"] == "FUNC":
                n_func += 1
                fn = f"stub_{base}"
                o.append(f"// {shown}  [{nid}]")
                o.append(f"HLE_NOINLINE u64 HLE_SYSV_ABI {fn}(u64 rdi, u64 rsi, u64 rdx, u64 rcx, u64 r8, u64 r9) {{")
                o.append(f"    LogStubCall({{kLibrary, {cstr(shown)}, {cstr(nid)}, HLE_RETURN_ADDRESS(),")
                o.append("                 rdi, rsi, rdx, rcx, r8, r9});")
                o.append("    return 0; // SCE_OK")
                o.append("}")
                o.append("")
                rows.append((nid, shown, s["module"], "Function", f"reinterpret_cast<void*>(&{fn})", 0))
            else:
                n_obj += 1
                size = int(s.get("size") or 0) or 8
                var = f"obj_{base}"
                o.append(f"// {shown}  [{nid}]  {s['type']}: dado, nao funcao. Buffer zerado de {size} bytes.")
                o.append(f"alignas(16) u8 {var}[{size}] = {{}};")
                o.append("")
                rows.append((nid, shown, s["module"], "Object", f"static_cast<void*>({var})", size))
        o.append("extern const HleSymbol kSymbols[] = {")
        for nid, shown, mod, kind, addr, size in rows:
            o.append(f"    {{{cstr(nid)}, {cstr(shown)}, kLibrary, {cstr(mod)}, SymbolKind::{kind}, {addr}, {size}}},")
        o.append("};")
        o.append(f"extern const std::size_t kSymbolCount = {len(rows)};")
        o.append("")
        o.append(f"}} // namespace hle::gen::{ident(lib)}")
        o.append("")
        (OUT / f"{lib}.cpp").write_text("\n".join(o), encoding="utf-8", newline="\n")

    # ---- tabela central ---------------------------------------------------
    t: list[str] = [HEADER]
    t.append('#include "hle_symbol_table.h"')
    t.append("")
    t.append("#include <vector>")
    t.append("")
    t.append("namespace hle {")
    t.append("namespace gen {")
    for lib in lib_order:
        t.append(f"namespace {ident(lib)} {{ extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }}")
    t.append("} // namespace gen")
    t.append("")
    t.append("namespace {")
    t.append("")
    t.append("struct LibraryTable {")
    t.append("    const HleSymbol* symbols;")
    t.append("    std::size_t count;")
    t.append("};")
    t.append("")
    t.append("const LibraryTable kLibraries[] = {")
    for lib in lib_order:
        t.append(f"    {{gen::{ident(lib)}::kSymbols, gen::{ident(lib)}::kSymbolCount}},")
    t.append("};")
    t.append("")
    t.append("constexpr ModuleAlias kModuleAliases[] = {")
    for needed, canon in aliases:
        t.append(f"    {{{cstr(needed)}, {cstr(canon)}}},")
    if not aliases:
        t.append('    {"", ""},')
    t.append("};")
    t.append("")
    t.append("std::vector<HleSymbol>& AllSymbols() {")
    t.append("    static std::vector<HleSymbol> all = [] {")
    t.append("        std::vector<HleSymbol> v;")
    t.append(f"        v.reserve({len(src)});")
    t.append("        for (const auto& lib : kLibraries) {")
    t.append("            v.insert(v.end(), lib.symbols, lib.symbols + lib.count);")
    t.append("        }")
    t.append("        return v;")
    t.append("    }();")
    t.append("    return all;")
    t.append("}")
    t.append("")
    t.append("} // namespace")
    t.append("")
    t.append("std::span<const HleSymbol> Symbols() {")
    t.append("    return AllSymbols();")
    t.append("}")
    t.append("")
    t.append("const std::unordered_map<std::string_view, const HleSymbol*>& SymbolMap() {")
    t.append("    static const std::unordered_map<std::string_view, const HleSymbol*> map = [] {")
    t.append("        std::unordered_map<std::string_view, const HleSymbol*> m;")
    t.append("        m.reserve(AllSymbols().size());")
    t.append("        for (const auto& sym : AllSymbols()) {")
    t.append("            m.emplace(sym.nid, &sym);")
    t.append("        }")
    t.append("        return m;")
    t.append("    }();")
    t.append("    return map;")
    t.append("}")
    t.append("")
    t.append("const HleSymbol* FindSymbolByName(std::string_view name) {")
    t.append("    for (const auto& sym : AllSymbols()) {")
    t.append("        if (sym.name == name) {")
    t.append("            return &sym;")
    t.append("        }")
    t.append("    }")
    t.append("    return nullptr;")
    t.append("}")
    t.append("")
    t.append("bool OverrideSymbol(std::string_view name, void* address) {")
    t.append("    for (auto& sym : AllSymbols()) {")
    t.append("        if (sym.name == name) {")
    t.append("            sym.address = address;")
    t.append("            sym.overridden = true;")
    t.append("            return true;")
    t.append("        }")
    t.append("    }")
    t.append("    return false;")
    t.append("}")
    t.append("")
    t.append("const HleSymbol* FindSymbol(std::string_view nid) {")
    t.append("    const auto& map = SymbolMap();")
    t.append("    const auto it = map.find(nid);")
    t.append("    return it != map.end() ? it->second : nullptr;")
    t.append("}")
    t.append("")
    t.append("void* ResolveNid(std::string_view nid) {")
    t.append("    const HleSymbol* sym = FindSymbol(nid);")
    t.append("    return sym ? sym->address : nullptr;")
    t.append("}")
    t.append("")
    t.append("std::span<const ModuleAlias> ModuleAliases() {")
    t.append(f"    return std::span<const ModuleAlias>(kModuleAliases, {len(aliases)});")
    t.append("}")
    t.append("")
    t.append("std::string_view CanonicalModule(std::string_view needed) {")
    t.append("    for (const auto& alias : ModuleAliases()) {")
    t.append("        if (alias.needed == needed) {")
    t.append("            return alias.canonical;")
    t.append("        }")
    t.append("    }")
    t.append("    return needed;")
    t.append("}")
    t.append("")
    t.append("} // namespace hle")
    t.append("")
    (OUT / "hle_symbol_table.cpp").write_text("\n".join(t), encoding="utf-8", newline="\n")

    cm = ["# GERADO por tools/gen_hle_stubs.py - NAO EDITAR.", "set(HLE_GENERATED_SOURCES"]
    cm += [f"    ${{CMAKE_CURRENT_LIST_DIR}}/{f}" for f in
           ["hle_stub.cpp", "hle_symbol_table.cpp"] + [f"{lib}.cpp" for lib in lib_order]]
    cm.append(")")
    cm.append("")
    (OUT / "sources.cmake").write_text("\n".join(cm), encoding="utf-8", newline="\n")

    print(f"Bibliotecas : {len(lib_order)}")
    print(f"Stubs       : {n_func} funcoes + {n_obj} objetos de dados = {n_func + n_obj}")
    print(f"Com nome    : {n_named} de {len(src)}")
    print(f"Aliases     : {aliases}")
    print(f"Saida       : {OUT}  ({len(list(OUT.iterdir()))} arquivos)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
