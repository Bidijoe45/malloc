#include <stdio.h>
#include <string.h>

#include "malloc_test.h"

#include "malloc_utils.h"
#include "libft_malloc/libft_malloc.h"

/*
    Checks for:

*/
void test1() {

    print_test(1, TEST_FAIL, "Free did not set the in_use flag correctly");
    print_test(1, TEST_OK, NULL);

}