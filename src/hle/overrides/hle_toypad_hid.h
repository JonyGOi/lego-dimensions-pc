// Fase 61: Toy Pad REAL (o portal fisico 0e6f:0241 ligado no USB) pela API de HID do Windows. O portal e um
// aparelho HID (classe 3) com relatorios de 32 bytes sem id: o Windows ja o reconhece sozinho, sem driver extra
// (diferente da libusb do Cemu / RPCS3, que pede o WinUSB pelo Zadig - com o WinUSB instalado o portal deixa de ser
// HID e nao aparece aqui). Os comandos do jogo vao direto para o portal; o que o portal manda (respostas e avisos
// de figura) volta pela funcao registrada em Start.
#pragma once

#include <string>

#include "common/types.h"

namespace hle::toypadhid {

using PacketCallback = void (*)(const u8* packet, u32 length); // 32 bytes

// Procura e abre o primeiro Toy Pad real. true se abriu; `description` diz o que foi achado (ou por que nao).
bool Open(std::string* description);
// Comeca a ler o portal numa thread propria (cada relatorio vai para `callback`).
void Start(PacketCallback callback);
// Manda um comando de ate 32 bytes ao portal. false se o portal sumiu.
bool Write(const u8* data, u32 length);
bool IsOpen();

} // namespace hle::toypadhid
