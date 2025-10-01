cmake_minimum_required ( VERSION 3.26 )

set ( TARGET dgm-lib )

make_static_library ( ${TARGET} )

if ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Android" )
    target_link_libraries ( ${TARGET}
        PUBLIC SFML::System SFML::Window SFML::Graphics SFML::Main android log
        PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
    )
else ()
    target_link_libraries ( ${TARGET}
        PUBLIC SFML::System SFML::Window SFML::Graphics SFML::Main
        PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
    )
endif()

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

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dgm-libConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/dgm-libConfig.cmake"
        INSTALL_DESTINATION lib/cmake/dgm-lib
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    install (
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake
        DESTINATION lib/cmake/${TARGET}
    )
else()
    message ( "PROJECT_VERSION is empty, git was likely fetched in shallow mode" )
    message ( "No install rules will be called" )
endif()
