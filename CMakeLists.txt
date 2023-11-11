cmake_minimum_required(VERSION 3.10)

project(
    libft_malloc
    VERSION 1.0
    DESCRIPTION "Reimplementation of malloc for fun"
    LANGUAGES C)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)