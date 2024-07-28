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
    char c = 33;

    for (size_t i=0; i < size; i++) {    
        address[i] = c;

        if (c == 126)
            c = 33;
        else
            c++;
    }
}

int fls_check_exected_size(chunk_header *chunk, size_t size) {
    size_t expected_size = ALIGN(size + sizeof(size_t) * 2);

    if (expected_size == size)
        return 1;

    if (size > expected_size
        && size < (expected_size + g_malloc_data.sizes[TINY_ZONE].chunk))
    {
        printf("chunk: %p -> size is bigger than expected size, \
             but should be in boundaries\n", chunk);
        return 1;
    }

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