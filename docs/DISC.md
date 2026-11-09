# Your disc

The game is not included. You need a dump of your own copy of
**Courier Crisis - The Saga of the Modern Fatalist (USA)**, serial **SLUS-00442**.

## Accepted formats

`.cue` (recommended), `.bin`, `.img`, `.iso`, `.chd`.

For a BIN/CUE dump keep the `.cue` and the `.bin` in the same folder and select
the **`.cue`**. The `.cue` must name the `.bin` correctly.

## What the build checks

The data track of the expected dump:

| | |
|---|---|
| Size | 377,717,088 bytes |
| MD5 | `b12ae82174ee512c1ca79326cbc7e01d` |
| SHA-1 | `128d0808e4ec8b8f84881bda2ea880ee4cfcd89b` |
| CRC32 | `4996ad1d` |
| Boot executable | `SLUS_004.42` |

A different region or a modified dump may be rejected or behave differently.

## What is created from it

`build.ps1` extracts the boot executable into `disc/` and generates the
recompiled code into `generated/`. At run time the game also turns code streamed
from the disc into native code in `cache/`. All of these are gitignored and are
derived from your copy of the game: do not commit or share them.