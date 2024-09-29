cmake_minimum_required ( VERSION 3.26 )

project ( examples )

add_compile_definitions ( RESOURCE_DIR="${PROJECT_SOURCE_DIR}/resources" )

add_subdirectory ( "${PROJECT_SOURCE_DIR}/example-02-controller" )
