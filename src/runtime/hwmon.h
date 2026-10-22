#pragma once

#include <string>

// Fase 65: leituras do PC para o contador de FPS / info (--stats-overlay, F10 no jogo).
//   GPU AMD: ADL do driver (atiadlxx.dll, ADL2_New_QueryPMLogData_Get); GPU NVIDIA: NVML (nvml.dll).
//   CPU: uso pelo GetSystemTimes. RAM: GlobalMemoryStatusEx.
// Temperatura e consumo da CPU precisam de um driver de kernel (o Afterburner / RTSS instala um): ficam de fora.
namespace runtime::hwmon {

struct Snapshot {
    std::string gpu_name; // "AMD" / "NVIDIA" (vazio = sem leitura da GPU)
    int gpu_temp = -1, hotspot = -1, vrm_temp = -1; // graus C
    int gpu_load = -1;                              // %
    int gpu_clock = -1, mem_clock = -1;             // MHz
    float gpu_power = -1.0f;                        // W
    int vram_mb = -1;                               // memoria de video em uso (MB)
    int cpu_load = -1;                              // %
    int ram_mb = -1;                                // RAM em uso no sistema (MB)
};

// Liga a thread que le tudo a cada segundo (chamadas seguintes nao fazem nada).
void Start();
Snapshot Get();

} // namespace runtime::hwmon
