# Starts the game from build-release with the PGXP depth buffer ("Depth-sorted world")
# and/or seam expansion ("Gap filling") forced on, to compare them by eye.
#
#   powershell -ExecutionPolicy Bypass -File tools\test_pgxp_options.ps1 -Seam fine
#   powershell -ExecutionPolicy Bypass -File tools\test_pgxp_options.ps1 -Depth
#   powershell -ExecutionPolicy Bypass -File tools\test_pgxp_options.ps1 -Depth -Seam wide
#
# The hotkeys for these (9 and F11) only exist in debug-tools builds, so the release
# build reads them from environment variables instead. Close the game between runs.
param(
    [switch]$Depth,
    [ValidateSet('off', 'fine', 'wide')][string]$Seam = 'off',
    [string]$BuildDir = 'build-release'
)
$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
$Exe = Get-ChildItem (Join-Path $Root $BuildDir) -Filter 'Courier*_Recompiled.exe' |
    Sort-Object LastWriteTime -Descending | Select-Object -First 1
if (!$Exe) { throw "No Courier*_Recompiled.exe in $BuildDir. Run .\build.ps1 first." }

$env:PSX_PGXP_DEPTH = if ($Depth) { '1' } else { '0' }
$env:PSX_PGXP_SEAM = @{ off = '0'; fine = '1'; wide = '2' }[$Seam]
Write-Host "PGXP depth buffer: $(if ($Depth) { 'on' } else { 'off' })   seam expansion: $Seam"

Push-Location (Split-Path -Parent $Exe.FullName)
try { & $Exe.FullName } finally { Pop-Location }
