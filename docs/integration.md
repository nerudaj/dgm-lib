# Integrating dgm-lib

Use the following demo program to validate that dgm-lib works:

```cpp
#include <DGM/dgm.hpp>

int main(int, char* [])
{
	auto&& window = dgm::Window({ 1280,720 }, "Test", false);
	sf::Event event;
	
	dgm::Circle circle(640.f, 360.f, 20.f);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}

		window.beginDraw();

		circle.debugRender(window, sf::Color::Red);

		window.endDraw();
	}

	return 0;
}
```

## FetchContent

This is the easiest way to integrate dgm-lib and SFML. Use FetchContent to clone the dgm-lib repo, optionally with a given revision and let CMake figure it out. You need to manually link against `sfml-main` (and `sfml-audio` / `sfml-network` if needed).

```
cmake_minimum_required ( VERSION 3.26 )

set ( CMAKE_CXX_STANDARD		23 )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Compiled" )

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
    dgm::dgm-lib sfml-main
)
```

The full example is available [here](../integration_tests/fetch_git).

## CPM

You can also use the Cmake Package Manager. This will transitively download and bootstrap SFML as well:

```
cmake_minimum_required ( VERSION 3.26 )

set ( CMAKE_CXX_STANDARD		23 )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Compiled" )

# Somehow get CPM here

CPMAddPackage("gh:nerudaj/dgm-lib#main")

# Target setup
project ( Demo )

add_executable ( ${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp"
)

target_link_libraries ( ${PROJECT_NAME}
    dgm::dgm-lib sfml-main
)
```

The full example is available [here](../integration_tests/cpm).

## Precompiled binaries

You can find precompiled binaries under [Releases](https://github.com/nerudaj/dgm-lib/releases). They provide faster compilation times, but you're responsible for also downloading SFML as well: 

```
set ( SFML_VERSION "3.0.0" )
set ( DGM_VERSION "v3.0.0" )

include (FetchContent)

FetchContent_Declare ( SFML
	URL "https://github.com/SFML/SFML/releases/download/${SFML_VERSION}/SFML-${SFML_VERSION}-windows-vc17-64-bit.zip"
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare ( DGM
	URL "https://github.com/nerudaj/dgm-lib/releases/download/${DGM_VERSION}/dgm-lib-${DGM_VERSION}-Windows-x64-MSVC-143-for-SFML-${SFML_VERSION}.zip"
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable ( DGM SFML )

set ( SFML_STATIC_LIBRARIES ON )
find_package ( SFML
    ${SFML_VERSION} REQUIRED
    COMPONENTS Main Window Graphics
    PATHS "${SFML_FOLDER}/lib/cmake"
)

find_package ( dgm-lib
    ${DGM_VERSION} REQUIRED
    PATHS "${DGM_FOLDER}/lib/cmake"
)

# ...
target_link_libraries ( ${TARGET} PUBLIC dgm::dgm-lib )
```

The full source for that can be found [here](../integration_tests/fetch_release).

# Automatic dependency fetching

dgm-lib uses SFML and nlohmann::json as dependencies.

By default, dgm-lib first checks via `find_package` if SFML/json of a correct version is already available somewhere. If not, it will download it through CPM. In the case you want a hard error if SFML/json is not found and not download it, you can disable that behavior, even when using CPM:

```
CPMAddPackage(
    NAME dgm-lib
    GITHUB_REPOSITORY nerudaj/dgm-lib
    GIT_TAG main
    OPTIONS
        DONT_FETCH_SFML ON
		DONT_FETCH_JSON ON
)
```

If you don't want dgm-lib to issue `find_package` call, you can use `DONT_LOOK_FOR_SMLF` and `DONT_LOOK_FOR_JSON` to surpress that behavior.

The full source for that can be found [here](../integration_tests/cpm_look_for_external_sfml/).