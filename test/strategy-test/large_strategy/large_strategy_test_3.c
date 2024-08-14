#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"

int main() {
    char *test_name = "large starategy test 3";
    
    size_t size = 1000000;
    char *ptr = malloc(size);

    ptr[0] = 'X';
    ptr[size - 1] = 'X';

    print_test(test_name, TEST_OK, NULL);
}