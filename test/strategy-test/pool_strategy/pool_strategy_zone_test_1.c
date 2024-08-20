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
     char *test_name = "test pool strategy zone 1";
    char *mem = malloc(1);
    chunk_header *chunk = get_chunk_header(mem);
    size_metadata meta = malloc_read_size_metadata(chunk);

    if (meta.size != g_malloc_data.sizes[TINY_ZONE].chunk) {
        print_test(test_name, TEST_FAIL, "Metadata: incorrect size");
        return 1;
    }

    if (meta.in_use == 0) {
        print_test(test_name, TEST_FAIL, "Metadata: in_use is 0, should be 1");
        return 1;
    }

    memory_zone *zone_header = (memory_zone *)((char*)chunk - SIZE_T_SIZE);
    if (zone_header->next_zone != NULL) {
        print_test(test_name, TEST_FAIL, "zone->next is not null");
        return 1;
    }

    size_t zone_size = zone_mgr_get_type_size(TINY_ZONE);
    if (zone_size != g_malloc_data.sizes[TINY_ZONE].zone) {
        print_test(test_name, TEST_FAIL, "Invalid zone size");
        return 1;
    }

    memory_zone *new_zone_1 = zone_mgr_create(TINY_ZONE, g_malloc_data.sizes[TINY_ZONE].zone);
    
    if (new_zone_1->next_zone != zone_header) {
        print_test(test_name, TEST_FAIL, "new created zone is not pointing to old zone");
        return 1;
    }

    memory_zone *new_zone_2 = zone_mgr_create(TINY_ZONE, g_malloc_data.sizes[TINY_ZONE].zone);
    
    if (new_zone_2->next_zone != new_zone_1 && new_zone_1->next_zone != zone_header) {
        print_test(test_name, TEST_FAIL, "new created zone is not pointing to old zone 2");
        return 1;
    }

    free(mem);

    print_test(test_name, TEST_OK, NULL);
}