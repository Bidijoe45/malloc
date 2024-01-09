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

void create_memory_blocks(malloc_block *memory_map_start, char *data_block_start, size_t size) {
    char *data_block = data_block_start;
    malloc_block *current_block = memory_map_start;
    malloc_block *previous_block = NULL;
    
    for (size_t i=0; i < N_BLOCKS_PER_SIZE; i++) {
        init_memory_block(current_block, size, data_block_start);
        data_block += size;
        previous_block = current_block;

        if (i == N_BLOCKS_PER_SIZE - 1)
            break;

        current_block++;
        previous_block->next = current_block;
    }
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

    char *data_block_start = (char *)memory_block + N_BLOCKS_PER_SIZE * sizeof(malloc_block);
    create_memory_blocks(memory_block, data_block_start, TINY_ZONE_SIZE);
    memory_block += sizeof(malloc_block) * N_BLOCKS_PER_SIZE;
    data_block_start += TINY_ZONE_SIZE * N_BLOCKS_PER_SIZE;
    create_memory_blocks(memory_block, data_block_start, SMALL_ZONE_SIZE);

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