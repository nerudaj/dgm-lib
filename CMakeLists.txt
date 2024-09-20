cmake_minimum_required ( VERSION 3.26 )

include ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_cpp_init.cmake" )

# Remember source directories
set ( DGM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
set ( DGM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" )

option ( ENABLE_TESTS "Generate unit test target" ON )
option ( ENABLE_SANDBOX "Generate sandbox target" ON )
option ( ENABLE_EXAMPLES "Generate example targets" ON )
option ( BOOTSTRAP_CPM "Whether to download CPM" ON )
option ( OVERRIDE_RUNTIME_OUTPUT_DIR "Specify value for CMAKE_RUNTIME_OUTPUT_DIRECTORY variable" ON )

# CppInit
include ( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/get_cpp_init.cmake" )

if ( ${BOOTSTRAP_CPM} )
	bootstrap_cpm ()
endif ()

get_git_version (
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

if ( ${OVERRIDE_RUNTIME_OUTPUT_DIR} )
	set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DGM_BINARY_DIR}/Compiled" )
endif ()

if ( ${ENABLE_TESTS} )
	enable_testing()
endif ()

CPMAddPackage( "gh:SFML/SFML#2.6.1" )
CPMAddPackage( "gh:nlohmann/json@3.11.2" )
CPMAddPackage( "gh:catchorg/Catch2@3.3.2" )

# Subprojects
add_subdirectory ( lib )

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
set ( CPACK_GENERATOR "ZIP" )
set ( CPACK_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${FULL_PROJECT_VERSION}-win${CMAKE_GENERATOR_PLATFORM}-vc17-${CMAKE_GENERATOR_PLATFORM}-for-SFML-${SFML_VERSION}" )

include ( CPack )
