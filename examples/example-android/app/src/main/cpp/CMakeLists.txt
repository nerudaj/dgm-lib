cmake_minimum_required ( VERSION 3.28 )

project ( example )

add_subdirectory ( ../../../../../.. )

set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")

add_library(${PROJECT_NAME} SHARED
        main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics dgm::dgm-lib android log)

# The ANativeActivity_onCreate function from SFML::Main needs to be exposed in
# our libsfml-example.so file so that is can be loaded at runtime.
target_link_libraries(${PROJECT_NAME} PUBLIC
        -Wl,--whole-archive
        SFML::Main
        -Wl,--no-whole-archive
)
