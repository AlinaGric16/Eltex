#include "main.h"

void overflow(char *ptr){
    char buffer[100];
    scanf("%99s", buffer);

    while(strlen(buffer) >= INPUT_LEN){
        printf("Лимит превышен.\n");
        while (getchar() != '\n');
        printf("Введите корректно: ");
        scanf("%99s", buffer);
    }

    strncpy(ptr, buffer, INPUT_LEN-1);
}