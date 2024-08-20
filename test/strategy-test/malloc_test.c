#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "libft_malloc/libft_malloc.h"

#include "malloc_types.h"
#include "malloc_state.h"
#include "malloc_utils.h"
#include "malloc_test.h"
#include "free_list_strategy.h"

void print_test(char *test_name, int result, char *reason) {
    char *result_str = (result == TEST_OK) ? "OK" : "FAILED";
    
    if (result == TEST_OK) {
        //fprintf(stdout, "TEST %s %s\n", test_name, result_str);
        malloc_print("TEST ");
        malloc_print(test_name);
        malloc_print(" ");
        malloc_print(result_str);
        malloc_print("\n");
    }
    else {
        //fprintf(stderr, "TEST %s %s: %s\n", test_name, result_str, reason);

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

    printf("metadata.size: %zu\n", metadata.size);

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