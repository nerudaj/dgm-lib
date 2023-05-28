cmake_minimum_required ( VERSION 3.19 )

file (
	COPY "${DGM_BINARY_DIR}/.clang-format"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

project ( lib-testrunner )

include_directories ( ${CATCH_FOLDER} )
copy_dlls_to_build_folders ()

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/src/TestDataDir.hpp.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/TestDataDir.hpp"
)

file (
	GLOB_RECURSE HEADER_FILES
	"*.hpp"
)

file (
	GLOB_RECURSE SOURCE_FILES
	"*.cpp"
)

message ( "${HEADER_FILES}" )
message ( "${SOURCE_FILES}" )

add_executable ( ${PROJECT_NAME}
	${HEADER_FILES} ${SOURCE_FILES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories( ${PROJECT_NAME}
	PUBLIC  "${DSH_FOLDER}/include"
)

target_link_libraries ( ${PROJECT_NAME}
	libdgm ${LIB_LVLD}
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)

add_test (
	NAME    ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
