PHARAOH'S ASCENT - WINDOWS 11 / FULL HD PATCH 1.0
Unofficial community compatibility patch, 24 October 2026

WHAT YOU NEED
Your own original English game download. Tested input:
Pharaoh-s-Ascent_Win_EN_ISO-Version.zip, containing Pharaohs Ascent.iso.
Game page: https://www.myabandonware.com/game/pharaoh-s-ascent-rgo
The original DATA/PHARAOH.EXE must have SHA256:
85889dc601adfc0e517d1de19dcdc8d735502c784a200a3c90e20e66bc3810be
The installer also checks all 194 required original files.
Other languages, modified files and different releases are not supported.

INSTALL
1. Extract this PATCH ZIP to a folder. Do not run it from inside the ZIP viewer.
2. Double-click INSTALL.cmd.
3. Select the ORIGINAL GAME ZIP or its extracted ISO.
4. Select a parent folder where you have write permission.
   A NEW Pharaohs-Ascent-Windows11 folder will be created there.
5. Open that new folder and start PLAY.cmd.
No XP installation, ISO mounting, Python, external extractor or administrator
account is required. Uses Windows PowerShell 5.1 and expand.exe from Windows.
The original download is read-only. Existing destinations are refused.
No game saves from the author's PC are included or copied.

DISPLAY
Initial output: a 1920x1080 borderless window.
The 1024x768 game canvas is scaled uniformly to 1440x1080, with 240-pixel
black side bars. Original graphics are upscaled, not remastered.
F11 or Alt+Enter toggles fullscreen at the current monitor resolution.
F10 minimizes. Edit PharaohHD.ini while the game is closed to change
Width, Height or Fullscreen (0=window, 1=fullscreen).
On a smaller display the output is limited to the available screen size.

PLAYING
Enter: enter the selected room. Arrow keys / numeric keypad: movement.
The original manual recommends turning Num Lock off.
Space + direction: jump. Ctrl + direction: fire.
Down + Space: crouch. Up: stand. Ctrl+Z: restart room.
Ctrl+X: return to the pyramid; use Exit to quit.
Move the pointer to the top edge to show the original menu.
Controller > Keyboard selects keyboard input.
Push the round ankh stone into the keyhole stone to open the exit.

KNOWN LIMITATIONS
Always launch through PLAY.cmd: /NV skips the legacy movies.
Do not re-enable movies in the game's preferences. In-game movie playback
is not fixed by this release. Original AVI files are kept in DATA but no
converted movies, codecs or video players are distributed with this patch.
The original .HLP help file is kept; a WinHelp reader is not included.
Tested on one Windows 11 x64 PC: startup, first rooms, mouse room selection,
keyboard movement (also manually confirmed), and aspect-preserving resizing.
The whole campaign, joystick controls, multi-monitor setups, ARM Windows and
other PCs have not been exhaustively tested. Keep backups of your saves.
The game retains its own original per-user preferences and save behavior;
this patch does not promise registry-free operation.

UNINSTALL / SAVES
Close the game. Keep the new game's saved folder if you want its saves.
Delete only the new game folder and the extracted patch folder.
The original ZIP / ISO / CD directory remains unchanged.
To retry installation, choose a different NEW destination. There is no
in-place upgrade or automatic save migration.

COMMAND LINE (optional)
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\Install.ps1 -SourcePath "C:\Downloads\Pharaoh-s-Ascent_Win_EN_ISO-Version.zip" -DestinationPath "C:\Games\Pharaoh-Ascent-W11" -NonInteractive
The parent of DestinationPath must already exist. SourcePath can also be an
ISO or a directory containing the original CD layout (DATA, LEVELS, WAVE...).
A PATCH-INSTALL-REPORT.json file records successful verification counts and
binary hashes. Temporary installation data is removed after success/failure.

CONTENTS / REPRODUCIBILITY
This patch contains a process-local 32-bit GDI adapter, replacement byte ranges
for the specific original EXE, a verified asset manifest, installer and sources.
It contains NO complete game executable, game graphics, audio, movies or saves.
All original game content must be supplied locally by the user.
The game and its trademarks belong to their respective owners. This patch
is not affiliated with Ambertec, Microsoft or My Abandonware.
Source and build notes are in src. The original game is not relicensed.
