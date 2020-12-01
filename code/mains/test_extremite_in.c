#include <stdio.h>

#include "extreminte.h"


int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s [-v] /serveur IP / /serveur port/\n", argv[0]);
        return -1;
    }

    char script[] = "./configure-tun.sh";
    char *ptrS = script;
    char tunName[] = "tun0";
    char *ptrT = script;


    if (argc=3) {
        return ext_in(ptrT, ptrS, argv[2], argv[3], 0);
    } else {
        if(argv[1] != "-v") {
            fprintf(stderr, "Usage: %s [-v] /serveur IP / /serveur port/\n", argv[0]);
            return -1;
        }
        return ext_in(ptrT, ptrS, argv[3], argv[4], 1);
    }
}