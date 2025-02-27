cmake_minimum_required ( VERSION 3.26 )

include ( "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/cpp-init.cmake" )

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

find_package ( DGM
    ${DGM_VERSION} REQUIRED
    PATHS "${DGM_FOLDER}/lib/cmake"
)

# Create the final executable
make_executable ( ${PROJECT_NAME}
    DEPS dgm::dgm-lib # SFML linked transitively
)