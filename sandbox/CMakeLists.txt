cmake_minimum_required ( VERSION 3.19 )

project ( sandbox )

add_executable ( ${PROJECT_NAME} WIN32
	"${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp"
)

target_link_libraries ( ${PROJECT_NAME}
	LINK_PRIVATE libdgm sfml-main
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)
