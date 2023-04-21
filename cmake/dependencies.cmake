set ( SFML_VERSION  "2.5.1" )
set ( JSON_VERSION  "3.11.2" )
set ( CATCH_VERSION "2.10.2" )

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
			DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		)
	endif ()
	
	FetchContent_GetProperties ( ${name} )
	if ( NOT "${${name}_POPULATED}" )
		message ( "Populating ${name}" )
		FetchContent_Populate ( ${name} )
		set ( "${name}_POPULATED" TRUE PARENT_SCOPE )
	else ()
		message ( "${name} already populated" )
	endif ()
	
	set ( "${name}_FOLDER" "${${lname}_SOURCE_DIR}" PARENT_SCOPE )
endfunction ()

# Download dependencies
fetch_dependency ( SFML  ${SFML_URL}  FALSE )
fetch_dependency ( JSON  ${JSON_URL}  FALSE )
fetch_dependency ( CATCH ${CATCH_URL} TRUE )

# Verify folder paths
message ( "Dependencies downloaded to: " )
message ( "  SFML:  ${SFML_FOLDER}" )
message ( "  JSON:  ${JSON_FOLDER}" )
message ( "  CATCH: ${CATCH_FOLDER}" )

# Make libraries visible to cmake linker
link_directories("${SFML_FOLDER}/lib")

# Create symbols for linking SFML
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