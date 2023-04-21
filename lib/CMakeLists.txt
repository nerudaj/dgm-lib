cmake_minimum_required ( VERSION 3.19 )

project ( libdgm )

file (
	COPY "${CMAKE_BINARY_DIR}/.clang-format"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}"
)

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
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/JsonLoader.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/LoaderInterface.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Math.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/NavMesh.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Objects.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Particle.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/ParticleSystem.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/ParticleSystemRenderer.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Path.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/ResourceManager.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/TileMap.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Time.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Traits.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/DGM/classes/Utility.hpp"
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
	"${CMAKE_CURRENT_SOURCE_DIR}/src/JsonLoader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/NavMesh.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Objects.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Particle.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ParticleSystem.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ParticleSystemRenderer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ResourceManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/TileMap.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp"
)

# Not contributing to build process, this is here to enable Intellisense
include_directories ( "${CMAKE_CURRENT_SOURCE_DIR}/include" )

add_library ( ${PROJECT_NAME}
	STATIC ${HEADERS} ${SOURCES}
	"${CMAKE_CURRENT_SOURCE_DIR}/.clang-format"
)

target_include_directories ( ${PROJECT_NAME}
	PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
	PUBLIC  "${SFML_FOLDER}/include"
	PRIVATE "${JSON_FOLDER}/single_include/nlohmann"
)

target_compile_features ( ${PROJECT_NAME}
	INTERFACE ${MY_CXX_COMPILE_FEATURES}
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)

target_link_libraries ( ${PROJECT_NAME}
	${LIB_SFML_MAIN} ${LIB_SFML_SYSTEM} ${LIB_SFML_GRAPHICS} ${LIB_SFML_WINDOW} ${LIB_SFML_AUDIO} xinput.lib
)

set ( DEBUG_BINARY_DIR   "${CMAKE_CURRENT_BINARY_DIR}/Debug" )
set ( RELEASE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Release" )

# Bundle dsh libs with DGM
set ( DGM_RAW_D "${DEBUG_BINARY_DIR}/${PROJECT_NAME}-d.lib" )
set ( DGM_RAW_R "${RELEASE_BINARY_DIR}/${PROJECT_NAME}.lib" )
set ( RESULT_D  "${DEBUG_BINARY_DIR}/${PROJECT_NAME}-d.lib" )
set ( RESULT_R  "${RELEASE_BINARY_DIR}/${PROJECT_NAME}.lib" )

# This glues xinput static lib into static library of DGM
add_custom_command ( TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND
		lib.exe /OUT:$<$<CONFIG:debug>:${RESULT_D}>$<$<CONFIG:release>:${RESULT_R}> $<$<CONFIG:debug>:${DGM_RAW_D}>$<$<CONFIG:release>:${DGM_RAW_R}> $<$<CONFIG:debug>:xinput.lib>$<$<CONFIG:release>:xinput.lib>
	COMMENT "lib.exe /OUT:$<$<CONFIG:debug>:${RESULT_D}>$<$<CONFIG:release>:${RESULT_R}> $<$<CONFIG:debug>:${DGM_RAW_D}>$<$<CONFIG:release>:${DGM_RAW_R}> $<$<CONFIG:debug>:${DSH_LIBS_D}>$<$<CONFIG:release>:${DSH_LIBS_R}>"
)

# Testing
enable_testing()
add_subdirectory ( tests )
# add_subdirectory ( benchmarks )

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
	FILES        "${DEBUG_BINARY_DIR}/libdgm-d.pdb"
	DESTINATION  "lib"
)

install (
	DIRECTORY    "${CMAKE_CURRENT_SOURCE_DIR}/include/DGM"
	DESTINATION  "include"
)
