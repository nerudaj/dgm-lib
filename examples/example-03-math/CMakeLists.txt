cmake_minimum_required ( VERSION 3.26 )

set ( TARGET "example-03-math" )

add_executable ( ${TARGET} WIN32
	"${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp"
)

target_link_libraries ( ${TARGET}
	LINK_PRIVATE examples-common
)

set_target_properties( ${TARGET} PROPERTIES
	FOLDER "examples"
)

apply_compile_options ( ${TARGET} )
enable_autoformatter ( ${TARGET} )
