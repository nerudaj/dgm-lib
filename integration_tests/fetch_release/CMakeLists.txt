cmake_minimum_required ( VERSION 3.26 )

include ( "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/get_cpp_init.cmake" )

project ( fetch_release )

message ( "Git branch: ${GIT_BRANCH}" )
message ( "Latest tag: ${LATEST_TAG}" )

set ( SFML_VERSION "2.6.1" )

# Fetch zips with compiled libraries
fetch_prebuilt_dependency (
    SFML
    URL "https://github.com/SFML/SFML/releases/download/${SFML_VERSION}/SFML-${SFML_VERSION}-windows-vc17-64-bit.zip"
)

fetch_prebuilt_dependency (
    DGM
    URL "https://github.com/nerudaj/dgm-lib/releases/download/${LATEST_TAG}/dgm-lib-${LATEST_TAG}-winx64-vc17-x64-for-SFML-${SFML_VERSION}.zip"
)

# Create linkable targets for SFML
find_library ( LIB_SFML_MAIN_D "sfml-main-d" NAMES "sfml-main-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_SYSTEM_D "sfml-system-d" NAMES "sfml-system-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_GRAPHICS_D "sfml-graphics-d" NAMES "sfml-graphics-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_WINDOW_D "sfml-window-d" NAMES "sfml-window-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_AUDIO_D "sfml-audio-d" NAMES "sfml-audio-d.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_NETWORK_D "sfml-network-d" NAMES "sfml-network-d.lib" HINTS "${SFML_FOLDER}/lib" )

find_library ( LIB_SFML_MAIN_R "sfml-main" NAMES "sfml-main.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_SYSTEM_R "sfml-system" NAMES "sfml-system.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_GRAPHICS_R "sfml-graphics" NAMES "sfml-graphics.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_WINDOW_R "sfml-window" NAMES "sfml-window.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_AUDIO_R "sfml-audio" NAMES "sfml-audio.lib" HINTS "${SFML_FOLDER}/lib" )
find_library ( LIB_SFML_NETWORK_R "sfml-network" NAMES "sfml-network.lib" HINTS "${SFML_FOLDER}/lib" )

set ( LIB_SFML_MAIN optimized ${LIB_SFML_MAIN_R} debug ${LIB_SFML_MAIN_D} )
set ( LIB_SFML_SYSTEM optimized ${LIB_SFML_SYSTEM_R} debug ${LIB_SFML_SYSTEM_D} )
set ( LIB_SFML_GRAPHICS optimized ${LIB_SFML_GRAPHICS_R} debug ${LIB_SFML_GRAPHICS_D} )
set ( LIB_SFML_WINDOW optimized ${LIB_SFML_WINDOW_R} debug ${LIB_SFML_WINDOW_D} )
set ( LIB_SFML_AUDIO optimized ${LIB_SFML_AUDIO_R} debug ${LIB_SFML_AUDIO_D} )
set ( LIB_SFML_NETWORK optimized ${LIB_SFML_NETWORK_R} debug ${LIB_SFML_NETWORK_D} )
set ( LIB_SFML ${LIB_SFML_MAIN} ${LIB_SFML_SYSTEM} ${LIB_SFML_GRAPHICS} ${LIB_SFML_WINDOW} ${LIB_SFML_AUDIO} ${LIB_SFML_NETWORK} )

add_library ( Dep_sfml INTERFACE )
target_include_directories ( Dep_sfml INTERFACE "${SFML_FOLDER}/include" )
target_link_libraries ( Dep_sfml INTERFACE ${LIB_SFML} )

# Create linkable target for dgm
find_library(LIB_DGM_D libdgm-d  NAMES libdgm-d.lib  HINTS "${DGM_FOLDER}/lib")
find_library(LIB_DGM_R libdgm  NAMES libdgm.lib  HINTS "${DGM_FOLDER}/lib")
set(LIB_DGM optimized ${LIB_DGM_R} debug ${LIB_DGM_D})

add_library ( Dep_dgm INTERFACE )
target_include_directories ( Dep_dgm INTERFACE "${DGM_FOLDER}/include" )
target_link_libraries ( Dep_dgm INTERFACE ${LIB_DGM} Dep_sfml xinput.lib )

# Create the final executable
make_executable ( ${PROJECT_NAME}
    DEPS Dep_dgm # SFML linked transitively
)