set(LIBRARY_SOURCES
    "${PROJECT_SOURCE_DIR}/include/libft_malloc/libft_malloc.h"
    malloc_types.h
    malloc_state.h
    pool_strategy.h
    pool_strategy.c
    zone_manager.h
    zone_manager.c
    malloc_utils.h
    malloc_utils.c
    malloc.c
)

add_library(${PROJECT_NAME} SHARED ${LIBRARY_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "../include")