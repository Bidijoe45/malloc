#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "invalid arguments :(\n");
        return 1;
    }
    
    int file = open(argv[1], 0);

    if (file == -1) {
        fprintf(stderr, "Invalid file");
        exit(1);
    }

    size_t n_allocations = atoi(argv[2]);
    char *allocations[n_allocations];

    int rd = 1;
    int i=0;
    while (rd > 0) {
        char c;
        char s_id[22];
        char s_size[22];
        char nl;

        memset(s_id, 0, 22);
        memset(s_size, 0, 22);
        
        s_id[21] = 0;
        s_size[21] = 0;

        rd = read(file, &c, 1);
        rd = read(file, &s_id, 21);
        rd = read(file, &s_size, 21);
        rd = read(file, &nl, 1);

        if (rd == 0)
            break;

        int id = atoi(s_id);
        int size = atoi(s_size);
        
        if (c == 'M') {
            fprintf(stderr, "%c %d %d\n", c, id, size);
            allocations[id] = malloc(size);
        }
        else if (c == 'R') {
            fprintf(stderr, "%c %d %d\n", c, id, size);
            allocations[id] = realloc(allocations[id], size);
        }
        else if (c == 'F') {
            fprintf(stderr, "%c %d\n", c, id);
            free(allocations[id]);
        }
    }

    close(file);

    return 0;
}