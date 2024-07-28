#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"

int main() {
    size_t sizes_to_test[8] = {63764, 84716, 112374, 89378, 73641, 93736, 123649, 83476};
    size_t n_sizes = 8;

    char *test_name = "large starategy test 1";
    
    for (size_t i=0; i < n_sizes; i++) {

        int size = sizes_to_test[i];
        char *mem = malloc(size);

        bool valid = false;

        valid = check_return_address_size(mem, g_malloc_data.sizes[TINY_ZONE].chunk);
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
        valid = check_metadata_in_use(mem, 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Free: metadata is in use, should not be");
            return 1;
        }
    }

    print_test(test_name, TEST_OK, NULL);
    return 0;
}