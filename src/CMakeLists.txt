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

add_library(${PROJECT_NAME} SHARED ${LIBRARY_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "_e")

target_include_directories(${PROJECT_NAME} PUBLIC "../include")