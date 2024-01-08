#ifndef malloc_utils
#define malloc_utils

#include "libft_malloc/libft_malloc.h"

#define N_SIZES 1
#define TINY_ALLOCATION_SIZE 16
#define SMALL_ALLOCATION_SIZE 100
#define LARGE_ALLOCATION_SIZE 1000
#define N_BLOCKS_PER_SIZE 10
#define TOTAL_BLOCKS (N_SIZES * N_BLOCKS_PER_SIZE)

typedef struct s_malloc_block {
    int in_use;
    size_t size;
    struct s_malloc_block *next;
    void *data;
} malloc_block;

extern malloc_block *g_memory_block;

malloc_block *create_initial_allocation();
void *create_new_mapping(size_t size);
void init_memory_block(malloc_block *block, size_t size, void *data);
void free_memory_block(malloc_block *block, malloc_block *previous_block);
void *find_memory_block(size_t size);
void *create_custom_allocation(size_t size);

#endif
