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

void test_pool_strategy_1(size_t *sizes_to_test, size_t n_sizes) {
    char *test_name = "test pool strategy 1";
    
    for (size_t i=0; i < n_sizes; i++) {

        int size = sizes_to_test[i];
        char *mem = malloc(size);

        bool valid = false;

        valid = check_return_address_size(mem, g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(mem, 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }

        write_dummy_data(mem, size);
        
        free(mem);
        valid = check_metadata_in_use(mem, 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Free: metadata is in use, should not be");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void test_pool_strategy_2(size_t *sizes_to_test, size_t n_sizes) {
    char *test_name = "test pool strategy 2";
    
    char *pointers[n_sizes];
    bool valid = false;

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address size is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers[i], size);
        free(pointers[i]);
        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Free: metadata is in use, should not be");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void test_pool_strategy_3(size_t *sizes_to_test, size_t n_sizes) {
    char *test_name = "test pool strategy 3";
    
    char *pointers[n_sizes];
    bool valid = false;

    size_t half_sizes = n_sizes / 2;
    size_t remaining_sizes = n_sizes - half_sizes;

    for (size_t i=0; i < half_sizes; i++) {
        int size = sizes_to_test[i];
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < half_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers[i], size);
        free(pointers[i]);
        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Freed address is invalid");
            return;
        }
    }

    for (size_t i=0; i < remaining_sizes; i++) {
        int size = sizes_to_test[i];
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < remaining_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Free: metadata is in use, should not be");
            return;
        }
    }

        for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is in use, should not be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void test_pool_strategy_4(size_t *sizes_to_test, size_t n_sizes) {
    char *test_name = "test pool strategy 4";
    
    char *pointers1[n_sizes];
    char *pointers2[n_sizes];
    bool valid = false;

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers1[i] = malloc(size);
        valid = check_return_address_size(pointers1[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers1[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers2[i] = malloc(size);
        valid = check_return_address_size(pointers2[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers2[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers1[i], size);
        free(pointers1[i]);

        valid = check_metadata_in_use(pointers1[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is in use, should not be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers2[i], size);
        free(pointers2[i]);

        valid = check_metadata_in_use(pointers1[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is in use, should not be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void test_pool_strategy_5(size_t *sizes_to_test, size_t n_sizes) {
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
            return;
        }

        valid = check_metadata_in_use(pointers1[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers2[i] = malloc(size);
        valid = check_return_address_size(pointers2[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers2[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers3[i] = malloc(size);
        valid = check_return_address_size(pointers3[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers3[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        pointers4[i] = malloc(size);
        valid = check_return_address_size(pointers4[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers4[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers1[i], size);
        free(pointers1[i]);

        valid = check_metadata_in_use(pointers1[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers2[i], size);
        free(pointers2[i]);

        valid = check_metadata_in_use(pointers2[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers3[i], size);
        free(pointers3[i]);

        valid = check_metadata_in_use(pointers3[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_sizes; i++) {
        int size = sizes_to_test[i];
        write_dummy_data(pointers4[i], size);
        free(pointers4[i]);

        valid = check_metadata_in_use(pointers4[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void test_pool_strategy_6() {
    char *test_name = "test pool strategy 6";
    size_t n_pointers = 1024;
    char *pointers[n_pointers];
    int size = g_malloc_data.sizes[TINY_ZONE].payload;
    bool valid = false;

    for (size_t i=0; i < n_pointers; i++) {
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_pointers; i++) {

        write_dummy_data(pointers[i], size);
        free(pointers[i]);
        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void test_pool_strategy_7() {
    char *test_name = "test pool strategy 7";
    size_t n_pointers = 1024;
    char *pointers[n_pointers];
    int size = g_malloc_data.sizes[TINY_ZONE].payload;
    bool valid = false;

    for (size_t i=0; i < n_pointers; i++) {
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_pointers; i += 2) {
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=1; i < n_pointers; i += 2) {
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void test_pool_strategy_8() {
    char *test_name = "test pool strategy 8";
    size_t n_pointers = 1024;
    char *pointers[n_pointers];
    int size = g_malloc_data.sizes[TINY_ZONE].payload;
    bool valid = false;

    for (size_t i=0; i < n_pointers; i++) {
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_pointers; i += 2) {
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_pointers; i += 2) {
        pointers[i] = malloc(size);
        valid = check_return_address_size(pointers[i], g_malloc_data.sizes[TINY_ZONE].chunk);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Returned address is invalid");
            return;
        }

        valid = check_metadata_in_use(pointers[i], 1);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=1; i < n_pointers; i += 2) {
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    for (size_t i=0; i < n_pointers; i += 2) {
        write_dummy_data(pointers[i], size);
        free(pointers[i]);

        valid = check_metadata_in_use(pointers[i], 0);
        if (!valid) {
            print_test(test_name, TEST_FAIL, "Metadata: is not in use, should be in use");
            return;
        }
    }

    print_test(test_name, TEST_OK, NULL);
}

void pool_strategy_zone_test_1() {
    char *test_name = "test pool strategy zone 1";
    char *mem = malloc(1);
    chunk_header *chunk = get_chunk_header(mem);
    size_metadata meta = malloc_read_size_metadata(chunk);

    if (meta.size != 128) {
        print_test(test_name, TEST_FAIL, "Metadata: incorrect size");
        return;
    }

    if (meta.in_use == 0) {
        print_test(test_name, TEST_FAIL, "Metadata: in_use is 0, should be 1");
        return;
    }

    memory_zone *zone_header = (memory_zone *)((char*)chunk - sizeof(memory_zone));
    if (zone_header->next_zone != NULL) {
        print_test(test_name, TEST_FAIL, "zone->next is not null");
        return;
    }

    size_t zone_size = get_zone_type_size(TINY_ZONE);
    if (zone_size != g_malloc_data.sizes[TINY_ZONE].zone) {
        print_test(test_name, TEST_FAIL, "Invalid zone size");
        return;
    }

    memory_zone *new_zone_1 = create_zone_by_type(TINY_ZONE);
    
    if (new_zone_1->next_zone != zone_header) {
        print_test(test_name, TEST_FAIL, "new created zone is not pointing to old zone");
        return;
    }

    memory_zone *new_zone_2 = create_zone_by_type(TINY_ZONE);
    
    if (new_zone_2->next_zone != new_zone_1 && new_zone_1->next_zone != zone_header) {
        print_test(test_name, TEST_FAIL, "new created zone is not pointing to old zone 2");
        return;
    }

    free(mem);

    print_test(test_name, TEST_OK, NULL);
}

void pool_strategy_chunk_test_1() {
    char *test_name = "test pool strategy chunk 1";

    char *mem = malloc(1);
    chunk_header *chunk = get_chunk_header(mem);

    if (chunk->next_chunk != g_malloc_data.chunks_list[TINY_ZONE]) {
        print_test(test_name, TEST_FAIL, "first allocated chunk is not the first in list");
        return;
    }

    free(mem);

    if (chunk != g_malloc_data.chunks_list[TINY_ZONE]) {
        print_test(test_name, TEST_FAIL, "freed chunk should be the first in list");
        return;
    }

    print_test(test_name, TEST_OK, NULL);
}

void pool_strategy_chunk_test_2() {
    char *test_name = "test pool strategy chunk 2";

    int payload_size = g_malloc_data.sizes[TINY_ZONE].payload;
    char *mem = malloc(1);
    chunk_header *chunk = get_chunk_header(mem);
    memory_zone *zone_header = (memory_zone *)((char*)chunk - sizeof(memory_zone));

    if (chunk->next_chunk != g_malloc_data.chunks_list[TINY_ZONE]) {
        print_test(test_name, TEST_FAIL, "first allocated chunk is not the first in list");
        return;
    }

    free(mem);

    if (chunk != g_malloc_data.chunks_list[TINY_ZONE]) {
        print_test(test_name, TEST_FAIL, "freed chunk should be the first in list");
        return;
    }

    int n_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - sizeof(memory_zone)) / g_malloc_data.sizes[TINY_ZONE].chunk;

    char *pointers[n_chunks];
    for (int i=0; i < n_chunks; i++) {
        char *ptr = malloc(payload_size);
        
        ptr[payload_size - 1] = 'X';
        pointers[i] = ptr;
    }

    for (int i=0; i < n_chunks; i++) {
        free(pointers[i]);
    }

    print_test(test_name, TEST_OK, NULL);
}

void pool_strategy_n_chunks_test() {
    char *test_name = "test pool strategy n chunks";

    char *mem = malloc(1);
    free(mem);
    size_t payload_size = g_malloc_data.sizes[TINY_ZONE].payload;
    size_t total_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - sizeof(memory_zone)) / g_malloc_data.sizes[TINY_ZONE].chunk;

    char *pointers[total_chunks];

    for (int i=0; i < total_chunks; i++) {
        char *chunk = malloc(payload_size);
        chunk[0] = 'X';
        chunk[g_malloc_data.sizes[TINY_ZONE].payload - 1] = 'X';
        pointers[i] = chunk;
    }

    for (int i=0; i < total_chunks; i++) {
        free(pointers[i]);
    }

    print_test(test_name, TEST_OK, NULL);
}

void pool_strategy_initialization_test() {
    char *test_name = "test pool strategy initizalization";

    char *mem = malloc(1);
    chunk_header *chunk = get_chunk_header(mem);
    size_t total_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - sizeof(memory_zone)) / g_malloc_data.sizes[TINY_ZONE].chunk;

    for (int i=0; i < total_chunks; i++) {
        size_metadata metadata = malloc_read_size_metadata(chunk);

        if (metadata.size != g_malloc_data.sizes[TINY_ZONE].chunk) {
            print_test(test_name, TEST_FAIL, "Invalid initialization");
            return;
        }

        chunk = chunk->next_chunk;
    }

    free(mem);
    print_test(test_name, TEST_OK, NULL);
}

long get_mem_usage() {
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

void test_run_all_pool_strategy_tests() {
    long initial_mem = get_mem_usage();

    size_t sizes_to_test[8] = {1, 10, 20, 30, 50, 80, 100, g_malloc_data.sizes[TINY_ZONE].chunk};

    pool_strategy_zone_test_1();
    pool_strategy_initialization_test();
    pool_strategy_n_chunks_test();
    pool_strategy_chunk_test_1();
    pool_strategy_chunk_test_2();
    test_pool_strategy_1(sizes_to_test, 8);
    test_pool_strategy_2(sizes_to_test, 8);
    test_pool_strategy_3(sizes_to_test, 8);
    test_pool_strategy_4(sizes_to_test, 8);
    test_pool_strategy_5(sizes_to_test, 8);
    test_pool_strategy_6();
    test_pool_strategy_7();
    test_pool_strategy_8();

    printf("\n");

    long final_mem = get_mem_usage();
    printf("Initial memory usage: %ld \n", initial_mem);
    printf("Final memory usage: %ld \n", final_mem - initial_mem);
    printf("User allcoated memory: %ld\n", user_allocated_memory);
    printf("Malloc allcoated memory: %ld\n", malloc_allocated_memory);
    printf("Malloc freed memory: %ld\n", malloc_freed_memory);
}