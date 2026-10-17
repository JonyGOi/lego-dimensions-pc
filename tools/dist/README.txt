LEGO Dimensions PC - unofficial port of the PS4 LEGO Dimensions to Windows
==========================================================================

WHAT IS INCLUDED
  The port (build\lego_dimensions.exe), the launcher, the preparation tools and a portable Python.
  The game and the DLCs are NOT included: you need YOUR OWN dump of the PS4 LEGO Dimensions.

LANGUAGE
  The launcher, the installer and the port's own screens are in Portuguese when Windows is in
  Portuguese and in English otherwise. The game itself uses your Windows language when the game has it
  (English, French, German, Spanish, Italian, Dutch, Danish and Portuguese); any other language
  falls back to English.

WHAT YOU NEED
  - Windows 10 (1803 or newer) or Windows 11, 64-bit.
  - A graphics card with Vulkan 1.3 and an up-to-date driver. Only tested on an AMD Radeon RX 6600.
  - 16 GB of RAM (the game uses about 5 GB; 8 GB may work, but Windows will use the page file and you
    will get stutters).
  - The game dump: the CUSA01176 folder (the one with eboot.bin, sce_sys and data\chunk*).
    Tested version: 01.24. Other versions may not work.
    Two layouts work:
      a) all in one (the update already inside the dump: data\chunk1 with PATCH.DAT, PATCH0.DAT...);
      b) base game + update in a separate folder, CUSA01176-patch (shadPS4 layout), inside the game
         folder or next to it. The launcher finds it on its own; if it has another name, choose it
         under "Update". Without the update (version 01.00) the port does not run.
  - (Optional) the DLCs: a folder with one subfolder per DLC, each with sce_sys\param.sfo and the
    DLCnn.DAT2 / DLCnn.HDR2 files - the same layout as the shadPS4 addcont folder.
  - (Optional) a DS4/Xbox controller and the LegoToypad app for the portal (without it, the port uses
    assets\portal.json).

HOW TO USE
  1. Open LegoDimensionsLauncher.exe.
  2. Under "Game folder", choose the CUSA01176 folder. The game art shows up at the top.
  3. Under "Update", check the CUSA01176-patch folder (only if the update is separate; it stays empty
     when the dump already includes the update).
  4. Under "DLC folder", choose the DLC folder (if the shadPS4 one exists, it is already filled in).
  5. Click Prepare (first time only). This:
       - converts the eboot.bin from your dump (no keys involved: the dump is already decrypted);
       - extracts the shaders from the game and DLC packages and translates all of them to Vulkan
         (the AOT pack).
     The translation is the long part: 20 to 60 minutes, depending on the CPU. Only once.
  6. Click PLAY. The loading screen prepares the shaders and pipelines before the game starts.

TIPS
  - "60 FPS" turns on 60 frames per second (the original game runs at 30).
  - "Fullscreen" starts the game in fullscreen. In game, F11 switches between fullscreen and window.
  - The controller only drives the game while the game window is in front: when you click the Toy Pad
    app (or any other window) the character stops. A button that was already held when you come back
    only counts after it is released.
  - Without "Show console", each session's log goes to build\logs\ (helps to track down problems).
  - The first time you visit each area the graphics driver still compiles new pipelines (it may hitch
    a little); they are stored and prepared on the loading screen the next times.
  - If you notice stutters, send the session log (build\logs\): the [ENGASGO], [QUADRO-LENTO] and
    [TRAVADA-PILHAS] lines show where the time went.
  - Saves are kept in assets\savedata0.
  - Use folder names without accents (the port receives paths in ANSI).
