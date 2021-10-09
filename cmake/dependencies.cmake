set ( DSH_VERSION   "1.6.1" )
set ( SFML_VERSION  "2.5.1" )
set ( JSON_VERSION  "3.7.3" )
set ( CATCH_VERSION "2.10.2" )

set ( DSH_URL   "https://github.com/nerudaj/dsh/releases/download/v${DSH_VERSION}/dsh-${DSH_VERSION}-vc16-64-bit.zip" )
set ( SFML_URL  "https://github.com/SFML/SFML/releases/download/${SFML_VERSION}/SFML-${SFML_VERSION}-windows-vc15-64-bit.zip" )
set ( JSON_URL  "https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/include.zip" )
set ( CATCH_URL "https://github.com/catchorg/Catch2/releases/download/v${CATCH_VERSION}/catch.hpp" )

include ( FetchContent )

function ( fetch_dependency name url headeronly )
	string ( TOLOWER ${name} lname )

	if ( ${headeronly} )	
		FetchContent_Declare ( ${name}
			URL                  ${url}
			DOWNLOAD_NO_PROGRESS TRUE
			DOWNLOAD_NO_EXTRACT  TRUE
		)
	else ()
		FetchContent_Declare ( ${name}
			URL ${url}
		)
	endif ()
	
	FetchContent_GetProperties ( ${name} )
	if ( NOT "${${name}_POPULATED}" )
		message ( "Populating ${name}" )
		FetchContent_Populate ( ${name} )
		set ( "${name}_POPULATED" TRUE PARENT_SCOPE )
	endif ()
	
	set ( "${name}_FOLDER" "${${lname}_SOURCE_DIR}" PARENT_SCOPE )
endfunction ()

# Download dependencies
fetch_dependency ( DSH   ${DSH_URL}   FALSE )
fetch_dependency ( SFML  ${SFML_URL}  FALSE )
fetch_dependency ( JSON  ${JSON_URL}  FALSE )
fetch_dependency ( CATCH ${CATCH_URL} TRUE )

# Verify folder paths
message ( "Dependencies downloaded to: " )
message ( "  DSH:   ${DSH_FOLDER}" )
message ( "  SFML:  ${SFML_FOLDER}" )
message ( "  JSON:  ${JSON_FOLDER}" )
message ( "  CATCH: ${CATCH_FOLDER}" )

# Make libraries visible to cmake linker
link_directories("${DSH_FOLDER}/lib")
link_directories("${SFML_FOLDER}/lib")

# Create symbols for linking libcfg, libstrings, libleveld and SFML

message ( "Looking for dsh libs" )
find_library(LIB_CFG_D config-d  NAMES config-d.lib  HINTS "${DSH_FOLDER}/lib")
find_library(LIB_STR_D strings-d NAMES strings-d.lib HINTS "${DSH_FOLDER}/lib")
find_library(LIB_LVLD_D leveld-d NAMES leveld-d.lib HINTS "${DSH_FOLDER}/lib")

find_library(LIB_CFG_R config  NAMES config.lib  HINTS "${DSH_FOLDER}/lib")
find_library(LIB_STR_R strings NAMES strings.lib HINTS "${DSH_FOLDER}/lib")
find_library(LIB_LVLD_R leveld NAMES leveld.lib HINTS "${DSH_FOLDER}/lib")

set(LIB_CFG optimized ${LIB_CFG_R} debug ${LIB_CFG_D})
set(LIB_STR optimized ${LIB_STR_R} debug ${LIB_STR_D})
set(LIB_LVLD optimized ${LIB_LVLD_R} debug ${LIB_LVLD_D})
message ( "OK" )

message ( "Looking for SFML" )
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