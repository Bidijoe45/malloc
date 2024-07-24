#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>

#include "malloc_state.h"
#include "malloc_utils.h"
#include "zone_manager.h"
#include "pool_strategy.h"
#include "free_list_strategy.h"

void initialize_malloc() {
    //FIXME: Borrar esto al final!
    printf("Malloc initialization\n");

    g_malloc_data.zones_list[TINY_ZONE] = NULL;
    g_malloc_data.zones_list[SMALL_ZONE] = NULL;
    g_malloc_data.zones_list[LARGE_ZONE] = NULL;

    pool_strategy_initialize();
    fls_initialize();

    //FIXME: Borrar esto al final!
    printf("page size: %d\n", getpagesize());
    printf("tiny zone size: %zu\n", g_malloc_data.sizes[TINY_ZONE].zone);
    printf("tiny zone chunk size: %zu\n", g_malloc_data.sizes[TINY_ZONE].chunk);
    printf("tiny zone payload size: %zu\n", g_malloc_data.sizes[TINY_ZONE].payload);

    printf("small zone size: %zu\n", g_malloc_data.sizes[SMALL_ZONE].zone);
    printf("small zone chunk size: %zu\n", g_malloc_data.sizes[SMALL_ZONE].chunk);
    printf("small zone payload size: %zu\n", g_malloc_data.sizes[SMALL_ZONE].payload);
    printf("\n");
}


size_t malloc_size_metadata_to_size_t(size_metadata metadata) {
    return (metadata.size & ~7) | (metadata.in_use & 1);
}

size_metadata malloc_size_t_to_metadata(size_t size) {
    size_metadata metadata;
    
    metadata.in_use = size & 1;
    metadata.size = size & ~7u;

    return metadata;
}

size_metadata malloc_read_size_metadata(chunk_header *chunk) {
    return malloc_size_t_to_metadata(chunk->size);
}

void malloc_write_size_metadata(chunk_header *chunk, size_metadata metadata) {
    chunk->size = malloc_size_metadata_to_size_t(metadata);
}

size_t *malloc_get_end_size(chunk_header *chunk, size_t size) {
    return (size_t *)(((char *)chunk) + size - sizeof(size_t));
}

void malloc_initialize_chunk(chunk_header *chunk) {
    chunk->prev_chunk = NULL;
    chunk->next_chunk = NULL;
    chunk->size = 0;
}

chunk_header *malloc_get_last_chunk(chunk_header *chunk) {
    if (chunk == NULL)
        return NULL;

    chunk_header *current_chunk = chunk;

    while (current_chunk->next_chunk != NULL) {
        current_chunk = current_chunk->next_chunk;
    }

    return current_chunk;
}

chunk_header *get_chunk_header(void *ptr) {
    return (chunk_header *)(ptr - sizeof(size_t));
}

void hexdump(void *mem, unsigned int len)
{
    unsigned int i, j, k;
    
    for(i = 0; i < len + ((len % 16) ? (16 - len % 16) : 0); i++) {
        if(i % 16 == 0) {
            printf("0x%06x: ", i);
            k = 0;
        }

        if (k == 4 || k == 12)
            printf(" ");

        if (k == 8)
            printf("   ");
        k++;

        if(i < len) {
            printf("%02x ", 0xFF & ((char*)mem)[i]);
        }
        else {
            printf("   ");
        }
        
        if(i % 16 == (16 - 1)) {
            for(j = i - (16 - 1); j <= i; j++) {
                if(j >= len) {
                    putchar(' ');
                }
                else if(isprint(((char*)mem)[j])) {
                    putchar(0xFF & ((char*)mem)[j]);        
                }
                else {
                    putchar('.');
                }
            }
            putchar('\n');
        }
    }
}

void visualize_memory(void)
{
    /*
    for (zone_header_t *tiny_zone = heap_g.tiny_zones_head; tiny_zone != NULL; tiny_zone = tiny_zone->next)
    {
        printf("| %p " GREEN "tiny zone header (%zu bytes): %p %p" NO_COLOR " |\n",
            tiny_zone,
            ZONE_HEADER_T_SIZE,
            tiny_zone->prev,
            tiny_zone->next);

        size_t *chunk_ptr = (size_t *)((uint8_t *)tiny_zone + ZONE_HEADER_T_SIZE);
        size_t chunk_size = CHUNK_SIZE_WITHOUT_FLAGS(*chunk_ptr);
        for (size_t i = 0; i < ((heap_g.tiny_zone_size - ZONE_HEADER_T_SIZE) / heap_g.tiny_zone_chunk_max_size); i++)
        {
            if ((*chunk_ptr & IN_USE) == IN_USE)
            {
                printf("| %p " BLUE "chunk header (%zu bytes): %zu %d%d" NO_COLOR " | " MAGENTA "in use body (%zu bytes)" NO_COLOR " |\n" ,
                    chunk_ptr, SIZE_T_SIZE, chunk_size, (uint8_t)(*chunk_ptr & PREVIOUS_FREE), (uint8_t)(*chunk_ptr & IN_USE),
                    chunk_size - SIZE_T_SIZE);
            }
            else
            {
                free_chunk_header_t *free_chunk_ptr = (free_chunk_header_t *)chunk_ptr;
                size_t *footer_size = (size_t *)((uint8_t *)chunk_ptr + chunk_size - SIZE_T_SIZE);
                printf("| %p " BLUE "chunk header (%zu bytes): %zu %d%d %p %p" NO_COLOR " | " YELLOW "free bytes (%zu bytes)" NO_COLOR " | " BLUE "size (%zu bytes): %zu" NO_COLOR " |\n" ,
                    chunk_ptr, sizeof(free_chunk_header_t), chunk_size, (uint8_t)(*chunk_ptr & PREVIOUS_FREE), (uint8_t)(*chunk_ptr & IN_USE), free_chunk_ptr->prev, free_chunk_ptr->next,
                    chunk_size - sizeof(free_chunk_header_t) - SIZE_T_SIZE, SIZE_T_SIZE, *footer_size);
            }
            chunk_ptr = (size_t *)((uint8_t *)(chunk_ptr) + chunk_size);
        }
    }
    */

    #define GREEN "\033[0;32m"
    #define BLUE "\033[0;34m"
    #define MAGENTA "\033[0;35m"
    #define YELLOW "\033[0;33m"
    #define NO_COLOR "\033[0m"

    memory_zone *small_zone = g_malloc_data.zones_list[SMALL_ZONE];
    for (; small_zone != NULL; small_zone = small_zone->next_zone)
    {
        printf("| %p " GREEN "small zone header (%zu bytes): %p" NO_COLOR " |\n",
            small_zone,
            sizeof(memory_zone),
            small_zone->next_zone);

        size_t *chunk_ptr = (size_t *)((uint8_t *)small_zone + sizeof(memory_zone));
        size_t chunk_size;
        for (size_t i = 0; i < (g_malloc_data.sizes[SMALL_ZONE].zone - sizeof(memory_zone)); i += chunk_size)
        {
            size_metadata chunk_metadata = malloc_read_size_metadata((chunk_header *)chunk_ptr);
            chunk_size = chunk_metadata.size;
            uint8_t in_use_bit_flag = chunk_metadata.in_use;
            if (chunk_metadata.in_use)
            {
                size_t in_use_bytes = chunk_size - sizeof(size_t);
                printf("| %p " BLUE "chunk header (%zu bytes): %zu %d" NO_COLOR " | " MAGENTA "in use body (%zu bytes)" NO_COLOR " |\n" ,
                    chunk_ptr, sizeof(size_t), chunk_size, in_use_bit_flag, // chunk header
                    in_use_bytes); // chunk data
            }
            else
            {
                chunk_header *free_chunk_ptr = (chunk_header *)chunk_ptr;
                size_t *footer_size = (size_t *)((uint8_t *)chunk_ptr + chunk_size - sizeof(size_t));
                size_t free_bytes = chunk_size - sizeof(chunk_header) - sizeof(size_t);
                printf("| %p " BLUE "chunk header (%zu bytes): %zu %d %p %p" NO_COLOR " | " YELLOW "free bytes (%zu bytes)" NO_COLOR " | " BLUE "size (%zu bytes): %zu" NO_COLOR " |\n" ,
                    chunk_ptr, sizeof(chunk_header), chunk_size, in_use_bit_flag, free_chunk_ptr->prev_chunk, free_chunk_ptr->next_chunk, // chunk header
                    free_bytes, // free space
                    sizeof(size_t), *footer_size); // footer size
            }
            chunk_ptr = (size_t *)((uint8_t *)(chunk_ptr) + chunk_size);
        }
    }  

    /*
    for (zone_header_t *large_zone = heap_g.large_zones_head; large_zone != NULL; large_zone = large_zone->next)
    {
        printf("| %p " GREEN "large zone header (%zu bytes): %p %p" NO_COLOR " |\n",
            large_zone,
            ZONE_HEADER_T_SIZE,
            large_zone->prev,
            large_zone->next);
        size_t *chunk_ptr = (size_t *)((uint8_t *)large_zone + ZONE_HEADER_T_SIZE);
        size_t chunk_size = CHUNK_SIZE_WITHOUT_FLAGS(*chunk_ptr);
        uint8_t in_use_bit_flag = (uint8_t)(*chunk_ptr & IN_USE);
        uint8_t prev_free_bit_flag = (uint8_t)(*chunk_ptr & PREVIOUS_FREE);
        if ((*chunk_ptr & IN_USE) == IN_USE)
        {
            size_t in_use_bytes = chunk_size - SIZE_T_SIZE;
            printf("| %p " BLUE "chunk header (%zu bytes): %zu %d%d" NO_COLOR " | " MAGENTA "in use body (%zu bytes)" NO_COLOR " |\n" ,
                chunk_ptr, SIZE_T_SIZE, chunk_size, prev_free_bit_flag, in_use_bit_flag, // chunk header
                in_use_bytes); // chunk data
        }
        else // should never go in here
        {
            free_chunk_header_t *free_chunk_ptr = (free_chunk_header_t *)chunk_ptr;
            size_t free_bytes = chunk_size - sizeof(free_chunk_header_t) - SIZE_T_SIZE;
            printf("| %p " BLUE "chunk header (%zu bytes): %zu %d%d %p %p" NO_COLOR " | " YELLOW "free bytes (%zu bytes)" NO_COLOR " |\n" ,
                chunk_ptr, sizeof(free_chunk_header_t), chunk_size, prev_free_bit_flag, in_use_bit_flag, free_chunk_ptr->prev, free_chunk_ptr->next, // chunk header
                free_bytes); // free space
        }
    }
    */
}

void print_chunk_list(chunk_header *chunk) {
    printf("--- Chunk list ---\n");
    while (chunk != NULL) {
        printf("%p\n", chunk);
        chunk = chunk->next_chunk;
    }
    printf("--- --- --- --- ---\n");
}

void print_zone_list(memory_zone *zone) {
    printf("--- zone list ---\n");
    while (zone != NULL) {
        printf("%p\n", zone);
        zone = zone->next_zone;
    }
    printf("--- --- --- --- ---\n");
}