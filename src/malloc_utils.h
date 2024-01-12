#ifndef malloc_utils
#define malloc_utils

#include "libft_malloc/libft_malloc.h"

#define N_SIZES 2
#define TINY_ZONE_SIZE 16
#define SMALL_ZONE_SIZE 128
#define N_BLOCKS_PER_SIZE 10
#define TOTAL_BLOCKS (N_SIZES * N_BLOCKS_PER_SIZE)

typedef struct s_malloc_block {
    int in_use;
    size_t size;
    struct s_malloc_block *next;
    void *data_location;
} malloc_block;

extern malloc_block *g_memory_block;

malloc_block *create_initial_allocation();
void *create_new_mapping(size_t size);
void init_memory_block(malloc_block *block, size_t size, void *data);
void free_memory_block(malloc_block *block, malloc_block *previous_block);
void *find_memory_block(size_t size);
void *create_custom_allocation(size_t size);
void print_malloc_block(malloc_block *block);

#endif
