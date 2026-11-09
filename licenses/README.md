# Third-party notices

`framework/` holds patches to two upstream projects. Neither is stored here: the
build clones them at the revisions listed in [`framework/PIN`](../framework/PIN).

- **PSXRecomp** (<https://github.com/mstan/psxrecomp>), copyright Matthew Stan,
  is provided under the PolyForm Noncommercial License 1.0.0. The license is
  reproduced in [`PSXRecomp-LICENSE.txt`](PSXRecomp-LICENSE.txt). Anything built
  with it is subject to the same noncommercial terms.
- **recomp-ui** (<https://github.com/RetroPortingToolKit/recomp-ui>) is under the
  MIT License: [`recomp-ui-LICENSE.txt`](recomp-ui-LICENSE.txt).

The build downloads further dependencies (libchdr, libwebp, SDL3, Dear ImGui and
others); their notices stay in the checkouts and are copied into release zips
under `licenses/`.

## Game and artwork

*Courier Crisis - The Saga of the Modern Fatalist* is the property of its
original rights holders. This project does not contain or distribute the game:
you supply your own disc. The cover image and logos under `launcher_assets/` and
`assets/` are used only to identify the game in the launcher; they remain the
property of their owners and will be removed on request.

## Disc-derived code

`generated/` (the recompiled game code, including the ahead-of-time build of
`GAME.EXE` from `aot/overlays.json`) and the overlay shard cache are produced on your
machine from your own disc and are gitignored. Do not commit or redistribute them.
An executable built from them, and so the release zip, contains that recompiled code. The
repository itself never does.