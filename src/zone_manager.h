#ifndef zone_manager
#define zone_manager

#include "malloc_types.h"

memory_zone *zone_mgr_create_by_type(zone_type type);
memory_zone *zone_mgr_create(zone_type type, size_t zone_size);
void zone_mgr_delete_by_type(memory_zone *zone, zone_type type);
void zone_mgr_delete(memory_zone *zone, zone_type type, size_t size);
size_t zone_mgr_get_type_size(zone_type type);

#endif