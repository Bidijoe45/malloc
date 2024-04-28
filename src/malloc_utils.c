#include <stdio.h>
#include <unistd.h>

#include "malloc_state.h"
#include "malloc_utils.h"
#include "zone_manager.h"

void initialize_malloc() {
    g_malloc_data.tiny_zone_size = getpagesize();
    g_malloc_data.small_zone_size = getpagesize() * 4;

    g_malloc_data.zones[TINY_ZONE] = NULL;
    g_malloc_data.zones[SMALL_ZONE] = NULL;
    g_malloc_data.zones[LARGE_ZONE] = NULL;

    g_malloc_data.zones[TINY_ZONE] = create_zone(g_malloc_data.tiny_zone_size);
    initialize_tiny_zone(g_malloc_data.zones[TINY_ZONE]);
}

void print_all() {
    memory_zone *zone = g_malloc_data.zones[TINY_ZONE];
    chunk_header *chunk = (chunk_header *)(zone + 1);

    size_t total_chunks = 0;
    while (chunk) {
        printf("=== CHUNK ===\n");
        printf("chunk_ptr: %p\n", chunk);
        printf("chunk_size: %zu\n", chunk->size);
        printf("chunk_prev: %p\n", chunk->prev_chunk);
        printf("chunk_next: %p\n", chunk->next_chunk);

        chunk = chunk->next_chunk;
        total_chunks++;
    }

    printf("total chunks: %zu\n", total_chunks);
}