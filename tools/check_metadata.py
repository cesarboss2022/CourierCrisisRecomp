"""Check that the project's JSON and TOML metadata parse."""

import json
import sys
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
JSON_FILES = (".recomp.json", "catalog_identity.json", "aot/overlays.json")
TOML_FILES = ("game.toml", "game_options.toml", "packaging/release.toml")


def main() -> int:
    for name in JSON_FILES:
        json.loads((ROOT / name).read_text(encoding="utf-8-sig"))
    for name in TOML_FILES:
        tomllib.loads((ROOT / name).read_text(encoding="utf-8"))
    for manifest in sorted((ROOT / "mods").rglob("manifest.toml")):
        tomllib.loads(manifest.read_text(encoding="utf-8"))
    print("metadata ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
