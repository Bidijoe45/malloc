#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "free_list_strategy.h" 
#include "malloc_state.h"
#include "malloc_types.h"
#include "zone_manager.h"
#include "malloc_utils.h"

void fls_initialize() {
    g_malloc_data.zones_list[SMALL_ZONE] = NULL;
    g_malloc_data.sizes[SMALL_ZONE].zone = getpagesize() * 800;
    g_malloc_data.sizes[SMALL_ZONE].chunk = g_malloc_data.sizes[SMALL_ZONE].zone / 128;
    g_malloc_data.sizes[SMALL_ZONE].payload = g_malloc_data.sizes[SMALL_ZONE].chunk - SIZE_T_SIZE * 2;
    //g_malloc_data.zones_list[SMALL_ZONE] = NULL;
    //g_malloc_data.sizes[SMALL_ZONE].zone = getpagesize();
    //g_malloc_data.sizes[SMALL_ZONE].chunk = g_malloc_data.sizes[SMALL_ZONE].zone / 128;
    //g_malloc_data.sizes[SMALL_ZONE].payload = g_malloc_data.sizes[SMALL_ZONE].chunk - SIZE_T_SIZE * 2;
    //g_malloc_data.chunks_list[SMALL_ZONE] = NULL;
}

size_t *fls_get_end_size(chunk_header *chunk, size_t size) {
    return (size_t *)(((uint8_t *)chunk) + size - SIZE_T_SIZE);
}

void fls_add_chunk_to_list(chunk_header *chunk) {
    if (g_malloc_data.chunks_list[SMALL_ZONE] != NULL) {
        g_malloc_data.chunks_list[SMALL_ZONE]->prev_chunk = chunk;
        chunk->next_chunk = g_malloc_data.chunks_list[SMALL_ZONE];
        g_malloc_data.chunks_list[SMALL_ZONE] = chunk;
    }
    else
        g_malloc_data.chunks_list[SMALL_ZONE] = chunk;
}

void fls_remove_chunk_from_list(chunk_header *chunk) {
    chunk_header *prev_chunk = chunk->prev_chunk;
    chunk_header *next_chunk = chunk->next_chunk;

    if (chunk == g_malloc_data.chunks_list[SMALL_ZONE]) {
        g_malloc_data.chunks_list[SMALL_ZONE] = chunk->next_chunk;
    }

    if (prev_chunk != NULL) {
        prev_chunk->next_chunk = next_chunk;
    }

    if (next_chunk != NULL) {
        next_chunk->prev_chunk = prev_chunk;
    }
}

chunk_header *fls_create_chunk(chunk_header *chunk, size_metadata metadata) {
    size_t *end_size = fls_get_end_size(chunk, metadata.size);

    chunk->next_chunk = NULL;
    chunk->prev_chunk = NULL;
    malloc_write_size_metadata(chunk, metadata);
    *end_size = malloc_size_metadata_to_size_t(metadata);

    return chunk;
}

chunk_header *fls_initialize_zone(memory_zone *zone) {
    chunk_header *chunk = (chunk_header *)(((uint8_t *)zone) + MEMORY_ZONE_SIZE);
    size_metadata metadata;

    metadata.in_use = 0;
    metadata.size = g_malloc_data.sizes[SMALL_ZONE].zone - MEMORY_ZONE_SIZE;
    
    chunk = fls_create_chunk(chunk, metadata);
    return chunk;
}

chunk_header *fls_find_best_fit_chunk(size_t size) {
    chunk_header *best_fit_chunk = NULL;
    size_t best_fit_size_delta = SIZE_MAX;
    chunk_header *current_chunk = g_malloc_data.chunks_list[SMALL_ZONE];

    while (current_chunk != NULL) {
        size_metadata current_metadata = malloc_read_size_metadata(current_chunk);
        size_t size_delta = llabs((ssize_t)current_metadata.size - (ssize_t)size);

        if (current_metadata.size >= size && size_delta < best_fit_size_delta) {
            best_fit_size_delta = size_delta;
            best_fit_chunk = current_chunk;
        }

        current_chunk = current_chunk->next_chunk;
    }    

    return best_fit_chunk;
}

void *fls_allocate(size_t size) {
    chunk_header *free_chunk = fls_find_best_fit_chunk(size);

    if (free_chunk == NULL) {
        memory_zone *new_zone = zone_mgr_create(SMALL_ZONE, g_malloc_data.sizes[SMALL_ZONE].zone);
        if (new_zone == NULL)
            return NULL;
    
        free_chunk = fls_initialize_zone(new_zone);
    }

    fls_remove_chunk_from_list(free_chunk);


    size_t chunk_size = size + SIZE_T_SIZE * 2;
    size_metadata free_chunk_metadata = malloc_read_size_metadata(free_chunk);
    size_t remaining_size = free_chunk_metadata.size - chunk_size;

    if (remaining_size <= g_malloc_data.sizes[TINY_ZONE].chunk) {
        chunk_size = free_chunk_metadata.size;
        remaining_size = 0;
    }
    
    size_metadata allocated_chunk_metadata = {.size= chunk_size, .in_use= 1};
    chunk_header *allocated_chunk = fls_create_chunk(free_chunk, allocated_chunk_metadata);
    size_t *allocated_chunk_end = fls_get_end_size( 
        allocated_chunk,
        allocated_chunk_metadata.size
        );

    if (remaining_size != 0) {
        size_metadata remaining_chunk_metadata = {.size= remaining_size, .in_use= 0};
        chunk_header *remaining_chunk = fls_create_chunk(
            (chunk_header *)( ((uint8_t *)allocated_chunk_end) + SIZE_T_SIZE), 
            remaining_chunk_metadata
            );
        fls_add_chunk_to_list(remaining_chunk);
    }

    return ((uint8_t *)allocated_chunk) + SIZE_T_SIZE;
}

memory_zone *fls_get_chunk_memory_zone(chunk_header *chunk) {
    memory_zone *zone = g_malloc_data.zones_list[SMALL_ZONE];

    if (zone == NULL)
        return NULL;

    while (zone != NULL) {
        memory_zone *zone_end = (memory_zone *)(((uint8_t *)zone) + g_malloc_data.sizes[SMALL_ZONE].zone);
        if ((uint8_t *)zone < (uint8_t *)chunk && (uint8_t *)chunk < (uint8_t *)zone_end)
            return zone;

        zone = zone->next_zone;
    }

    return NULL;
}

chunk_header *fls_get_prev_chunk_by_size(chunk_header *chunk) {
    memory_zone *zone_start = fls_get_chunk_memory_zone(chunk);
    size_t *prev_end_size = (size_t *)( ((char *)chunk) - SIZE_T_SIZE );
    size_metadata metadata = malloc_size_t_to_metadata(*prev_end_size);
    chunk_header *prev_chunk = (chunk_header *)(((char *)chunk) - metadata.size);

    if (metadata.size == 0)
        return NULL;

    if ((char *)prev_chunk < (((char *)zone_start) + MEMORY_ZONE_SIZE))
        return NULL;

    return prev_chunk;
}

chunk_header *fls_get_next_chunk_by_size(chunk_header *chunk) {
    memory_zone *zone_start = fls_get_chunk_memory_zone(chunk);
    memory_zone *zone_end = (memory_zone *)(((char *)zone_start) + g_malloc_data.sizes[SMALL_ZONE].zone);
    size_metadata metadata = malloc_read_size_metadata(chunk);
    chunk_header *next_chunk = (chunk_header *)(((char *)chunk) + metadata.size);

    if ((char *)next_chunk >= (char *)zone_end)
        return NULL;

    return next_chunk;
}

void fls_merge_free_chunks(chunk_header *chunk) {
    chunk_header *left_most_chunk = chunk;
    chunk_header *right_most_chunk = chunk;
    size_metadata chunk_metadata = malloc_read_size_metadata(chunk);
    size_t size_to_merge = chunk_metadata.size;

    while (left_most_chunk != NULL) {
        chunk_header *prev_chunk = fls_get_prev_chunk_by_size(left_most_chunk);

        if (prev_chunk == NULL)
            break;

        size_metadata metadata = malloc_read_size_metadata(prev_chunk);

        if (metadata.in_use == 1)
            break;
        
        size_to_merge += metadata.size;
        left_most_chunk = prev_chunk;
        fls_remove_chunk_from_list(left_most_chunk);
    }

    while (right_most_chunk != NULL) {
        chunk_header *next_chunk = fls_get_next_chunk_by_size(right_most_chunk);

        if (next_chunk == NULL)
            break;

        size_metadata metadata = malloc_read_size_metadata(next_chunk);

        if (metadata.in_use == 1)
            break;

        size_to_merge += metadata.size;
        right_most_chunk = next_chunk;
        fls_remove_chunk_from_list(right_most_chunk);
    }

    if (left_most_chunk == right_most_chunk)
        return;
    
    fls_remove_chunk_from_list(chunk);
    size_metadata new_chunk_metadata = {.size = size_to_merge, .in_use= 0};
    fls_create_chunk(left_most_chunk, new_chunk_metadata);
    fls_add_chunk_to_list(left_most_chunk);

    if (new_chunk_metadata.size == g_malloc_data.sizes[SMALL_ZONE].chunk
        && g_malloc_data.chunks_list[SMALL_ZONE]->next_chunk != NULL)
    {
        fls_remove_chunk_from_list(left_most_chunk);
        zone_mgr_delete(
            fls_get_chunk_memory_zone(left_most_chunk),
            SMALL_ZONE,
            g_malloc_data.sizes[SMALL_ZONE].zone
        );
    }
}

void fls_free(chunk_header *chunk, size_metadata metadata) {

    chunk->prev_chunk = NULL;
    chunk->next_chunk = NULL;
    metadata.in_use = 0;

    malloc_write_size_metadata(chunk, metadata);
    fls_add_chunk_to_list(chunk);
    fls_merge_free_chunks(chunk);
}
