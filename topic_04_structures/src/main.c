#include "main.h"

int main(){

    struct abonent guide[MAX] = {0};
    int item = 0;

    while(item != 5){
        printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\n4) Вывод всех записей\n5) Выход\n");
        scanf("%d", &item);
        while (getchar() != '\n');
        struct abonent *ptr = guide;
        switch (item)
        {
            case 1:
                add_subscriber(ptr);
                break;
            case 2:
                delete_subscriber(ptr);
                break;
            case 3:
                name_search(ptr);
                break;
            case 4:
                output_of_all_records(ptr);
                break;
        }
    }

    return 0;
}

int add_subscriber(struct abonent *ptr){
    int count = 0;

    while(ptr->name[0] != '\0'){
        ptr++;
        count++;
        if(count >= MAX){
            printf("Справочник переполнен.\n");
            return 1;
        }
    }

    printf("Имя(до %d символов): ", INPUT_LEN);
    overflow(ptr->name);
   
    printf("Фамилия(до %d символов): ", INPUT_LEN);
    overflow(ptr->second_name);

    printf("Номер(до %d цифр): ", INPUT_LEN);
    overflow(ptr->tel);

    while (strspn(ptr->tel, "0123456789") != strlen(ptr->tel)) {
        printf("Некорректный номер.\nНомер: ");
        overflow(ptr->tel);
    }

    return 0;
}

void overflow(char *ptr){
    char buffer[100];
    scanf("%99s", buffer);

    while(strlen(buffer) >= INPUT_LEN){
        printf("Лимит превышен.\n");
        while (getchar() != '\n');
        printf("Введите корректно: ");
        scanf("%99s", buffer);
    }

    strncpy(ptr, buffer, INPUT_LEN);
}

int delete_subscriber(struct abonent *ptr){
    int number = 0;

    output_of_all_records(ptr);
    printf("Идентификатор абонента: ");
    scanf("%d", &number);

    if(number >= MAX || ptr[number].name[0] == '\0'){
        printf("Абонента не существует.\n");
        return 1;
    }

    ptr[number] = (struct abonent){0};
    printf("Абонент удален.\n");

    return 0;
}

void name_search(struct abonent *ptr){
    char pattern[INPUT_LEN] = {0};
    int flag = 0;

    printf("Имя(до %d символов): ", INPUT_LEN);
    overflow(pattern);

    for(int i = 0; i < MAX; i++){
        if(!strcmp(ptr[i].name, pattern)){
            printf("%d. %s %s %s\n", i, ptr[i].name, ptr[i].second_name, ptr[i].tel);
            flag = 1;
        }
    }

    if(!flag) printf("Совпадений не найдено.\n");
}

void output_of_all_records(struct abonent *ptr){
    for(int i = 0; i < MAX; i++){
        if(ptr[i].name[0] != '\0'){
            printf("%d. %s %s %s\n", i, ptr[i].name, ptr[i].second_name, ptr[i].tel);
        }
    }
}
