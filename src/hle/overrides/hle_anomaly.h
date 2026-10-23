#pragma once

#include <string>
#include <vector>

#include "common/types.h"

// Fase 67: Anomaly, o carregador de mods do LEGO Dimensions de PS4 feito por connorh315
// (https://github.com/connorh315/anomaly, GPL-2.0-or-later), refeito dentro do port. No PS4 / shadPS4 ele e um
// .prx que se pendura em funcoes do eboot 1.24; aqui os mesmos ganchos sao postos direto no eboot carregado:
//   - antes de o jogo montar o PATCH, cada pacote de mod (/app0/mods/<nome>.HDR + .DAT) e aberto pela funcao do
//     proprio jogo e posto na frente da lista de pacotes: os arquivos do mod tem prioridade e arquivos novos valem;
//   - stuff/text/text.csv e chars/collection.txt de todos os pacotes sao lidos em camadas (os textos e as figuras
//     de cada mod se somam em vez de um apagar o outro);
//   - no menu principal, a entrada "trocar perfil" vira "Anomaly (N carregados)".
// O /app0/mods e virtual: o hle_filesystem liga cada nome ao arquivo da pasta de mods do port (mods.py / mods.txt).
namespace hle::anomaly {

// Nomes dos pacotes (sem extensao) que o jogo vai abrir como /app0/mods/<nome>, na ordem de prioridade.
void SetMods(std::vector<std::string> names);

// Poe os ganchos no eboot (antes do Protect). false = eboot que nao e o 1.24 ou funcao com outro comeco.
bool Install(u8* base);

// Quantos pacotes de mod o jogo abriu.
int Loaded();

} // namespace hle::anomaly
