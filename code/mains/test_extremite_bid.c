#include <stdio.h>
#include <stdlib.h>

#include "extreminte.h"


int main(int argc, char **argv) {
    if (argc < 4 || 5 < argc) {
        fprintf(stderr, "Usage: %s [-v] /localPort/ /serveurIP/ /serveurPort/ \n", argv[0]);
        exit(1);
    }

    char script[] = "./configure-tun.sh";
    char *ptrS = script;
    char tunName[] = "tun0";
    char *ptrT = tunName;

    if (argc == 3) {
        //             tunName, configScript, serveurIP, serveurPort, outputFD, localport, verbose
        return ext_bid(tunName, script,       argv[2],   argv[3],     1,        argv[1],   0);
    } else {
        //             tunName, configScript, serveurIP, serveurPort, outputFD, localport, verbose
        return ext_bid(tunName, script,       argv[3],   argv[4],     1,        argv[2],   1);
    }
}