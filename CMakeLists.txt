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
	-fno-exceptions -fno-rtti -ffreestanding -fvisibility=hidden -fvisibility-inlines-hidden
	-fno-stack-protector
	-ftls-model=initial-exec -Wno-missing-attributes -Wno-unused-parameter
)
target_link_options(c PRIVATE -nostdlib -Wl,--entry=_start,--fatal-warnings,--no-undefined,-z,noexecstack)
target_include_directories(c SYSTEM PUBLIC include)
target_include_directories(c PRIVATE sys)

option(OPTIMIZED_ASM "Use optimized assembly routines (turn off if you get compile errors)" OFF)

if(OPTIMIZED_ASM)
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
target_compile_options(libunwind PRIVATE -funwind-tables -fno-exceptions -fno-rtti -nostdinc++ -fvisibility=hidden)
target_compile_definitions(libunwind PRIVATE
	_LIBUNWIND_IS_NATIVE_ONLY
	_LIBUNWIND_HIDE_SYMBOLS)
target_include_directories(libunwind PUBLIC include thirdparty/libunwind/include)

target_link_libraries(c PRIVATE libunwind)

add_library(dummy SHARED dummy.cpp)
target_link_options(dummy PRIVATE -nostdlib)
add_dependencies(c dummy)

macro(dummy_lib name)
	string(REGEX REPLACE "\.so.*" ".so" short_name "${name}")
	add_custom_command(TARGET dummy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:dummy> ${CMAKE_CURRENT_BINARY_DIR}/"${name}"
		COMMAND patchelf --set-soname "${name}" ${CMAKE_CURRENT_BINARY_DIR}/${name})
	if(NOT "${name}" STREQUAL "${short_name}")
		add_custom_command(TARGET dummy POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E create_symlink "${name}" "${CMAKE_CURRENT_BINARY_DIR}/${short_name}"
		)
	endif()
endmacro()

add_custom_command(TARGET dummy POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:dummy> ${CMAKE_CURRENT_BINARY_DIR}/libc.so.6
	COMMAND patchelf --set-soname libc.so.6 ${CMAKE_CURRENT_BINARY_DIR}/libc.so.6
)

add_custom_command(TARGET dummy POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E create_symlink libc.so "${CMAKE_CURRENT_BINARY_DIR}/ld-linux.so.2"
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
