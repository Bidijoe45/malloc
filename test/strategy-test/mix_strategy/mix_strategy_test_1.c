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
    char *test_name = "mix strategy test 1";
    int valid = 0;

    size_t tmp_buff_size = 60000;
    char tmp_mem_buff[tmp_buff_size];

    char *tiny_mem = malloc(100);
    valid = check_metadata_in_use(tiny_mem, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    write_dummy_data(tiny_mem, 100);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, tiny_mem, 100);

    char *small_mem = realloc(tiny_mem, 10000);
    valid = check_metadata_in_use(small_mem, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, small_mem, 100) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    write_dummy_data(small_mem, 10000);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, small_mem, 10000);

    char *large_mem = realloc(small_mem, 60000);
    valid = check_metadata_in_use(large_mem, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, large_mem, 10000) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    free(large_mem);

    print_test(test_name, TEST_OK, NULL);
    return 0;
}