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
    char* options;    // None atm
    char* remoteIP;   // IP of the other end of the tunnel
    char* remotePort; // Port of the other end of the tunnel
} Config;

Config* parseConfigFile(char* fileName) {
    Config* config = (Config*) malloc(sizeof(Config));

    /* Open the config file */
    FILE* file = fopen(fileName, 'r');
    if (file == NULL) {
        perror("Can't open the config file");
        exit(1);
    }

    char* line = NULL;
    size_t lineSize = 0;
    ssize_t read;

    char key[1000], value[1000];
    int keySize, valueSize;

    while ((read = getline(&line, &lineSize, file)) != -1) {
        // Ignore comments
        if (line[0] == '#') continue;
        
        // parse the key value on the line
        char* target = (char*) *key;
        for (int i = 0; i < lineSize; i++) {
            if (line[i] == '=' && !keySize) {
                // the first '=' mark the end of the key
                target = (char*) *value;
                keySize = i;
            } else {
                target[i - keySize - 1] = line[i];
            }
        }
        valueSize = lineSize - keySize - 1;
        if (valueSize < 0) {
            perror("Invalid config file: Line without separator");
            exit(3);
        }

        key[keySize] = '\0'; value[valueSize] = '\0';

        char* keyM = malloc(keySize * sizeof(char));
        char* valueM = malloc(valueSize * sizeof(char));
        strcpy(key, keyM);
        strcpy(value, valueM);

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

        keySize = 0; valueSize = 0;
    }

    fclose(file);
    if (line) free(line);
    return config;
}

int main(int argc, char **argv) {
    if (argc < 2 || 3 < argc) {
        fprintf(stderr, "Usage: %s [-v] /config file/\n", argv[0]);
        exit(1);
    }

    printf("Hello");

    char* configFile = argv[1];
    if (argc == 3) configFile = argv[2];
    Config* config = parseConfigFile(configFile);

    if (argc == 3) {
        printf("Configuration:\n");
        printf("\t- Tun Name    : %s\n", config->tunName);
        printf("\t- Local IP    : %s\n", config->localIP);
        printf("\t- Local Port  : %s\n", config->localPort);
        printf("\t- Remote IP   : %s\n", config->remoteIP);
        printf("\t- Remote PORT : %s\n", config->remotePort);
        printf("\t- Options     : %s\n", config->options);
    }

    char* tunName = malloc(IFNAMSIZ);
    strcpy(tunName, config->tunName);
    int tunfd = tun_alloc(tunName, IFF_TUN);
    if(tunfd < 0){
        fprintf(stderr, "Error during tun allocation");
        exit(2);
    }
    char cmd[1000];
    sprintf(cmd, "./configure-tun.sh %s %s", tunName, config->localIP);
    system(cmd);
    if (argc == 3) printf("Tun Interface ('%s') configured successfully!\n", tunName);

    ext_bid(config->localPort, tunfd,
            tunfd, config->remoteIP, config->remotePort,
            argc==3);



}