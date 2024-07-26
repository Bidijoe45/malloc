#include <stdio.h>

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"
#include "malloc_test.h"
#include "malloc_types.h"

void test_run_all_large_strategy_tests() {
    printf("User allcoated memory: %ld\n", user_allocated_memory);
    printf("Malloc allcoated memory: %ld\n", malloc_allocated_memory);
    printf("Malloc freed memory: %ld\n", malloc_freed_memory);
}