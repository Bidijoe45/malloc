#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "libft_malloc/libft_malloc.h"
#include "malloc_utils.h"

void *create_new_mapping(size_t length) {
    return mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
}

malloc_block *create_initial_allocation() {
    size_t total_size = 10;
    //total_size += SMALL_ALLOCATION_SIZE * sizeof(malloc_block) * N_BLOCKS_PER_SIZE;
    //total_size += LARGE_ALLOCATION_SIZE * sizeof(malloc_block) * N_BLOCKS_PER_SIZE;

    malloc_block *map = create_new_mapping(total_size);
    if (map == MAP_FAILED) {
        printf("ERROR: creating map\n");
        printf("%s\n", strerror(errno));
        exit(1);
    }
    
   
    /*
    //Allocate small sizes
    for (size_t i=0; i < N_BLOCKS_PER_SIZE; i++) {
        set_memory_block(current_block, SMALL_ALLOCATION_SIZE);

        if (prev_block != NULL)
            prev_block->next = (struct malloc_block *)current_block;

        prev_block = current_block;
        current_block += GET_BLOCK_SIZE(TINY_ALLOCATION_SIZE);
    }

    //Allocate large sizes
    for (size_t i=0; i < N_BLOCKS_PER_SIZE; i++) {
        set_memory_block(current_block, LARGE_ALLOCATION_SIZE);
        current_block += GET_BLOCK_SIZE(LARGE_ALLOCATION_SIZE);

        if (prev_block != NULL)
            prev_block->next = (struct malloc_block *)current_block;

        prev_block = current_block;
        current_block += GET_BLOCK_SIZE(TINY_ALLOCATION_SIZE);
    }
    */

    return map;
}

void init_memory_block(malloc_block *block, size_t length) {
    block->in_use = 0;
    block->length = length;
    block->next = NULL;

}

void free_memory_block(malloc_block *block) {

}

void *find_memory_block(size_t length) {
    return NULL;
}
