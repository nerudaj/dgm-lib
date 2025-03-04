cmake_minimum_required ( VERSION 3.26 )

add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/shared" )

set ( TARGET "examples-common" )

add_library ( ${TARGET} INTERFACE )

target_link_libraries ( ${TARGET}
	INTERFACE examples-shared
)

add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-01-tilemap" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-02-controller" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-03-math" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-04-particle-effects" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-05-camera-manual-control" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-06-spatial-buffer" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-07-texture-atlas" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-08-pathfind" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-09-collisions" )
