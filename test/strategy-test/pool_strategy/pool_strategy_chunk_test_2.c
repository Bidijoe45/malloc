#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/resource.h>

#include "malloc_test.h"

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "zone_manager.h"

int main() {
    char *test_name = "test pool strategy chunk 2";

    int payload_size = g_malloc_data.sizes[TINY_ZONE].payload;
    char *mem = malloc(1);
    chunk_header *chunk = get_chunk_header(mem);
    memory_zone *zone_header = (memory_zone *)((char*)chunk - sizeof(memory_zone));

    if (chunk->next_chunk != g_malloc_data.chunks_list[TINY_ZONE]) {
        print_test(test_name, TEST_FAIL, "first allocated chunk is not the first in list");
        return 1;
    }

    free(mem);

    if (chunk != g_malloc_data.chunks_list[TINY_ZONE]) {
        print_test(test_name, TEST_FAIL, "freed chunk should be the first in list");
        return 1;
    }

    int n_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - sizeof(memory_zone)) / g_malloc_data.sizes[TINY_ZONE].chunk;

    char *pointers[n_chunks];
    for (int i=0; i < n_chunks; i++) {
        char *ptr = malloc(payload_size);
        
        ptr[payload_size - 1] = 'X';
        pointers[i] = ptr;
    }

    for (int i=0; i < n_chunks; i++) {
        free(pointers[i]);
    }

    print_test(test_name, TEST_OK, NULL);
}