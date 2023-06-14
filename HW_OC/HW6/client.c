#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<time.h>
#include<error.h>

// Клиент
// отвечает за удаление close_share

int shm_id, close_shm_id, num;
int *share, *close_share;

void sigfunc(int sig) {
    *close_share = -1;
    sleep(2);
    
    if (close_share != NULL) {
        shmdt(close_share);
    }
    if (shmctl(close_shm_id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
        perror("shared memory remove error");
        exit(-1);
    }
    printf("Вспомогательная разделяемая память успешно удалена. Процесс завершает свою работу.\n");
    exit(0);
}

int main() {
    // Обработка сигнала SIGINT, которая чистит разделяемую память
    signal(SIGINT, sigfunc);    
    srand(time(NULL));
    shm_id = shmget(0x2FF, getpagesize(), 0666 | IPC_CREAT);
    printf("shm_id = %d\n", shm_id);
    if (shm_id < 0){
        perror("shmget()");
        exit(1);
   }
    
    close_shm_id = shmget(0x3FF, getpagesize(), 0666 | IPC_CREAT);
    if (close_shm_id == -1){
        perror("close shmget()");
        exit(1);
    }

    /* подключение сегмента к адресному пространству процесса */
    share = (int *)shmat(shm_id, 0, 0);
    if (share == NULL){
        perror("shmat()");
        exit(2);
    }
    printf("share = %p\n", share);
    
    close_share = (int *)shmat(close_shm_id, 0, 0);
    if (close_share == NULL){
        perror("close shmat()");
        exit(2);
    }
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
    
    shmdt(close_share);
    if (shmctl(close_shm_id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
        perror("shared memory remove error");
        exit(-1);
    }
    printf("Вспомогательная разделяемая память успешно удалена. Процесс завершает свою работу.\n");
    return 0;
}
