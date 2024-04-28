#include <stdio.h>
#include <unistd.h>
#include "libft_malloc/libft_malloc.h"

#include "malloc_types.h"
#include "malloc_state.h"
#include "malloc_utils.h"
#include "malloc_test.h"

void print_test(int test_n, int result, char *reason) {
    char *result_str = (result == TEST_OK) ? "OK" : "FAILED";
    
    if (result == TEST_OK)
        printf("TEST %d %s\n", test_n, result_str);
    else
        printf("TEST %d %s -> %s\n", test_n, result_str, reason);
}

int main() {

    void *m = malloc(1);

    hexdump(g_malloc_data.zones[TINY_ZONE], g_malloc_data.tiny_zone_size);
}