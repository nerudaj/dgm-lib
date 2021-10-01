set ( DSH_VERSION   "1.6.1" )
set ( SFML_VERSION  "2.5.1" )
set ( JSON_VERSION  "3.7.3" )
set ( CATCH_VERSION "2.10.2" )

set ( DSH_URL   "https://github.com/nerudaj/dsh/releases/download/v${DSH_VERSION}/dsh-${DSH_VERSION}-vc16-64-bit.zip" )
set ( SFML_URL  "https://github.com/SFML/SFML/releases/download/${SFML_VERSION}/SFML-${SFML_VERSION}-windows-vc15-64-bit.zip" )
set ( JSON_URL  "https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.hpp" )
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
fetch_dependency ( JSON  ${JSON_URL}  TRUE )
fetch_dependency ( CATCH ${CATCH_URL} TRUE )

# Verify folder paths
message ( "${DSH_FOLDER}" )
message ( "${SFML_FOLDER}" )
message ( "${JSON_FOLDER}" )
message ( "${CATCH_FOLDER}" )

# Make libraries visible to cmake linker
link_directories("${DSH_FOLDER}/lib")
link_directories("${SFML_FOLDER}/lib")

# dsh is internally bundled with DGM while SFML is dynamically linked
find_library(LIB_CFG_D config-d  NAMES config-d.lib  HINTS "${DSH_FOLDER}/lib")
find_library(LIB_STR_D strings-d NAMES strings-d.lib HINTS "${DSH_FOLDER}/lib")
find_library(LIB_LVLD_D leveld-d NAMES leveld-d.lib HINTS "${DSH_FOLDER}/lib")

message ( "${LIB_CFG_D}" )

find_library(LIB_CFG_R config  NAMES config.lib  HINTS "${DSH_FOLDER}/lib")
find_library(LIB_STR_R strings NAMES strings.lib HINTS "${DSH_FOLDER}/lib")
find_library(LIB_LVLD_R leveld NAMES leveld.lib HINTS "${DSH_FOLDER}/lib")

set(LIB_CFG optimized ${LIB_CFG_R} debug ${LIB_CFG_D})
set(LIB_STR optimized ${LIB_STR_R} debug ${LIB_STR_D})
set(LIB_LVLD optimized ${LIB_LVLD_R} debug ${LIB_LVLD_D})
