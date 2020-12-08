#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include "iftun.h"
#include "extreminte.h"

/* taille maximale des lignes */
#define MAXLIGNE 80
#define CIAO "Au revoir ...\n"

int continuousRecvWrite (int fromFD, int toFD, int verbose) {
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

    // create the buffer
    char *buffer = (char *) calloc(BUFFER_SIZE, sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr,
                "Can't allocate the buffer for continuousRecvWrite\n");
        free(buffer);
        return -1;
    }
    
    // Read from the socket and write to the tun
    if (verbose) printf("Start recovering and writing\n");
    int sizeRecv, writeSuccess;
    while(1) {
        sizeRecv = recv(fromFD, buffer, BUFFER_SIZE, 0);
        if (sizeRecv < 0) {
            fprintf(stderr,
                    "Error while reading from the 'from' File descriptor\n");
            free(buffer);
            return sizeRecv;
        }
        writeSuccess = write(toFD, buffer, sizeRecv);
        if (writeSuccess < 0) {
            fprintf(stderr,
                    "Error while reading from the 'to' File descriptor\n");
            free(buffer);
            return writeSuccess;
        }
        if (writeSuccess =! sizeRecv) {
            fprintf(stderr,
                    "Error can't write the whole message to the 'to' File descriptor (%d/%d)\n",
                    writeSuccess, sizeRecv);
            free(buffer);
            return -1;
        }
    }
    
    free(buffer);
    return EXIT_SUCCESS;
}

int ext_out(char* localPort, int outputFD, int verbose) {
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

    if ((err = getaddrinfo(NULL, localPort, &indic, &resol))< 0) {
        fprintf(stderr,
                "Resolution Error: %s\n", gai_strerror(err));
        return err;
    }
    if (verbose) printf("Listen on Port: %s\n", localPort);

    /* Création de la socket, de type TCP / IP */
    if ((serverSocket = socket(resol->ai_family, resol->ai_socktype, resol->ai_protocol)) < 0) {
        fprintf(stderr,
                "Socket Allocation Error\n");
        return serverSocket;
    }
    if (verbose) printf("Socket allocated successfully! (%i)\n", serverSocket);

    /* On rend le port réutilisable rapidement /!\ */
    on = 1;
    if ((err = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
        fprintf(stderr,
                "Socket Option Error\n");
        return err;
    }
    if (verbose) printf("Option(s) set successfully!\n");

    /* Association de la socket s à l'adresse obtenue par résolution */
    if ((err = bind(serverSocket, resol->ai_addr, sizeof(struct sockaddr_in))) < 0) {
        fprintf(stderr,
                "Socket Binding Error\n");
        return err;
    }
    freeaddrinfo(resol); /* /!\ Libération mémoire */
    if (verbose) printf("Socket bound successfully!\n");

    /* la socket est prête à recevoir */
    if ((err = listen(serverSocket, SOMAXCONN)) < 0) {
        fprintf(stderr,
                "Error while listening\n");
        return err;
    }
    if (verbose) printf("Listen successfully!\n");

    /* Start accepting conection */
    while (1) {
        /* attendre et gérer indéfiniment les connexions entrantes */
        if (verbose) printf("Waiting for incoming connections...\n");

        len = sizeof(struct sockaddr_in);
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&client, (socklen_t *)&len)) <0) {
            fprintf(stderr,
                    "Accepting Connection Error\n");
            return clientSocket;
        } else if (verbose) printf(" New connection accepted\n");
        
        /* Nom réseau du client */
        char hotec[NI_MAXHOST];
        char portc[NI_MAXSERV];
        
        err = getnameinfo((struct sockaddr *)&client, len, hotec, NI_MAXHOST, portc, NI_MAXSERV, 0);
        if (err < 0) {
            if (verbose) fprintf(stderr,
                                 "Client Resolution Error (socket: %i): %s\n",
                                 clientSocket, gai_strerror(err));
        } else if (verbose)
            printf("Client accepted successfully!\n\t(socket:%i) ip=%s port=%s\n",
                   clientSocket, hotec, portc);
        
        /* traitement */
        continuousRecvWrite(clientSocket, outputFD, verbose);
        if (verbose) printf("Client exited!\n\t(socket:%i) ip=%s port=%s\n",
                            clientSocket, hotec, portc);
        close(clientSocket);
    }

    close(serverSocket);
    return EXIT_SUCCESS;
}


int ext_in(int inputFD, char* remoteIP, char* remotePort, int verbose) {
    /* Modified from:
     *   echo / client simple
     *   Master Informatique 2012 -- Université Aix-Marseille  
     *   Emmanuel Godard
    */
    
    char ip[NI_MAXHOST];    /* adresse IPv4 en notation pointée */
    struct addrinfo *resol; /* struct pour la résolution de nom */
    int clientSocket;       /* descripteur de socket */
    int err;

    /* Résolution de l'hôte */
    if((err = getaddrinfo(remoteIP, remotePort, NULL, &resol)) <0) {
        fprintf(stderr,
                "Serveur Resolution Error: %s\n", gai_strerror(err));
        return err;
    }
    if (verbose) printf("Remote serveur resolced successfully\n");


    /* On extrait l'addresse IP */
    sprintf(ip, "%s", inet_ntoa(((struct sockaddr_in*)resol->ai_addr)->sin_addr));

    /* Création de la socket, de type TCP / IP */
    /* On ne considère que la première adresse renvoyée par getaddrinfo */
    if ((clientSocket=socket(resol->ai_family,resol->ai_socktype, resol->ai_protocol)) <0) {
        fprintf(stderr,
                "Socket Allocation Error\n");
        return clientSocket;
    }
    if (verbose) printf("Socket allocated successfully! (%i)\n", clientSocket);

    /* Connexion */
    if (verbose) printf("Try to connect to %s (%s) on port %s\n", remoteIP, ip, remotePort);
    if ((err = connect(clientSocket, resol->ai_addr,sizeof(struct sockaddr_in))) <0) {
        fprintf(stderr,
                "Connexion Error\n");
        return err;
    }
    freeaddrinfo(resol); /* /!\ Libération mémoire */
    if (verbose) printf("Connect to %s (%s) on port %s successfully\n", remoteIP, ip, remotePort);
    
    continuousReadWrite(inputFD, clientSocket, verbose);

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
    close(clientSocket);

    if (verbose) printf("Fin de la session.\n");
    return EXIT_SUCCESS;
}

int  ext_bid(char* localPort, int outputFD,
             int inputFD, char* remoteIP, char* remotePort,
             int verbose) {
    
    int pid = fork();
    if(pid < 0) {
        fprintf(stderr,
                "Error while forking process\n");
        return pid;
    }
    
    if(pid == 0){
        /* main process, launch the "serveur" */
        if (verbose) printf("Fork Successful! Launching ext_out\n");
        ext_out(localPort, outputFD, verbose);
    }
    else{
        /* Secondary process, launch the "client" */
        if (verbose) printf("Fork Successful! Launching ext_out\n");
        while( ext_in(inputFD, remoteIP, remotePort, verbose) != EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;
}