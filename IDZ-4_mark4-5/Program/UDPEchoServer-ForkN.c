#include "UDPEchoServer.h"
#define _OPEN_SYS_ITOA_EXT
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

typedef char bool; 
#define true 1     
#define false 0  

void DieWithError(char *errorMessage);  /* Функция ошибки */

void* ProcessMain(void* servSock);
void HandleUDPClient();

#define RCVBUFSIZE 32   /* Размер принимаемого буфера */
#define SIZE 40     // Длина массива

int array[SIZE];    // Массив
pthread_mutex_t mutex;    // Мьютекс доступа к массиву
int count_of_ok_flowers = 40; // Число неувядших цветов
int servSock = 0; // Дескриптор сокета сервера
bool hasCleaner = false; // Наличие уборщика - клиента, который завершает работу сервера

void* server_func(void* thread_args) {
    while (!hasCleaner) {
        pthread_mutex_lock(&mutex); // Ожидание доступа к массиву

        // Ужасно костыльный код, но ниичего :) здесь просто поиск
        // трех попарно неравных индексов и значение массива в которых
        // равно единице
        if (count_of_ok_flowers == 0) {
            printf("Все цветы увяли. Грустно.\n");
            sleep(5);
            continue;
        }

        int idx1 = rand() % SIZE;
        while (array[idx1] == 0) {
            idx1 = rand() % SIZE;
        }
        array[idx1] = 0;
        count_of_ok_flowers -= 1;

        if (count_of_ok_flowers == 0) {
            printf("Цветок под номером %d увял. Все цветы увяли.", idx1);
            sleep(5);
            continue;
        }

        int idx2 = rand() % SIZE;
        while (array[idx2] == 0) {
            idx2 = rand() % SIZE;
        }

        while (idx1 == idx2) {
            idx2 = rand() % SIZE;
            while (array[idx2] == 0) {
                idx2 = rand() % SIZE;
            }
        }
        array[idx2] = 0;
        count_of_ok_flowers -= 1;

        if (count_of_ok_flowers == 0) {
            printf("Цветки под номерами %d и %d увяли. Увяли все цветы.", idx1, idx2);
            sleep(5);
            continue;
        }

        int idx3 = rand() % SIZE;
        while (array[idx3] == 0) {
            idx3 = rand() % SIZE;
        }

        while (idx3 == idx1 || idx3 == idx2) {
            idx3 = rand() % SIZE;
            while (array[idx3] == 0) {
                idx3 = rand() % SIZE;
            }
        }
        array[idx3] = 0;
        count_of_ok_flowers -= 1;

        printf("Цветки под номерами %d, %d и %d увяли. Количество неувядших цветов: %d \n", idx1, idx2, idx3, count_of_ok_flowers);

        pthread_mutex_unlock(&mutex); // Освобождение доступа к массиву

        sleep(5); // Пауза в 5 секунд
    }
}


int main(int argc, char *argv[])
{
    char *servIP;
    unsigned short servPort;

    if (argc != 3)
    {
        fprintf(stderr, "Неправильный формат ввода. Необходимо:  %s <IP сервера (сада)> <Порт сервера (сада)>\n", argv[0]);
        exit(1);
    }
    servIP = argv[1];
    servPort = atoi(argv[2]);

    // Создание сервера по айпи и порту, возвращает дескриптор сокета сервера
    servSock = CreateUDPServerSocket(servIP, servPort);

    // Инициализация генератора случайных чисел
    srand(time(NULL));

    // Заполнение массива единицами
    int i;
    for (i = 0; i < SIZE; ++i) {
        array[i] = 1;
    }

    pthread_t array_handler_thread;
    pthread_t main_thread;

    // Инициализация мьютекса
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&array_handler_thread, NULL, server_func, NULL);
    pthread_create(&main_thread, NULL, ProcessMain, NULL);
    pthread_join(array_handler_thread, NULL);
    pthread_join(main_thread, NULL);
    printf("Сад закрыт. Уборщик всех выгнал.\n");
    close(servSock);
    exit(0);
}

// Основной процесс для каждого потока.
// Принимает коннект клиента, печатает соответствующее
// сообщение, производит основные действия (Handle)
void* ProcessMain(void* thread_args)
{
    while (!hasCleaner)
    {
        HandleUDPClient();
    }
}

// Основные действия по работе с клиентом
// по его сокету
void HandleUDPClient()
{
    char buffer[RCVBUFSIZE];        /* Буфер (для имени садовника (процесса) и номера цветка) */
    int recvMsgSize;
    struct sockaddr_in clntAddr;
    unsigned int clntAddrLen;
	
    // Получаем сообщение от клиента (его имя). Здесь
    // некоторый костыль, что мы получаем ровно 20 сообщений.
    // Можно сделать бесконечное количество, это вообще изменяемая
    // часть кода, мне показался такой вариант безопаснее.
    for (int i = 0; i < 20; ++i) {
	clntAddrLen = sizeof(clntAddr);
        // Получаем имя клиента (садовника)
	if ((recvMsgSize = recvfrom(servSock, buffer, RCVBUFSIZE, 0,
            (struct sockaddr *) &clntAddr, &clntAddrLen)) < 0)
            DieWithError("recvfrom() failed");

	if (strcmp(buffer, "Уборщик") == 0) {
            printf("Уборщик начинает свою работу.\n");
	    hasCleaner = true;
	    close(servSock);
	    return;
	}	
	pthread_mutex_lock(&mutex);
        // Алгоритм поиcка неполитого цветка + соответствующий вывод
        int j;
        for (j = 0; j < SIZE; ++j) {
            if (array[j] == 0) {
                array[j] = 1;
                ++count_of_ok_flowers;
		// Это небольшая добавка к коду из ИДЗ-3, потому что
		// buffer у нас один, а меняется только его длина
                char gardener_name[recvMsgSize + 1];
                for (int k = 0; k < recvMsgSize; ++k) {
                    gardener_name[k] = buffer[k];
                }
		gardener_name[recvMsgSize] = '\0';
                printf("Цветок номер %d полит садовником %s. Количество политых цветков: %d\n", j, gardener_name, count_of_ok_flowers);
                break;
            }
        }

        char jString[3];
        snprintf(jString, 3, "%d", j);
	
	if (sendto(servSock, jString, 3, 0,
             (struct sockaddr *) &clntAddr, sizeof(clntAddr)) < 0)
            DieWithError("sendto() sent a different number of bytes than expected");
        pthread_mutex_unlock(&mutex);
    }
}

