cmake_minimum_required(VERSION 3.10)

project(
    libft_malloc
    VERSION 1.0
    DESCRIPTION "Reimplementation of malloc for fun"
    LANGUAGES C)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
#set(CMAKE_VERBOSE_MAKEFILE TRUE)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)