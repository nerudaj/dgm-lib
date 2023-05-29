# Download .clang-format
file (
	DOWNLOAD
		"https://raw.githubusercontent.com/nerudaj/clang-format/main/.clang-format"
		"${DGM_BINARY_DIR}/.clang-format"
)

# Download and bootstrap CPM
file ( DOWNLOAD
	https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/get_cpm.cmake
	"${DGM_SOURCE_DIR}/cmake/cpm.cmake"
)

include ( "${DGM_SOURCE_DIR}/cmake/cpm.cmake" )

set ( SFML_VERSION "2.5.1" )
set ( JSON_VERSION "3.11.2" )
set ( CATCH_VERSION "3.3.2" )

CPMAddPackage( "gh:SFML/SFML#${SFML_VERSION}" )
CPMAddPackage( "gh:nlohmann/json@${JSON_VERSION}" )
CPMAddPackage( "gh:catchorg/Catch2@${CATCH_VERSION}" )
