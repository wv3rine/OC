#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
    int fd[2][2]; // 2 канала для связи между процессами. 0-й - для общения 1 и 2 процесса, 1-й - для 2 и 3
    pid_t pid[3]; // id дочерних процессов
    int in_file_id, out_file_id;

    if (argc != 3) {
        printf("Ошибка! Принимается следующий ввод: %s <имя входного файла> <имя выходного файла>\n", argv[0]);
        return 1;
    }
    

    // создание каналов между процессами
    if (pipe(fd[0]) < 0 || pipe(fd[1]) < 0) {
        printf("Ошибка! Невозможно создать каналы.\n");
        exit(-1);
    }

    // создание первого процесса
    if ((pid[0] = fork()) == -1) {
        perror("Ошибка! Невозможно создать процесс.\n");
        return 1;
    } else if (pid[0] == 0) {
        // первый процесс - читает текстовые данные из файла и передает через канал первой пайпы второму процессу
        char buffer[BUFFER_SIZE];
        int size;

	// открытие файлов для чтения
    	in_file_id = open(argv[1], O_RDONLY);
    	if (in_file_id < 0 ) {
	    close(in_file_id);
            printf("Ошибка! Невозможно открыть файл\n");
            exit(-1);
    	}
        
        int read_bytes = read(in_file_id, buffer, BUFFER_SIZE);
        if (read_bytes < 0) {
	    close(in_file_id);
            printf("Невозможно прочитать файл.\n");
            exit(-1);
        }
        close(in_file_id);
        
        close(fd[0][0]); // закрываем входной поток канала
        if (size = write(fd[0][1], buffer, BUFFER_SIZE) != BUFFER_SIZE){
            printf("Невозможно записать строку в пайп.\n"); 
            exit(-1); 
        } 
        close(fd[0][1]); // закрываем ненужный выходной поток канала
        return 0;
    }

    // создание второго процесса
    if ((pid[1] = fork()) == -1) {
        perror("Ошибка! Невозможно создать процесс.\n");
        return 1;
    } else if (pid[1] == 0) {
        // второй процесс - принимает данные через первую пайпу, считает количество чисел и передает через вторую пайпу третьему процессу
        char buffer[BUFFER_SIZE];
        int size;

        close(fd[0][1]); // закрываем ненужный выходной поток канала
        // Пробуем прочитать из pip'а BUFFER_SIZE байт в массив, т.е. всю
        // записанную строку 
        if (size = read(fd[0][0], buffer, BUFFER_SIZE) < 0){
            printf("Невозможно прочитать строку.\n"); 
            exit(-1); 
        } 
        // Простой алгоритм из задачи. Читаем очередной символ,
        // если он цифра, то к ответу добавляются все числа, содержащие
        // данную цифру как последнюю. Счетчик count - число ненулевых цифр
	// до данной. Соответственно к ответу добавляется count, тк мы добавляем
	// все встреченные числа, начинающиеся на ненулевую цифру и заканчивающиеся данной
        int answer = 0;
        for (int i = 0; i < strlen(buffer); ++i) {
            int count = 0;
            while (buffer[i] <= '9' && buffer[i] >= '0') {
		if (buffer[i] != '0') {
		    ++count;
		}
                answer += count;
                ++i;
            }
        }
        close(fd[0][0]); // завершаем работу с первым каналом, закрываем
        
        
        close(fd[1][0]); // закрываем входной поток второй пайпы
        write(fd[1][1], &answer, sizeof(int));
        close(fd[1][1]); // завершаем работу со вторым каналом, закрываем
        return 0;
    }

    // создание третьего процесса
    if ((pid[2] = fork()) == -1) {
        printf("Ошибка! Невозможно создать процесс.\n");
        exit(-1);
    } else if (pid[2] == 0) {
        // третий процесс - принимает данные через второй пайп и записывает итог в файл
        close(fd[1][1]); // закрываем ненужный выходной поток второй пайп
        int answer;

	 // открытие файла для записи
    	out_file_id = open(argv[2], O_WRONLY);
    	if (out_file_id < 0) {
            printf("Ошибка! Невозможно открыть файл\n");
            exit(-1);
    	}
        
        int read_bytes = read(fd[1][0], &answer, sizeof(int));
	// Здесь просто один из способов преобразовать число
	// в строку, который мне подошел
	int length = snprintf(NULL, 0, "%d", answer);
	char buffer[length + 1];
	snprintf(buffer, length + 1, "%d", answer);
        if (write(out_file_id, buffer, length) < 0) {
	    close(out_file_id);
            printf("Ошибка! Невозможно записать данные в файл\n");
            exit(-1);
        }
        
        close(out_file_id);
        close(fd[1][0]); // закрываем входной поток второй пайпы
        return 0;
    }

    // родительский процесс закрывает все ненужные файловые дескрипторы и ожидает завершения дочерних процессов
	
    close(fd[0][0]);
    close(fd[0][1]);

    close(fd[1][0]);
    close(fd[1][1]);

    waitpid(pid[0], NULL, 0);
    waitpid(pid[1], NULL, 0);
    waitpid(pid[2], NULL, 0);

    return 0;
}