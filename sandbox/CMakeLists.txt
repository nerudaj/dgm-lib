cmake_minimum_required ( VERSION 3.26 )

set ( TARGET sandbox )

add_executable ( ${TARGET} WIN32
	"${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp"
)

target_link_libraries ( ${TARGET}
	LINK_PRIVATE dgm::dgm
)

apply_compile_options ( ${TARGET} )
