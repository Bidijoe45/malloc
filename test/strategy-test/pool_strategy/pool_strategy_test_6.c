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
    char *test_name = "test pool strategy 6";
    size_t n_pointers = 1024;
    char *pointers[n_pointers];
    int size = 120;
    bool valid = false;

    for (size_t i=0; i < n_pointers; i++) {
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
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

    for (size_t i=0; i < n_pointers; i++) {

        write_dummy_data(pointers[i], size);
        free(pointers[i]);
        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return 1;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}