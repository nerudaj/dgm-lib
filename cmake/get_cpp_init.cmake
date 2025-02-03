# Function: fetch_cpp_init
# Description: Fetches the cpp-init setup from the specified Git repository and sets up the CPPINIT_FOLDER path. If SCRIPT_DIR is specified, cpp.cmake and bootstrap.cmake scripts will be copied to that directory.
# Arguments:
#   SCRIPT_DIR - (Optional) Directory to which cpp.cmake and bootstrap.cmake scripts will be copied. If specified, CPPINIT_FOLDER is set to this value. If not, CPPINIT_FOLDER will be set to "${cpp-init_SOURCE_DIR}/cmake".
# Example:
#   fetch_cpp_init()
#   fetch_cpp_init(SCRIPT_DIR "/path/to/scripts")
function ( fetch_cpp_init )
    set ( oneValueArgs SCRIPT_DIR )
    cmake_parse_arguments ( FETCH_CPP_INIT "" "${oneValueArgs}" "" ${ARGN} )

    include ( FetchContent )
    FetchContent_Declare(
        cpp-init
        GIT_REPOSITORY https://github.com/nerudaj/cpp-init
        GIT_TAG "v0.4.0"
    )
    FetchContent_MakeAvailable ( cpp-init )
    
    if ( FETCH_CPP_INIT_SCRIPT_DIR )
        set ( CPPINIT_FOLDER "${FETCH_CPP_INIT_SCRIPT_DIR}" )
        file ( COPY_FILE
            "${cpp-init_SOURCE_DIR}/cmake/cpp.cmake"
            "${CPPINIT_FOLDER}/cpp.cmake"
        )
        file ( COPY_FILE
            "${cpp-init_SOURCE_DIR}/cmake/bootstrap.cmake"
            "${CPPINIT_FOLDER}/bootstrap.cmake"
        )
        file ( COPY_FILE
            "${cpp-init_SOURCE_DIR}/cmake/macros.cmake"
            "${CPPINIT_FOLDER}/macros.cmake"
        )
    else ()
        set( CPPINIT_FOLDER "${cpp-init_SOURCE_DIR}/cmake" )
    endif ()
    
    set ( CLANG_FORMAT_PATH "${cpp-init_SOURCE_DIR}/.clang-format" PARENT_SCOPE )
    set ( CLANG_TIDY_PATH "${cpp-init_SOURCE_DIR}/.clang-tidy" PARENT_SCOPE )

    return( PROPAGATE CPPINIT_FOLDER )
endfunction()

# set ( CPP_INIT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

fetch_cpp_init ( SCRIPT_DIR "${CPP_INIT_ROOT_DIR}" )

include ( "${CPPINIT_FOLDER}/cpp.cmake" )
include ( "${CPPINIT_FOLDER}/bootstrap.cmake" )
include ( "${CPPINIT_FOLDER}/macros.cmake" )
