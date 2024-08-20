#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"
#include "free_list_strategy.h"


int main() {
   char *test_name = "free list strategy test 4";
    size_t n_allocations = 10;
    size_t sizes[n_allocations];
    initialize_malloc();
    initialize_test_sizes_array(sizes, n_allocations, g_malloc_data.sizes[TINY_ZONE].chunk + 100, g_malloc_data.sizes[SMALL_ZONE].payload);
    char *pointers[n_allocations];

    for (int i=0; i < n_allocations; i++) {
        pointers[i] = malloc(sizes[i]);

        pointers[i][0] = 'X';
        pointers[i][sizes[i] - 1] = 'X';

        chunk_header *chunk = get_chunk_header(pointers[i]);
        size_metadata metadata = malloc_read_size_metadata(chunk);
        if (fls_check_exected_size(chunk, metadata.size)) {
            print_test(test_name, TEST_FAIL, "invalid chunk size");
            return 1;
        }

        if (metadata.in_use != 1) {
            print_test(test_name, TEST_FAIL, "chunk should be in use");
            return 1;
        }

    }

    for (int i=0; i < n_allocations; i += 2) {
        free(pointers[i]);
        chunk_header *chunk = get_chunk_header(pointers[i]);

        if (!fls_is_chunk_free(chunk)) {
            //printf("chunk: %p\n", chunk);
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            return 1;
        }
    }

    for (int i=0; i < n_allocations; i += 2) {
        pointers[i] = malloc(sizes[i]);

        pointers[i][0] = 'X';
        pointers[i][sizes[i] - 1] = 'X';

        chunk_header *chunk = get_chunk_header(pointers[i]);
        size_metadata metadata = malloc_read_size_metadata(chunk);
        if (fls_check_exected_size(chunk, metadata.size)) {
            print_test(test_name, TEST_FAIL, "invalid chunk size");
            //printf("failed at chunk i: %d\n", i);
            return 1;
        }

        if (metadata.in_use != 1) {
            print_test(test_name, TEST_FAIL, "chunk should be in use");
            //printf("failed at chunk i: %d\n", i);
            return 1;
        }
    }

    for (int i=1; i < n_allocations; i += 2) {
        free(pointers[i]);
        chunk_header *chunk = get_chunk_header(pointers[i]);

        if (!fls_is_chunk_free(chunk)) {
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            return 1;
        }
    }

    for (int i=1; i < n_allocations; i += 2) {
        pointers[i] = malloc(sizes[i]);

        pointers[i][0] = 'X';
        pointers[i][sizes[i] - 1] = 'X';

        chunk_header *chunk = get_chunk_header(pointers[i]);
        size_metadata metadata = malloc_read_size_metadata(chunk);
        if (fls_check_exected_size(chunk, metadata.size)) {
            print_test(test_name, TEST_FAIL, "invalid chunk size");
            //printf("failed at chunk i: %d\n", i);
            return 1;
        }

        if (metadata.in_use != 1) {
            print_test(test_name, TEST_FAIL, "chunk should be in use");
            //printf("failed at chunk i: %d\n", i);
            return 1;
        }
    }

    for (int i=0; i < n_allocations; i++) {
        free(pointers[i]);
        chunk_header *chunk = get_chunk_header(pointers[i]);

        if (!fls_is_chunk_free(chunk)) {
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            return 1;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}