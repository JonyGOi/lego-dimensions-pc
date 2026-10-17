// libc.prx (Dinkumware) - os 108 simbolos de funcao que o eboot importa. A parte de stdio
// (FILE, printf, scanf) fica em hle_libc_stdio.cpp.
//
// Lembretes de ABI: o jogo e LP64 (long = 64 bits) e toda chamada de volta para o jogo
// (comparador de qsort, handlers) precisa de ponteiro de funcao sysv_abi.
#include <algorithm>
#include <atomic>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <utility>
#include <malloc.h>
#include <vector>

#include <windows.h>

#include "hle/overrides/hle_override.h"
#include "runtime/crash_handler.h"

namespace {

using namespace hle;

// ============================ memoria e strings ============================
HLE_IMPL(void*, memcpy, void* dst, const void* src, u64 n) {
    HLE_QUIET("%p <- %p, %" PRIu64, dst, src, n);
    return std::memcpy(dst, src, n);
}
HLE_OVERRIDE(memcpy);

HLE_IMPL(void*, memmove, void* dst, const void* src, u64 n) {
    HLE_QUIET("%p <- %p, %" PRIu64, dst, src, n);
    return std::memmove(dst, src, n);
}
HLE_OVERRIDE(memmove);

HLE_IMPL(void*, memset, void* dst, int value, u64 n) {
    HLE_QUIET("%p, 0x%02x, %" PRIu64, dst, value & 0xFF, n);
    return std::memset(dst, value, n);
}
HLE_OVERRIDE(memset);

HLE_IMPL(int, memcmp, const void* a, const void* b, u64 n) {
    HLE_QUIET("%p, %p, %" PRIu64, a, b, n);
    return std::memcmp(a, b, n);
}
HLE_OVERRIDE(memcmp);

HLE_IMPL(void*, memchr, const void* p, int c, u64 n) {
    HLE_QUIET("%p, %d, %" PRIu64, p, c, n);
    return const_cast<void*>(std::memchr(p, c, n));
}
HLE_OVERRIDE(memchr);

HLE_IMPL(int, memcpy_s, void* dst, u64 dst_size, const void* src, u64 n) {
    HLE_QUIET("%p[%" PRIu64 "] <- %p, %" PRIu64, dst, dst_size, src, n);
    if (!dst || !src || n > dst_size) {
        if (dst && dst_size) {
            std::memset(dst, 0, dst_size);
        }
        return 22; // EINVAL
    }
    std::memcpy(dst, src, n);
    return 0;
}
HLE_OVERRIDE(memcpy_s);

HLE_IMPL(u64, strlen, const char* s) {
    HLE_QUIET("%p", static_cast<const void*>(s));
    return std::strlen(s);
}
HLE_OVERRIDE(strlen);

HLE_IMPL(u64, strnlen_s, const char* s, u64 max) {
    HLE_QUIET("%p, %" PRIu64, static_cast<const void*>(s), max);
    return s ? strnlen(s, max) : 0;
}
HLE_OVERRIDE(strnlen_s);

HLE_IMPL(char*, strcpy, char* dst, const char* src) {
    HLE_QUIET("%p <- \"%.48s\"", static_cast<void*>(dst), src);
    return std::strcpy(dst, src);
}
HLE_OVERRIDE(strcpy);

HLE_IMPL(char*, strncpy, char* dst, const char* src, u64 n) {
    HLE_QUIET("%p <- %p, %" PRIu64, static_cast<void*>(dst), static_cast<const void*>(src), n);
    return std::strncpy(dst, src, n);
}
HLE_OVERRIDE(strncpy);

HLE_IMPL(char*, strcat, char* dst, const char* src) {
    HLE_QUIET("%p += \"%.48s\"", static_cast<void*>(dst), src);
    return std::strcat(dst, src);
}
HLE_OVERRIDE(strcat);

HLE_IMPL(char*, strncat, char* dst, const char* src, u64 n) {
    HLE_QUIET("%p += %p, %" PRIu64, static_cast<void*>(dst), static_cast<const void*>(src), n);
    return std::strncat(dst, src, n);
}
HLE_OVERRIDE(strncat);

HLE_IMPL(int, strcmp, const char* a, const char* b) {
    HLE_QUIET("\"%.32s\", \"%.32s\"", a, b);
    return std::strcmp(a, b);
}
HLE_OVERRIDE(strcmp);

HLE_IMPL(int, strncmp, const char* a, const char* b, u64 n) {
    HLE_QUIET("%p, %p, %" PRIu64, static_cast<const void*>(a), static_cast<const void*>(b), n);
    return std::strncmp(a, b, n);
}
HLE_OVERRIDE(strncmp);

HLE_IMPL(char*, strchr, const char* s, int c) {
    HLE_QUIET("%p, '%c'", static_cast<const void*>(s), c);
    return const_cast<char*>(std::strchr(s, c));
}
HLE_OVERRIDE(strchr);

HLE_IMPL(char*, strrchr, const char* s, int c) {
    HLE_QUIET("%p, '%c'", static_cast<const void*>(s), c);
    return const_cast<char*>(std::strrchr(s, c));
}
HLE_OVERRIDE(strrchr);

HLE_IMPL(char*, strstr, const char* haystack, const char* needle) {
    HLE_QUIET("%p, \"%.32s\"", static_cast<const void*>(haystack), needle);
    return const_cast<char*>(std::strstr(haystack, needle));
}
HLE_OVERRIDE(strstr);

HLE_IMPL(u64, strspn, const char* s, const char* accept) {
    HLE_QUIET("%p", static_cast<const void*>(s));
    return std::strspn(s, accept);
}
HLE_OVERRIDE(strspn);

HLE_IMPL(u64, strcspn, const char* s, const char* reject) {
    HLE_QUIET("%p", static_cast<const void*>(s));
    return std::strcspn(s, reject);
}
HLE_OVERRIDE(strcspn);

HLE_IMPL(char*, strtok, char* s, const char* delim) {
    HLE_QUIET("%p", static_cast<void*>(s));
    static thread_local char* state = nullptr;
    return strtok_s(s, delim, &state);
}
HLE_OVERRIDE(strtok);

// ================================= heap ====================================
// Heap do host, alinhado em 16 bytes como o malloc do Orbis.
void* HeapAlloc_(u64 size) {
    return _aligned_malloc(size ? size : 1, 16);
}

HLE_IMPL(void*, malloc, u64 size) {
    void* p = HeapAlloc_(size);
    HLE_QUIET("%" PRIu64 " -> %p", size, p);
    return p;
}
HLE_OVERRIDE(malloc);

HLE_IMPL(void, free, void* p) {
    HLE_QUIET("%p", p);
    _aligned_free(p);
}
HLE_OVERRIDE(free);

// operator new(unsigned long) / new[] / delete / delete[]
HLE_IMPL(void*, _Znwm, u64 size) {
    void* p = HeapAlloc_(size);
    HLE_QUIET("%" PRIu64 " -> %p", size, p);
    if (!p) {
        log::Fatal("operator new(%" PRIu64 ") sem memoria", size);
    }
    return p;
}
HLE_OVERRIDE(_Znwm);

HLE_IMPL(void*, _Znam, u64 size) {
    void* p = HeapAlloc_(size);
    HLE_QUIET("%" PRIu64 " -> %p", size, p);
    if (!p) {
        log::Fatal("operator new[](%" PRIu64 ") sem memoria", size);
    }
    return p;
}
HLE_OVERRIDE(_Znam);

HLE_IMPL(void, _ZdlPv, void* p) {
    HLE_QUIET("%p", p);
    _aligned_free(p);
}
HLE_OVERRIDE(_ZdlPv);

HLE_IMPL(void, _ZdaPv, void* p) {
    HLE_QUIET("%p", p);
    _aligned_free(p);
}
HLE_OVERRIDE(_ZdaPv);

struct SceLibcMallocManagedSize {
    u16 size;
    u16 version;
    u32 reserved;
    u64 max_system_size;
    u64 current_system_size;
    u64 max_inuse_size;
    u64 current_inuse_size;
};

HLE_IMPL(int, malloc_stats_fast, SceLibcMallocManagedSize* stats) {
    HLE_TRACE("%p", static_cast<void*>(stats));
    if (!stats) {
        return -1;
    }
    // O heap e o do Windows; nao ha contabilidade real para devolver.
    stats->max_system_size = stats->current_system_size = 0;
    stats->max_inuse_size = stats->current_inuse_size = 0;
    return 0;
}
HLE_OVERRIDE(malloc_stats_fast);

// ============================ runtime C/C++ ================================
using AtexitFn = void (HLE_SYSV_ABI*)();
using CxaDtor = void (HLE_SYSV_ABI*)(void*);

// Handlers ficam so registrados: o processo termina por TerminateProcess, sem roda-los.
std::vector<void*> g_atexit;

HLE_IMPL(int, atexit, AtexitFn fn) {
    HLE_TRACE("%p", reinterpret_cast<void*>(fn));
    g_atexit.push_back(reinterpret_cast<void*>(fn));
    return 0;
}
HLE_OVERRIDE(atexit);

HLE_IMPL(int, __cxa_atexit, CxaDtor dtor, void* arg, void* dso) {
    HLE_QUIET("%p(%p)", reinterpret_cast<void*>(dtor), arg);
    g_atexit.push_back(reinterpret_cast<void*>(dtor));
    return 0;
}
HLE_OVERRIDE(__cxa_atexit);

// Guarda de inicializacao de static local: byte 0 = "ja inicializado".
CRITICAL_SECTION g_guard_cs;
const bool g_guard_cs_init = (InitializeCriticalSection(&g_guard_cs), true);

HLE_IMPL(int, __cxa_guard_acquire, u8* guard) {
    HLE_QUIET("%p", static_cast<void*>(guard));
    if (guard[0]) {
        return 0;
    }
    EnterCriticalSection(&g_guard_cs); // recursiva: inicializadores podem se aninhar
    if (guard[0]) {
        LeaveCriticalSection(&g_guard_cs);
        return 0;
    }
    return 1;
}
HLE_OVERRIDE(__cxa_guard_acquire);

HLE_IMPL(void, __cxa_guard_release, u8* guard) {
    HLE_QUIET("%p", static_cast<void*>(guard));
    guard[0] = 1;
    LeaveCriticalSection(&g_guard_cs);
}
HLE_OVERRIDE(__cxa_guard_release);

HLE_IMPL(void, _init_env, void* params) {
    HLE_TRACE("%p", params);
}
HLE_OVERRIDE(_init_env);

HLE_IMPL(void, catchReturnFromMain, int status) {
    HLE_TRACE("main retornou %d", status);
}
HLE_OVERRIDE(catchReturnFromMain);

HLE_IMPL(void, exit, int status) {
    HLE_TRACE("%d", status);
    std::printf("\nO jogo chamou exit(%d).\n", status);
    log::Shutdown(0);
}
HLE_OVERRIDE(exit);

HLE_IMPL(void, _Exit, int status) {
    HLE_TRACE("%d", status);
    std::printf("\nO jogo chamou _Exit(%d).\n", status);
    log::Shutdown(0);
}
HLE_OVERRIDE(_Exit);

HLE_IMPL(void, abort) {
    log::Fatal("o jogo chamou abort()");
}
HLE_OVERRIDE(abort);

HLE_IMPL(void, __cxa_pure_virtual) {
    log::Fatal("chamada de metodo virtual puro (__cxa_pure_virtual)");
}
HLE_OVERRIDE(__cxa_pure_virtual);

void* g_terminate_handler = nullptr;

HLE_IMPL(void*, _ZSt13set_terminatePFvvE, void* handler) { // std::set_terminate
    HLE_TRACE("%p", handler);
    return std::exchange(g_terminate_handler, handler);
}
HLE_OVERRIDE(_ZSt13set_terminatePFvvE);

HLE_IMPL(void, _ZSt9terminatev) {
    log::Fatal("std::terminate()");
}
HLE_OVERRIDE(_ZSt9terminatev);

HLE_IMPL(void, _ZSt11_Xbad_allocv) {
    log::Fatal("std::_Xbad_alloc()");
}
HLE_OVERRIDE(_ZSt11_Xbad_allocv);

HLE_IMPL(void, _ZSt14_Xlength_errorPKc, const char* what) {
    log::Fatal("std::_Xlength_error(\"%s\")", what ? what : "");
}
HLE_OVERRIDE(_ZSt14_Xlength_errorPKc);

HLE_IMPL(void, _ZSt14_Xout_of_rangePKc, const char* what) {
    log::Fatal("std::_Xout_of_range(\"%s\")", what ? what : "");
}
HLE_OVERRIDE(_ZSt14_Xout_of_rangePKc);

// Excecoes C++ do jogo precisariam do unwinder do Orbis sobre o .eh_frame do eboot.
HLE_IMPL(void, _Unwind_Resume, void* exception) {
    log::Fatal("_Unwind_Resume(%p): unwinding de excecao C++ nao suportado", exception);
}
HLE_OVERRIDE(_Unwind_Resume);

HLE_IMPL(void*, __cxa_begin_catch, void* exception) {
    log::Fatal("__cxa_begin_catch(%p): excecao C++ nao suportada", exception);
}
HLE_OVERRIDE(__cxa_begin_catch);

HLE_IMPL(int, __gxx_personality_v0) {
    log::Fatal("__gxx_personality_v0: unwinding de excecao C++ nao suportado");
}
HLE_OVERRIDE(__gxx_personality_v0);

// std::ios_base::Init / std::_Winit: inicializam cout/wcout da libc++, que nao existem aqui.
// Um par por unidade de traducao que inclui <iostream> (centenas): silenciados.
HLE_IMPL(void, _ZNSt8ios_base4InitC1Ev, void* self) {
    HLE_QUIET("%p", self);
}
HLE_OVERRIDE(_ZNSt8ios_base4InitC1Ev);

HLE_IMPL(void, _ZNSt8ios_base4InitD1Ev, void* self) {
    HLE_QUIET("%p", self);
}
HLE_OVERRIDE(_ZNSt8ios_base4InitD1Ev);

HLE_IMPL(void, _ZNSt6_WinitC1Ev, void* self) {
    HLE_QUIET("%p", self);
}
HLE_OVERRIDE(_ZNSt6_WinitC1Ev);

HLE_IMPL(void, _ZNSt6_WinitD1Ev, void* self) {
    HLE_QUIET("%p", self);
}
HLE_OVERRIDE(_ZNSt6_WinitD1Ev);

HLE_IMPL(const char*, _ZSt13_Syserror_mapi, int code) { // std::_Syserror_map(int)
    HLE_TRACE("%d", code);
    return std::strerror(code);
}
HLE_OVERRIDE(_ZSt13_Syserror_mapi);

// setjmp/longjmp: como as duas pontas sao nossas, o layout do jmp_buf e livre (8 qwords,
// cabe nos 96 bytes do jmp_buf do Orbis). Salva exatamente os callee-saved da ABI System V.
__attribute__((naked)) void impl_setjmp() {
    __asm__("movq (%rsp), %rax\n"
            "movq %rax, 0(%rdi)\n" // endereco de retorno
            "movq %rbx, 8(%rdi)\n"
            "leaq 8(%rsp), %rax\n"
            "movq %rax, 16(%rdi)\n" // rsp apos o retorno
            "movq %rbp, 24(%rdi)\n"
            "movq %r12, 32(%rdi)\n"
            "movq %r13, 40(%rdi)\n"
            "movq %r14, 48(%rdi)\n"
            "movq %r15, 56(%rdi)\n"
            "xorl %eax, %eax\n"
            "retq\n");
}
HLE_OVERRIDE(setjmp);

__attribute__((naked)) void impl_longjmp() {
    __asm__("movl %esi, %eax\n"
            "testl %eax, %eax\n"
            "jnz 1f\n"
            "incl %eax\n" // longjmp(buf, 0) faz setjmp retornar 1
            "1:\n"
            "movq 8(%rdi), %rbx\n"
            "movq 24(%rdi), %rbp\n"
            "movq 32(%rdi), %r12\n"
            "movq 40(%rdi), %r13\n"
            "movq 48(%rdi), %r14\n"
            "movq 56(%rdi), %r15\n"
            "movq 16(%rdi), %rsp\n"
            "jmpq *0(%rdi)\n");
}
HLE_OVERRIDE(longjmp);

// ============================== utilitarios ================================
using CompareFn = int (HLE_SYSV_ABI*)(const void*, const void*);

HLE_IMPL(void, qsort, void* base, u64 count, u64 size, CompareFn compare) {
    HLE_QUIET("%p, n=%" PRIu64 ", size=%" PRIu64, base, count, size);
    if (count < 2 || !size) {
        return;
    }
    // O qsort do host chamaria o comparador com a ABI da Microsoft: ordena ponteiros com um
    // lambda que chama o comparador do jogo e depois reescreve o vetor na nova ordem.
    auto* bytes = static_cast<u8*>(base);
    std::vector<const u8*> order(count);
    for (u64 i = 0; i < count; ++i) {
        order[i] = bytes + i * size;
    }
    std::stable_sort(order.begin(), order.end(),
                     [compare](const u8* a, const u8* b) { return compare(a, b) < 0; });
    std::vector<u8> sorted(count * size);
    for (u64 i = 0; i < count; ++i) {
        std::memcpy(sorted.data() + i * size, order[i], size);
    }
    std::memcpy(bytes, sorted.data(), sorted.size());
}
HLE_OVERRIDE(qsort);

// rand() do Orbis (FreeBSD) devolve 31 bits: RAND_MAX = 0x7FFFFFFF. O da MSVCRT devolve 15
// (RAND_MAX = 0x7FFF); codigo do jogo que divide por RAND_MAX ou compara com limites grandes
// veria so valores minusculos. Gerador proprio (xorshift64*), estado por processo.
std::atomic<u64> g_rand_state{0x9E3779B97F4A7C15ULL};
std::atomic<u64> g_rand_calls{0};
// --rand15: reproduz a faixa de 15 bits da MSVCRT, so para comparar o comportamento do jogo.
bool g_rand_15_bits = false;

// Amostra de quem chama rand(): a cada 2^16 chamadas guarda o endereco de retorno.
constexpr int kRandSlots = 8;
std::atomic<u64> g_rand_caller[kRandSlots];
std::atomic<u64> g_rand_caller_hits[kRandSlots];

HLE_IMPL(int, rand) {
    HLE_QUIET("");
    u64 x = g_rand_state.load(std::memory_order_relaxed);
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    g_rand_state.store(x, std::memory_order_relaxed);
    if ((g_rand_calls.fetch_add(1, std::memory_order_relaxed) & 0xFFFF) == 0) {
        const u64 caller = HLE_RETURN_ADDRESS();
        for (int i = 0; i < kRandSlots; ++i) {
            u64 expected = 0;
            if (g_rand_caller[i].load() == caller || g_rand_caller[i].compare_exchange_strong(expected, caller)) {
                g_rand_caller_hits[i].fetch_add(1);
                break;
            }
        }
    }
    const int value = static_cast<int>((x * 0x2545F4914F6CDD1DULL) >> 33);
    return g_rand_15_bits ? (value & 0x7FFF) : value;
}
HLE_OVERRIDE(rand);

HLE_IMPL(void, srand, u32 seed) {
    HLE_TRACE("%u", seed);
    g_rand_state.store(0x9E3779B97F4A7C15ULL ^ (static_cast<u64>(seed) << 17 | seed | 1));
}
HLE_OVERRIDE(srand);

HLE_IMPL(s64, strtol, const char* s, char** end, int base) {
    HLE_QUIET("\"%.32s\"", s);
    return _strtoi64(s, end, base);
}
HLE_OVERRIDE(strtol);

HLE_IMPL(u64, _Stoul, const char* s, char** end, int base) {
    HLE_QUIET("\"%.32s\"", s);
    return _strtoui64(s, end, base);
}
HLE_OVERRIDE(_Stoul);

HLE_IMPL(s64, _Stoll, const char* s, char** end, int base) {
    HLE_QUIET("\"%.32s\"", s);
    return _strtoi64(s, end, base);
}
HLE_OVERRIDE(_Stoll);

// Dinkumware: strtod com um expoente decimal extra (strtod(s) == _Stod(s, end, 0)).
HLE_IMPL(double, _Stod, const char* s, char** end, s64 pten) {
    HLE_QUIET("\"%.32s\"", s);
    const double v = std::strtod(s, end);
    return pten ? v * std::pow(10.0, static_cast<double>(pten)) : v;
}
HLE_OVERRIDE(_Stod);

// struct tm do FreeBSD: os 9 ints do padrao + tm_gmtoff (long) + tm_zone (char*)
struct OrbisTm {
    int tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday, tm_yday, tm_isdst;
    s64 tm_gmtoff;
    const char* tm_zone;
};

HLE_IMPL(OrbisTm*, gmtime, const s64* timer) {
    HLE_TRACE("%" PRId64, timer ? *timer : 0);
    static thread_local OrbisTm result;
    if (!timer) {
        return nullptr;
    }
    const __time64_t t = *timer;
    std::tm host{};
    if (_gmtime64_s(&host, &t) != 0) {
        return nullptr;
    }
    result = {host.tm_sec, host.tm_min, host.tm_hour, host.tm_mday, host.tm_mon,
              host.tm_year, host.tm_wday, host.tm_yday, host.tm_isdst, 0, "UTC"};
    return &result;
}
HLE_OVERRIDE(gmtime);

// ---- tabelas de ctype da Dinkumware (indice -1..255; -1 = EOF) -------------
constexpr short kXD = 0x01, kUP = 0x02, kSP = 0x04, kPU = 0x08, kLO = 0x10, kDI = 0x20, kCN = 0x40, kBB = 0x80;

short g_ctype[257];
short g_tolower[257];
short g_toupper[257];

const bool g_ctype_init = [] {
    for (int c = 0; c < 256; ++c) {
        short bits = 0;
        if (c < 128) {
            if (c >= 'A' && c <= 'Z') bits |= kUP;
            if (c >= 'a' && c <= 'z') bits |= kLO;
            if (c >= '0' && c <= '9') bits |= kDI;
            if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) bits |= kXD;
            if (c == ' ') bits |= kSP;
            else if (c >= '\t' && c <= '\r') bits |= kCN;
            else if (c < 0x20 || c == 0x7F) bits |= kBB;
            else if (!(bits & (kUP | kLO | kDI))) bits |= kPU;
        }
        g_ctype[c + 1] = bits;
        g_tolower[c + 1] = static_cast<short>((c >= 'A' && c <= 'Z') ? c + 32 : c);
        g_toupper[c + 1] = static_cast<short>((c >= 'a' && c <= 'z') ? c - 32 : c);
    }
    g_ctype[0] = 0;
    g_tolower[0] = g_toupper[0] = -1;
    return true;
}();

HLE_IMPL(const short*, _Getpctype) {
    HLE_QUIET("");
    return g_ctype + 1;
}
HLE_OVERRIDE(_Getpctype);

HLE_IMPL(const short*, _Getptolower) {
    HLE_QUIET("");
    return g_tolower + 1;
}
HLE_OVERRIDE(_Getptolower);

HLE_IMPL(const short*, _Getptoupper) {
    HLE_QUIET("");
    return g_toupper + 1;
}
HLE_OVERRIDE(_Getptoupper);

// ================================ matematica ===============================
#define HLE_MATH1(name, type, expr)                                                                \
    HLE_IMPL(type, name, type x) {                                                                 \
        HLE_QUIET("%g", static_cast<double>(x));                                                   \
        return expr;                                                                               \
    }                                                                                              \
    HLE_OVERRIDE(name)

#define HLE_MATH2(name, type, expr)                                                                \
    HLE_IMPL(type, name, type x, type y) {                                                         \
        HLE_QUIET("%g, %g", static_cast<double>(x), static_cast<double>(y));                       \
        return expr;                                                                               \
    }                                                                                              \
    HLE_OVERRIDE(name)

HLE_MATH1(tan, double, std::tan(x));
HLE_MATH1(atan, double, std::atan(x));
HLE_MATH1(asin, double, std::asin(x));
HLE_MATH1(exp, double, std::exp(x));
HLE_MATH1(acosf, float, std::acos(x));
HLE_MATH1(tanf, float, std::tan(x));
HLE_MATH1(atanf, float, std::atan(x));
HLE_MATH1(expf, float, std::exp(x));
HLE_MATH1(exp2f, float, std::exp2(x));
HLE_MATH2(atan2, double, std::atan2(x, y));
HLE_MATH2(pow, double, std::pow(x, y));
HLE_MATH2(atan2f, float, std::atan2(x, y));
HLE_MATH2(powf, float, std::pow(x, y));
HLE_MATH2(fmodf, float, std::fmod(x, y));

HLE_IMPL(double, ldexp, double x, int e) {
    HLE_QUIET("%g, %d", x, e);
    return std::ldexp(x, e);
}
HLE_OVERRIDE(ldexp);

HLE_IMPL(float, ldexpf, float x, int e) {
    HLE_QUIET("%g, %d", static_cast<double>(x), e);
    return std::ldexp(x, e);
}
HLE_OVERRIDE(ldexpf);

HLE_IMPL(float, modff, float x, float* integral) {
    HLE_QUIET("%g", static_cast<double>(x));
    return std::modf(x, integral);
}
HLE_OVERRIDE(modff);

// Dinkumware: sin/cos compartilham _Sin(x, quadrante): 0 = sin, 1 = cos.
template <typename T>
T QuadrantSin(T x, u32 quadrant) {
    switch (quadrant & 3) {
    case 0: return std::sin(x);
    case 1: return std::cos(x);
    case 2: return -std::sin(x);
    default: return -std::cos(x);
    }
}

HLE_IMPL(double, _Sin, double x, u32 quadrant) {
    HLE_QUIET("%g, %u", x, quadrant);
    return QuadrantSin(x, quadrant);
}
HLE_OVERRIDE(_Sin);

HLE_IMPL(float, _FSin, float x, u32 quadrant) {
    HLE_QUIET("%g, %u", static_cast<double>(x), quadrant);
    return QuadrantSin(x, quadrant);
}
HLE_OVERRIDE(_FSin);

// Dinkumware: _Log(x, base): 0 = ln, 1 (positivo) = log10, -1 (negativo) = log2 (log2/log2f chamam
// _Log(x, -1)). Fase 60: antes qualquer base diferente de 0 virava log10, e todo log2 do jogo saia
// 3,3 vezes menor (parametros de nevoa e de exposicao calculados na CPU).
template <typename T>
T DinkumwareLog(T x, int base) {
    return base < 0 ? std::log2(x) : base > 0 ? std::log10(x) : std::log(x);
}

void NoteLogBase(int base) { // DIAGNOSTICO Fase 60: quais bases o jogo usa (uma linha por base)
    static std::atomic<u32> seen{0};
    const u32 bit = base < 0 ? 1u : base > 0 ? 2u : 4u;
    if (!(seen.fetch_or(bit) & bit)) {
        std::printf("[LIBM] _Log/_FLog chamado com base %d (%s)\n", base, base < 0 ? "log2" : base > 0 ? "log10" : "ln");
    }
}

HLE_IMPL(double, _Log, double x, int base) {
    HLE_QUIET("%g, %d", x, base);
    NoteLogBase(base);
    return DinkumwareLog(x, base);
}
HLE_OVERRIDE(_Log);

HLE_IMPL(float, _FLog, float x, int base) {
    HLE_QUIET("%g, %d", static_cast<double>(x), base);
    NoteLogBase(base);
    return DinkumwareLog(x, base);
}
HLE_OVERRIDE(_FLog);

// Dinkumware: classifica um float. >0 = NaN/Inf, 0 = zero, <0 = finito.
HLE_IMPL(short, _FDtest, float* px) {
    HLE_QUIET("%g", static_cast<double>(*px));
    switch (std::fpclassify(*px)) {
    case FP_NAN: return 2;        // _NANCODE
    case FP_INFINITE: return 1;   // _INFCODE
    case FP_ZERO: return 0;
    case FP_SUBNORMAL: return -2; // _DENORM
    default: return -1;           // _FINITE
    }
}
HLE_OVERRIDE(_FDtest);

} // namespace

namespace hle {

void SetRand15Bits(bool enabled) {
    g_rand_15_bits = enabled;
}

// Para o resumo final: quantas vezes rand() foi chamado e de onde (amostrado).
void PrintRandStats() {
    const u64 calls = g_rand_calls.load();
    if (calls < 100000) {
        return;
    }
    std::printf("rand(): %" PRIu64 " chamadas; chamadores (amostra de 1 a cada 65536):\n", calls);
    for (int i = 0; i < kRandSlots; ++i) {
        if (g_rand_caller[i].load()) {
            std::printf("  ~%" PRIu64 " chamadas de %s\n", g_rand_caller_hits[i].load() * 65536,
                        runtime::DescribeAddress(g_rand_caller[i].load()).c_str());
        }
    }
}

} // namespace hle
