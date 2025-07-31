#include "calc.h"

int main(){

    int param1 = 0, param2 = 0, item = 0;

    while (item != 5)
    {
        int flag = 1;
        printf("1) Сложение\n"
            "2) Вычитание\n"
            "3) Умножение\n"
            "4) Деление\n"
            "5) Выход\n");

        scanf("%d", &item);
        if((item > 5 || item < 1) || getchar() != '\n'){
            item = 0;
            while(getchar() != '\n');
            continue;
        }

        if(item == 5) return 0;

        printf("Параметр 1: ");
        flag = scanf("%d", &param1);
        if(!flag || getchar() != '\n'){
            printf("Ошибка!\n");
            while(getchar() != '\n');
            continue;
        }

        printf("Параметр 2: ");
        flag = scanf("%d", &param2);
        if(!flag || getchar() != '\n'){
            printf("Ошибка!\n");
            while(getchar() != '\n');
            continue;
        }

        switch (item)
        {
            case 1:
                printf("Результат: %d\n", add(param1, param2));
                break;
            case 2:
                printf("Результат: %d\n", sub(param1, param2));
                break;
            case 3:
                printf("Результат: %d\n", mul(param1, param2));
                break;
            case 4:
                printf("Результат: %d\n", div(param1, param2));
                break;
        }
    }
    
    return 0;
}