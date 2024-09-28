#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "malloc_state.h"
#include "malloc_utils.h"

#define TINY_SIZE 0
#define SMALL_SIZE 1
#define LARGE_SIZE 2

#define TEST_FULL 1

int get_zone_type_by_size(int size) {
    size = ALIGN(size);

    if (size <= g_malloc_data.sizes[TINY_ZONE].payload) {
        return TINY_SIZE;
    }
    if (size <= g_malloc_data.sizes[SMALL_ZONE].payload) {
        return SMALL_SIZE;
    }
    else if (size > g_malloc_data.sizes[SMALL_ZONE].payload) {
        return LARGE_SIZE;
    }

    return -1;
}

int get_zone_type_expected_size(int size) {
    size = ALIGN(size);

    if (size <= g_malloc_data.sizes[TINY_ZONE].payload) {
        return g_malloc_data.sizes[TINY_SIZE].chunk;
    }
    if (size <= g_malloc_data.sizes[SMALL_ZONE].payload) {
        return size + SIZE_T_SIZE * 2;
    }
    else if (size > g_malloc_data.sizes[SMALL_ZONE].payload) {
        return size + SIZE_T_SIZE;
    }

    return -1;
}

int check_out_of_zone(char *ptr, int allocation_size) {
    if (ptr == NULL)
        return false;

    int zone_type = get_zone_type_by_size(allocation_size);
    memory_zone *current_zone = g_malloc_data.zones_list[zone_type];
    int max_zones_iterations = 100000;
    size_t zone_i = 0;
    while (current_zone != NULL) {
        int zone_size = (zone_type < 2) ? g_malloc_data.sizes[zone_type].zone : get_zone_type_expected_size(allocation_size);
        memory_zone *zone_end = (memory_zone *)(((char *)current_zone) + zone_size);
        
        if (ptr > (char *)current_zone && ptr < (char *)zone_end) {
            return false;
        }

        if (zone_i > max_zones_iterations) {
            write(1, "ERROR: MAX ZONE ITERATIONS\n", strlen("ERROR: MAX ZONE ITERATIONS\n"));
            exit(1);
        }
        zone_i++;

        current_zone = current_zone->next_zone;
    }

    current_zone = g_malloc_data.zones_list[zone_type];
    while (current_zone != NULL) {
        int zone_size = (zone_type < 2) ? g_malloc_data.sizes[zone_type].zone : get_zone_type_expected_size(allocation_size);
        memory_zone *zone_end = (memory_zone *)(((char *)current_zone) + zone_size);
        //fprintf(stderr, "zone: %p -> %p\n", current_zone, zone_end);
        current_zone = current_zone->next_zone;
    }

    //fprintf(stderr, "invalid ptr: %p \n", ptr);

    return true;
}

bool check_return_address_size(void *address, size_t expected_size) {
    chunk_header *chunk = get_chunk_header(address);
    size_metadata metadata = malloc_read_size_metadata(chunk);

    if (metadata.size < expected_size || (metadata.size > (expected_size + g_malloc_data.sizes[TINY_SIZE].chunk)))
        return false;

    return true;
}

int chunk_header_is_valid(char *ptr, int allocation_size) {

    if (TEST_FULL == 0)
        return true;

    chunk_header *header = get_chunk_header(ptr);
    int invalid_zone = check_out_of_zone(ptr, allocation_size);
    
    if (invalid_zone) {
        //printf("chunk header: invalid zone\n");
        write(1, "chunk header: invalid zone\n", strlen("chunk header: invalid zone\n"));
        return false;
    }

    int expected_size = get_zone_type_expected_size(allocation_size);
    int valid_address = check_return_address_size(ptr, expected_size);
    if (!valid_address) {
        //printf("chunk header: invalid address size\n");
        return false;
    }

    return true;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        //fprintf(stderr, "invalid arguments :(\n");
        write(1, "Invalid argument\n", strlen("Invalid argument\n"));
        return 1;
    }
    
    int file = open(argv[1], 0);

    if (file == -1) {
        write(1, "Invalid file\n", strlen("Invalid file\n"));
        exit(1);
    }

    char parsed_n_allocations[31];
    memset(parsed_n_allocations, 0, sizeof(char) * 31);
    int first_line = read(file, &parsed_n_allocations, 30);
    size_t n_allocations = atoi(parsed_n_allocations);
    char *allocations[n_allocations];
    int allocations_sizes[n_allocations];
    
    for (int i=0; i < n_allocations; i++)
        allocations[i] = NULL;

    for (int i=0; i < n_allocations; i++)
        allocations_sizes[i] = 0;

    int rd = 1;
    int i=0;
    while (rd > 0) {
        char c;
        char s_id[22];
        char s_size[22];
        char nl;

        memset(s_id, 0, 22);
        memset(s_size, 0, 22);
        
        s_id[21] = 0;
        s_size[21] = 0;

        rd = read(file, &c, 1);
        rd = read(file, &s_id, 21);
        rd = read(file, &s_size, 21);
        rd = read(file, &nl, 1);

        if (rd == 0)
            break;

        int id = atoi(s_id);
        int size = atoi(s_size);

        if (c == 'M') {
            allocations[id] = malloc(size);
            allocations_sizes[id] = size;
            //fprintf(stderr, "%c: %d\n", c, id);

            if (allocations[id] == NULL) {
                write(1, "ERROR: malloc returned 0\n", strlen("ERROR: malloc returned 0\n"));
                //exit(1);
            }

            int vaid_header = chunk_header_is_valid(allocations[id], allocations_sizes[id]);
            if (!vaid_header) {
                //fprintf(stderr, "ERROR: INVALID CHUNK HEADER: %c %d\n", c, id);
                write(1, "ERROR: INVALID CHUNK HEADER\n", strlen("ERROR: INVALID CHUNK HEADER\n"));
                exit(1);
            }

            if (allocations[id] == NULL) {
                //fprintf(stderr, "Malloc returned NULL\n");
                continue;
            }

            if (size >= 8) {
                allocations[id][0] = c;
                allocations[id][1] = c;
                allocations[id][2] = c;
                allocations[id][3] = c;
                int *int_pos = (int *)(&allocations[id][4]);
                *int_pos = size;
            }

            if (size >= 16) {
                allocations[id][size - 1] = c;
                allocations[id][size - 2] = c;
                allocations[id][size - 3] = c;
                allocations[id][size - 4] = c;
                int *int_pos = (int *)(&allocations[id][size - 8]);
                *int_pos = size;
            }
            
            if (size < 8) {
                allocations[id][0] = 'M';
            }
        }
        else if (c == 'R') {
            allocations[id] = realloc(allocations[id], size);
            allocations_sizes[id] = size;
            //fprintf(stderr, "%c: %d\n", c, id);

            int vaid_header = chunk_header_is_valid(allocations[id], allocations_sizes[id]);
            if (!vaid_header) {
                fprintf(stderr, "ERROR: INVALID CHUNK HEADER: %c %d\n", c, id);
                write(1, "ERROR: INVALID CHUNK HEADER:\n", strlen("ERROR: INVALID CHUNK HEADER:\n"));
                exit(1);
            }
            
            if (allocations[id] == NULL) {
                //fprintf(stderr, "Realloc returned NULL\n");
                write(1, "Realloc returned NULL\n", strlen("Realloc returned NULL\n"));
                continue;
            }

            if (size >= 8) {
                allocations[id][0] = c;
                allocations[id][1] = c;
                allocations[id][2] = c;
                allocations[id][3] = c;
                int *int_pos = (int *)(&allocations[id][4]);
                *int_pos = size;
            }

            if (size >= 16) {
                allocations[id][size - 1] = c;
                allocations[id][size - 2] = c;
                allocations[id][size - 3] = c;
                allocations[id][size - 4] = c;
                int *int_pos = (int *)(&allocations[id][size - 8]);
                *int_pos = size;
            }
            
            if (size < 8) {
                allocations[id][0] = 'M';
            }
        }
        else if (c == 'F') {
            free(allocations[id]);
            allocations[id] = NULL;
            //fprintf(stderr, "%c: %d\n", c, id);
        }

        //Check all chunks
        for (int i=0; i < n_allocations; i++) {
            if(allocations[i] == NULL)
                continue;

            int chunk_is_valid = chunk_header_is_valid(allocations[i], allocations_sizes[i]);
            if (!chunk_is_valid) {
                //fprintf(stderr, "ERROR: ALL CHUNKS, INVALID CHUNK HEADER, %i\n", i);
                write(1, "ERROR: ALL CHUNKS, INVALID CHUNK HEADER\n", strlen("ERROR: ALL CHUNKS, INVALID CHUNK HEADER\n"));
                exit(1);
            }
        }
    }

    close(file);

    return 0;
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
