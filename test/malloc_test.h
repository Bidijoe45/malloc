#ifndef MALLOC_TEST
#define MALLOC_TEST

#include "malloc_utils.h"

#define TEST_OK 0
#define TEST_FAIL 1

void print_test(int test_n, int result, char *reason);
malloc_block *get_block_from_data(void *g_block, void *data_allocation);
malloc_block *get_previous_block(void *g_block, malloc_block *target_block);
int block_is_allocated(void *g_block, void *data_ptr);
int block_is_freed(void *g_block, void *data_ptr);

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test7();
void test8();

#endif