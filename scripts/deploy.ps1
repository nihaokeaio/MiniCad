param(
    [Parameter(Mandatory=$true)]
    [string]$ExePath,

    [Parameter(Mandatory=$true)]
    [string]$QtRoot,

    [Parameter(Mandatory=$true)]
    [string]$OcctPackRoot,

    [Parameter(Mandatory=$true)]
    [string]$OutputDir
)

$ErrorActionPreference="Stop"

$occtRoot = Join-Path $OcctPackRoot "opencascade-7.9.3-vc14-64"
$deployDir = (New-Item -ItemType Directory -Path $OutputDir -Force).FullName

$exeName = Split-Path -Path $ExePath -Leaf
$deployedExe = Join-Path $deployDir $exeName
Copy-Item -LiteralPath $ExePath -Destination $deployedExe

& "$QtRoot/bin/windeployqt.exe" --release --no-translations --no-compiler-runtime $deployedExe
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed"
}

$runtimeFiles = @(
    "$occtRoot/win64/vc14/bin/TKMath.dll"
    "$occtRoot/win64/vc14/bin/TKernel.dll"
    "$OcctPackRoot/3rdparty-vc14-64/tbb-2021.13.0-x64/bin/tbb12.dll"
    "$OcctPackRoot/3rdparty-vc14-64/jemalloc-vc14-64/bin/jemalloc.dll"
)
Copy-Item -LiteralPath $runtimeFiles -Destination $deployDir

Copy-Item "$QtRoot/plugins/platforms/qoffscreen.dll" "$deployDir/platforms/"

@(
    "Qt + OCCT learning demo"
    "Requires Windows x64 and a compatible Microsoft Visual C++ v14 x64 Redistributable."
    "Run: $exeName"
    "Smoke check: $exeName --smoke-test"
) | Set-Content -Path "$deployDir/README.txt" -Encoding utf8

Write-Output "Deployment ready: $deployDir"