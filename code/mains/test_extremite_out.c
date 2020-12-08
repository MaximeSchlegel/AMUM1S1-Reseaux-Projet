#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "iftun.h"
#include "extreminte.h"


int main(int argc, char **argv) {
    if (argc < 2 || 4 < argc) {
        fprintf(stderr, "Usage: %s [-v] [-tun] /port/\n", argv[0]);
        exit(1);
    }
    if (argc == 2) {
        return ext_out(argv[1], 1, 0);
    } else if (argc == 3 && argv[2][1] == 'v') {
        return ext_out(argv[2], 1, 1);
    } else {
        /* Create the tun */
        char* tunName = malloc(IFNAMSIZ);
        strcpy(tunName, "tun0");
        int tunfd = tun_alloc(tunName, IFF_TUN);
        if(tunfd < 0){
            fprintf(stderr, "Error during tun allocation");
            exit(2);
        }
        system("./configure-tun.sh tun0");
        if (argc == 4) {
            printf("Tun Interface ('tun0') configured successfully! (with: './cofigure-tun.sh')\n");
            return ext_out(argv[3], tunfd, 1);
        }
        return ext_out(argv[3], tunfd, 0);
    }
}