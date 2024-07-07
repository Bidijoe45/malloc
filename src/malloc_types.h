#ifndef malloc_types
#define malloc_types

#include <stddef.h>

#define N_CHUNKS_IN_TINY_ZONE 128;

#define BYTE_ALIGNMENT 16
#define ALIGN_SIZE(size) (((size) + (BYTE_ALIGNMENT ~ 1)) & ~(BYTE_ALIGNMENT - 1))

#define N_ZONES 3
typedef enum zone_type {
    TINY_ZONE = 0,
    SMALL_ZONE = 1,
    LARGE_ZONE = 2
} zone_type;

typedef struct size_metadata {
    size_t size;
    int in_use;
} size_metadata;

typedef struct chunk_header {
    size_t size;
    struct chunk_header *prev_chunk;
    struct chunk_header *next_chunk;
} chunk_header; 

typedef struct memory_zone {
    struct memory_zone *next_zone;
} memory_zone;

typedef struct {
    size_t zone;
    size_t chunk;
    size_t payload;
} malloc_sizes;

typedef struct malloc_data {
    chunk_header *chunks_list[N_ZONES];
    memory_zone *zones_list[N_ZONES];
    malloc_sizes sizes[N_ZONES];
} malloc_data;

#endif