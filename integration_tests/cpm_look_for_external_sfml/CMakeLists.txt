cmake_minimum_required ( VERSION 3.26 )

include ( get_cpm.cmake )

set ( CMAKE_CXX_STANDARD 23 )

project ( test-cpm )

CPMAddPackage("gh:SFML/SFML#3.0.1")
CPMAddPackage(
    NAME dgm-lib
    GITHUB_REPOSITORY nerudaj/dgm-lib
    GIT_TAG ${GIT_BRANCH}
    OPTIONS
        DONT_FETCH_SFML ON
)

add_executable ( ${PROJECT_NAME} Main.cpp )

target_link_libraries ( ${PROJECT_NAME} dgm::dgm-lib )
