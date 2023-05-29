cmake_minimum_required ( VERSION 3.19 )

# Remember source directories
set ( DGM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
set ( DGM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DGM_BINARY_DIR}/Compiled" )

# Enforce C++ standards and compiler behaviour
include ( "${DGM_SOURCE_DIR}/cmake/cpp.cmake" )
include ( "${DGM_SOURCE_DIR}/cmake/dependencies.cmake" )
include ( "${DGM_SOURCE_DIR}/cmake/utils.cmake" )
include ( "${DGM_SOURCE_DIR}/cmake/version.cmake" )

option ( ENABLE_TESTS "Build unit test target" OFF )
option ( ENABLE_SANDBOX "Build sandbox target" OFF )

# Project setup
project ( dgm-lib)

if ( ${ENABLE_TESTS} )
	enable_testing()
endif ()

# Subprojects
add_subdirectory ( lib )

if ( ${ENABLE_SANDBOX} )
	add_subdirectory ( sandbox )
endif()

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
set ( CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}" )
set ( CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}" )
set ( CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}" )
set ( CPACK_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${PROJECT_VERSION}-win${CMAKE_GENERATOR_PLATFORM}-vc17-${CMAKE_GENERATOR_PLATFORM}-for-SFML-${SFML_VERSION}" )

include ( CPack )
