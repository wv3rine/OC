#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<sys/shm.h>
#include<sys/mman.h>
#include<stdlib.h>
#include<time.h>
#include<error.h>
#include<sys/types.h>
#include<fcntl.h>

// Клиент
// отвечает за удаление close_share

int shm_id, close_shm_id, num;
int *share, *close_share;
const char* shar_obj = "main_share_object";
const char* close_shar_obj = "close_share_object";

void sigfunc(int sig) {
    *close_share = -1;
    sleep(2);
    
    if (shm_unlink(close_shar_obj) == -1) {
        perror("close shm_unlink()");
	exit(-1);
    }
    if (close(close_shm_id) == -1) {
	perror("close close()");
	exit(-1);
    }
    printf("Основная разделяемая память успешно удалена. Процесс завершает свою работу.\n");

    exit(0);
}

int main() {
    // Обработка сигнала SIGINT, которая чистит разделяемую память
    signal(SIGINT, sigfunc);    

    srand(time(NULL));

    // открытие разделяемых памятей
    shm_id = shm_open(shar_obj, O_CREAT | O_RDWR, 0666);
    if (shm_id == -1){
        perror("main shm_open()");
        exit(1);
    }
    close_shm_id = shm_open(close_shar_obj, O_CREAT | O_RDWR, 0666);
    if (close_shm_id == -1) {
        perror("close shm_open()");
        exit(1);
    }

    // получение доступов
    share =  mmap(0, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0);
    if (share == (int*) - 1){
        perror("main mmap()");
        exit(2);
    }    
    close_share =  mmap(0, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, close_shm_id, 0);
    if (close_share == (int*) - 1){
        perror("close mmap()");
        exit(2);
    }

    // Действия
    *close_share = 0;
    while (1) {
        printf("\n");
        num = random() % 1000;
        if (*close_share == -1) {
            break;
        }
        *share = num;
        printf("Сгенерированное число: %d\n", num);
        sleep(1);
    }
    
    if (shm_unlink(close_shar_obj) == -1) {
        perror("close shm_unlink()");
	exit(-1);
    }
    if (close(close_shm_id) == -1) {
	perror("close close()");
	exit(-1);
    }

    printf("Вспомогательная разделяемая память успешно удалена. Процесс завершает свою работу.\n");
    return 0;
}
