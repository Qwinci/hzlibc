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

if(ANSI_ONLY)
	set(__HZLIBC_ANSI_ONLY 1)
endif()
