// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.
#pragma once

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
