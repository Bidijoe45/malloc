#ifndef pool_strategy
#define pool_strategy

#include <stddef.h>

#include "malloc_utils.h"

void create_pool_strategy(memory_zone *zone, size_t chunk_size, size_t total_chunks);

#endif