#include <stdio.h>  
#include <stdlib.h> 

// Просто функция, вызываемая при ошибке
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
