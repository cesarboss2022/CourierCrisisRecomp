# Fetches the pinned framework checkouts (psxrecomp, recomp-ui) into the
# repository root and applies this project's patches to them.
#
# The checkouts are disposable build inputs (gitignored): every run resets them
# to the pinned commit, so local edits inside psxrecomp/ or recomp-ui/ are lost.
# Put framework changes in framework/patches/ instead.
#
# Override a clone URL (for example a local mirror) with
#   $env:COURIER_PSXRECOMP_URL / $env:COURIER_RECOMP_UI_URL
$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)

function Invoke-Git {
    & git @args
    if ($LASTEXITCODE -ne 0) { throw "git $($args -join ' ') failed (exit $LASTEXITCODE)" }
}

if (!(Get-Command git -ErrorAction SilentlyContinue)) { throw 'git is required.' }

$pins = Get-Content (Join-Path $Root 'framework/PIN') |
    Where-Object { $_ -and $_ -notmatch '^\s*#' }

foreach ($line in $pins) {
    $name, $commit, $url = $line.Trim() -split '\s+'
    $override = [Environment]::GetEnvironmentVariable(
        'COURIER_' + ($name.ToUpper() -replace '-', '_') + '_URL')
    if ($override) { $url = $override }

    $dir = Join-Path $Root $name
    if (!(Test-Path (Join-Path $dir '.git'))) {
        Write-Host "Cloning $name from $url"
        Invoke-Git clone --quiet $url $dir
    }

    & git -C $dir cat-file -e "$commit`^{commit}" 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Fetching $name $commit"
        Invoke-Git -C $dir fetch --quiet origin $commit
    }
    Invoke-Git -C $dir checkout --quiet --force --detach $commit
    Invoke-Git -C $dir clean --quiet -fd
    Invoke-Git -C $dir submodule update --quiet --init --recursive

    $patch = Join-Path $Root "framework/patches/$name-courier.patch"
    if (Test-Path $patch) {
        # --ignore-whitespace: the patch has LF line ends, a Windows checkout may have CRLF.
        Invoke-Git -C $dir apply --check --ignore-whitespace $patch
        Invoke-Git -C $dir apply --ignore-whitespace --whitespace=nowarn $patch
        Write-Host "$name @ $($commit.Substring(0,8)) + $name-courier.patch"
    } else {
        Write-Host "$name @ $($commit.Substring(0,8))"
    }
}
