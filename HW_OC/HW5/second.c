#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int fd;
    char *first_name = "first_fifo";
    char *second_name = "second_fifo";
    // открытие первого канала на чтение
    if ((fd = open(first_name, O_RDONLY)) < 0) {
        perror("Second process: can't open for reading");
        exit(-1);
    }
    printf("Первый канал успешно открыт!\n");
    
    char first_buffer[128];
    ssize_t readen_size = read(fd, first_buffer, 128);
    int size = strlen(first_buffer);
    // чтение из канала
    if (size != readen_size) {
        perror("First process: can't read all string");
        exit(-1);
    }
    printf("Чтение из первого канала успешно произведено\n");

    // закрытие первого канала
    if (close(fd) < 0) {
        perror("Second process: can't close after reading");
        exit(-1);
    }

    printf("Сообщение из первой программы: %s", first_buffer);

    char second_buffer[] = "Hello world from 2nd process!\n";
    size = strlen(second_buffer);
    // открытие второго канала на запись
    if ((fd = open(second_name, O_WRONLY)) < 0) {
        perror("Second process: can't open for writing");
        exit(-1);
    }
    printf("Второй канал успешно открыт!\n");

    // запись во второй канал
    ssize_t written_size = write(fd, second_buffer, size);
    if (written_size != size) {
        perror("Second process: can't write all string");
        exit(-1);
    }
    printf("Запись во второй канал произведена.\n");
    
    // закрытие второго канала
    if (close(fd) < 0) {
        perror("Second process: can't close for writing");
	exit(-1);
    }
    return 0;
}