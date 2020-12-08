 #include <stdio.h>

#include "iftun.h"

int main(int argc, char **argv) {
    if (argc < 2 || 3 < argc) {
        fprintf(stderr, "Usage: %s [-v] /tun Name/ \n", argv[0]);
        return -1;
    }

    char* script = "./configure-tun-ipset.sh";
    
    if (argc == 2) {
        return iftun(argv[1], script, 1, 0);
    } else {
        return iftun(argv[2], script, 1, 1);
    }
}