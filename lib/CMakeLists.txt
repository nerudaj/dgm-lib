cmake_minimum_required ( VERSION 3.19 )

project ( libdgm )

set ( HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/dgm.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Animation.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/App.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/AppState.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Buffer.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Camera.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Clip.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Collision.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Controller.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Conversion.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Error.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Math.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Objects.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Particle.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/ParticleSystem.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/ParticleSystemRenderer.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/ResourceManager.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/RingBuffer.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Shell.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/TileMap.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Time.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Window.hpp"
)

set ( SOURCES 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Animation.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/App.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Clip.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Collision.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Controller.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Conversion.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Objects.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Particle.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ParticleSystem.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ParticleSystemRenderer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ResourceManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Shell.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/TileMap.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp"
)

# Not contributing to build process, this is here to enable Intellisense
include_directories ( "${CMAKE_CURRENT_SOURCE_DIR}/include" )

add_library ( ${PROJECT_NAME}
	STATIC ${HEADERS} ${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.editorconfig"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
	PUBLIC  "${SFML_FOLDER}/include"
	PUBLIC  "${DSH_FOLDER}/include"
	PRIVATE "${JSON_FOLDER}/single_include/nlohmann"
)

target_compile_features ( ${PROJECT_NAME}
	INTERFACE ${MY_CXX_COMPILE_FEATURES}
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE /W4 /MP
)

target_link_libraries ( ${PROJECT_NAME}
	${LIB_CFG} ${LIB_LVLD} ${LIB_STR} ${LIB_SFML_MAIN} ${LIB_SFML_SYSTEM} ${LIB_SFML_GRAPHICS} ${LIB_SFML_WINDOW} ${LIB_SFML_AUDIO}
)

set ( DEBUG_BINARY_DIR   "${CMAKE_CURRENT_BINARY_DIR}/Debug" )
set ( RELEASE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Release" )

# Bundle dsh libs with DGM
set ( DGM_RAW_D "${DEBUG_BINARY_DIR}/${PROJECT_NAME}-d.lib" )
set ( DGM_RAW_R "${RELEASE_BINARY_DIR}/${PROJECT_NAME}.lib" )
set ( RESULT_D  "${DEBUG_BINARY_DIR}/${PROJECT_NAME}-d.lib" )
set ( RESULT_R  "${RELEASE_BINARY_DIR}/${PROJECT_NAME}.lib" )

# This glues static libraries of dsh into static library of DGM
# it respects debug/release versions
add_custom_command ( TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND
		lib.exe /OUT:$<$<CONFIG:debug>:${RESULT_D}>$<$<CONFIG:release>:${RESULT_R}> $<$<CONFIG:debug>:${DGM_RAW_D}>$<$<CONFIG:release>:${DGM_RAW_R}> $<$<CONFIG:debug>:${LIB_CFG_D}>$<$<CONFIG:release>:${LIB_CFG_R}> $<$<CONFIG:debug>:${LIB_LVLD_D}>$<$<CONFIG:release>:${LIB_LVLD_R}> $<$<CONFIG:debug>:${LIB_STR_D}>$<$<CONFIG:release>:${LIB_STR_R}>
	COMMENT "lib.exe /OUT:$<$<CONFIG:debug>:${RESULT_D}>$<$<CONFIG:release>:${RESULT_R}> $<$<CONFIG:debug>:${DGM_RAW_D}>$<$<CONFIG:release>:${DGM_RAW_R}> $<$<CONFIG:debug>:${DSH_LIBS_D}>$<$<CONFIG:release>:${DSH_LIBS_R}>"
)

# Testing
enable_testing()
add_subdirectory ( tests )
add_subdirectory ( benchmarks )

# Instalation
install (
	FILES        "${RELEASE_BINARY_DIR}/libdgm.lib"
	DESTINATION  "lib"
)

install (
	FILES        "${DEBUG_BINARY_DIR}/libdgm-d.lib"
	DESTINATION  "lib"
)

install (
	DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/include/DGM"
	DESTINATION  "include"
)

install (
	DIRECTORY    "${DSH_FOLDER}/include"
	DESTINATION  "include"
)
