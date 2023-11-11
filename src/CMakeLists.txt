set(LIBRARY_SOURCES
    "${libft_malloc_SOURCE_DIR}/include/libft_malloc/libft_malloc.h"
    malloc_utils.h
    malloc_utils.c
    malloc.c
)

add_library(${PROJECT_NAME} SHARED ${LIBRARY_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "../include")