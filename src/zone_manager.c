#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

#include "malloc_state.h"
#include "malloc_types.h"
#include "zone_manager.h"
#include "pool_strategy.h"

memory_zone *create_zone(zone_type type) {
    size_t zone_size = get_zone_type_size(type);
    memory_zone *new_zone = (memory_zone *)mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    
    if (new_zone == NULL) {
        printf("ERROR: mmap failed\n");
        exit(1);
        //FIXME: What should happen if mmap fails?
    }
    
    new_zone->next_zone = NULL;

    if (g_malloc_data.zones_list[type] != NULL) {
        new_zone->next_zone = g_malloc_data.zones_list[type];
        g_malloc_data.zones_list[type] = new_zone;
    }
    else 
        g_malloc_data.zones_list[type] = new_zone;

    return new_zone;
}

void delete_zone(memory_zone *zone, zone_type type) {
    size_t zone_size = get_zone_type_size(type);
    memory_zone *last_zone = NULL;
    memory_zone *current_zone = g_malloc_data.zones_list[type];
    
    while (current_zone != NULL) {

        if (current_zone == zone)
            break;

        last_zone = current_zone;
        current_zone = current_zone->next_zone;
    }

    if (current_zone == NULL)
        return;

    if (current_zone == g_malloc_data.zones_list[type]) {
        g_malloc_data.zones_list[type] = current_zone->next_zone;
        return;
    }

    if (last_zone != NULL)
        last_zone->next_zone = current_zone->next_zone;

    munmap(zone, zone_size);
}

size_t count_zones_by_size(zone_type type) {
    size_t n_zones = 0;
    memory_zone *current_zone = g_malloc_data.zones_list[type];

    while (current_zone != NULL) {
        n_zones++;
        current_zone = current_zone->next_zone;
    }

    return n_zones;
}

size_t get_zone_type_size(zone_type type) {
    return g_malloc_data.sizes[type].zone;
}
