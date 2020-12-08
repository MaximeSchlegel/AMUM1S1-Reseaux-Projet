#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "iftun.h"
#include "extreminte.h"


int main(int argc, char **argv) {

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s [-v] /serveur IP / /serveur port/\n", argv[0]);
        exit(1);
    }

    /* Create the tun */
    char* tunName = malloc(IFNAMSIZ);
    strcpy(tunName, "tun0");
    int tunfd = tun_alloc(tunName, IFF_TUN);
    if(tunfd < 0){
        fprintf(stderr, "Error during tun allocation");
        exit(2);
    }
    system("./configure-tun.sh tun0");
    if (argc == 4) printf("Tun Interface ('tun0') configured successfully! (with: './cofigure-tun.sh')\n");

    /* Launch the input */
    if (argc == 3) {
        return ext_in(tunfd, argv[1], argv[2], 0);
    } else {
        return ext_in(tunfd, argv[2], argv[3], 1);
    }
}