#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"
#include "free_list_strategy.h"

int main() {
    char *test_name = "free list strategy test 5";

    size_t n_allocations = 10000;
    char *pointers[n_allocations];
    size_t size = 25364;
    for (int i=0; i < n_allocations; i++) {
        pointers[i] = malloc(size);

        pointers[i][0] = 'X';
        pointers[i][size - 1] = 'X';

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
            printf("chunk: %p\n", chunk);
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            return 1;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}