#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "iftun.h"
#include "extreminte.h"

typedef struct Config {
    char* tunName;    // Name of the tun interface
    char* localIP;    // Adress of the tun interface
    char* localPort;  // Local port for sending
    char* options;    // LAN accessible by the tunnel 
    char* remoteIP;   // IP of the other end of the tunnel
    char* remotePort; // Port of the other end of the tunnel
} Config;

Config* parseConfigFile(char* fileName) {
    Config* config = (Config*) malloc(sizeof(Config));

    /* Open the config file */
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Can't open the config file");
        exit(1);
    }

    char* line = NULL;
    size_t lineSize = 0;
    ssize_t read;

    int keySize=0, valueSize=0, skipped = 0;

    while ((read = getline(&line, &lineSize, file)) != -1) {
        char* key = malloc(read * sizeof(char));
        char* value = malloc(read * sizeof(char));
        char* target = key;

        // Ignore comments
        if (line[0] == '#') {
            free(key);
            free(value);
            continue;
        }

        // parse the key value on the line
        for (int i = 0; i < read && line[i]!='\n' && line[i]!='\r'; i++) {
            if (line[i] == ' ') {
                // ignore spaces
                skipped++;
                continue;
            }
            
            if (line[i] == '=' && keySize==0) {
                // the first '=' mark the end of the key
                target = value;
                keySize = i;
                skipped++;
            } else {
                target[i - keySize - skipped] = line[i];
            }
        }
        valueSize = read - keySize - skipped;

        if (valueSize < 0) {
            perror("Invalid config file: Line without separator");
            exit(3);
        }

        key[keySize] = '\0'; value[valueSize] = '\0';

        char* keyM = (char*) malloc(keySize * sizeof(char));
        char* valueM = (char*) malloc(valueSize * sizeof(char));
        strcpy(keyM, key);
        strcpy(valueM, value);

        if (!strcmp(keyM, "tun") && !config->tunName) {
            config->tunName = valueM;
        } else if (!strcmp(keyM, "inip") && !config->localIP){
            config->localIP = valueM;
        } else if (!strcmp(keyM, "inip") && !config->localIP){
            config->localIP = valueM;
        } else if (!strcmp(keyM, "inport") && !config->localPort){
            config->localPort = valueM;
        } else if (!strcmp(keyM, "options") && !config->options){
            config->options = valueM;
        } else if (!strcmp(keyM, "outip") && !config->remoteIP){
            config->remoteIP = valueM;
        } else if (!strcmp(keyM, "outport") && !config->remotePort){
            config->remotePort = valueM;
        } else {
            perror("Invalid config file: Invalid Key");
            exit(4);
        }

        free(key);
        free(value);
        keySize = 0; valueSize = 0; skipped = 0;
    }

    fclose(file);
    if (line) free(line);
    return config;
}

int main(int argc, char **argv) {
    if (argc < 2 || 5 < argc) {
        fprintf(stderr, "Usage: %s [-v] [-rad /vmNb/] /config file/\n", argv[0]);
        exit(1);
    }

    int verbose = 0;
    int radvd = 0;
    char* vmNb = NULL;
    char* configFile = NULL;

    for (int i=1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strlen(argv[i]) == 2 && argv[i][1] == 'v') {
                verbose = 1;
            } else {
                if (strlen(argv[i]) == 4 && argv[i][1] == 'r' && argc > i + 1) {
                    radvd = 1;
                    vmNb = argv[i+1];
                    i++;
                } else {
                    fprintf(stderr, "Usage: %s [-v] [-rad /vmNb/] /config file/\n", argv[0]);
                    exit(2);
                }
            }
        } else {
            if (!configFile) {
                configFile = argv[i];
            } else {
                fprintf(stderr, "Usage: %s [-v] [-rad /vmNb/] /config file/\n", argv[0]);
                exit(3);
            }
        }
    }

    if (!configFile) {
        fprintf(stderr, "Usage: %s [-v] [-rad /vm nb/] /config file/\n", argv[0]);
        exit(4);
    }
    Config* config = parseConfigFile(configFile);
    
    if (verbose) {
        printf("Option:\n");
        printf("\t- config file  : %s \n", configFile);
        printf("\t- radvd advert : %i (%s) \n", radvd, vmNb);
        printf("\t- verbose      : %i \n", verbose);
        printf("Configuration:\n");
        printf("\t- Tun Name     : %s \n", config->tunName);
        printf("\t- Local IP     : %s \n", config->localIP);
        printf("\t- Local Port   : %s \n", config->localPort);
        printf("\t- Remote IP    : %s \n", config->remoteIP);
        printf("\t- Remote PORT  : %s \n", config->remotePort);
        printf("\t- Options      : %s \n", config->options);
    }

    char* tunName = malloc(IFNAMSIZ);
    strcpy(tunName, config->tunName);
    int tunfd = tun_alloc(tunName, IFF_TUN);
    if(tunfd < 0){
        fprintf(stderr, "Error during tun allocation");
        exit(2);
    }

    int cmd_size = 1000; // should be plenty space to create the command
    char *cmd = malloc(cmd_size * sizeof(char));
    snprintf(cmd, cmd_size, "sh ./configure-tun.sh %s %s %s",
             tunName, config->localIP, config->options);    
    // system("chmod +x configure-tun.sh");
    system(cmd);
    free(cmd);
    if (verbose) printf("Tun Interface ('%s') configured successfully!\n", tunName);

    if (radvd) {
        int cmd_size = 1000; // should be plenty space to create the command
        char *cmd = malloc(cmd_size * sizeof(char));
        snprintf(cmd, cmd_size, "sh ./configure-radvd.sh ./radvd-vm%s.conf", vmNb);
        // system("chmod +x configure-tun.sh");
        system(cmd);
        free(cmd);
        if (verbose) printf("Radvd configured successfully! (with ./radvd-vm%s.conf)\n", vmNb);
    }
    
    ext_bid(config->localPort, tunfd,
            tunfd, config->remoteIP, config->remotePort,
            verbose);
}