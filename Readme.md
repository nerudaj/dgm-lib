[![Build-Test-Package](https://github.com/nerudaj/dgm-lib/actions/workflows/build-test-package.yml/badge.svg)](https://github.com/nerudaj/dgm-lib/actions/workflows/build-test-package.yml)

# Readme

This project is an extension library for [SFML](http://sfml-dev.org). You can use it on its own, but recommended usage is as a part of [dgm-sdk](https://github.com/nerudaj/dgm-lib) project.

This library provides primitives for collision computations, vector manipulations, animations, particle system and convenience wrappers for main render window and camera manipulation.

## Requirements

* CMake v3.26 (FetchContent is unstable in newer versions)
* Microsoft Visual Studio 2022 v17.3 or newer

## Integration

### Integration using CMake

You can use `FetchContent` to link this library to your application. Doing so will also bring SFML as a dependency. However, `dgm-lib` only needs `sfml-system`, `sfml-graphics` and `sfml-window` to link, so you need to link `sfml-main` target to your application manually (plus network and audio if applicable).

```
include (FetchContent)

FetchContent_Declare ( LIBDGM_GIT
    GIT_REPOSITORY "https://github.com/nerudaj/dgm-lib"
    GIT_TAG "origin/main"
)

FetchContent_MakeAvailable ( LIBDGM_GIT )

# Target setup
project ( Demo )

add_executable ( ${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp"
)

target_link_libraries ( ${PROJECT_NAME}
    libdgm sfml-main
)
```

### Integration from releases

Under Releases section on Github you can find prepared releases. To integrate those, you need to properly include and link SFML and also link `xinput.lib`.

## Development

Dependencies are managed via CPM. Latest CPM script is downloaded during configure and written into the `cmake` folder.

Project has following options:

 * ENABLE_TESTS - Default: ON. When enabled, unit-tests target is created.
 * ENABLE_SANDBOX - Default: ON. When enabled, simple sandbox target with window is created so developer can play around with features.

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

