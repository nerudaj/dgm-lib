set ( CMAKE_GENERATOR_PLATFORM     x64)
# We need cmake to generate /cpp:latest instead of C++20 which lacks some features in MSVC currently
if ( "${CMAKE_VERSION}" VERSION_GREATER "3.19.5" )
	message ( "Sanity: Setting CXX standard to 23" )
	set ( CMAKE_CXX_STANDARD           23 )
else()
	message ( "Sanity: Setting CXX standard to 20" )
	set ( CMAKE_CXX_STANDARD		   20 )
endif()
set ( CMAKE_CXX_STANDARD_REQUIRED  ON )
set ( CMAKE_CXX_EXTENSIONS         OFF )
set ( MY_CXX_COMPILE_FEATURES      cxx_generic_lambdas cxx_range_for cxx_strong_enums )
set ( CXX_COMPILE_OPTIONS          /W4 /MP /we4265 /we4834 )
# C4265 - Missing virtual dtor
# C4834 - Discarding result of [[nodiscard]] function

if (NOT CMAKE_DEBUG_POSTFIX)
	set(CMAKE_DEBUG_POSTFIX "-d")
endif()