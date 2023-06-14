#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>
#include<signal.h>
#include<stdlib.h>
#include<error.h>

int shm_id, close_shm_id;
int *share, *close_share;

// сервер
// Отвечает за удаление share

void sigfunc(int sig) {
    *close_share = -1;
    sleep(2);
    if (share != NULL) {
        shmdt(share);
    }
    if (shmctl(shm_id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
        perror("shared memory remove error");
        exit(-1);
    }
    printf("Основная разделяемая память успешно удалена. Процесс завершает свою работу.\n");

    exit(0);
}

int main() {
    signal(SIGINT, sigfunc);

    shm_id = shmget(0x2FF, getpagesize(), 0666 | IPC_CREAT);
    if (shm_id == -1){
        perror("main shmget()");
        exit(1);
    }
    
    close_shm_id = shmget(0x3FF, getpagesize(), 0666 | IPC_CREAT);
    if (close_shm_id == -1){
        perror("close shmget()");
        exit(1);
    }

    share = (int *)shmat(shm_id, 0, 0);
    if (share == NULL){
        perror("main shmat()");
        exit(2);
    }
    
    close_share = (int *)shmat(close_shm_id, 0, 0);
    if (close_share == NULL){
        perror("close shmat()");
        exit(2);
    }
    *close_share = 0;
    while (*close_share != -1) {
        sleep(1);
        printf("%d\n", *share);
    }

    shmdt(share);
    if (shmctl(shm_id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
        perror("shared memory remove error");
        exit(-1);
    }
    printf("Основная разделяемая память успешно удалена. Процесс завершает свою работу.\n");
    return 0;
}
