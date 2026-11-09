# Playing

## First launch

Extract the whole zip somewhere writable (not `C:\Program Files`), run
`CourierCrisis_Recompiled.exe`, pick your disc and press Play. Saves, settings and
the shard cache stay beside the exe. To update, extract the new zip over the old
folder: saves and settings are kept.

## Performance

The game's code is compiled ahead of time from your disc when the game is built, so it
runs native from the first frame: there is no warm-up and no first-visit slowdown. 60 fps
in heavy scenes still depends on a fast CPU thread.

## In-game menu (F10)

| Group | What |
|---|---|
| Display / Graphics | Internal resolution, filtering, anti-aliasing, PGXP |
| Audio | Volumes |
| Picture | **Always High-Detail Textures** and **Draw Distance** (both apply the next time the game starts) |
| Frame Rate | **60 FPS** feature and **Unlock 60 FPS** option (applies at once) |
| Visual / Quality of Life | Extra enhancements |

Every row you touch is saved to `mods/state.toml`.

## 60 FPS

On by default in levels. The game's own cap is one flip per two vblanks; the mod
lowers it to one. Turn **Unlock 60 FPS** off for the original 30 fps. Game speed is
the same either way.

## High-detail textures

The game shows a building with a coarse texture from far away and swaps in a sharper one when you get close.
**Always High-Detail Textures** (on by default) keeps the sharp one at every distance. It applies when the game
starts, so restart after changing it. It draws more detail, so a heavy scene can cost a little more CPU.

## Draw distance

The game draws streets, buildings and cars only up to a distance it adapts between 2000 and 4000 units, so
things appear as you move. **Draw Distance** multiplies that range: Original, 1.25x, 1.5x, 2.0x, 2.5x or 3.0x.
It applies when the game starts, so restart after changing it. Farther means more to draw: a heavy scene costs
more CPU, so lower it if the frame rate drops.

## Widescreen

The game's own culling and clipping only knew 320 pixels, so the picture's side bands would show cut objects and an empty sky.
Those tests now use the revealed margin and the sky is spread over the full width, so 16:9 looks like the middle of
the screen does. At 4:3 nothing changes.

16:9 by default. Validated in the first level and the menus; other levels and
21:9 have not been checked.

## If something is wrong

- *Disc not recognised*: use the `.cue`, not the `.bin`; see [DISC.md](DISC.md).
- *Black screen*: update the GPU driver; try another renderer in the launcher.
- *Music cuts or fps swings*: lower the internal resolution or turn off 60 FPS.
- *Launcher is slow to appear* (tens of seconds on some PCs): known, see
  [STATUS.md](STATUS.md).