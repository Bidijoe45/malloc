#ifndef pool_strategy
#define pool_strategy

#include <stddef.h>

#include "malloc_types.h"
#include "malloc_utils.h"

void *pool_strategy_allocate();
void pool_strategy_free(chunk_header *chunk);
void pool_strategy_initialize();
void pool_strategy_initialize_zone(memory_zone *zone);

#endif