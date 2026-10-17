# Componentes de terceiros

| Componente | Onde | Licença |
|---|---|---|
| [shadPS4](https://github.com/shadps4-emu/shadPS4): recompilador de shaders, commit `eeecd10` | compilado no `tools/shader_compiler_tool` | GPL-2.0-or-later |
| shadPS4: nomes dos NIDs (`aerolib`) | `tools/data/aerolib.csv` | GPL-2.0-or-later |
| [Zydis](https://github.com/zyantific/zydis) e Zycore | `externals/zydis` | MIT |
| [SDL 3](https://github.com/libsdl-org/SDL) | `externals/sdl3` (biblioteca estática) | zlib |
| [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) | `externals/vulkan-headers` | Apache-2.0 / MIT |
| Python (runtime portátil, só no pacote de distribuição) | `tools/python` no instalador | PSF License |
| Microsoft Visual C++ Redistributable (DLLs no pacote) | `build/` no instalador | licença de redistribuição da Microsoft |

O texto de cada licença acompanha o componente na pasta dele. A licença deste projeto (GPL-2.0) está em [LICENSE](LICENSE).
