#include "hle/overrides/hle_override.h"

#include <cstdio>

#include "hle_symbol_table.h"

namespace hle {

namespace {
OverrideRegistrar* g_head = nullptr; // inicializado em tempo de compilacao, antes dos registrars
}

OverrideRegistrar::OverrideRegistrar(const char* name_, void* function_)
    : name(name_), function(function_), next(g_head) {
    g_head = this;
}

OverrideStats InstallOverrides() {
    OverrideStats stats;
    for (auto* reg = g_head; reg; reg = reg->next) {
        if (OverrideSymbol(reg->name, reg->function)) {
            ++stats.installed;
        } else {
            ++stats.not_imported;
            std::printf("AVISO: override '%s' nao corresponde a nenhum simbolo importado\n", reg->name);
        }
    }
    return stats;
}

} // namespace hle
