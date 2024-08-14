#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"

int main() {
    size_t sizes_to_test[8] = {70764, 84716, 112374, 89378, 73641, 93736, 123649, 83476};
    size_t n_sizes = 8;

    char *test_name = "large starategy test 2";
    
    char *pointers[n_sizes];
    bool valid = false;

    size_t half_sizes = n_sizes / 2;
    size_t remaining_sizes = n_sizes - half_sizes;

    for (size_t i=0; i < half_sizes; i++) {
        int size = sizes_to_test[i];
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], calculate_large_expected_size(size));
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return 1;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < half_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers[i], size);
        free(pointers[i]);
    }

    for (size_t i=0; i < remaining_sizes; i++) {
        int size = sizes_to_test[i];
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], calculate_large_expected_size(size));
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return 1;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < remaining_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

    }

        for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], calculate_large_expected_size(size));
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return 1;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

    }

    print_test(test_name, TEST_OK, NULL);
}