set(LIBRARY_SOURCES
    "${PROJECT_SOURCE_DIR}/include/libft_malloc/libft_malloc.h"
    malloc_types.h
    malloc_state.h
    pool_strategy.h
    pool_strategy.c
    free_list_strategy.c
    free_list_strategy.h
    large_strategy.c
    large_strategy.h
    zone_manager.h
    zone_manager.c
    malloc_utils.h
    malloc_utils.c
    malloc.c
)

if (DEFINED ENV{HOSTTYPE})
    set(HOSTTYPE "$ENV{HOSTTYPE}")
else()
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND uname -s
        OUTPUT_VARIABLE OS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(HOSTTYPE "${ARCH}_${OS}")
endif()

add_library(${PROJECT_NAME} SHARED ${LIBRARY_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_NAME "ft_malloc")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "_${HOSTTYPE}.so")
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

get_target_property(LIBRARY_OUTPUT_NAME ${PROJECT_NAME} LIBRARY_OUTPUT_NAME)
get_target_property(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_NAME} LIBRARY_OUTPUT_DIRECTORY)

set(library_name "libft_malloc_${HOSTTYPE}.so")
ADD_CUSTOM_TARGET(link_target ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink "${library_name}" "${LIBRARY_OUTPUT_DIRECTORY}/libft_malloc.so")

target_include_directories(${PROJECT_NAME} PUBLIC "../include")