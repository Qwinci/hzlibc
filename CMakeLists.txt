cmake_minimum_required(VERSION 3.20)
project(hzlibc)

set(CMAKE_CXX_STANDARD 20)

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
	-ftls-model=initial-exec -Wno-missing-attributes
)
target_link_options(c PRIVATE -nostdlib -Wl,--entry=_start,--fatal-warnings,--no-undefined)
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

add_executable(libc_test test.c)
add_dependencies(libc_test c)
target_link_options(libc_test PRIVATE -Wl,-rpath=$ORIGIN)
add_custom_command(TARGET libc_test POST_BUILD
	COMMAND patchelf --set-interpreter ${CMAKE_CURRENT_BINARY_DIR}/libc.so $<TARGET_FILE:libc_test>
)

add_library(libc_test_lib SHARED lib.c)
add_dependencies(libc_test libc_test_lib)
# target_link_libraries(libc_test PRIVATE libc_test_lib)
