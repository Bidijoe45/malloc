#include <unistd.h>
#include <sys/mman.h>

#include "malloc_types.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "pool_strategy.h"

memory_zone *create_zone(size_t size) {
    memory_zone *map = (memory_zone *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    map->next_zone = NULL;

    return map;
}

void delete_zone(memory_zone *zone) {
    //munmap(zone, zone->);
}

void initialize_tiny_zone(memory_zone *zone) {
    size_t chunk_size = getpagesize() / N_CHUNKS_IN_TINY_ZONE;
    size_t total_chunks = N_CHUNKS_IN_TINY_ZONE;
    create_pool_strategy(g_malloc_data.zones[TINY_ZONE], chunk_size, total_chunks);
}

void initialize_small_zone(memory_zone *zone) {

}