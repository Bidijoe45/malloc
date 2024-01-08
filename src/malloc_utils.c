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

//FIXME: Refactor this function, will be very big
malloc_block *create_initial_allocation() {
    size_t tiny_size = N_BLOCKS_PER_SIZE * sizeof(malloc_block) + TINY_ALLOCATION_SIZE * N_BLOCKS_PER_SIZE;
    size_t medium_size = 0;
    size_t large_size = 0;
    size_t total_size = tiny_size + medium_size + large_size;

    malloc_block *memory_block = create_new_mapping(total_size);
    if (memory_block == MAP_FAILED) {
        printf("ERROR: creating map\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }

    char *data_block = (char *)memory_block + N_BLOCKS_PER_SIZE * sizeof(malloc_block);

    //Initialize tiny memory blocks
    malloc_block *current_block = memory_block;
    malloc_block *previous_block = NULL;
    for (size_t i=0; i < N_BLOCKS_PER_SIZE; i++) {
        init_memory_block(current_block, TINY_ALLOCATION_SIZE, data_block);
        data_block += TINY_ALLOCATION_SIZE;
        previous_block = current_block;

        if (i == N_BLOCKS_PER_SIZE - 1)
            break;

        current_block++;
        previous_block->next = current_block;
    }

    //TODO: Initialize medium blocks
    //TODO: Initialize large blocks

    return memory_block;
}

void init_memory_block(malloc_block *block, size_t size, void *data) {
    block->in_use = 0;
    block->size = size;
    block->data = data;
    block->next = NULL;

    //TODO: Delete this from here
    char *data_block = block->data;
    strcpy(data_block, "123456789");
}

void free_memory_block(malloc_block *block, malloc_block *previous_block) {
    block->in_use = 0;

    if (block->size > LARGE_ALLOCATION_SIZE) {
        delete_mapping(block, block->size);

        if (block->next == NULL)
            previous_block->next = NULL;
        else
            previous_block->next = block->next;
    }
}

void *find_memory_block(size_t size) {
    malloc_block *current_block = g_memory_block;
    while (current_block != NULL) {
        
        if (current_block->in_use == 0 && size <= current_block->size) {
            current_block->in_use = 1;
            return current_block->data;
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

    return memory_block->data;
}