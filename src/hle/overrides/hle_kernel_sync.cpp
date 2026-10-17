// libkernel: semaforos, event flags e equeue; libSceRtc: relogio.
#include <chrono>
#include <cinttypes>
#include <condition_variable>
#include <cstring>
#include <map>
#include <mutex>
#include <thread>

#include <windows.h>

#include "hle/overrides/hle_equeue.h"
#include "hle/hle_perf.h"
#include "hle/overrides/hle_override.h"
#include "runtime/crash_handler.h"

namespace {

using namespace hle;

constexpr int kESRCH = static_cast<int>(0x80020003);

constexpr u32 kSemaMagic = 0x53454D31;   // "SEM1"
constexpr u32 kEventMagic = 0x45564631;  // "EVF1"
constexpr u32 kEqueueMagic = 0x45515531; // "EQU1"

// sceKernelWaitEventFlag: waitMode
constexpr u32 kWaitAnd = 0x01;
constexpr u32 kWaitOr = 0x02;
constexpr u32 kWaitClearAll = 0x10;
constexpr u32 kWaitClearPat = 0x20;

struct Sema {
    u32 magic = kSemaMagic;
    std::mutex mutex;
    std::condition_variable cv;
    s32 count = 0;
    s32 max = 0;
    char name[32] = {};
};

struct EventFlag {
    u32 magic = kEventMagic;
    std::mutex mutex;
    std::condition_variable cv;
    u64 pattern = 0;
    char name[32] = {};
};

// Espera com o timeout do Orbis: ponteiro para microssegundos, nullptr = para sempre.
// Na saida o Orbis grava o tempo restante (0 em timeout).
template <typename Pred>
bool WaitWithTimeout(std::condition_variable& cv, std::unique_lock<std::mutex>& lock, u32* timeout_us, Pred pred) {
    if (!timeout_us) {
        cv.wait(lock, pred);
        return true;
    }
    const auto start = std::chrono::steady_clock::now();
    const bool ok = cv.wait_for(lock, std::chrono::microseconds(*timeout_us), pred);
    if (!ok) {
        *timeout_us = 0;
        return false;
    }
    const u64 elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    *timeout_us = elapsed >= *timeout_us ? 0 : static_cast<u32>(*timeout_us - elapsed);
    return true;
}

// ------------------------------ semaforos -----------------------------------
HLE_IMPL(int, sceKernelCreateSema, Sema** out, const char* name, u32 attr, s32 init_count, s32 max_count,
         const void* opt) {
    int result = err::OK;
    if (!out || !name || init_count < 0 || max_count <= 0 || init_count > max_count || opt) {
        result = err::kEINVAL;
    } else {
        auto* sema = new Sema;
        sema->count = init_count;
        sema->max = max_count;
        std::strncpy(sema->name, name, sizeof(sema->name) - 1);
        *out = sema;
    }
    HLE_TRACE("\"%s\" attr=0x%x init=%d max=%d -> %s", name ? name : "", attr, init_count, max_count,
              result ? "ERRO" : "OK");
    return result;
}
HLE_OVERRIDE(sceKernelCreateSema);

HLE_IMPL(int, sceKernelDeleteSema, Sema* sema) {
    HLE_TRACE("\"%s\"", sema ? sema->name : "");
    if (!sema || sema->magic != kSemaMagic) {
        return kESRCH;
    }
    sema->magic = 0;
    delete sema;
    return err::OK;
}
HLE_OVERRIDE(sceKernelDeleteSema);

HLE_IMPL(int, sceKernelSignalSema, Sema* sema, s32 count) {
    HLE_QUIET("\"%s\" +%d", sema ? sema->name : "", count);
    if (!sema || sema->magic != kSemaMagic) {
        return kESRCH;
    }
    {
        std::lock_guard lock(sema->mutex);
        if (count <= 0 || sema->count + count > sema->max) {
            return err::kEINVAL;
        }
        sema->count += count;
    }
    sema->cv.notify_all();
    return err::OK;
}
HLE_OVERRIDE(sceKernelSignalSema);

HLE_IMPL(int, sceKernelWaitSema, Sema* sema, s32 need, u32* timeout_us) {
    HLE_QUIET("\"%s\" -%d", sema ? sema->name : "", need);
    if (!sema || sema->magic != kSemaMagic) {
        return kESRCH;
    }
    if (need <= 0 || need > sema->max) {
        return err::kEINVAL;
    }
    runtime::WaitScope waiting("sceKernelWaitSema");
    std::unique_lock lock(sema->mutex);
    if (!WaitWithTimeout(sema->cv, lock, timeout_us, [&] { return sema->count >= need; })) {
        return err::kETIMEDOUT;
    }
    sema->count -= need;
    return err::OK;
}
HLE_OVERRIDE(sceKernelWaitSema);

// ----------------------------- event flags ----------------------------------
HLE_IMPL(int, sceKernelCreateEventFlag, EventFlag** out, const char* name, u32 attr, u64 init_pattern,
         const void* opt) {
    int result = err::OK;
    if (!out || !name || opt) {
        result = err::kEINVAL;
    } else {
        auto* flag = new EventFlag;
        flag->pattern = init_pattern;
        std::strncpy(flag->name, name, sizeof(flag->name) - 1);
        *out = flag;
    }
    HLE_TRACE("\"%s\" attr=0x%x init=0x%" PRIx64 " -> %s", name ? name : "", attr, init_pattern,
              result ? "ERRO" : "OK");
    return result;
}
HLE_OVERRIDE(sceKernelCreateEventFlag);

HLE_IMPL(int, sceKernelDeleteEventFlag, EventFlag* flag) {
    HLE_TRACE("\"%s\"", flag ? flag->name : "");
    if (!flag || flag->magic != kEventMagic) {
        return kESRCH;
    }
    flag->magic = 0;
    delete flag;
    return err::OK;
}
HLE_OVERRIDE(sceKernelDeleteEventFlag);

HLE_IMPL(int, sceKernelSetEventFlag, EventFlag* flag, u64 bits) {
    HLE_QUIET("\"%s\" |= 0x%" PRIx64, flag ? flag->name : "", bits);
    if (!flag || flag->magic != kEventMagic) {
        return kESRCH;
    }
    {
        std::lock_guard lock(flag->mutex);
        flag->pattern |= bits;
    }
    flag->cv.notify_all();
    return err::OK;
}
HLE_OVERRIDE(sceKernelSetEventFlag);

// No Orbis, Clear recebe a mascara dos bits que FICAM: pattern &= bits.
HLE_IMPL(int, sceKernelClearEventFlag, EventFlag* flag, u64 bits) {
    HLE_QUIET("\"%s\" &= 0x%" PRIx64, flag ? flag->name : "", bits);
    if (!flag || flag->magic != kEventMagic) {
        return kESRCH;
    }
    std::lock_guard lock(flag->mutex);
    flag->pattern &= bits;
    return err::OK;
}
HLE_OVERRIDE(sceKernelClearEventFlag);

HLE_IMPL(int, sceKernelWaitEventFlag, EventFlag* flag, u64 bits, u32 wait_mode, u64* result_pattern,
         u32* timeout_us) {
    HLE_QUIET("\"%s\" bits=0x%" PRIx64 " modo=0x%x", flag ? flag->name : "", bits, wait_mode);
    if (!flag || flag->magic != kEventMagic) {
        return kESRCH;
    }
    const bool wait_and = (wait_mode & kWaitAnd) != 0;
    if (!bits || wait_and == ((wait_mode & kWaitOr) != 0)) {
        return err::kEINVAL; // exatamente um entre AND e OR
    }
    runtime::WaitScope waiting("sceKernelWaitEventFlag");
    std::unique_lock lock(flag->mutex);
    // Ao contrario do semaforo, o event flag NAO devolve o tempo restante: o timeout do chamador
    // fica intacto. O jogo depende disso - a thread usb_polling inicializa o timeout (5 s) uma
    // unica vez e reutiliza a mesma variavel a cada volta do laco.
    u32 timeout_copy = timeout_us ? *timeout_us : 0;
    const bool ok = WaitWithTimeout(flag->cv, lock, timeout_us ? &timeout_copy : nullptr, [&] {
        return wait_and ? (flag->pattern & bits) == bits : (flag->pattern & bits) != 0;
    });
    if (result_pattern) {
        *result_pattern = flag->pattern;
    }
    if (!ok) {
        return err::kETIMEDOUT;
    }
    if (wait_mode & kWaitClearAll) {
        flag->pattern = 0;
    } else if (wait_mode & kWaitClearPat) {
        flag->pattern &= ~bits;
    }
    return err::OK;
}
HLE_OVERRIDE(sceKernelWaitEventFlag);

// -------------------------------- equeue ------------------------------------
// Fila real: os subsistemas HLE registram (ident, filter) e disparam eventos por
// hle::equeue::Trigger; sceKernelWaitEqueue entrega os disparados e bloqueia quando nao ha.
// sceKernelDeleteEqueue e sceKernelGetEventUserData nao sao importados pelo eboot.
struct RegisteredEvent {
    void* udata = nullptr;
    bool triggered = false;
    s64 data = 0;
    u32 fflags = 0;
};

struct Equeue {
    u32 magic = kEqueueMagic;
    std::mutex mutex;
    std::condition_variable cv;
    std::map<std::pair<u64, s16>, RegisteredEvent> events;
    char name[32] = {};
};

Equeue* AsEqueue(void* handle) {
    auto* eq = static_cast<Equeue*>(handle);
    return (eq && eq->magic == kEqueueMagic) ? eq : nullptr;
}

HLE_IMPL(int, sceKernelCreateEqueue, Equeue** out, const char* name) {
    HLE_TRACE("\"%s\"", name ? name : "");
    if (!out) {
        return err::kEINVAL;
    }
    auto* eq = new Equeue;
    if (name) {
        std::strncpy(eq->name, name, sizeof(eq->name) - 1);
    }
    *out = eq;
    return err::OK;
}
HLE_OVERRIDE(sceKernelCreateEqueue);

HLE_IMPL(int, sceKernelWaitEqueue, Equeue* eq, equeue::KernelEvent* events, int max_events, int* out_count,
         u32* timeout_us) {
    HLE_QUIET("\"%s\" max=%d", AsEqueue(eq) ? eq->name : "?", max_events);
    if (!AsEqueue(eq)) {
        return kESRCH;
    }
    if (!events || max_events < 1) {
        return err::kEINVAL;
    }
    static perf::Counter perf_counter("WaitEqueue (inclui o tempo bloqueado)");
    const perf::Scope perf_scope(perf_counter);
    runtime::WaitScope waiting("sceKernelWaitEqueue");
    std::unique_lock lock(eq->mutex);
    const auto any_triggered = [eq] {
        for (const auto& [key, ev] : eq->events) {
            if (ev.triggered) {
                return true;
            }
        }
        return false;
    };
    u32 timeout_copy = timeout_us ? *timeout_us : 0; // o timeout do chamador nao e alterado
    const bool ok = WaitWithTimeout(eq->cv, lock, timeout_us ? &timeout_copy : nullptr, any_triggered);
    int count = 0;
    if (ok) {
        for (auto& [key, ev] : eq->events) {
            if (!ev.triggered || count >= max_events) {
                continue;
            }
            events[count++] = {key.first, key.second, 0, ev.fflags, ev.data, ev.udata};
            ev.triggered = false;
        }
    }
    if (out_count) {
        *out_count = count;
    }
    return ok ? err::OK : err::kETIMEDOUT;
}
HLE_OVERRIDE(sceKernelWaitEqueue);

} // namespace

namespace hle::equeue {

bool IsValid(Handle queue) {
    return AsEqueue(queue) != nullptr;
}

bool Register(Handle queue, u64 ident, s16 filter, void* udata) {
    Equeue* eq = AsEqueue(queue);
    if (!eq) {
        return false;
    }
    std::lock_guard lock(eq->mutex);
    eq->events[{ident, filter}].udata = udata;
    return true;
}

bool Unregister(Handle queue, u64 ident, s16 filter) {
    Equeue* eq = AsEqueue(queue);
    if (!eq) {
        return false;
    }
    std::lock_guard lock(eq->mutex);
    return eq->events.erase({ident, filter}) != 0;
}

bool Trigger(Handle queue, u64 ident, s16 filter, s64 data) {
    Equeue* eq = AsEqueue(queue);
    if (!eq) {
        return false;
    }
    {
        std::lock_guard lock(eq->mutex);
        const auto it = eq->events.find({ident, filter});
        if (it == eq->events.end()) {
            return false;
        }
        it->second.triggered = true;
        it->second.data = data;
        ++it->second.fflags; // contador de disparos fundidos
    }
    eq->cv.notify_all();
    return true;
}

} // namespace hle::equeue

namespace {

using namespace hle;

// ================================ libSceRtc =================================
// Tick do Rtc: microssegundos desde 0001-01-01 00:00:00 UTC.
constexpr u64 kUnixEpochTicks = 62135596800000000ULL;
constexpr int kRtcErrorInvalidPointer = static_cast<int>(0x80010602); // INVALID_PARAMETER
constexpr int kRtcErrorInvalidValue = static_cast<int>(0x80010604);   // INVALID_DATE_PARAMETER

struct RtcDateTime {
    u16 year, month, day, hour, minute, second;
    u32 microsecond;
};

struct RtcTick {
    u64 tick;
};

u64 NowTicks() {
    using namespace std::chrono;
    return kUnixEpochTicks + duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

// Algoritmos de calendario civil (Howard Hinnant): dias desde 1970-01-01 <-> ano/mes/dia.
s64 DaysFromCivil(s64 y, u32 m, u32 d) {
    y -= m <= 2;
    const s64 era = (y >= 0 ? y : y - 399) / 400;
    const u32 yoe = static_cast<u32>(y - era * 400);
    const u32 doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const u32 doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<s64>(doe) - 719468;
}

void TicksToDateTime(u64 ticks, RtcDateTime* out) {
    const s64 unix_us = static_cast<s64>(ticks) - static_cast<s64>(kUnixEpochTicks);
    s64 days = unix_us / 86400000000LL;
    s64 rem = unix_us % 86400000000LL;
    if (rem < 0) {
        rem += 86400000000LL;
        --days;
    }
    const s64 z = days + 719468;
    const s64 era = (z >= 0 ? z : z - 146096) / 146097;
    const u32 doe = static_cast<u32>(z - era * 146097);
    const u32 yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const u32 doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const u32 mp = (5 * doy + 2) / 153;
    const u32 d = doy - (153 * mp + 2) / 5 + 1;
    const u32 m = mp < 10 ? mp + 3 : mp - 9;
    out->year = static_cast<u16>(static_cast<s64>(yoe) + era * 400 + (m <= 2));
    out->month = static_cast<u16>(m);
    out->day = static_cast<u16>(d);
    out->hour = static_cast<u16>(rem / 3600000000LL);
    out->minute = static_cast<u16>(rem / 60000000LL % 60);
    out->second = static_cast<u16>(rem / 1000000LL % 60);
    out->microsecond = static_cast<u32>(rem % 1000000LL);
}

u64 DateTimeToTicks(int y, int mo, int d, int h, int mi, int s, int us) {
    const s64 days = DaysFromCivil(y, mo, d);
    const s64 unix_us = ((days * 24 + h) * 60 + mi) * 60000000LL + s * 1000000LL + us;
    return static_cast<u64>(unix_us + static_cast<s64>(kUnixEpochTicks));
}

s64 LocalOffsetMinutes() {
    TIME_ZONE_INFORMATION tz;
    const DWORD mode = GetTimeZoneInformation(&tz);
    return -(tz.Bias + (mode == TIME_ZONE_ID_DAYLIGHT ? tz.DaylightBias : tz.StandardBias));
}

HLE_IMPL(int, sceRtcGetCurrentTick, RtcTick* tick) {
    HLE_QUIET("%p", static_cast<void*>(tick));
    if (!tick) {
        return kRtcErrorInvalidPointer;
    }
    tick->tick = NowTicks();
    return err::OK;
}
HLE_OVERRIDE(sceRtcGetCurrentTick);

// Sem rede: o "relogio de rede" e o proprio relogio do host.
HLE_IMPL(int, sceRtcGetCurrentNetworkTick, RtcTick* tick) {
    HLE_QUIET("%p", static_cast<void*>(tick));
    if (!tick) {
        return kRtcErrorInvalidPointer;
    }
    tick->tick = NowTicks();
    return err::OK;
}
HLE_OVERRIDE(sceRtcGetCurrentNetworkTick);

HLE_IMPL(int, sceRtcGetCurrentClock, RtcDateTime* time, int timezone_minutes) {
    HLE_QUIET("tz=%d", timezone_minutes);
    if (!time) {
        return kRtcErrorInvalidPointer;
    }
    TicksToDateTime(NowTicks() + static_cast<s64>(timezone_minutes) * 60000000LL, time);
    return err::OK;
}
HLE_OVERRIDE(sceRtcGetCurrentClock);

HLE_IMPL(int, sceRtcGetCurrentClockLocalTime, RtcDateTime* time) {
    HLE_QUIET("%p", static_cast<void*>(time));
    if (!time) {
        return kRtcErrorInvalidPointer;
    }
    TicksToDateTime(NowTicks() + LocalOffsetMinutes() * 60000000LL, time);
    return err::OK;
}
HLE_OVERRIDE(sceRtcGetCurrentClockLocalTime);

HLE_IMPL(int, sceRtcSetTime_t, RtcDateTime* time, s64 unix_seconds) {
    HLE_QUIET("%" PRId64, unix_seconds);
    if (!time) {
        return kRtcErrorInvalidPointer;
    }
    TicksToDateTime(kUnixEpochTicks + static_cast<u64>(unix_seconds) * 1000000ULL, time);
    return err::OK;
}
HLE_OVERRIDE(sceRtcSetTime_t);

HLE_IMPL(int, sceRtcSetTick, RtcDateTime* time, const RtcTick* tick) {
    HLE_QUIET("%p", static_cast<void*>(time));
    if (!time || !tick) {
        return kRtcErrorInvalidPointer;
    }
    TicksToDateTime(tick->tick, time);
    return err::OK;
}
HLE_OVERRIDE(sceRtcSetTick);

// "YYYY-MM-DDTHH:MM:SS[.fff][Z|+HH:MM|-HH:MM]"
int ParseRfc3339(RtcTick* tick, const char* text) {
    if (!tick || !text) {
        return kRtcErrorInvalidPointer;
    }
    int y, mo, d, h, mi, s, consumed = 0;
    if (std::sscanf(text, "%4d-%2d-%2d%*1[Tt ]%2d:%2d:%2d%n", &y, &mo, &d, &h, &mi, &s, &consumed) != 6) {
        return kRtcErrorInvalidValue;
    }
    const char* p = text + consumed;
    int us = 0;
    if (*p == '.') {
        int scale = 100000;
        for (++p; *p >= '0' && *p <= '9'; ++p, scale /= 10) {
            us += (*p - '0') * scale;
        }
    }
    s64 offset_minutes = 0;
    if (*p == '+' || *p == '-') {
        int oh = 0, om = 0;
        if (std::sscanf(p + 1, "%2d:%2d", &oh, &om) != 2) {
            return kRtcErrorInvalidValue;
        }
        offset_minutes = (*p == '-' ? -1 : 1) * (oh * 60 + om);
    }
    tick->tick = DateTimeToTicks(y, mo, d, h, mi, s, us) - offset_minutes * 60000000LL;
    return err::OK;
}

HLE_IMPL(int, sceRtcParseRFC3339, RtcTick* tick, const char* text) {
    const int result = ParseRfc3339(tick, text);
    HLE_TRACE("\"%s\" -> %s", text ? text : "", result ? "ERRO" : "OK");
    return result;
}
HLE_OVERRIDE(sceRtcParseRFC3339);

// O Orbis aceita varios formatos aqui (RFC 2822, ISO 8601...); so o RFC 3339 esta implementado.
HLE_IMPL(int, sceRtcParseDateTime, RtcTick* tick, const char* text) {
    const int result = ParseRfc3339(tick, text);
    HLE_TRACE("\"%s\" -> %s", text ? text : "", result ? "ERRO (so RFC 3339 e suportado)" : "OK");
    return result;
}
HLE_OVERRIDE(sceRtcParseDateTime);

} // namespace
