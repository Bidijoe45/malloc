#ifndef malloc_utils
#define malloc_utils

#include "malloc_types.h"

void initialize_malloc();
void initialize_tiny_zone(memory_zone *zone);

size_metadata malloc_read_size_metadata(chunk_header *chunk);
void malloc_write_size_metadata(chunk_header *chunk, size_metadata metadata);
void malloc_initialize_chunk(chunk_header *chunk);
chunk_header *malloc_get_last_chunk(chunk_header *chunk);
chunk_header *get_chunk_header(void *ptr);

void hexdump(void *mem, unsigned int len);

#endif
