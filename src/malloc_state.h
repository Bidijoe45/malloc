#ifndef malloc_state
#define malloc_state

#include "malloc_types.h"

extern malloc_data g_malloc_data;

//FIXME: Debug only
extern long user_allocated_memory;
extern long malloc_allocated_memory;
extern long malloc_freed_memory;

#endif