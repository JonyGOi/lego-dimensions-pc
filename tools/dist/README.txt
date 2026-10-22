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
  The launcher's "Language" selector picks one by hand (the launcher reopens when its own text language
  changes). The languages other than Portuguese and English read the game's original text: a dump with a
  fan translation (a replaced text.csv with a single column) closes the game in those languages. Use
  Portugues or English in that case.

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
  - (Optional) DS4/Xbox controllers (up to 4 for local co-op) and, for the portal, the real Toy Pad on
    USB, or the LegoToypad app, or assets\portal.json.

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
  - "Resolution" is the size of the game window (1280x720 by default). The game renders at 1920x1080 as
    on the PS4 and the picture is scaled to the window. In fullscreen the monitor size is used.
  - "Update" looks for a newer version of the port in the GitHub releases and, if there is one, downloads
    and installs it on top. Saves, prepared files, options and portal.json stay as they are; close the
    game first.
  - "Skip all cutscenes" lets you skip any cutscene (the game's own skip button), even the ones the game
    would only let you skip after watching them or at 100%.
  - The controller only drives the game while the game window is in front: when you click the Toy Pad
    app (or any other window) the character stops. A button that was already held when you come back
    only counts after it is released.
  - Without "Show console", each session's log goes to build\logs\ (helps to track down problems).
  - "Icons" changes the button icons the game shows: Automatic uses the ones of the connected controller
    (Xbox 360, Xbox One/Series, PS3, PS4/PS5, or Wii U for Nintendo controllers); or pick one platform. The
    icons come from your own game files (Prepare builds them; PLAY builds them if missing). There are no
    keyboard icons.
  - Local co-op: plug in a second controller and press Options on it; player 2 joins in split screen
    (up to 4 controllers, one per player).
  - Real Toy Pad: plugged into USB before the game starts, it is used on its own, with no extra driver
    (portal.json and the app are left out). If you installed the WinUSB driver with Zadig (Cemu /
    RPCS3), switch the portal back to the default Windows driver. --no-real-toypad uses the virtual one.
  - With the LegoToypad app 1.6 or newer ("Toypad LEDs" setting on), the portal lights show in the app
    the way the game lights them: colour, flashes and fades.
  - The first time you visit each area the graphics driver still compiles new pipelines (it may hitch
    a little); they are stored and prepared on the loading screen the next times.
  - If you notice stutters, send the session log (build\logs\): the [ENGASGO], [QUADRO-LENTO] and
    [TRAVADA-PILHAS] lines show where the time went.
  - Saves are kept in assets\savedata0.
  - "FPS and info counter" shows, in the top-left corner, FPS, frame time (with a graph), graphics card
    temperature / load / clock / power, video memory, CPU load and RAM. F10 turns it on and off in game.
    Graphics card readings come from the driver (AMD and NVIDIA); CPU temperature and power need a
    separate driver (like Afterburner's), so they are not shown.
  - "Player nicknames..." changes the name the game shows instead of Player1, Player2... (up to 16
    letters; stored in jogadores.txt in the port folder).
  - LD PC Transfer (button in the launcher, or LD_PC_Transfer.exe): converts the progress between this
    port, PS4 / shadPS4, Xbox 360 (Xenia), Wii U (Cemu, updated game) and PS3 (RPCS3, experimental).
    Pick the source save folder (or a folder above it) and the destination; the original is not changed
    and whatever the destination had goes to the saves_backup folder. Only the progress comes over; the
    options go back to default. On a real console the save still has to be re-signed (Apollo / Save
    Wizard / Horizon). Xbox One is not possible: its saves live in the cloud, encrypted.
  - Mods ("Mods" button in the launcher): takes the PS4 / shadPS4 LEGO Dimensions mods. "Install a mod
    from a .zip" puts the mod in the port's mods\ folder (or extract it there yourself, one folder per
    mod); clicking its name turns it on / off. Mods already in the shadPS4 mods folder (CUSA01176-mods,
    next to the game) or in CUSA01176\Mods work too. Accepted kinds: a .HDR + .DAT pack (e.g. made with
    BrickVault) and loose files laid out like the game folder (data\, sce_sys\). The game and its
    original packs are not changed: the port just shows the game the mod's files instead of the original
    ones. The launcher prepares the mods when you click PLAY (the first time with a new mod takes a few
    seconds). A mod can only replace files the game already has; new files are left out (the launcher
    says so in its log).
  - Use folder names without accents (the port receives paths in ANSI).
