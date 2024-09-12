#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
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
            fprintf(stderr, "ERROR: MAX ZONE ITERATIONS\n");
            exit(1);
        }
        zone_i++;

        current_zone = current_zone->next_zone;
    }

    current_zone = g_malloc_data.zones_list[zone_type];
    while (current_zone != NULL) {
        int zone_size = (zone_type < 2) ? g_malloc_data.sizes[zone_type].zone : get_zone_type_expected_size(allocation_size);
        memory_zone *zone_end = (memory_zone *)(((char *)current_zone) + zone_size);
        fprintf(stderr, "zone: %p -> %p\n", current_zone, zone_end);
        current_zone = current_zone->next_zone;
    }

    fprintf(stderr, "invalid ptr: %p \n", ptr);

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
        printf("chunk header: invalid zone\n");
        return false;
    }

    int expected_size = get_zone_type_expected_size(allocation_size);
    int valid_address = check_return_address_size(ptr, expected_size);
    if (!valid_address) {
        printf("chunk header: invalid address size\n");
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
            fprintf(stderr, "%c: %d\n", c, id);

            int vaid_header = chunk_header_is_valid(allocations[id], allocations_sizes[id]);
            if (!vaid_header) {
                fprintf(stderr, "ERROR: INVALID CHUNK HEADER: %c %d\n", c, id);
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
            fprintf(stderr, "%c: %d\n", c, id);

            int vaid_header = chunk_header_is_valid(allocations[id], allocations_sizes[id]);
            if (!vaid_header) {
                fprintf(stderr, "ERROR: INVALID CHUNK HEADER: %c %d\n", c, id);
                exit(1);
            }
            
            if (allocations[id] == NULL) {
                fprintf(stderr, "Realloc returned NULL\n");
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
            fprintf(stderr, "%c: %d\n", c, id);
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