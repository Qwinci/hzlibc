target_include_directories(c PRIVATE .)
target_sources(c PRIVATE
	cha_cha.cpp
	ctype.cpp
	mutex.cpp
	stack.cpp
	stdio_unlocked.cpp
	string.cpp
	thread.cpp
)

if(NOT ANSI_ONLY)
	target_sources(c PRIVATE dns.cpp)
endif()

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	add_subdirectory(x86_64)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
	add_subdirectory(i386)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
	add_subdirectory(aarch64)
else()
	message(FATAL_ERROR "Unsupported architecture ${CMAKE_SYSTEM_PROCESSOR}")
endif()
