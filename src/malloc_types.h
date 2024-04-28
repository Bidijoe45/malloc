#ifndef malloc_types
#define malloc_types

#define N_CHUNKS_IN_TINY_ZONE 128;

enum zone_type {
    TINY_ZONE = 0,
    SMALL_ZONE = 1,
    LARGE_ZONE = 2
};

typedef struct chunk_header {
    size_t size;
    struct chunk_header *prev_chunk;
    struct chunk_header *next_chunk;
} chunk_header;

typedef struct memory_zone {
    struct memory_zone *next_zone;
} memory_zone;

typedef struct malloc_data {
    size_t tiny_zone_size;
    size_t small_zone_size;
    memory_zone *zones[3];
} malloc_data;

#endif