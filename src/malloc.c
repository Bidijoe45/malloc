#include <stdio.h>

#include "libft_malloc/libft_malloc.h"
#include "malloc_utils.h"

malloc_block *g_memory_block = NULL;

void *malloc(size_t size) {
    if (g_memory_block == NULL)
        g_memory_block = create_initial_allocation();
    
    void *block = find_memory_block(size);
    if (block == NULL)
        block = create_custom_allocation(size);

    return block;
}

void free(void *ptr) {
    malloc_block *current_block = g_memory_block;
    malloc_block *previous_block = NULL;

    while (current_block != NULL) {
        
        if (current_block->data_location == ptr) {
            free_memory_block(current_block, previous_block);
            return;
        }

        previous_block = current_block;
        current_block = current_block->next;
    }

}

void *realloc(void *ptr, size_t size) {
    return 0;
}

void show_alloc_mem() {
    printf("=== ALLOCATED MEMORY ===\n");

    malloc_block *current_block = g_memory_block;
    size_t total_blocks = 0;

    size_t i = 0;
    while (current_block != NULL) {
        printf("--- BLOCK %zu ---\n", i++);
        print_malloc_block(current_block);
        printf("--- --- --- ---\n");
        printf("block address diff: %llu\n", (u_int64_t)current_block->next - (uint64_t)current_block);

        if (current_block->next != NULL)
            printf("data address diff: %llu\n", (u_int64_t)current_block->next->data_location - (uint64_t)current_block->data_location);

        total_blocks++;

        current_block = current_block->next;
    }

    printf("========================\n");
    printf("total blocks: %zu\n", total_blocks);
    printf("========================\n");

}