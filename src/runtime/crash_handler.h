#pragma once

#include <atomic>
#include <string>

#include "common/types.h"

namespace runtime {

// Informa onde esta a imagem do jogo, para traduzir enderecos em "eboot+0x...".
void SetGuestImage(u64 base, u64 image_size, u64 reserve_size);

// "eboot+0x470df0" quando o endereco cai dentro da imagem; senao o valor em hexadecimal.
std::string DescribeAddress(u64 address);

// Instala o Vectored Exception Handler: na primeira excecao fatal imprime registradores,
// a instrucao exata (Zydis), a pilha e as ultimas chamadas HLE, e encerra o processo.
void InstallCrashHandler();

// Threads que executam codigo do jogo. O registro alimenta o nome nas linhas de log, o crash
// dump e o relatorio do watchdog.
struct GuestThread {
    char name[32];
    void* handle;
    unsigned long thread_id;
    std::atomic<const char*> waiting{nullptr}; // chamada HLE bloqueante em andamento
    std::atomic<u64> waits_completed{0};       // quantas esperas bloqueantes ja terminaram
    std::atomic<bool> finished{false};
};

GuestThread* RegisterCurrentThread(const char* name);
GuestThread* CurrentThread();
const char* CurrentThreadName();
// Threads do jogo que ainda nao terminaram.
int AliveGuestThreads();

// Marca a thread atual como bloqueada em `what` enquanto o objeto existir.
class WaitScope {
public:
    explicit WaitScope(const char* what);
    ~WaitScope();

private:
    GuestThread* thread_;
    const char* previous_ = nullptr;
};

// Estado de todas as threads do jogo: onde estao e em que chamada estao bloqueadas.
void DumpAllThreads(const char* reason);

// Amostrador de CPU: durante `seconds`, a cada ~4 ms, suspende cada thread do jogo so o tempo
// de copiar RIP e os primeiros enderecos de retorno, e no fim imprime por thread quanto tempo
// ela passou executando (fora de esperas do HLE) e os lugares onde mais foi vista.
void SampleThreads(double seconds, const char* reason);

// Fase 59: perfil com a pilha inteira do host (RtlVirtualUnwind) de UMA thread do jogo, ~1 kHz.
// Imprime o tempo inclusivo (a funcao ou algo que ela chamou) e exclusivo (a propria funcao).
void ProfileThreadDeep(const char* thread_name, double seconds, const char* reason);

// Fase 59: uma linha por thread do jogo com a pilha do host naquele instante (travadas curtas).
void DumpStallStacks(const char* reason);

// Enquanto existir (na thread atual), uma violacao de acesso nao derruba o processo pelo crash
// handler: segue para o __try/__except de quem esta lendo memoria que pode ter sido desmapeada.
// Chama codigo gerado (ABI System V: rdi, rsi) que le ponteiros do jogo. Uma violacao de acesso la
// dentro nao derruba o processo: a execucao volta para ca e a funcao devolve false. (O codigo
// gerado nao tem tabela de unwind, entao __try nao serve.)
bool CallGuardedWalker(void* function, const u32* user_data, u32* flat);

struct GuardedReadScope {
    GuardedReadScope();
    ~GuardedReadScope();
};

} // namespace runtime
