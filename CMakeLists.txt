cmake_minimum_required(VERSION 3.20)
project(hzlibc C CXX ASM)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(c SHARED
	src/rtld/main.cpp
	src/rtld/shared_object.cpp
	src/rtld/object_storage.cpp
	src/rtld/start.cpp
	src/rtld/rtld.cpp
	src/cxx.cpp
)
target_compile_options(c PRIVATE
	-Wall -Wextra
	-ffreestanding -fvisibility=hidden
	-fno-stack-protector
	-ftls-model=initial-exec -Wno-missing-attributes -Wno-unused-parameter
)
target_compile_definitions(c PRIVATE
	__USE_XOPEN2K8
)
target_compile_options(c PRIVATE
	$<$<COMPILE_LANGUAGE:CXX>: -fno-exceptions -fno-rtti -fvisibility-inlines-hidden -nostdinc++>)
target_link_options(c PRIVATE -nostdlib -Wl,--entry=_start,--fatal-warnings,--no-undefined,-z,noexecstack)

check_cxx_compiler_flag(-Wno-unknown-warning-option COMPILER_SUPPORTS_UNKNOWN_WARNING_OPT)
if(COMPILER_SUPPORTS_UNKNOWN_WARNING_OPT)
	target_compile_options(c PRIVATE -Wno-unknown-warning-option)
endif()

target_include_directories(c SYSTEM PUBLIC include)
target_include_directories(c PRIVATE sys)

option(OPTIMIZED_ASM "Use optimized assembly routines (turn off if you get compile errors)" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR
	CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR
	CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
	set(OPTIMIZED_ASM ON)
endif()

if(OPTIMIZED_ASM)
	message(STATUS "Using optimized assembly routines")
	target_compile_definitions(c PRIVATE -DOPTIMIZED_ASM)
endif()

add_subdirectory(src)
add_subdirectory(sys)

add_subdirectory(hzutils)
target_link_libraries(c PRIVATE hzutils)

add_library(libunwind OBJECT
	thirdparty/libunwind/src/UnwindRegistersRestore.S
	thirdparty/libunwind/src/UnwindRegistersSave.S
	thirdparty/libunwind/src/UnwindLevel1.c
	thirdparty/libunwind/src/UnwindLevel1-gcc-ext.c
	thirdparty/libunwind/src/libunwind.cpp
)
target_compile_options(libunwind PRIVATE
	$<$<COMPILE_LANGUAGE:CXX>: -fno-exceptions -fno-rtti -fvisibility-inlines-hidden -nostdinc++>)
target_compile_options(libunwind PRIVATE
	-funwind-tables -fvisibility=hidden)
target_compile_definitions(libunwind PRIVATE
	_LIBUNWIND_IS_NATIVE_ONLY
	_LIBUNWIND_HIDE_SYMBOLS)
target_include_directories(libunwind PUBLIC include thirdparty/libunwind/include)

target_link_libraries(c PRIVATE libunwind)

add_library(dummy SHARED dummy.cpp)
target_link_options(dummy PRIVATE -nostdlib)
add_dependencies(c dummy)

macro(dummy_lib name)
	string(REGEX REPLACE "\\.so.*" ".so" short_name "${name}")
	message(STATUS "name ${name} short ${short_name}")
	add_custom_command(TARGET dummy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:dummy> "${CMAKE_CURRENT_BINARY_DIR}/${name}"
		COMMAND patchelf --set-soname "${name}" "${CMAKE_CURRENT_BINARY_DIR}/${name}")
	if(NOT "${name}" STREQUAL "${short_name}")
		add_custom_command(TARGET dummy POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E create_symlink "${name}" "${CMAKE_CURRENT_BINARY_DIR}/${short_name}"
		)
		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${short_name}" TYPE LIB)
	endif()

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}" TYPE LIB)
endmacro()

add_custom_command(TARGET dummy POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:dummy> ${CMAKE_CURRENT_BINARY_DIR}/libc.so.6
	COMMAND patchelf --set-soname libc.so.6 ${CMAKE_CURRENT_BINARY_DIR}/libc.so.6
)

add_custom_command(TARGET dummy POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E create_symlink libc.so "${CMAKE_CURRENT_BINARY_DIR}/ld-linux.so.2"
	COMMAND ${CMAKE_COMMAND} -E create_symlink libc.so "${CMAKE_CURRENT_BINARY_DIR}/ld-linux-x86-64.so.2"
)

dummy_lib("libanl.so.1")
dummy_lib("libc_malloc_debug.so.0")
dummy_lib("libdl.so.2")
dummy_lib("libm.so.6")
dummy_lib("libmemusage.so")
dummy_lib("libmvec.so.1")
dummy_lib("libnsl.so.1")
dummy_lib("libnss_compat.so.2")
dummy_lib("libnss_db.so.2")
dummy_lib("libnss_dns.so.2")
dummy_lib("libnss_files.so.2")
dummy_lib("libnss_hesiod.so.2")
dummy_lib("libpcprofile.so")
dummy_lib("libpthread.so.0")
dummy_lib("libresolv.so.2")
dummy_lib("librt.so.1")
dummy_lib("libthread_db.so.1")
dummy_lib("libutil.so.1")

target_sources(c PUBLIC FILE_SET HEADERS
	BASE_DIRS include
	FILES

	# ansi
	include/assert.h
	include/ctype.h
	include/errno.h
	include/fenv.h
	include/inttypes.h
	include/limits.h
	include/math.h
	include/sigjmp.h
	include/signal.h
	include/stdint.h
	include/stdio.h
	include/stdlib.h
	include/string.h
	include/threads.h
	include/time.h
	include/uchar.h
	include/wchar.h
	include/wctype.h

	# posix
	include/dirent.h
	include/dlfcn.h
	include/elf.h
	include/endian.h
	include/fcntl.h
	include/fnmatch.h
	include/getopt.h
	include/grp.h
	include/iconv.h
	include/ifaddrs.h
	include/langinfo.h
	include/locale.h
	include/mntent.h
	include/netdb.h
	include/nl_types.h
	include/poll.h
	include/pthread.h
	include/pwd.h
	include/regex.h
	include/resolv.h
	include/sched.h
	include/search.h
	include/semaphore.h
	include/spawn.h
	include/strings.h
	include/syslog.h
	include/termios.h
	include/ucontext.h
	include/unistd.h
	include/utime.h
	include/arpa/inet.h
	include/arpa/nameser.h
	include/net/if.h
	include/netinet/in.h
	include/sys/ioctl.h
	include/sys/mman.h
	include/sys/poll.h
	include/sys/resource.h
	include/sys/select.h
	include/sys/sem.h
	include/sys/shm.h
	include/sys/socket.h
	include/sys/stat.h
	include/sys/statvfs.h
	include/sys/time.h
	include/sys/types.h
	include/sys/uio.h
	include/sys/un.h
	include/sys/utsname.h
	include/sys/wait.h

	# glibc
	include/error.h
	include/execinfo.h
	include/features.h
	include/libgen.h
	include/link.h
	include/malloc.h
	include/memory.h
	include/stdio_ext.h
	include/sys/sysmacros.h

	# libintl
	include/libintl.h

	# linux
	include/attr/xattr.h
	include/sys/capability.h
	include/sys/epoll.h
	include/sys/eventfd.h
	include/sys/inotify.h
	include/sys/io.h
	include/sys/ipc.h
	include/sys/personality.h
	include/sys/prctl.h
	include/sys/random.h
	include/sys/sendfile.h
	include/sys/statfs.h
	include/sys/sysinfo.h
	include/sys/xattr.h

	# internal
	include/bits/iov.h
	include/bits/mbstate_t.h
	include/bits/seek.h
	include/bits/sigset_t.h
	include/bits/utils.h

	# bsd
	include/bsd/stdlib.h
	include/sys/file.h
)

install(TARGETS c)
install(TARGETS c FILE_SET HEADERS)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ld-linux-x86-64.so.2" TYPE LIB)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ld-linux.so.2" TYPE LIB)
else()
	message(FATAL_ERROR "Unsupported architecture ${CMAKE_SYSTEM_PROCESSOR}")
endif()
