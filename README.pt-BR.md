# LEGO Dimensions PC

[English](README.md) | **Português**

Port nativo **não oficial** do LEGO Dimensions do PS4 (CUSA01176, versão 01.24) para Windows x64.

Não é um emulador de PS4. O executável do jogo roda direto no processador do PC, e as bibliotecas do sistema do PS4 foram reescritas para o Windows. Os gráficos do PS4 (GCN) são traduzidos para Vulkan, e os shaders do jogo são traduzidos antes de jogar, a partir dos próprios pacotes do jogo.

> **Este repositório não contém nada do jogo.** Para jogar você precisa do **seu próprio dump** do LEGO Dimensions do PS4 (e, se quiser, das suas DLCs). Nada de jogo, DLC, eboot, módulos do sistema ou saves é distribuído aqui.

## Download

Baixe o instalador (`LEGO_Dimensions_PC_Setup.exe`) ou o zip portátil na página de **[Releases](../../releases)**.

## O que você precisa

- Windows 10 (1803 ou mais novo) ou Windows 11, 64 bits.
- Placa de vídeo com **Vulkan 1.3** e driver atualizado. Testado em AMD Radeon RX 6600.
- **16 GB de RAM**. O jogo usa uns 5 GB; com 8 GB pode funcionar, mas com travadas.
- O dump do jogo: a pasta `CUSA01176` (com `eboot.bin`, `sce_sys` e `data\chunk*`), **versão 01.24**. Duas estruturas funcionam:
  1. tudo junto: a atualização já dentro do dump (`data\chunk1` com `PATCH.DAT`, `PATCH0.DAT`...);
  2. jogo base + atualização numa pasta separada, `CUSA01176-patch` (formato do shadPS4), dentro da pasta do jogo ou ao lado dela.
- Opcional: DLCs (uma subpasta por DLC, no formato da pasta `addcont` do shadPS4), controle DS4/Xbox e o app LegoToypad para o portal.

## Como usar

1. Instale (ou extraia o zip) e abra o **LegoDimensionsLauncher.exe**.
2. Escolha a pasta do jogo. A pasta da atualização separada é achada sozinha. Escolha também a das DLCs, se tiver.
3. Clique em **Preparar** (só na primeira vez). Isso converte o `eboot.bin` do seu dump e traduz todos os shaders do jogo para Vulkan. Leva de 20 a 60 minutos.
4. Clique em **JOGAR**.

Detalhes e dicas no [LEIA-ME](tools/dist/LEIA-ME.txt).

## Estado

- Dá para jogar a história, Vorton e as fases com DLCs, a 60 fps (opção "60 FPS" no launcher).
- Áudio, saves, controle e portal (Toy Pad virtual ou o app LegoToypad) funcionam.
- Tela cheia (opção "Tela cheia" no launcher ou F11 no jogo).
- O controle só mexe no jogo com a janela do jogo na frente: ao clicar no app do Toy Pad, o personagem para.
- Launcher, instalador e telas do port em português ou inglês, conforme o idioma do Windows.
- Na primeira visita a uma área nova o driver de vídeo ainda compila pipelines. Isso pode dar um engasgo curto, mas os objetos não somem (sem pop-in).
- Cenas muito pesadas (~6000 draws por quadro) ficam abaixo de 60 fps em processadores de 6 núcleos.
- Algumas cenas abertas (por exemplo a tomada aérea de Metropolis) ficam com muito mais névoa que no PS4. Em investigação.
- Só foi testado em placa AMD. Relatos com NVIDIA e Intel são bem-vindos (abra uma *issue* com o log de `build\logs\`).

## Compilar a partir do código

Requer:
- Visual Studio 2022 ou mais novo, com as ferramentas C++ e o **clang-cl**;
- CMake;
- [Vulkan SDK](https://vulkan.lunarg.com/) 1.4;
- Python 3.

```bat
set VULKAN_SDK=C:\VulkanSDK\1.4.363.0
build.bat                    :: o port -> build\lego_dimensions.exe
tools\dist\build_dist.bat    :: launcher e instalador
```

**Tradutor de shaders** (`tools/shader_compiler_tool`): usa o recompilador de shaders do [shadPS4](https://github.com/shadps4-emu/shadPS4), sem modificações, no commit [`eeecd10`](https://github.com/shadps4-emu/shadPS4/commit/eeecd103f648572b249b422e759e76799274b34c). A única troca é `override/resource_patching_pass.cpp`, que fica nesta pasta.
1. Compile esse commit do shadPS4 (Release, clang-cl).
2. Aponte `SHADPS4_DIR` para ele.
3. Rode `tools\shader_compiler_tool\build.bat`.

**Pacote de distribuição:** `python tools/dist/montar.py` gera o instalador e o zip em `dist\`.

## Licença

GPL-2.0. Veja [LICENSE](LICENSE). As bibliotecas de terceiros e suas licenças estão em [TERCEIROS.md](TERCEIROS.md).

## Aviso

Projeto de fã, sem fins lucrativos. Não tem ligação com o LEGO Group, a TT Games, a Warner Bros. Games ou a Sony Interactive Entertainment. LEGO e LEGO Dimensions são marcas do LEGO Group. PlayStation é marca da Sony. Use só com uma cópia do jogo que você possui.
