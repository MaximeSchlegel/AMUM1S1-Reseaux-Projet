#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "iftun.h"



int tun_alloc(char *dev, int flags) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("Can't open the tun interface");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers) 
     *        IFF_TAP   - TAP device  
     *
     *        IFF_NO_PI - Do not provide packet information  
     */
    ifr.ifr_flags = flags;

    if (*dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name); // copy the name of the interface in the dev variable (overwrite previous value)
    
    return fd;
}

int continuousTransfert(int fromFD, int toFD) {
    // Size of the buffer for the read, should at least be the MTU (1500) 
    int BUFFER_SIZE = 3000 ;

    // Check the file descriptor
    if (fromFD < 0) {
        perror("Invalid 'from' File descriptor\n");
        return -1;
    }
    if (toFD < 0) {
        perror("Invalid 'to' File descriptor\n");
        return -2;
    }

    char *buffer = (char *) calloc(BUFFER_SIZE, sizeof(char));
    int sizeRead, writeSuccess;
    
    // Read from in and output on out
    while(1) {
        sizeRead = read(fromFD, buffer, BUFFER_SIZE);
        buffer[sizeRead] = '\0';
        writeSuccess = write(toFD, buffer, sizeRead);
        if (writeSuccess < 0) {
            perror("Unable to write\n");
            return -3;
        }
    }

    free(buffer);
    return 0;
}

int iftun(char *dev, char *configScript, int toFD) {
    
    int tunfd;

    tunfd = tun_alloc(dev, IFF_TUN);

    if(tunfd < 0){
        perror("Error during interface allocation");
        exit(-1);
    }

    if (configScript) {
        int cmd_size = 1000; // TODO: compute the real size of the command
        char *cmd = malloc(cmd_size);
        snprintf(cmd, cmd_size, "%s %s", configScript, dev);
        system(cmd);
    } else {
        printf("Faire la configuration de %s...\n", dev);
        printf("Appuyez sur une touche pour continuer\n");
        getchar();
    }

    continuousTransfert(tunfd, toFD);

    return 0;
}
