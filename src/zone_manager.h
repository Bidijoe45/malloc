#ifndef zone_manager
#define zone_manager

#include "malloc_types.h"

memory_zone *create_zone(zone_type type);
void delete_zone(memory_zone *zone, zone_type type);
size_t get_zone_type_size(zone_type type);
size_t count_zones_by_size(zone_type type);

#endif