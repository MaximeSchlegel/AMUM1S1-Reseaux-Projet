#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "iftun.h"
#include "extreminte.h"

/* taille maximale des lignes */
#define MAXLIGNE 80
#define CIAO "Au revoir ...\n"

int transfertSocketToTun (int socket, int tun) {
    // Size of the buffer for the read, should at least be the MTU (1500) 
    int BUFFER_SIZE = 1500 ;

    // Check the file descriptors
    if (socket < 0) {
        perror("Invalid 'socket' File descriptor\n");
        return -1;
    }
    if (tun < 0) {
        perror("Invalid 'tun' File descriptor\n");
        return -2;
    }

    // create the buffer
    char *buffer = (char *) calloc(BUFFER_SIZE, sizeof(char));
    int sizeRecv, writeSuccess;
    
    // Read from the socket and write to the tun
    sizeRecv = recv(socket, buffer, BUFFER_SIZE, 0);
    if (write(tun, buffer, sizeRecv) < 0) {
        perror("Unable to write\n");
        return -3;
    }
    
    free(buffer);
    return 0;
}

int ext_out(char* port,
            int outputFD,
            int verbose) {
    
    /* Modified from:
     *   echo / serveur simpliste
     *   Master Informatique 2012 -- Université Aix-Marseille  
     *   Emmanuel Godard
    */
    
    int serverSocket, clientSocket;                     /* descripteurs de socket */
    int len, on;                                        /* utilitaires divers */
    struct addrinfo *resol;                             /* résolution */
    struct addrinfo indic = {AI_PASSIVE,                /* Toute interface */
                             PF_INET, SOCK_STREAM, 0,   /* IP mode connecté */
                             0, NULL, NULL, NULL};
    struct sockaddr_in client;                          /* adresse de socket du client */
    int err;                                            /* code d'erreur */

    err = getaddrinfo(NULL, port, &indic, &resol);
    if (err < 0) {
        fprintf(stderr, "Résolution: %s\n", gai_strerror(err));
        exit(2);
    }
    if (verbose) printf("Ecoute sur le port %s\n", port);

    /* Création de la socket, de type TCP / IP */
    if ((serverSocket = socket(resol->ai_family, resol->ai_socktype, resol->ai_protocol)) < 0) {
        perror("allocation de socket");
        exit(3);
    }
    if (verbose) printf("le n° de la socket est : %i\n", serverSocket);

    /* On rend le port réutilisable rapidement /!\ */
    on = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("option socket");
        exit(4);
    }
    if (verbose) printf("Option(s) OK!\n");

    /* Association de la socket s à l'adresse obtenue par résolution */
    if (bind(serverSocket, resol->ai_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("bind");
        exit(5);
    }
    freeaddrinfo(resol); /* /!\ Libération mémoire */
    if (verbose) printf("bind!\n");

    /* la socket est prête à recevoir */
    if (listen(serverSocket, SOMAXCONN) < 0) {
        perror("listen");
        exit(6);
    }
    if (verbose) printf("listen!\n");

    // crée l'interface tun0 sur le serveur
    char* tun_name = malloc(IFNAMSIZ);
    tun_name[0]='\0';
    int tunfd = tun_alloc(tun_name, IFF_TUN);
    if(tunfd < 0){
        perror("Error during interface allocation");
        exit(-1);
    };

    while (1)
    {
        /* attendre et gérer indéfiniment les connexions entrantes */
        len = sizeof(struct sockaddr_in);
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&client, (socklen_t *)&len)) < 0) {
            perror("accept");
            exit(7);
        }
        
        /* Nom réseau du client */
        char hotec[NI_MAXHOST];
        char portc[NI_MAXSERV];
        
        err = getnameinfo((struct sockaddr *)&client, len, hotec, NI_MAXHOST, portc, NI_MAXSERV, 0);
        if (err < 0) {
            fprintf(stderr, "résolution client (%i): %s\n", clientSocket, gai_strerror(err));
        } else {
            if (verbose) printf("accept! (%i) ip=%s port=%s\n", clientSocket, hotec, portc);
        }
        
        /* traitement */
        if (transfertSocketToTun(clientSocket, outputFD) < 0) {
            perror("error, client");
        }
        // if (transfertSocketToTun(clientSocket, tunfd) < 0) {
        //     perror("error, client");
        // }

    }

    close(serverSocket);
    return EXIT_SUCCESS;
}


int ext_in(char* tunName,
           char *configScript,
           char* serveurIP,
           char* serveurPort,
           int verbose) {
    
    /* Modified from:
     *   echo / client simple
     *   Master Informatique 2012 -- Université Aix-Marseille  
     *   Emmanuel Godard
    */
    
    char ip[NI_MAXHOST];    /* adresse IPv4 en notation pointée */
    struct addrinfo *resol; /* struct pour la résolution de nom */
    int socketClient;       /* descripteur de socket */

    /* Résolution de l'hôte */
    if ( getaddrinfo(serveurIP, serveurPort, NULL, &resol) < 0 ){
        perror("Can't resolved serveur adresse");
        exit(1);
    }

    /* On extrait l'addresse IP */
    sprintf(ip, "%s", inet_ntoa(((struct sockaddr_in*)resol->ai_addr)->sin_addr));

    /* Création de la socket, de type TCP / IP */
    /* On ne considère que la première adresse renvoyée par getaddrinfo */
    if ((socketClient=socket(resol->ai_family,resol->ai_socktype, resol->ai_protocol))<0) {
        perror("allocation de socket");
        exit(3);
    }
    if (verbose) printf("le n° de la socket est : %i\n",socketClient);

    /* Connexion */
    if (verbose) printf("Essai de connexion à %s (%s) sur le port %s\n\n", serveurIP, ip, serveurPort);
    if (connect(socketClient, resol->ai_addr,sizeof(struct sockaddr_in))<0) {
        perror("connexion");
        exit(4);
    }
    freeaddrinfo(resol); /* /!\ Libération mémoire */
    
    iftun(tunName, configScript, socketClient);

    // /* Session */
    // char tampon[MAXLIGNE + 3]; /* tampons pour les communications */
    // ssize_t lu;
    // int fini=0;
    // while( 1 ) { 
    //     /* Jusqu'à fermeture de la socket (ou de stdin)     */
    //     /* recopier à l'écran ce qui est lu dans la socket  */
    //     /* recopier dans la socket ce qui est lu dans stdin */

    //     /* réception des données */
    //     lu = recv(,tampon,MAXLIGNE,0); /* bloquant */
    //     if (lu == 0 ) {
    //         fprintf(stderr,"Connexion terminée par l'hôte distant\n");
    //         break; /* On sort de la boucle infinie */
    //     }
    //     tampon[lu] = '\0';
    //     printf("reçu: %s",tampon);
    //     if ( fini == 1 )
    //         break;  /* on sort de la boucle infinie*/
    
    //     /* recopier dans la socket ce qui est entré au clavier */    
    //     if ( fgets(tampon,MAXLIGNE - 2,stdin) == NULL ){/* entrée standard fermée */
    //         fini=1;
    //         fprintf(stderr,"Connexion terminée !!\n");
    //         fprintf(stderr,"Hôte distant informé...\n");
    //         shutdown(s, SHUT_WR); /* terminaison explicite de la socket 
	// 		                         dans le sens client -> serveur */
    //         /* On ne sort pas de la boucle tout de suite ... */
    //     }else{   /* envoi des données */
    //         send(s,tampon,strlen(tampon),0);
    //     }
    // } 
    /* Destruction de la socket */
    close(socketClient);

    if (verbose) printf("Fin de la session.\n");
    return EXIT_SUCCESS;
}

int  ext_bid(char* tunName,char *configScript,char* serveurIP,char* serveurPort, int outputFD, char* port,int verbose){
    int f = fork();
    
    if(f<0){
        return -1;
    }
    else if(f==0){
        ext_in(tunName, configScript, serveurIP, serveurPort, verbose);
    }
    else{
        ext_out(port, outputFD, verbose);
    }
    return 1;
}