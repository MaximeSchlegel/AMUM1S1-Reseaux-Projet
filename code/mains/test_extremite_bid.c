#include <stdio.h>
#include <stdlib.h>

#include "extreminte.h"


int main(int argc, char **argv) {
    if (argc < 2 || 3 < argc) {
        fprintf(stderr, "Usage: %s [-v] /port/ /serveur IP/\n", argv[0]);
        exit(1);
    }

    char script[] = "./configure-tun.sh";
    char *ptrS = script;
    char tunName[] = "tun0";
    char *ptrT = tunName;

    if (argc == 3) {
        //             char* tunName, char *configScript, char* serveurIP,char* serveurPort,int outputFD,char* port,int verbose
        return ext_bid(tunName, script, argv[2], 1, 0);
    } else {
        return ext_bid(argv[2], 1, 1);
    }
}