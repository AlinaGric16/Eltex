#include <stdio.h>

int main(){

    int num = 0;
    unsigned char byte = 0;

    printf("Введите число (int): ");
    scanf("%d", &num);
    printf("Введите новый третий байт (0-255): ");
    scanf("%hhd", &byte);

    char *ptr = (char*)&num;
    ptr += 2;
    *ptr = byte;

    printf("Результат: %d", num);

    return 0;
}