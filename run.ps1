# Starts the game you built with .\build.ps1.
#
#   powershell -ExecutionPolicy Bypass -File .\run.ps1
#   powershell -ExecutionPolicy Bypass -File .\run.ps1 -Disc "D:\Games\Courier Crisis (USA).cue"
#
# Without -Disc the launcher opens and asks for your disc image.
param(
    [string]$Disc,
    [string]$BuildDir = 'build-release'
)
$ErrorActionPreference = 'Stop'

$Root = $PSScriptRoot
# A local build is named after the window title (Courier_Crisis_Recompiled.exe); the release
# package renames it to CourierCrisis_Recompiled.exe. Take the newest one in the build folder.
$Exe = Get-ChildItem (Join-Path $Root $BuildDir) -Filter 'Courier*_Recompiled.exe' -ErrorAction SilentlyContinue |
    Sort-Object LastWriteTime -Descending | Select-Object -First 1
if (!$Exe) { throw "No Courier*_Recompiled.exe was found in $BuildDir. Run .\build.ps1 first." }
$Runtime = $Exe.FullName

$GameArguments = @()
if ($Disc) {
    if (!(Test-Path $Disc)) { throw "Disc image not found: $Disc" }
    $GameArguments = @('--game', (Join-Path (Split-Path -Parent $Runtime) 'game.toml'), '--disc', (Resolve-Path $Disc).Path)
}

Push-Location (Split-Path -Parent $Runtime)
try { & $Runtime @GameArguments } finally { Pop-Location }
