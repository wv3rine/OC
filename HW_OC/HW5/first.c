#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char *first_name = "first_fifo";
    char *second_name = "second_fifo";
    if (mkfifo(first_name, 0666) == -1) {
	perror("Cant create first fifo\n");
	exit(-1);
    }
    if (mkfifo(second_name, 0666) == -1) {
	perror("Cant create second fifo\n");
	exit(-1);
    }

    char first_buffer[] = "Hello world from 1st process!\n";
    int size = strlen(first_buffer);

    int fd;
    // открытие первого именнованого канала 
    // и получение его файлового дескриптора
    if ((fd = open(first_name, O_WRONLY)) < 0) {
        perror("First process: can't open for writing");
        exit(-1);
    }
    printf("Первый канал успешно открыт!\n");

    // запись в канал и проверка на количество записанных байт
    ssize_t written_size = write(fd, first_buffer, size);
    if (size != written_size) {
        perror("First process: can't write all string");
        exit(-1);
    }
    printf("Запись в первый канал произведена.\n");

    // закрытие канала
    if (close(fd) < 0) {
        perror("First process: can't close after writing");
        exit(-1);
    }
    
    // открытие второго канала
    if ((fd = open(second_name, O_RDONLY)) < 0) {
        perror("First process: can't open for reading");
        exit(-1);
    }
    
    printf("Второй канал успешно открыт!\n");
    char second_buffer[128];
    // чтение из канала
    ssize_t readen_size = read(fd, second_buffer, 128);
    size = strlen(second_buffer);
    if (size != readen_size) {
        perror("First process: can't read all string");
        exit(-1);
    }
    printf("Чтение из второго канала успешно произведено\n");

    printf("Сообщение из второй программы: %s", second_buffer);
    if (close(fd) < 0) {
        perror("First process: can't close after reading");
        exit(-1);
    }
    unlink(first_name);
    unlink(second_name);
    return 0;
}