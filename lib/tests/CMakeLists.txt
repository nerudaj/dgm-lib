cmake_minimum_required ( VERSION 3.19 )

project ( lib-testrunner )

include_directories ( ${CATCH_FOLDER} )

set ( SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Math.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Conversion.cpp"
)

add_executable ( ${PROJECT_NAME}
	${SOURCES}
)

target_link_libraries ( ${PROJECT_NAME}
	libdgm
)

set_property (TARGET ${PROJECT_NAME}
    PROPERTY VS_PACKAGE_REFERENCES "MSTest.TestAdapter_2.2.7"
)

add_test (
	NAME    ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
