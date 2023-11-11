#ifndef malloc_utils
#define malloc_utils

#include "libft_malloc/libft_malloc.h"

#define TINY_ALLOCATION_SIZE 10
#define N_BLOCKS_PER_SIZE 10

typedef struct s_malloc_block {
    int in_use;
    size_t length;
    char *next;
} malloc_block;

malloc_block *create_initial_allocation();
void *create_new_mapping(size_t length);
void init_memory_block(malloc_block *block, size_t length);
void free_memory_block(malloc_block *block);
void *find_memory_block(size_t length);

#endif
