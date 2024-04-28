#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

#include "malloc_types.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "pool_strategy.h"

memory_zone *create_zone(memory_zone *last_zone, size_t size) {
    memory_zone *map = (memory_zone *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    map->next_zone = NULL;

    if (last_zone != NULL)
        last_zone->next_zone = map;

    return map;
}

void delete_zone(memory_zone *zone) {
    //TODO:
    //munmap(zone, zone->);
}

void initialize_tiny_zone(memory_zone *zone) {
    size_t chunk_size = getpagesize() / N_CHUNKS_IN_TINY_ZONE;
    size_t total_chunks = N_CHUNKS_IN_TINY_ZONE;
    g_malloc_data.tiny_zone_chunk_size = chunk_size;
    g_malloc_data.tiny_zone_payload_size = chunk_size - (sizeof(size_t));
    create_pool_strategy(g_malloc_data.zones[TINY_ZONE], chunk_size, total_chunks);

    printf("tiny_zone_chunk_size: %zu\n", g_malloc_data.tiny_zone_chunk_size);
}

void initialize_small_zone(memory_zone *zone) {
    //TODO: 
}