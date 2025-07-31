#include "calc.h"

int div(int a, int b){
    if(b == 0){
        printf("Деление на 0!\n");
        return 0;
    } 
    return a / b;
}