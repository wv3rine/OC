#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXPENDING 5    /* Количество запросов связи, принимаемых сервером одновременно. Оптимально 5 */

void DieWithError(char *errorMessage);  /* Функция для ошибки */

int CreateUDPServerSocket(char* servIP, unsigned short port)
{
    int sock;                        /* Дескриптор для создающегося сокета сервера */
    struct sockaddr_in servAddr;     /* Структура адреса сокета */

    /* Создание сокета */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Инициализация адреса */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;                /* Инициализация уровня Internet */
    servAddr.sin_addr.s_addr = inet_addr(servIP); /* Инициализция IP */
    servAddr.sin_port = htons(port);              /* Инициализация порта */

    /* Привязка сеокета к адресу */
    if (bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("bind() failed");

    return sock;
}
