cmake_minimum_required ( VERSION 3.26 )

set ( CMAKE_CXX_STANDARD 23 )

project ( test-cpm )

include ( FetchContent )

FetchContent_Declare ( LIBDGM_GIT
	GIT_REPOSITORY "https://github.com/nerudaj/dgm-lib"
	GIT_TAG "${GIT_BRANCH}"
)

FetchContent_MakeAvailable ( LIBDGM_GIT )

add_executable ( ${PROJECT_NAME} Main.cpp )

target_link_libraries ( ${PROJECT_NAME} dgm::dgm-lib sfml-main )
