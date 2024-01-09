#include <stdio.h>
#include <string.h>

#include "malloc_test.h"

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"

/*
    Checks for:
        - First allocation return first block
        - Data direction is write/read correctly
        - First allocated block is freed
*/
void test1() {
    int test_num = 1;
    char *str = "987654321";

    //This should return the first block
    char *allocation = malloc(10);

    //Check if the first block is in use
    malloc_block *block = g_memory_block;

    if (block->in_use != 1) {
        print_test(test_num, TEST_FAIL, "Block was not assigned to use");
        return;
    }

    //Check if memory is copied correctly
    strcpy(allocation, str);
    int cmp = strcmp(allocation, str);

    if (cmp != 0) {
        print_test(test_num, TEST_FAIL, "Memory was not copied correctly");
        return;
    }

    //This should free the first block
    free(allocation);

    //Check if the first block is not in use anymore
    if (block->in_use != 0) {
        print_test(test_num, TEST_FAIL, "Free did not set the in_use flag correctly");
        return;
    }

    print_test(test_num, TEST_OK, NULL);
}

/*
    Checks for:
        - Two allocations
        - Allocations should be one next to the other
        - Blocks are freed correctly
*/
void test2() {
    int test_num = 2;

    //This should return the first block
    char *allocation1 = malloc(5);

    //This should return the second block
    char *allocation2 = malloc(8);

    show_alloc_mem();

    malloc_block *g_block = g_memory_block;

    //Check if first allocation is in use
    if (g_block->in_use != 1) {
        print_test(test_num, TEST_FAIL, "First allocation is not the first block");
        return ;
    }

    //Check if second allocation is in use
    g_block++;
    if (g_block->in_use != 1) {
        print_test(test_num, TEST_FAIL, "Second allocation is not the first block");
        return ;
    }

    //Check if block 2 is freed correctly
    free(allocation2);
    if (g_block->in_use != 0) {
        print_test(test_num, TEST_FAIL, "Second allocation is not freed correctly");
        return ;
    }

    //Check if bloc 1 is freed correctly
    free(allocation1);

    g_block--;
    if (g_block->in_use != 0) {
        print_test(test_num, TEST_FAIL, "First allocation is not freed correctly");
        return ;
    }

    print_test(test_num, TEST_OK, NULL);
}

/*
    Checks for:
        - Data returned from the malloc corresponds to correct memory block
*/
void test3() {
    int test_num = 3;

    char *allocation1 = malloc(6);
    char *allocation2 = malloc(6);
    char *allocation3 = malloc(6);
    char *allocation4 = malloc(6);

    //Allocation 1
    malloc_block *block1 = get_block_from_data(g_memory_block, allocation1);

    if (block1->data_location == NULL) {
        print_test(test_num, TEST_FAIL, "Allocation 1 data ptr cannot be found in any block");
        return;
    } 

    if (block1->data_location != allocation1) {
        print_test(test_num, TEST_FAIL, "Allocation 1 data ptr is not the same as block data ptr");
        return;
    }

    //Allocation 2
    malloc_block *block2 = get_block_from_data(g_memory_block, allocation2);

    if (block2->data_location == NULL) {
        print_test(test_num, TEST_FAIL, "Allocation 2 data ptr cannot be found in any block");
        return;
    } 

    if (block2->data_location != allocation2) {
        print_test(test_num, TEST_FAIL, "Allocation 2 data ptr is not the same as block data ptr");
        return;
    }

    //Allocation 3
    malloc_block *block3 = get_block_from_data(g_memory_block, allocation3);

    if (block3->data_location == NULL) {
        print_test(test_num, TEST_FAIL, "Allocation 3 data ptr cannot be found in any block");
        return;
    } 

    if (block3->data_location != allocation3) {
        print_test(test_num, TEST_FAIL, "Allocation 3 data ptr is not the same as block data ptr");
        return;
    }

    //Allocation 4
    malloc_block *block4 = get_block_from_data(g_memory_block, allocation4);

    if (block4->data_location == NULL) {
        print_test(test_num, TEST_FAIL, "Allocation 4 data ptr cannot be found in any block");
        return;
    } 

    if (block4->data_location != allocation4) {
        print_test(test_num, TEST_FAIL, "Allocation 4 data ptr is not the same as block data ptr");
        return;
    }

    free(allocation1);
    free(allocation2);
    free(allocation3);
    free(allocation4);

    //Check free for allocation 1
    int block_freed_correctly = check_block_is_freed(g_memory_block, allocation1);

    if (block_freed_correctly == 0) {
        print_test(test_num, TEST_FAIL, "Allocation 1 is not freed correctly");
        return;
    }

    //Check free for allocation 2
    block_freed_correctly = check_block_is_freed(g_memory_block, allocation2);
    
    if (block_freed_correctly == 0) {
        print_test(test_num, TEST_FAIL, "Allocation 2 is not freed correctly");
        return;
    }

    //Check free for allocation 3
    block_freed_correctly = check_block_is_freed(g_memory_block, allocation3);
    
    if (block_freed_correctly == 0) {
        print_test(test_num, TEST_FAIL, "Allocation 3 is not freed correctly");
        return;
    }

    //Check free for allocation 4
    block_freed_correctly = check_block_is_freed(g_memory_block, allocation4);
    
    if (block_freed_correctly == 0) {
        print_test(test_num, TEST_FAIL, "Allocation 4 is not freed correctly");
        return;
    }

    print_test(test_num, TEST_OK, NULL);
}

/*
    Checks for:
        - Allocation bigger than block biggest size
*/
void test4() {
    int test_num = 4;
    size_t size = SMALL_ZONE_SIZE + 10;

    char *allocation = malloc(size);
    
    malloc_block *block = get_block_from_data(g_memory_block, allocation);
    
    //Check if data returned by malloc exists in all blocks
    if (block == NULL) {
        print_test(test_num, TEST_FAIL, "Allocation data ptr cannot be found in any block");
        return;
    }

    //Check if size is correct
    if (block->size != size) {
        print_test(test_num, TEST_FAIL, "Block size is not correct");
        return;
    }

    //Check if memory is copied correctly
    char *str = "987654321";
    strcpy(allocation, str);
    int cmp = strcmp(allocation, str);

    if (cmp != 0) {
        print_test(test_num, TEST_FAIL, "Memory was not copied correctly");
        return;
    }

    //Check if block is freed correctly
    free(allocation);

    block = get_block_from_data(g_memory_block, allocation);

    if (block != NULL) {
        print_test(test_num, TEST_FAIL, "Allocation data ptr should be NULL because block should have been unnmaped");
        return;
    }

    print_test(test_num, TEST_OK, NULL);
}