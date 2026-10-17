// Listener do app LegoToypad (HarrysofXD), Fase 27.
//
// O app e um cliente TCP: abre uma conexao com 127.0.0.1:<porta> (9191 por padrao, secao
// [Listener] do LegoToypad.ini) para cada acao. No shadPS4 quem atende e o ShadToypadBridge, que
// traduz para os comandos de IPC do emulador; aqui o proprio port atende. Protocolo (conferido no
// bridge descompilado):
//
//   cabecalho de 5 bytes: cmd, pad (1 = centro, 2 = esquerda, 3 = direita), indice (0..6),
//                         pad de origem, indice de origem
//   cmd 1 LOAD    + 180 bytes da tag (NTAG213, 45 paginas) + u16 tamanho + caminho UTF-8 do .bin
//                   (se o arquivo existir com >= 180 bytes, e ele que vale e recebe as escritas)
//   cmd 2 REMOVE
//   cmd 3 MOVE    da posicao de origem para (pad, indice)
//   cmd 4 GET_LED -> 30 bytes: 'L', contador, 3, e 9 bytes por pad
#include <winsock2.h>
#include <ws2tcpip.h>

#include <atomic>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "hle/overrides/hle_toypad.h"

#pragma comment(lib, "ws2_32.lib")

namespace hle {

void InjectToypadPacket(const toypad::Packet& packet); // hle_usbd.cpp
void TakeToypadExternalControl();                      // hle_usbd.cpp

namespace {

std::atomic<bool> g_started{false};
std::atomic<u64> g_requests{0};

bool RecvAll(SOCKET s, void* buffer, int length) {
    char* p = static_cast<char*>(buffer);
    for (int done = 0; done < length;) {
        const int n = recv(s, p + done, length - done, 0);
        if (n <= 0) {
            return false;
        }
        done += n;
    }
    return true;
}

void Announce(const std::vector<toypad::Packet>& packets) {
    if (!toypad::SessionReady()) {
        return; // o jogo ainda nao abriu a sessao: a vigia do portal anuncia tudo quando abrir
    }
    for (const toypad::Packet& packet : packets) {
        InjectToypadPacket(packet);
    }
}

void HandleClient(SOCKET s) {
    u8 header[5];
    if (!RecvAll(s, header, 5)) {
        return;
    }
    const u8 cmd = header[0], pad = header[1], index = header[2];
    ++g_requests;
    if (cmd == 4) { // GET_LED
        u8 snapshot[30];
        toypad::LedSnapshot(snapshot);
        send(s, reinterpret_cast<const char*>(snapshot), sizeof(snapshot), 0);
        return;
    }
    if (pad < 1 || pad > 3 || index >= toypad::kSlotCount) {
        std::printf("[TOYPAD-APP] pedido recusado: cmd %u pad %u indice %u\n", cmd, pad, index);
        return;
    }
    TakeToypadExternalControl();
    switch (cmd) {
    case 1: { // LOAD
        toypad::TagData tag{};
        u8 length_bytes[2];
        if (!RecvAll(s, tag.data(), static_cast<int>(tag.size())) || !RecvAll(s, length_bytes, 2)) {
            return;
        }
        const u32 length = length_bytes[0] | (length_bytes[1] << 8);
        std::string path(length, '\0');
        if (length && !RecvAll(s, path.data(), static_cast<int>(length))) {
            return;
        }
        bool from_file = false;
        if (!path.empty()) { // o arquivo da figura manda, se existir: e nele que o progresso fica gravado
            std::ifstream in(std::filesystem::u8path(path), std::ios::binary);
            toypad::TagData file{};
            if (in && in.read(reinterpret_cast<char*>(file.data()), file.size())) {
                tag = file;
                from_file = true;
            }
        }
        std::vector<toypad::Packet> events;
        if (const auto removed = toypad::RemoveIndex(index)) {
            events.push_back(*removed);
        }
        u32 id = 0;
        if (const auto added = toypad::LoadTag(pad, index, tag, from_file ? path : std::string(), &id)) {
            events.push_back(*added);
        }
        std::printf("[TOYPAD-APP] LOAD pad %u indice %u: id %u, uid %02x %02x %02x %02x %02x %02x %02x%s%s\n", pad, index, id, tag[0], tag[1],
                    tag[2], tag[4], tag[5], tag[6], tag[7], from_file ? ", arquivo " : "", from_file ? path.c_str() : "");
        Announce(events);
        break;
    }
    case 2: { // REMOVE
        std::vector<toypad::Packet> events;
        if (const auto removed = toypad::RemoveIndex(index)) {
            events.push_back(*removed);
        }
        std::printf("[TOYPAD-APP] REMOVE pad %u indice %u%s\n", pad, index, events.empty() ? " (ja estava vazio)" : "");
        Announce(events);
        break;
    }
    case 3: { // MOVE
        const u8 old_pad = header[3], old_index = header[4];
        if (old_pad < 1 || old_pad > 3 || old_index >= toypad::kSlotCount) {
            std::printf("[TOYPAD-APP] MOVE recusado: origem pad %u indice %u\n", old_pad, old_index);
            return;
        }
        const std::vector<toypad::Packet> events = toypad::MoveIndex(pad, index, old_index);
        std::printf("[TOYPAD-APP] MOVE %u/%u -> %u/%u%s\n", old_pad, old_index, pad, index, events.empty() ? " (origem vazia)" : "");
        Announce(events);
        break;
    }
    default:
        std::printf("[TOYPAD-APP] comando desconhecido 0x%02x\n", cmd);
        break;
    }
}

void ListenerThread(int port) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::printf("[TOYPAD-APP] WSAStartup falhou\n");
        return;
    }
    const SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<u_short>(port));
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // so a propria maquina
    if (listener == INVALID_SOCKET || bind(listener, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0 ||
        listen(listener, 4) != 0) {
        std::printf("[TOYPAD-APP] nao foi possivel escutar em 127.0.0.1:%d (erro %d). A porta esta em uso? (o ShadToypadBridge usa a "
                    "mesma)\n",
                    port, WSAGetLastError());
        if (listener != INVALID_SOCKET) {
            closesocket(listener);
        }
        return;
    }
    std::printf("[TOYPAD-APP] listener do LegoToypad ativo em 127.0.0.1:%d\n", port);
    for (;;) {
        const SOCKET client = accept(listener, nullptr, nullptr);
        if (client == INVALID_SOCKET) {
            break;
        }
        DWORD timeout_ms = 3000;
        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout_ms), sizeof(timeout_ms));
        HandleClient(client);
        shutdown(client, SD_SEND);
        closesocket(client);
    }
}

} // namespace

void StartToypadListener(int port) {
    if (port <= 0 || g_started.exchange(true)) {
        return;
    }
    std::thread(ListenerThread, port).detach();
}

u64 ToypadListenerRequests() {
    return g_requests.load();
}

} // namespace hle
