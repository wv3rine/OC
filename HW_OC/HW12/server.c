#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXRECVSTRING 255  /* Longest string to receive */

int main(int argc, char *argv[]) {
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    char *broadcastIP;                /* IP broadcast address */
    unsigned short broadcastPort;     /* Server port */
    char sendString[MAXRECVSTRING + 1];             /* String to broadcast */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLen;       /* Length of string to broadcast */

    if (argc < 3)                     /* Test for correct number of parameters */
    {
        fprintf(stderr,"Формат ввода: %s <IP-адрес> <порт>\n", argv[0]);
        exit(1);
    }

    broadcastIP = argv[1];            /* First arg:  broadcast IP address */ 
    broadcastPort = atoi(argv[2]);    /* Second arg:  broadcast port */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, (void *) &broadcastPermission,
          sizeof(broadcastPermission)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    /* Construct local address structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);/* Broadcast IP address */
    broadcastAddr.sin_port = htons(broadcastPort);         /* Broadcast port */

    while (1) {
        printf("Введите строку: ");
        fgets(sendString, MAXRECVSTRING + 1, stdin);
        sendString[strcspn(sendString, "\n")] = '\0'; // Remove newline character
        sendStringLen = strlen(sendString);

        if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)
                &broadcastAddr, sizeof(broadcastAddr)) != sendStringLen) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        printf("\nОтправлено сообщение: %s\n", sendString);
    }
}