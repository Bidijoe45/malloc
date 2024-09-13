#ifndef malloc_utils
#define malloc_utils

#include "malloc_types.h"

void initialize_malloc();

size_metadata malloc_read_size_metadata(chunk_header *chunk);
size_t malloc_size_metadata_to_size_t(size_metadata metadata);
size_metadata malloc_size_t_to_metadata(size_t size);
void malloc_write_size_metadata(chunk_header *chunk, size_metadata metadata);
void malloc_initialize_chunk(chunk_header *chunk);
chunk_header *malloc_get_last_chunk(chunk_header *chunk);
chunk_header *get_chunk_header(void *ptr);

void hexdump(void *mem, unsigned int len);
void visualize_memory(void);
void print_chunk_list(chunk_header *chunk);
void print_zone_list(memory_zone *zone);

void malloc_print(char *str);
void malloc_print_size(size_t n);
void malloc_print_address_hex(void *ptr);
void malloc_print_hex_number(size_t n);

#endif
