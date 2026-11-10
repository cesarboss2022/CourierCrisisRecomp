# Changelog

What changed in each release.

## v0.2.1

### Enhancements

- New launcher page, Enhancements: three profiles (Original, Recommended, Maximum) and every picture, motion, audio and filter option as a card with a plain description. The page shows which profile is active
- New Help page in the launcher, with how to start, what to do when something goes wrong, and links
- Launcher shows the game's own icon and its full name, and explains errors such as an old executable that does not contain an enhancement
- The in-game menu (F10) lists only options that apply at once, plus the video rows that relaunch when the menu closes, grouped like the Enhancements page. Options that need a restart are set in the launcher
- Default stick dead zone is now 20% (was about 10%); a gamepad that already has a saved value keeps it

### Known limitations

- The campaign is not fully tested yet, see [STATUS.md](docs/STATUS.md)
- A higher draw distance means more to draw: heavy scenes cost more CPU

## v0.2.0

### Enhancements

- Always High-Detail Textures: buildings keep their sharp close-up textures at any distance (on by default, applies at the next start)
- Draw Distance: draws the world farther, Original, 1.25x, 1.5x, 2.0x, 2.5x or 3.0x (applies at the next start)
- Better PGXP: the game's world now gets perspective-correct textures, and the polygons the game subdivides near the camera are tracked too. About 87% of triangles are sub-pixel precise, up from 53%, with less texture shake
- Widescreen: objects in the side bands are no longer dropped or cut at the original 320-pixel edge, and the sky and the ground gradient fill the full width

### Fixes

- The v0.1.0 package was packed from an older executable than the one it was built with. This release is packed from the current build

### Known limitations

- The campaign is not fully tested yet, see [STATUS.md](docs/STATUS.md)
- A higher draw distance means more to draw: heavy scenes cost more CPU

## v0.1.0

### Enhancements

- Windows x64 native build of Courier Crisis (USA, SLUS-00442) on PSXRecomp
- Widescreen (16:9) with the HUD and menus kept in place
- 60 FPS in levels, with the option "Unlock 60 FPS" in the Frame Rate group of the in-game menu
- The game's code (`GAME.EXE`) is compiled ahead of time from your disc, so it runs native from the first frame
- In-game options menu on F10: display, audio, system, frame rate and quality of life
- Game-specific launcher theme with the cover, the logo and the icon

### Known limitations

- The campaign is not fully tested yet, see [STATUS.md](docs/STATUS.md)