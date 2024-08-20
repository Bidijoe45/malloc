#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"

int main() {
    size_t n_sizes = 8;
    size_t sizes_to_test[n_sizes];
    initialize_malloc();
    initialize_test_sizes_array(sizes_to_test, n_sizes, g_malloc_data.sizes[SMALL_ZONE].chunk + 1000, 1000000);

    char *test_name = "large starategy test 1";
    
    for (size_t i=0; i < n_sizes; i++) {

        size_t size = sizes_to_test[i];

        char *mem = malloc(size);

        if (mem == NULL) {
            print_test(test_name, TEST_FAIL, "Malloc returned null");
            return 1;
        }

        bool valid = false;
        size_t expected_size = calculate_large_expected_size(size);
        valid = check_return_address_size(mem, expected_size);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return 1;
        }

        valid = check_metadata_in_use(mem, 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }

        write_dummy_data(mem, size);
        
        free(mem);
    }

    print_test(test_name, TEST_OK, NULL);
    return 0;
}