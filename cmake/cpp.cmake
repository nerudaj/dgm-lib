set ( CMAKE_GENERATOR_PLATFORM     x64)
set ( CMAKE_CXX_STANDARD           20 )
set ( CMAKE_CXX_STANDARD_REQUIRED  ON )
set ( CMAKE_CXX_EXTENSIONS         OFF )
set ( MY_CXX_COMPILE_FEATURES      cxx_generic_lambdas cxx_range_for cxx_strong_enums )

if (NOT CMAKE_DEBUG_POSTFIX)
	set(CMAKE_DEBUG_POSTFIX "-d")
endif()