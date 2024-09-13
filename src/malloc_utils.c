#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

#include "malloc_state.h"
#include "malloc_utils.h"
#include "zone_manager.h"
#include "pool_strategy.h"
#include "free_list_strategy.h"
#include "large_strategy.h"

void initialize_malloc() {
    //FIXME: Borrar esto al final!
    //printf("Malloc initialization\n");
    malloc_print("Malloc intialization\n");

    pool_strategy_initialize();
    fls_initialize();
    lgs_initialize();

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
    return (chunk_header *)(ptr - SIZE_T_SIZE);
}

/**
 * Shitty functions bc linux printf uses malloc inside 
 */
void malloc_print_hex_number(size_t n)
{
    if (n >= 16)
        malloc_print_hex_number(n / 16);
    
    char c = n % 16;
    c += c > 9 ? ('a' - 10) : '0';
    write(1, &c, 1);
}

void malloc_print_address_hex(void *ptr)
{
    uintptr_t p = (uintptr_t)ptr;
    write(1, "0x", 2);
    malloc_print_hex_number(p);
}

size_t malloc_strlen(char *str) {

    if (str == NULL)
        return 0;

    size_t i = 0;
    while (str[i] != 0)
        i++;
    return i;
}

void malloc_print(char *str) {
    if (str == NULL)
        return;

    size_t str_size = malloc_strlen(str);
    write(1, str, str_size);
}

void malloc_print_size(size_t n)
{
	if (n / 10 > 0)
		malloc_print_size(n / 10);

	char c = (n % 10) + '0';
	write(1, &c, 1);
}

/*
void malloc_print_zone(zone_type zone_type, memory_zone *zone, size_t zone_size) {
    malloc_print_zone_type(zone_type);
    malloc_print(" : ");
    malloc_print_address_hex(zone);
    malloc_print("\n");

    memory_zone *zone_end = (memory_zone *)((uint8_t*)zone + zone_size);
    chunk_header *chunk = (chunk_header *)((uint8_t*)zone + MEMORY_ZONE_SIZE);

    size_t n_chunks
    for (int i=0; i < ) {
        size_metadata chunk_metadata = malloc_read_size_metadata(chunk);

        // start address
        malloc_print_address_hex(chunk);
        // -
        malloc_print(" - ");
        // end address
        chunk_header *chunk_end = (chunk_header *)((uint8_t*)chunk + chunk_metadata.size - 1);
        malloc_print_address_hex(chunk_end);
        // :
        malloc_print(" : ");
        // size
        malloc_print_size(chunk_metadata.size);
        // bytes
        malloc_print(" bytes\n");

        chunk = (chunk_header *)((uint8_t*)chunk + chunk_metadata.size);
    }

}
*/

//TODO: remove
void visualize_memory(void)
{

    for (memory_zone *tiny_zone = g_malloc_data.zones_list[TINY_ZONE]; tiny_zone != NULL; tiny_zone = tiny_zone->next_zone)
    {

        malloc_print_address_hex(tiny_zone);
        malloc_print(" tiny zone header (");
        malloc_print_size(MEMORY_ZONE_SIZE);
        malloc_print(" bytes): ");
        malloc_print_address_hex(tiny_zone->next_zone);
        malloc_print("\n");

        size_t *chunk_ptr = (size_t *)((uint8_t *)tiny_zone + MEMORY_ZONE_SIZE);
        size_t chunk_size;
        for (size_t i = 0; i < ((g_malloc_data.sizes[TINY_ZONE].zone - MEMORY_ZONE_SIZE) / g_malloc_data.sizes[TINY_ZONE].chunk); i++)
        {
            chunk_header *chunk = (chunk_header *)chunk_ptr;
            size_metadata chunk_metadata = malloc_read_size_metadata((chunk_header *)chunk_ptr);
            chunk_size = chunk_metadata.size;
            uint8_t in_use_bit_flag = chunk_metadata.in_use;
            if (chunk_metadata.in_use)
            {
                malloc_print_address_hex(chunk_ptr);
                malloc_print(" tny chunk_header (");
                malloc_print_size(SIZE_T_SIZE);
                malloc_print(" bytes): ");
                malloc_print_size(chunk_size);
                malloc_print(" ");
                malloc_print_size(chunk_metadata.in_use);
                malloc_print(" in use body (");
                malloc_print_size(chunk_size - SIZE_T_SIZE);
                malloc_print(" bytes)\n");
            }
            else
            {
                malloc_print_address_hex(chunk_ptr);
                malloc_print(" tny chunk_header (");
                malloc_print_size(sizeof(chunk_header));
                malloc_print(" bytes): ");
                malloc_print_size(chunk_size);
                malloc_print(" ");
                malloc_print_size(chunk_metadata.in_use);
                malloc_print(" ");
                malloc_print_address_hex(chunk->prev_chunk);
                malloc_print(" ");
                malloc_print_address_hex(chunk->next_chunk);
                malloc_print(" ");
                malloc_print(" free bytes (");
                malloc_print_size(chunk_size - SIZE_T_SIZE);
                malloc_print(" bytes)");
                malloc_print("\n");
            }
            chunk_ptr = (size_t *)((uint8_t *)(chunk_ptr) + chunk_size);
        }
    }


    memory_zone *small_zone = g_malloc_data.zones_list[SMALL_ZONE];
    for (; small_zone != NULL; small_zone = small_zone->next_zone)
    {
        malloc_print_address_hex(small_zone);
        malloc_print(" small zone header (");
        malloc_print_size(MEMORY_ZONE_SIZE);
        malloc_print(" bytes): ");
        malloc_print_address_hex(small_zone->next_zone);
        malloc_print("\n");

        size_t *chunk_ptr = (size_t *)(((uint8_t *)small_zone) + MEMORY_ZONE_SIZE);
        size_t chunk_size;
        for (size_t i = 0; i < (g_malloc_data.sizes[SMALL_ZONE].zone - MEMORY_ZONE_SIZE); i += chunk_size)
        {
            size_metadata chunk_metadata = malloc_read_size_metadata((chunk_header *)chunk_ptr);
            chunk_size = chunk_metadata.size;
            uint8_t in_use_bit_flag = chunk_metadata.in_use;
            if (chunk_metadata.in_use)
            {
                size_t in_use_bytes = chunk_size - SIZE_T_SIZE;

                malloc_print_address_hex(chunk_ptr);
                malloc_print(" sm chunk_header (");
                malloc_print_size(SIZE_T_SIZE);
                malloc_print(" bytes): ");
                malloc_print_size(chunk_size);
                malloc_print(" ");
                malloc_print_size((size_t)in_use_bit_flag);
                malloc_print(" in use body (");
                malloc_print_size(in_use_bytes);
                malloc_print(")\n");

            }
            else
            {
                chunk_header *free_chunk_ptr = (chunk_header *)chunk_ptr;
                size_t *footer_size = (size_t *)((uint8_t *)chunk_ptr + chunk_size - SIZE_T_SIZE);
                size_t free_bytes = chunk_size - sizeof(chunk_header) - SIZE_T_SIZE;
                
                malloc_print_address_hex(chunk_ptr);
                malloc_print(" sm chunk_header (");
                malloc_print_size(sizeof(chunk_header));
                malloc_print(" bytes): ");
                malloc_print_size(chunk_size);
                malloc_print(" ");
                malloc_print_size((size_t)in_use_bit_flag);
                malloc_print(" ");
                malloc_print_address_hex(free_chunk_ptr->prev_chunk);
                malloc_print(" ");
                malloc_print_address_hex(free_chunk_ptr->next_chunk);
                malloc_print(" ");
                malloc_print(" free bytes (");
                malloc_print_size(free_bytes);
                malloc_print(" bytes) ");
                malloc_print(" size (");
                malloc_print_size(SIZE_T_SIZE);
                malloc_print("): ");
                malloc_print_size(*footer_size);
                malloc_print("\n");

            }
            chunk_ptr = (size_t *)((uint8_t *)(chunk_ptr) + chunk_size);
        }
    }  


    for (memory_zone *large_zone = g_malloc_data.zones_list[LARGE_ZONE]; large_zone != NULL; large_zone = large_zone->next_zone)
    {
        malloc_print_address_hex(large_zone);
        malloc_print(" large zone header (");
        malloc_print_size(MEMORY_ZONE_SIZE);
        malloc_print(" bytes): ");
        malloc_print_address_hex(large_zone->next_zone);
        malloc_print("\n");

        size_t *chunk_ptr = (size_t *)((uint8_t *)large_zone + MEMORY_ZONE_SIZE);
        size_metadata chunk_metadata = malloc_read_size_metadata((chunk_header *)chunk_ptr);
        size_t chunk_size = chunk_metadata.size;
        uint8_t in_use_bit_flag = chunk_metadata.in_use;
        if (in_use_bit_flag)
        {
            size_t in_use_bytes = chunk_size - SIZE_T_SIZE;

            malloc_print_address_hex(chunk_ptr);
            malloc_print(" lg chunk_header (");
            malloc_print_size(SIZE_T_SIZE);
            malloc_print(" bytes): ");
            malloc_print_size(chunk_size);
            malloc_print(" ");
            malloc_print_size((size_t)in_use_bit_flag);
            malloc_print(" in use body (");
            malloc_print_size(in_use_bytes);
            malloc_print(")\n");
        }
        
    }

}

//TODO: remove this
void hexdump(void *mem, unsigned int len) {
    unsigned char *buf = (unsigned char*)mem;
    unsigned int i, j;

    for (i = 0; i < len; i += 16) {
        // Print the memory address
        printf("0x%06x: ", (unsigned int)((unsigned long)buf + i));

        // Print the hex values
        for (j = 0; j < 16; j++) {
            if (i + j < len)
                printf("%02x ", buf[i + j]);
            else
                printf("   ");

            if (j == 7)
                printf(" ");
        }

        printf(" ");

        // Print the ASCII representation
        for (j = 0; j < 16; j++) {
            if (i + j < len) {
                if (isprint(buf[i + j]))
                    printf("%c", buf[i + j]);
                else
                    printf(".");
            } else {
                printf(" ");
            }
        }

        printf("\n");
    }
}

//TODO: remove this
void print_chunk_list(chunk_header *chunk) {
    printf("--- Chunk list ---\n");
    while (chunk != NULL) {
        printf("%p\n", chunk);
        chunk = chunk->next_chunk;
    }
    printf("--- --- --- --- ---\n");
}

//TODO: remove this
void print_zone_list(memory_zone *zone) {
    printf("--- zone list ---\n");
    while (zone != NULL) {
        printf("%p\n", zone);
        zone = zone->next_zone;
    }
    printf("--- --- --- --- ---\n");
}