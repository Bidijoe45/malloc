#include "large_strategy.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_utils.h"

void lgs_initialize() {
    g_malloc_data.zones_list[LARGE_ZONE] = NULL;
    g_malloc_data.chunks_list[LARGE_ZONE] = NULL;
}

void *lgs_allocate(size_t size) {
    size = ALIGN(size + sizeof(memory_zone));
    memory_zone *new_zone = zone_mgr_create(LARGE_ZONE, size);

    if (new_zone == NULL)
        return NULL;

    chunk_header *chunk = (chunk_header *)(((char *)new_zone) + sizeof(memory_zone));
    size_metadata metadata = {.size=size, .in_use=1};
    
    malloc_write_size_metadata(chunk, metadata);

    return ((char *)chunk) + sizeof(size_t);
}

void lgs_free(chunk_header *chunk, size_metadata metadata) {
    memory_zone *zone = (memory_zone *)(((char *)chunk) - sizeof(memory_zone));

    zone_mgr_delete(zone, LARGE_ZONE, metadata.size);
}
