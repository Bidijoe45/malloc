#include <stdio.h>
#include <stdbool.h>

#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"
#include "malloc_utils.h"

void *malloc(size_t size) {
    static bool initialized = false;
    if (initialized == false) {
        initialize_malloc();
        initialized = true;
    }

    if (size <= g_malloc_data.tiny_zone_payload_size) {

    }

    return 0;
}

void free(void *ptr) {

}

void *realloc(void *ptr, size_t size) {
    return 0;
}

void show_alloc_mem() {
    printf("=== ALLOCATED MEMORY ===\n");

    print_all();

    printf("=== ================ ===\n");
}