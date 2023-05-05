[![Build-Test-Package](https://github.com/nerudaj/dgm-lib/actions/workflows/build-test-package.yml/badge.svg)](https://github.com/nerudaj/dgm-lib/actions/workflows/build-test-package.yml)

# Readme

This project is an extension library for [SFML](http://sfml-dev.org). It isn't supposed to be used on its own but rather as a part of [dgm-sdk](https://github.com/nerudaj/dgm-lib) project.

Library provides primitives for collisions, particle effects, animations and tile based levels with their own file storage format. It also provides interfaces for putting together whole applications, etc.

## Requirements

* CMake v3.26 (FetchContent is unstable in newer versions)
* Microsoft Visual Studio 2022 v17.3 or newer

## Building

All dependencies are pulled in via cmake so you first have to configure the project:

```
mkdir vsbuild
cd vsbuild
cmake ..
```

This will create file dgm-lib.sln under vsbuild folder. You can now open that file and build the project normally through visual studio. Solution contains three important subprojects:

* dgm-lib - The library itself
* dgm-lib-benchmarks - Sandbox for benchmarking
* lib-testrunner - Unit tests

## Packaging

To create full release package, just run the make-release.bat. It will configure the project, build it and run the tests prior to packaging. If everything goes well, you will end up with `RELEASE` with zipfile with full package.

Version bump must be performed in both `changelog.txt` and `cmake/version.cmake`.

## Dependencies

This project depends on couple other projects:

* dshlibs
* nlohmann/json
* sfml
* catch2

If you want to bump any of them, update `cmake/dependencies.cmake` file. First couple of lines contains versions of those dependencies. Just bumping the version should be sufficient to update it.

## Versioning

Following logic is employed when versioning:

* Change to MAJOR version will only be mandated by a complete overhaul of the library
* Change to MINOR is issued whenever potentially API breaking changes occur
* Change to PATCH is basically any change that doesn't affect backwards API compatibility
