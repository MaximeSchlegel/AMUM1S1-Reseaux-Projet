#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include "iftun.h"


int tun_alloc(char *dev, int flags) {
    /* From: https://backreference.org/2010/03/26/tuntap-interface-tutorial/
     * Arguments taken by the function:
     *   - char *dev: the name of an interface (or '\0'). MUST have enough space to hold the
     *                interface name if '\0' is passed
     *   - int flags: interface flags (eg, IFF_TUN etc.)
     */
    
    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    /* open the clone device */
    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        fprintf(stderr, "Can't open the '/dev/net/tun'\n");
        return fd;
    }

    /* preparation of the struct ifr, of type "struct ifreq" */
    memset(&ifr, 0, sizeof(ifr));

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers) 
     *        IFF_TAP   - TAP device 
     *        IFF_NO_PI - Do not provide packet information  */
    ifr.ifr_flags = flags;


    /* if a device name was specified, put it in the structure; otherwise, the kernel will try to
     * allocate the "next" device of the specified type  */
    if (*dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    /* try to create the device */
    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
        fprintf(stderr, "Can't can't create %s tun\n", dev);
        close(fd);
        return err;
    }

    /* if the operation was successful, write back the name of the interface to the variable "dev",
     * so the caller can know it. 
     * Note that the caller MUST reserve space in *dev (see calling code below)  */
    strcpy(dev, ifr.ifr_name);
    
    return fd;
}

int continuousReadWrite(int fromFD, int toFD, int verbose) {
    // Size of the buffer for the read, should at least be the MTU (1500) 
    int BUFFER_SIZE = 1500 ;

    // Check the file descriptor
    if (fromFD < 0) {
        fprintf(stderr,
                "Invalid 'from' File descriptor\n");
        return fromFD;
    }
    if (toFD < 0) {
        fprintf(stderr,
                "Invalid 'to' File descriptor\n");
        return toFD;
    }

    char *buffer = (char *) calloc(BUFFER_SIZE, sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr,
                "Can't allocate the buffer for continuousReadWrite\n");
        free(buffer);
        return -1;
    }
    
    // Read from in and output on out
    if (verbose) printf("Start reading and writing\n");
    int sizeRead, writeSuccess;
    while(1) {
        sizeRead = read(fromFD, buffer, BUFFER_SIZE);
        if (sizeRead < 0) {
            fprintf(stderr,
                    "Error while reading from the 'from' File descriptor\n");
            free(buffer);
            return sizeRead;
        }

        writeSuccess = write(toFD, buffer, sizeRead);
        if (writeSuccess < 0) {
            fprintf(stderr,
                    "Error while reading from the 'to' File descriptor\n");
            free(buffer);
            return writeSuccess;
        }
        if (writeSuccess =! sizeRead) {
            fprintf(stderr,
                    "Error can't write the whole message to the 'to' File descriptor (%i/%i)\n",
                    writeSuccess, sizeRead);
            free(buffer);
            return -1;
        }
    }
    free(buffer);
    return EXIT_SUCCESS;
}

int iftun(char* dev, char* configScript, int toFD, int verbose) {
    /* create the tun interface */
    int tunfd;
    tunfd = tun_alloc(dev, IFF_TUN + IFF_NO_PI);
    if(tunfd < 0) {
        perror("Error during tun allocation");
        return -1;
    }
    if (verbose) printf("Tun Interface ('%s') created successfully!\n", dev);

    /* configure it either with a script or manually */
    if (configScript) {
        int cmd_size = 1000; // should be plenty space to create the command
        char *cmd = malloc(cmd_size * sizeof(char));
        snprintf(cmd, cmd_size, "%s %s", configScript, dev);
        system(cmd);
        free(cmd);
        if (verbose) printf("Tun Interface ('%s') configured successfully! (with: '%s')\n",
                            dev, configScript);
    } else {
        printf("Faire la configuration de %s...\n", dev);
        printf("Appuyez sur une touche pour continuer\n");
        getchar();
    }

    /* start the reading and writing */
    return continuousReadWrite(tunfd, toFD, verbose);
}
