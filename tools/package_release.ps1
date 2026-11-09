# Builds the public-channel release into build-dist/ and packs the Windows zip
# into dist/ with the framework's bundled-release packager.
#
#   powershell -ExecutionPolicy Bypass -File tools\package_release.ps1
#
# Needs generated/ (run build.ps1 -Disc ... once first) and Git for
# Windows (its bash runs the packager). No disc, shard cache or retail BIOS
# ends up in the zip.
$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $Root

$Version = (Get-Content VERSION -Raw).Trim()
$Marker = Join-Path $Root 'generated/SLUS_004.42_dispatch.c'
$Static = Join-Path $Root 'generated/overlays_static.c'
if (!(Test-Path $Marker) -or !(Test-Path $Static)) { throw 'generated/ is missing or incomplete: run build.ps1 -Disc <disc> first.' }

& (Join-Path $PSScriptRoot 'setup_framework.ps1')

$git = (Get-Command git).Source
$gitRoot = Split-Path -Parent (Split-Path -Parent $git)
$bash = Join-Path $gitRoot 'bin/bash.exe'
if (!(Test-Path $bash)) { throw 'Git for Windows bash.exe not found next to git.' }
$env:PATH = "$(Split-Path -Parent $bash);$gitRoot\usr\bin;$env:PATH"
$env:PSX_RELEASE_STAGE_PYTHON = (Get-Command python).Source -replace '\\', '/'
$env:RELEASE_VERSION = $Version

python psxrecomp/psxrecomp_cli.py rebuild --config game.toml --project-root . `
    --build-dir build-dist --no-pgo `
    --cmake-extra=-DPSX_RELEASE_CHANNEL=public `
    --cmake-extra=-DPSX_GAME_VERSION=$Version `
    --cmake-extra=-DPSXRECOMP_BIOS_STEMS=OpenBIOS
if ($LASTEXITCODE -ne 0) { throw 'release build failed' }

# objdump (used to check the exe's DLL imports) comes with the portable toolchain.
$bin = (Get-Content toolchain/.psxrecomp-bin -ErrorAction SilentlyContinue)
if ($bin -and (Test-Path $bin)) { $env:PATH = "$bin;$env:PATH" }

& $bash psxrecomp/tools/package_game_release.sh --root . --build-dir build-dist `
    --artifact windows-x64 --zip-prefix CourierCrisis `
    --exe-name CourierCrisis_Recompiled --display-name 'Courier Crisis Recompiled' `
    --recompiler-build psxrecomp/recompiler/build `
    --disc-hint 'your legally owned Courier Crisis (USA) disc' `
    --ship-without-overlay-cache-because "shards are compiled from the player's own disc; shipping them would redistribute disc-derived code" `
    --doc packaging/release/LEIA-ME.txt
if ($LASTEXITCODE -ne 0) { throw 'packaging failed' }

Get-ChildItem dist -Filter "CourierCrisis-$Version-*.zip" |
    ForEach-Object {
        $hash = (Get-FileHash $_.FullName -Algorithm SHA256).Hash
        Write-Host "$($_.FullName)  SHA-256 $hash"
    }
