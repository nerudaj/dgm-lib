cmake_minimum_required ( VERSION 3.19 )

project ( libdgm )

file (
	COPY "${DGM_BINARY_DIR}/.clang-format"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

glob_headers_and_sources ( HEADERS SOURCES )

add_library ( ${PROJECT_NAME}
	STATIC ${HEADERS} ${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private_include"
)

target_compile_features ( ${PROJECT_NAME}
	INTERFACE ${MY_CXX_COMPILE_FEATURES}
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)

target_link_libraries ( ${PROJECT_NAME}
	LINK_PUBLIC sfml-system sfml-window sfml-graphics
	LINK_PUBLIC xinput.lib
	LINK_PRIVATE nlohmann_json::nlohmann_json
)

# Testing
if ( ${ENABLE_TESTS} )
	add_subdirectory ( tests )
endif ()

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
