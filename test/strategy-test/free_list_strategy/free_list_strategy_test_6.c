#include <stdio.h>
#include <string.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"
#include "free_list_strategy.h"

int main() {
    char *test_name = "free list strategy test 6";
    size_t tmp_buff_size = 60000;
    char tmp_mem_buff[tmp_buff_size];
    int valid = 0;

    size_t mem1_size = small_upper_bound - 5000;
    char *mem1 = malloc(mem1_size);
    valid = check_metadata_in_use(mem1, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    write_dummy_data(mem1, mem1_size);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, mem1, mem1_size);

    size_t mem2_size = mem1_size + 1000;
    char *mem2 = realloc(mem1, mem2_size);
    valid = check_metadata_in_use(mem2, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    if (memcmp(&tmp_mem_buff, mem2, mem1_size) != 0) {
        print_test(test_name, TEST_FAIL, "Reallocated data is not the same as original data");
        return 1;
    }

    write_dummy_data(mem2, mem2_size);

    memset(&tmp_mem_buff, 0, tmp_buff_size);
    memcpy(&tmp_mem_buff, mem2, mem2_size);

    size_t mem3_size = mem2_size - 1000;
    char *mem3 = realloc(mem2, mem3_size);
    valid = check_metadata_in_use(mem3, 1);
    if (!valid) {
        print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
        return 1;
    }

    print_test(test_name, TEST_OK, NULL);
}