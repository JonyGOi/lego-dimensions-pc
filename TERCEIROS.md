# Componentes de terceiros

| Componente | Onde | Licença |
|---|---|---|
| [shadPS4](https://github.com/shadps4-emu/shadPS4): recompilador de shaders, commit `eeecd10` | compilado no `tools/shader_compiler_tool` | GPL-2.0-or-later |
| shadPS4: nomes dos NIDs (`aerolib`) | `tools/data/aerolib.csv` | GPL-2.0-or-later |
| shadPS4 v0.7.0: extração de PKG (`pkg.cpp`, `pfs.h`, `pkg_type`, `keys.h`, as funções de criptografia que ela usa, refeitas no CNG do Windows) | `tools/pkg_extractor` (`build/pkg_extractor.exe`) | GPL-2.0-or-later |
| [miniz](https://github.com/richgel999/miniz): só o inflate (`tinfl`) | `tools/pkg_extractor/miniz` | MIT |
| [Zydis](https://github.com/zyantific/zydis) e Zycore | `externals/zydis` | MIT |
| [SDL 3](https://github.com/libsdl-org/SDL) | `externals/sdl3` (biblioteca estática) | zlib |
| [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) | `externals/vulkan-headers` | Apache-2.0 / MIT |
| Python (runtime portátil, só no pacote de distribuição) | `tools/python` no instalador | PSF License |
| [Anomaly](https://github.com/connorh315/anomaly) v1.1 de connorh315: carregador de mods do LEGO Dimensions de PS4 (ganchos, text.csv / collection.txt em camadas), refeito dentro do port | `src/hle/overrides/hle_anomaly.cpp` | GPL-2.0-or-later |
| [7-Zip](https://www.7-zip.org) 26.03 (`7z.exe` + `7z.dll`, só no pacote de distribuição: instala mods .rar / .7z) | `tools/7zip` no instalador | GNU LGPL, com a restrição do unRAR em parte do `7z.dll`, e BSD |
| Microsoft Visual C++ Redistributable (DLLs no pacote) | `build/` no instalador | licença de redistribuição da Microsoft |

O texto de cada licença acompanha o componente na pasta dele. A licença deste projeto (GPL-2.0) está em [LICENSE](LICENSE).
