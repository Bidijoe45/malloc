#include <stdio.h>
#include <stdbool.h>

#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "malloc_utils.h"
#include "pool_strategy.h"

malloc_data g_malloc_data;

long user_allocated_memory;
long malloc_allocated_memory;
long malloc_freed_memory;

void *malloc(size_t size) {
    static bool initialized = false;
    if (initialized == false) {
        initialize_malloc();
        initialized = true;
    }

    //FIXME: debug only
    user_allocated_memory += size;

    if (size <= g_malloc_data.sizes[TINY_ZONE].payload) {
        //FIXME: debug only
        malloc_allocated_memory += g_malloc_data.sizes[TINY_ZONE].chunk;
        return pool_strategy_allocate();
    }
    else if (size <= g_malloc_data.sizes[SMALL_ZONE].payload) {
        //TODO: free list strategy allocaiton
    }
    else {
        //TODO: Large strategy allocations
    }

    printf("Malloc returned NULL\n");

    return NULL;
}

void free(void *ptr) {
    chunk_header *chunk = get_chunk_header(ptr);
    size_metadata chunk_metadata = malloc_read_size_metadata(chunk);

    //printf("free size: %zu\n", chunk_metadata.size);

    if (chunk_metadata.size == g_malloc_data.sizes[TINY_ZONE].chunk) {
        //FIXME: Debug only
        malloc_freed_memory += g_malloc_data.sizes[TINY_ZONE].chunk;
        pool_strategy_free(chunk);
    }
    else if (chunk_metadata.size == g_malloc_data.sizes[SMALL_ZONE].chunk) {
        //TODO: free list strategy free
    }
    else {
        //TODO: Large strategy free
    }
}

void *realloc(void *ptr, size_t size) {
    return 0;
}

void show_alloc_mem() {
    printf("=== ALLOCATED MEMORY ===\n");


    printf("=== ================ ===\n");
}