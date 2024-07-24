#ifndef large_strategy
#define large_strategy

#include "malloc_types.h"

void *lgs_allocate(size_t size);
void lgs_free(chunk_header *chunk, size_metadata metadata);
void lgs_initialize();

#endif