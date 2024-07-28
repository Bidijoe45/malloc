#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "malloc_utils.h"
#include "pool_strategy.h"
#include "free_list_strategy.h"
#include "large_strategy.h"

malloc_data g_malloc_data;

void *malloc(size_t size) {
    static bool initialized = false;
    if (initialized == false) {
        initialize_malloc();
        initialized = true;
    }

    if (size <= g_malloc_data.sizes[TINY_ZONE].payload) {
        return pool_strategy_allocate();
    }
    else if (size <= g_malloc_data.sizes[SMALL_ZONE].payload) {
        return fls_allocate(size);
    }
    else {
        return lgs_allocate(size);
    }

    return NULL;
}


void free(void *ptr) {
    chunk_header *chunk = get_chunk_header(ptr);
    size_metadata chunk_metadata = malloc_read_size_metadata(chunk);

    if (chunk_metadata.size == g_malloc_data.sizes[TINY_ZONE].chunk) {
        pool_strategy_free(chunk);
    }
    else if (chunk_metadata.size > g_malloc_data.sizes[TINY_ZONE].chunk
            && chunk_metadata.size <= g_malloc_data.sizes[SMALL_ZONE].zone)
    {
        fls_free(chunk, chunk_metadata);
    }
    else {
        lgs_free(chunk, chunk_metadata);
    }
}

void *realloc(void *ptr, size_t size) {
    chunk_header *chunk = get_chunk_header(ptr);
    size_metadata metadata = malloc_read_size_metadata(chunk);

    //From tiny to tiny nothing is needed
    if (size <= g_malloc_data.sizes[TINY_ZONE].payload && metadata.size <= g_malloc_data.sizes[TINY_ZONE].chunk) {
        return ptr;
    }

    char *new_mem = malloc(size);
    size_t i=0;
    while (i < size) {
        new_mem[i] = ((char *)ptr)[i];
        i++;
    }
    free(ptr);

    return new_mem;
}

void show_alloc_mem() {
    printf("=== ALLOCATED MEMORY ===\n");


    printf("=== ================ ===\n");
}