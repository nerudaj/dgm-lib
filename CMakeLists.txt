cmake_minimum_required ( VERSION 3.26 )

# Remember source directories
set ( DGM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
set ( DGM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" )

# Deprecated options, will be removed after the next release
option ( ENABLE_TESTS "Deprecated, use DGM_BUILD_TESTS instead" OFF )
option ( ENABLE_SANDBOX "Deprecated, use DGM_BUILD_SANDBOX instead" OFF )
option ( ENABLE_EXAMPLES "Deprecated, use DGM_BUILD_EXAMPLES instead" OFF )
option ( ENABLE_LINTER "Deprecated, use DGM_ENABLE_LINTER instead" OFF )
option ( ENABLE_LEGACY_ANIMATION "Deprecated, use DGM_ENABLE_LEGACY_ANIMATION instead" OFF )
option ( BOOTSTRAP_CPM "Deprecated, use DGM_BOOTSTRAP_CPM instead" OFF )
option ( OVERRIDE_RUNTIME_OUTPUT_DIR "Deprecated, use DGM_OVERRIDE_RUNTIME_OUTPUT_DIR instead" OFF )
option ( DONT_LOOK_FOR_SFML "Deprecated, use DGM_DONT_LOOK_FOR_SFML instead" OFF )
option ( DONT_FETCH_SFML "Deprecated, use DGM_DONT_FETCH_SFML instead" OFF )
option ( DONT_LOOK_FOR_JSON "Deprecated, use DGM_DONT_LOOK_FOR_JSON instead" OFF )
option ( DONT_FETCH_JSON "Deprecated, use DGM_DONT_FETCH_JSON instead" OFF )

# Configure options
option ( DGM_BUILD_TESTS "Generate unit test target" ON )
option ( DGM_BUILD_SANDBOX "Generate sandbox target" ON )
option ( DGM_BUILD_EXAMPLES "Generate example targets" ON )
option ( DGM_ENABLE_LINTER "Enable clang-tidy on lib target" OFF )
option ( DGM_ENABLE_LEGACY_ANIMATION "Enable old implementation of dgm::Animation" ON )
option ( DGM_BOOTSTRAP_CPM "Whether to download CPM" ON )
option ( DGM_OVERRIDE_RUNTIME_OUTPUT_DIR "Specify value for CMAKE_RUNTIME_OUTPUT_DIRECTORY variable" ON )
option ( DGM_DONT_LOOK_FOR_SFML "Don't look for SFML, use CPM to download it" OFF )
option ( DGM_DONT_FETCH_SFML "Don't auto-fetch SFML, fail if not found" OFF )
option ( DGM_DONT_LOOK_FOR_JSON "Don't look for nlohmann::json, use CPM to download it" OFF )
option ( DGM_DONT_FETCH_JSON "Don't auto-fetch SFML, fail if not found" OFF )

# This will be removed alongside deprecated options
macro ( set_if_true dst src )
    if (${src})
        set ( dst ON )
    endif ()
endmacro()

set_if_true( DGM_BUILD_TESTS ENABLE_TESTS )
set_if_true( DGM_BUILD_SANDBOX ENABLE_SANDBOX )
set_if_true( DGM_BUILD_EXAMPLES ENABLE_EXAMPLES )
set_if_true( DGM_ENABLE_LINTER ENABLE_LINTER )
set_if_true( DGM_ENABLE_LEGACY_ANIMATION ENABLE_LEGACY_ANIMATION )
set_if_true( DGM_BOOTSTRAP_CPM BOOTSTRAP_CPM )
set_if_true( DGM_OVERRIDE_RUNTIME_OUTPUT_DIR OVERRIDE_RUNTIME_OUTPUT_DIR )
set_if_true( DGM_DONT_LOOK_FOR_SFML DONT_LOOK_FOR_SFML )
set_if_true( DGM_DONT_FETCH_SFML DONT_FETCH_SFML )
set_if_true( DGM_DONT_LOOK_FOR_JSON DONT_LOOK_FOR_JSON )
set_if_true( DGM_DONT_FETCH_JSON DONT_FETCH_JSON )

# CppInit
include ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpp-init.cmake" )
include ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions.cmake" )

if ( ${DGM_BOOTSTRAP_CPM} )
    DGM_BOOTSTRAP_CPM ()
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
    set ( DGM_BUILD_TESTS OFF )
    set ( DGM_BUILD_SANDBOX OFF )
    set ( DGM_BUILD_EXAMPLES OFF )
    set ( DGM_OVERRIDE_RUNTIME_OUTPUT_DIR OFF )
endif ()

message ("Configure options:")
message ("  DGM_BUILD_TESTS: ${DGM_BUILD_TESTS}")
message ("  DGM_BUILD_SANDBOX: ${DGM_BUILD_SANDBOX}")
message ("  DGM_BUILD_EXAMPLES: ${DGM_BUILD_EXAMPLES}")
message ("  DGM_ENABLE_LINTER: ${DGM_ENABLE_LINTER}")
message ("  DGM_ENABLE_LEGACY_ANIMATION: ${DGM_ENABLE_LEGACY_ANIMATION}" )
message ("  DGM_BOOTSTRAP_CPM: ${DGM_BOOTSTRAP_CPM}")
message ("  DGM_OVERRIDE_RUNTIME_OUTPUT_DIR: ${DGM_OVERRIDE_RUNTIME_OUTPUT_DIR}")
message ("  DGM_DONT_LOOK_FOR_SFML: ${DGM_DONT_LOOK_FOR_SFML}")
message ("  DGM_DONT_FETCH_SFML: ${DGM_DONT_FETCH_SFML}")
message ("  DGM_DONT_LOOK_FOR_JSON: ${DGM_DONT_LOOK_FOR_JSON}")
message ("  DGM_DONT_FETCH_JSON: ${DGM_DONT_FETCH_JSON}")

if ( ${DGM_OVERRIDE_RUNTIME_OUTPUT_DIR} )
    set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DGM_BINARY_DIR}/Compiled" )
endif ()

if ( ${DGM_BUILD_TESTS} )
    enable_testing()
endif ()

set ( SFML_VERSION "3.0.1" )

# Dependencies
find_or_fetch_dependency(
	PACKAGE_NAME SFML
	VERSION 3.0
	CPM_URL "gh:SFML/SFML#${SFML_VERSION}"
	COMPONENTS System Window Graphics
	DONT_LOOK ${DGM_DONT_LOOK_FOR_SFML}
	DONT_FETCH ${DGM_DONT_FETCH_SFML}
)

find_or_fetch_dependency(
	PACKAGE_NAME json
	VERSION 3
	CPM_URL "gh:nlohmann/json@3.12.0"
	DONT_LOOK ${DGM_DONT_LOOK_FOR_JSON}
	DONT_FETCH ${DGM_DONT_FETCH_JSON}
)

# Subprojects
add_subdirectory ( lib )
add_library ( dgm::dgm-lib ALIAS dgm-lib )

if ( ${DGM_BUILD_SANDBOX} )
    add_subdirectory ( sandbox )
endif()

if ( ${DGM_BUILD_EXAMPLES} )
    add_subdirectory ( examples )
endif ()

if ( ${DGM_BUILD_TESTS} )
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