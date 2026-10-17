// Temporizadores de alta precisao por funcao do HLE (Fase 14).
//
//   static hle::perf::Counter counter("nome");   // uma vez, no arquivo que mede
//   hle::perf::Scope scope(counter);             // no inicio do trecho medido
//
// hle::perf::PrintWindow() imprime, para a janela desde a ultima chamada, o tempo total e por
// frame de cada contador, e zera a janela. O custo de um Scope e de duas leituras de relogio.
#pragma once

#include <atomic>
#include <chrono>

#include "common/types.h"

namespace hle::perf {

struct Counter {
    explicit Counter(const char* name);

    const char* name;
    std::atomic<u64> ns{0};
    std::atomic<u64> calls{0};
    std::atomic<u64> max_ns{0};
    Counter* next = nullptr;
};

class Scope {
public:
    explicit Scope(Counter& counter) : counter_(counter), start_(std::chrono::steady_clock::now()) {}
    ~Scope() {
        const u64 ns = static_cast<u64>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start_).count());
        counter_.ns.fetch_add(ns, std::memory_order_relaxed);
        counter_.calls.fetch_add(1, std::memory_order_relaxed);
        u64 seen = counter_.max_ns.load(std::memory_order_relaxed);
        while (ns > seen && !counter_.max_ns.compare_exchange_weak(seen, ns, std::memory_order_relaxed)) {
        }
    }
    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;

private:
    Counter& counter_;
    std::chrono::steady_clock::time_point start_;
};

// Uma linha "[PERF]" por contador com atividade na janela de `frames` frames / `seconds` s.
void PrintWindow(double seconds, u64 frames);

} // namespace hle::perf
