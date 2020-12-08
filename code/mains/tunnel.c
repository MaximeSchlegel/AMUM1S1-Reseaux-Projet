#include "iftun.h"
#include "extreminte.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

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
            return -2;
        }
    }
    tunName = argv[opt];

    if (output) {
        int outputFd = open(argv[3], O_WRONLY | O_CREAT);
        return iftun( tunName, configScriptPath, outputFd, 1);
    }
    return iftun( tunName, configScriptPath, 1, 1);
}