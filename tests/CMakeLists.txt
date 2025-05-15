include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG 3ef16ef8b30f52a8f1ff677faa1730e76835aa40
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

macro(libc_test name)
	add_executable(${name}_test ${ARGN})
	target_link_options(${name}_test PRIVATE -Wl,-rpath=${CMAKE_BINARY_DIR})
	target_link_libraries(${name}_test PRIVATE GTest::gtest_main)
	add_dependencies(${name}_test c)
	add_custom_command(TARGET ${name}_test POST_BUILD
		COMMAND patchelf --set-interpreter ${CMAKE_BINARY_DIR}/libc.so $<TARGET_FILE:${name}_test>
	)

	gtest_discover_tests(${name}_test)

	add_executable(${name}_host_test ${ARGN})
	target_link_libraries(${name}_host_test PRIVATE GTest::gtest_main)

	gtest_discover_tests(${name}_host_test)
endmacro()

libc_test(ansi ansi.cpp)
libc_test(posix posix.cpp)
libc_test(manual manual.cpp)
