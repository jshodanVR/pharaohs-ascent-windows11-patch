# Pharaoh's Ascent — Windows 11 / Full HD Patch

Unofficial compatibility patch for the **English Windows ISO edition** of Pharaoh's Ascent (1997). The original game is required and is not included.

**[Download patch 1.0](https://github.com/jshodanVR/pharaohs-ascent-windows11-patch/releases/latest)** · **[Guida completa in italiano](GUIDA-ITALIANO.md)**

## What this patch does

- Bypasses the legacy installation/configuration and display startup paths that prevented the tested copy from starting on Windows 11.
- Adds a 1920×1080 borderless output with uniform scaling: the original 1024×768 canvas becomes 1440×1080, with 240-pixel black bars on each side. Images retain their proportions.
- Adds fullscreen toggling with F11 or Alt+Enter and minimizing with F10.
- Creates a separate game folder from the original ZIP or ISO, verifies 194 original files, and expands 55 compressed graphics.

This is an upscale of the original artwork, not a high-resolution remaster. **Legacy in-game movie playback is not fixed; the launcher skips movies with `/NV`.**

## 1. Obtain the correct original game

1. Open the [Pharaoh's Ascent page on My Abandonware](https://www.myabandonware.com/game/pharaoh-s-ascent-rgo).
2. Scroll to **Download Pharaoh's Ascent → Windows Version**.
3. Choose **ISO Version**, listed as approximately **147 MB** when this guide was written.
4. The tested download is **`Pharaoh-s-Ascent_Win_EN_ISO-Version.zip`** (154,341,818 bytes). It contains **`Pharaohs Ascent.iso`** (182,626,304 bytes).
5. Keep that ZIP. You can select it directly in the patch installer; you do not need to extract or mount the ISO or run the old setup program.

Only the original English ISO edition is supported. A filename alone does not prove compatibility: the installer checks the required files by SHA-256. German editions, demos, repacks, previously modified installations, and other releases are not supported by this version. Obtain and use the original game in accordance with the rights applicable to your copy.

Optional exact-download verification:

| File | SHA-256 |
|---|---|
| Original game ZIP | `511e70a7195a1e443f2079602a3b44fffc409945898b8c6b0bfb8dd802ae12c4` |
| ISO inside it | `b80a04ab3fd199966be5917a411e5d9433ffe1c53a3a4477ce83b600681c60e7` |

For example, use `Get-FileHash -Algorithm SHA256 "C:\path\Pharaohs Ascent.iso"` in PowerShell. If the hosting site changes ZIP compression, the outer ZIP hash may change; the installer still decides compatibility by checking the original game files.

## 2. Download and extract the patch

1. Open this repository's **[latest release](https://github.com/jshodanVR/pharaohs-ascent-windows11-patch/releases/latest)**.
2. Under **Assets**, download **`Pharaoh-Ascent-W11-Patch-1.0.zip`**. This is the small patch, about 32 KB, not the 147 MB game download.
3. Right-click the patch ZIP in File Explorer and choose **Extract All**.
4. Open the extracted folder containing `INSTALL.cmd`, `Install.ps1`, `patch.json`, `PharaohHD.dll`, and `src`.

Use the named release asset, rather than GitHub's automatically generated **Source code (zip)**. Keep all patch files together, including the `src` folder. Do not run the installer inside File Explorer's ZIP preview.

## 3. Install

1. Double-click **`INSTALL.cmd`**.
2. In the file picker, select the **original game ZIP** from step 1, or its extracted `Pharaohs Ascent.iso`. Do not select the patch ZIP.
3. Select a writable **parent folder**, such as Documents or a Games folder.
4. The installer creates a new **`Pharaohs-Ascent-Windows11`** folder inside it. If that folder already exists, choose another parent folder: existing games and saves are deliberately never overwritten.
5. Wait for the success message. Verification, copying, executable patching and graphics expansion happen automatically.
6. Open the newly created game folder and double-click **`PLAY.cmd`**.

No Windows XP installation, administrator account, mounted ISO, Python, external archive utility, system DLL replacement or codec installation is required. Windows PowerShell 5.1 and `expand.exe`, supplied with Windows 11, are used. The launcher applies a process-only PowerShell execution-policy option; it does not change the system's execution-policy setting.

The original ZIP/ISO is read-only. The new game folder contains everything needed to run this installation; keep the original download for future reinstalls. Always start with `PLAY.cmd` so movies remain skipped.

## 4. Display and controls

| Action | Key / setting |
|---|---|
| Enter selected room | Enter |
| Move | Arrow keys / numeric keypad; turn Num Lock off |
| Jump | Space + direction |
| Fire | Ctrl + direction |
| Crouch / stand | Down + Space / Up |
| Restart room | Ctrl+Z |
| Return to pyramid | Ctrl+X; choose Exit there to quit |
| Fullscreen at current monitor resolution | F11 or Alt+Enter |
| Minimize | F10 |

Move the pointer to the top edge to reveal the original menu. Choose **Controller → Keyboard** if needed. Push the round ankh stone into the keyhole stone to open the exit.

Initial output is a 1920×1080 borderless window. Fullscreen uses your current monitor resolution while preserving the game's proportions. Black side bars are expected. On smaller displays, output is limited to the available screen size.

To change startup display settings, close the game and edit `PharaohHD.ini` in the installed game folder:

```ini
[Display]
Width=1920
Height=1080
Fullscreen=0
```

Set `Fullscreen=1` for fullscreen startup. Leave debug logging disabled during normal play.

## Troubleshooting

| Problem | What to do |
|---|---|
| Destination already exists | Choose a different parent folder. Installation does not overwrite or upgrade an existing folder. |
| Unsupported or damaged game file | Use the original English ISO download above. Re-download if incomplete. Do not bypass hash checks or mix files from another release. |
| Missing patch files or damaged DLL | Extract the entire patch ZIP again, including `src`. |
| Game does not start | Use `PLAY.cmd` from the new game folder, not the original CD executable. Check that installation reported success. |
| Character does not move | Select Controller → Keyboard, turn Num Lock off, click the game and hold an arrow briefly. |
| Movies fail | Keep movies disabled and use `PLAY.cmd`. This release does not fix video playback. |
| Old help file will not open | The original `.HLP` file is retained; a WinHelp reader is not supplied. |

If installation still fails, report the exact error and Windows version in [Issues](https://github.com/jshodanVR/pharaohs-ascent-windows11-patch/issues). Include `PATCH-INSTALL-REPORT.json` if it exists. Do not upload the game, ISO, personal saves or private information.

## Saves, removal and limitations

Back up the installed game's `saved` folder before removing or replacing an installation. No personal saves are included or migrated automatically. Close the game, preserve any saves you need, then remove only the new game folder and extracted patch folder to uninstall. The original download remains unchanged. Original per-user game preferences/save behavior are retained; registry-free operation is not promised.

Tested on one Windows 11 x64 PC: startup, room selection, first rooms, shooting, keyboard movement confirmed by the player, and aspect-preserving display. The complete campaign, other PCs, ARM Windows, joysticks and multi-monitor setups have not been exhaustively tested. See [test report](patch/TEST-REPORT.txt).

## Source and technical details

The release contains the 32-bit process-local GDI adapter, an installer, 13 executable overwrite records totaling 542 bytes, original-asset hashes and source code. It contains no complete game executable, artwork, sound, movies or saves. The installer reconstructs the patched executable locally from the verified original.

Source is under [`patch/src`](patch/src); [build notes](patch/src/BUILD.txt) describe TinyCC 0.9.27 x86 compilation. Rebuilding the supplied adapter source reproduced the distributed DLL's SHA-256. Advanced command-line installation is documented in [README-EN.txt](patch/README-EN.txt).

The game and its trademarks belong to their respective owners. This project is unofficial and is not affiliated with the original publisher, Microsoft or My Abandonware. The original game is not relicensed.
