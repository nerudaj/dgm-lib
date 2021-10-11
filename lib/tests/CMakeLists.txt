cmake_minimum_required ( VERSION 3.19 )

project ( lib-testrunner )

include_directories ( ${CATCH_FOLDER} )
copy_dlls_to_build_folders ()

set ( SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/App.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Clip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Collision.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Conversion.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Math.cpp"
)

add_executable ( ${PROJECT_NAME}
	${SOURCES}
)

target_link_libraries ( ${PROJECT_NAME}
	libdgm
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE /W4 /MP
)

add_test (
	NAME    ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
