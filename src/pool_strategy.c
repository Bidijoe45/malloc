#include <unistd.h>

#include "malloc_state.h"
#include "pool_strategy.h"
#include "malloc_types.h"
#include "zone_manager.h"

void pool_strategy_initialize_zone(memory_zone *zone) {
    chunk_header *first_chunk = (chunk_header *)(((char *)zone) + sizeof(memory_zone));
    chunk_header *prev_chunk = NULL;
    size_t total_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - sizeof(memory_zone)) / g_malloc_data.sizes[TINY_ZONE].chunk;
    size_metadata size_metadata;

    size_metadata.in_use = 0;
    size_metadata.size = g_malloc_data.sizes[TINY_ZONE].chunk;
    
    for (size_t i=0; i < total_chunks; i++) {
        char *current_chunk_addr = (char *)first_chunk + (i * g_malloc_data.sizes[TINY_ZONE].chunk);
        chunk_header *current_chunk = (chunk_header *)current_chunk_addr;

        malloc_initialize_chunk(current_chunk);

        if (prev_chunk) {
            current_chunk->prev_chunk = prev_chunk;
            prev_chunk->next_chunk = current_chunk;
        }

        malloc_write_size_metadata(current_chunk, size_metadata);

        prev_chunk = current_chunk;
    }

    g_malloc_data.chunks_list[TINY_ZONE] = first_chunk;
}

void pool_strategy_initialize() {
    g_malloc_data.zones_list[TINY_ZONE] = NULL;
    g_malloc_data.sizes[TINY_ZONE].zone = getpagesize();
    g_malloc_data.sizes[TINY_ZONE].chunk = 128;
    g_malloc_data.sizes[TINY_ZONE].payload = g_malloc_data.sizes[TINY_ZONE].chunk - sizeof(size_t);
    g_malloc_data.chunks_list[TINY_ZONE] = NULL;
}

void *pool_strategy_allocate() {
    chunk_header *chunk = g_malloc_data.chunks_list[TINY_ZONE];
    
    if (chunk == NULL) {
        memory_zone *new_zone = create_zone_by_type(TINY_ZONE);
        pool_strategy_initialize_zone(new_zone);
        chunk = g_malloc_data.chunks_list[TINY_ZONE];
    }

    size_metadata metadata = malloc_read_size_metadata(chunk);
    metadata.in_use = 1;

    malloc_write_size_metadata(chunk, metadata);

    g_malloc_data.chunks_list[TINY_ZONE] = chunk->next_chunk;

    return ((char *)chunk) + sizeof(size_t);
}

void pool_strategy_free(chunk_header *chunk) {
    chunk->prev_chunk = NULL;
    chunk->next_chunk = NULL;
    
    if (g_malloc_data.chunks_list[TINY_ZONE] != NULL) {
        chunk->next_chunk = g_malloc_data.chunks_list[TINY_ZONE];
        g_malloc_data.chunks_list[TINY_ZONE]->prev_chunk = chunk;
    }

    g_malloc_data.chunks_list[TINY_ZONE] = chunk;

    size_metadata metadata = malloc_read_size_metadata(chunk);
    metadata.in_use = 0;

    malloc_write_size_metadata(chunk, metadata);
}