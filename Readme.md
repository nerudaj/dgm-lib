[![Build-Test-Package](https://github.com/nerudaj/dgm-lib/actions/workflows/build-test-package.yml/badge.svg)](https://github.com/nerudaj/dgm-lib/actions/workflows/build-test-package.yml)

# Readme

This project is an extension library for [SFML](http://sfml-dev.org). You can use it on its own, but recommended usage is as a part of [dgm-sdk](https://github.com/nerudaj/dgm-lib) project.

This library provides primitives for collision computations, vector manipulations, animations, particle system and convenience wrappers for main render window and camera manipulation.

## Requirements

* CMake v3.26 (FetchContent is unstable in newer versions)
* Microsoft Visual Studio 2022 v17.3 or newer

## Integration

### Integration using CMake

`FetchContent` can be used to download `dgm-lib` from GitHub and integrated. See [integration guide](docs/integration.md) on how to do so.

### Integration from releases

Under Releases section on Github you can find prepared releases. To integrate those, you need to properly include and link SFML and also link `xinput.lib`.

## Development

Dependencies are managed via CPM. Latest CPM script is downloaded during configure and written into the `cmake` folder.

Project has following options:

 * ENABLE_TESTS - Default: ON. When enabled, unit-tests target is created.
 * ENABLE_SANDBOX - Default: ON. When enabled, simple sandbox target with window is created so developer can play around with features.
 * OVERRIDE_RUNTIME_OUTPUT_DIR - Default: ON. When enabled, all runtime build artifacts (dlls and exes), even those from depedencies are compiled into one common folder (${CMAKE_BINARY_DIR}/Compiled). This way executables can be run right away without worrying about DLL lookup.

All these options are disabled when the library is used as a dependency to improve build times and to ensure proper placement of build artifacts.

## Packaging

To create full release package, just run the make-release.bat. It will configure the project, build it and run the tests prior to packaging. If everything goes well, you will end up with `RELEASE` with zipfile with full package.

Version bump must be performed in both `changelog.txt` and `cmake/version.cmake`.

## Dependencies

This project depends on couple other projects:

* sfml
* nlohmann/json
* catch2 (unit-tests target)

If you want to bump any of them, update the `cmake/dependencies.cmake` file.

## Versioning

This library is trying its best to follow the semantic versioning.

