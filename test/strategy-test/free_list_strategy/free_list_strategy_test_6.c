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

    char *mem = malloc(1000);

    visualize_memory();

    free(mem);

    print_test(test_name, TEST_OK, NULL);
}