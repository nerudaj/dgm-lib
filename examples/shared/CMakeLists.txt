cmake_minimum_required ( VERSION 3.22 )

set ( TARGET examples-shared )

add_library ( ${TARGET} STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/DemoData.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/DemoData.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Level.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Level.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Helpers.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/FpsCounter.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/WhiteRectangleParticle.hpp"
)

target_include_directories ( ${TARGET}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries ( ${TARGET}
	LINK_PUBLIC dgm::dgm
)

set_target_properties( ${TARGET} PROPERTIES
	FOLDER "examples"
)

target_compile_definitions ( ${TARGET} PUBLIC
	RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../resources"
)

apply_compile_options ( ${TARGET} )
enable_autoformatter ( ${TARGET} )
