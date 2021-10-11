# Readme

This project is an extension library for [SFML](http://sfml-dev.org). It isn't supposed to be used on its own but rather as a part of [dgm-sdk](https://github.com/nerudaj/dgm-lib) project.

Library provides primitives for collisions, particle effects, animations and tile based levels with their own file storage format. It also provides interfaces for putting together whole applications, etc.

## Requirements

* Cmake v3.19 or newer
* Microsoft Visual Studio 2019 v16.11.3 or newer

## Building

All dependencies are pulled in via cmake so you first have to configure the project:

```
mkdir vsbuild
cd vsbuild
cmake ..
```

This will create file dgm-lib.sln under vsbuild folder. You can now open that file and build the project normally through visual studio. Solution contains two subprojects:

* dgm-lib - The library itself
* lib-testrunner - Unit tests

## Packaging

To create full release package, just run the make-release.bat. It will configure the project, build it and run the tests prior to packaging. If everything goes well, you will end up with `RELEASE` with zipfile with full package.

## Dependencies

This project depends on couple other projects:

* dshlibs
* nlohmann/json
* sfml
* catch2

If you want to bump any of them, update `cmake/dependencies.cmake` file. First couple of lines contains versions of those dependencies. Just bumping the version should be sufficient to update it.
