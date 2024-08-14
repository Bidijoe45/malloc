#ifndef MALLOC_TEST
#define MALLOC_TEST

#include <stdbool.h>
#include <stdint.h>

#include "malloc_utils.h"

#define TEST_OK 0
#define TEST_FAIL 1

void print_test(char *test_name, int result, char *reason);
bool check_return_address_size(void *address, size_t expected_size);
bool check_metadata_in_use(void *address, int in_use);
void write_dummy_data(char *address, size_t size);
int fls_is_chunk_free(chunk_header *chunk);
int fls_check_exected_size(chunk_header *chunk, size_t size);
size_t calculate_large_expected_size(size_t size);

#endif