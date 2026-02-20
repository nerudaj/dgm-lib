cmake_minimum_required ( VERSION 3.26 )

# Remember source directories
set ( DGM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
set ( DGM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" )

option ( ENABLE_TESTS "Generate unit test target" ON )
option ( ENABLE_SANDBOX "Generate sandbox target" ON )
option ( ENABLE_EXAMPLES "Generate example targets" ON )
option ( ENABLE_LINTER "Enable clang-tidy on lib target" OFF )
option ( ENABLE_LEGACY_ANIMATION "Enable old implementation of dgm::Animation" ON )
option ( BOOTSTRAP_CPM "Whether to download CPM" ON )
option ( OVERRIDE_RUNTIME_OUTPUT_DIR "Specify value for CMAKE_RUNTIME_OUTPUT_DIRECTORY variable" ON )
option ( DONT_LOOK_FOR_SFML "Don't look for SFML, use CPM to download it" OFF )
option ( DONT_FETCH_SFML "Don't auto-fetch SFML, fail if not found" OFF )
option ( DONT_LOOK_FOR_JSON "Don't look for nlohmann::json, use CPM to download it" OFF )
option ( DONT_FETCH_JSON "Don't auto-fetch SFML, fail if not found" OFF )

# CppInit
include ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpp-init.cmake" )
include ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions.cmake" )

if ( ${BOOTSTRAP_CPM} )
    bootstrap_cpm ()
endif ()

message ( "Generator used: ${CMAKE_GENERATOR}" )

if ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Android" )
    message ( "Building for Android" )
elseif ( "${CMAKE_GENERATOR}" STREQUAL "Ninja" )
    message ( "Ninja generator used, not setting CMAKE_GENERATOR_PLATFORM" )
else ()
    if (  "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "" )
        message ("CMAKE_GENERATOR_PLATFORM not set, defaulting to x64")
        set ( CMAKE_GENERATOR_PLATFORM x64 )
    else()
        message ("Detected CMAKE_GENERATOR_PLATFORM: '${CMAKE_GENERATOR_PLATFORM}'" )
    endif ()
endif ()


set ( CMAKE_CXX_STANDARD 23 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

get_version_from_git (
    PROJECT_VERSION_VARIABLE GIT_PROJECT_VERSION
    FULL_VERSION_VARIABLE FULL_PROJECT_VERSION
)

# Project setup
project ( dgm-lib VERSION "${GIT_PROJECT_VERSION}" )

if (NOT ${PROJECT_IS_TOP_LEVEL} )
    message ( "INFO: dgm-lib is used as a dependency, turning off unwanted features" )
    set ( ENABLE_TESTS OFF )
    set ( ENABLE_SANDBOX OFF )
    set ( ENABLE_EXAMPLES OFF )
    set ( OVERRIDE_RUNTIME_OUTPUT_DIR OFF )
endif ()

message ("Configure options:")
message ("  ENABLE_TESTS: ${ENABLE_TESTS}")
message ("  ENABLE_SANDBOX: ${ENABLE_SANDBOX}")
message ("  ENABLE_EXAMPLES: ${ENABLE_EXAMPLES}")
message ("  ENABLE_LINTER: ${ENABLE_LINTER}")
message ("  ENABLE_LEGACY_ANIMATION: ${ENABLE_LEGACY_ANIMATION}" )
message ("  BOOTSTRAP_CPM: ${BOOTSTRAP_CPM}")
message ("  OVERRIDE_RUNTIME_OUTPUT_DIR: ${OVERRIDE_RUNTIME_OUTPUT_DIR}")
message ("  DONT_LOOK_FOR_SFML: ${DONT_LOOK_FOR_SFML}")
message ("  DONT_FETCH_SFML: ${DONT_FETCH_SFML}")
message ("  DONT_LOOK_FOR_JSON: ${DONT_LOOK_FOR_JSON}")
message ("  DONT_FETCH_JSON: ${DONT_FETCH_JSON}")

if ( ${OVERRIDE_RUNTIME_OUTPUT_DIR} )
    set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DGM_BINARY_DIR}/Compiled" )
endif ()

if ( ${ENABLE_TESTS} )
    enable_testing()
endif ()

set ( SFML_VERSION "3.0.1" )

# Dependencies
find_or_fetch_dependency(
	PACKAGE_NAME SFML
	VERSION 3.0
	CPM_URL "gh:SFML/SFML#${SFML_VERSION}"
	COMPONENTS System Window Graphics
	DONT_LOOK ${DONT_LOOK_FOR_SFML}
	DONT_FETCH ${DONT_FETCH_SFML}
)

find_or_fetch_dependency(
	PACKAGE_NAME json
	VERSION 3
	CPM_URL "gh:nlohmann/json@3.12.0"
	DONT_LOOK ${DONT_LOOK_FOR_JSON}
	DONT_FETCH ${DONT_FETCH_JSON}
)

# Subprojects
add_subdirectory ( lib )
add_library ( dgm::dgm-lib ALIAS dgm-lib )

if ( ${ENABLE_SANDBOX} )
    add_subdirectory ( sandbox )
endif()

if ( ${ENABLE_EXAMPLES} )
    add_subdirectory ( examples )
endif ()

if ( ${ENABLE_TESTS} )
    add_subdirectory ( tests )
endif ()

# Installation
install (
    FILES       "${DGM_SOURCE_DIR}/changelog.txt"
    DESTINATION "."
)

install (
    DIRECTORY   "${DGM_SOURCE_DIR}/licenses"
    DESTINATION "."
)

# Packaging
if ( ${PROJECT_IS_TOP_LEVEL} )
    if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
        set ( COMPILER_ID "${CMAKE_CXX_COMPILER_ID}-${MSVC_TOOLSET_VERSION}" )
    elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
        set ( COMPILER_ID "Clang" )
    else ()
        message ( FATAL_ERROR "Unsupported compiler ID: ${CMAKE_CXX_COMPILER_ID}" )
    endif ()

    set ( CPACK_GENERATOR "ZIP" )
    set ( CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${FULL_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}-${COMPILER_ID}-for-SFML-${SFML_VERSION}" )
    
    message ( "Computed package name: ${CPACK_PACKAGE_FILE_NAME}" )

    include ( CPack )
endif ()