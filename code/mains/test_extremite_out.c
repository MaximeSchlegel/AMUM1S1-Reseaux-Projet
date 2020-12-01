#include <stdio.h>

#include "extreminte.h"


int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s [-v] /port/\n", argv[0]);
        return -1;
    }
    if (argc=2) {
        return ext_out(argv[1], 1, 0);
    } else {
        if(argv[1] != "-v") {
            fprintf(stderr, "Usage: %s [-v] /port/\n", argv[0]);
            return -1;
        }
        return ext_out(argv[2], 1, 1);
    }
}