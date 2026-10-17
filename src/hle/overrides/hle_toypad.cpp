#include "hle/overrides/hle_toypad.h"

#include <bit>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

#include "hle/hle_log.h"

namespace hle::toypad {

namespace {

// Chave TEA dos comandos do portal, como quatro palavras little-endian.
constexpr u32 kCommandKey[4] = {0xB0F6FE55, 0x410BBF62, 0xB47CB3C9, 0x7B293E97};
constexpr u32 kDelta = 0x9E3779B9;

// Constantes que entram, junto com o UID, na chave de cada figura e na senha da tag.
constexpr u8 kCharConstant[17] = {0xB7, 0xD5, 0xD7, 0xE6, 0xE7, 0xBA, 0x3C, 0xA8, 0xD8,
                                  0x75, 0x47, 0x68, 0xCF, 0x23, 0xE9, 0xFE, 0xAA};
constexpr char kPasswordText[] = "(c) Copyright LEGO 2014"; // 23 bytes, seguidos de AA AA

struct Figure {
    bool present = false;
    int pad = 0; // zona (1..3)
    Uid uid{};
    u32 id = 0;
    TagData data{};
    std::string path; // arquivo que recebe as escritas (figuras do app LegoToypad)
};

std::mutex g_mutex;
Stats g_stats;
u32 g_rng[4] = {}; // gerador do desafio, semeado pelo jogo no comando 0xB1
bool g_session_ready = false;
// Indice da figura no protocolo = posicao neste vetor (0..6). As figuras do portal.json usam o
// indice igual a zona (1..3), como desde a Fase 8; o app LegoToypad escolhe o indice.
Figure g_figures[kSlotCount];
u8 g_led[kPadCount][3] = {}; // ultima cor pedida pelo jogo em cada zona
u8 g_led_counter = 0;

u32 Load32(const u8* p) {
    return p[0] | (p[1] << 8) | (p[2] << 16) | (static_cast<u32>(p[3]) << 24);
}

void Store32(u8* p, u32 v) {
    p[0] = static_cast<u8>(v);
    p[1] = static_cast<u8>(v >> 8);
    p[2] = static_cast<u8>(v >> 16);
    p[3] = static_cast<u8>(v >> 24);
}

u32 NextRandom() {
    const u32 e = g_rng[0] - std::rotl(g_rng[1], 21);
    g_rng[0] = g_rng[1] ^ std::rotl(g_rng[2], 19);
    g_rng[1] = g_rng[2] + std::rotl(g_rng[3], 6);
    g_rng[2] = g_rng[3] + e;
    g_rng[3] = e + g_rng[0];
    return g_rng[3];
}

void SeedRandom(u32 seed) {
    g_rng[0] = 0xF1EA5EED;
    g_rng[1] = g_rng[2] = g_rng[3] = seed;
    for (int i = 0; i < 42; ++i) {
        NextRandom();
    }
}

// Mistura `words` palavras little-endian de `bytes` (hash usado na chave e na senha).
u32 Scramble(const u8* bytes, int words) {
    u32 value = 0;
    for (int i = 0; i < words; ++i) {
        value = Load32(bytes + i * 4) + std::rotr(value, 25) + std::rotr(value, 10) - value;
    }
    return value;
}

// 55 <len> <seq> <payload> <checksum>, resto zerado
Packet Reply(u8 sequence, const u8* payload, u32 payload_size) {
    Packet reply{};
    reply[0] = 0x55;
    reply[1] = static_cast<u8>(payload_size + 1);
    reply[2] = sequence;
    if (payload_size) {
        std::memcpy(&reply[3], payload, payload_size);
    }
    reply[3 + payload_size] = Checksum(reply.data(), 3 + payload_size);
    return reply;
}

Packet FigureEvent(int pad, int index, bool removed, const Uid& uid) {
    Packet event{};
    event[0] = 0x56;
    event[1] = 0x0B;
    event[2] = static_cast<u8>(pad);
    event[3] = 0x00;
    event[4] = static_cast<u8>(index); // indice da figura
    event[5] = removed ? 1 : 0;
    std::memcpy(&event[6], uid.data(), uid.size());
    event[13] = Checksum(event.data(), 13);
    return event;
}

std::string Hex(const u8* data, u32 count) {
    std::string out;
    char byte[4];
    for (u32 i = 0; i < count; ++i) {
        std::snprintf(byte, sizeof(byte), "%02x ", data[i]);
        out += byte;
    }
    return out;
}

// Figura pelo indice do protocolo (0..6), ou nullptr.
Figure* FigureAt(u8 index) {
    if (index >= kSlotCount || !g_figures[index].present) {
        return nullptr;
    }
    return &g_figures[index];
}

} // namespace

u8 Checksum(const u8* data, u32 count) {
    u32 sum = 0;
    for (u32 i = 0; i < count; ++i) {
        sum += data[i];
    }
    return static_cast<u8>(sum);
}

std::array<u8, 8> Encrypt(const u8* block, const u32* key) {
    const u32* k = key ? key : kCommandKey;
    u32 v0 = Load32(block), v1 = Load32(block + 4), sum = 0;
    for (int i = 0; i < 32; ++i) {
        sum += kDelta;
        v0 += ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        v1 += ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
    }
    std::array<u8, 8> out;
    Store32(out.data(), v0);
    Store32(out.data() + 4, v1);
    return out;
}

std::array<u8, 8> Decrypt(const u8* block, const u32* key) {
    const u32* k = key ? key : kCommandKey;
    u32 v0 = Load32(block), v1 = Load32(block + 4), sum = kDelta * 32;
    for (int i = 0; i < 32; ++i) {
        v1 -= ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        v0 -= ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        sum -= kDelta;
    }
    std::array<u8, 8> out;
    Store32(out.data(), v0);
    Store32(out.data() + 4, v1);
    return out;
}

std::array<u32, 4> FigureKey(const Uid& uid) {
    std::array<u32, 4> key;
    for (int i = 0; i < 4; ++i) {
        // UID + constante = 24 bytes; o ultimo byte da ultima palavra usada vira 0xAA.
        u8 base[24];
        std::memcpy(base, uid.data(), 7);
        std::memcpy(base + 7, kCharConstant, 17);
        const int words = 3 + i;
        base[words * 4 - 1] = 0xAA;
        key[i] = Scramble(base, words);
    }
    return key;
}

TagData BuildTag(const Uid& uid, u32 character_id) {
    TagData tag{};
    // Paginas 0-2: UID no formato NTAG (com os bytes de verificacao BCC0/BCC1)
    tag[0] = uid[0];
    tag[1] = uid[1];
    tag[2] = uid[2];
    tag[3] = static_cast<u8>(0x88 ^ uid[0] ^ uid[1] ^ uid[2]);
    std::memcpy(&tag[4], &uid[3], 4);
    tag[8] = static_cast<u8>(uid[3] ^ uid[4] ^ uid[5] ^ uid[6]);
    tag[9] = 0x48;
    // Pagina 3: capability container de um NTAG213
    tag[12] = 0xE1;
    tag[13] = 0x10;
    tag[14] = 0x12;

    if (character_id < 1000) {
        // Personagem: [id, id] cifrado com a chave da figura nas paginas 0x24-0x25
        u8 plain[8];
        Store32(plain, character_id);
        Store32(plain + 4, character_id);
        const auto key = FigureKey(uid);
        std::memcpy(&tag[0x24 * 4], Encrypt(plain, key.data()).data(), 8);
    } else {
        // Veiculo/gadget: id em claro na pagina 0x24 e marcador na 0x26
        Store32(&tag[0x24 * 4], character_id);
        tag[0x26 * 4 + 1] = 0x01;
    }

    // Pagina 0x2B: senha de leitura derivada do UID
    u8 base[32];
    std::memcpy(base, uid.data(), 7);
    std::memcpy(base + 7, kPasswordText, 23);
    base[30] = base[31] = 0xAA;
    Store32(&tag[0x2B * 4], Scramble(base, 8));
    return tag;
}

bool SessionReady() {
    std::lock_guard lock(g_mutex);
    return g_session_ready;
}

std::optional<Packet> PlaceFigure(int pad, const Uid& uid, u32 character_id) {
    std::lock_guard lock(g_mutex);
    if (pad < 1 || pad > kPadCount) {
        return std::nullopt;
    }
    Figure& figure = g_figures[pad];
    if (figure.present && figure.uid == uid && figure.id == character_id) {
        return std::nullopt;
    }
    figure.present = true;
    figure.pad = pad;
    figure.uid = uid;
    figure.id = character_id;
    figure.data = BuildTag(uid, character_id);
    figure.path.clear();
    ++g_stats.figures_placed;
    return FigureEvent(pad, pad, false, uid);
}

u32 TagCharacterId(const TagData& tag) {
    const Uid uid{tag[0], tag[1], tag[2], tag[4], tag[5], tag[6], tag[7]};
    if (tag[0x26 * 4 + 1] == 0x01) { // veiculo / gadget: id em claro
        return Load32(&tag[0x24 * 4]);
    }
    const auto key = FigureKey(uid);
    const auto plain = Decrypt(&tag[0x24 * 4], key.data());
    const u32 id = Load32(plain.data());
    return id == Load32(plain.data() + 4) ? id : 0;
}

std::optional<Packet> LoadTag(int pad, int index, const TagData& tag, const std::string& path, u32* character_id) {
    std::lock_guard lock(g_mutex);
    if (pad < 1 || pad > kPadCount || index < 0 || index >= kSlotCount) {
        return std::nullopt;
    }
    Figure& figure = g_figures[index];
    figure.present = true;
    figure.pad = pad;
    figure.uid = {tag[0], tag[1], tag[2], tag[4], tag[5], tag[6], tag[7]};
    figure.data = tag;
    figure.id = TagCharacterId(tag);
    figure.path = path;
    if (character_id) {
        *character_id = figure.id;
    }
    ++g_stats.figures_placed;
    return FigureEvent(pad, index, false, figure.uid);
}

std::optional<Packet> RemoveIndex(int index) {
    std::lock_guard lock(g_mutex);
    if (index < 0 || index >= kSlotCount || !g_figures[index].present) {
        return std::nullopt;
    }
    Figure& figure = g_figures[index];
    figure.present = false;
    ++g_stats.figures_removed;
    return FigureEvent(figure.pad, index, true, figure.uid);
}

std::vector<Packet> MoveIndex(int new_pad, int new_index, int old_index) {
    std::lock_guard lock(g_mutex);
    std::vector<Packet> events;
    if (new_pad < 1 || new_pad > kPadCount || new_index < 0 || new_index >= kSlotCount || old_index < 0 || old_index >= kSlotCount ||
        !g_figures[old_index].present) {
        return events;
    }
    Figure moved = g_figures[old_index];
    events.push_back(FigureEvent(moved.pad, old_index, true, moved.uid));
    g_figures[old_index].present = false;
    if (new_index != old_index && g_figures[new_index].present) { // destino ocupado: sai o que estava la
        events.push_back(FigureEvent(g_figures[new_index].pad, new_index, true, g_figures[new_index].uid));
    }
    moved.pad = new_pad;
    g_figures[new_index] = moved;
    events.push_back(FigureEvent(new_pad, new_index, false, moved.uid));
    return events;
}

std::vector<Packet> PresentEvents() {
    std::lock_guard lock(g_mutex);
    std::vector<Packet> events;
    for (int index = 0; index < kSlotCount; ++index) {
        if (g_figures[index].present) {
            events.push_back(FigureEvent(g_figures[index].pad, index, false, g_figures[index].uid));
        }
    }
    return events;
}

void LedSnapshot(u8 out[30]) {
    std::lock_guard lock(g_mutex);
    std::memset(out, 0, 30);
    out[0] = 'L';
    out[1] = g_led_counter;
    out[2] = 3;
    for (int pad = 0; pad < kPadCount; ++pad) { // 9 bytes por zona; os 3 primeiros levam a cor atual
        std::memcpy(out + 3 + pad * 9, g_led[pad], 3);
    }
}

std::optional<Packet> RemoveFigure(int pad) {
    std::lock_guard lock(g_mutex);
    if (pad < 1 || pad > kPadCount || !g_figures[pad].present) {
        return std::nullopt;
    }
    Figure& figure = g_figures[pad];
    figure.present = false;
    ++g_stats.figures_removed;
    return FigureEvent(figure.pad, pad, true, figure.uid);
}

std::optional<Packet> HandleCommand(const u8* command, u32 length) {
    if (!command || length < 5 || command[0] != 0x55) {
        return std::nullopt;
    }
    const u8 len = command[1];
    const u8 cmd = command[2];
    const u8 sequence = command[3];
    const u8* payload = command + 4;
    if (len < 2 || 2u + len >= length) {
        return std::nullopt;
    }
    const bool checksum_ok = Checksum(command, 2 + len) == command[2 + len];

    std::lock_guard lock(g_mutex);
    ++g_stats.commands;
    const char* name = "?";
    Packet reply{};
    bool quiet = false;

    switch (cmd) {
    case 0xB0: { // ativacao: o portal devolve o mesmo texto de copyright
        name = "WAKE";
        ++g_stats.wake;
        g_session_ready = false; // o jogo recomeca a sessao (e reanuncia as figuras depois)
        static const char kText[] = "(c) LEGO 2014";
        reply = Reply(sequence, reinterpret_cast<const u8*>(kText), 13);
        break;
    }
    case 0xB1: { // semente do gerador: [seed LE][confirmacao], cifrado
        name = "SEED";
        ++g_stats.seed;
        const auto plain = Decrypt(payload);
        SeedRandom(Load32(plain.data()));
        const u8 answer[8] = {plain[4], plain[5], plain[6], plain[7], 0, 0, 0, 0};
        reply = Reply(sequence, Encrypt(answer).data(), 8);
        break;
    }
    case 0xB3: { // desafio: responde [proximo aleatorio LE][confirmacao], cifrado
        name = "CHALLENGE";
        ++g_stats.challenge;
        const auto plain = Decrypt(payload);
        u8 answer[8];
        Store32(answer, NextRandom());
        std::memcpy(answer + 4, plain.data(), 4);
        reply = Reply(sequence, Encrypt(answer).data(), 8);
        g_session_ready = true;
        break;
    }
    case 0xC0: // cor de um pad
    case 0xC1: // consulta de cor
    case 0xC2: // fade
    case 0xC3: // flash
    case 0xC4: // fade aleatorio
    case 0xC6: // fade em todos
    case 0xC7: // flash em todos
    case 0xC8: // cor em todos
        name = "LED";
        quiet = ++g_stats.led > 4; // chegam aos montes
        ++g_led_counter;
        if (cmd == 0xC0 && len >= 6) { // [zona (0 = todas)][r][g][b]
            for (int pad = 0; pad < kPadCount; ++pad) {
                if (payload[0] == 0 || payload[0] == pad + 1) {
                    std::memcpy(g_led[pad], payload + 1, 3);
                }
            }
        } else if (cmd == 0xC8 && len >= 14) { // por zona: [ligado][r][g][b]
            for (int pad = 0; pad < kPadCount; ++pad) {
                if (payload[pad * 4]) {
                    std::memcpy(g_led[pad], payload + pad * 4 + 1, 3);
                }
            }
        }
        reply = Reply(sequence, nullptr, 0);
        break;
    case 0xD2: { // leitura: [indice][pagina] -> status + 4 paginas (16 bytes)
        name = "READ";
        ++g_stats.read;
        u8 body[17] = {};
        const Figure* figure = FigureAt(payload[0]);
        const u32 offset = payload[1] * 4u;
        if (figure && offset + 16 <= figure->data.size()) {
            std::memcpy(body + 1, figure->data.data() + offset, 16);
        }
        reply = Reply(sequence, body, 17);
        break;
    }
    case 0xD3: { // escrita: [indice][pagina][4 bytes]
        name = "WRITE";
        ++g_stats.write;
        Figure* figure = FigureAt(payload[0]);
        if (figure && payload[1] < kTagPages) {
            std::memcpy(figure->data.data() + payload[1] * 4u, payload + 2, 4);
            if (!figure->path.empty()) { // figura do app: o progresso volta para o arquivo dela
                std::ofstream out(std::filesystem::u8path(figure->path), std::ios::binary | std::ios::in | std::ios::out);
                if (out) {
                    out.write(reinterpret_cast<const char*>(figure->data.data()), static_cast<std::streamsize>(figure->data.size()));
                }
            }
        }
        const u8 status = 0;
        reply = Reply(sequence, &status, 1);
        break;
    }
    case 0xD4: { // modelo: [indice][...][confirmacao] cifrado -> [id LE][confirmacao] cifrado
        name = "MODEL";
        ++g_stats.model;
        const auto plain = Decrypt(payload);
        const Figure* figure = FigureAt(plain[0]);
        u8 answer[8] = {};
        if (figure) {
            Store32(answer, figure->id);
            std::memcpy(answer + 4, plain.data() + 4, 4);
        }
        u8 body[9] = {0};
        std::memcpy(body + 1, Encrypt(answer).data(), 8);
        reply = Reply(sequence, body, 9);
        break;
    }
    default: // 0xD0 lista de tags, 0xE1 senha, 0xE5 ativo, 0xFF consulta de LEDs...
        name = "NAO IMPLEMENTADO";
        ++g_stats.unknown;
        reply = Reply(sequence, nullptr, 0);
        break;
    }

    if (!quiet) {
        log::Tagged("PORTAL", "cmd 0x%02X %s seq=%u%s: %s-> %s", cmd, name, sequence,
                    checksum_ok ? "" : " (checksum invalido)", Hex(command, 3u + len).c_str(),
                    Hex(reply.data(), 3u + reply[1]).c_str());
    }
    return reply;
}

Stats GetStats() {
    std::lock_guard lock(g_mutex);
    return g_stats;
}

} // namespace hle::toypad
