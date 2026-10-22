// Fase 65: leituras do PC para o contador de FPS / info (ver hwmon.h).
#include "runtime/hwmon.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>

#include <windows.h>

namespace runtime::hwmon {

namespace {

std::mutex g_mutex;
Snapshot g_snapshot;
std::atomic<bool> g_started{false};

// ---------------- AMD: ADL (ids do adl_defines.h do SDK do GPUOpen) ----------------
constexpr int kAdlMaxSensors = 256;
constexpr int kPmGfxClock = 1, kPmMemClock = 2, kPmTempEdge = 8, kPmTempVrVddc = 10, kPmActivityGfx = 19, kPmAsicPower = 23,
              kPmTempHotspot = 27;
struct AdlSensor {
    int supported;
    int value;
};
struct AdlPmLogOutput {
    int size;
    AdlSensor sensors[kAdlMaxSensors];
};
using AdlMalloc = void*(__stdcall*)(int);
using AdlCreate = int (*)(AdlMalloc, int, void**);
using AdlCount = int (*)(void*, int*);
using AdlPrimary = int (*)(void*, int*);
using AdlPmLog = int (*)(void*, int, AdlPmLogOutput*);
using AdlVram = int (*)(void*, int, int*);

void* __stdcall AdlAlloc(int size) { return std::malloc(static_cast<size_t>(size)); }

struct Adl {
    void* context = nullptr;
    int adapter = -1;
    AdlPmLog pmlog = nullptr;
    AdlVram vram = nullptr;

    bool Open() {
        HMODULE dll = LoadLibraryA("atiadlxx.dll");
        if (!dll) {
            return false;
        }
        const auto create = reinterpret_cast<AdlCreate>(GetProcAddress(dll, "ADL2_Main_Control_Create"));
        const auto count = reinterpret_cast<AdlCount>(GetProcAddress(dll, "ADL2_Adapter_NumberOfAdapters_Get"));
        const auto primary = reinterpret_cast<AdlPrimary>(GetProcAddress(dll, "ADL2_Adapter_Primary_Get"));
        pmlog = reinterpret_cast<AdlPmLog>(GetProcAddress(dll, "ADL2_New_QueryPMLogData_Get"));
        vram = reinterpret_cast<AdlVram>(GetProcAddress(dll, "ADL2_Adapter_VRAMUsage_Get"));
        if (!create || !pmlog || create(AdlAlloc, 1, &context) != 0) {
            return false;
        }
        // a placa que esta ligada no monitor primeiro; senao a primeira que responder com o clock da GPU
        int first = -1, total = 0;
        if (primary && primary(context, &first) == 0 && Works(first)) {
            adapter = first;
            return true;
        }
        if (count && count(context, &total) == 0) {
            for (int i = 0; i < total; ++i) {
                if (Works(i)) {
                    adapter = i;
                    return true;
                }
            }
        }
        return false;
    }

    bool Works(int index) {
        auto* out = new AdlPmLogOutput{};
        const bool ok = index >= 0 && pmlog(context, index, out) == 0 && out->sensors[kPmGfxClock].supported;
        delete out;
        return ok;
    }

    void Read(Snapshot& s) {
        auto* out = new AdlPmLogOutput{};
        if (pmlog(context, adapter, out) == 0) {
            const auto get = [&](int id) { return out->sensors[id].supported ? out->sensors[id].value : -1; };
            s.gpu_name = "AMD";
            s.gpu_clock = get(kPmGfxClock);
            s.mem_clock = get(kPmMemClock);
            s.gpu_temp = get(kPmTempEdge);
            s.vrm_temp = get(kPmTempVrVddc);
            s.hotspot = get(kPmTempHotspot);
            s.gpu_load = get(kPmActivityGfx);
            const int watts = get(kPmAsicPower);
            s.gpu_power = watts >= 0 ? static_cast<float>(watts) : -1.0f;
        }
        delete out;
        int mb = 0;
        if (vram && vram(context, adapter, &mb) == 0) {
            s.vram_mb = mb;
        }
    }
};

// ---------------- NVIDIA: NVML ----------------
struct NvUtil {
    unsigned gpu, memory;
};
struct NvMemory {
    unsigned long long total, free, used;
};
using NvInit = int (*)();
using NvHandle = int (*)(unsigned, void**);
using NvTemp = int (*)(void*, int, unsigned*);
using NvUtilFn = int (*)(void*, NvUtil*);
using NvClock = int (*)(void*, int, unsigned*);
using NvPower = int (*)(void*, unsigned*);
using NvMem = int (*)(void*, NvMemory*);

struct Nvml {
    void* device = nullptr;
    NvTemp temp = nullptr;
    NvUtilFn util = nullptr;
    NvClock clock = nullptr;
    NvPower power = nullptr;
    NvMem mem = nullptr;

    bool Open() {
        HMODULE dll = LoadLibraryA("nvml.dll");
        if (!dll) {
            dll = LoadLibraryA("C:\\Program Files\\NVIDIA Corporation\\NVSMI\\nvml.dll");
        }
        if (!dll) {
            return false;
        }
        const auto init = reinterpret_cast<NvInit>(GetProcAddress(dll, "nvmlInit_v2"));
        const auto handle = reinterpret_cast<NvHandle>(GetProcAddress(dll, "nvmlDeviceGetHandleByIndex_v2"));
        temp = reinterpret_cast<NvTemp>(GetProcAddress(dll, "nvmlDeviceGetTemperature"));
        util = reinterpret_cast<NvUtilFn>(GetProcAddress(dll, "nvmlDeviceGetUtilizationRates"));
        clock = reinterpret_cast<NvClock>(GetProcAddress(dll, "nvmlDeviceGetClockInfo"));
        power = reinterpret_cast<NvPower>(GetProcAddress(dll, "nvmlDeviceGetPowerUsage"));
        mem = reinterpret_cast<NvMem>(GetProcAddress(dll, "nvmlDeviceGetMemoryInfo"));
        return init && handle && init() == 0 && handle(0, &device) == 0;
    }

    void Read(Snapshot& s) {
        s.gpu_name = "NVIDIA";
        unsigned value = 0;
        if (temp && temp(device, 0 /*NVML_TEMPERATURE_GPU*/, &value) == 0) {
            s.gpu_temp = static_cast<int>(value);
        }
        NvUtil rates{};
        if (util && util(device, &rates) == 0) {
            s.gpu_load = static_cast<int>(rates.gpu);
        }
        if (clock && clock(device, 0 /*NVML_CLOCK_GRAPHICS*/, &value) == 0) {
            s.gpu_clock = static_cast<int>(value);
        }
        if (clock && clock(device, 2 /*NVML_CLOCK_MEM*/, &value) == 0) {
            s.mem_clock = static_cast<int>(value);
        }
        if (power && power(device, &value) == 0) {
            s.gpu_power = value / 1000.0f; // mW
        }
        NvMemory memory{};
        if (mem && mem(device, &memory) == 0) {
            s.vram_mb = static_cast<int>(memory.used >> 20);
        }
    }
};

unsigned long long FileTimeValue(const FILETIME& t) { return (static_cast<unsigned long long>(t.dwHighDateTime) << 32) | t.dwLowDateTime; }

void Loop() {
    Adl adl;
    Nvml nvml;
    const bool has_adl = adl.Open();
    const bool has_nvml = !has_adl && nvml.Open();
    std::printf("[INFO] contador de FPS: GPU %s\n", has_adl ? "AMD (ADL)" : has_nvml ? "NVIDIA (NVML)" : "sem leitura (so FPS, CPU e RAM)");
    FILETIME idle0{}, kernel0{}, user0{};
    GetSystemTimes(&idle0, &kernel0, &user0);
    for (;;) {
        Sleep(1000);
        Snapshot s;
        if (has_adl) {
            adl.Read(s);
        } else if (has_nvml) {
            nvml.Read(s);
        }
        FILETIME idle{}, kernel{}, user{};
        if (GetSystemTimes(&idle, &kernel, &user)) {
            const unsigned long long di = FileTimeValue(idle) - FileTimeValue(idle0);
            const unsigned long long dt = (FileTimeValue(kernel) - FileTimeValue(kernel0)) + (FileTimeValue(user) - FileTimeValue(user0)); // kernel inclui idle
            s.cpu_load = dt ? static_cast<int>(100 * (dt - di) / dt) : 0;
            idle0 = idle, kernel0 = kernel, user0 = user;
        }
        MEMORYSTATUSEX memory{sizeof(memory)};
        if (GlobalMemoryStatusEx(&memory)) {
            s.ram_mb = static_cast<int>((memory.ullTotalPhys - memory.ullAvailPhys) >> 20);
        }
        std::lock_guard lock(g_mutex);
        g_snapshot = s;
    }
}

} // namespace

void Start() {
    if (!g_started.exchange(true)) {
        std::thread(Loop).detach();
    }
}

Snapshot Get() {
    std::lock_guard lock(g_mutex);
    return g_snapshot;
}

} // namespace runtime::hwmon
