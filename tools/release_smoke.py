#!/usr/bin/env python3
"""Start a packed release as a player would and write whether it ran, for package_release.py --publish.

The package is copied first and only the copy is tested and named by its digest. It is unpacked into a folder of its
own. Every file its MANIFEST.json names must be there unchanged and its version must be the release's. The game
then starts muted and hidden from power-on with the disc: it must keep running, its frame counter must keep
advancing and the runtime must report no fatal error. The result is written as JSON with the package's digest and its
MANIFEST.json's, and a failed or broken run replaces an earlier pass.
"""

import argparse
import datetime
import hashlib
import json
import math
import os
import shutil
import subprocess
import sys
import tempfile
import time
import zipfile
from dataclasses import asdict, dataclass, field
from pathlib import Path

EXECUTABLE = "CourierCrisis_Recompiled.exe"
HEARTBEAT = "psx_freeze_heartbeat.json"
REQUIRED_CHECKS = ("files", "version", "start")
MIN_SECONDS = 25.0
MIN_FRAMES = 600
SETTLE_SECONDS = 8.0


@dataclass
class Check:
    """One check's name, whether it passed and what it saw."""

    name: str
    passed: bool
    detail: str
    seconds: float | None = None


def judge_start(code: int | None, ran: float, frames: list[int], fatal: object) -> tuple[bool, str]:
    """Whether a start ran: still alive at the end, frames drawn and still advancing, no fatal error."""
    problems = [
        *([f"ended with {code}"] if code is not None else []),
        *(["the runtime reported a fatal error"] if fatal else []),
        *(["no heartbeat"] if not frames else []),
        *([f"only {frames[-1]} frames"] if frames and frames[-1] < MIN_FRAMES else []),
        *(["the frame counter stopped"] if len(frames) > 1 and frames[-1] <= frames[0] else []),
    ]
    return not problems, f"{', '.join(problems) or 'ran'} after {ran:.1f} s, frames {frames[:1]}..{frames[-1:]}"


@dataclass
class Smoke:
    """A packed release, unpacked into a work folder, and the checks run on it."""

    package: Path
    disc: Path
    work: Path
    checks: list[Check] = field(default_factory=list)
    program: Path = field(init=False)
    digest: str = field(init=False)

    def unpack(self) -> None:
        """Copies the package, then unpacks the copy into a folder of its own."""
        snapshot = self.work / self.package.name
        shutil.copy2(self.package, snapshot)
        self.digest = hashlib.sha256(snapshot.read_bytes()).hexdigest()
        self.program = self.work / "program"
        with zipfile.ZipFile(snapshot) as archive:
            archive.extractall(self.program)

    def manifest(self) -> bytes:
        return (self.program / "MANIFEST.json").read_bytes()

    def check_files(self) -> None:
        listed = json.loads(self.manifest())["files"]
        wrong = [
            name
            for name, digest in listed.items()
            if not (self.program / name).is_file()
            or hashlib.sha256((self.program / name).read_bytes()).hexdigest() != digest
        ]
        game = [name for name in listed if name.startswith("mods/bundled/courier.enhancement.")]
        has_exe = (self.program / EXECUTABLE).is_file() and not (self.program / "psxrecomp").exists()
        self.checks.append(
            Check(
                "files",
                not wrong and bool(game) and has_exe,
                f"{len(listed)} files, {len(game)} game mod files, executable {has_exe}, "
                f"{len(wrong)} missing or changed {wrong[:5]}",
            )
        )

    def check_version(self, version: str) -> None:
        stamp = (self.program / "psx_game_version.txt").read_text(errors="replace").strip()
        self.checks.append(Check("version", stamp == version.removeprefix("v"), f"stamp {stamp}, release {version}"))

    def heartbeat_frames(self) -> tuple[int | None, object]:
        path = self.program / HEARTBEAT
        try:
            data = json.loads(path.read_text(errors="replace"))
        except (OSError, ValueError):
            return None, None
        return data.get("frame_count"), data.get("fatal")

    def check_start(self, seconds: float) -> None:
        """Starts the game as the launcher does, hidden and muted, with the disc, and samples its heartbeat."""
        environment = dict(
            os.environ, PSX_HIDDEN_WINDOW="1", PSX_HOST_MUTE="1", SDL_AUDIODRIVER="dummy", PSX_NO_LAUNCHER="1"
        )
        command = [str(self.program / EXECUTABLE), "--no-launcher", "--game", str(self.program / "game.toml")]
        command += ["--disc", str(self.disc)]
        log = self.work / "startup.log"
        frames: list[int] = []
        fatal: object = None
        with log.open("wb") as output:
            process = subprocess.Popen(command, cwd=self.program, env=environment, stdout=output, stderr=output)
            started = time.monotonic()
            while process.poll() is None and time.monotonic() - started < seconds:
                time.sleep(1.0)
                if time.monotonic() - started >= SETTLE_SECONDS:
                    count, fatal = self.heartbeat_frames()
                    if count is not None:
                        frames.append(int(count))
            code = process.poll()
            ran = time.monotonic() - started
            if code is None:
                process.kill()
                process.wait()
        passed, detail = judge_start(code, ran, frames, fatal)
        tail = " | ".join(log.read_text(encoding="utf-8", errors="replace").strip().splitlines()[-2:])[-300:]
        self.checks.append(Check("start", passed, f"{detail}: {tail}", round(ran, 1)))


def verdict_of(version: str, package: Path, digest: str | None, manifest: bytes | None, checks: list[Check]) -> dict:
    """The result: a pass only when every check that is needed ran and passed."""
    ran = {check.name for check in checks if check.passed}
    return {
        "version": version,
        "platform": "windows",
        "package": package.name,
        "package_sha256": digest,
        "manifest_sha256": hashlib.sha256(manifest).hexdigest() if manifest is not None else None,
        "passed": all(name in ran for name in REQUIRED_CHECKS) and all(check.passed for check in checks),
        "checks": [asdict(check) for check in checks],
        "time": datetime.datetime.now(datetime.UTC).replace(microsecond=0).isoformat(),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", required=True, help="the release tag, such as v0.1.0")
    parser.add_argument("--package", type=Path, required=True, help="the Portable ZIP")
    parser.add_argument("--disc", type=Path, required=True, help="the CUE of a supported disc")
    parser.add_argument(
        "--seconds", type=float, default=30.0, help=f"how long the start runs, {MIN_SECONDS:.0f} at least"
    )
    parser.add_argument("--stamp", type=Path, required=True, help="where the JSON result goes")
    args = parser.parse_args()
    args.stamp.unlink(missing_ok=True)
    if not math.isfinite(args.seconds) or args.seconds < MIN_SECONDS:
        parser.error(f"--seconds must be {MIN_SECONDS:.0f} or more")
    package = args.package.resolve()
    checks: list[Check] = []
    manifest = digest = None
    with tempfile.TemporaryDirectory(prefix="couriercrisis-smoke-") as folder:
        smoke = Smoke(package, args.disc.resolve(), Path(folder), checks)
        try:
            smoke.unpack()
            digest = smoke.digest
            manifest = smoke.manifest()
            smoke.check_files()
            smoke.check_version(args.version)
            smoke.check_start(args.seconds)
        except (OSError, ValueError, KeyError, subprocess.CalledProcessError, zipfile.BadZipFile) as error:
            checks.append(Check("error", False, f"{type(error).__name__}: {error}"))
    verdict = verdict_of(args.version, package, digest, manifest, checks)
    args.stamp.parent.mkdir(parents=True, exist_ok=True)
    args.stamp.write_text(json.dumps(verdict, indent=2) + "\n", encoding="utf-8")
    for check in verdict["checks"]:
        print(f"{'PASS' if check['passed'] else 'FAIL'} {check['name']}: {check['detail']}")
    print(f"{'passed' if verdict['passed'] else 'FAILED'}, written to {args.stamp}")
    return 0 if verdict["passed"] else 1


if __name__ == "__main__":
    sys.exit(main())
