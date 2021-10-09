@echo off

set BUILDDIR=vsbuild
set MSVCDIR="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

rem Sourcing path to cmake
call /tools/doomsh.cmd

echo Phase 1 - Purging
rd /s /q %BUILDDIR%
rd /s /q project-template\%BUILDDIR%

echo Phase 2 - Configuring
mkdir %BUILDDIR%
cd %BUILDDIR%
cmake.exe ..
cd ..

echo Phase 3 - Building
call %MSVCDIR% 

cd %BUILDDIR%
devenv dgm-lib.sln /Build Debug
devenv dgm-lib.sln /Build Release

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
