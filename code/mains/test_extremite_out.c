#include <stdio.h>
#include <stdlib.h>

#include "extreminte.h"


int main(int argc, char **argv) {
    if (argc < 2 || 3 < argc) {
        fprintf(stderr, "Usage: %s [-v] /port/\n", argv[0]);
        exit(1);
    }
    if (argc == 2) {
        return ext_out(argv[1], 1, 0);
    } else {
        return ext_out(argv[2], 1, 1);
    }
}