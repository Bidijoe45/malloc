#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "malloc_state.h"
#include "malloc_utils.h"
#include "zone_manager.h"
#include "pool_strategy.h"
#include "free_list_strategy.h"
#include "large_strategy.h"

void initialize_malloc() {
    pool_strategy_initialize();
    fls_initialize();
    lgs_initialize();
}

size_t malloc_size_metadata_to_size_t(size_metadata metadata) {
    return (metadata.size & ~7) | (metadata.in_use & 1);
}

size_metadata malloc_size_t_to_metadata(size_t size) {
    size_metadata metadata;
    
    metadata.in_use = size & 1;
    metadata.size = size & ~7u;

    return metadata;
}

size_metadata malloc_read_size_metadata(chunk_header *chunk) {
    return malloc_size_t_to_metadata(chunk->size);
}

void malloc_write_size_metadata(chunk_header *chunk, size_metadata metadata) {
    chunk->size = malloc_size_metadata_to_size_t(metadata);
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
    return (chunk_header *)(ptr - SIZE_T_SIZE);
}

void malloc_print_hex_number(size_t n)
{
    if (n >= 16)
        malloc_print_hex_number(n / 16);
    
    char c = n % 16;
    c += c > 9 ? ('a' - 10) : '0';
    write(1, &c, 1);
}

void malloc_print_address_hex(void *ptr)
{
    uintptr_t p = (uintptr_t)ptr;
    write(1, "0x", 2);
    malloc_print_hex_number(p);
}

void malloc_print(char *str) {
    if (str == NULL)
        return;

    size_t str_size = strlen(str);
    write(1, str, str_size);
}

void malloc_print_size(size_t n)
{
	if (n / 10 > 0)
		malloc_print_size(n / 10);

	char c = (n % 10) + '0';
	write(1, &c, 1);
}