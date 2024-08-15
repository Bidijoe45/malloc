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

    char *tiny_mem = malloc(98);
    valid = check_metadata_in_use(tiny_mem, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    write_dummy_data(tiny_mem, 98);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, tiny_mem, 98);

    char *small_mem = realloc(tiny_mem, 9684);
    valid = check_metadata_in_use(small_mem, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, small_mem, 98) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    write_dummy_data(small_mem, 9684);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, small_mem, 9684);
    
    char *tiny_mem_2 = realloc(small_mem, 50);
    valid = check_metadata_in_use(tiny_mem_2, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, tiny_mem_2, 50) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    write_dummy_data(tiny_mem_2, 50);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, tiny_mem_2, 50);

    char *large_mem = realloc(tiny_mem_2, 49763);
    valid = check_metadata_in_use(large_mem, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, large_mem, 50) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    write_dummy_data(large_mem, 49763);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, large_mem, 49763);

    char *small_mem2 = realloc(large_mem, 20000);
    valid = check_metadata_in_use(small_mem2, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, small_mem2, 20000) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    write_dummy_data(small_mem2, 20000);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, small_mem2, 20000);

    char *small_mem3 = realloc(small_mem2, 15345);
    valid = check_metadata_in_use(small_mem3, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, small_mem3, 15345) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    free(small_mem3);

    print_test(test_name, TEST_OK, NULL);
    return 0;
}