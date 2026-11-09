# Building from source

## Requirements

- Windows 10/11 x64.
- Git, Python 3.11+ and network access for the first build.
- Your disc image, see [DISC.md](DISC.md).
- Nothing else to install: the PSXRecomp CLI downloads a portable clang/CMake/Ninja
  toolchain (about 760 MB) on first use.
- Packaging a release also needs Git for Windows (its `bash.exe`).

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\build.ps1 -Disc "D:\Games\Courier Crisis (USA).cue"
```

Steps, each one also runnable alone:

1. `tools\setup_framework.ps1` clones PSXRecomp and recomp-ui into the repository
   root at the commits in [`framework/PIN`](../framework/PIN) and applies
   `framework/patches/*.patch`. These checkouts are disposable and gitignored: every
   run resets them, so put framework changes in a patch.
2. `psxrecomp_cli.py generate` builds the recompiler, extracts `SLUS_004.42` and
   writes the game C into `generated/` (about 130 MB). It then reads `GAME.EXE`
   from the disc and compiles it ahead of time into `generated/overlays_static*.c`
   (about 70 MB of C, 24k entry points, about 30 s), following
   [`aot/overlays.json`](../aot/overlays.json). Skipped when both already exist;
   pass `-Regenerate` to force it.
3. `psxrecomp_cli.py rebuild` compiles the game into `build-release/` and stages
   `overlay_toolchain/` beside it. Close the game before rebuilding, the link step
   fails with "Permission denied" otherwise.

## The framework patches

| Patch | What it does |
|---|---|
| `psxrecomp-courier.patch` | Native CPU spans for overlay shards, also on the bundled tcc fallback (`tools/compile_overlays.py`, `overlay_dispatch_preamble.c.inc`) |
| `recomp-ui-courier.patch` | The `courier` launcher theme, the in-game options menu (F10) rows, scrolling and wrapped descriptions |

Both are small on purpose. To change one, edit the checkout, then regenerate it:

```powershell
git -C psxrecomp diff --output=..\framework\patches\psxrecomp-courier.patch
```

Moving to a newer framework means updating `framework/PIN`, re-running
`setup_framework.ps1` and fixing any patch that no longer applies.

## Editing the plugins

`src/mods/*.c` are compiled into the exe through `CMakeLists.txt`. After changing one,
re-run `.\build.ps1` (no `-Disc` needed once `generated/` exists).

## Why `GAME.EXE` is compiled ahead of time

The boot EXE (`SLUS_004.42`, 149 KB) only loads `GAME.EXE` (645 KB, at `0x80010000`),
where the game actually lives. Without the ahead-of-time step that code runs in the
interpreter until a background compiler turns it into shards one visit at a time.
`aot/overlays.json` declares `GAME.EXE` as one image with its SHA-256, and
`generate` compiles every function it can find. Coverage was checked against what the
game really executes: 18 of the 19 observed entry points are served. The 19th is 12
instructions at `0x800027B4`, outside `GAME.EXE`, which stays on the runtime path.

The runtime shard cache (`cache/`) still exists for that remainder. It depends on a
hash of the recompiler; if a rebuild changes the hash, delete `cache/`.

## Continuous integration

No runner has the game, so CI never builds the game itself (the same as DisruptorRecomp).
`.github/workflows/build.yml` fetches the pinned framework and applies the patches, checks
the plugins against the patched headers and the metadata, and compiles every source of the
runtime with no game code linked (`-DPSXRECOMP_REQUIRE_GAME_C=OFF`). `ruff` checks the Python
tools. CodeQL, Scorecard and zizmor run as in DisruptorRecomp, and Dependabot keeps the pinned
actions up to date.

## Releases

A release is built on the maintainer's machine from their own disc, then checked, signed and
uploaded. It goes in two runs:

1. Pack. Needs a clean checkout, a `## v0.1.0` section in `CHANGELOG.md` and `VERSION` to match:

   ```powershell
   python tools/package_release.py --version v0.1.0
   ```

   `tools/package_release.ps1` builds the public channel in `build-dist/` (no debug tools)
   and zips it with the framework's packager. The script adds `MANIFEST.json` (a SHA-256 for
   every file) and writes `dist/releases/v0.1.0/windows/packages/` with the zip and `packed.json`.
2. Smoke-test that file, push the commit and wait for the `build` workflow:

   ```powershell
   python tools/release_smoke.py --version v0.1.0 `
       --package dist/releases/v0.1.0/windows/packages/CourierCrisisRecomp-win-Portable.zip `
       --disc <cue> --stamp dist/releases/v0.1.0/windows.smoke.json
   ```

   It tests a copy: every file in `MANIFEST.json` must be there unchanged, the version stamp
   must match, and the game starts hidden and muted from power-on with your disc and must keep
   running with its frame counter advancing.
3. Publish. This packs nothing: it signs the zip with Sigstore (`cosign`, a browser opens) and
   creates a **draft** release with `gh`:

   ```powershell
   python tools/package_release.py --version v0.1.0 --publish --identity <your account>
   ```

   It stops unless the packing came from a clean checkout of `HEAD`, the zip is unchanged,
   the smoke test passed on that exact zip and the newest `build` run on the commit is green.
   Release notes are `release/notes/v0.1.0.md` when it exists (kept out of git), else the
   changelog section.

`--sign` alone signs the packing. The zip contains code recompiled from your disc (the boot
EXE and `GAME.EXE`): read [licenses/README.md](../licenses/README.md) before publishing one.