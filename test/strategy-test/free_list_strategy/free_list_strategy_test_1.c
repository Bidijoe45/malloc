#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"
#include "free_list_strategy.h"


int main() {
    char *test_name = "free list strategy test 1";
    size_t allocation_size = 255;
    char *mem = malloc(allocation_size);

    chunk_header *chunk = get_chunk_header(mem);
    size_metadata metadata = malloc_read_size_metadata(chunk);

    if (fls_check_exected_size(chunk, allocation_size))  {
        print_test(test_name, TEST_FAIL, "invalid chunk size");
        return 1;
    }

    if (metadata.in_use != 1)  {
        print_test(test_name, TEST_FAIL, "chunk should be in use");
        return 1;
    }

    for (int i=0; i < allocation_size; i++)
        mem[i] = 'X';

    free(mem);

    metadata = malloc_read_size_metadata(chunk);

    if (metadata.in_use == 1) {
        print_test(test_name, TEST_FAIL, "chunk should not be in use");
        return 1;
    }

    print_test(test_name, TEST_OK, NULL);
}