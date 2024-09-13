#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/resource.h>

#include "malloc_test.h"

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"

int main() {
    size_t n_sizes = 8;
    size_t sizes_to_test[8];
    initialize_malloc();
    initialize_pool_test_sizes_array(sizes_to_test, n_sizes);

    char *test_name = "test pool strategy 5";
    
    char *pointers1[n_sizes];
    char *pointers2[n_sizes];
    char *pointers3[n_sizes];
    char *pointers4[n_sizes];
    bool valid = false;

    for (size_t i=0; i < n_sizes; i++) {

        int size = sizes_to_test[i];

        pointers1[i] = malloc(size);
        valid = check_return_address_size(pointers1[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return 1;
        }

        valid = check_metadata_in_use(pointers1[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers2[i] = malloc(size);
        valid = check_return_address_size(pointers2[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return 1;
        }

        valid = check_metadata_in_use(pointers2[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers3[i] = malloc(size);
        valid = check_return_address_size(pointers3[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return 1;
        }

        valid = check_metadata_in_use(pointers3[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers4[i] = malloc(size);
        valid = check_return_address_size(pointers4[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return 1;
        }

        valid = check_metadata_in_use(pointers4[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers1[i], size);
        free(pointers1[i]);

        valid = check_metadata_in_use(pointers1[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers2[i], size);
        free(pointers2[i]);

        valid = check_metadata_in_use(pointers2[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers3[i], size);
        free(pointers3[i]);

        valid = check_metadata_in_use(pointers3[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers4[i], size);
        free(pointers4[i]);

        valid = check_metadata_in_use(pointers4[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}