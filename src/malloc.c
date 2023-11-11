#include <stdio.h>

#include "libft_malloc/libft_malloc.h"
#include "malloc_utils.h"

static malloc_block *memory_blocks = NULL;

void *malloc(size_t size) {

    printf("Custom malloc called\n");

    if (memory_blocks == NULL)
        memory_blocks = create_initial_allocation();
    return 0;
}

void free(void *ptr) {
    printf("free function called\n");
}

void *realloc(void *ptr, size_t size) {
    printf("realloc function called\n");
    return 0;
}

void show_alloc_mem() {
    printf("=== ALLOCATED MEMORY ===\n");

    malloc_block *current_block = memory_blocks;

    while (current_block != NULL) {
        printf("--- BLOCK ---\n");
        printf("address: %p -> %llu\n", current_block, (uint64_t)current_block);
        printf("in_use: %d\n", current_block->in_use);
        printf("length: %zu\n", current_block->length);
        //printf("data: %c\n", GET_BLOCK_DATA(current_block));

        printf("next: %p -> %llu\n", current_block->next, (uint64_t)current_block->next);

        current_block = (malloc_block *)current_block->next;
    }

    printf("========================\n");
}