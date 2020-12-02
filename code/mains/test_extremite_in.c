#include <stdio.h>
#include <stdlib.h>

#include "extreminte.h"


int main(int argc, char **argv) {

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s [-v] /serveur IP / /serveur port/\n", argv[0]);
        exit(1);
    }

    char script[] = "./configure-tun.sh";
    char *ptrS = script;
    char tunName[] = "tun0";
    char *ptrT = tunName;

    if (argc == 3) {
        return ext_in(ptrT, ptrS, argv[1], argv[2], 0);
    } else {
        return ext_in(ptrT, ptrS, argv[2], argv[3], 1);
    }
}