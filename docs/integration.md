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
    libdgm sfml-main
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
    libdgm sfml-main
)
```

The full example is available [here](../integration_tests/cpm).

## Precompiled binaries

You can find precompiled binaries under [Releases](https://github.com/nerudaj/dgm-lib/releases). They are more cumbersome to setup up, but you will get waaay faster compile times, if you use them (alongside with precompiled SFML). Assuming the release archive is unzipped into `${DGM_FOLDER}`, you can easily create linkable target as this:

```
find_library(LIB_DGM_D libdgm-d  NAMES libdgm-d.lib  HINTS "${DGM_FOLDER}/lib")
find_library(LIB_DGM_R libdgm  NAMES libdgm.lib  HINTS "${DGM_FOLDER}/lib")
set(LIB_DGM optimized ${LIB_DGM_R} debug ${LIB_DGM_D})

add_library ( Dep_dgm INTERFACE )
target_include_directories ( Dep_dgm INTERFACE "${DGM_FOLDER}/include" )
target_link_libraries ( Dep_dgm INTERFACE ${LIB_DGM} Dep_sfml xinput.lib )
```

DGM only has 1 static lib, so it is quite easy, setting up SFML is more verbose because it has 6. The full source for that can be found [here](../integration_tests/fetch_release).
