#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/resource.h>

#include "malloc_test.h"

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"
#include "malloc_state.h"

int main() {
   
    char *mem = malloc(98261);
    write_dummy_data(mem, 98261);

    char *mem2 = realloc(mem, 53209);
    write_dummy_data(mem2, 53209);

    char *mem3 = realloc(mem2, 60000);
    write_dummy_data(mem3, 60000);

    char *mem4 = realloc(mem3, 70000);
    write_dummy_data(mem4, 70000);

    char *mem5 = realloc(mem4, 100000);
    write_dummy_data(mem5, 100000);

    return 0;
}