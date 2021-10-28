@echo off

set BUILDDIR=vsbuild

rem Sourcing path to cmake
call /tools/doomsh.cmd

echo Phase 1 - Purging
rd /s /q %BUILDDIR%

echo Phase 2 - Configuring
mkdir %BUILDDIR%
cd %BUILDDIR%
cmake.exe ..
cd ..

echo Phase 3 - Building

cd %BUILDDIR%
cmake --build . --config Debug
cmake --build . --config Release

echo Phase 4 - Run Tests

ctest -C Debug
ctest -C Release

echo Phase 5 - Packaging
cpack
cd ..

echo Phase 6 - Finalizing
mkdir RELEASE
move %BUILDDIR%\*.zip RELEASE

echo Done!

@echo on
