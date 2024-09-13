#include <stdint.h>

#include "large_strategy.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_utils.h"

void lgs_initialize() {
    g_malloc_data.zones_list[LARGE_ZONE] = NULL;
    g_malloc_data.chunks_list[LARGE_ZONE] = NULL;
}

void *lgs_allocate(size_t size) {
    size_t zone_size = size + MEMORY_ZONE_SIZE + SIZE_T_SIZE;
    size_t chunk_size = size + SIZE_T_SIZE;
    memory_zone *new_zone = zone_mgr_create(LARGE_ZONE, zone_size);

    if (new_zone == NULL)
        return NULL;

    chunk_header *chunk = (chunk_header *)(((uint8_t *)new_zone) + MEMORY_ZONE_SIZE);
    size_metadata metadata = {.size=chunk_size, .in_use=1};
    
    malloc_write_size_metadata(chunk, metadata);

    return ((uint8_t *)chunk) + SIZE_T_SIZE;
}

void lgs_free(chunk_header *chunk, size_metadata metadata) {
    memory_zone *zone = (memory_zone *)(((uint8_t *)chunk) - MEMORY_ZONE_SIZE);
    zone_mgr_delete(zone, LARGE_ZONE, metadata.size + MEMORY_ZONE_SIZE);
}

void lgs_print_zones(memory_zone *zone) {
    if (g_malloc_data.zones_list[LARGE_ZONE] == NULL)
        return;

    malloc_print("LARGE : ");
    malloc_print_address_hex(zone);
    malloc_print("\n");

    for (; zone != NULL; zone = zone->next_zone) {
        chunk_header *chunk = (chunk_header *)((uint8_t*)zone + MEMORY_ZONE_SIZE);
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
    }

}