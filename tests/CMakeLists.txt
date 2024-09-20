cmake_minimum_required ( VERSION 3.26 )

set ( TARGET unit-tests )

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestDataDir.hpp.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestDataDir.hpp"
)

bootstrap_clang_format()
glob_headers_and_sources ( HEADERS SOURCES )

add_executable ( ${TARGET}
	${HEADERS} ${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories( ${TARGET}
	PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${TARGET}
	LINK_PRIVATE libdgm
	LINK_PRIVATE Catch2::Catch2WithMain
)

apply_compile_options ( ${TARGET} )

add_test (
	NAME    ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
