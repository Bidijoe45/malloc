#ifndef MALLOC_TEST
#define MALLOC_TEST

#include "malloc_utils.h"

#define TEST_OK 0
#define TEST_FAIL 1

void print_test(int test_n, int result, char *reason);
malloc_block *get_block_from_data(void *g_block, void *data_allocation);
int check_block_is_allocated(void *g_block, void *data);
int check_block_is_freed(void *g_block, void *data);

void test1();
void test2();
void test3();
void test4();

#endif