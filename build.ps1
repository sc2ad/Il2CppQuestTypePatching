Param(
    [Parameter(Mandatory=$false)]
    [Switch]$clean,
    [Parameter(Mandatory=$false)]
    [Switch]$local_test,
    [Parameter(Mandatory=$false)]
    [Switch]$local_test_coroutine
)

if ($clean.IsPresent) {
    if (Test-Path -Path "build") {
        Remove-Item build -Recurse
    }
}

$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

# Clean-Build-Folder
# build tests

if ($local_test.IsPresent) {
    & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DLOCAL_TEST=a -B build
} elseif ($local_test_coroutine.IsPresent) {
    & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DLOCAL_TEST_COROUTINE=a -B build
} else {
    & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" -B build
}
& cmake --build ./build

$ExitCode = $LastExitCode

# Post build, we actually want to transform the compile_commands.json file such that it has no \\ characters and ONLY has / characters
(Get-Content -Path build/compile_commands.json) |
    ForEach-Object {$_ -Replace '\\\\', '/'} | Set-Content -Path build/compile_commands.json

if (-not ($ExitCode -eq 0)) {
    $msg = "ExitCode: " + $ExitCode
    Write-Output $msg
    exit $ExitCode
}
