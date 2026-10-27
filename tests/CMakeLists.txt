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
	PRIVATE  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries ( ${TARGET}
	PRIVATE dgm::dgm-lib
)

apply_compile_options ( ${TARGET} )
enable_autoformatter ( ${TARGET} )

add_test (
	NAME    ${TARGET}
	COMMAND ${TARGET}
)
