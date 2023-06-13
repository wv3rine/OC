#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int source_fd, target_fd;
    ssize_t bytes_read;
    char buffer[BUFFER_SIZE];

    // Проверяем аргументы командной строки
    if (argc != 3) {
        printf("Ошибка! правильный ввод: %s <имя входного файла> <имя выходного файла>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Открываем исходный файл для чтения
    source_fd = open(argv[1], O_RDONLY);
    if (source_fd == -1) {
        perror("source open");
        exit(-1);
    }

    // Открываем целевой файл для записи
    target_fd = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (target_fd == -1) {
        perror("target open");
        exit(-1);
    }

    // Читаем исходный файл и записываем его в целевой
    while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0) {
        if (write(target_fd, buffer, bytes_read) != bytes_read) {
            perror("write");
            exit(-1);
        }
    }
    if (bytes_read == -1) {
        perror("read");
        exit(-1);
    }

    // Закрываем файлы
    if (close(source_fd) == -1) {
        perror("close source");
        exit(-1);
    }
    if (close(target_fd) == -1) {
        perror("close target");
        exit(-1);
    }

    printf("Работа завершена.\n");
    return 0;
}