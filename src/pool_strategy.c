#include "pool_strategy.h"
#include "malloc_types.h"

void create_pool_strategy(memory_zone *zone, size_t chunk_size, size_t total_chunks) {
    chunk_header *first_chunk = (chunk_header *)(zone + 1);
    chunk_header *prev_chunk = NULL;
    
    for (size_t i=0; i < total_chunks; i++) {
        char *current_chunk_addr = (char *)first_chunk + (i * chunk_size);
        chunk_header *current_chunk = (chunk_header *)current_chunk_addr;

        if (prev_chunk) {
            current_chunk->prev_chunk = prev_chunk;
            prev_chunk->next_chunk = current_chunk;
        }
        
        current_chunk->next_chunk = NULL;
        current_chunk->size = chunk_size;

        prev_chunk = current_chunk;
    }
}