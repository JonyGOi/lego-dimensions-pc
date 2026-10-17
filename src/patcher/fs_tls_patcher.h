#pragma once

#include <utility>
#include <vector>

#include "common/types.h"

namespace patcher {

struct FsPatchStats {
    u64 candidates = 0; // bytes 0x64 que decodificaram como instrucao com operando em FS
    u64 patched = 0;
    std::vector<u64> patched_samples; // primeiros enderecos, para conferencia

    // "mov r64, fs:[disp32]" com disp != 0: mesma codificacao de 9 bytes da forma reescrita,
    // entao um achado aqui e codigo de verdade (nao da para bater por acaso em dados).
    u64 absolute_nonzero = 0;
    std::vector<std::pair<u64, s64>> absolute_nonzero_samples; // (offset, disp)

    // Demais formas (fs:[reg], fs:[reg+disp], outros mnemonicos). Contagem NAO confiavel:
    // inclui bytes 0x64 de .rodata e de meio de instrucao. A garantia real e o VEH, que avisa
    // se o jogo executar qualquer instrucao com FS.
    u64 other_forms = 0;
};

// Procura em [code, code + size) leituras do thread pointer do Orbis
//     mov r64, qword ptr fs:[0]
// e reescreve no lugar para
//     mov r64, qword ptr gs:[gs_offset]
// As duas formas tem o mesmo tamanho: troca-se o prefixo 0x64 por 0x65 e o deslocamento.
// A regiao precisa estar com permissao de escrita.
FsPatchStats PatchFsThreadPointer(u8* code, u64 size, u32 gs_offset);

} // namespace patcher
