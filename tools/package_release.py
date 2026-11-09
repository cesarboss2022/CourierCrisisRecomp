#!/usr/bin/env python3
"""Pack, check, sign and publish a Windows release of Courier Crisis Recompiled.

A release goes in two runs:

1. Pack without --publish. The framework's bundled-release packager (tools/package_release.ps1) builds the public
   channel from your own disc and zips it; this script adds MANIFEST.json (a SHA-256 for every file), names the
   package, and records its digest and the commit in packed.json beside it.
2. Run tools/release_smoke.py on that package, push the commit and wait for the build workflow, then run again with
   --publish. It packs nothing: it signs and uploads that packing as a draft GitHub release, and stops unless the
   packing came from a clean checkout of this commit, its files are unchanged, the smoke test passed on its file and
   the newest build run on the commit is green.

--sign alone signs the package with Sigstore (cosign, keyless: a browser opens for the sign-in). The release is
built from the maintainer's own disc, so it contains code recompiled from it: read the legal notes in
licenses/README.md before publishing.
"""

import argparse
import hashlib
import json
import re
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PACKAGE_NAME = "CourierCrisisRecomp-win-Portable.zip"
PREFIX = "CourierCrisis"
STAMP_NAME = "windows.smoke.json"


def run(*command: str, check: bool = True, capture: bool = False) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, cwd=ROOT, check=check, capture_output=capture, text=True)


def git(*arguments: str) -> str:
    return run("git", *arguments, capture=True).stdout.strip()


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def release_dir(version: str) -> Path:
    return ROOT / "dist" / "releases" / version / "windows"


def changelog_section(version: str) -> str:
    """The text of the `## vX.Y.Z` section of CHANGELOG.md, or an empty string."""
    text = (ROOT / "CHANGELOG.md").read_text(encoding="utf-8")
    found = re.search(rf"^## {re.escape(version)}\s*$(.*?)(?=^## |\Z)", text, re.MULTILINE | re.DOTALL)
    return found.group(1).strip() if found else ""


def check_version(version: str) -> None:
    if not re.fullmatch(r"v\d+\.\d+\.\d+", version):
        raise SystemExit(f"{version} is not a release tag such as v0.1.0")
    stamp = (ROOT / "VERSION").read_text().strip()
    if version != f"v{stamp}":
        raise SystemExit(f"VERSION says {stamp}, not {version.removeprefix('v')}")
    if not changelog_section(version):
        raise SystemExit(f"CHANGELOG.md has no '## {version}' section with text")


def dirty() -> list[str]:
    return [line for line in git("status", "--porcelain").splitlines() if line.strip()]


def pack(version: str, allow_dirty: bool, skip_build: bool) -> None:
    check_version(version)
    changes = dirty()
    if changes and not allow_dirty:
        raise SystemExit(
            "uncommitted changes (use --allow-dirty for a package that must not be published):\n"
            + "\n".join(changes[:10])
        )
    if not skip_build:
        run("powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(ROOT / "tools/package_release.ps1"))
    built = ROOT / "dist" / f"{PREFIX}-{version.removeprefix('v')}-windows-x64.zip"
    if not built.is_file():
        raise SystemExit(f"the packager did not write {built}")
    out = release_dir(version) / "packages"
    shutil.rmtree(out, ignore_errors=True)
    out.mkdir(parents=True)
    package = out / PACKAGE_NAME
    commit = git("rev-parse", "HEAD")
    with zipfile.ZipFile(built) as source:
        files = {
            entry.filename: hashlib.sha256(source.read(entry)).hexdigest()
            for entry in source.infolist()
            if not entry.is_dir()
        }
        manifest = json.dumps({"version": version, "commit": commit, "files": files}, indent=2, sort_keys=True) + "\n"
        with zipfile.ZipFile(package, "w", zipfile.ZIP_DEFLATED) as target:
            for entry in source.infolist():
                target.writestr(entry, source.read(entry))
            target.writestr("MANIFEST.json", manifest)
    packed = {
        "version": version,
        "package": package.name,
        "package_sha256": sha256(package),
        "commit": commit,
        "dirty": bool(changes),
    }
    (out / "packed.json").write_text(json.dumps(packed, indent=2) + "\n", encoding="utf-8")
    stamp = release_dir(version).parent / STAMP_NAME
    print(f"packed {package} ({packed['package_sha256']})")
    print("next:")
    print(f"  python tools/release_smoke.py --version {version} --package {package} --disc <cue> --stamp {stamp}")
    print("  push the commit, wait for the build workflow, then:")
    print(f"  python tools/package_release.py --version {version} --publish")


def sign(package: Path, identity: str | None) -> Path:
    """Signs the package with cosign (keyless) and, when an identity is given, requires the bundle to name it."""
    bundle = package.with_name(package.name + ".sigstore.json")
    run("cosign", "sign-blob", "--yes", "--bundle", str(bundle), str(package))
    if identity:
        run(
            "cosign", "verify-blob", "--bundle", str(bundle),
            "--certificate-identity", identity,
            "--certificate-oidc-issuer-regexp", ".*",
            str(package),
        )  # fmt: skip
    return bundle


def newest_build_is_green(commit: str) -> bool:
    try:
        found = _build_runs(commit)
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False
    runs = json.loads(found or "[]")
    return bool(runs) and runs[0].get("status") == "completed" and runs[0].get("conclusion") == "success"


def _build_runs(commit: str) -> str:
    return run(
        "gh", "run", "list", "--workflow", "build.yml", "--commit", commit, "--limit", "1",
        "--json", "conclusion,status",
        capture=True,
    ).stdout  # fmt: skip


def publish(version: str, sign_files: bool, identity: str | None) -> None:
    check_version(version)
    out = release_dir(version) / "packages"
    packed = json.loads((out / "packed.json").read_text(encoding="utf-8"))
    package = out / packed["package"]
    problems = []
    if dirty():
        problems.append("the checkout has uncommitted changes")
    if packed["dirty"]:
        problems.append("the packing came from a checkout with uncommitted changes")
    if packed["commit"] != git("rev-parse", "HEAD"):
        problems.append("the packing is of another commit than HEAD")
    if sha256(package) != packed["package_sha256"]:
        problems.append("the package changed after it was packed")
    stamp_path = release_dir(version).parent / STAMP_NAME
    stamp = json.loads(stamp_path.read_text(encoding="utf-8")) if stamp_path.is_file() else {}
    if not stamp.get("passed") or stamp.get("package_sha256") != packed["package_sha256"]:
        problems.append("the smoke test did not pass on this package")
    if not newest_build_is_green(packed["commit"]):
        problems.append("the newest build run on this commit is not green")
    if problems:
        raise SystemExit("not publishing:\n  " + "\n  ".join(problems))
    assets = [str(package)]
    if sign_files:
        assets.append(str(sign(package, identity)))
    notes = ROOT / "release" / "notes" / f"{version}.md"
    body = notes.read_text(encoding="utf-8") if notes.is_file() else changelog_section(version)
    body += (
        "\n\n## Install\n\nExtract the zip to a folder you can write to and run `CourierCrisis_Recompiled.exe`. "
        "No game data is included: supply your own disc (see docs/DISC.md).\n"
    )
    run("gh", "release", "create", version, *assets, "--draft", "--title", version, "--notes", body)
    print(f"draft release {version} created; review it on GitHub before publishing")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--version", required=True, help="the release tag, such as v0.1.0")
    parser.add_argument("--publish", action="store_true", help="sign and upload an existing packing as a draft")
    parser.add_argument("--sign", action="store_true", help="sign the packing with Sigstore")
    parser.add_argument("--identity", help="the account the signature must name")
    parser.add_argument("--allow-dirty", action="store_true", help="pack with uncommitted changes (never publishable)")
    parser.add_argument("--skip-build", action="store_true", help="reuse dist/ from the last tools/package_release.ps1")
    args = parser.parse_args()
    if args.publish:
        publish(args.version, True, args.identity)
    elif args.sign:
        out = release_dir(args.version) / "packages"
        packed = json.loads((out / "packed.json").read_text(encoding="utf-8"))
        if dirty():
            raise SystemExit("not signing: the checkout has uncommitted changes")
        print(f"signed: {sign(out / packed['package'], args.identity)}")
    else:
        pack(args.version, args.allow_dirty, args.skip_build)
    return 0


if __name__ == "__main__":
    sys.exit(main())
