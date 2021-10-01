function (copy_dlls_to_build_folders)
	file (GLOB DEBUG_SFML_DLLS   ${CMAKE_BINARY_DIR}/_deps/sfml-src/bin/*-d-2.dll)
	file (GLOB RELEASE_SFML_DLLS ${CMAKE_BINARY_DIR}/_deps/sfml-src/bin/*[oskmw]-2.dll)
	set ( OPENAL_DLL ${CMAKE_BINARY_DIR}/_deps/sfml-src/bin/openal32.dll )

	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Debug" "${CMAKE_CURRENT_BINARY_DIR}/Release" )

	configure_file (
		${OPENAL_DLL} ${CMAKE_CURRENT_BINARY_DIR}/Debug/ COPYONLY
	)

	configure_file (
		${OPENAL_DLL} ${CMAKE_CURRENT_BINARY_DIR}/Release/ COPYONLY
	)

	foreach ( DLL ${DEBUG_SFML_DLLS} )
		configure_file ( ${DLL} ${CMAKE_CURRENT_BINARY_DIR}/Debug/ COPYONLY )
	endforeach ( DLL )

	foreach ( DLL ${RELEASE_SFML_DLLS} )
		configure_file ( ${DLL} ${CMAKE_CURRENT_BINARY_DIR}/Release/ COPYONLY )
	endforeach ( DLL )
endfunction (copy_dlls_to_build_folders)

function(append_infile_to_outfile INFILE OUTFILE)
	file(READ ${INFILE} CONTENTS)
	file(APPEND ${OUTFILE} "${CONTENTS}")
endfunction()