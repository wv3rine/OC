#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RCVBUFSIZE 50   /* Размер буфера получения */

void DieWithError(char *errorMessage);  /* Функция ошибки */

int main(int argc, char *argv[])
{
    char buffer[RCVBUFSIZE];
    char *servIP;
    unsigned short port;
    int servSock, clntSock1, clntSock2;
    struct sockaddr_in servAddr, clntAddr1, clntAddr2;
    unsigned int clntLen1 = sizeof(clntAddr1);
    unsigned int clntLen2 = sizeof(clntAddr2);

    if (argc != 3) {
        fprintf(stderr, "Неправильный формат ввода. Необходимо:  %s <IP сервера (сада)> <Порт сервера (сада)> <Максимальное количество потоков (садоников)>\n", argv[0]);
        exit(1);
    }
    servIP = argv[1];
    port = atoi(argv[2]);

    /* Создание сокета */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        perror("socket() failed");

    /* Инициализация адреса */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;                /* Инициализация уровня Internet */
    servAddr.sin_addr.s_addr = inet_addr(servIP); /* Инициализция IP */
    servAddr.sin_port = htons(port);              /* Инициализация порта */

    /* Привязка сеокета к адресу */
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        perror("bind() failed");

    /* Постановка сервера в состояние "listen", т.е. он может коннектиться с клиентами */
    if (listen(servSock, 2) < 0)
        perror("listen() failed");

    if ((clntSock1 = accept(servSock, (struct sockaddr *) &clntAddr1, &clntLen1)) < 0)
        perror("accept() failed");
    printf("Клиент 1 успешно подключен\n");

    if ((clntSock2 = accept(servSock, (struct sockaddr *) &clntAddr2, &clntLen2)) < 0)
        perror("accept() failed");
    printf("Клиент 2 успешно подключен\n");

    while (1) {
        int bytes_received = recv(clntSock1, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            perror("Recv failed");
            exit(EXIT_FAILURE);
        }
        buffer[bytes_received] = '\0';
        send(clntSock2, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "The End\n") == 0) {
            break;
        }
    }

    close(clntSock1);
    close(clntSock2);
    close(servSock);
    printf("Server closed\n");
}
