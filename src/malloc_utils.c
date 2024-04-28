#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "malloc_state.h"
#include "malloc_utils.h"
#include "zone_manager.h"

malloc_data g_malloc_data;

void initialize_malloc() {
    g_malloc_data.tiny_zone_size = getpagesize();
    g_malloc_data.small_zone_size = getpagesize() * 4;

    g_malloc_data.zones[TINY_ZONE] = NULL;
    g_malloc_data.zones[SMALL_ZONE] = NULL;
    g_malloc_data.zones[LARGE_ZONE] = NULL;

    g_malloc_data.zones[TINY_ZONE] = create_zone(NULL, g_malloc_data.tiny_zone_size);
    initialize_tiny_zone(g_malloc_data.zones[TINY_ZONE]);

    printf("tiny zone size: %zu\n", g_malloc_data.tiny_zone_size);
    printf("tiny zone chunk size: %zu\n", g_malloc_data.tiny_zone_chunk_size);
    printf("tiny zone payload size: %zu\n", g_malloc_data.tiny_zone_payload_size);
}

void print_all() {
    memory_zone *zone = g_malloc_data.zones[TINY_ZONE];
    chunk_header *chunk = (chunk_header *)(zone + 1);

    size_t total_chunks = 0;
    while (chunk) {
        printf("=== CHUNK ===\n");
        printf("chunk_ptr: %p\n", chunk);
        printf("chunk_size: %zu\n", chunk->size);
        printf("chunk_prev: %p\n", chunk->prev_chunk);
        printf("chunk_next: %p\n", chunk->next_chunk);

        chunk = chunk->next_chunk;
        total_chunks++;
    }

    printf("total chunks: %zu\n", total_chunks);
}

void hexdump(void *mem, unsigned int len)
{
        unsigned int i, j, k;
        
        for(i = 0; i < len + ((len % 16) ? (16 - len % 16) : 0); i++)
        {
                /* print offset */
                if(i % 16 == 0)
                {
                        printf("0x%06x: ", i);
                        k = 0;
                }

                if (k == 4 || k == 12)
                    printf(" ");

                if (k == 8)
                    printf("   ");
                k++;

                /* print hex data */
                if(i < len)
                {
                        printf("%02x ", 0xFF & ((char*)mem)[i]);
                }
                else /* end of block, just aligning for ASCII dump */
                {
                        printf("   ");
                }
                
                /* print ASCII dump */
                if(i % 16 == (16 - 1))
                {
                        for(j = i - (16 - 1); j <= i; j++)
                        {
                                if(j >= len) /* end of block, not really printing */
                                {
                                        putchar(' ');
                                }
                                else if(isprint(((char*)mem)[j])) /* printable char */
                                {
                                        putchar(0xFF & ((char*)mem)[j]);        
                                }
                                else /* other char */
                                {
                                        putchar('.');
                                }
                        }
                        putchar('\n');
                }
        }
}