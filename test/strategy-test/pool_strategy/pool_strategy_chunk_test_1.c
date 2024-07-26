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
    char *test_name = "test pool strategy chunk 1";

    char *mem = malloc(1);
    chunk_header *chunk = get_chunk_header(mem);

    if (chunk->next_chunk != g_malloc_data.chunks_list[TINY_ZONE]) {
        print_test(test_name, TEST_FAIL, "first allocated chunk is not the first in list");
        return 1;
    }

    free(mem);

    if (chunk != g_malloc_data.chunks_list[TINY_ZONE]) {
        print_test(test_name, TEST_FAIL, "freed chunk should be the first in list");
        return 1;
    }

    print_test(test_name, TEST_OK, NULL);
}