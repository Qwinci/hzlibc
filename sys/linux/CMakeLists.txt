if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	add_subdirectory(x86_64)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
	add_subdirectory(i386)
else()
	message(FATAL_ERROR "Unsupported architecture ${CMAKE_SYSTEM_PROCESSOR}")
endif()

target_sources(c PRIVATE
	sys.cpp
)
