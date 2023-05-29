cmake_minimum_required ( VERSION 3.19 )

project ( unit-tests )

file (
	COPY "${DGM_BINARY_DIR}/.clang-format"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestDataDir.hpp.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/TestDataDir.hpp"
)

glob_headers_and_sources ( HEADERS SOURCES )

add_executable ( ${PROJECT_NAME}
	${HEADERS} ${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories( ${PROJECT_NAME}
	PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${PROJECT_NAME}
	LINK_PRIVATE libdgm
	LINK_PRIVATE Catch2::Catch2WithMain
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)

add_test (
	NAME    ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
