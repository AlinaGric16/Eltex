#include "main.h"

void add_subscriber(struct node **ptr){
    struct node *new_node = calloc(1, sizeof(struct node));
    if(new_node == NULL){
        perror("Out of memmory!\n");
        exit(EXIT_FAILURE);
    }
    
    if(*ptr != NULL){
        struct node *last_node = *ptr;
        
        while(last_node->next != NULL){
            last_node = last_node->next;
        }

        new_node->prev = last_node;
        last_node->next = new_node;
    }
    else{
        *ptr = new_node;
    }
    
    printf("Имя(до %d символов): ", INPUT_LEN);
    overflow(new_node->data.name);
   
    printf("Фамилия(до %d символов): ", INPUT_LEN);
    overflow(new_node->data.second_name);

    printf("Номер(до %d цифр): ", INPUT_LEN);
    overflow(new_node->data.tel);

}

int delete_subscriber(struct node **ptr_ptr){
    struct node *ptr = *ptr_ptr;
    int number = 0;
    int iden;

    output_of_all_records(ptr);
    if(ptr == NULL) return 0;

    printf("Идентификатор абонента: ");
    scanf("%d", &number);

    for(iden = 1; ptr->next != NULL && iden != number; iden++){
        ptr = ptr->next;
    }

    if(iden != number){
        printf("Абонента не существует.\n");
        return 1;
    }

    if(ptr->prev != NULL) ptr->prev->next = ptr->next;
    if(ptr->next != NULL) ptr->next->prev = ptr->prev;

    if(number == 1){
        if(ptr->next != NULL) *ptr_ptr = ptr->next;
        else *ptr_ptr = NULL;
    }

    free(ptr);
    
    printf("Абонент удален.\n");

    return 0;
}

void name_search(struct node *ptr){
    char pattern[INPUT_LEN] = {0};
    int flag = 0;

    printf("Имя(до %d символов): ", INPUT_LEN);
    overflow(pattern);

    for(int i = 1; ptr != NULL && i > 0; i++){
        if(!strcmp(ptr->data.name, pattern)){
            printf("%d. %s %s %s\n", i, ptr->data.name, ptr->data.second_name, ptr->data.tel);
            flag = 1;
        }
        if(ptr->next != NULL) ptr = ptr->next;
        else i = -1;
    }

    if(!flag) printf("Совпадений не найдено.\n");
}

void output_of_all_records(struct node *ptr){
    if(ptr == NULL) printf("Справочник пуст.\n");
    else{
        for(int i = 1; i > 0; i++){
            printf("%d. %s %s %s\n", i, ptr->data.name, ptr->data.second_name, ptr->data.tel);
            if(ptr->next != NULL) ptr = ptr->next;
            else i = -1;
        }
    }
}

void completion(struct node *ptr){
    int flag = 1;
    struct node *next_ptr = ptr;
    if(ptr == NULL) flag = 0;
    while(flag){
        ptr = next_ptr;
        if(ptr->next != NULL) next_ptr = ptr->next;
        else flag = 0;
        free(ptr);
    }
}