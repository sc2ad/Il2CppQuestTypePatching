Param(
    [Parameter(Mandatory=$false)]
    [Switch]$clean,
    [Parameter(Mandatory=$false)]
    [Switch]$local_test,
    [Parameter(Mandatory=$false)]
    [Switch]$local_test_coroutine
)

# if user specified clean, remove all build files
if ($clean.IsPresent)
{
    if (Test-Path -Path "build")
    {
        remove-item build -R
    }
}

$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

if (($clean.IsPresent) -or (-not (Test-Path -Path "build")))
{
    $out = new-item -Path build -ItemType Directory
}

cd build
if ($local_test.IsPresent) {
    & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="Release" -DLOCAL_TEST=a ../
} elseif ($local_test_coroutine.IsPresent) {
    & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="Release" -DLOCAL_TEST_COROUTINE=a../
} else {
    & cmake -G "Ninja" -DCMAKE_BUILD_TYPE="Release" ../
}

& cmake --build .
$ExitCode = $LastExitCode
cd ..
exit $ExitCode