#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>
#include "libft_malloc/libft_malloc.h"

#include "malloc_types.h"
#include "malloc_state.h"
#include "malloc_utils.h"
#include "malloc_test.h"
#include "free_list_strategy.h"

void print_test(char *test_name, int result, char *reason) {
    char *result_str = (result == TEST_OK) ? "OK" : "FAILED";
    
    if (result == TEST_OK) {
        malloc_print("TEST ");
        malloc_print(test_name);
        malloc_print(" ");
        malloc_print(result_str);
        malloc_print("\n");
    }
    else {
        malloc_print("TEST ");
        malloc_print(test_name);
        malloc_print(" ");
        malloc_print(result_str);
        malloc_print(" ");
        malloc_print(reason);
        malloc_print("\n");
    }
}

chunk_header *get_chunk_header(void *address) {
    uint8_t *start_addr = (uint8_t *)address;
    chunk_header *chunk = (chunk_header *)(start_addr - SIZE_T_SIZE);

    return chunk;
}

bool check_return_address_size(void *address, size_t expected_size) {
    chunk_header *chunk = get_chunk_header(address);
    size_metadata metadata = malloc_read_size_metadata(chunk);

    if (metadata.size != expected_size)
        return false;

    return true;
}

bool check_metadata_in_use(void *address, int in_use) {
    chunk_header *chunk = get_chunk_header(address);
    size_metadata metadata = malloc_read_size_metadata(chunk);

    if (metadata.in_use != in_use)
        return false;

    return true;
}

void write_dummy_data(char *address, size_t size) {
    char c = 33;

    for (size_t i=0; i < size; i++) {    
        address[i] = c;

        if (c == 126)
            c = 33;
        else
            c++;
    }
}

size_t calculate_large_expected_size(size_t size) {
    return ALIGN(size) + SIZE_T_SIZE;
}

int fls_check_exected_size(chunk_header *chunk, size_t size) {
    size_t expected_size = ALIGN(size) + SIZE_T_SIZE * 2;

    if (expected_size == size)
        return 1;

    return 0;
}

int fls_is_chunk_free(chunk_header *chunk) {
    memory_zone *chunk_zone = fls_get_chunk_memory_zone(chunk);

    if (chunk_zone == NULL)
        return 1;
    
    size_metadata metadata = malloc_read_size_metadata(chunk);
    if (metadata.in_use == 0) {
        return 1;
    }

    return 0;
}

void initialize_test_sizes_array(size_t *sizes, size_t n_sizes, size_t min_size, size_t max_size) {
    size_t increment = (max_size - min_size) / n_sizes;
    size_t val = min_size;

    sizes[0] = min_size + 1;
    for (size_t i=1; i < n_sizes; i++) {
        sizes[i] = val;
        val += increment;
    }
}

void initialize_pool_test_sizes_array(size_t *sizes, size_t n_sizes) {
    initialize_test_sizes_array(sizes, n_sizes, 1, g_malloc_data.sizes[TINY_ZONE].payload);
}

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