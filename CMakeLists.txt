cmake_minimum_required(VERSION 3.10)

project(
    ft_malloc
    VERSION 1.0
    DESCRIPTION "Reimplementation of malloc for fun"
    LANGUAGES C)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CTEST_OUTPUT_ON_FAILURE ON)
enable_testing()

add_subdirectory(src)
add_subdirectory(test)