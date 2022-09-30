cmake_minimum_required ( VERSION 3.19 )

project ( dgm-lib-sandbox )
copy_dlls_to_build_folders()

add_executable ( ${PROJECT_NAME} WIN32
	"${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp"
)

target_link_libraries ( ${PROJECT_NAME}
	libdgm
)

target_compile_features ( ${PROJECT_NAME}
	INTERFACE ${MY_CXX_COMPILE_FEATURES}
)

target_compile_options ( ${PROJECT_NAME}
	PRIVATE ${CXX_COMPILE_OPTIONS}
)
