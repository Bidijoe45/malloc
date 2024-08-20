#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/resource.h>

#include "malloc_test.h"

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"

int main() {
    char *test_name = "pool strategy realloc test 1";
    int valid = 0;

    initialize_malloc();
    int payload_size = g_malloc_data.sizes[TINY_ZONE].payload;
    
    char tmp_mem_buff[payload_size];

    char *tiny_mem = malloc(payload_size - payload_size / 2);
    valid = check_metadata_in_use(tiny_mem, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    write_dummy_data(tiny_mem, payload_size - payload_size / 2);

    memset(&tmp_mem_buff, 0, payload_size - payload_size / 2);
    memcpy(&tmp_mem_buff, tiny_mem, payload_size);

    char *tiny_mem2 = realloc(tiny_mem, payload_size);
    valid = check_metadata_in_use(tiny_mem2, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }
    
    if (memcmp(&tmp_mem_buff, tiny_mem2, payload_size - payload_size / 2) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    write_dummy_data(tiny_mem2, payload_size);
    
    memset(&tmp_mem_buff, 0, payload_size - payload_size / 2);
    memcpy(&tmp_mem_buff, tiny_mem2, payload_size);

    char *tiny_mem3 = realloc(tiny_mem2, payload_size - payload_size / 3);
    valid = check_metadata_in_use(tiny_mem3, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, tiny_mem3, payload_size - payload_size / 3) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    print_test(test_name, TEST_OK, NULL);
    return 0;
}