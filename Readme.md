[![Windows-MSVC-CI](https://github.com/nerudaj/dgm-lib/actions/workflows/main.yml/badge.svg)](https://github.com/nerudaj/dgm-lib/actions/workflows/main.yml) [![Windows-Clang-Ninja-CI](https://github.com/nerudaj/dgm-lib/actions/workflows/main-clang.yml/badge.svg)](https://github.com/nerudaj/dgm-lib/actions/workflows/main-clang.yml) [![CI-Ubuntu-Ninja-Clang](https://github.com/nerudaj/dgm-lib/actions/workflows/main-clang-ubuntu.yml/badge.svg)](https://github.com/nerudaj/dgm-lib/actions/workflows/main-clang-ubuntu.yml)[![Android-CI](https://github.com/nerudaj/dgm-lib/actions/workflows/android.yml/badge.svg?branch=main)](https://github.com/nerudaj/dgm-lib/actions/workflows/android.yml) [![IntegrationTests](https://github.com/nerudaj/dgm-lib/actions/workflows/integration.yml/badge.svg)](https://github.com/nerudaj/dgm-lib/actions/workflows/integration.yml) 

# Readme

This project is an extension library for [SFML](http://sfml-dev.org). The library provides primitives for collision computations, vector manipulations, animations, particle system and convenience wrappers for main render window and camera manipulation. The library is fully compatible with Visual Studio 2022 and Clang for Android (shipped with Android Studio).

## Requirements

* CMake v3.28 or newer
* Microsoft Visual Studio 2022 v17.3 or newer
* OR clang 20 (or newer) + Ninja

## How to build:

* Windows + MSVC:
```
mkdir _build
cd _build
cmake ..
cmake --build . --config Debug
```

* Windows + Clang
```
mkdir _build
cd _build
cmake -G Ninja -D CMAKE_CXX_COMPILER=clang++ ..
```

* Ubuntu
```
mkdir _build
cd _build
cmake -G Ninja -D CMAKE_CXX_COMPILER=clang++-21 -D CMAKE_C_COMPILER=clang-21 ..
```


## Integration

### Integration using CMake

`FetchContent` can be used to download `dgm-lib` from GitHub and integrated. Alternatively, you can fetch prebuild release binaries and link them through `find_package`. See [integration guide](docs/integration.md) on how to do so.

### Integration from releases

Under Releases section on Github you can find prepared releases.

## Development

Dependencies are managed via CPM. Latest CPM script is downloaded during configure and written into the `cmake` folder.

Project has following options:

 * `ENABLE_TESTS` - Default: ON. When enabled, unit-tests target is created.
 * `ENABLE_EXAMPLES` - Default: ON. When enabled, example targets are created.
 * `ENABLE_SANDBOX` - Default: ON. When enabled, simple sandbox target with window is created so developer can play around with features.
 * `BOOTSTRAP_CPM` - Default: ON. When enable, newest version of CMake Package Manager is downloaded and included. This is required for dependencies. Only turn OFF when your project is also using CPM.
 * `OVERRIDE_RUNTIME_OUTPUT_DIR` - Default: ON. When enabled, all runtime build artifacts (dlls and exes), even those from depedencies are compiled into one common folder (`${CMAKE_BINARY_DIR}/Compiled`). This way executables can be run right away without worrying about DLL lookup.
 * `DONT_LOOK_FOR_SFML` - Default: OFF. When enabled, CMake won't attempt to locate SFML through `find_package`.
 * `DONT_FETCH_SFML` - Default: OFF. When enabled, CMake won't bring in SFML through CPM.

`ENABLE_*` and `BOOTSTRAP_CPM` options are disabled when the library is used as a dependency to improve build times and to ensure proper placement of build artifacts. The `DONT_LOOK_FOR_SFML` and `DONT_FETCH_SFML` are used to affect how SFML is sourced into the library. If you for example use both TGUI and dgm-lib, you want to ensure both these libraries are using the same version of SFML. Therefore, you can disable automatic fetching. On the other hand, if you only want to use SFLM from within dgm-lib, you can save time during configure by not looking for SFML and fetching it through CPM right away.

## Documentation

 * [App State Pattern](docs/app.md)
 * [Texture Atlas](docs/texture_atlas.md)

## Dependencies

This project depends on couple other projects:

* sfml
* nlohmann/json
* catch2 (unit-tests target)

These are fetched automatically through CMake FetchContent.

## Versioning

This library is trying its best to follow the semantic versioning.

