cmake_minimum_required ( VERSION 3.26 )

set ( TARGET "example-02-controller" )

add_executable ( ${TARGET} WIN32
	"${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp"
)

target_link_libraries ( ${TARGET}
	LINK_PRIVATE libdgm
)

apply_compile_options ( ${TARGET} )
