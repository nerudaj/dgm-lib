cmake_minimum_required ( VERSION 3.26 )

include ( "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/get_cpp_init.cmake" )

project ( fetch_release )

message ( "Git branch: ${GIT_BRANCH}" )
message ( "Latest tag: ${LATEST_TAG}" )

set ( SFML_VERSION "3.0.0" )

# Fetch zips with compiled libraries
fetch_prebuilt_dependency (
    SFML
    URL "https://github.com/SFML/SFML/releases/download/${SFML_VERSION}/SFML-${SFML_VERSION}-windows-vc17-64-bit.zip"
)

fetch_prebuilt_dependency (
    DGM
    URL "https://github.com/nerudaj/dgm-lib/releases/download/${LATEST_TAG}/dgm-lib-${LATEST_TAG}-Windows-x64-MSVC-143-for-SFML-${SFML_VERSION}.zip"
)

# Create linkable targets for SFML
set ( SFML_STATIC_LIBRARIES ON )
find_package ( SFML
    ${SFML_VERSION} REQUIRED
    COMPONENTS Main Window Graphics
    PATHS "${SFML_FOLDER}/lib/cmake"
)

# Create linkable target for dgm
find_library(LIB_DGM_D libdgm-d  NAMES dgm-lib-d.lib  HINTS "${DGM_FOLDER}/lib")
find_library(LIB_DGM_R libdgm  NAMES dgm-lib.lib  HINTS "${DGM_FOLDER}/lib")
set(LIB_DGM optimized ${LIB_DGM_R} debug ${LIB_DGM_D})

add_library ( Dep_dgm INTERFACE )
target_include_directories ( Dep_dgm INTERFACE "${DGM_FOLDER}/include" )
target_link_libraries ( Dep_dgm INTERFACE ${LIB_DGM} SFML::System SFML::Main SFML::Graphics SFML::Window xinput.lib )

# Create the final executable
make_executable ( ${PROJECT_NAME}
    DEPS Dep_dgm # SFML linked transitively
)