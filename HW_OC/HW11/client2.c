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
    int sock;                        /* Дескриптор сокета */
    struct sockaddr_in servAddr;
    unsigned short servPort;
    char *servIP;
    char *name;                      /* Имя клиента (садовника) */
    char buffer[RCVBUFSIZE];         /* Буффер для отправляемой информации (имени) */

    if ((argc < 2) || (argc > 4))
    {
        fprintf(stderr, "Формат ввода: %s <IP сервера> <Порт сервера>\n",
                argv[0]);
        exit(1);
    }

    servIP = argv[1];
    servPort = atoi(argv[2]);

    /* Создание сокета */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        perror("socket() failed");

    /* Инициализация структуры адреса сервера */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;             /* Инициализация протоколоа уровня Internet */
    servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Инициализация IP */
    servAddr.sin_port        = htons(servPort); /* Инициализация порта */

    /* Подключение к серверу */
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        perror("connect() failed");

     while (1) {
        int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            perror("Recv failed");
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0';

        printf("Полученное сообщение: %s", buffer);
        if (strcmp(buffer, "The End\n") == 0) {
            break;
        }
    }

    close(sock);
    printf("Клиент 2 завершил свою работу.");
    return 0;
}
