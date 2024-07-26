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