target_include_directories(c PRIVATE .)
target_sources(c PRIVATE
	log.cpp
	allocator.cpp
)

add_subdirectory(ansi)
add_subdirectory(internal)

if(NOT ANSI_ONLY)
	add_subdirectory(bsd)
	add_subdirectory(glibc)
	add_subdirectory(intl)
	add_subdirectory(linux)
	add_subdirectory(lsb)
	add_subdirectory(posix)
endif()
