#include "main.h"

int main(){
    char matrix[N][N] = {0};
    char array[N] = {0};

    printf("Part 1.\nMatrix:\n");
    create_matrix(matrix);
    print_matrix(matrix);

    printf("Part2.\nArray:\n");
    create_array(array);
    print_array(array);
    printf("Reverse array:\n");
    reverse_array(array);
    print_array(array);

    printf("Part 3.\nMatrix:\n");
    fill_matrix(matrix);
    print_matrix(matrix);

    printf("Part 4.\nMatrix:\n");
    snail_matrix(matrix);
    print_matrix(matrix);

    return 0;
}

// Part 1
void create_matrix(char mat[N][N]){
    int counter = 1;
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            mat[i][j] = counter;
            counter++;
        }
    }
}

void print_matrix(char mat[N][N]){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

// Part 2
void create_array(char arr[N]){
    for (int i = 0; i < N; i++){
        arr[i] = i+1;
    }
}

void print_array(char arr[N]){
    for (int i = 0; i < N; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void reverse_array(char arr[N]){
    int tmp = 0;
    for (int i = 0; i < N / 2; i++)
    {
        tmp = arr[i];
        arr[i] = arr[N - 1 - i];
        arr[N - 1 - i] = tmp;
    }
}

// Part 3
void fill_matrix(char mat[N][N]){
    int counter = 0;
    for (int i = 0; i < N; i++){
        if (i != N-1) counter = 0;
        for (int j = 0; j < N; j++){
            if(j == N - 1 - i) counter = 1;
            mat[i][j] = counter;
        }
    }
}

// Part 4
void snail_matrix(char mat[N][N]){
    int i = 0, j = 0, counter = 1;
    for(int k = 0; counter < N*N; k++){
        i = k;
        for(j = k; j < N - 1 - k; j++){
            mat[i][j] = counter;
            counter++;
        }
        j = N - 1 - k;
        for(int i = k; i < N - 1 - k; i++){
            mat[i][j] = counter;
            counter++;
        }
        i = N - 1 - k;
        for(j = N - 1 - k; j > k; j--){
            mat[i][j] = counter;
            counter++;
        }
        j = k;
        for(i = N - 1 - k; i > k; i--){
            mat[i][j] = counter;
            counter++;
        }
    }
    if(N % 2 != 0) mat[N/2][N/2] = counter;
}