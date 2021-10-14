cmake_minimum_required ( VERSION 3.19 )

# Enforce C++ standards and compiler behaviour
include ( "${CMAKE_SOURCE_DIR}/cmake/cpp.cmake" )

# Project setup
project ( dgm-lib)
include ( "${CMAKE_SOURCE_DIR}/cmake/version.cmake" )
enable_testing()

# Dependencies
include ( "${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake" )
include ( "${CMAKE_SOURCE_DIR}/cmake/utils.cmake" )

# Subprojects
add_subdirectory ( lib )

# Installation
install (
	FILES       "${CMAKE_SOURCE_DIR}/changelog.txt"
	DESTINATION "."
)

install (
	DIRECTORY   "${CMAKE_SOURCE_DIR}/licenses"
	DESTINATION "."
)

# Packaging
set ( CPACK_GENERATOR "ZIP" )
set ( CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}" )
set ( CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}" )
set ( CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}" )
set ( CPACK_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${PROJECT_VERSION}-windows-vc16-${CMAKE_GENERATOR_PLATFORM}" )

include ( CPack )
