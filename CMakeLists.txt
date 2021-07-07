cmake_minimum_required(VERSION 3.8.2)
project(libfs)

set(SOURCE_FILES src/main.cpp src/libfs_tests.cpp include/libfs.h)
include_directories( include third_party )
add_executable(LibfsTests ${SOURCE_FILES})

set_property(TARGET LibfsTests PROPERTY CXX_STANDARD 20)


set_property(TARGET LibfsTests PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET LibfsTests PROPERTY CXX_EXTENSIONS OFF)


if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( LibfsTests PRIVATE -Wall -Wextra -pedantic -Werror )
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( LibfsTests PRIVATE /W4 /WX )
endif()
