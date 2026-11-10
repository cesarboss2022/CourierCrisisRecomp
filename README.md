<p align="center">
  <img src="docs/images/logo.png" alt="Courier Crisis Recompiled" width="520">
</p>

<p align="center">
  A native PC recompilation of <b>Courier Crisis - The Saga of the Modern Fatalist</b> (PlayStation, 1997)
</p>

<p align="center">
  <a href="docs/STATUS.md"><img src="https://img.shields.io/badge/status-alpha-orange?style=for-the-badge" alt="Status: alpha"></a>
  <a href="docs/STATUS.md"><img src="https://img.shields.io/badge/campaign-not%20fully%20tested%20yet-yellow?style=for-the-badge" alt="Campaign: not fully tested yet"></a>
</p>

<p align="center">
  <a href="https://github.com/cesarboss2022/CourierCrisisRecomp/actions/workflows/build.yml"><img src="https://img.shields.io/github/actions/workflow/status/cesarboss2022/CourierCrisisRecomp/build.yml?label=build" alt="Build"></a>
  <a href="https://github.com/cesarboss2022/CourierCrisisRecomp/releases"><img src="https://img.shields.io/github/v/release/cesarboss2022/CourierCrisisRecomp" alt="Release"></a>
  <a href="https://github.com/cesarboss2022/CourierCrisisRecomp/releases"><img src="https://img.shields.io/github/downloads/cesarboss2022/CourierCrisisRecomp/total?cacheSeconds=1800" alt="Downloads"></a>
  <a href="https://github.com/cesarboss2022/CourierCrisisRecomp/releases"><img src="https://img.shields.io/badge/platform-Windows%20x64-1a231b" alt="Windows x64"></a>
  <a href="https://scorecard.dev/viewer/?uri=github.com/cesarboss2022/CourierCrisisRecomp"><img src="https://img.shields.io/ossf-scorecard/github.com/cesarboss2022/CourierCrisisRecomp?label=OpenSSF" alt="OpenSSF Scorecard"></a>
</p>

## Quick start

1. Download from [Releases](https://github.com/cesarboss2022/CourierCrisisRecomp/releases):
    - **Windows:** unpack [**CourierCrisisRecomp-win-Portable.zip**](https://github.com/cesarboss2022/CourierCrisisRecomp/releases/latest/download/CourierCrisisRecomp-win-Portable.zip) to a folder you can write to, and run `CourierCrisis_Recompiled.exe`
2. Pick your **Courier Crisis (USA, SLUS-00442)** disc image in the launcher it opens
3. Press **Play**

**No game data is included.** Supply your own disc image, see [docs/DISC.md](docs/DISC.md). No retail BIOS
is needed: the game runs on the bundled MIT-licensed OpenBIOS.

## Launcher

- Pick your disc image and press **Play**; the launcher also sets the video, audio and controls
- A game-specific theme with the cover and logo
- Keyboard, XInput and SDL gamepads, with remapping in the launcher
- Updates keep your saves and settings: extract the new zip over the old folder

Inside the game, **F10** opens the options menu: display, audio, system, frame rate and quality of life.

## Settings

| | Setting | Default | What it changes |
| --- | --- | --- | --- |
| **Picture** | Internal resolution | 4x | Up to 32x the original resolution, limited by your graphics card |
| | Aspect ratio | 16:9 | The picture fills a wide screen, with the HUD and menus kept in place |
| | Anti-aliasing | FXAA | Smoother edges |
| | Image filtering | Configurable | Texture filtering and dithering, under Mods |
| | High-detail textures | On | Buildings keep their sharp close-up textures at any distance, instead of swapping when you get near |
| | Draw distance | Original | Draws the world farther, from 1.25x to 3.0x. Needs a restart |
| **Motion** | 60 FPS gameplay | On | Levels run at up to 60 FPS instead of 30. Game speed does not change |
| | Smooth motion | Off | Adds in-between frames, **experimental** |
| **World** | Sub-pixel geometry (PGXP) | On | Steadies the polygon edges that wobble on the PlayStation |
| | Perspective-correct textures | On | Textures keep their shape near the viewer |
| **And more** | | | CRT filter, post-processing, enhanced audio, faster loading and others, all in the [player's guide](docs/PLAYING.md) |

The game's code is compiled to native code ahead of time, so it runs native from the first frame: there is
no warm-up and no slowdown the first time you enter a level.

---

<p align="center">
  <a href="docs/PLAYING.md">Player's guide</a> ·
  <a href="docs/DISC.md">Supported disc</a> ·
  <a href="CHANGELOG.md">Changelog</a> ·
  <a href="docs/STATUS.md">Status</a> ·
  <a href="docs/BUILD.md">Build from source</a>
</p>
