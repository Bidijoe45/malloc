#ifndef zone_manager
#define zone_manager

#include "malloc_types.h"

memory_zone *create_zone(size_t size);
void delete_zone(memory_zone *zone);
void initialize_tiny_zone(memory_zone *zone);

#endif