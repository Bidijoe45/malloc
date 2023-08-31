#include <stdio.h>

#include "malloc.h"

void free(void *ptr) {
    printf("free function called\n");
    printf("free function called\n");
}

void *malloc(size_t size) {
    printf("malloc function called\n");
    return 0;
}

void *realloc(void *ptr, size_t size) {
    printf("realloc function called\n");
    return 0;
}

void show_alloc_mem() {
    printf("show_alloc_mem function called\n");
}