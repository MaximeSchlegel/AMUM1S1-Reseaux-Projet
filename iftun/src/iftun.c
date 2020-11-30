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



int tun_alloc(char *dev, int flags) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
    {
        perror("alloc tun");
        exit(-1);
    }

    memset(&ifr, 0, sizeof(ifr));

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers) 
     *        IFF_TAP   - TAP device  
     *
     *        IFF_NO_PI - Do not provide packet information  
     */
    
    ifr.ifr_flags = flags;

    if (*dev)
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0)
    {
        close(fd);
        return err;
    }
    strcpy(dev, ifr.ifr_name);
    return fd;
}

int continuousTransfert(int fromFileDesc, int toFileDesc) {
    
    int BUFFER_SIZE = 10000 ;

    if (fromFileDesc < 0) {
        perror("Invalid 'from' File descriptor\n");
        return -1;
    }
    if (toFileDesc < 0) {
        perror("Invalid 'to' File descriptor\n");
        return -1;
    }

    char *buffer = (char *) calloc(BUFFER_SIZE, sizeof(char));
    int sz, w;
    
    while(1) {
        sz = read(fromFileDesc, buffer, BUFFER_SIZE);
        buffer[sz] = '\0';
        w = write(toFileDesc, buffer, sz);
        if (w < 0) {
            exit(1);
        }
    }

    return 0;
}

int iftun(char *dev,
          char *configScript,
          int toFileDesc) {
    
    int tunfd;

    tunfd = tun_alloc(dev, IFF_TUN);

    if(tunfd < 0){
        perror("Allocating interface");
        exit(1);
    }

    if (configScript) {
        int cmd_size = 100; // TODO: compute the real size of the command
        char *cmd = malloc(cmd_size);
        snprintf(cmd, cmd_size, "%s %s", configScript, dev);
        // printf("%d\n", cmd_size);
        // printf(cmd);
        system(cmd);
    } else {
        printf("Faire la configuration de %s...\n", dev);
        printf("Appuyez sur une touche pour continuer\n");
        getchar();
    }

    // printf("Interface %s Configurée:\n", dev);
    // system("ip addr");
    // printf("Appuyez sur une touche pour lacer la lecture\n");
    // getchar();
    continuousTransfert(tunfd, toFileDesc);

    return 0;
}

int main(int argc, char **argv) {

    char* configScriptPath = NULL;
    char* tunName = NULL;
    char* output = NULL;

    int opt = 1;
    for (; opt < argc - 2; opt++) {
        if (argv[opt][0] == '-') {
            switch (argv[opt][1]) {
            case 'c':
                configScriptPath = argv[opt + 1];
                opt++;
                break;
            case 'o':
                output = argv[opt + 1];
                opt++;
                break;
            default:
                fprintf(stderr, "Usage: %s [-c /configuration script path/] [-o /output file path/] /tun name/\n", argv[0]);
                return -1;
            }
        } else {
            fprintf(stderr, "Usage: %s [-c /configuration script path/] [-o /output file path/] /tun name/\n", argv[0]);
            return -1;
        }
    }
    tunName = argv[opt];

    if (output) {
        int outputFd = open(argv[3], O_WRONLY | O_CREAT);
        return iftun( tunName, configScriptPath, outputFd);
    }
    return iftun( tunName, configScriptPath, 1);
}
