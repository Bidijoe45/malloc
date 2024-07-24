#include <unistd.h>
#include <stdlib.h>

#include "free_list_strategy.h"
#include "malloc_state.h"
#include "malloc_types.h"
#include "zone_manager.h"
#include "malloc_utils.h"


#include <stdio.h>

void fls_initialize() {
    g_malloc_data.sizes[SMALL_ZONE].zone = getpagesize() * 4;
    g_malloc_data.sizes[SMALL_ZONE].chunk = g_malloc_data.sizes[SMALL_ZONE].zone - sizeof(memory_zone);
    g_malloc_data.sizes[SMALL_ZONE].payload = g_malloc_data.sizes[SMALL_ZONE].chunk - (sizeof(size_t) * 2);
    g_malloc_data.chunks_list[SMALL_ZONE] = NULL;
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
    size_t *end_size = malloc_get_end_size(chunk, metadata.size);

    chunk->next_chunk = NULL;
    chunk->prev_chunk = NULL;
    malloc_write_size_metadata(chunk, metadata);
    *end_size = malloc_size_metadata_to_size_t(metadata);

    return chunk;
}

chunk_header *fls_initialize_zone(memory_zone *zone) {
    //FIXME: Debug only
    printf("fls new zone\n");
    chunk_header *chunk = (chunk_header *)(((char *)zone) + sizeof(memory_zone));
    size_metadata metadata;

    metadata.in_use = 0;
    metadata.size = g_malloc_data.sizes[SMALL_ZONE].chunk;

    chunk = fls_create_chunk(chunk, metadata);
    return chunk;
}

chunk_header *fls_find_best_fit_chunk(size_t size) {
    chunk_header *best_fit_chunk = NULL;
    size_t best_fit_size_delta = -1;
    chunk_header *current_chunk = g_malloc_data.chunks_list[SMALL_ZONE];

    while (current_chunk != NULL) {
        size_metadata current_metadata = malloc_read_size_metadata(current_chunk);
        size_t size_delta = llabs((ssize_t)current_metadata.size - (ssize_t)ALIGN(size));

        if (current_metadata.size < ALIGN(size)
            || best_fit_size_delta < size_delta) {
            current_chunk = current_chunk->next_chunk;
            continue;
        }

        best_fit_size_delta = current_metadata.size;
        best_fit_chunk = current_chunk;
        current_chunk = current_chunk->next_chunk;
    }    

    return best_fit_chunk;
}

void *fls_allocate(size_t size) {
    chunk_header *free_chunk = fls_find_best_fit_chunk(size);

    if (free_chunk == NULL) {
        memory_zone *new_zone = create_zone(SMALL_ZONE);
        free_chunk = fls_initialize_zone(new_zone);
    }

    fls_remove_chunk_from_list(free_chunk);

    size_metadata free_chunk_metadata = malloc_read_size_metadata(free_chunk);
    size_t allocation_chunk_size = ALIGN(size + sizeof(size_t) * 2);
    size_t remaining_size = free_chunk_metadata.size - allocation_chunk_size;

    if (remaining_size <= g_malloc_data.sizes[TINY_ZONE].chunk) {
        allocation_chunk_size = free_chunk_metadata.size;
        remaining_size = 0;
    }

    //FIXME: Debug only
    malloc_allocated_memory += allocation_chunk_size;

    size_metadata allocated_chunk_metadata = {.size= allocation_chunk_size, .in_use= 1};
    chunk_header *allocated_chunk = fls_create_chunk(free_chunk, allocated_chunk_metadata);
    size_t *allocated_chunk_end_size = malloc_get_end_size(allocated_chunk, allocated_chunk_metadata.size);

    if (remaining_size != 0) {
        size_metadata remaining_chunk_metadata = {.size= remaining_size, .in_use= 0};
        chunk_header *remaining_chunk = fls_create_chunk((chunk_header *)(allocated_chunk_end_size + 1), remaining_chunk_metadata);
        fls_add_chunk_to_list(remaining_chunk);
    }

    return ((char *)allocated_chunk) + sizeof(size_t);
}

memory_zone *fls_get_chunk_memory_zone(chunk_header *chunk) {
    memory_zone *zone = g_malloc_data.zones_list[SMALL_ZONE];

    if (zone == NULL)
        return NULL;

    while (zone != NULL) {
        memory_zone *zone_end = (memory_zone *)(((char *)zone) + g_malloc_data.sizes[SMALL_ZONE].zone);
        if ((char *)zone < (char *)chunk && (char *)chunk < (char *)zone_end)
            return zone;
        zone = zone->next_zone;
    }

    return NULL;
}

chunk_header *fls_get_prev_chunk_by_size(chunk_header *chunk) {
    memory_zone *zone_start = fls_get_chunk_memory_zone(chunk);
    size_t *prev_end_size = (size_t *)( ((char *)chunk) - sizeof(size_t) );
    size_metadata metadata = malloc_size_t_to_metadata(*prev_end_size);
    chunk_header *prev_chunk = (chunk_header *)(((char *)chunk) - metadata.size);

    if (metadata.size == 0)
        return NULL;

    if ((char *)prev_chunk < (((char *)zone_start) + sizeof(memory_zone)))
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
        delete_zone(fls_get_chunk_memory_zone(left_most_chunk), SMALL_ZONE);
    }
}

void fls_free(chunk_header *chunk, size_metadata metadata) {
    //FIXME: Debug only
    malloc_freed_memory += metadata.size;

    chunk->prev_chunk = NULL;
    chunk->next_chunk = NULL;
    metadata.in_use = 0;

    malloc_write_size_metadata(chunk, metadata);
    fls_add_chunk_to_list(chunk);
    fls_merge_free_chunks(chunk);
}
