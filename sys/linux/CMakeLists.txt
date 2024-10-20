if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	add_subdirectory(x86_64)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
	add_subdirectory(i386)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
	add_subdirectory(aarch64)
else()
	message(FATAL_ERROR "Unsupported architecture ${CMAKE_SYSTEM_PROCESSOR}")
endif()

target_sources(c PRIVATE
	sys.cpp
)

target_sources(c PUBLIC FILE_SET HEADERS
	BASE_DIRS aarch64/include
	FILES aarch64/include/bits/syscall_nums_aarch64.h
)

target_sources(c PUBLIC FILE_SET HEADERS
	BASE_DIRS i386/include
	FILES i386/include/bits/syscall_nums_i386.h
)

target_sources(c PUBLIC FILE_SET HEADERS
	BASE_DIRS x86_64/include
	FILES x86_64/include/bits/syscall_nums_x86_64.h
)

target_sources(c PUBLIC FILE_SET HEADERS
	BASE_DIRS include
	FILES
	include/syscall.h
	include/sys/syscall.h
)

target_include_directories(c PUBLIC include)
