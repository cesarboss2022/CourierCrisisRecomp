# Status

Alpha. The game boots and plays; some areas are unverified.

## Works

- Boot, menus, level play, saves.
- 16:9 widescreen in the first level, title screen and menus.
- 60 fps in levels, stable in a heavy scene on the author's machine.
- `GAME.EXE` compiled ahead of time from the disc (see [BUILD.md](BUILD.md)): checked at
  boot and in the heavy scene; at boot about 72% of samples are in static code and 0.2% in
  the interpreter.
- In-game options menu, launcher theme.

## Not verified

- Widescreen in later levels, and 21:9 / fit modes.
- 60 fps on slower CPUs. The ahead-of-time build removes the interpreter and the warm-up,
  but I only measured on one machine.
- The ahead-of-time coverage: 18 of 19 observed entry points. Code the game generates or
  copies into RAM at run time is not covered and still falls back to the interpreter.
- Controller layouts other than an Xbox pad.
- Linux and macOS: not built.

## Known issues

- The launcher can take 30 to 40 seconds to appear on the author's PC. The delay is
  inside SDL's video/controller initialisation; the cause is not found.
- Native CPU spans are marked experimental by the framework (approximate cycle
  accounting). They apply to the remaining runtime shards, not to the ahead-of-time code.
- The runtime shard cache is rebuilt whenever the recompiler hash changes.
- The executable is unsigned; Windows SmartScreen may warn.

## Framework

Built on PSXRecomp at the commit in [`framework/PIN`](../framework/PIN) with the
patches in [`framework/patches`](../framework/patches).