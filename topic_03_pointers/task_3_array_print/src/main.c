#include <stdio.h>

int main(){

    int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int *ptr = array;
    
    for(int i = 0; i < 10; i++){
        printf("%d ", *ptr);
        ptr++;
    }
    printf("\n");
    
    return 0;
}