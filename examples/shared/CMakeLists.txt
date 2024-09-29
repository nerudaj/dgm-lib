cmake_minimum_required ( VERSION 3.22 )

set ( TARGET examples-shared )

add_library ( ${TARGET} STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/DemoData.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/DemoData.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Level.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Level.cpp"
)

target_link_libraries ( ${TARGET}
	LINK_PUBLIC libdgm
)

set_target_properties( ${TARGET} PROPERTIES
	FOLDER "examples"
)

apply_compile_options ( ${TARGET} )
enable_autoformatter ( ${TARGET} )
