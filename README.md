# LEGO Dimensions PC

**English** | [Português](README.pt-BR.md)

An **unofficial** native port of the PS4 LEGO Dimensions (CUSA01176, version 01.24) to Windows x64.

It is not a PS4 emulator. The game executable runs directly on the PC's processor, and the PS4 system libraries were rewritten for Windows. The PS4 graphics (GCN) are translated to Vulkan, and the game's shaders are translated before you play, from the game's own packages.

> **This repository contains nothing from the game.** To play you need **your own dump** of the PS4 LEGO Dimensions (and, if you want, your DLCs). No game files, DLC, eboot, system modules or saves are distributed here.

## Download

Get the installer (`LEGO_Dimensions_PC_Setup.exe`) or the portable zip from the **[Releases](../../releases)** page.

## What you need

- Windows 10 (1803 or newer) or Windows 11, 64-bit.
- A graphics card with **Vulkan 1.3** and an up-to-date driver. Tested on an AMD Radeon RX 6600.
- **16 GB of RAM**. The game uses about 5 GB; 8 GB may work, but with stutters.
- The game dump: the `CUSA01176` folder (with `eboot.bin`, `sce_sys` and `data\chunk*`), **version 01.24**. Two layouts work:
  1. all in one: the update already inside the dump (`data\chunk1` with `PATCH.DAT`, `PATCH0.DAT`...);
  2. base game + update in a separate folder, `CUSA01176-patch` (shadPS4 layout), inside the game folder or next to it.
- Optional: DLCs (one subfolder per DLC, in the shadPS4 `addcont` layout), a DS4/Xbox controller and the LegoToypad app for the portal.

## How to use

1. Install (or extract the zip) and open **LegoDimensionsLauncher.exe**.
2. Choose the game folder. A separate update folder is found automatically. Choose the DLC folder too, if you have one.
3. Click **Prepare** (first time only). This converts the `eboot.bin` from your dump and translates all of the game's shaders to Vulkan. It takes 20 to 60 minutes.
4. Click **PLAY**.
5. Later, **Check updates** in the launcher downloads and installs new versions from the GitHub releases (from 1.3 on).

Details and tips in the [README](tools/dist/README.txt) that ships with the port.

## Language

The launcher, the installer and the port's own screens (loading screen, save picker) are in Portuguese when Windows is in Portuguese and in English otherwise. The game itself is told your Windows language when the game has it (English, French, German, Spanish, Italian, Dutch, Danish, Portuguese); anything else falls back to English. The **Language** selector in the launcher picks one by hand (Portuguese, English, Spanish, French, German, Italian, Dutch, Danish).

The languages other than Portuguese and English need the game's original text files: a dump with a fan translation that replaced `text.csv` closes the game in those languages.

## Status

- The story, Vorton and the DLC levels are playable at 60 fps (the "60 FPS" option in the launcher).
- Audio, saves, controller and the portal (the real Toy Pad on USB, the LegoToypad app or a virtual Toy Pad) work.
- Local co-op: up to 4 controllers, player 2 joins in split screen by pressing Options.
- Button icons follow your controller (Xbox 360, Xbox One/Series, PS3, PS4/PS5, Wii U for Nintendo controllers), built from the game's own files; the launcher can also lock one set.
- **LD PC Transfer** (button in the launcher): converts the progress between the port, PS4 / shadPS4, Xbox 360 (Xenia), Wii U (Cemu) and PS3 (RPCS3, experimental). The original save is not changed and whatever the destination had goes to the `saves_backup` folder. Xbox One is not possible: its saves live in the cloud, encrypted.
- The LegoToypad app (1.6 or newer) mirrors the portal lights the way the game drives them: colour, flashes and fades.
- Fullscreen ("Fullscreen" option in the launcher, or F11 in game) and a window size selector ("Resolution"; the game still renders at 1920x1080 and is scaled to the window).
- "Skip all cutscenes" option: any cutscene can be skipped, even the ones the game only lets you skip after watching them or at 100%.
- The controller only drives the game while the game window is in front: clicking the Toy Pad app makes the character stop.
- The first time you visit a new area the graphics driver still compiles pipelines. That can cause a short hitch, but objects never disappear (no pop-in).
- Very heavy scenes (~6000 draws per frame) run below 60 fps on 6-core CPUs.
- Some outdoor scenes (for example the aerial shot of Metropolis) have much heavier fog than on the PS4. Under investigation.
- Only tested on AMD graphics. Reports with NVIDIA and Intel are welcome (open an *issue* with the log from `build\logs\`).

## Building from source

Requires:
- Visual Studio 2022 or newer, with the C++ tools and **clang-cl**;
- CMake;
- [Vulkan SDK](https://vulkan.lunarg.com/) 1.4;
- Python 3.

```bat
set VULKAN_SDK=C:\VulkanSDK\1.4.363.0
build.bat                    :: the port -> build\lego_dimensions.exe
tools\dist\build_dist.bat    :: launcher and installer
```

**Shader translator** (`tools/shader_compiler_tool`): uses the shader recompiler from [shadPS4](https://github.com/shadps4-emu/shadPS4), unmodified, at commit [`eeecd10`](https://github.com/shadps4-emu/shadPS4/commit/eeecd103f648572b249b422e759e76799274b34c). The only replacement is `override/resource_patching_pass.cpp`, which lives in that folder.
1. Build that shadPS4 commit (Release, clang-cl).
2. Point `SHADPS4_DIR` at it.
3. Run `tools\shader_compiler_tool\build.bat`.

**Distribution package:** `python tools/dist/montar.py` builds the installer and the zip in `dist\`.

Source code comments and log messages are in Portuguese.

## License

GPL-2.0. See [LICENSE](LICENSE). Third-party libraries and their licenses are listed in [THIRD_PARTY.md](THIRD_PARTY.md).

## Disclaimer

Non-profit fan project. Not affiliated with the LEGO Group, TT Games, Warner Bros. Games or Sony Interactive Entertainment. LEGO and LEGO Dimensions are trademarks of the LEGO Group. PlayStation is a trademark of Sony. Only use it with a copy of the game you own.
