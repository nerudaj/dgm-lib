cmake_minimum_required ( VERSION 3.26 )

set ( TARGET dgm-lib )

if ( ${ENABLE_LINTER} )
    set ( MAKE_LIBRARY_OPTIONS ENABLE_LINTER )
endif()

make_static_library ( ${TARGET} ${MAKE_LIBRARY_OPTIONS} )

message ( "Current system: ${CMAKE_SYSTEM_NAME}" )

if ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Android" )
    set ( EXTRA_ANDROID_LIBS android log )
endif()

if ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
    set ( EXTRA_LINUX_LIBS stdc++exp )
endif()

if ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
    set ( EXTRA_WINDOWS_LIBS SFML::Main )
endif()

target_link_libraries ( ${TARGET}
    PUBLIC SFML::System SFML::Window SFML::Graphics ${EXTRA_WINDOWS_LIBS} ${EXTRA_ANDROID_LIBS} ${EXTRA_LINUX_LIBS}
    PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
)

if ( ${ENABLE_LEGACY_ANIMATION} )
	target_compile_definitions( ${TARGET} PUBLIC LEGACY_ANIMATION=1 )
endif ()

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
