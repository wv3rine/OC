#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/mman.h>
#include<signal.h>
#include<stdlib.h>
#include<error.h>
#include<sys/types.h>
#include<fcntl.h>


int shm_id, close_shm_id;
int *share, *close_share;
const char* shar_obj = "main_share_object";
const char* close_shar_obj = "close_share_object";

// сервер
// Отвечает за удаление share

void sigfunc(int sig) {
    *close_share = -1;
    sleep(2);
    if (shm_unlink(shar_obj) == -1) {
        perror("main shm_unlink()");
	    exit(-1);
    }
    if (close(shm_id) == -1) {
	    perror("main close()");
	    exit(-1);
    }
    printf("Основная разделяемая память успешно удалена. Процесс завершает свою работу.\n");

    exit(0);
}

int main() {
    signal(SIGINT, sigfunc);

    // открытие разделяемых памятей
    shm_id = shm_open(shar_obj, O_CREAT | O_RDWR, 0666);
    if (shm_id == -1) {
        perror("main shm_open()");
        exit(1);
    }
    close_shm_id = shm_open(close_shar_obj, O_CREAT | O_RDWR, 0666);
    if (close_shm_id == -1) {
        perror("close shm_open()");
        exit(1);
    }

    // задание размера паямти
    if (ftruncate(shm_id, sizeof(int)) == -1) {
        perror("main ftruncate()");
        exit(1);
    }
    if (ftruncate(close_shm_id, sizeof(int)) == -1) {
        perror("close ftruncate()");
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

    // действия
    *close_share = 0;
    while (*close_share != -1) {
        sleep(1);
        printf("%d\n", *share);
    }

    if (shm_unlink(shar_obj) == -1) {
        perror("main shm_unlink()");
	exit(-1);
    }
    if (close(shm_id) == -1) {
	perror("main close()");
	exit(-1);
    }
    printf("Основная разделяемая память успешно удалена. Процесс завершает свою работу.\n");
    return 0;
}
