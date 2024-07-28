#include "large_strategy.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_utils.h"

void lgs_initialize() {
    g_malloc_data.zones_list[LARGE_ZONE] = NULL;
    g_malloc_data.chunks_list[LARGE_ZONE] = NULL;
}

void lgs_add_chunk_to_list(chunk_header *chunk) {
    if (g_malloc_data.chunks_list[LARGE_ZONE] != NULL) {
        g_malloc_data.chunks_list[LARGE_ZONE]->prev_chunk = chunk;
        chunk->next_chunk = g_malloc_data.chunks_list[LARGE_ZONE];
        g_malloc_data.chunks_list[LARGE_ZONE] = chunk;
    }
    else
        g_malloc_data.chunks_list[LARGE_ZONE] = chunk;
}

void lgs_remove_chunk_from_list(chunk_header *chunk) {
    chunk_header *prev_chunk = chunk->prev_chunk;
    chunk_header *next_chunk = chunk->next_chunk;

    if (chunk == g_malloc_data.chunks_list[LARGE_ZONE]) {
        g_malloc_data.chunks_list[LARGE_ZONE] = chunk->next_chunk;
    }

    if (prev_chunk != NULL) {
        prev_chunk->next_chunk = next_chunk;
    }

    if (next_chunk != NULL) {
        next_chunk->prev_chunk = prev_chunk;
    }
}

void *lgs_allocate(size_t size) {
    memory_zone *new_zone = zone_mgr_create(LARGE_ZONE, size);
    if (new_zone == NULL)
        return NULL;

    chunk_header *chunk = (chunk_header *)(((char *)new_zone) + sizeof(memory_zone));
    size_metadata metadata = {.size=size, .in_use=1};
    
    malloc_write_size_metadata(chunk, metadata);
    lgs_add_chunk_to_list(chunk);

    return ((char *)chunk) + sizeof(size_t);
}

void lgs_free(chunk_header *chunk, size_metadata metadata) {
    memory_zone *zone = (memory_zone *)(((char *)chunk) - sizeof(memory_zone));

    lgs_remove_chunk_from_list(chunk);
    zone_mgr_delete(zone, LARGE_ZONE, metadata.size);
}
