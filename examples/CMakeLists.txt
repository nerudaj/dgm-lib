cmake_minimum_required ( VERSION 3.26 )

#add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/shared" )

set ( TARGET "examples-common" )

add_library ( ${TARGET} INTERFACE )

target_compile_definitions ( ${TARGET} INTERFACE
	RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)

target_link_libraries ( ${TARGET}
	INTERFACE libdgm #examples-shared libdgm
)

function ( execute_example_as_test TARGET )
	if ( ${BUILD_TESTS} )
		add_test (
			NAME "${TARGET}"
			COMMAND "${TARGET}"
		)
	endif ()
endfunction ()


add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-01-app" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-02-controller" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-03-math" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-04-particle-effects" )

