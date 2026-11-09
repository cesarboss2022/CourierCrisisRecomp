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
| Frame Rate | **60 FPS** feature and **Unlock 60 FPS** option (applies at once) |
| Visual / Quality of Life | Extra enhancements |

Every row you touch is saved to `mods/state.toml`.

## 60 FPS

On by default in levels. The game's own cap is one flip per two vblanks; the mod
lowers it to one. Turn **Unlock 60 FPS** off for the original 30 fps. Game speed is
the same either way.

## Widescreen

16:9 by default. Validated in the first level and the menus; other levels and
21:9 have not been checked.

## If something is wrong

- *Disc not recognised*: use the `.cue`, not the `.bin`; see [DISC.md](DISC.md).
- *Black screen*: update the GPU driver; try another renderer in the launcher.
- *Music cuts or fps swings*: lower the internal resolution or turn off 60 FPS.
- *Launcher is slow to appear* (tens of seconds on some PCs): known, see
  [STATUS.md](STATUS.md).