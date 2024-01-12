#include <stdio.h>
#include "libft_malloc/libft_malloc.h"

#include "malloc_utils.h"
#include "malloc_test.h"

void print_test(int test_n, int result, char *reason) {
    char *result_str = (result == TEST_OK) ? "OK" : "FAILED";
    
    if (result == TEST_OK)
        printf("TEST %d %s\n", test_n, result_str);
    else
        printf("TEST %d %s -> %s\n", test_n, result_str, reason);
}

malloc_block *get_block_from_data(void *g_block, void *data_allocation) {
    malloc_block *current_block = g_block;
    while (current_block != NULL) {

        if (current_block->data_location == data_allocation)
            return current_block;

        current_block = current_block->next;
    }

    return NULL;
}

malloc_block *get_previous_block(void *g_block, malloc_block *target_block) {
    malloc_block *current_block = g_block;
    while (current_block != NULL) {

        if (current_block->next == target_block)
            return current_block;

        current_block = current_block->next;
    }

    return NULL;
}

int block_is_allocated(void *g_block, void *data_ptr) {
    malloc_block *current_block = get_block_from_data(g_block, data_ptr);

    if (current_block->in_use == 1)
        return 1;

    return 0;
}

int block_is_freed(void *g_block, void *data_ptr) {
    malloc_block *current_block = get_block_from_data(g_block, data_ptr);  

    if (current_block->in_use == 0)
        return 1;

    return 0;
}

int main() {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
}