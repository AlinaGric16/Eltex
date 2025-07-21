#include <stdio.h>

#define MAX 50

char* search_for_substring(char *ptr_str, char *ptr_substr);

int main(){

    char str[MAX], substr[MAX];
    char *ptr_str = str, *ptr_substr = substr;
    
    printf("Введите строку (до 50 символов) ");
    fgets(ptr_str, MAX, stdin);
    printf("Введите подстроку (до 50 символов) ");
    fgets(ptr_substr, MAX, stdin);

    char *ptr_res = search_for_substring(ptr_str, ptr_substr);
    if(ptr_res != NULL) printf("%s", ptr_res);
    else printf("NULL\n");

    return 0;
}

char* search_for_substring(char *ptr_str, char *ptr_substr){
    char *ptr_res = NULL;
    int flag = 0;
    while(*ptr_str != '\n' && flag == 0){
        if(*ptr_str == *ptr_substr){
            flag = 1;
            char *ps = ptr_str, *pss = ptr_substr;
            while(flag && *pss != '\n'){
                if(*ps != *pss){
                    flag = 0;
                }
                ps++;
                pss++;
            }
        }
        ptr_str++;
    }

    if(flag){
        ptr_res = ptr_str - 1;
    }

    return ptr_res;
}