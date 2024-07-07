#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

long get_mem_usage() {
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}


int main()
{
    long initial_mem = get_mem_usage();

    char *mem = malloc(20000);
    memset(mem, 'x', 20000);
    //char *mem2 = malloc(20000);
    //memset(mem2, 'x', 20000);

    //free(mem);
    //free(mem2);

    long final_mem = get_mem_usage();
    printf("initial memory usage: %ld\n", initial_mem);
    printf("Final memory usage: %ld \n", final_mem - initial_mem);
}
