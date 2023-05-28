cmake_minimum_required ( VERSION 3.19 )

set ( DGM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
set ( DGM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" )

# Enforce C++ standards and compiler behaviour
include ( "${DGM_SOURCE_DIR}/cmake/cpp.cmake" )

# Download some helper files
file (
	DOWNLOAD
		"https://raw.githubusercontent.com/nerudaj/clang-format/main/.clang-format"
		"${DGM_BINARY_DIR}/.clang-format"
)

# Project setup
project ( dgm-lib)
include ( "${DGM_SOURCE_DIR}/cmake/version.cmake" )
enable_testing()

# Dependencies
include ( "${DGM_SOURCE_DIR}/cmake/dependencies.cmake" )
include ( "${DGM_SOURCE_DIR}/cmake/utils.cmake" )

# Subprojects
add_subdirectory ( lib )
add_subdirectory ( sandbox )

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
set ( CPACK_PACKAGE_FILE_NAME           "${PROJECT_NAME}-${PROJECT_VERSION}-windows-vc17-${CMAKE_GENERATOR_PLATFORM}" )

include ( CPack )
