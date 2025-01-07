cmake_minimum_required ( VERSION 3.26 )

set ( TARGET unit-tests )

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestDataDir.hpp.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestDataDir.hpp"
)

glob_headers_and_sources ( HEADERS SOURCES )

add_executable ( ${TARGET}
	${HEADERS} ${SOURCES}
)

target_include_directories( ${TARGET}
	PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${TARGET}
	LINK_PRIVATE dgm::dgm
)

apply_compile_options ( ${TARGET} )
enable_autoformatter ( ${TARGET} )

add_test (
	NAME    ${TARGET}
	COMMAND ${TARGET}
)
