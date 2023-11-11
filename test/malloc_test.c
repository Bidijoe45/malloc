#include <stdio.h>
#include "libft_malloc/libft_malloc.h"

#include "malloc_test.h"

void print_test(int test_n, int result) {
    char *result_str = (result == 0) ? "OK" : "FAILED";
    printf("TEST %d %s\n", test_n, result_str);
}

int main() {
    test1();
}