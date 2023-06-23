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

## CPM

You can also use the Cmake Package Manager. This will transitively download and bootstrap SFML as well:

```
cmake_minimum_required ( VERSION 3.26 )

set ( CMAKE_CXX_STANDARD		23 )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Compiled" )

file ( DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/get_cpm.cmake
  "${CMAKE_BINARY_DIR}/cpm.cmake"
)

include ( "${CMAKE_BINARY_DIR}/cpm.cmake" )

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
