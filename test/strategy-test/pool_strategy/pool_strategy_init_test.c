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
       char *test_name = "test pool strategy initizalization";

    char *mem = malloc(1);
    chunk_header *chunk = get_chunk_header(mem);
    size_t total_chunks = (g_malloc_data.sizes[TINY_ZONE].zone - sizeof(memory_zone)) / g_malloc_data.sizes[TINY_ZONE].chunk;

    for (int i=0; i < total_chunks; i++) {
        size_metadata metadata = malloc_read_size_metadata(chunk);

        if (metadata.size != g_malloc_data.sizes[TINY_ZONE].chunk) {
            print_test(test_name, TEST_FAIL, "Invalid initialization");
            return 1;
        }

        chunk = chunk->next_chunk;
    }

    free(mem);
    print_test(test_name, TEST_OK, NULL);
}