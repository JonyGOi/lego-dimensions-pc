#pragma once

#include <atomic>

#include "common/types.h"
#include "hle_stub.h"

// Log central do HLE. Tres tipos de linha:
//   [0001] STUB lib::nome ...   stub gerado, ainda sem implementacao
//   [0002] HLE  nome(...)       override implementado e relevante (HLE_TRACE)
//   [GAME] texto                saida de texto do proprio jogo (printf, puts, fwrite...)
// Overrides de alta frequencia usam HLE_QUIET: so contam a chamada, e so aparecem no log
// com --verbose-hle.
#include <string>

namespace hle::log {

extern bool g_verbose;

void Init(u64 max_lines, bool verbose);

// Logger instalado nos stubs gerados (hle::SetStubLogger).
void StubLogger(const StubCall& call);

void Call(const char* name, u64 return_address, const char* fmt, ...)
    __attribute__((format(printf, 3, 4)));

// Linha com etiqueta propria: "[PAD] ...", "[PORTAL] ...", "[GAME STATE] ...".
void Tagged(const char* tag, const char* fmt, ...) __attribute__((format(printf, 2, 3)));

// Linha sem prefixo (ex.: ">>> [RADAR DE TELA] ... <<<").
void Raw(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

// Acesso a arquivo: linha "[FS] ...".
void Fs(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

// Texto do jogo; agrupado por linha e impresso com o prefixo "[GAME] ". stream: 1=stdout 2=stderr
void GameText(int stream, const char* data, u64 size);
void FlushGameText();

// "nome=chamadas por frame" das `count` funcoes HLE mais chamadas desde a ultima consulta
// (contadores das funcoes silenciadas), para o relatorio de desempenho.
std::string TopCallDeltas(size_t count, u64 frames);
// Linhas suprimidas pelo limitador de repeticao (mais de 10 iguais por segundo).
u64 SuppressedLines();

void DumpHistory();
// Total de chamadas a stubs (funcoes sem implementacao) cujo "biblioteca::nome" comeca com
// `prefix`; `detail` recebe "nome=vezes" de cada uma.
u64 StubCallsWithPrefix(const char* prefix, std::string* detail);
// Stubs chamados (o que falta implementar) e contadores das funcoes silenciadas.
void PrintSummary();

[[noreturn]] void Shutdown(int exit_code);
// Erro irrecuperavel detectado pelo HLE (abort, pure virtual, excecao C++...): imprime a
// mensagem e dispara um breakpoint para o crash handler mostrar registradores e pilha.
[[noreturn]] void Fatal(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

struct QuietCounter {
    explicit QuietCounter(const char* name);
    void Hit() { count.fetch_add(1, std::memory_order_relaxed); }

    const char* name;
    std::atomic<u64> count{0};
    QuietCounter* next = nullptr;
};

} // namespace hle::log

// Dentro de um override "impl_<nome>": __func__ + 5 e o nome do simbolo do Orbis.
#define HLE_TRACE(fmt, ...)                                                                        \
    ::hle::log::Call(__func__ + 5, HLE_RETURN_ADDRESS(), fmt __VA_OPT__(, ) __VA_ARGS__)

#define HLE_QUIET(fmt, ...)                                                                        \
    do {                                                                                           \
        static ::hle::log::QuietCounter hle_quiet_counter_{__func__ + 5};                          \
        hle_quiet_counter_.Hit();                                                                  \
        if (::hle::log::g_verbose) {                                                               \
            HLE_TRACE(fmt __VA_OPT__(, ) __VA_ARGS__);                                             \
        }                                                                                          \
    } while (0)
