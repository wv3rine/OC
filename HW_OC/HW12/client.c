#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAXRECVSTRING 255  /* Longest string to receive */

int main(int argc, char *argv[]) {
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast Address */\
    unsigned short broadcastPort;     /* Port */
    char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */

    if (argc != 2) {
        fprintf(stderr,"Формат ввода: %s <порт сервера>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    broadcastPort = atoi(argv[1]);   /* First arg: broadcast port */

    /* Create a best-effort datagram socket using UDP */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));        /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                      /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);       /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);           /* Broadcast port */

    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    while (1) {
        /* Receive a single datagram from the server */
        if ((recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, NULL, 0)) < 0) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        recvString[recvStringLen] = '\0';
        printf("Сообщение от сервера: %s\n", recvString);
    }

    close(sock);
    exit(0);
}