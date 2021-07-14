cmake_minimum_required(VERSION 3.8.2)
project(libfs)

##### Build the unit test executable. #####

set(SOURCE_FILES src/main.cpp src/libfs_tests.cpp include/libfs.h)
include_directories( include third_party ) # For the unit test framework (catch.hpp).
add_executable(run_libfs_tests ${SOURCE_FILES})

set_property(TARGET run_libfs_tests PROPERTY CXX_STANDARD 11)
set_property(TARGET run_libfs_tests PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET run_libfs_tests PROPERTY CXX_EXTENSIONS OFF)


if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( run_libfs_tests PRIVATE -Wall -Wextra -pedantic -Werror )
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( run_libfs_tests PRIVATE /W4 /WX )
endif()


##### Build the documentation using Doxygen. One needs to run 'make doc' to actually do this. #####

find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API        
documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
         message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/libfs.doxyfile)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    message("Doxygen build started.")

    add_custom_target(doc
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_in}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
endif()
