cmake_minimum_required ( VERSION 3.26 )

add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/shared" )

set ( TARGET "examples-common" )

add_library ( ${TARGET} INTERFACE )

target_compile_definitions ( ${TARGET} INTERFACE
	RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)

target_link_libraries ( ${TARGET}
	INTERFACE examples-shared libdgm
)

add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-01-tilemap" )
add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-02-controller" )

# add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-01-app" )
# 
# add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-03-math" )
# add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-04-particle-effects" )
# add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-05-pathfind" )
# add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-06-simple-ai" )
# add_subdirectory ( "${CMAKE_CURRENT_SOURCE_DIR}/example-07-spatial-buffer" )


# Future ordering:
## TileMap
## Controller
## Math
## ParticleEffects
## Collisions
## Camera
## App
## Spatial Effect