#include "main.h"

int main(){

    struct node *ptr = NULL;
    int item = 0;

    while(item != 5){
        printf("1) Добавить абонента\n"
            "2) Удалить абонента\n"
            "3) Поиск абонентов по имени\n"
            "4) Вывод всех записей\n"
            "5) Выход\n");
            
        if(scanf("%d", &item) != 1) item = 0;
        while (getchar() != '\n');

        switch (item)
        {
            case 1:
                add_subscriber(&ptr);
                break;
            case 2:
                delete_subscriber(&ptr);
                break;
            case 3:
                name_search(ptr);
                break;
            case 4:
                output_of_all_records(ptr);
                break;
        }
    }

    completion(ptr);
    return 0;
}