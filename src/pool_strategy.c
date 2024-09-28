#include <stdint.h>
#include <unistd.h>

#include "malloc_state.h"
#include "pool_strategy.h"
#include "malloc_types.h"
#include "zone_manager.h"
#include "malloc_utils.h"

void pool_strategy_initialize() {
    g_malloc_data.zones_list[TINY_ZONE] = NULL;
    g_malloc_data.chunks_list[TINY_ZONE] = NULL;
    g_malloc_data.sizes[TINY_ZONE].zone = getpagesize();
    g_malloc_data.sizes[TINY_ZONE].chunk = g_malloc_data.sizes[TINY_ZONE].zone / 128;
    g_malloc_data.sizes[TINY_ZONE].payload = g_malloc_data.sizes[TINY_ZONE].chunk - SIZE_T_SIZE;
}

void pool_strategy_initialize_zone(memory_zone *zone) {
    chunk_header *first_chunk = (chunk_header *)(((uint8_t *)zone) + MEMORY_ZONE_SIZE);
    chunk_header *prev_chunk = NULL;
    size_t total_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - MEMORY_ZONE_SIZE) / g_malloc_data.sizes[TINY_ZONE].chunk;
    size_metadata size_metadata;

    size_metadata.in_use = 0;
    size_metadata.size = g_malloc_data.sizes[TINY_ZONE].chunk;
    
    for (size_t i=0; i < total_chunks; i++) {
        uint8_t *current_chunk_addr = (uint8_t *)first_chunk + (i * g_malloc_data.sizes[TINY_ZONE].chunk);
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

void *pool_strategy_allocate() {
    chunk_header *chunk = g_malloc_data.chunks_list[TINY_ZONE];
    if (chunk == NULL) {
        memory_zone *new_zone = zone_mgr_create(TINY_ZONE, g_malloc_data.sizes[TINY_ZONE].zone);
        if (new_zone == NULL)
            return NULL;

        pool_strategy_initialize_zone(new_zone);
        chunk = g_malloc_data.chunks_list[TINY_ZONE];
    }

    size_metadata metadata = malloc_read_size_metadata(chunk);
    metadata.in_use = 1;
    malloc_write_size_metadata(chunk, metadata);
    g_malloc_data.chunks_list[TINY_ZONE] = chunk->next_chunk;

    return ((char *)chunk) + SIZE_T_SIZE;
}

memory_zone *pool_strategy_get_chunk_memory_zone(chunk_header *chunk) {
    memory_zone *zone = g_malloc_data.zones_list[TINY_ZONE];
    if (zone == NULL)
        return NULL;

    while (zone != NULL) {
        memory_zone *zone_end = (memory_zone *)(((uint8_t *)zone) + g_malloc_data.sizes[TINY_ZONE].zone);
        if ((uint8_t *)zone < (uint8_t *)chunk && (uint8_t *)chunk < (uint8_t *)zone_end)
            return zone;

        zone = zone->next_zone;
    }

    return NULL;
}

void pool_strategy_free_zone(chunk_header *chunk) {
    memory_zone *zone = pool_strategy_get_chunk_memory_zone(chunk);
    memory_zone *zone_end = (memory_zone *)(((uint8_t *)zone) + g_malloc_data.sizes[TINY_ZONE].zone);

    chunk = (chunk_header *)(((uint8_t *)zone) + MEMORY_ZONE_SIZE);
    while ((uint8_t *)chunk < (uint8_t *)zone_end) {
        size_metadata metadata = malloc_read_size_metadata(chunk);
        if (metadata.in_use == 1)
            return;
        chunk = (chunk_header *)(((uint8_t *)chunk) + g_malloc_data.sizes[TINY_ZONE].chunk);
    }

    if (g_malloc_data.zones_list[TINY_ZONE] == zone
        && zone->next_zone == NULL)
    {
        return;
    }

    size_t total_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - MEMORY_ZONE_SIZE) / g_malloc_data.sizes[TINY_ZONE].chunk;
    size_t current_chunk = 0;
    chunk = (chunk_header *)(((uint8_t *)zone) + MEMORY_ZONE_SIZE);
    while ((current_chunk < total_chunks) && ((uint8_t *)chunk < (uint8_t *)zone_end)) {
        if (chunk == g_malloc_data.chunks_list[TINY_ZONE])
            g_malloc_data.chunks_list[TINY_ZONE] = chunk->next_chunk;
        else if (chunk->next_chunk == NULL) {
            chunk->prev_chunk->next_chunk = NULL;
        }
        else {
            chunk->prev_chunk->next_chunk = chunk->next_chunk;
            chunk->next_chunk->prev_chunk = chunk->prev_chunk;
        }
        
        chunk = (chunk_header *)(((uint8_t *)chunk) + g_malloc_data.sizes[TINY_ZONE].chunk);
        current_chunk++;
    }

    zone_mgr_delete(
        zone,
        TINY_ZONE,
        g_malloc_data.sizes[TINY_ZONE].zone
    );
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
    pool_strategy_free_zone(chunk);
}

void pool_strategy_print_zone_chunks(memory_zone *zone) {
    size_t n_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - MEMORY_ZONE_SIZE) / g_malloc_data.sizes[TINY_ZONE].chunk;
    chunk_header *chunk = (chunk_header *)((uint8_t*)zone + MEMORY_ZONE_SIZE);
    for (int i=0; i < n_chunks; i++) {
        size_metadata chunk_metadata = malloc_read_size_metadata(chunk);

        // start address
        malloc_print_address_hex(chunk);
        // -
        malloc_print(" - ");
        // end address
        chunk_header *chunk_end = (chunk_header *)((uint8_t*)chunk + chunk_metadata.size - 1);
        malloc_print_address_hex(chunk_end);
        // :
        malloc_print(" : ");
        // size
        malloc_print_size(chunk_metadata.size);
        // bytes
        malloc_print(" bytes\n");

        chunk = (chunk_header *)((uint8_t*)chunk + chunk_metadata.size);
    }
}

void pool_strategy_print_zone(memory_zone *zone) {
    if (g_malloc_data.zones_list[TINY_ZONE] == NULL)
        return;

    malloc_print("TINY : ");
    malloc_print_address_hex(zone);
    malloc_print("\n");

    for (; zone != NULL; zone = zone->next_zone) {
        pool_strategy_print_zone_chunks(zone);
    }
}