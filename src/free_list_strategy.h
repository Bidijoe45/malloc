#ifndef free_list_strategy
#define free_list_strategy

#include "malloc_types.h"

void *fls_allocate(size_t size);
void fls_free(chunk_header *chunk, size_metadata metadata);
void fls_initialize();
chunk_header *fls_initialize_zone(memory_zone *zone);
memory_zone *fls_get_chunk_memory_zone(chunk_header *chunk);
size_t *fls_get_end_size(chunk_header *chunk, size_t size);

#endif
