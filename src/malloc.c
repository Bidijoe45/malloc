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

long user_allocated_memory;
long malloc_allocated_memory;
long malloc_freed_memory;

void sigv_handler(int sig_num)
{
    printf("SIGV catched\n");
    hexdump(g_malloc_data.zones_list[SMALL_ZONE], g_malloc_data.sizes[SMALL_ZONE].zone);
    printf("User allcoated memory: %ld\n", user_allocated_memory);
    printf("Malloc allcoated memory: %ld\n", malloc_allocated_memory);
    printf("Malloc freed memory: %ld\n", malloc_freed_memory);
    exit(1);
}

void atexit_handler() {
    //hexdump(g_malloc_data.zones_list[SMALL_ZONE], g_malloc_data.sizes[SMALL_ZONE].zone);
    visualize_memory();
    exit(0);
}

void *malloc(size_t size) {
    printf("my malloc\n");
    static bool initialized = false;
    if (initialized == false) {
        initialize_malloc();
        initialized = true;
        //signal(SIGSEGV, sigv_handler);
        atexit(atexit_handler);  
    }

    //FIXME: debug only
    user_allocated_memory += size;

    if (size <= g_malloc_data.sizes[TINY_ZONE].payload) {
        //FIXME: debug only
        malloc_allocated_memory += g_malloc_data.sizes[TINY_ZONE].chunk;

        return pool_strategy_allocate();
    }
    else if (size <= g_malloc_data.sizes[SMALL_ZONE].payload) {
        return fls_allocate(size);
    }
    else {
        return lgs_allocate(size);
    }

    printf("Malloc returned NULL\n");

    return NULL;
}


void free(void *ptr) {
    chunk_header *chunk = get_chunk_header(ptr);
    size_metadata chunk_metadata = malloc_read_size_metadata(chunk);

    if (chunk_metadata.size == g_malloc_data.sizes[TINY_ZONE].chunk) {
        //FIXME: Debug only
        malloc_freed_memory += g_malloc_data.sizes[TINY_ZONE].chunk;
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
    return 0;
}

void show_alloc_mem() {
    printf("=== ALLOCATED MEMORY ===\n");


    printf("=== ================ ===\n");
}