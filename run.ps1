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
$Runtime = Join-Path $Root "$BuildDir/CourierCrisis_Recompiled.exe"
if (!(Test-Path $Runtime)) { throw "CourierCrisis_Recompiled.exe was not found in $BuildDir. Run .\build.ps1 first." }

$GameArguments = @()
if ($Disc) {
    if (!(Test-Path $Disc)) { throw "Disc image not found: $Disc" }
    $GameArguments = @('--game', (Join-Path (Split-Path -Parent $Runtime) 'game.toml'), '--disc', (Resolve-Path $Disc).Path)
}

Push-Location (Split-Path -Parent $Runtime)
try { & $Runtime @GameArguments } finally { Pop-Location }
