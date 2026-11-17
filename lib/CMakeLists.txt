set ( TARGET dgm-lib )

if ( DGM_ENABLE_LINTER )
    set ( MAKE_LIBRARY_OPTIONS ENABLE_LINTER )
endif()

make_static_library ( ${TARGET} ${MAKE_LIBRARY_OPTIONS} )

message ( "Current system: ${CMAKE_SYSTEM_NAME}" )

target_link_libraries ( ${TARGET}
    PUBLIC SFML::System SFML::Window SFML::Graphics
    PRIVATE $<BUILD_INTERFACE:nlohmann_json::nlohmann_json>
)

target_link_libraries ( dgm-lib PUBLIC
    # On Windows, we must additionally link SFML::Main
    $<$<PLATFORM_ID:Windows>:SFML::Main>
    # On Android, we must additionally link android, log
    $<$<PLATFORM_ID:Android>:android;log>
    # On Linux, if we ever support GCC / libstdc++, we must include libstdc++exp (not needed for clang)
    $<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU>>:stdc++exp>
) 

target_compile_features( ${TARGET} PUBLIC cxx_std_23 )

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
    message ( WARNING "dgm-lib: PROJECT_VERSION is empty, git was likely fetched in shallow mode" )
    message ( WARNING "dgm-lib: No install rules will be called" )
endif()
