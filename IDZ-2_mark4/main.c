#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#define SEM_NAME "sem_name_1"
#define SHM_NAME "shm_name_1"
#define SIZE 40  // Размер массива flowers (массив цветков)

int shm_fd;  // Идентификатор разделяемой памяти
sem_t* p_sem;  // Указатель на основной семафор
int* flowers; // Основной массив. flowers[i] = 1, если цветок полит, иначе 0

void sigfunc(int sig) {
    // Освобождение разделяемой памяти и семафора
    if (munmap(flowers, SIZE * sizeof(int)) == -1) {
        printf("Ошибка! Невозможно освободить разделяемую память.\n");
        exit(-1);
    }
    // Закрытие разделяемой памяти (закрытие потока)
    if (close(shm_fd) == -1) {
        printf("Ошибка! Невозможно закрыть поток к разделяемой памяти.\n");
        exit(-1);
    }
    // Удаляем разделяемую память
    if (shm_unlink(SHM_NAME) == -1) {
        printf("Ошибка! Невозможно удалить разделяемую память.\n");
        exit(-1);
    }
    // Закрытие семафора
    if (sem_close(p_sem) == -1) {
        printf("Ошибка! Невозможно закрыть семафор.\n");
        exit(-1);
    }
    // Удаление семафора
    if (sem_unlink(SEM_NAME) == -1) {
        printf("Ошибка! Невозможно удалить семафор.");
        exit(-1);
    }
    printf("\n\nСад закрывается.\n\n");
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Ошибка! необходимо ввести число процессов-садовников (рекомендуется 2)\n");
        exit(-1);
    }
    int count_of_gardeners = atoi(argv[1]);
    if (count_of_gardeners > 10 || count_of_gardeners < 1) {
        printf("Ошибка! Число садовников должно быть больше нуля и не должно превышать 10\n.");
        exit(-1);
    }
    pid_t pid[11]; // id дочерних процессов
    printf("Сад скоро откроется. Чтобы его покинуть безопасно, нажмите сочетание клавиш CTRL + C.\n");
    // Перехваиваю сигнал на функцию sigfunc
    sleep(2);

    // Создание разделяемой памяти для массива A
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        printf("Ошибка! Невозможно открыть разделяемую память\n");
        exit(-1);
    }
    // Задача размера разделяемой памяти (40*sizeof(int) - массив политости цветков)
    if (ftruncate(shm_fd, SIZE * sizeof(int)) == -1) {
        printf("Ошибка! Проблема с выделенной памятью в разделяемой памяти\n");
        exit(-1);
    }
    // Получить доступ к памяти
    flowers = mmap(NULL, SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (flowers == (int*) - 1) {
        printf("Ошибка подключения к адресному пространству памяти!\n");
        exit(-1);
    }
    
    // Инициализация значений массива цветков
    for (int i = 0; i < SIZE; ++i) {
        flowers[i] = 1;
    }

    // Создание именованного семафора
    p_sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (p_sem < 0) {
        printf("Ошибка! Невозможно открыть семафор.\n");
        exit(-1);
    }

    // Создание первого дочернего процесса
    if (pid[0] = fork() == 0) {
        // Как я понял (я не нашел информации в интернете и в лекциях, потыкал на практике),
        // signal вызывается для каждого из дочерних потоков, поэтому чистим мы только в одном
        // из них, т.к. адресное пространство у них общее (которое чистится) и иначе мы несколько
        // раз удаляем одно и то же
        signal(SIGINT, sigfunc);
        printf("Сад начал свою работу!\n");
        while (1) {
            sem_wait(p_sem);  // Блокировка семафора
            // Простой алгоритм поиска трех случайных единичек в массиве
            for (int i = 0; i < 3; i++) {
                int j;
                do {
                    j = rand() % SIZE;
                } while (flowers[j] == 0);
                flowers[j] = 0;
                printf("Цветок %d завял.\n", j);
            }
            sem_post(p_sem);  // Освобождение семафора
            sleep(5);
        }
        exit(0);
    }

    // Небольшая пауза между созданием сада и впусканием садовника
    sleep(3);
    // Создание процессов садовников с id от 0 до count_of_gardeners - 1
    for (int g_id = 0; g_id < count_of_gardeners; ++g_id) {
        if (pid[g_id] = fork() == 0) {
            while (1) {
                sem_wait(p_sem);  // Блокировка семафора
                // поиск нулевого цветка, алгоритм простой
                int k;
                for (k = 0; k < SIZE; ++k) {
                    if (flowers[k] == 0) {
                        flowers[k] = 1;
                        break;
                    }
                }
                if (k == SIZE) {
                    printf("Все цветы политы.\n");
                } else {
                   printf("Цветок %d полит садовником %d.\n", k, g_id); 
                }
                sem_post(p_sem);  // Освобождение семафора
                sleep(3);
            }
            exit(0);
        }
        sleep(1);
    }

    // Ожидание завершения дочерних процессов
    for (int i = 0; i < count_of_gardeners + 1; ++i) {
        waitpid(pid[i], NULL, 0);
    }

    return 0;
}