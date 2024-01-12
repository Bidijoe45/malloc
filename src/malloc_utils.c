#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "libft_malloc/libft_malloc.h"
#include "malloc_utils.h"

void *create_new_mapping(size_t size) {
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
}

void delete_mapping(void *map, size_t size) {
    munmap(map, size);
}

size_t calculate_allocation_size(size_t size) {
    return N_BLOCKS_PER_SIZE * sizeof(malloc_block) + size * N_BLOCKS_PER_SIZE;
}

void init_memory_block(malloc_block *block, size_t size, void *data_location) {
    block->in_use = 0;
    block->size = size;
    block->data_location = data_location;
    block->next = NULL;

    //TODO: Delete this from here, is only for debugging
    char *data_block = block->data_location;
    strcpy(data_block, "123456789");
}

malloc_block *create_initial_allocation() {
    size_t tiny_size = calculate_allocation_size(TINY_ZONE_SIZE);
    size_t small_size = calculate_allocation_size(SMALL_ZONE_SIZE);

    malloc_block *memory_block = create_new_mapping(tiny_size + small_size);
    if (memory_block == MAP_FAILED) {
        printf("ERROR: creating map\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    //TODO: refactor this
    char *data_block = (char *)memory_block + sizeof(malloc_block) * TOTAL_BLOCKS;
    malloc_block *current_block = memory_block;
    malloc_block *previous_block = NULL;
    
    for (size_t i=0; i < N_BLOCKS_PER_SIZE; i++) {
        init_memory_block(current_block, TINY_ZONE_SIZE, data_block);
        data_block += TINY_ZONE_SIZE;
        previous_block = current_block;

        if (i == N_BLOCKS_PER_SIZE - 1)
            break;

        current_block++;
        previous_block->next = current_block;
    }

    for (size_t i=0; i < N_BLOCKS_PER_SIZE; i++) {
        init_memory_block(current_block, SMALL_ZONE_SIZE, data_block);
        data_block += SMALL_ZONE_SIZE;
        previous_block = current_block;

        if (i == N_BLOCKS_PER_SIZE - 1)
            break;

        current_block++;
        previous_block->next = current_block;
    }

    return memory_block;
}

void free_memory_block(malloc_block *block, malloc_block *previous_block) {
    block->in_use = 0;
    
    if (block->size > SMALL_ZONE_SIZE) {
        if (block->next == NULL)
            previous_block->next = NULL;
        else
            previous_block->next = block->next;
        delete_mapping(block, block->size);
    }
}

void *find_memory_block(size_t size) {
    malloc_block *current_block = g_memory_block;
    while (current_block != NULL) {
        
        if (current_block->in_use == 0 && size <= current_block->size) {
            current_block->in_use = 1;
            return current_block->data_location;
        }

        current_block = current_block->next;
    }

    return NULL;
}

void *create_custom_allocation(size_t size) {
    malloc_block *memory_block = create_new_mapping(size);
    if (memory_block == MAP_FAILED) {
        printf("ERROR: creating map\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    char *data_block = (char *)memory_block + sizeof(malloc_block);
    init_memory_block(memory_block, size, data_block);

    malloc_block *current_block = g_memory_block;
    while (current_block->next != NULL)
        current_block = current_block->next;

    current_block->next = memory_block;

    return memory_block->data_location;
}

void print_malloc_block(malloc_block *block) {
    printf("address: %p -> %llu\n", block, (uint64_t)block);
    printf("in_use: %d\n", block->in_use);
    printf("size: %zu\n", block->size);
    printf("next: %p -> %llu\n", block->next, (uint64_t)block->next);
    printf("data_address: %p\n", block->data_location);
    printf("data: %s\n", (char *)block->data_location);
}