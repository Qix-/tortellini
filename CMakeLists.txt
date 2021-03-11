project (tortellini)
cmake_minimum_required (VERSION 3.2)

option (TORTELLINI_BUILD_TESTS "Configure test targets for Tortellini" ON)

add_library (tortellini INTERFACE)
target_include_directories (tortellini INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources (tortellini INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/tortellini.hh")

if (TORTELLINI_BUILD_TESTS)
	enable_testing ()
	add_executable (tortellini-tests test.cc catch2.hpp)
	target_link_libraries (tortellini-tests PRIVATE tortellini)

	target_compile_options (tortellini-tests PRIVATE
		$<IF:$<CXX_COMPILER_ID:MSVC>,/W4,-Wall -Wextra -Werror -pedantic>
	)

	add_test (
		NAME tortellini-test
		COMMAND $<TARGET_FILE:tortellini-tests>
	)
endif ()
