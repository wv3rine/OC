#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RCVBUFSIZE 32   /* Размер буфера получения */

void DieWithError(char *errorMessage);  /* Функция ошибки */

int main(int argc, char *argv[])
{
    int sock;                        /* Дескриптор сокета */
    struct sockaddr_in servAddr;
    struct sockaddr_in fromAddr;
    unsigned short servPort;
    char *servIP;
    char *name;                      /* Имя клиента (садовника) */
    char buffer[RCVBUFSIZE];         /* Буффер для отправляемой информации (имени) */
    unsigned int nameLength;
    unsigned int fromSize;

    if ((argc < 3) || (argc > 5))
    {
        fprintf(stderr, "Формат ввода: %s <IP сервера (сада)> <Имя садовника (клиента)> <Порт сервера (сада)>\n",
                argv[0]);
        exit(1);
    }

    servIP = argv[1];
    name = argv[2];
    servPort = atoi(argv[3]);

    /* Создание сокета */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Инициализация структуры адреса сервера */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;             /* Инициализация протоколоа уровня Internet */
    servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Инициализация IP */
    servAddr.sin_port        = htons(servPort); /* Инициализация порта */

    nameLength = strlen(name);

    if (strcmp(name, "Уборщик") == 0) {
        if (sendto(sock, name, nameLength, 0, (struct sockaddr *)
		&servAddr, sizeof(servAddr)) != nameLength)
            DieWithError("send() sent a different number of bytes than expected");
        printf("Уборщик попал в сад. Начинается чистка\n");
	close(sock);
	exit(0);
    }


    for(int i = 0; i < 20; ++i) {
        /* Посылаем свое имя на сервер, предполагая, что садовник готов работать */
        if (sendto(sock, name, nameLength, 0, (struct sockaddr *)
		&servAddr, sizeof(servAddr)) != nameLength)
            DieWithError("send() sent a different number of bytes than expected");

        printf("%d) %s полил цветок номер ", i, name);
	
	
	fromSize = sizeof(fromAddr);
        if ((recvfrom(sock, buffer, 3, 0,
            (struct sockaddr *) &fromAddr, &fromSize)) <= 0)
            DieWithError("recvfrom() failed");

        if (servAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
        {
            fprintf(stderr,"Error: received a packet from unknown source.\n");
            exit(1);
        }
        printf("%s\n", buffer);
        sleep(2);
    }

    close(sock);
    exit(0);
}
