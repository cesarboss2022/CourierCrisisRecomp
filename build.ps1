# Builds CourierCrisis_Recompiled.exe from your own disc.
#
#   powershell -ExecutionPolicy Bypass -File build.ps1 -Disc "D:\Games\Courier Crisis (USA).cue"
#
# 1. fetches and patches the pinned framework (tools/setup_framework.ps1)
# 2. generates the recompiled C from the disc into generated/ (skipped when present)
# 3. compiles the game into build-release/
#
# The first run downloads the portable clang/CMake toolchain (about 760 MB) and
# builds the recompiler, so it needs network access and a few minutes.
param(
    [string]$Disc,
    [string]$BuildDir = 'build-release',
    [switch]$Regenerate
)
$ErrorActionPreference = 'Stop'

$Root = $PSScriptRoot
Set-Location $Root

foreach ($tool in 'git', 'python') {
    if (!(Get-Command $tool -ErrorAction SilentlyContinue)) { throw "Missing prerequisite: $tool" }
}
python -c "import sys; sys.exit(sys.version_info < (3, 11))"
if ($LASTEXITCODE -ne 0) { throw 'Python 3.11 or newer is required.' }

& (Join-Path $PSScriptRoot 'tools/setup_framework.ps1')

$Cli = Join-Path $Root 'psxrecomp/psxrecomp_cli.py'
$Marker = Join-Path $Root 'generated/SLUS_004.42_dispatch.c'
# GAME.EXE, the game's code, is compiled ahead of time into this file (aot/overlays.json).
$Static = Join-Path $Root 'generated/overlays_static.c'

if ($Regenerate -or !(Test-Path $Marker) -or !(Test-Path $Static)) {
    if (!$Disc) { throw 'generated/ is missing or incomplete: pass -Disc <path to your .cue/.chd/.iso>. See docs/DISC.md.' }
    if (!(Test-Path $Disc)) { throw "Disc image not found: $Disc" }
    python $Cli generate --config game.toml --project-root . --disc $Disc
    if ($LASTEXITCODE -ne 0) { throw 'generate failed' }
}

python $Cli rebuild --config game.toml --project-root . --build-dir $BuildDir --no-pgo
if ($LASTEXITCODE -ne 0) { throw 'rebuild failed' }

Write-Host ''
Write-Host "Done: $BuildDir\CourierCrisis_Recompiled.exe"
