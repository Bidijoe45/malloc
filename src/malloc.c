#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "malloc_utils.h"
#include "pool_strategy.h"
#include "free_list_strategy.h"
#include "large_strategy.h"

malloc_data g_malloc_data = {
    .zones_list[0] = NULL,
    .zones_list[1] = NULL,
    .zones_list[2] = NULL,
    .chunks_list[0] = NULL,
    .chunks_list[1] = NULL,
    .chunks_list[2] = NULL
};

void *malloc(size_t size) {
    if (size == 0)
        return NULL;

    static bool initialized = false;
    if (initialized == false) {
        initialize_malloc();
        initialized = true;
    }

    size_t aligned_size = ALIGN(size);

    if (aligned_size <= g_malloc_data.sizes[TINY_ZONE].payload) {
        return pool_strategy_allocate();
    }
    if (aligned_size <= g_malloc_data.sizes[SMALL_ZONE].payload) {
        return fls_allocate(aligned_size);
    }
    else if (aligned_size > g_malloc_data.sizes[SMALL_ZONE].payload) {
        return lgs_allocate(aligned_size);
    }

    return NULL;
}


void free(void *ptr) {
    if (ptr == NULL)
        return;

    chunk_header *chunk = get_chunk_header(ptr);
    size_metadata chunk_metadata = malloc_read_size_metadata(chunk);

    if (chunk_metadata.size == g_malloc_data.sizes[TINY_ZONE].chunk) {
        pool_strategy_free(chunk);
    }
    else if (chunk_metadata.size > g_malloc_data.sizes[TINY_ZONE].chunk
            && chunk_metadata.size < g_malloc_data.sizes[SMALL_ZONE].chunk)
    {
        fls_free(chunk, chunk_metadata);
    }
    else {
        lgs_free(chunk, chunk_metadata);
    }
}

void *realloc(void *ptr, size_t size) {
    if (ptr == NULL || size == 0)
        return ptr;
    
    chunk_header *chunk = get_chunk_header(ptr);
    size_metadata metadata = malloc_read_size_metadata(chunk);

    size = ALIGN(size);

    //From tiny to tiny nothing is needed
    if (size <= g_malloc_data.sizes[TINY_ZONE].payload
        && metadata.size <= g_malloc_data.sizes[TINY_ZONE].chunk) {
        return ptr;
    }

    char *new_mem = malloc(size);
    if (new_mem == NULL)
        return NULL;

    size_t size_to_copy = size < (metadata.size - SIZE_T_SIZE) ? size : (metadata.size - SIZE_T_SIZE);
    for (size_t i=0; i < size_to_copy; i++) {
        new_mem[i] = ((char *)ptr)[i];
    }
    free(ptr);

    return new_mem;
}

void show_alloc_mem() {
    write(1, "=== ALLOCATED MEMORY ===\n", strlen("=== ALLOCATED MEMORY ===\n"));
    pool_strategy_print_zone(g_malloc_data.zones_list[TINY_ZONE]);
    fls_print_zones(g_malloc_data.zones_list[SMALL_ZONE]);
    lgs_print_zones(g_malloc_data.zones_list[LARGE_ZONE]);
    write(1, "=== ================ ===\n", strlen("=== ================ ===\n"));
}