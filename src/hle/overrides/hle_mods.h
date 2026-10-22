#pragma once

#include <filesystem>

// Fase 64: carregador de mods. Aceita os dois tipos de mod de PS4 / shadPS4 do LEGO Dimensions:
//   - pacote da TT Games (.HDR + .DAT com outro nome, ex. AllWorld.DAT feito com o BrickVault);
//   - arquivos soltos no formato do /app0 (como a pasta CUSA01176-mods do shadPS4).
// Quem acha os mods e prepara tudo e o tools/setup/mods.py (o launcher roda antes de abrir o jogo): para cada
// arquivo que um mod troca, a entrada dele na tabela de todo pacote do jogo que o tem passa a apontar para os bytes
// do mod, postos depois do fim do .DAT. Aqui so se le o manifesto (assets\mods_cache\mods.txt) e se liga no VFS.
// Por que nao montar o pacote do mod: um pacote PATCH a mais com arquivos repetidos derruba o jogo (eboot+0xcd5dc4).
namespace hle::mods {

// Liga os mods do manifesto e imprime o resumo. Devolve quantos pacotes do jogo ganharam arquivos de mod mais
// quantas pastas de arquivos soltos entraram.
int Setup(const std::filesystem::path& manifest);

} // namespace hle::mods
