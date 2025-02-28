cmake_minimum_required ( VERSION 3.26 )

set ( TARGET dgm-lib )

make_static_library ( ${TARGET} )

target_link_libraries ( ${TARGET}
	PUBLIC SFML::System SFML::Window SFML::Graphics SFML::Main
	PUBLIC xinput.lib
	PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
)

if ( NOT "${PROJECT_VERSION}" STREQUAL "" )
    install (
        DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/include"
        DESTINATION  "."
    )

    install ( TARGETS ${TARGET}
        EXPORT ${TARGET}Config
        ARCHIVE DESTINATION LIB
    )

    install ( EXPORT ${TARGET}Config
        NAMESPACE dgm::
        DESTINATION lib/cmake/${TARGET}
    )

    include (CMakePackageConfigHelpers)

    write_basic_package_version_file(
        ${TARGET}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion
    )

    install (
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake
        DESTINATION lib/cmake/${TARGET}
    )
else()
    message ( "PROJECT_VERSION is empty, git was likely fetched in shallow mode" )
    message ( "No install rules will be called" )
endif()
