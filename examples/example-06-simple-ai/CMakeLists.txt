cmake_minimum_required ( VERSION 3.19 )

set ( TARGET "example-06-simple-ai" )

file (
	GLOB HEADER_FILES
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

file (
	GLOB SOURCE_FILES
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable ( ${TARGET} WIN32
	${HEADER_FILES} ${SOURCE_FILES}
)

target_link_libraries ( ${TARGET}
	LINK_PRIVATE examples-common
)

set_target_properties( ${TARGET} PROPERTIES
	FOLDER "examples"
)

apply_compile_options ( ${TARGET} )
enable_autoformatter ( ${TARGET} )
execute_example_as_test ( ${TARGET} )

