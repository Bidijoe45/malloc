#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"
#include "free_list_strategy.h"


int main() {
   char *test_name = "free list strategy test 3";

    size_t allocation_size = 7000;
    size_t n_allocations = 1024;
    void *pointers[n_allocations];

    for (int i=0; i < n_allocations; i++) {
        char *mem = malloc(allocation_size);
        pointers[i] = mem;
        chunk_header *chunk = get_chunk_header(mem);
        size_metadata metadata = malloc_read_size_metadata(chunk);

        mem[0] = 'X';
        mem[allocation_size - 1] = 'X';

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