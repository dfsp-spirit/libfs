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
	target_compile_options( run_libfs_tests PRIVATE /W3 /WX )
    target_compile_definitions(run_libfs_tests PRIVATE _CRT_SECURE_NO_WARNINGS) # Disable MSVCC non-standard warnings/errors about fopen, strcpy, etc.
endif()


##### Build the demo app executable. #####

set(SOURCE_FILES_DEMO src/demo_main.cpp include/libfs.h)
add_executable(demo_libfs ${SOURCE_FILES_DEMO})

set_property(TARGET demo_libfs PROPERTY CXX_STANDARD 11)
set_property(TARGET demo_libfs PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET demo_libfs PROPERTY CXX_EXTENSIONS OFF)


if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( demo_libfs PRIVATE -Wall -Wextra -pedantic -Werror )
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( demo_libfs PRIVATE /W3 /WX )
    target_compile_definitions(demo_libfs PRIVATE _CRT_SECURE_NO_WARNINGS) # Disable MSVCC non-standard warnings/errors about fopen, strcpy, etc.
endif()


##### Build the documentation using Doxygen. One needs to run 'make doc' to actually do this. #####

## Hint: if this does not work anymore on your system after software changes, delete the 'CMakeCache.txt' file and try again.
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)
if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen not found, cannot build the documentation.")
    else()
        message("Doxygen found, building docs.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile_cmake)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    message("Doxygen build started.")

    add_custom_target(doc
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_in}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)
else()
    message("Not building docs")
endif()
