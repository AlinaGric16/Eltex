#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    void *handle;
    int (*func_ptr)(int, int);
    char*error;
    char *menu[4] = {"add", "sub", "mul", "div"};
    int param1 = 0, param2 = 0, item = 0;

    handle = dlopen("libcalc.so", RTLD_LAZY);

    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

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

        if(item == 5){
            dlclose(handle);
            return 0;
        } 

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

        func_ptr = dlsym(handle, menu[item-1]);

        error = dlerror();
        if (error != NULL) {
            fprintf(stderr, "%s\n", error);
            dlclose(handle);
            exit(EXIT_FAILURE);
        }

        printf("Результат: %d\n", (*func_ptr)(param1, param2));
    }

    dlclose(handle);

    return 0;
}