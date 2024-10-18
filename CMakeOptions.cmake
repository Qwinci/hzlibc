option(HEADERS_ONLY "Only install headers" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR
	CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR
	CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
	set(DEFAULT_OPTIMIZED_ASM ON)
else()
	set(DEFAULT_OPTIMIZED_ASM OFF)
endif()

option(OPTIMIZED_ASM "Use optimized assembly routines (turn off if you get compile errors)" ${DEFAULT_OPTIMIZED_ASM})
option(ENABLE_LIBC_TESTING "Enable libc testing" OFF)
option(ANSI_ONLY "Only enable the ansi c part of the libc along with the dynamic linker" OFF)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	set(ARCH_DEFAULT_LIBRARY_PATHS "/usr/local/lib64;/usr/lib64;/usr/local/lib;/usr/lib")
else()
	set(ARCH_DEFAULT_LIBRARY_PATHS "/usr/local/lib32;/usr/lib32;/usr/local/lib;/usr/lib")
endif()

set(DEFAULT_LIBRARY_PATHS "${ARCH_DEFAULT_LIBRARY_PATHS}" CACHE STRING
	"Semicolon-separated list of default library search paths")

if(ANSI_ONLY)
	set(__HZLIBC_ANSI_ONLY 1)
endif()
