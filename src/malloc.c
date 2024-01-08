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
        
        if (current_block->data == ptr) {
            free_memory_block(current_block, previous_block);
            return;
        }

        previous_block = current_block;
        current_block = current_block->next;
    }

    printf("Cannot deallocate invalid memory block\n");
    exit(1);
}

void *realloc(void *ptr, size_t size) {
    return 0;
}

void show_alloc_mem() {
    printf("=== ALLOCATED MEMORY ===\n");

    malloc_block *current_block = g_memory_block;

    size_t i = 0;
    while (current_block != NULL) {
        printf("--- BLOCK %zu ---\n", i++);
        printf("address: %p -> %llu\n", current_block, (uint64_t)current_block);
        printf("in_use: %d\n", current_block->in_use);
        printf("size: %zu\n", current_block->size);
        printf("next: %p -> %llu\n", current_block->next, (uint64_t)current_block->next);
        printf("data_address: %p\n", current_block->data);
        printf("data: %s\n", (char *)current_block->data);
        printf("--- --- --- ---\n");

        printf("block address diff: %llu\n", (u_int64_t)current_block->next - (uint64_t)current_block);

        if (current_block->next != NULL)
            printf("data address diff: %llu\n", (u_int64_t)current_block->next->data - (uint64_t)current_block->data);

        current_block = current_block->next;
    }

    printf("========================\n");
}