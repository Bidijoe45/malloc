#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "malloc_state.h"
#include "malloc_utils.h"
#include "zone_manager.h"
#include "pool_strategy.h"

void initialize_malloc() {
    //FIXME: Borrar esto al final!
    printf("Malloc initialization\n");

    //FIXME: Cada size deberia iniciarlo la propia estrategia
    g_malloc_data.sizes[SMALL_ZONE].zone = getpagesize() * 4;
    g_malloc_data.sizes[LARGE_ZONE].zone = 0;

    g_malloc_data.zones_list[TINY_ZONE] = NULL;
    g_malloc_data.zones_list[SMALL_ZONE] = NULL;
    g_malloc_data.zones_list[LARGE_ZONE] = NULL;

    pool_strategy_initialize();

    //FIXME: Borrar esto al final!
    printf("page size: %d\n", getpagesize());
    printf("tiny zone size: %zu\n", g_malloc_data.sizes[TINY_ZONE].zone);
    printf("tiny zone chunk size: %zu\n", g_malloc_data.sizes[TINY_ZONE].chunk);
    printf("tiny zone payload size: %zu\n", g_malloc_data.sizes[TINY_ZONE].payload);

    printf("small zone size: %zu\n", g_malloc_data.sizes[SMALL_ZONE].zone);
    printf("small zone chunk size: %zu\n", g_malloc_data.sizes[SMALL_ZONE].chunk);
    printf("small zone payload size: %zu\n", g_malloc_data.sizes[SMALL_ZONE].payload);
    printf("\n");
}

size_metadata malloc_read_size_metadata(chunk_header *chunk) {
    size_metadata metadata;
    size_t size = chunk->size;

    metadata.in_use = size & 1;
    metadata.size = size & ~7u;

    return metadata;
}

void malloc_write_size_metadata(chunk_header *chunk, size_metadata metadata) {
    chunk->size = (metadata.size & ~7) | (metadata.in_use & 1);
}

void malloc_initialize_chunk(chunk_header *chunk) {
    chunk->prev_chunk = NULL;
    chunk->next_chunk = NULL;
    chunk->size = 0;
}

chunk_header *malloc_get_last_chunk(chunk_header *chunk) {
    if (chunk == NULL)
        return NULL;

    chunk_header *current_chunk = chunk;

    while (current_chunk->next_chunk != NULL) {
        current_chunk = current_chunk->next_chunk;
    }

    return current_chunk;
}

chunk_header *get_chunk_header(void *ptr) {
    return (chunk_header *)(ptr - sizeof(size_t));
}

void hexdump(void *mem, unsigned int len)
{
    unsigned int i, j, k;
    
    for(i = 0; i < len + ((len % 16) ? (16 - len % 16) : 0); i++) {
        if(i % 16 == 0) {
            printf("0x%06x: ", i);
            k = 0;
        }

        if (k == 4 || k == 12)
            printf(" ");

        if (k == 8)
            printf("   ");
        k++;

        if(i < len) {
            printf("%02x ", 0xFF & ((char*)mem)[i]);
        }
        else {
            printf("   ");
        }
        
        if(i % 16 == (16 - 1)) {
            for(j = i - (16 - 1); j <= i; j++) {
                if(j >= len) {
                    putchar(' ');
                }
                else if(isprint(((char*)mem)[j])) {
                    putchar(0xFF & ((char*)mem)[j]);        
                }
                else {
                    putchar('.');
                }
            }
            putchar('\n');
        }
    }
}