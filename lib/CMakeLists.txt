cmake_minimum_required ( VERSION 3.26 )

set ( TARGET libdgm )

glob_headers_and_sources ( HEADERS SOURCES )

add_library ( ${TARGET}
	STATIC ${HEADERS} ${SOURCES}
)

target_include_directories ( ${TARGET}
	PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private_include"
)

target_link_libraries ( ${TARGET}
	LINK_PUBLIC sfml-system sfml-window sfml-graphics sfml-main
	LINK_PUBLIC xinput.lib
	LINK_PRIVATE nlohmann_json::nlohmann_json
)

apply_compile_options ( ${TARGET} )
enable_autoformatter ( ${TARGET} )

# Instalation
install (
	FILES        "${CMAKE_CURRENT_BINARY_DIR}/Release/libdgm.lib"
	DESTINATION  "lib"
)

install (
	FILES        "${CMAKE_CURRENT_BINARY_DIR}/Debug/libdgm-d.lib"
	DESTINATION  "lib"
)

install (
	FILES        "${CMAKE_CURRENT_BINARY_DIR}/Debug/libdgm-d.pdb"
	DESTINATION  "lib"
)

install (
	DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/include/DGM"
	DESTINATION  "include"
)
