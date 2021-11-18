cmake_minimum_required ( VERSION 3.19 )

project ( dgm-lib-benchmark )

copy_dlls_to_build_folders ()

# Bootstrap benchmark
set ( GOOGLEBENCH_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/googlebench" )
set ( GOOGLEBENCH_VERSION "1.6.0" )
set ( GOOGLEBENCH_RELEASE_URL "https://github.com/google/benchmark/archive/refs/tags/v${GOOGLEBENCH_VERSION}.zip" )
set ( GOOGLEBENCH_ARTIFACT_PATH "${CMAKE_CURRENT_BINARY_DIR}/googlebench.zip" )

if ( NOT EXISTS "${GOOGLEBENCH_SOURCE_DIR}" )
	message ( "Downloading Google Benchmark" )
	file ( DOWNLOAD "${GOOGLEBENCH_RELEASE_URL}" "${GOOGLEBENCH_ARTIFACT_PATH}" )
	file ( ARCHIVE_EXTRACT INPUT "${GOOGLEBENCH_ARTIFACT_PATH}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}" )
	file ( RENAME "${CMAKE_CURRENT_SOURCE_DIR}/benchmark-${GOOGLEBENCH_VERSION}" "${GOOGLEBENCH_SOURCE_DIR}" )
else()
	message ( "Google Benchmark already downloaded" )
endif()

set ( BENCHMARK_ENABLE_TESTING OFF )
add_subdirectory ( "${GOOGLEBENCH_SOURCE_DIR}" )

# Create compile target
set ( SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp"
)

add_executable ( ${PROJECT_NAME}
	${SOURCES}
)

target_link_libraries ( ${PROJECT_NAME}
	libdgm benchmark::benchmark
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE /W4 /MP
)

