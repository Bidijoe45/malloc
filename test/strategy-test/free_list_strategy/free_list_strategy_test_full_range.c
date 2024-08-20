#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"
#include "free_list_strategy.h"

int main() {
    char *test_name = "free list strategy test 6";

    char *m = malloc(2048);
    free(m);

    size_t min_size = 130;
    size_t max_size = g_malloc_data.sizes[SMALL_ZONE].chunk;
    size_t total_sizes = max_size - min_size;
    char *allocations[total_sizes];

    int i = 0;
    while (i < total_sizes) {
        size_t size = min_size + i;
        allocations[i] = malloc(size);
        allocations[i][0] = 'X';
        allocations[i][size - 1] = 'X';
        i++;
    }

    i = 0;
    while (i < total_sizes) {
        free(allocations[i]);
        i++;
    }

    print_test(test_name, TEST_OK, NULL);
}