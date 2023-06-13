#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

unsigned long long factorial(unsigned long long n) {
    unsigned long long factorial = 1;
    for (unsigned long long i = 2; i <= n; ++i) {
        factorial *= i;
    }

    return factorial;
}

unsigned long long fibonacci(unsigned long long n) {
    unsigned long long f = 0;
    unsigned long long s = 1;
    if (n == 0) {
        return f;
    }

    if (n == 1) {
        return s;
    }

    unsigned long long result = f + s;
    for (unsigned long long i = 3; i <= n; ++i) {
        f = s;
        s = result;
        result = f + s;
    }

    return result;
}

int main(int argc, char** argv) {
    unsigned long long n;
    if (argc != 2) {
        printf("Ошибка! ввод - <число>.\n");
        exit(-1);
    }
    n = atoi(argv[1]);
    pid_t chpid = fork();
    if (chpid < 0) {
        printf("Ошибка! Невозможно создать дочерний процесс.");
	exit(-1);
    } else if (chpid == 0) {
        printf("Факторил из процесса-ребенка: %lld\n", factorial(n));
    } else {
	printf("Число Фибоначчи из процесса-родителя: %lld\n", fibonacci(n));
    }

    return 0;
}