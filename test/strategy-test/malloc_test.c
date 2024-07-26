#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "libft_malloc/libft_malloc.h"

#include "malloc_types.h"
#include "malloc_state.h"
#include "malloc_utils.h"
#include "malloc_test.h"

void print_test(char *test_name, int result, char *reason) {
    char *result_str = (result == TEST_OK) ? "OK" : "FAILED";
    
    if (result == TEST_OK)
        fprintf(stdout, "TEST %s %s\n", test_name, result_str);
    else
        fprintf(stderr, "TEST %s %s: %s\n", test_name, result_str, reason);
}

chunk_header *get_chunk_header(void *address) {
    uint8_t *start_addr = (uint8_t *)address;
    chunk_header *chunk = (chunk_header *)(start_addr - sizeof(size_t)); //This is because the payload stars after size in chunk_header

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
    for (size_t i=0; i < size; i++) {
        address[i] = 100;
    }
}