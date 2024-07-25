#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"
#include "free_list_strategy.h"

int check_exected_size(chunk_header *chunk, size_t size) {
    size_t expected_size = ALIGN(size + sizeof(size_t) * 2);

    if (expected_size == size)
        return 1;

    if (size > expected_size
        && size < (expected_size + g_malloc_data.sizes[TINY_ZONE].chunk))
    {
        printf("chunk: %p -> size is bigger than expected size, \
             but should be in boundaries\n", chunk);
        return 1;
    }

    return 0;
}

int is_chunk_free(chunk_header *chunk) {
    memory_zone *chunk_zone = fls_get_chunk_memory_zone(chunk);

    if (chunk_zone == NULL)
        return 1;
    
    size_metadata metadata = malloc_read_size_metadata(chunk);
    if (metadata.in_use == 0) {
        return 1;
    }

    return 0;
}

void free_list_strategy_test_1() {
    char *test_name = "free list strategy test 1";
    size_t allocation_size = 255;
    char *mem = malloc(allocation_size);

    chunk_header *chunk = get_chunk_header(mem);
    size_metadata metadata = malloc_read_size_metadata(chunk);

    if (check_exected_size(chunk, allocation_size)) {
        print_test(test_name, TEST_FAIL, "invalid chunk size");
        return;
    }

    if (metadata.in_use != 1) {
        print_test(test_name, TEST_FAIL, "chunk should be in use");
        return;
    }

    for (int i=0; i < allocation_size; i++)
        mem[i] = 'X';

    free(mem);

    metadata = malloc_read_size_metadata(chunk);

    if (metadata.in_use == 1) {
        print_test(test_name, TEST_FAIL, "chunk should not be in use");
        return;
    }

    print_test(test_name, TEST_OK, NULL);
}

void free_list_startegy_test_2() {
    char *test_name = "free list strategy test 2";

    size_t allocation_size = 255;
    size_t n_allocations = 255;
    void *pointers[n_allocations];

    for (int i=0; i < n_allocations; i++) {
        char *mem = malloc(allocation_size);
        pointers[i] = mem;
        chunk_header *chunk = get_chunk_header(mem);
        size_metadata metadata = malloc_read_size_metadata(chunk);

        mem[0] = 'X';
        mem[allocation_size - 1] = 'X';

        if (check_exected_size(chunk, metadata.size)) {
            print_test(test_name, TEST_FAIL, "invalid chunk size");
            printf("failed at chunk i: %d\n", i);
            return;
        }

        if (metadata.in_use != 1) {
            print_test(test_name, TEST_FAIL, "chunk should be in use");
            printf("failed at chunk i: %d\n", i);
            return;
        }
    }

    for (int i=0; i < n_allocations; i++) {
        free(pointers[i]);
        chunk_header *chunk = get_chunk_header(pointers[i]);

        if (!is_chunk_free(chunk)) {
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void free_list_startegy_test_3() {
    char *test_name = "free list strategy test 3";

    size_t allocation_size = 1024;
    size_t n_allocations = 1024;
    void *pointers[n_allocations];

    for (int i=0; i < n_allocations; i++) {
        char *mem = malloc(allocation_size);
        pointers[i] = mem;
        chunk_header *chunk = get_chunk_header(mem);
        size_metadata metadata = malloc_read_size_metadata(chunk);

        mem[0] = 'X';
        mem[allocation_size - 1] = 'X';

        if (check_exected_size(chunk, metadata.size)) {
            
            print_test(test_name, TEST_FAIL, "invalid chunk size");
            printf("failed at chunk i: %d\n", i);
            return;
        }

        if (metadata.in_use != 1) {
            print_test(test_name, TEST_FAIL, "chunk should be in use");
            printf("failed at chunk i: %d\n", i);
            return;
        }
    }

    for (int i=0; i < n_allocations; i++) {
        free(pointers[i]);
        chunk_header *chunk = get_chunk_header(pointers[i]);

        if (!is_chunk_free(chunk)) {
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void free_list_startegy_test_4() {
    char *test_name = "free list strategy test 4";
    size_t sizes[10] = {1475, 2674, 4678, 3617, 2626, 64642, 23642, 35428, 15267, 8364};
    size_t n_allocations = 10;
    char *pointers[n_allocations];

    for (int i=0; i < n_allocations; i++) {
        pointers[i] = malloc(sizes[i]);

        pointers[i][0] = 'X';
        pointers[i][sizes[i] - 1] = 'X';

        chunk_header *chunk = get_chunk_header(pointers[i]);
        size_metadata metadata = malloc_read_size_metadata(chunk);
        if (check_exected_size(chunk, metadata.size)) {
            print_test(test_name, TEST_FAIL, "invalid chunk size");
            return;
        }

        if (metadata.in_use != 1) {
            print_test(test_name, TEST_FAIL, "chunk should be in use");
            return;
        }

    }

    for (int i=0; i < n_allocations; i += 2) {
        free(pointers[i]);
        chunk_header *chunk = get_chunk_header(pointers[i]);

        if (!is_chunk_free(chunk)) {
            printf("chunk: %p\n", chunk);
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            //return;
        }
    }

    for (int i=0; i < n_allocations; i += 2) {
        pointers[i] = malloc(sizes[i]);

        pointers[i][0] = 'X';
        pointers[i][sizes[i] - 1] = 'X';

        chunk_header *chunk = get_chunk_header(pointers[i]);
        size_metadata metadata = malloc_read_size_metadata(chunk);
        if (check_exected_size(chunk, metadata.size)) {
            print_test(test_name, TEST_FAIL, "invalid chunk size");
            printf("failed at chunk i: %d\n", i);
            return;
        }

        if (metadata.in_use != 1) {
            print_test(test_name, TEST_FAIL, "chunk should be in use");
            printf("failed at chunk i: %d\n", i);
            return;
        }
    }

    for (int i=1; i < n_allocations; i += 2) {
        free(pointers[i]);
        chunk_header *chunk = get_chunk_header(pointers[i]);

        if (!is_chunk_free(chunk)) {
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            return;
        }
    }

    for (int i=1; i < n_allocations; i += 2) {
        pointers[i] = malloc(sizes[i]);

        pointers[i][0] = 'X';
        pointers[i][sizes[i] - 1] = 'X';

        chunk_header *chunk = get_chunk_header(pointers[i]);
        size_metadata metadata = malloc_read_size_metadata(chunk);
        if (check_exected_size(chunk, metadata.size)) {
            print_test(test_name, TEST_FAIL, "invalid chunk size");
            printf("failed at chunk i: %d\n", i);
            return;
        }

        if (metadata.in_use != 1) {
            print_test(test_name, TEST_FAIL, "chunk should be in use");
            printf("failed at chunk i: %d\n", i);
            return;
        }
    }

    for (int i=0; i < n_allocations; i++) {
        free(pointers[i]);
        chunk_header *chunk = get_chunk_header(pointers[i]);

        if (!is_chunk_free(chunk)) {
            print_test(test_name, TEST_FAIL, "chunk should not be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void test_run_all_free_list_strategy_tests() {
    free_list_strategy_test_1();
    free_list_startegy_test_2();
    free_list_startegy_test_3();
    free_list_startegy_test_4();
    free_list_strategy_test_5();

    printf("User allcoated memory: %ld\n", user_allocated_memory);
    printf("Malloc allcoated memory: %ld\n", malloc_allocated_memory);
    printf("Malloc freed memory: %ld\n", malloc_freed_memory);
}
